#include "GooseUI/platform/win32_decorations.h"
#include "GooseUI/platform/win32_window.h"

#define WIN32_DEFAULT_CLIENT_DECORATION_HEIGHT 25


namespace GooseUI::platform // Local
{
    void _CreateDecoration(windowDecoration type, graphics::titleBarData*& titleBar, absractions::iWindow* window, event::dispatcher& evtDispatcher)
    {
        platform::win32_window* wWindow = static_cast<platform::win32_window*>(window);

        // Server Side Decoration
        LONG_PTR style = GetWindowLongPtr(wWindow->getHwnd(), GWL_STYLE);
        if(type == windowDecoration::ServerSide)
        {
            style |= (WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
            SetWindowLongPtr(wWindow->getHwnd(), GWL_STYLE, style);
            SetWindowPos(wWindow->getHwnd(), nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

            return;
        }

        // Client Side DecorSetup
        style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
        style |= WS_POPUP;

        SetWindowLongPtr(wWindow->getHwnd(), GWL_STYLE, style);
        SetWindowPos(wWindow->getHwnd(), nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

        // Client Side DecorUI
        titleBar = new graphics::titleBarData();
        titleBar->evtDispatcher = &evtDispatcher;
        uintptr_t windowEventID = reinterpret_cast<uintptr_t>(wWindow->getHwnd()) * 2;

        titleBar->bar = new widgets::titleBar(windowEventID, evtDispatcher, SCALE_HORIZONTAL, ALIGN_LEFT | ALIGN_RIGHT | ALIGN_TOP, 0, 0, wWindow->getWidth(), WIN32_DEFAULT_CLIENT_DECORATION_HEIGHT);
        evtDispatcher.add(windowEventID, [wWindow](GooseUI::event::data evt){ 
            ::ReleaseCapture();
            ::SendMessage(wWindow->getHwnd(), WM_NCLBUTTONDOWN, HTCAPTION, 0);
        });
        titleBar->bar->addToWindow(window);

        // Close Button
        {
            boxButtonCreationInfo info {};
            info.eventID = windowEventID + 1;
            info.evtDispatcher = &evtDispatcher;
            info.scaleing = SCALE_NONE; 
            info.alignment = ALIGN_RIGHT | ALIGN_TOP | ALIGN_BOTTOM;
            info.X = titleBar->bar->getWidth() - WIN32_DEFAULT_CLIENT_DECORATION_HEIGHT;
            info.Y = titleBar->bar->getHeight() - WIN32_DEFAULT_CLIENT_DECORATION_HEIGHT + 2;
            info.width = WIN32_DEFAULT_CLIENT_DECORATION_HEIGHT - 6;
            info.height = WIN32_DEFAULT_CLIENT_DECORATION_HEIGHT - 6;
            titleBar->closeButton = widgets::createBoxButton(info);
            
            titleBar->closeButton->setColor({ 0.91f, 0.12f, 0.15f, 1.0f });
            evtDispatcher.add(windowEventID + 1, [wWindow](GooseUI::event::data evt){ wWindow->destroy(); });
            titleBar->closeButton->setParent(titleBar->bar);
        }
    }
    
    void _removeDecoration(graphics::titleBarData* titleBar, absractions::iWindow* window)
    {
        platform::win32_window* wWindow = static_cast<platform::win32_window*>(window);
        if(titleBar != nullptr)
        {
            uintptr_t windowEventID = reinterpret_cast<uintptr_t>(wWindow->getHwnd()) * 2;
            if(titleBar->closeButton != nullptr)
            { 
                titleBar->closeButton->removeParent(); 
                titleBar->evtDispatcher->remove(windowEventID + 1);
            }

            if(titleBar->bar != nullptr)
            {
                titleBar->bar->removeFromWindow();
                titleBar->evtDispatcher->remove(windowEventID);
            }

            delete(titleBar);
            titleBar = nullptr;

            SetWindowPos(wWindow->getHwnd(), NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        }

        LONG_PTR style = GetWindowLongPtr(wWindow->getHwnd(), GWL_STYLE);
        style |= (WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
        SetWindowLongPtr(wWindow->getHwnd(), GWL_STYLE, style);

        SetWindowPos(wWindow->getHwnd(), NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
}

namespace GooseUI::platform // Public
{
    void win32_ModifieDecoration(absractions::iWindow *window, graphics::titleBarData *&titleBar, const titlebarCreationInfo &titleBarInfo)
    {
        platform::win32_window* wWindow = static_cast<platform::win32_window*>(window);

        LONG_PTR style = GetWindowLongPtr(wWindow->getHwnd(), GWL_STYLE);
        DWORD flags = WS_CAPTION | WS_THICKFRAME | WS_DLGFRAME;
        
        if(!titleBarInfo.visible)
        {
            if((style & flags) != 0){ _removeDecoration(titleBar, window); }
            return;
        }

        if((style & flags) != 0){ _CreateDecoration(titleBarInfo.type, titleBar, window, *titleBarInfo.evtDispatcher); }
    }

    int win32_edgeHitTest(int x, int y, int win_width, int win_height)
    {
        bool top    = y <= WIN32_BORDER_PADDING + (WIN32_BORDER_PADDING / 2);
        bool bottom = y >= (win_height - WIN32_BORDER_PADDING);
        bool left   = x <= WIN32_BORDER_PADDING + WIN32_BORDER_PADDING;
        bool right  = x >= (win_width - WIN32_BORDER_PADDING);

        if (top && left)     return HTTOPLEFT; // Top-Left
        if (top && right)    return HTTOPRIGHT; // Top-Right
        if (bottom && right) return HTBOTTOMRIGHT; // Bottom-Right
        if (bottom && left)  return HTBOTTOMLEFT; // Bottom-Left
        if (top)             return HTTOP; // Top
        if (right)           return HTRIGHT; // Right
        if (bottom)          return HTBOTTOM; // Bottom
        if (left)            return HTLEFT; // Left

        return HTCLIENT;
    }
}