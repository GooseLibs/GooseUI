#include "GooseUI/platform/x11_window.h"
#include "GooseUI/platform/x11_decorations.h"

#include <X11/Xatom.h>
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
    
    void x11_window::_gl_destoryContext(){} // TODO
    #endif

    #if GOOSEUI_HAS_VULKAN
    #endif
}

namespace GooseUI::platform // Public
{
    x11_window::x11_window(const windowCreationInfo& info)
    {
        printf("GooseUI: Using [DisplayServer -> x11]\n");
        
        _display = XOpenDisplay(nullptr);
        if(!_display) { printf("GooseUI: Failed to open xDisplay \n"); }
        
        // Calulate Screen Posistion
        int defaultScreen = DefaultScreen(_display);
        int screenWidth = DisplayWidth(_display, defaultScreen);
        int screenHeight = DisplayHeight(_display, defaultScreen);
        int posX, posY;
        
        switch(info.posistion)
        {
            case SCREEN_TOP:
                posX = (screenWidth - info.width) / 2;
                break;
            case SCREEN_BOTTOM:
                posX = (screenWidth - info.width) / 2;
                posY = screenHeight - info.height;
                break;
            case SCREEN_LEFT:
                posY = (screenHeight - info.height) / 2;
                break;
            case SCREEN_RIGHT:
                posX = screenWidth - info.width;
                posY = (screenHeight - info.height) / 2;
                break;
            case SCREEN_TOP_LEFT:
                break;
            case SCREEN_TOP_RIGHT:
                posX = screenWidth - info.width;
                break;
            case SCREEN_BOTTOM_LEFT:
                posY = screenHeight - info.height;
                break;
            case SCREEN_BOTTOM_RIGHT:
                posX = screenWidth - info.width;
                posY = screenHeight - info.height;
                break;
            case SCREEN_CENTER:
                posX = (screenWidth - info.width) / 2;
                posY = (screenHeight - info.height) / 2;
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
            info.width,
            info.height,
            1,
            BlackPixel(_display, defaultScreen),
            WhitePixel(_display, defaultScreen)
        );
        
        // Forgo the proper error logic I will, thou only has error message... :3c
        if(!_window) { printf("GooseUI: Failed to create X11 Window \n"); }

        // Titlebar
        long hints[5] = { 2, 0, 0, 0, 0 };
        Atom motifHintsAtom = XInternAtom(_display, "_MOTIF_WM_HINTS", False);
        Atom windowTypeAtom = XInternAtom(_display, "_NET_WM_WINDOW_TYPE", False);
        Atom windowTypeNormalAtom = XInternAtom(_display, "_NET_WM_WINDOW_TYPE_NORMAL", False);

        XChangeProperty(_display, _window, motifHintsAtom, motifHintsAtom, 32, PropModeReplace, (unsigned char*)hints, 5);
        XChangeProperty(_display, _window, windowTypeAtom, XA_ATOM, 32, PropModeReplace, (unsigned char*)&windowTypeNormalAtom , 1);

        XClassHint classHint;
        classHint.res_name = (char*)"GooseUI";
        classHint.res_class = (char*)"GooseUI";
        XSetClassHint(_display, _window, &classHint);

        XWMHints* wmHints = XAllocWMHints();
        if(wmHints)
        {
            wmHints->flags = InputHint | StateHint;
            wmHints->input = True;
            wmHints->initial_state = NormalState;
            XSetWMHints(_display, _window, wmHints);
            XFree(wmHints);
        }

        // Attributes
        XSetWindowAttributes attributes;
        attributes.background_pixmap = None;
        XChangeWindowAttributes(_display, _window, CWBackPixmap, &attributes);
        
        // Protocalls and input
        _wm_delete_window = XInternAtom(_display, "WM_DELETE_WINDOW", False);
        XSetWMProtocols(_display, _window, &_wm_delete_window, 1);
        XSelectInput(_display, _window, ExposureMask | ButtonPressMask | ButtonReleaseMask | StructureNotifyMask | PointerMotionMask);
        
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
    displayService x11_window::getDisplayService() const { return displayService::x11; }
    void x11_window::setBackgroundColor(color color){ _bgColor = color; }

    // Titlebar
    void x11_window::setTitleBarDecorations(const titlebarCreationInfo& info){ x11_ModifieDecoration(this, _clientDecorations, info);}
    absractions::iWidget* x11_window::getClientTitleBar() { if(_clientDecorations){ return _clientDecorations->bar; } return nullptr; }

    // Window Size
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

    void x11_window::isResizeable(bool isResizeable)
    {
        XSizeHints* sizeHints = XAllocSizeHints();

        long suppliedHints;
        XGetWMNormalHints(_display, _window, sizeHints, &suppliedHints);
        
        sizeHints->flags &= ~(PMinSize | PMaxSize);
        
        XSetWMNormalHints(_display, _window, sizeHints);
        XFree(sizeHints);
    }

    void x11_window::maximize()
    {
        Atom wmState = XInternAtom(_display, "_NET_WM_STATE", false);
        Atom maxWidth = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_HORZ", false);
        Atom maxHeight = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_VERT", false);
        
        XEvent event = {};
        event.xclient.type = ClientMessage;
        event.xclient.window = _window;
        event.xclient.message_type = wmState;
        event.xclient.format = 32;
        event.xclient.data.l[0] = 1;
        event.xclient.data.l[1] = maxWidth;
        event.xclient.data.l[2] = maxHeight;
        event.xclient.data.l[3] = 1;
        
        XSendEvent(_display, DefaultRootWindow(_display), false, SubstructureRedirectMask | SubstructureNotifyMask, &event);
        XFlush(_display);
    }

    void x11_window::minimize()
    {
        XIconifyWindow(_display, _window, DefaultScreen(_display)); 
        XFlush(_display);
    }

    void x11_window::restoreSize()
    {
        Atom wmState = XInternAtom(_display, "_NET_WM_STATE", false);
        Atom maxWidth = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_HORZ", false);
        Atom maxHeight = XInternAtom(_display, "_NET_WM_STATE_MAXIMIZED_VERT", false);
        
        XEvent event = {};
        event.xclient.type = ClientMessage;
        event.xclient.window = _window;
        event.xclient.message_type = wmState;
        event.xclient.format = 32;
        event.xclient.data.l[0] = 0;
        event.xclient.data.l[1] = maxWidth;
        event.xclient.data.l[2] = maxHeight;
        event.xclient.data.l[3] = 1;
        
        XSendEvent(_display, DefaultRootWindow(_display), false, SubstructureRedirectMask | SubstructureNotifyMask, &event);
        XFlush(_display);
    }
    
    int x11_window::getWidth() { XWindowAttributes windowAtr; XGetWindowAttributes(_display, _window, &windowAtr); return windowAtr.width; }
    int x11_window::getHeight() { XWindowAttributes windowAtr; XGetWindowAttributes(_display, _window, &windowAtr); int height = windowAtr.height; return height; }

    // Window Visibility
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

    void x11_window::show() { XMapWindow(_display, _window); XFlush(_display); }
    void x11_window::hide() { XUnmapWindow(_display, _window); XFlush(_display); }
    void x11_window::destroy() { XDestroyWindow(_display, _window); XFlush(_display); }

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
                case MotionNotify:
                {
                    if(!_clientDecorations){ break; }
                    
                    int x = event.xmotion.x + X11_BORDER_PADDING;
                    int y = event.xmotion.y + X11_BORDER_PADDING;

                    int resizeDirection = x11_edgeHitTest(x, y, getWidth(), getHeight());
                    if (resizeDirection != -1) 
                    {
                        Cursor cursor = x11_getCursor(_display, resizeDirection);
                        XDefineCursor(_display, _window, cursor);
                        XFreeCursor(_display, cursor);
                    } 
                    else{ XUndefineCursor(_display, _window); }

                    break;
                }
                case ButtonPress:
                {
                    evtData.mouseX = event.xbutton.x;
                    evtData.mouseY = event.xbutton.y;
                    evtData.mouseRootX = event.xbutton.x_root;
                    evtData.mouseRootY = event.xbutton.y_root;
        
                    if(event.xbutton.button == Button1) 
                    { 
                        evtData.dataType = event::type::leftMouseDown;
                        if(!_clientDecorations){ break; }
                        
                        int resizeDirection = x11_edgeHitTest(evtData.mouseX += X11_BORDER_PADDING, evtData.mouseY += X11_BORDER_PADDING, getWidth(), getHeight());
                        if(resizeDirection != -1){ x11_startNativeResize(this, evtData.mouseRootX, evtData.mouseRootY, resizeDirection); handelWidgets = false; }
                        break;
                    }
                    
                    if(event.xbutton.button == Button3) { evtData.dataType = event::type::rightMouseDown; break; }
                    break;
                }
                case ButtonRelease:
                {
                    evtData.mouseX = event.xbutton.x;
                    evtData.mouseY = event.xbutton.y;
                    evtData.mouseRootX = event.xbutton.x_root;
                    evtData.mouseRootY = event.xbutton.y_root;
                            
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
}