#ifndef _GOOSEUI_WL_WINDOW_H_
#define _GOOSEUI_WL_WINDOW_H_

#include "GooseUI/types.h"
#include "GooseUI/context.h"

#include "GooseUI/graphics/titleBar.h"

#include "GooseUI/abstractions/iWindow.h"
#include "GooseUI/abstractions/iRenderer.h"
#include "GooseUI/abstractions/iWidget.h"

#include "GooseUI/modules/wayland-protocols/xdg-shell.h"
#include "GooseUI/modules/wayland-protocols/xdg-decoration.h"

#include <wayland-client.h>

namespace GooseUI
{
    namespace platform
    {
        class wl_window : public absractions::iWindow
        {
            struct wl_windowState
            {
                int width;
                int height;
                bool needUpdate;
            };
            
            // Static classes - Used by all the windows
            static wl_display* _display;
            static wl_registry* _registry;
            static wl_compositor* _compositor;
            static wl_seat* _seat;

            static uint32_t _lastPointerSerial;
            
            static xdg_wm_base* _xdg_wm_base;
            static zxdg_decoration_manager_v1* _decoration_manager;

            static void _registry_handle(void* data, wl_registry* reg, uint32_t id, const char* interface, uint32_t version);
            static void _registry_remover(void* data, wl_registry* reg, uint32_t id);

            static const wl_registry_listener _registry_listener;
            static const xdg_surface_listener _xdg_surface_listener;
            static const xdg_toplevel_listener _xdg_toplevel_listener;

            // Per Instance
            wl_surface* _surface = nullptr;
            
            xdg_surface* _xdg_surface = nullptr;
            xdg_toplevel* _xdg_toplevel = nullptr;
            zxdg_toplevel_decoration_v1* _xdg_toplevel_decorations = nullptr;
            uint32_t _decorationMode = ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE;
            
            wl_windowState _windowState;
            void* _windowCtx;
            
            void _gl_createContext();
            void _gl_destoryContext();

            void _vk_createContext();
            void _vk_destoryContext();
            
            void _startRenderFrame();
            void _endRenderFrame();

            public:
            wl_window(const windowCreationInfo& info);
            virtual ~wl_window();

            wl_surface* getSurface();
            wl_display* getDisplay();

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

#endif /*_GOOSEUI_WL_WINDOW_H_*/