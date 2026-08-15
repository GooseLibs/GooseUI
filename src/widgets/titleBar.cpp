#include "GooseUI/widgets/titleBar.h"

#include "GooseUI/context.h"


namespace GooseUI::widgets
{
    titleBar::titleBar(int eventID, event::dispatcher& evtDispatcher, widgetScaleing widgetScaleing, int widgetAlignment, int x, int y, int width, int height)
    : _eventID(eventID), _evtDispatcher(evtDispatcher)
    {
        _widgetScaleing = widgetScaleing;
        _alignment = widgetAlignment;
        _posX = x;
        _posY = y;
        _width = width;
        _height = height;

        _isVisible = true;
        _color = { 0.85f, 0.85f, 0.85f, 1.0f };
    }

    // Widget Specific
    void titleBar::setColor(const color& color){ _color = color; }

    // Overides
    void titleBar::draw(){ _preDraw(); application::getRenderer()->drawRect(_posX, _posY, _width, _height, _color); }
    void titleBar::pollEvent(event::data evtData)
    { 
        if(evtData.dataType == event::type::leftMouseDown && evtData.mouseX >= _posX && evtData.mouseX <= _posX + _width && evtData.mouseY >= _posY && evtData.mouseY <= _posY + _height)
            { _evtDispatcher.dispatch(_eventID, evtData); }
    }
    
    void titleBar::setSize(int width, int height){ _height = height; _posY = _posY - height; }
    void titleBar::setPosistion(int X, int Y){ return; }
    void titleBar::setScaleRestraints(int minWidth, int minHeight, int maxWidth, int maxHeight){ return; }
    void titleBar::setPosRestraints(int minX, int minY, int maxX, int maxY){ return; }
}