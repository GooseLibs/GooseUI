#include "GooseUI/widgets/boxButton.h"

#include "GooseUI/context.h"


namespace GooseUI::widgets // Public
{
    boxButton::boxButton(const boxButtonCreationInfo& info)
        : _eventID(info.eventID), _evtDispatcher(*info.evtDispatcher)
        {
            _widgetScaleing = info.scaleing;
            _alignment = info.alignment;
            _posX = info.X;
            _posY = info.Y;
            _width = info.width;
            _height = info.height;
    
            _isVisible = true;
            _isPressed = false;
            _outlineSize = 1; 
            _color = { 0.85f, 0.85f, 0.85f, 1.0f };
        }

    boxButton::~boxButton()
    {
        _evtDispatcher.remove(_eventID);
    }

    boxButton* createBoxButton(const boxButtonCreationInfo& info){ return new boxButton(info); }

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