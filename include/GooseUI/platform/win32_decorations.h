#ifndef _GOOSEUI_WIN32_STATIC_H_
#define _GOOSEUI_WIN32_STATIC_H_

#include "GooseUI/types.h"
#include "GooseUI/graphics/titleBar.h"
#include "GooseUI/abstractions/iWindow.h"

#define WIN32_BORDER_PADDING 3


namespace GooseUI 
{
    namespace platform
    {
        // Decorations
        void win32_ModifieDecoration(absractions::iWindow *window, graphics::titleBarData *&titleBar, const titlebarCreationInfo& titleBarInfo);

        // Resizeing
        int win32_edgeHitTest(int x, int y, int win_width, int win_height);
    }
}

#endif /*_GOOSEUI_WIN32_STATIC_H_*/