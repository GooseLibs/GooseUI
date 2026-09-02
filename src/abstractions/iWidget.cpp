#include "GooseUI/abstractions/iWidget.h"
#include "GooseUI/abstractions/iWindow.h"

#include "GooseUI/graphics/layout.h"


namespace GooseUI::absractions // Abstraction Default
{
    void iWidget::_preDraw()
    {
        if(_hostParent){ _isVisible = _hostParent->isVisible(); }
        if(!_isVisible || !_hostWindow){ return; }
        
        if(_hostParent)
        {
            graphics::layout::calculateLayout(
                true,
                _widgetScaleing,
                _alignment,
                _layoutRestraints,
                _initalBounds,
                _hostParent->getWidth(), _hostParent->getHeight(),
                _hostParent->getPosX(), _hostParent->getPosY(),
                _posX, _posY,
                _width, _height
            );
        }else 
        {
            int yOffset = 0;
            if (_hostWindow->getClientTitleBar() != nullptr && this != _hostWindow->getClientTitleBar()) 
                { yOffset = _hostWindow->getClientTitleBar()->getHeight(); }
            
            graphics::layout::calculateLayout(
                false,
                _widgetScaleing,
                _alignment,
                _layoutRestraints,
                _initalBounds,
                _hostWindow->getWidth(), _hostWindow->getHeight(),
                0, 0,
                _posX, _posY,
                _width, _height
            );

            if(_hostWindow->getClientTitleBar() != nullptr && this != _hostWindow->getClientTitleBar())
                { if(_posY < yOffset){ _posY = yOffset; } }
        }
    }
    
    // Parent = Window
    void iWidget::addToWindow(iWindow* window)
    {
        if(!window || _hostParent){ return; }
        
        _hostWindow = window;
        _hostWindow->addWidgetToVector(this);

        int yOffset = 0;
        if (_hostWindow->getClientTitleBar() != nullptr && this != _hostWindow->getClientTitleBar()) 
            { yOffset = _hostWindow->getClientTitleBar()->getHeight(); }
        
        graphics::layout::getInitalOffsets(
            _initalBounds, 
            _hostWindow->getWidth(), 
            _hostWindow->getHeight(),
            _posX, _posY, 
            _width, _height
        );
    }
    
    void iWidget::removeFromWindow()
    {
        if(!_hostWindow || _hostParent){ return; }
        
        _hostWindow->removeWidgetFromVector(this);
        _hostWindow = nullptr;
    }
    
    // Parent = Widget
    void iWidget::setParent(iWidget* widget)
    {
        if(!widget || _hostWindow){ return; }
        
        _hostParent = widget; _hostWindow = _hostParent->getWindow();
        _hostWindow->addWidgetToVector(this);
        
        _layoutRestraints.minX = _hostParent->getPosX();
        _layoutRestraints.minY = _hostParent->getPosY();
        
        graphics::layout::getInitalOffsets(
            _initalBounds, 
            _hostParent->getWidth(), 
            _hostParent->getHeight(), 
            _posX, 
            _posY, 
            _width, _height
        );
    }
    
    void iWidget::removeParent()
    {
        if(!_hostParent){ return; }
        
        _hostWindow->removeWidgetFromVector(this);
        
        _hostWindow = nullptr; 
        _hostParent = nullptr;
        
        _layoutRestraints.minX = 0;
        _layoutRestraints.minY = 0;
    }
}