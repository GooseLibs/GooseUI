#ifndef _GOOSEUI_WIN32_WINDOW_H_
#define _GOOSEUI_WIN32_WINDOW_H_

#include "GooseUI/types.h"
#include "GooseUI/context.h"

#include "GooseUI/graphics/titleBar.h"

#include "GooseUI/abstractions/iWindow.h"
#include "GooseUI/abstractions/iRenderer.h"
#include "GooseUI/abstractions/iWidget.h"

#include <windows.h>
#include <cstdio>

namespace GooseUI 
{
    namespace platform
    {
        class win32_window : public absractions::iWindow
        {
            HWND _hwnd;
            HINSTANCE _hInstance;

            void _gl_createContext();
            void _gl_shareContext();
            void _gl_destoryContext();
    
            void _vk_createContext();
            void _vk_destoryContext();
                
            void _startRenderFrame();
            void _endRenderFrame();

            protected:
            static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

            public:
            win32_window(const windowCreationInfo& info);
            virtual ~win32_window();

            HWND getHwnd();

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
            void close() override;
    
            // Widget Management
            void addWidgetToVector(absractions::iWidget* widget) override;
            void removeWidgetFromVector(absractions::iWidget* widget) override;
            void renderWidgets() override;
            void handelEvents() override;
        };
    }
}

#endif /*_GOOSEUI_WIN32_WINDOW_H_*/