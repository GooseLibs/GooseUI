#ifndef _GOOSEUI_X11_STATIC_H_
#define _GOOSEUI_X11_STATIC_H_

#include "GooseUI/types.h"
#include "GooseUI/graphics/titleBar.h"
#include "GooseUI/abstractions/iWindow.h"

#include <X11/Xlib.h>
#include <string>

#define X11_BORDER_PADDING 3


namespace GooseUI 
{
    namespace platform
    {
        // Decorations
        void x11_CreateDecoration(windowDecoration type, graphics::titleBarData*& titleBar, absractions::iWindow* window, event::dispatcher& evtDispatcher);
        void x11_removeDecoration(graphics::titleBarData* titleBar, absractions::iWindow* window);
        void x11_ModifieDecoration(absractions::iWindow *window, graphics::titleBarData *&titleBar, const titlebarCreationInfo& titleBarInfo);

        // Resizeing
        Cursor x11_getCursor(Display* display, int direction);
        int x11_edgeHitTest(int x, int y, int win_width, int win_height);
        void x11_startNativeResize(absractions::iWindow* iWindow, int mouseRootX, int mouseRootY, int direction);
    }
}

#endif /*_GOOSEUI_X11_STATIC_H_*/