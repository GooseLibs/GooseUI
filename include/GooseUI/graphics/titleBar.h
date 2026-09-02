#ifndef _GOOSEUI_DECORATION_H_
#define _GOOSEUI_DECORATION_H_

#include "GooseUI/widgets/titleBar.h"
#include "GooseUI/widgets/boxButton.h"

#include "GooseUI/events/eventDispatcher.h"


namespace GooseUI
{
    namespace graphics
    {
        // Title bar Data, Default Decorations (Do to GNOME)
        struct titleBarData
        {
            event::dispatcher* evtDispatcher;
            
            widgets::titleBar* bar = nullptr;
            widgets::boxButton* closeButton = nullptr;

            titleBarData() = default;

            ~titleBarData()
            {
                delete closeButton;
                delete bar;
            }
        };
    }
}

#endif /*_GOOSEUI_DECORATION_H_*/