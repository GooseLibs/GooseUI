#ifndef _GOOSEUI_CONTEXT_H_
#define _GOOSEUI_CONTEXT_H_

#if GOOSEUI_HAS_VULKAN
    // TODO
#endif

#if GOOSEUI_HAS_OPENGL
    #include "GooseUI/graphics/GL/glRenderer.h"
#endif

/* 
    The Idea of this is too prevent people from having multiple Backends per application, which could cause future issues.
    This also simplifies adding a access point for a backendPanel to allow user access without dependence on making a custom widget
    
    4/27/26 - Added the fontFactory pointer here, it should also be static, but I dont need a identifier (backendType) because there only one font lib per os (Windows DWrite, Everything else FreeType)
    7/27/26 - Moved fontFactory pointer to fondManager to encurage its use
*/

namespace GooseUI
{
    class application
    {  
        public:
        //enum class graphicsBackend{ OpenGL, Vulkan }; - We add VK Support later
        enum class backendType{ OpenGL };
        
        // Graphics
        static void init(backendType backendType);
        static backendType getBackendType();
        static absractions::iRenderer* getRenderer();
    };
}

#endif /*_GOOSEUI_CONTEX_H_*/