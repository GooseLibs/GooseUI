#ifndef _GOOSEUI_DECORATION_H_
#define _GOOSEUI_DECORATION_H_

#include "GooseUI/widgets/titleBar.h"
#include "GooseUI/widgets/boxButton.h"

#include "GooseUI/events/eventDispatcher.h"

#define DEF_GSA_WINDOW_BORDER_PADDING 3
#define DEF_GSA_WINDOW_TITLEBAR_HEIGHT 25

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

        class titleBar
        {
            public:
            static void createDefaultDecorations(windowDecoration type, titleBarData *&titleBar, absractions::iWindow *window, int windowEventID, event::dispatcher &evtDispatcher);
            static void removeDefaultDecorations(titleBarData *titleBar, int windowEventID, absractions::iWindow *window);
            static void modifieDefaultDecorations();
        };
    }
}

#endif /*_GOOSEUI_DECORATION_H_*/