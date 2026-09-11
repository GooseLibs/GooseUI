#include "GooseUI/platform/x11_window.h"

#include <X11/Xatom.h>
#include <X11/Xcursor/Xcursor.h>
#include <algorithm>


namespace GooseUI::platform // Local
{
    struct WM_Hints { unsigned long flags; unsigned long functions; unsigned long decorations; long input_mode; unsigned long status; };
    
    bool hasServerSideDecorations(Display* display, Window &window)
    {   
        Atom motifHintsAtom = XInternAtom(display, "_MOTIF_WM_HINTS", True);
        if(motifHintsAtom == None){ return true; }

        Atom actualType;
        int actualFormat;
        unsigned long nItems, bytesAfter;
        WM_Hints* hints = nullptr;
        int result = XGetWindowProperty(display, window, motifHintsAtom, 0, 5, False, AnyPropertyType, &actualType, &actualFormat, &nItems, &bytesAfter, reinterpret_cast<unsigned char**>(&hints));

        bool decorated = true;
        if (hints && (hints->flags & 2)) 
        {
            decorated = hints->decorations;
            XFree(hints);
        }

        return decorated;
    }
    
    int isCursorOnWindowEdge(int x, int y, int win_width, int win_height)
    {
        bool top    = y <= DEF_GSA_WINDOW_BORDER_PADDING + (DEF_GSA_WINDOW_BORDER_PADDING / 2);
        bool bottom = y >= (win_height - DEF_GSA_WINDOW_BORDER_PADDING);
        bool left   = x <= DEF_GSA_WINDOW_BORDER_PADDING + DEF_GSA_WINDOW_BORDER_PADDING;
        bool right  = x >= (win_width - DEF_GSA_WINDOW_BORDER_PADDING);
    
        if (top && left)     return 0; // Top-Left
        if (top && right)    return 2; // Top-Right
        if (bottom && right) return 4; // Bottom-Right
        if (bottom && left)  return 6; // Bottom-Left
        if (top)             return 1; // Top
        if (right)           return 3; // Right
        if (bottom)          return 5; // Bottom
        if (left)            return 7; // Left
    
        return -1;
    }

    Cursor getDirectionalCursor(Display* display, int direction)
    {
        enum resizeDirection 
        {
            WM_RESIZE_TOPLEFT     = 0,
            WM_RESIZE_TOP         = 1,
            WM_RESIZE_TOPRIGHT    = 2,
            WM_RESIZE_RIGHT       = 3,
            WM_RESIZE_BOTTOMRIGHT = 4,
            WM_RESIZE_BOTTOM      = 5,
            WM_RESIZE_BOTTOMLEFT  = 6,
            WM_RESIZE_LEFT        = 7,
            WM_RESIZE_MOVE        = 8
        };
    
        const char* cursorName = nullptr;
        switch (direction) 
        {
            case WM_RESIZE_TOP:         cursorName = "n-resize"; break;
            case WM_RESIZE_BOTTOM:      cursorName = "s-resize"; break;
            case WM_RESIZE_LEFT:        cursorName = "w-resize"; break;
            case WM_RESIZE_RIGHT:       cursorName = "e-resize"; break;
            case WM_RESIZE_TOPLEFT:     cursorName = "nw-resize"; break;
            case WM_RESIZE_TOPRIGHT:    cursorName = "ne-resize"; break;
            case WM_RESIZE_BOTTOMLEFT:  cursorName = "sw-resize"; break;
            case WM_RESIZE_BOTTOMRIGHT: cursorName = "se-resize"; break;
            default:                    return None;
        }
    
        return XcursorLibraryLoadCursor(display, cursorName);
    }

    void startNativeResize(absractions::iWindow *iwindow, int mouseRootX, int mouseRootY, int direction)
    {
        platform::x11_window* xWindow = static_cast<platform::x11_window*>(iwindow);

        // Check if can resize
        XSizeHints hints; long flags;
        if (direction >= 0 && direction <= 7 && XGetWMNormalHints(xWindow->getDisplay(), xWindow->getWindow(), &hints, &flags)) 
        { if ((hints.flags & (PMinSize | PMaxSize)) == (PMinSize | PMaxSize) && hints.min_width == hints.max_width && hints.min_height == hints.max_height){ return; }}
        
        XEvent xev = {};
        xev.type = ClientMessage;
        xev.xclient.window = xWindow->getWindow();
        xev.xclient.message_type = XInternAtom(xWindow->getDisplay(), "_NET_WM_MOVERESIZE", False);
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = mouseRootX;
        xev.xclient.data.l[1] = mouseRootY;
        xev.xclient.data.l[2] = direction;
        xev.xclient.data.l[3] = 1;
        xev.xclient.data.l[4] = 1;
        
        XSendEvent(xWindow->getDisplay(), DefaultRootWindow(xWindow->getDisplay()), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
    }
}

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
        if(!_isRunning && _window == None){ return; }
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
        
