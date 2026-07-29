#ifndef _GOOSEUI_BOXBUTTON_H_
#define _GOOSEUI_BOXBUTTON_H_

#include "GooseUI/abstractions/iWidget.h"
#include "GooseUI/events/eventDispatcher.h"

#include "GooseUI/types.h"

namespace GooseUI
{
    namespace widgets
    {
        class boxButton : public absractions::iWidget
        {
            event::dispatcher& _evtDispatcher;
            color _color;

            int _eventID;
            int _outlineSize;
                            
            bool _isPressed;

            public:
            boxButton(int eventID, event::dispatcher& evtDispatcher, widgetScaleing widgetScaleing, int widgetAlignment, int x, int y, int width, int height);
            ~boxButton();

            void setOutlineSize(int size);
            void setColor(const color& color);

            // Overides
            void draw() override;
            void pollEvent(event::data evtData) override;
        };

        boxButton* createBoxButton(
            int evtID,
            event::dispatcher& evtDispatcher,
            widgetScaleing widgetScaleing, 
            int widgetAlignment, 
            int x, int y, 
            int width, int height
        );
    }
}

#endif /*_GOOSEUI_BOXBUTTON_H_*/