#ifndef _GOOSEUI_WL_DECORATIONS_H_
#define _GOOSEUI_WL_DECORATIONS_H_

#include "GooseUI/types.h"
#include "GooseUI/graphics/titleBar.h"
#include "GooseUI/abstractions/iWindow.h"

#include <string>

namespace GooseUI 
{
    namespace platform
    {
        // Decorations
        void wl_ModifieDecoration(absractions::iWindow *window, graphics::titleBarData *&titleBar, const titlebarCreationInfo& titleBarInfo);
    }
}

#endif /*_GOOSEUI_WL_DECORATIONS_H_*/