        if(_display != None)
        {
            XDestroyWindow(_display, _window);
            XFlush(_display);
            
            _window = None;
            XCloseDisplay(_display);
            _display = nullptr;
        }
    }

    Display* x11_window::getDisplay() { return _display; }
    Window x11_window::getWindow() { return _window; }

    // OVERIDES
    displayService x11_window::getDisplayService() const { return displayService::x11; }
    void x11_window::setBackgroundColor(color color){ _bgColor = color; }

    // Titlebar
    void x11_window::setTitleBarDecorations(const titlebarCreationInfo& info)
    { 
        int windowEventID = static_cast<int>(_window) * 2;

        // Remove decorations
        if(!info.visible)
        {
            if(_clientDecorations != nullptr)
            { 
                graphics::titleBar::removeDefaultDecorations(_clientDecorations, windowEventID, this); 
                
                Atom extentsAtom = XInternAtom(_display, "_GTK_FRAME_EXTENTS", False);
                XDeleteProperty(_display, _window, extentsAtom);
            }

            if(hasServerSideDecorations(_display, _window))
            {
                Atom motifHintsAtom = XInternAtom(_display, "_MOTIF_WM_HINTS", False);
                XDeleteProperty(_display, _window, motifHintsAtom);
            }

            return;
        }

        WM_Hints hints;
        Atom motifHintsAtom = XInternAtom(_display, "_MOTIF_WM_HINTS", False);
        hints.flags = (1L << 1);
        hints.functions = 0L;
        hints.input_mode = 0L;
        hints.status = 0L;

        // Server Side Decoration
        if(info.type == windowDecoration::ServerSide)
        {
            hints.decorations = 1L;
            XChangeProperty(_display, _window, motifHintsAtom, motifHintsAtom, 32, PropModeReplace, (unsigned char*)&hints, 5);
            return;
        }

        // Client Side Decorations
        hints.decorations = 0L;

        Atom extentsAtom = XInternAtom(_display, "_GTK_FRAME_EXTENTS", False);
        long extents[4] = { DEF_GSA_WINDOW_BORDER_PADDING, DEF_GSA_WINDOW_BORDER_PADDING, DEF_GSA_WINDOW_BORDER_PADDING, DEF_GSA_WINDOW_BORDER_PADDING };
        XChangeProperty(_display, _window, motifHintsAtom, motifHintsAtom, 32, PropModeReplace, (unsigned char*)&hints, 5);
        XChangeProperty(_display, _window, extentsAtom, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)extents, 4);

        event::dispatcher &deRefDispatcher = *info.evtDispatcher;
        graphics::titleBar::createDefaultDecorations(info.type, _clientDecorations, this, windowEventID, deRefDispatcher);

        // Header
        deRefDispatcher.add(windowEventID, [this](GooseUI::event::data evt){
            Atom netWmMoveResize = XInternAtom(_display, "_NET_WM_MOVERESIZE", False);

            XEvent xev = {};
            xev.type = ClientMessage;
            xev.xclient.window = _window;
            xev.xclient.message_type = netWmMoveResize;
            xev.xclient.format = 32;
            xev.xclient.data.l[0] = evt.mouseRootX;
            xev.xclient.data.l[1] = evt.mouseRootY;
            xev.xclient.data.l[2] = 8;
            xev.xclient.data.l[3] = 1;
            xev.xclient.data.l[4] = 1;

            XSendEvent(_display, DefaultRootWindow(_display), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
        });

        // Close Button
        deRefDispatcher.add(windowEventID + 1, [this](GooseUI::event::data evt){ close(); });
    }
    
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
    void x11_window::close() { _isRunning = false; }

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
                    
                    int x = event.xmotion.x + DEF_GSA_WINDOW_BORDER_PADDING;
                    int y = event.xmotion.y + DEF_GSA_WINDOW_BORDER_PADDING;

                    int resizeDirection = isCursorOnWindowEdge(x, y, getWidth(), getHeight());
                    if (resizeDirection != -1) 
                    {
                        Cursor cursor = getDirectionalCursor(_display, resizeDirection);
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
                        
                        int resizeDirection = isCursorOnWindowEdge(evtData.mouseX += DEF_GSA_WINDOW_BORDER_PADDING, evtData.mouseY += DEF_GSA_WINDOW_BORDER_PADDING, getWidth(), getHeight());
                        if(resizeDirection != -1){ startNativeResize(this, evtData.mouseRootX, evtData.mouseRootY, resizeDirection); handelWidgets = false; }
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