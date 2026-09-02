#include "GooseUI/widgets/panel.h"

#include "GooseUI/context.h"


namespace GooseUI::widgets // Public
{
    panel::panel(const panelCreationInfo& info)
    {
        _widgetScaleing = info.scaleing;
        _alignment = info.alignment;
        _posX = info.X;
        _posY = info.Y;
        _width = info.width;
        _height = info.height;

        _isVisible = true;
        _outlineSize = 0;
        _color = { 0.85f, 0.85f, 0.85f, 1.0f };
        _outlineColor = { 0.0f, 0.0f, 0.0f, 0.0f };
    }

    panel* createPanel(const panelCreationInfo& info){ return new panel(info); }

    // Widget Specific
    void panel::setColor(const color& color){ _color = color; }
    void panel::setOutlineColor(const color& color){ _outlineColor = color; }
    void panel::setOutlineSize(int size){ _outlineSize = size; }

    void panel::draw()
    {
        _preDraw();

        if(_outlineSize > 0){ application::getRenderer()->drawRect(_posX - _outlineSize, _posY - _outlineSize, _width + 2 * _outlineSize, _height + 2 * _outlineSize, _outlineColor); }
        application::getRenderer()->drawRect(_posX, _posY, _width, _height, _color);
    }

    void panel::pollEvent(event::data evtData){ return; }
}