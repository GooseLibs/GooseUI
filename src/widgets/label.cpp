#include "GooseUI/widgets/label.h"

#include "GooseUI/graphics/text.h"
#include "GooseUI/graphics/layout.h"

#include "GooseUI/font/fontManager.h"


namespace GooseUI::widgets // Public
{
    label::label(const labelCreationInfo& info)
    {
        _widgetScaleing = info.scaleing;
        _alignment = info.alignment;
        _posX = info.X;
        _posY = info.Y;
        _width = info.width;
        _height = info.height;

        _isVisible = true;
        _color = { 0.0f, 0.0f, 0.0f, 1.0f };
    }

    label* createLable(const labelCreationInfo& info){ return new label(info); }

    // Widget Specific
    void label::setFont(const std::string &fontPath, font::fontData fontData){ _font = font::manager::instance().getFont(fontPath, fontData); }
    void label::setColor(const color &color){ _color = color; }
    void label::setText(const std::string &text){ _label = text; }

    // Overides
    void label::draw()
    {
        _preDraw();

        if(_label != "" && _font != nullptr)
        {
            int textWidth, textHeight;
            float textScale;
                        
            graphics::text::getSize(_font, _label, 1, textWidth, textHeight);
            graphics::layout::calculateBitmapTextLayout(_width, _height, textScale, textWidth, textHeight);

            graphics::text::draw(_font, _label, _posX + (_width - textWidth) / 2, _posY + (_height + textHeight) / 2, textScale, _color);
        }
    }

    void label::pollEvent(event::data evtData){ return; }
}