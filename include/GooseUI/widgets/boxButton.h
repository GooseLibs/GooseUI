#ifndef _GOOSEUI_BOXBUTTON_H_
#define _GOOSEUI_BOXBUTTON_H_

#include "GooseUI/abstractions/iWidget.h"
#include "GooseUI/events/eventDispatcher.h"

#include "GooseUI/types.h"

namespace GooseUI
{
    struct boxButtonCreationInfo
    {
        int eventID;
        event::dispatcher* evtDispatcher;

        int alignment;
        int X = 0;
        int Y = 0;

        widgetScaleing scaleing;
        int width;
        int height;
    };
    
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
            boxButton(const boxButtonCreationInfo& info);
            ~boxButton();

            void setOutlineSize(int size);
            void setColor(const color& color);

            // Overides
            void draw() override;
            void pollEvent(event::data evtData) override;
        };

        boxButton* createBoxButton(const boxButtonCreationInfo& info);
    }
}

#endif /*_GOOSEUI_BOXBUTTON_H_*/