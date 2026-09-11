#include "GooseUI/platform/wl_window.h"

#include "GooseUI/types.h"
#include "GooseUI/graphics/titleBar.h"

#include <algorithm>
#include <cstring>


namespace GooseUI::platform // Local
{
    bool hasServerSideDecorations(zxdg_decoration_manager_v1* decorationManager, zxdg_toplevel_decoration_v1* xdg_toplevel_decorations, uint32_t &decorationMode)
    {
        if(decorationManager == nullptr || xdg_toplevel_decorations == nullptr){ return false; }
        return decorationMode == ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE;
    }
}

namespace GooseUI::platform // Private
{
    // Static
    wl_display* wl_window::_display = nullptr;
    wl_registry* wl_window::_registry = nullptr; 
    wl_compositor* wl_window::_compositor = nullptr;
    wl_seat* wl_window::_seat = nullptr;

    uint32_t wl_window::_lastPointerSerial = 0;
    
    xdg_wm_base* wl_window::_xdg_wm_base = nullptr;
    zxdg_decoration_manager_v1* wl_window::_decoration_manager = nullptr;

    void wl_window::_registry_handle(void* data, wl_registry* reg, uint32_t id, const char* interface, uint32_t version)
    {
        if(std::strcmp(interface, "wl_compositor") == 0)
        {
            _compositor = (wl_compositor*)wl_registry_bind(reg, id, &wl_compositor_interface, std::min(version, 4u));
        }else if(std::strcmp(interface, "xdg_wm_base") == 0)
        {
            _xdg_wm_base = (xdg_wm_base*)wl_registry_bind(reg, id, &xdg_wm_base_interface, std::min(version, 4u));

            static const xdg_wm_base_listener wm_listener = {[](void*, xdg_wm_base* wm, uint32_t s){ xdg_wm_base_pong(wm, s); }};
            xdg_wm_base_add_listener(wl_window::_xdg_wm_base, &wm_listener, nullptr);
        }else if(std::strcmp(interface, zxdg_decoration_manager_v1_interface.name) == 0)
        {
            _decoration_manager = (zxdg_decoration_manager_v1*)wl_registry_bind(reg, id, &zxdg_decoration_manager_v1_interface, 1);
        }else if(strcmp(interface, wl_seat_interface.name) == 0)
        {
            _seat = (wl_seat*)wl_registry_bind(reg, id, &wl_seat_interface, std::min(version, 4u));
        }
    }
    
    void wl_window::_registry_remover(void* data, wl_registry* reg, uint32_t id){}

    const wl_registry_listener wl_window::_registry_listener = { _registry_handle, _registry_remover };
    const xdg_surface_listener wl_window::_xdg_surface_listener = {
        .configure = [](void* data, xdg_surface* xdg_surface, uint32_t serial) { xdg_surface_ack_configure(xdg_surface, serial); }
    };
    const xdg_toplevel_listener wl_window::_xdg_toplevel_listener = {
        .configure = [](void* data, xdg_toplevel* toplevel, int32_t width, int32_t height, wl_array* states) {
            wl_window* window = static_cast<wl_window*>(data);

            if(width > 0 && height > 0)
            {
                window->_windowState.width = width;
                window->_windowState.height = height;
            }

            window->_windowState.needUpdate = true;
        },
        .close = [](void* data, xdg_toplevel* toplevel) {},
        .configure_bounds = [](void* data, xdg_toplevel* toplevel, int32_t width, int32_t height) {},
        .wm_capabilities = [](void* data, xdg_toplevel* toplevel, wl_array* capabilities) {}
    };

    // Window
    void wl_window::_startRenderFrame()
    {
        switch (application::getBackendType()) 
        {
            #if GOOSEUI_HAS_OPENGL
            case application::backendType::OpenGL:
            {
                graphics::gl::glRenderer* glBackend = static_cast<graphics::gl::glRenderer*>(application::getRenderer());
                graphics::gl::wl_glContext* glContext = static_cast<graphics::gl::wl_glContext*>(_windowCtx);
                
                eglMakeCurrent(glBackend->getContext().display, glContext->surface, glContext->surface, glBackend->getContext().ctx);\
                if(_windowState.needUpdate)
                {
                    wl_egl_window_resize(glContext->window, _windowState.width, _windowState.height, 0, 0);
                }

                break;
            }
            #endif
            
            #if GOOSEUI_ENABLE_VULKAN
            #endif
            
            default:
                printf("GooseUI: Backend Not Initilized! \n");
                break;
        }

        _windowState.needUpdate = false;
        application::getRenderer()->beginFrame(getWidth(), getHeight(), _bgColor);
    }

