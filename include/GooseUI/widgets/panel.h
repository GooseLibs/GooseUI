#ifndef _GOOSEUI_PANEL_H_
#define _GOOSEUI_PANEL_H_

#include "GooseUI/abstractions/iWidget.h"

#include "GooseUI/types.h"

namespace GooseUI
{
    struct panelCreationInfo
    {
        widgetAlignment alignment;
        int X = 0;
        int Y = 0;

        widgetScaleing scaleing;
        int width;
        int height;
    };
    
    namespace widgets
    {
        class panel : public absractions::iWidget
        {
            color _color;
            color _outlineColor;

            int _outlineSize;

            public:
            panel(const panelCreationInfo& info);
            ~panel() = default;

            void setColor(const color& color);
            void setOutlineColor(const color& color);
            void setOutlineSize(int size);

            // Overides
            void draw() override;
            void pollEvent(event::data evtData) override;
        };

        panel* createPanel(const panelCreationInfo& info);
    }
}

#endif /*_GOOSEUI_PANEL_H_*/