#include "GooseUI/widgets/boxButton.h"

#include "GooseUI/context.h"


namespace GooseUI::widgets
{
    boxButton::boxButton(int eventID, event::dispatcher& evtDispatcher, widgetScaleing widgetScaleing, int widgetAlignment, int x, int y, int width, int height)
        : _eventID(eventID), _evtDispatcher(evtDispatcher)
        {
            _widgetScaleing = widgetScaleing;
            _alignment = widgetAlignment;
            _posX = x;
            _posY = y;
            _width = width;
            _height = height;
    
            _isVisible = true;
            _isPressed = false;
            _outlineSize = 1; 
            _color = { 0.85f, 0.85f, 0.85f, 1.0f };
        }

    boxButton::~boxButton()
    {
        _evtDispatcher.remove(_eventID);
    }

    boxButton* createBoxButton(int eventID, event::dispatcher& evtDispatcher, widgetScaleing widgetScaleing, int widgetAlignment, int x, int y, int width, int height)
        { return new boxButton(eventID, evtDispatcher, widgetScaleing, widgetAlignment, x, y, width, height); }

    // Widget Specific
    void boxButton::setOutlineSize(int size) { _outlineSize = size; }
    void boxButton::setColor(const color& color){ _color = color;}

    void boxButton::draw()
    {
        _preDraw();
        
        if(!_isPressed) { application::getRenderer()->drawRect(_posX - _outlineSize, _posY - _outlineSize, _width + 2 * _outlineSize, _height + 2 * _outlineSize, { 0.0f, 0.0f, 0.0f, 1.0f }); }
        application::getRenderer()->drawRect(_posX, _posY, _width, _height, _color);
    }

    void boxButton::pollEvent(event::data evtData)
    { 
        if(evtData.dataType == event::type::leftMouseDown && evtData.mouseX >= _posX && evtData.mouseX <= _posX + _width && evtData.mouseY >= _posY && evtData.mouseY <= _posY + _height)
                    { _evtDispatcher.dispatch(_eventID, evtData); _isPressed = true; }
        
        if(evtData.dataType == event::type::leftMouseUp && _isPressed == true) { _isPressed = false; }
    }
}