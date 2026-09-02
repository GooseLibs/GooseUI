#ifndef _GOOSEUI_IWINDOW_H_
#define _GOOSEUI_IWINDOW_H_

#include "GooseUI/types.h"
#include "GooseUI/graphics/titleBar.h"

#include <string>
#include <vector>


namespace GooseUI
{
    struct windowCreationInfo
    {
        int width = 800;
        int height = 800;
        screenPosistion posistion = screenPosistion::SCREEN_CENTER;
    };

    struct titlebarCreationInfo
    {
        event::dispatcher* evtDispatcher;
        windowDecoration type;

        std::string windowTitle = "";
        bool visible = true;
        bool hasCloseButton = true;
        bool hasMinimizeButton = true;
        bool hasMaximizeButton = true;
    };
    
    namespace absractions
    {
        class iWindow
        {
            protected:
            std::vector<iWidget*> _widgets;

            graphics::titleBarData* _clientDecorations;
            
            color _bgColor;
            bool _isRunning;
            
            public:
            virtual ~iWindow() = default;

            virtual bool isRunning() const { return _isRunning; };
            virtual displayService getDisplayService() const = 0;

            virtual void setBackgroundColor(color color) = 0;
            
            // Window Configuration
            virtual void setTitleBarDecorations(const titlebarCreationInfo& info) = 0;
            virtual absractions::iWidget* getClientTitleBar() = 0;
            
            // Window Size
            virtual void setSize(int width, int height) = 0;
            virtual void isResizeable(bool isResizeable) = 0;
            
            virtual void maximize() = 0;
            virtual void minimize() = 0;
            virtual void restoreSize() = 0;

            virtual int getWidth() = 0;
            virtual int getHeight() = 0;
            
            // Window Visibility
            virtual void isAllwaysOnTop(bool isOnTop) = 0;
            
            virtual void show() = 0;
            virtual void hide() = 0;
            virtual void close() = 0;

            // Widget Management
            virtual void addWidgetToVector(iWidget* widget) = 0;
            virtual void removeWidgetFromVector(iWidget* widget) = 0;
            virtual void renderWidgets() = 0;
            virtual void handelEvents() = 0;
        };

        absractions::iWindow* createWindow(const windowCreationInfo& info);
    }
}

#endif /*_GOOSEUI_IWINDOW_H_*/