    void wl_window::_endRenderFrame()
    {
        application::getRenderer()->endFrame();
        
        switch (application::getBackendType()) 
        {
            #if GOOSEUI_HAS_OPENGL
            case application::backendType::OpenGL:
            {
                graphics::gl::glRenderer* glBackend = static_cast<graphics::gl::glRenderer*>(application::getRenderer());
                graphics::gl::wl_glContext* glContext = static_cast<graphics::gl::wl_glContext*>(_windowCtx);

                eglSwapBuffers(glBackend->getContext().display, glContext->surface);
                eglMakeCurrent(glBackend->getContext().display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
                
                break;
            }
            #endif
            
            #if GOOSEUI_ENABLE_VULKAN
            #endif
            
            default:
                printf("GooseUI: Backend Not Initilized! \n");
                break;
        }
    }

    #if GOOSEUI_HAS_OPENGL
    void wl_window::_gl_createContext()
    {
        graphics::gl::glRenderer* glBackend = static_cast<graphics::gl::glRenderer*>(application::getRenderer());
        EGLDisplay eglDisplay = EGL_NO_DISPLAY;

        if(!glBackend->hasContext())
        {
            eglDisplay = eglGetDisplay((EGLNativeDisplayType)_display);
            eglInitialize(eglDisplay, nullptr, nullptr);
        } else 
        {
            eglDisplay = glBackend->getContext().display;
        }

        const EGLint configAttribs[] = {
            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
            EGL_RED_SIZE,        8,
            EGL_GREEN_SIZE,      8,
            EGL_BLUE_SIZE,       8,
            EGL_ALPHA_SIZE,      8,
            EGL_DEPTH_SIZE,      24,
            EGL_STENCIL_SIZE,    8,
            EGL_NONE
        };

        const EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL_NONE
        };

        EGLConfig config; EGLint configs;
        eglChooseConfig(eglDisplay, configAttribs, &config, 1, &configs);

        eglBindAPI(EGL_OPENGL_API);

        graphics::gl::wl_glContext* glCtx = new graphics::gl::wl_glContext();
        EGLContext sharedCtx = EGL_NO_CONTEXT;
        
        glCtx->window = wl_egl_window_create(_surface, _windowState.width, _windowState.height);
        glCtx->surface = eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType)glCtx->window, nullptr);
        _windowCtx = glCtx;

        if(!glBackend->hasContext())
        {
            sharedCtx = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, contextAttribs);
            glBackend->setContext(new graphics::gl::glContext{sharedCtx, eglDisplay}); 
        }else 
        {
            sharedCtx = glBackend->getContext().ctx;
        }

        eglMakeCurrent(eglDisplay, glCtx->surface, glCtx->surface, sharedCtx);
    }

    void wl_window::_gl_destoryContext()
    {
        
    }
    #endif

    #if GOOSEUI_HAS_VULKAN
    #endif
}

namespace GooseUI::platform // public
{
    wl_window::wl_window(const windowCreationInfo& info)
    {
        printf("GooseUI: Using [Wayland -> Wayland]\n");

        if(!_display)
        {
            _display = wl_display_connect(nullptr);
            if(!_display){ printf("GooseUI: Failed to open Wayland Display Server \n"); }

            _registry = wl_display_get_registry(_display);
            wl_registry_add_listener(_registry, &_registry_listener, nullptr);

            wl_display_roundtrip(_display);
            if(!_compositor || !_xdg_wm_base){ printf("GooseUI: Interfaces (compositor/xdg_wm) not found \n"); }
        }

        // wayland :<...
        printf("GooseUI [NOTICE]: Wayland does not support client defiend window posistioning\n");

        // Create window
        _windowState.height = info.height;
        _windowState.width = info.width;
        
        _surface = wl_compositor_create_surface(_compositor);
        _xdg_surface = xdg_wm_base_get_xdg_surface(_xdg_wm_base, _surface);
        _xdg_toplevel = xdg_surface_get_toplevel(_xdg_surface);

        xdg_surface_add_listener(_xdg_surface, &_xdg_surface_listener, this);
        xdg_toplevel_add_listener(_xdg_toplevel, &_xdg_toplevel_listener, this);

        wl_surface_commit(_surface);
        wl_display_roundtrip(_display);

        // Init Backend
        _bgColor = { 1.0f, 1.0f, 1.0f, 1.0f };

        switch (application::getBackendType()) 
        {
            #if GOOSEUI_HAS_OPENGL
            case application::backendType::OpenGL:
                _gl_createContext();
                application::getRenderer()->initRenderer();
                break;
            #endif
            
            #if GOOSEUI_ENABLE_VULKAN
            #endif
            
            default:
                printf("GooseUI: Backend Not Initilized! \n");
                break;
        }

        _isRunning = true;
    }
    
