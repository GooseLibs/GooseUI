#ifndef _GOOSEUI_WL_WINDOW_H_
#define _GOOSEUI_WL_WINDOW_H_

#include "GooseUI/context.h"

#include "GooseUI/abstractions/iWindow.h"
#include "GooseUI/abstractions/iRenderer.h"
#include "GooseUI/abstractions/iWidget.h"

#include "GooseUI/modules/wayland-protocals/xdg-shell.h"

#include <wayland-client.h>

namespace GooseUI
{
    namespace platform
    {
        class wl_window : public absractions::iWindow
        {
            // I hate this. I hate this. Everything is tell me not too but the rubber duck is telling me to do
            static wl_display* _display;
            static wl_registry* _registry;
            static wl_compositor* _compositor;
            static xdg_wm_base* _xdg_wm_base;

            static void _registry_handle(void* data, wl_registry* reg, uint32_t id, const char* interface, uint32_t version);
            static void _registry_remover(void* data, wl_registry* reg, uint32_t id);
            
            static const wl_registry_listener _registry_listener;
            static const xdg_surface_listener _xdg_surface_listener;
            static const xdg_toplevel_listener _xdg_toplevel_listener;

            wl_surface* _surface = nullptr;
            xdg_surface* _xdg_surface = nullptr;
            xdg_toplevel* _xdg_toplevel = nullptr;
            
            void* _windowCtx;
            
            void _gl_createContext();
            void _gl_shareContext();
            void _gl_destoryContext();

            void _vk_createContext();
            void _vk_shareContext();
            
            void _startRenderFrame();
            void _endRenderFrame();

            public:
            wl_window(const std::string& title, int width, int height, screenPosistion posistion);
            virtual ~wl_window();

            wl_surface* getSurface();
            wl_display* getDisplay();

            // Overides
            // Window Configuration
            void setWindowIcon(const std::string& ICO) override;
            void setHeader(const std::string& title, bool isVisible, bool hasButtons, bool hasMinimize, bool hasMaximise) override;
            
            void isResizeable(bool isResizeable) override;
            void isAllwaysOnTop(bool isOnTop) override;

            // Window Size
            void setSize(int width, int height) override;
            
            void maximize() override;
            void minimize() override;
            void restoreSize() override;

            // Window Visibility
            void show() override;
            void hide() override;
            void destroy() override;

            // Widget Management
            void addWidgetToVector(absractions::iWidget* widget) override;
            void removeWidgetFromVector(absractions::iWidget* widget) override;
            void renderWidgets() override;
            void handelEvents() override;

            // Reuturns
            displayService getDisplayService() const override;
            int getWidth() override;
            int getHeight() override;
        };
    }
}

#endif /*_GOOSEUI_WL_WINDOW_H_*/