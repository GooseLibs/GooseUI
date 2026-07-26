#include "GooseUI/platform/wl_window.h"
#include "GooseUI/context.h"

#include <cstring>


namespace GooseUI::platform // Static 
{
    wl_display* wl_window::_display = nullptr;
    wl_registry* wl_window::_registry = nullptr; 
    wl_compositor* wl_window::_compositor = nullptr;
    xdg_wm_base* wl_window::_xdg_wm_base = nullptr;
    
    void wl_window::_registry_handle(void* data, wl_registry* reg, uint32_t id, const char* interface, uint32_t version)
    {
        if(std::strcmp(interface, "wl_compositor") == 0)
        {
            wl_window::_compositor = (wl_compositor*)wl_registry_bind(reg, id, &wl_compositor_interface, std::min(version, 4u));
        }else if(std::strcmp(interface, "xdg_wm_base") == 0)
        {
            wl_window::_xdg_wm_base = (xdg_wm_base*)wl_registry_bind(reg, id, &xdg_wm_base_interface, std::min(version, 4u));
            static const xdg_wm_base_listener wm_listener = {[](void*, xdg_wm_base* wm, uint32_t s){ xdg_wm_base_pong(wm, s); }};
            xdg_wm_base_add_listener(wl_window::_xdg_wm_base, &wm_listener, nullptr);
        }
    }

    void wl_window::_registry_remover(void* data, wl_registry* reg, uint32_t id) {}
    const wl_registry_listener wl_window::_registry_listener = { _registry_handle, _registry_remover };

    // XDG
    const xdg_surface_listener  wl_window::_xdg_surface_listener = {
        .configure = [](void* data, xdg_surface* xdg_surface, uint32_t serial) {
            xdg_surface_ack_configure(xdg_surface, serial);
        }
    };

    const xdg_toplevel_listener wl_window::_xdg_toplevel_listener = {
        .configure = [](void* data, xdg_toplevel* toplevel, int32_t width, int32_t height, wl_array* states) {
            wl_window* window = static_cast<wl_window*>(data);
        },
        .close = [](void* data, xdg_toplevel* toplevel) {
        },
        .configure_bounds = [](void* data, xdg_toplevel* toplevel, int32_t width, int32_t height) {},
        .wm_capabilities = [](void* data, xdg_toplevel* toplevel, wl_array* capabilities) {}
    };
}

namespace GooseUI::platform // Private
{
    void wl_window::_startRenderFrame()
    {
        
    }

    void wl_window::_endRenderFrame()
    {
        
    }

    #if GOOSEUI_HAS_OPENGL
    void wl_window::_gl_createContext()
    {
        
    }

    void wl_window::_gl_shareContext() {}

    void wl_window::_gl_destoryContext()
    {
        
    }
    #endif

    #if GOOSEUI_HAS_VULKAN
    #endif
}

namespace GooseUI::platform // Public
{
    wl_window::wl_window(const std::string& title, int width, int height, screenPosistion posistion)
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

        // Not Calculate screen posistion
        printf("GooseUI: Wayland does not support client defiend screen posistion\n");

        // Create window, title, etc
        _surface = wl_compositor_create_surface(_compositor);
        _xdg_surface = xdg_wm_base_get_xdg_surface(_xdg_wm_base, _surface);
        _xdg_toplevel = xdg_surface_get_toplevel(_xdg_surface);

        xdg_surface_add_listener(_xdg_surface, &_xdg_surface_listener, this);
        xdg_toplevel_add_listener(_xdg_toplevel, &_xdg_toplevel_listener, this);
        xdg_toplevel_set_title(_xdg_toplevel, title.c_str());

        wl_display_roundtrip(_display);
        
        // Init Backend
        _bgColor = { 1.0f, 1.0f, 1.0f, 1.0f };
        
        switch (application::getBackendType()) 
        {
            #if GOOSEUI_HAS_OPENGL
            case application::backendType::OpenGL:
                _gl_createContext();
                application::getRenderer()->initRenderer();
                _gl_shareContext();
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

    wl_surface* wl_window::getSurface() { return _surface; }
    wl_display* wl_window::getDisplay() { return _display; }

    // OVERIDES
    // Window Configuration
    void wl_window::setWindowIcon(const std::string& ICO) {} // TODO
    void wl_window::setHeader(const std::string& title, bool isVisible, bool hasButtons, bool hasMinimize, bool hasMaximise) {}
    void wl_window::isResizeable(bool isResizeable) {}
    void wl_window::isAllwaysOnTop(bool isOnTop) {}
    void wl_window::setSize(int width, int height) {}
    void wl_window::maximize() {}
    void wl_window::minimize() {}
    void wl_window::restoreSize() {}

    // Window Visibility
    void wl_window::show() {}
    void wl_window::hide() {}
    void wl_window::destroy() {}

    // Widget Management
    void wl_window::addWidgetToVector(absractions::iWidget* widget) {}
    void wl_window::removeWidgetFromVector(absractions::iWidget* widget) {}
    void wl_window::renderWidgets() {}
    void wl_window::handelEvents() {}

    // returns
    displayService wl_window::getDisplayService() const { return displayService::wayland; }
    int wl_window::getWidth() {};
    int wl_window::getHeight() {};
}