    wl_window::~wl_window()
    {
        if(!_isRunning && _surface == None){ return; }
        _isRunning = false;

        switch (application::getBackendType()) 
        {
            #if GOOSEUI_HAS_OPENGL
            case application::backendType::OpenGL:
                _gl_destoryContext();
                break;
            #endif
            
            #if GOOSEUI_ENABLE_VULKAN
            #endif
            
            default:
                printf("GooseUI: Backend Not Initilized! \n");
                break;
        }
    }
    
    wl_surface* wl_window::getSurface(){ return _surface; }
    wl_display* wl_window::getDisplay(){ return _display; }

    // OVERIDES
    displayService wl_window::getDisplayService() const { return displayService::wayland; }
    void wl_window::setBackgroundColor(color color){ _bgColor = color; }
    
    // Titlebar
    void wl_window::setTitleBarDecorations(const titlebarCreationInfo& info)
    {
        int windowEventID = static_cast<int>(reinterpret_cast<intptr_t>(_xdg_toplevel)) * 2;

        // Remove Decorations
        if(!info.visible)
        {
            if(_clientDecorations != nullptr)
            { 
                graphics::titleBar::removeDefaultDecorations(_clientDecorations, windowEventID, this); 
                if(_xdg_surface){ xdg_surface_set_window_geometry(_xdg_surface, 0, 0, _windowState.width, _windowState.height); }
            }

            if(hasServerSideDecorations(_decoration_manager, _xdg_toplevel_decorations, _decorationMode))
            {
                zxdg_toplevel_decoration_v1_unset_mode(_xdg_toplevel_decorations);
            }

            return;
        }

        // Server Side Decorations
        if(_xdg_toplevel_decorations == nullptr && _decoration_manager != nullptr){ _xdg_toplevel_decorations = zxdg_decoration_manager_v1_get_toplevel_decoration(_decoration_manager, _xdg_toplevel); }
        if(info.type == windowDecoration::ServerSide)
        {
            if(_xdg_toplevel_decorations)
            {
                zxdg_toplevel_decoration_v1_set_mode(_xdg_toplevel_decorations, ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);
                _decorationMode = ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE;
                return;
            }
        }

        // Client Side Decorations
        if(_xdg_toplevel_decorations){ zxdg_toplevel_decoration_v1_set_mode(_xdg_toplevel_decorations, ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE); }
        if(_xdg_surface){ xdg_surface_set_window_geometry(_xdg_surface, 0, DEF_GSA_WINDOW_BORDER_PADDING, _windowState.width, _windowState.height - DEF_GSA_WINDOW_BORDER_PADDING); }
        _decorationMode =  ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE;

        event::dispatcher &deRefDispatcher = *info.evtDispatcher;
        graphics::titleBar::createDefaultDecorations(info.type, _clientDecorations, this, windowEventID, deRefDispatcher);

        // Header
        deRefDispatcher.add(windowEventID, [this](GooseUI::event::data evt){
            if(_xdg_toplevel && _seat){ xdg_toplevel_move(_xdg_toplevel, _seat, _lastPointerSerial); }
        });

        // Close Button
        deRefDispatcher.add(windowEventID + 1, [this](GooseUI::event::data evt){ close(); });
    }
    
    absractions::iWidget* wl_window::getClientTitleBar() { if(_clientDecorations){ return _clientDecorations->bar; } return nullptr; }
    
    // Window Size
    void wl_window::setSize(int width, int height){}
    void wl_window::isResizeable(bool isResizeable){}
    void wl_window::maximize(){}
    void wl_window::minimize(){}
    void wl_window::restoreSize(){}
    
    int wl_window::getWidth(){ return _windowState.width; }
    int wl_window::getHeight(){ return _windowState.height; }
    
    // Window Visibility
    void wl_window::isAllwaysOnTop(bool isOnTop){}
    void wl_window::show(){}
    void wl_window::hide(){}
    void wl_window::close(){ _isRunning = false;  }
    
    // Widget Management
    void wl_window::addWidgetToVector(absractions::iWidget* widget) { _widgets.push_back(widget); }
    void wl_window::removeWidgetFromVector(absractions::iWidget* widget)
    {
        std::vector<absractions::iWidget*>::iterator target = std::find(_widgets.begin(), _widgets.end(), widget);
        if(target != _widgets.end()){ _widgets.erase(target); }
    }
    
    void wl_window::renderWidgets()
    {
        if(!application::getRenderer()) return;
        _startRenderFrame();
        
        for(absractions::iWidget* widget : _widgets)
        {
            if(widget)
            {
                widget->draw(); // Thought I was just copying and pasteing, huh?
            }
        }
        
        _endRenderFrame();
    }
    
    void wl_window::handelEvents() 
    {
        renderWidgets();
    }
}