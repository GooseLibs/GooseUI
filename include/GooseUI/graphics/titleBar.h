#ifndef _GOOSEUI_DECORATION_H_
#define _GOOSEUI_DECORATION_H_

#include "GooseUI/widgets/panel.h"
#include "GooseUI/widgets/boxButton.h"

#include "GooseUI/events/eventDispatcher.h"


namespace GooseUI
{
    namespace graphics
    {
        struct titleBar
        {
            event::dispatcher* evtDispatcher = nullptr;
            widgets::panel* bar = nullptr;

            // Default Decorations (Because GNOME has to be sooo special)
            widgets::boxButton* closeButton = nullptr;
            widgets::boxButton* minimizeButton = nullptr;
            widgets::boxButton* maximizeButton = nullptr;

            ~titleBar()
            {
                delete closeButton;
                delete minimizeButton;
                delete maximizeButton;
                delete bar;
            }
        };
    }
}

#endif /*_GOOSEUI_DECORATION_H_*/