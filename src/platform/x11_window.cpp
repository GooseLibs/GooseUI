#include "GooseUI/platform/x11_window.h"
#include "GooseUI/context.h"

#include <algorithm>


namespace GooseUI::platform // Private
{
    void x11_window::_startRenderFrame()
    {
        switch (application::getBackendType()) 
        {
            #if GOOSEUI_HAS_OPENGL
            case application::backendType::OpenGL:
            {
                graphics::gl::glRenderer* glBackend = static_cast<graphics::gl::glRenderer*>(application::getRenderer());
                eglMakeCurrent(glBackend->getContext().display, (EGLSurface)_windowCtx, (EGLSurface)_windowCtx, glBackend->getContext().ctx);

                break;
            }
            #endif
            
            #if GOOSEUI_ENABLE_VULKAN
            #endif
            
            default:
                printf("GooseUI: Backend Not Initilized! \n");
                break;
        }
        
        XWindowAttributes windowAtr; 
        XGetWindowAttributes(_display, _window, &windowAtr);
        
        application::getRenderer()->beginFrame(getWidth(), getHeight(), _bgColor);
    }
    
    void x11_window::_endRenderFrame()
    {
        application::getRenderer()->endFrame();
        
        switch (application::getBackendType()) 
        {
            #if GOOSEUI_HAS_OPENGL
            case application::backendType::OpenGL:
            {
                graphics::gl::glRenderer* glBackend = static_cast<graphics::gl::glRenderer*>(application::getRenderer());
                
                eglSwapBuffers(glBackend->getContext().display, (EGLSurface)_windowCtx);
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
    
    // Graphic Dependent
    #if GOOSEUI_HAS_OPENGL
    void x11_window::_gl_createContext()
    {
        graphics::gl::glRenderer* glBackend = static_cast<graphics::gl::glRenderer*>(application::getRenderer());
        EGLDisplay eglDisplay = EGL_NO_DISPLAY;
        
        if(!glBackend->hasContext())
        {
            eglDisplay = eglGetDisplay((EGLNativeDisplayType)_display);
            eglInitialize(eglDisplay, nullptr, nullptr);
        }else 
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

        EGLSurface surface = eglCreateWindowSurface(eglDisplay, config, (EGLNativeWindowType)_window, nullptr);
        EGLContext sharedCtx = EGL_NO_CONTEXT;

        _windowCtx = surface;
        
        if (!glBackend->hasContext()) 
        { 
            sharedCtx = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, contextAttribs);
            glBackend->setContext(new graphics::gl::glContext{sharedCtx, eglDisplay}); 
        } else 
        {
            sharedCtx = glBackend->getContext().ctx;
        }

        eglMakeCurrent(eglDisplay, surface, surface, sharedCtx);
    }
    
    void x11_window::_gl_shareContext() {} // Unused
    
    void x11_window::_gl_destoryContext()
    {
        // TODO
    }
    #endif

    #if GOOSEUI_HAS_VULKAN
    #endif
}

namespace GooseUI::platform // Public
{
    x11_window::x11_window(const std::string& title, int width, int height, screenPosistion posistion)
    {
        printf("GooseUI: Using [xSever -> x11]\n");
        
        _display = XOpenDisplay(nullptr);
        if(!_display) { printf("GooseUI: Failed to open xDisplay \n"); }
        
        // Calulate Screen Posistion
        int defaultScreen = DefaultScreen(_display);
        int screenWidth = DisplayWidth(_display, defaultScreen);
        int screenHeight = DisplayHeight(_display, defaultScreen);
        int posX, posY;
        
        switch(posistion)
        {
            case SCREEN_TOP:
                posX = (screenWidth - width) / 2;
                break;
            case SCREEN_BOTTOM:
                posX = (screenWidth - width) / 2;
                posY = screenHeight - height;
                break;
            case SCREEN_LEFT:
                posY = (screenHeight - height) / 2;
                break;
            case SCREEN_RIGHT:
                posX = screenWidth - width;
                posY = (screenHeight - height) / 2;
                break;
            case SCREEN_TOP_LEFT:
                break;
            case SCREEN_TOP_RIGHT:
                posX = screenWidth - width;
                break;
            case SCREEN_BOTTOM_LEFT:
                posY = screenHeight - height;
                break;
            case SCREEN_BOTTOM_RIGHT:
                posX = screenWidth - width;
                posY = screenHeight - height;
                break;
            case SCREEN_CENTER:
                posX = (screenWidth - width) / 2;
                posY = (screenHeight - height) / 2;
                break;

            default:
                break;
        }
        
        // Create Window
        _window = XCreateSimpleWindow(
            _display,
            RootWindow(_display, defaultScreen),
            posX,
            posY,
            width,
            height,
            1,
            BlackPixel(_display, defaultScreen),
            WhitePixel(_display, defaultScreen)
        );
        
        // Forgo the proper error logic I will, thou only has error message... :3c
        if(!_window) { printf("GooseUI: Failed to create X11 Window \n"); }
        XStoreName(_display, _window, title.c_str());

        // Attributes
        XSetWindowAttributes attributes;
        attributes.background_pixmap = None;
        XChangeWindowAttributes(_display, _window, CWBackPixmap, &attributes);
        
        // Protocalls and input
        _wm_delete_window = XInternAtom(_display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(_display, _window, &_wm_delete_window, 1);
        
        XSelectInput(_display, _window, ExposureMask | ButtonPressMask | ButtonReleaseMask | StructureNotifyMask);
        
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
    
    x11_window::~x11_window()
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
        
        XDestroyWindow(_display, _window);
        XCloseDisplay(_display);
    }
    
    Display* x11_window::getDisplay() { return _display; }
    Window x11_window::getWindow() { return _window; }
    
    // OVERIDES
    // Window Configuration
    void x11_window::setWindowIcon(const std::string& ICO) 
    {
        // TODO
    };
    
    void x11_window::setHeader(const std::string& title, bool isVisible, bool hasButtons, bool hasMinimize, bool hasMaximise)
    {
        if(!title.empty()) { XStoreName(_display, _window, title.c_str()); }
        
        // This still is pain, I fear Wayland... please be easier
        if(isVisible)
        {
            struct { long flags, functions, decorations, inputMode, status; } *existingHints;
            struct { long flags, functions, decorations, inputMode, status; } hints = {0};
            
            Atom motif = XInternAtom(_display, "_MOTIF_WM_HINTS", False);
            Atom aType;
            
            int aFormat;
            unsigned long nitems, bytesAfter;
            unsigned char *prop;
            
            hints.flags = (1L << 0) | (1L << 1);
            hints.decorations = (1L << 3) | (1L << 1);
            hints.functions = (1L << 2) | (1L << 5);
            
            // Restore Exsisting Hints managed by other functions
            bool isResiablable = true;
            if (XGetWindowProperty(_display, _window, motif, 0, 5, False, motif, &aType, &aFormat, &nitems, &bytesAfter, &prop) == Success && prop) 
            {
                existingHints = reinterpret_cast<decltype(existingHints)>(prop);
                isResiablable = (existingHints->functions & (1L << 1));
                XFree(prop);
            }
            
            if(isResiablable) {
                hints.functions |= (1L << 1);
            }
            
            // Buttons
            if(hasButtons)
            {
                if(hasMinimize)
                {
                    hints.decorations |= (1L << 5);
                    hints.functions   |= (1L << 3);
                }
                
                if(hasMaximise)
                {
                    hints.decorations |= (1L << 6);
                    hints.functions   |= (1L << 4);
                }
            }
            
            XChangeProperty(_display, _window, motif, motif, 32, PropModeReplace, reinterpret_cast<unsigned char*>(&hints), 5);
        }else 
        {
            XDeleteProperty(_display, _window, XInternAtom(_display, "_MOTIF_WM_HINTS", False));
        }
        
        XFlush(_display);
    }
    
    void x11_window::isResizeable(bool isResizeable)
    {
        XSizeHints* sizeHints = XAllocSizeHints();
        
        if(!isResizeable)
        {
            sizeHints->flags = PMinSize | PMaxSize;
            sizeHints->min_width = sizeHints->max_width = getWidth();
            sizeHints->min_height = sizeHints->max_height = getHeight();
        }else 
        {
            long currentHints;
            XGetWMNormalHints(_display, _window, sizeHints, &currentHints);
            sizeHints->flags &= ~(PMinSize | PMaxSize);
        }
        
        XSetWMNormalHints(_display, _window, sizeHints);
        XFree(sizeHints);
    }
    
    void x11_window::isAllwaysOnTop(bool isOnTop)
    {
        Atom wmState = XInternAtom(_display, "_NET_WM_STATE", False);
        Atom wmAbove = XInternAtom(_display, "_NET_WM_STATE_ABOVE", False);
        
        XEvent event;
        event.type = ClientMessage;
        event.xclient.window = _window;
        event.xclient.message_type = wmState;
        event.xclient.format = 32;
        
        if(isOnTop) { event.xclient.data.l[0] = 1; }
        else { event.xclient.data.l[0] = 0; }
        
        event.xclient.data.l[2] = 0;
        event.xclient.data.l[3] = 1;
        event.xclient.data.l[4] = 0;
        
        XSendEvent(_display, DefaultRootWindow(_display), False, SubstructureRedirectMask | SubstructureNotifyMask, &event);
        XFlush(_display);
    }
    
    void x11_window::setSize(int width, int height)
    {
        XSizeHints sizeHints;
        sizeHints.flags = PSize;
        sizeHints.width = width;
        sizeHints.height = height;
        
        XSetNormalHints(_display, _window, &sizeHints);
        XResizeWindow(_display, _window, width, height);
        XFlush(_display);
    }
    
    void x11_window::maximize()
    {
        Atom wmState = XInternAtom(_display, "_NET_WM_STATE", false);
        Atom maxWidth = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_HORZ", false);
        Atom maxHeight = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_VERT", false);
        
        XEvent event;
        event.xclient.type = ClientMessage;
        event.xclient.window = _window;
        event.xclient.message_type = wmState;
        event.xclient.format = 32;
        event.xclient.data.l[0] = 1;
        event.xclient.data.l[1] = maxWidth;
        event.xclient.data.l[2] = maxHeight;
        
        XSendEvent(_display, DefaultRootWindow(_display), false, SubstructureNotifyMask, &event);
    }
    
    void x11_window::minimize()
    {
        XIconifyWindow(_display, _window, DefaultScreen(_display)); XFlush(_display);
    }
    
    void x11_window::restoreSize()
    {
        Atom wmState = XInternAtom(_display, "_NET_WM_STATE", false);
        Atom maxWidth = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_HORZ", false);
        Atom maxHeight = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_VERT", false);
        
        XEvent event;
        event.xclient.type = ClientMessage;
        event.xclient.window = _window;
        event.xclient.message_type = wmState;
        event.xclient.format = 32;
        event.xclient.data.l[0] = 0;
        event.xclient.data.l[1] = maxWidth;
        event.xclient.data.l[2] = maxHeight;
        
        XSendEvent(_display, DefaultRootWindow(_display), false, SubstructureNotifyMask, &event);
    }
    
    // Window Visibility
    void x11_window::show() { XMapWindow(_display, _window); XFlush(_display); }
    void x11_window::hide() { XUnmapWindow(_display, _window); XFlush(_display); }
    void x11_window::destroy() { XDestroyWindow(_display, _window); }
    
    // Widget Management
    void x11_window::addWidgetToVector(absractions::iWidget* widget) { _widgets.push_back(widget); }
    void x11_window::removeWidgetFromVector(absractions::iWidget* widget)
    {
        std::vector<absractions::iWidget*>::iterator target = std::find(_widgets.begin(), _widgets.end(), widget);
        if(target != _widgets.end()){ _widgets.erase(target); }
    }
    
    void x11_window::renderWidgets()
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
    
    void x11_window::handelEvents()
    {
        XEvent event;
        int events = XPending(_display);
        
        while(events > 0) // FIX??
        {
            XNextEvent(_display, &event);
            events--;
            
            if (event.xany.window != _window) 
            {
                XPutBackEvent(_display, &event);
                continue;
            }
            
            // Set evtData & run event loop
            bool handelWidgets = true;
            event::data evtData;
                    
            switch (event.type)
            {
                case ClientMessage:
                {
                    if((Atom)event.xclient.data.l[0] == _wm_delete_window) { _isRunning = false; handelWidgets = false; }
                    break;
                }
                case ButtonPress:
                {
                    evtData.mouseX = event.xbutton.x;
                    evtData.mouseY = event.xbutton.y;
        
                    if(event.xbutton.button == Button1) { evtData.dataType = event::type::leftMouseDown; break; }
                    if(event.xbutton.button == Button3) { evtData.dataType = event::type::rightMouseDown; break; }
                    break;
                }
                case ButtonRelease:
                {
                    evtData.mouseX = event.xbutton.x;
                    evtData.mouseY = event.xbutton.y;
                            
                    if(event.xbutton.button == Button1) { evtData.dataType = event::type::leftMouseUp; break; }
                    break;
                }
        
                default: { handelWidgets = false; break; }
            }
        
            if(handelWidgets)
            {
                for(absractions::iWidget* widget : _widgets)
                {
                    if(widget)
                    {
                        widget->pollEvent(evtData);
                    }
                }
            }
        }
  
        renderWidgets();
    }
    
    // Returns
    displayService x11_window::getDisplayService() const { return displayService::x11; }
    int x11_window::getWidth() { XWindowAttributes windowAtr; XGetWindowAttributes(_display, _window, &windowAtr); return windowAtr.width; }
    int x11_window::getHeight() { XWindowAttributes windowAtr; XGetWindowAttributes(_display, _window, &windowAtr); return windowAtr.height; }
}