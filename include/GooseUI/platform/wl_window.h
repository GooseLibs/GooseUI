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
#include "GooseUI/modules/wayland-protocols/cursor-shape.h"

#include <wayland-client.h>
#include <wayland-cursor.h>

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
            static wl_pointer* _pointer;
            static wl_window* _pointerFocusedWindow;
            
            static wp_cursor_shape_manager_v1* _cursorShapeManager;
            static wp_cursor_shape_device_v1* _cursorShapeDevice;

            static uint32_t _lastPointerSerial;
            static uint32_t _lastEnterSerial;
            
            static xdg_wm_base* _xdg_wm_base;
            static zxdg_decoration_manager_v1* _decoration_manager;
            
            static void _registry_handle(void* data, wl_registry* reg, uint32_t id, const char* interface, uint32_t version);
            static void _registry_remover(void* data, wl_registry* reg, uint32_t id);
            static void _seat_capabilities(void* data, wl_seat* seat, uint32_t capabilities);

            static void _xdg_surface_configure(void* data, xdg_surface* surface, uint32_t serial);
            static void _xdg_toplevel_configure(void* data, xdg_toplevel* toplevel, int32_t width, int32_t height, wl_array* states);
            static void _xdg_toplevel_close(void* data, xdg_toplevel* toplevel);

            static const wl_registry_listener _registry_listener;
            static const wl_seat_listener _seat_listener; 
            static const wl_pointer_listener _pointer_listener; 
             
            static const xdg_surface_listener _xdg_surface_listener;
            static const xdg_toplevel_listener _xdg_toplevel_listener;
            static const zxdg_toplevel_decoration_v1_listener _xdg_decoration_listener;

            // Per Instance
            wl_surface* _surface = nullptr;
            
            xdg_surface* _xdg_surface = nullptr;
            xdg_toplevel* _xdg_toplevel = nullptr;
            zxdg_toplevel_decoration_v1* _xdg_toplevel_decorations = nullptr;

            uint32_t _decorationMode = ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE;
            uint32_t _getDirectionalCursor(int direction);
            uint32_t _isCursorOnWindowEdge(int x, int y, int win_width, int win_height);
            
            int _mouseX, _mouseY;
            
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