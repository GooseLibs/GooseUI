#include "GooseUI/abstractions/iWindow.h"

#if defined(_WIN32)

    #include "GooseUI/platform/win32_window.h"
    GooseUI::absractions::iWindow* GooseUI::absractions::createWindow(const windowCreationInfo& info)
    {
        return new GooseUI::platform::win32_window(info);
    }

#elif defined(__unix__) && !defined(__APPLE__)

    #if defined(GOOSEUI_WAYLAND_SUPPORT)
        //#include "GooseUI/platform/wl_window.h"
    #endif
    
    #if defined(GOOSEUI_XORG_SUPPORT)
        #include "GooseUI/platform/x11_window.h"
    #endif

    GooseUI::absractions::iWindow* GooseUI::absractions::createWindow(const windowCreationInfo& info)
    {
        #if defined(GOOSEUI_WAYLAND_SUPPORT)
        const char* xdgSession = std::getenv("XDG_SESSION_TYPE");
        const char* wlDisplay = std::getenv("WAYLAND_DISPLAY");

        //if(wlDisplay !=nullptr || xdgSession && std::string(xdgSession) == "wayland")
            //    {return new GooseUI::platform::wl_window(title, width, height, posistion);}
        #endif

        #if defined(GOOSEUI_XORG_SUPPORT)
        return new GooseUI::platform::x11_window(info);
        #endif

        return nullptr;
    }
#else
#endif