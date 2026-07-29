#include "GooseUI/context.h"


namespace GooseUI // Private
{
    // Graphic
    static application::backendType _backendType = application::backendType::OpenGL;
    static absractions::iRenderer* _renderer = nullptr;
    //static application* _instance = nullptr;
}

namespace GooseUI // Public
{
    // Graphics
    void application::init(backendType backendType)
    {
        if(_renderer){ return; }
        
        #if GOOSEUI_HAS_OPENGL
        if(backendType == backendType::OpenGL)
        { 
            _renderer = new graphics::gl::glRenderer(); 
            _backendType = backendType::OpenGL;
        }
        #endif
        
        #if GOOSEUI_HAS_VULKAN
            // TODO
        #endif
    }
    
    application::backendType application::getBackendType(){ return _backendType; }
    absractions::iRenderer* application::getRenderer(){ return _renderer; }
}