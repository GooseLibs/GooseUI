#include "GooseUI/platform/wl_window.h"
#include "GooseUI/platform/wl_decorations.h"

#include <algorithm>
#include <cstring>


namespace GooseUI::platform // Private
{
    // Static
    wl_display* wl_window::_display = nullptr;
    wl_registry* wl_window::_registry = nullptr; 
    wl_compositor* wl_window::_compositor = nullptr;
    xdg_wm_base* wl_window::_xdg_wm_base = nullptr;

    void wl_window::_registry_handle(void* data, wl_registry* reg, uint32_t id, const char* interface, uint32_t version){}
    void wl_window::_registry_remover(void* data, wl_registry* reg, uint32_t id){}

    const wl_registry_listener wl_window::_registry_listener = { _registry_handle, _registry_remover };
    const xdg_surface_listener wl_window::_xdg_surface_listener = {};
    const xdg_toplevel_listener wl_window::_xdg_toplevel_listener = {};

    // Window
    void wl_window::_startRenderFrame()
    {
        
    }

    void wl_window::_endRenderFrame()
    {
        
    }

    #if GOOSEUI_HAS_OPENGL
    void wl_window::_gl_createContext()
    {
        
    }

    void wl_window::_gl_destoryContext()
    {
        
    }
    #endif

    #if GOOSEUI_HAS_VULKAN
    #endif
}

namespace GooseUI::platform // public
{
    wl_window::wl_window(const windowCreationInfo& info)
    {
    
    }
    
    wl_window::~wl_window()
    {
        if(!_isRunning && _surface == None){ return; }
        _isRunning = false;

        switch (application::getBackendType()) 
        {
            #if GOOSEUI_HAS_OPENGL
            case application::backendType::OpenGL:
                _gl_destoryContext();
                break;
            #endif
            
            #if GOOSEUI_ENABLE_VULKAN
            #endif
            
            default:
                printf("GooseUI: Backend Not Initilized! \n");
                break;
        }
    }
    
    wl_surface* wl_window::getSurface(){}
    wl_display* wl_window::getDisplay(){}

    // OVERIDES
    displayService wl_window::getDisplayService() const { return displayService::wayland; }
    void wl_window::setBackgroundColor(color color){ _bgColor = color; }
    
    // Titlebar
    void wl_window::setTitleBarDecorations(const titlebarCreationInfo& info){}
    absractions::iWidget* wl_window::getClientTitleBar() { if(_clientDecorations){ return _clientDecorations->bar; } return nullptr; }
    
    // Window Size
    void wl_window::setSize(int width, int height){}
    void wl_window::isResizeable(bool isResizeable){}
    void wl_window::maximize(){}
    void wl_window::minimize(){}
    void wl_window::restoreSize(){}
    
    int wl_window::getWidth(){}
    int wl_window::getHeight(){}
    
    // Window Visibility
    void wl_window::isAllwaysOnTop(bool isOnTop){}
    void wl_window::show(){}
    void wl_window::hide(){}
    void wl_window::close(){ _isRunning = false;  }
    
    // Widget Management
    void wl_window::addWidgetToVector(absractions::iWidget* widget) { _widgets.push_back(widget); }
    void wl_window::removeWidgetFromVector(absractions::iWidget* widget)
    {
        std::vector<absractions::iWidget*>::iterator target = std::find(_widgets.begin(), _widgets.end(), widget);
        if(target != _widgets.end()){ _widgets.erase(target); }
    }
    
    void wl_window::renderWidgets()
    {
        if(!application::getRenderer()) return;
        _startRenderFrame();
        
        for(absractions::iWidget* widget : _widgets)
        {
            if(widget)
            {
                widget->draw(); // Thought I was just copying and pasteing, huh?
            }
        }
        
        _endRenderFrame();
    }
    
    void wl_window::handelEvents() {}
}