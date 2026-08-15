#include "GooseUI/graphics/GL/glRenderer.h"
#include "GooseUI/graphics/GL/glShaders.h"
#include "GooseUI/graphics/GL/glLoader.h"

#include <stdio.h>


namespace GooseUI::graphics::gl // Private
{
    unsigned int glRenderer::_createShaderProgram(const std::string& vertexShader, const std::string& fragmentShader)
    {
        unsigned int compiledVertexShader = _compileShader(vertexShader, GL_VERTEX_SHADER);
        unsigned int compiledFragmentShader = _compileShader(fragmentShader, GL_FRAGMENT_SHADER);
        unsigned int program = glCreateProgram();

        glAttachShader(program, compiledVertexShader);
        glAttachShader(program, compiledFragmentShader);

        glLinkProgram(program);

        glDeleteShader(compiledVertexShader);
        glDeleteShader(compiledFragmentShader);

        return program;
    }
    
    unsigned int glRenderer::_compileShader(const std::string& shader, unsigned int type)
    {
        unsigned int shaderID = glCreateShader(type);
        const char* c_shader = shader.c_str();

        glShaderSource(shaderID, 1, &c_shader, nullptr);
        glCompileShader(shaderID);

        int completed;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &completed);
        if(!completed)
        {
            char infoLog[512];
            glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
            printf("[GooseUI] Shader compile error: %s", infoLog);
        }

        return shaderID;
    }
}

namespace GooseUI::graphics::gl // Public
{
    glRenderer::glRenderer() {}
    glRenderer::~glRenderer()
    {
        glDeleteBuffers(1, &_sharedShader.vbo);
        glDeleteBuffers(1, &_sharedShader.ebo);
        glDeleteVertexArrays(1, &_sharedShader.vao);
        glDeleteProgram(_sharedShader.shader);
    }
    
    void glRenderer::initRenderer()
    {        
        if(!gl::isloaded()) { gl::loadGL(); printf("[GooseUI] Initilizing OpenGL \n"); }
        
        // initilize Resources
        if(_sharedShader.vbo == 0)
        {
            glGenVertexArrays(1, &_sharedShader.vao);
            glBindVertexArray(_sharedShader.vao);

            glGenBuffers(1, &_sharedShader.vbo);
            glBindBuffer(GL_ARRAY_BUFFER, _sharedShader.vbo);
            glBufferData(GL_ARRAY_BUFFER, 6 * 8 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

            unsigned int indices[] = {0, 1, 2, 2, 3, 0};
            glGenBuffers(1, &_sharedShader.ebo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _sharedShader.ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(4 * sizeof(float)));
            glEnableVertexAttribArray(2);

            _sharedShader.shader = _createShaderProgram(shared_vertexSrc, shared_fragmentSrc);
            glBindVertexArray(0);
        }
    }
    
    void glRenderer::beginFrame(int windowWidth, int windowHeight, const color& color)
    {
        _windowWidth = windowWidth;
        _windowHeight = windowHeight;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glClearColor(color.R, color.G, color.B, color.A);
        glClear(GL_COLOR_BUFFER_BIT);

        glViewport(0, 0, _windowWidth, _windowHeight);

        int localResolution = glGetUniformLocation(_sharedShader.shader, "uResolution");
        glUniform2f(localResolution, (float)_windowWidth, (float)_windowHeight);
    }
    
    void glRenderer::endFrame()
    {
        // HANDELED PER WINDOW
    }

    unsigned int glRenderer::uploadTexture(const unsigned char* pixelData, int width, int height, bool crispPixels, bool grayscale)
    {    
        // Convert form Crossplatform API to OpenGL
        unsigned int internalFormat = grayscale ? GL_R8 : GL_RGBA8;
        unsigned int format = grayscale ? GL_RED : GL_RGBA;
        unsigned int filter = crispPixels ? GL_NEAREST : GL_LINEAR;
            
        unsigned int id;
        glGenTextures(1, &id);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, id);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        if (grayscale) 
        {
            glTexParameteri(GL_TEXTURE_2D, 0x8E42, GL_RED);
            glTexParameteri(GL_TEXTURE_2D, 0x8E43, GL_RED);
            glTexParameteri(GL_TEXTURE_2D, 0x8E44, GL_RED);
            glTexParameteri(GL_TEXTURE_2D, 0x8E45, GL_RED);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, pixelData);
        glBindTexture(GL_TEXTURE_2D, 0);
        return id;
    }
    
    void glRenderer::drawRect(float X, float Y, float W, float H, const color& C)
    {
        float x0 = (2.0f * X) / _windowWidth - 1.0f;
        float x1 = (2.0f * (X + W)) / _windowWidth - 1.0f;
    
        float y0 = 1.0f - 2.0f * Y / _windowHeight;
        float y1 = 1.0f - 2.0f * (Y + H) / _windowHeight;

        float vertices[] = {
            x0, y0, 0.0f, 0.0f, C.R, C.G, C.B, C.A,
            x1, y0, 0.0f, 0.0f, C.R, C.G, C.B, C.A,
            x1, y1, 0.0f, 0.0f, C.R, C.G, C.B, C.A,
            x0, y1, 0.0f, 0.0f, C.R, C.G, C.B, C.A,
        };
        
        glUseProgram(_sharedShader.shader);
        glBindVertexArray(_sharedShader.vao);

        int useTexLoc = glGetUniformLocation(_sharedShader.shader, "u_UseTexture");
        glUniform1f(useTexLoc, 0.0f);
        
        glBindBuffer(GL_ARRAY_BUFFER, _sharedShader.vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    
    void glRenderer::drawTexture(unsigned int ID, float X, float Y, float W, float H, float u0, float u1, float v0, float v1, const color& C)
    {
        float x0 = (2.0f * X) / _windowWidth - 1.0f;
        float x1 = (2.0f * (X + W)) / _windowWidth - 1.0f;
        float y0 = 1.0f - 2.0f * Y / _windowHeight;
        float y1 = 1.0f - 2.0f * (Y + H) / _windowHeight;

        float vertices[6][8] = {
            { x0, y0, u0, v0, C.R, C.G, C.B, C.A },
            { x1, y0, u1, v0, C.R, C.G, C.B, C.A },
            { x0, y1, u0, v1, C.R, C.G, C.B, C.A },

            { x1, y0, u1, v0, C.R, C.G, C.B, C.A },
            { x1, y1, u1, v1, C.R, C.G, C.B, C.A },
            { x0, y1, u0, v1, C.R, C.G, C.B, C.A },
        };

        glUseProgram(_sharedShader.shader);
        glBindVertexArray(_sharedShader.vao);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ID);
    
        GLint texLocation = glGetUniformLocation(_sharedShader.shader, "u_Texture");
        glUniform1i(texLocation, 0);

        int useTexLoc = glGetUniformLocation(_sharedShader.shader, "u_UseTexture");
        glUniform1f(useTexLoc, 1);

        glBindBuffer(GL_ARRAY_BUFFER, _sharedShader.vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
    
    void glRenderer::drawSdfFont()
    {
        // TODO
    }
    
    // Returns
    int glRenderer::hasContext() { return _context != nullptr; };
    glContext& glRenderer::getContext() { return *_context; }
    void glRenderer::setContext(glContext* context) { _context = context; }
}