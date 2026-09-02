#include "GooseUI/graphics/GL/glLoader.h"

#include <stdio.h>

#if defined(_WIN32)
    #include <windows.h>
    #include <GL/gl.h>

    static void* (*wglGetProc)(const char*) = nullptr;
    static HMODULE lib = nullptr;
#else
    #include <dlfcn.h>
    static void* lib = nullptr;
#endif


namespace // Local
{
    static void* getProcAddress(const char* name)
    {
        void* result = nullptr;
        if(lib == nullptr) { return NULL; }
    
        #if defined(_WIN32)
            if(wglGetProc) result = wglGetProc(name);
            if(!result) result = (void*)GetProcAddress(lib, name);
        #else
            result = dlsym(lib, name);
        #endif
    
        return result;
    }
    
    static int openLib()
    {
        #if defined (_WIN32)
            lib = LoadLibraryA("opengl32.dll");
            if(!lib) { return 0; }
    
            wglGetProc = (void*(*)(const char*))GetProcAddress(lib, "wglGetProcAddress");
            return 1;
        #else
        #if defined(__APPLE__)
            static const char *glPaths[] = 
            {
                "/System/Library/Frameworks/OpenGL.framework/OpenGL",
                "/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL"
            };
        #else
            static const char *glPaths[] =
            {
                "libGL.so.1", 
                "libGL.so"
            };
        #endif
            for(int i = 0; i < (sizeof(glPaths) / sizeof(glPaths[0])); i++)
            {
                lib = dlopen(glPaths[i], RTLD_NOW | RTLD_GLOBAL);
                if(lib != NULL) { return 1; }
            }
        #endif
    
        return 0;
    }
}

namespace GooseUI::graphics::gl // Public
{
    PFNGLCREATESHADERPROC glCreateShader = nullptr;
    PFNGLSHADERSOURCEPROC glShaderSource = nullptr;
    PFNGLCOMPILESHADERPROC glCompileShader = nullptr;
    PFNGLCREATEPROGRAMPROC glCreateProgram = nullptr;
    PFNGLATTACHSHADERPROC glAttachShader = nullptr;
    PFNGLLINKPROGRAMPROC glLinkProgram = nullptr;
    PFNGLUSEPROGRAMPROC glUseProgram = nullptr;
    PFNGLDELETEBUFFERSPROC glDeleteBuffers = nullptr;
    PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = nullptr;
    PFNGLDELETEPROGRAMPROC glDeleteProgram = nullptr;
    PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = nullptr;
    PFNGLBINDVERTEXARRAYPROC glBindVertexArray = nullptr;
    PFNGLGENBUFFERSPROC glGenBuffers = nullptr;
    PFNGLBUFFERDATAPROC glBufferData = nullptr;
    PFNGLBINDBUFFERPROC glBindBuffer = nullptr;
    PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = nullptr;
    PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = nullptr;
    PFNGLGETSHADERIVPROC glGetShaderiv = nullptr;
    PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = nullptr;
    PFNGLCLEARCOLORPROC glClearColor = nullptr;
    PFNGLCLEARPROC glClear = nullptr;
    PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = nullptr;
    PFNGLUNIFORM1FPROC glUniform1f = nullptr;
    PFNGLUNIFORM2FPROC glUniform2f = nullptr;
    PFNGLUNIFORM4FPROC glUniform4f = nullptr;
    PFNGLDRAWELEMENTSPROC glDrawElements = nullptr;
    PFNGLDELETESHADERPROC glDeleteShader = nullptr;
    PFNGLBUFFERSUBDATAPROC glBufferSubData = nullptr;
    PFNGLACTIVETEXTUREPROC glActiveTexture = nullptr;
    PFNGLUNIFORM1IPROC glUniform1i = nullptr;
    PFNGLBLENDEQUATIONPROC glBlendEquation = nullptr;
    PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = nullptr;

    int isloaded()
    {
        if(lib != NULL)
        {
            return 1;
        }

        return 0;
    }

    int loadGL()
    {
        if(!openLib()) { return 0; }

        glCreateShader = (PFNGLCREATESHADERPROC) getProcAddress("glCreateShader");
        glShaderSource = (PFNGLSHADERSOURCEPROC) getProcAddress("glShaderSource");
        glCompileShader = (PFNGLCOMPILESHADERPROC)getProcAddress("glCompileShader");
        glCreateProgram = (PFNGLCREATEPROGRAMPROC)getProcAddress("glCreateProgram");
        glAttachShader = (PFNGLATTACHSHADERPROC) getProcAddress("glAttachShader");
        glLinkProgram = (PFNGLLINKPROGRAMPROC) getProcAddress("glLinkProgram");
        glUseProgram = (PFNGLUSEPROGRAMPROC) getProcAddress("glUseProgram");
        glDeleteBuffers = (PFNGLDELETEBUFFERSPROC) getProcAddress("glDeleteBuffers");
        glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC) getProcAddress("glDeleteVertexArrays");
        glDeleteProgram = (PFNGLDELETEPROGRAMPROC) getProcAddress("glDeleteProgram");
        glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC) getProcAddress("glGenVertexArrays");
        glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC) getProcAddress("glBindVertexArray");
        glGenBuffers = (PFNGLGENBUFFERSPROC) getProcAddress("glGenBuffers");
        glBufferData = (PFNGLBUFFERDATAPROC) getProcAddress("glBufferData");
        glBindBuffer = (PFNGLBINDBUFFERPROC) getProcAddress("glBindBuffer");
        glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC) getProcAddress("glVertexAttribPointer");
        glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC) getProcAddress("glEnableVertexAttribArray");
        glGetShaderiv = (PFNGLGETSHADERIVPROC) getProcAddress("glGetShaderiv");
        glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) getProcAddress("glGetShaderInfoLog");
        glClearColor = (PFNGLCLEARCOLORPROC) getProcAddress("glClearColor");
        glClear = (PFNGLCLEARPROC) getProcAddress("glClear");
        glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) getProcAddress("glGetUniformLocation");
        glUniform1f = (PFNGLUNIFORM1FPROC) getProcAddress("glUniform1f");
        glUniform2f = (PFNGLUNIFORM2FPROC) getProcAddress("glUniform2f");
        glUniform4f = (PFNGLUNIFORM4FPROC) getProcAddress("glUniform4f");
        glDrawElements = (PFNGLDRAWELEMENTSPROC) getProcAddress("glDrawElements");
        glDeleteShader = (PFNGLDELETESHADERPROC) getProcAddress("glDeleteShader");
        glBufferSubData = (PFNGLBUFFERSUBDATAPROC) getProcAddress("glBufferSubData");
        glActiveTexture = (PFNGLACTIVETEXTUREPROC) getProcAddress("glActiveTexture");
        glUniform1i = (PFNGLUNIFORM1IPROC) getProcAddress("glUniform1i");
        glBlendEquation = (PFNGLBLENDEQUATIONPROC) getProcAddress("glBlendEquation");
        glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) getProcAddress("glDisableVertexAttribArray");

        return 1;
    }

    static void closeGL()
    {
        if(lib != NULL)
        {
        #if defined (_WIN32)
            FreeLibrary(lib);
        #else
            dlclose(lib);
        #endif

            lib = NULL;
        }
    }
}