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
        void win32_CreateDecoration(windowDecoration type, graphics::titleBarData*& titleBar, absractions::iWindow* window, event::dispatcher& evtDispatcher);
        void win32_removeDecoration(graphics::titleBarData* titleBar, absractions::iWindow* window);
        void win32_ModifieDecoration(absractions::iWindow *window, graphics::titleBarData *&titleBar, const titlebarCreationInfo& titleBarInfo);
    }
}

#endif /*_GOOSEUI_WIN32_STATIC_H_*/