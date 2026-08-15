#ifndef _GOOSEUI_X11_WINDOW_H_
#define _GOOSEUI_X11_WINDOW_H_

#include "GooseUI/types.h"
#include "GooseUI/context.h"

#include "GooseUI/graphics/titleBar.h"

#include "GooseUI/abstractions/iWindow.h"
#include "GooseUI/abstractions/iRenderer.h"
#include "GooseUI/abstractions/iWidget.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

/*
    This is what I use for testing, I have no clue how it acts on other systems:
    WM: LabWC
    Server: xWayland
*/


namespace GooseUI 
{
    namespace platform
    {
        class x11_window : public absractions::iWindow
        {
            Atom _wm_delete_window;

            Display* _display = nullptr;
            ::Window _window;
    
            void* _windowCtx;
            
            void _gl_createContext();
            void _gl_destoryContext();
    
            void _vk_createContext();
            void _vk_destoryContext();
                
            void _startRenderFrame();
            void _endRenderFrame();
    
            public:
            x11_window(const windowCreationInfo& info);
            virtual ~x11_window();

            Display* getDisplay();
            ::Window getWindow();

            // OVERIDES
            displayService getDisplayService() const override;
            void setBackgroundColor(color color) override;

            // Window Titlebar
            void setTitleBarDecorations(const titlebarCreationInfo& info) override;
            absractions::iWidget* getClientTitleBar() override;

            // Window Size
            void setSize(int width, int height) override;
            void isResizeable(bool isResizeable) override;
                
            void maximize() override;
            void minimize() override;
            void restoreSize() override;

            int getWidth() override;
            int getHeight() override;

            // Window Visibility
            void isAllwaysOnTop(bool isOnTop) override;
                
            void show() override;
            void hide() override;
            void destroy() override;
    
            // Widget Management
            void addWidgetToVector(absractions::iWidget* widget) override;
            void removeWidgetFromVector(absractions::iWidget* widget) override;
            void renderWidgets() override;
            void handelEvents() override;
        };
    }
}

#endif /*_GOOSEUI_X11_WINDOW_H_*/