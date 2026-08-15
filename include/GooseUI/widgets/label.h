#ifndef _GOOSEUI_LABEL_H_
#define _GOOSEUI_LABEL_H_

#include "GooseUI/abstractions/iWidget.h"
#include "GooseUI/abstractions/iFont.h"

#include "GooseUI/types.h"

namespace GooseUI
{
    namespace widgets
    {
        struct labelCreationInfo
        {
            widgetAlignment alignment;
            int X = 0;
            int Y = 0;
    
            widgetScaleing scaleing;
            int width;
            int height;
        };
        
        class label : public absractions::iWidget
        {
            color _color;
            absractions::iFont* _font;
            std::string _label;

            public:
            label(const labelCreationInfo& info);
            ~label() = default;

            void setFont(const std::string &fontPath, font::fontData fontData);
            void setColor(const color &color);
            void setText(const std::string &text);

            // Overides
            void draw() override;
            void pollEvent(event::data evtData) override;
        };

        label* createLable(const labelCreationInfo& info);
    }
}

#endif /*_GOOSEUI_LABEL_H_*/