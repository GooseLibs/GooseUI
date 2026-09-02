#include "GooseUI/platform/x11_decorations.h"
#include "GooseUI/platform/x11_window.h"

#include <X11/Xatom.h>
#include <X11/Xcursor/Xcursor.h>

#define X11_DEFAULT_CLIENT_DECORATION_HEIGHT 25


namespace GooseUI::platform // Private
{
    struct WM_Hints { unsigned long flags; unsigned long functions; unsigned long decorations; long input_mode; unsigned long status; };
    
    bool _hasDecorations(Display* display, Window window)
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
}

namespace GooseUI::platform // Public
{
    void x11_CreateDecoration(windowDecoration type, graphics::titleBarData *&titleBar, absractions::iWindow *window, event::dispatcher &evtDispatcher)
    {
        platform::x11_window* xWindow = static_cast<platform::x11_window*>(window);
        //x11_removeDecoration(titleBar, window);

        WM_Hints hints;
        Atom motifHintsAtom = XInternAtom(xWindow->getDisplay(), "_MOTIF_WM_HINTS", False);
        hints.flags = (1L << 1);
        hints.functions = 0L;
        hints.input_mode = 0L;
        hints.status = 0L;

        // Server Side Decoration
        if(type == windowDecoration::ServerSide)
        {
            hints.decorations = 1L;
            XChangeProperty(xWindow->getDisplay(), xWindow->getWindow(), motifHintsAtom, motifHintsAtom, 32, PropModeReplace, (unsigned char*)&hints, 5);
            return;
        }

        // Client Side DecorSetup
        hints.decorations = 0L;

        Atom extentsAtom = XInternAtom(xWindow->getDisplay(), "_GTK_FRAME_EXTENTS", False);
        long extents[4] = { X11_BORDER_PADDING, X11_BORDER_PADDING, X11_BORDER_PADDING, X11_BORDER_PADDING };
        XChangeProperty(xWindow->getDisplay(), xWindow->getWindow(), motifHintsAtom, motifHintsAtom, 32, PropModeReplace, (unsigned char*)&hints, 5);
        XChangeProperty(xWindow->getDisplay(), xWindow->getWindow(), extentsAtom, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)extents, 4);
        
        // Client Side DeocrUI
        titleBar = new graphics::titleBarData();
        titleBar->evtDispatcher = &evtDispatcher;

        int windowEventID = static_cast<int>(xWindow->getWindow()) * 2;
        
        titleBar->bar = new widgets::titleBar(windowEventID, evtDispatcher, SCALE_HORIZONTAL, ALIGN_LEFT | ALIGN_RIGHT | ALIGN_TOP, 0, 0, xWindow->getWidth(), X11_DEFAULT_CLIENT_DECORATION_HEIGHT);
        evtDispatcher.add(windowEventID, [xWindow](GooseUI::event::data evt){ 
            Atom netWmMoveResize = XInternAtom(xWindow->getDisplay(), "_NET_WM_MOVERESIZE", False);

            XEvent xev = {};
            xev.type = ClientMessage;
            xev.xclient.window = xWindow->getWindow();
            xev.xclient.message_type = netWmMoveResize;
            xev.xclient.format = 32;
            xev.xclient.data.l[0] = evt.mouseRootX;
            xev.xclient.data.l[1] = evt.mouseRootY;
            xev.xclient.data.l[2] = 8;
            xev.xclient.data.l[3] = 1;
            xev.xclient.data.l[4] = 1;

            XSendEvent(xWindow->getDisplay(), DefaultRootWindow(xWindow->getDisplay()), False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
        });
        titleBar->bar->addToWindow(window);

        // Close Button
        {
            boxButtonCreationInfo info {};
            info.eventID = windowEventID + 1;
            info.evtDispatcher = &evtDispatcher;
            info.scaleing = SCALE_NONE; 
            info.alignment = ALIGN_RIGHT | ALIGN_TOP | ALIGN_BOTTOM;
            info.X = titleBar->bar->getWidth() - X11_DEFAULT_CLIENT_DECORATION_HEIGHT;
            info.Y = titleBar->bar->getHeight() - X11_DEFAULT_CLIENT_DECORATION_HEIGHT + 2;
            info.width = X11_DEFAULT_CLIENT_DECORATION_HEIGHT - 6;
            info.height = X11_DEFAULT_CLIENT_DECORATION_HEIGHT - 6;
            titleBar->closeButton = widgets::createBoxButton(info);
            
            titleBar->closeButton->setColor({ 0.91f, 0.12f, 0.15f, 1.0f });
            evtDispatcher.add(windowEventID + 1, [xWindow](GooseUI::event::data evt){ xWindow->close(); });
            titleBar->closeButton->setParent(titleBar->bar);
        }
    }

    void x11_removeDecoration(graphics::titleBarData *titleBar, absractions::iWindow *window)
    {
        platform::x11_window* xWindow = static_cast<platform::x11_window*>(window);
        if(titleBar != nullptr)
        {
            int windowEventID = static_cast<int>(xWindow->getWindow()) * 2;
            if(titleBar->closeButton != nullptr)
            { 
                titleBar->closeButton->removeParent(); 
                titleBar->evtDispatcher->remove(windowEventID + 1);
            }

            if(titleBar->bar != nullptr)
            {
                titleBar->bar->removeFromWindow();
                titleBar->evtDispatcher->remove(windowEventID);
            }

            delete(titleBar);
            titleBar = nullptr;

            Atom extentsAtom = XInternAtom(xWindow->getDisplay(), "_GTK_FRAME_EXTENTS", False);
            XDeleteProperty(xWindow->getDisplay(), xWindow->getWindow(), extentsAtom);
        }

        Atom motifHintsAtom = XInternAtom(xWindow->getDisplay(), "_MOTIF_WM_HINTS", False);
        XDeleteProperty(xWindow->getDisplay(), xWindow->getWindow(), motifHintsAtom);
    }

    void x11_ModifieDecoration(absractions::iWindow *window, graphics::titleBarData *&titleBar, const titlebarCreationInfo& titleBarInfo)
    {
        platform::x11_window* xWindow = static_cast<platform::x11_window*>(window);
        
        if(!titleBarInfo.visible)
        {
            if(_hasDecorations(xWindow->getDisplay(), xWindow->getWindow())){ x11_removeDecoration(titleBar, window); }
            return;
        }

        if(!_hasDecorations(xWindow->getDisplay(), xWindow->getWindow())){ x11_CreateDecoration(titleBarInfo.type, titleBar, window, *titleBarInfo.evtDispatcher); }
    }

    // Resizeing
    int x11_edgeHitTest(int x, int y, int win_width, int win_height)
    {
        bool top    = y <= X11_BORDER_PADDING + (X11_BORDER_PADDING / 2);
        bool bottom = y >= (win_height - X11_BORDER_PADDING);
        bool left   = x <= X11_BORDER_PADDING + X11_BORDER_PADDING;
        bool right  = x >= (win_width - X11_BORDER_PADDING);
    
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

    Cursor x11_getCursor(Display* display, int direction)
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

    void x11_startNativeResize(absractions::iWindow *iwindow, int mouseRootX, int mouseRootY, int direction)
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