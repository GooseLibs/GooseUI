#ifndef _GOOSEUI_TITLEBAR_H_
#define _GOOSEUI_TITLEBAR_H_

#include "GooseUI/abstractions/iWidget.h"

#include "GooseUI/events/eventDispatcher.h"
#include "GooseUI/types.h"

namespace GooseUI
{
    namespace widgets
    {
        class titleBar : public absractions::iWidget
        {
            event::dispatcher& _evtDispatcher;
            color _color;

            int _eventID;

            public:
            titleBar(int eventID, event::dispatcher& evtDispatcher, widgetScaleing widgetScaleing, int widgetAlignment, int x, int y, int width, int height);
            ~titleBar() = default;

            void setColor(const color& color);

            // Overides
            void draw() override;
            void pollEvent(event::data evtData) override;

            void setSize(int width, int height) override;
            void setPosistion(int X, int Y) override;
            void setScaleRestraints(int minWidth, int minHeight, int maxWidth, int maxHeight) override;
            void setPosRestraints(int minX, int minY, int maxX, int maxY) override;
        };
    }
}

#endif /*_GOOSEUI_TITLEBAR_H_*/