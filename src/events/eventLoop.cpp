#include "GooseUI/events/eventLoop.h"
#include "GooseUI/abstractions/iWindow.h"

#if defined(_WIN32)

    #include <windows.h>

#elif defined(__APPLE__)

    #error Unsuported

#else

    #if GOOSEUI_XORG_SUPPORT
        #include <GooseUI/platform/x11_window.h>
    #endif

    #if GOOSEUI_WAYLAND_SUPPORT
    
    #endif

#endif


namespace GooseUI::event // Public
{
    void loop::add(absractions::iWindow *window)
    {
        if(!window){ return; }
        _windows.push_back(window);
    }

    void loop::remove(absractions::iWindow *window)
    {
        for(size_t i = 0; i < _windows.size(); ++i)
        {
            absractions::iWindow* _window = _windows[i];
            if(_window == window)
            {
                _windows[i] = _windows.back();
                _windows.pop_back();
                --i;

                break;
            }
        }
    }
    
    void loop::run()
    {
        if (_windows.empty()){ return; }
        
        displayService service = (*_windows.begin())->getDisplayService();
        absractions::iWindow* mainWindow = _windows.front();

        // Begin Loop
        while(true)
        {
            bool isAlive = true;
            for(size_t i = 0; i < _windows.size(); ++i)
            {
                absractions::iWindow* window = _windows[i];

                if(!window->isRunning()) 
                {
                    window->close();
                    delete window;

                    _windows[i] = _windows.back();
                    _windows.pop_back();
                    --i;
                    
                    continue; 
                }

                window->handelEvents();
                if(window == mainWindow && !window->isRunning()){ isAlive = false; }
            }

            if(!isAlive || _windows.empty()){ break; }
            #if defined(_WIN32)
            
            MSG msg; // Fixes issue when closing window, still need to fix the unfocused window closeing all windows issue though
            while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                if(msg.message == WM_QUIT) return;
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            
            MsgWaitForMultipleObjects(0, nullptr, FALSE, INFINITE, QS_ALLINPUT);
            continue;

            #elif defined(__APPLE__)

            #error TODO
            continue;

            #else

                #if GOOSEUI_XORG_SUPPORT
                if(service == displayService::x11)
                {
                    int lastXfd = 0; 
                    fd_set in_fds;
                    
                    FD_ZERO(&in_fds);
                    for(absractions::iWindow* window : _windows)
                    {
                        platform::x11_window* x11Window = dynamic_cast<platform::x11_window*>(window);
                        int xfd = ConnectionNumber(x11Window->getDisplay());
                        FD_SET(xfd, &in_fds); 
                        lastXfd = std::max(lastXfd, xfd);
                    }

                    struct timeval timeout { 0, 160000 }; // Temp fix for multi-window, yes I hate it too
                    select(lastXfd + 1, &in_fds, nullptr, nullptr, &timeout);
                    
                    continue;
                }
                #endif

                #if GOOSEUI_WAYLAND_SUPPORT
                if(service == displayService::wayland)
                {

                }
                #endif

            #endif
        }
    }
}