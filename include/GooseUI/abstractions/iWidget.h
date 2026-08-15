#ifndef _GOOSEUI_IWIDGET_H_
#define _GOOSEUI_IWIDGET_H_

#include "GooseUI/types.h"
#include "GooseUI/events/event.h"

namespace GooseUI
{
    namespace absractions
    {
        class iWidget
        {
            protected:
            iWindow* _hostWindow = nullptr;
            iWidget* _hostParent = nullptr;
            
            layoutRestraints _layoutRestraints;
            widgetScaleing _widgetScaleing;
            
            int _initalBounds[4];
            int _alignment;
            
            int _posX, _posY;
            int _width, _height;
            
            void _preDraw();
            
            bool _isVisible = false;
            
            public:
            virtual ~iWidget() = default;
            
            // Core
            virtual void draw() = 0;
            virtual void pollEvent(event::data evtData) = 0;
            
            // Pre-Defiend Fixed Functions - All widgets need these and my hand hurts :<
            void addToWindow(iWindow* window);
            void removeFromWindow();
            void setParent(iWidget* widget);
            void removeParent();

            // Optinal Overides
            virtual void show() { _isVisible = true; }
            virtual void hide() { _isVisible = false; }
            
            virtual void setSize(int width, int height) { _width = width, _height = height; };
            virtual void setPosistion(int X, int Y) { _posX = X, _posY = Y; };
            virtual void setScaleRestraints(int minWidth, int minHeight, int maxWidth, int maxHeight) { _layoutRestraints.minWidth = minWidth; _layoutRestraints.minHeight = minHeight; _layoutRestraints.maxWidth = maxWidth; _layoutRestraints.maxHeight = maxHeight; };
            virtual void setPosRestraints(int minX, int minY, int maxX, int maxY) { _layoutRestraints.minX = minX; _layoutRestraints.minY = minY; _layoutRestraints.maxX = maxX; _layoutRestraints.maxY = maxY; };
            
            // Returns
            iWindow* getWindow() const { return _hostWindow; }
            iWidget* getParent() const { return _hostParent; }
            
            bool isVisible() const { return _isVisible; }
            
            int getPosX() const { return _posX; };
            int getPosY() const { return _posY; }
            int getWidth() const { return _width; }
            int getHeight() const { return _height; }
        };
    }
}

#endif /*_GOOSEUI_IWIDGET_H_*/