/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <TGUI/Backend/Renderer/GLES2/BackendRenderTargetGLES2.hpp>
#include <TGUI/Backend/Renderer/OpenGL.hpp>

#if TGUI_BUILD_AS_CXX_MODULE
    import tgui;
#else
    #include <TGUI/Backend/Renderer/BackendText.hpp>
    #include <TGUI/Backend/Window/Backend.hpp>
    #include <TGUI/Container.hpp>
#endif

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <numeric>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_NODISCARD static GLuint createShaderProgram()
    {
        // Select the vertex and fragment shaders based on which GLES version is available.
        // The version for both shaders should be the same (at least with some mesa drivers).
        const GLchar* vertexShaderSource;
        const GLchar* fragmentShaderSource;
        if (TGUI_GLAD_GL_ES_VERSION_3_1)
        {
            vertexShaderSource =
                "#version 310 es\n"
                "layout(location=0) uniform mat4 projectionMatrix;\n"
                "layout(location=0) in vec2 inPosition;\n"
                "layout(location=1) in vec4 inColor;\n"
                "layout(location=2) in vec2 inTexCoord;\n"
                "out vec4 color;\n"
                "out vec2 texCoord;\n"
                "void main() {\n"
                "    gl_Position = projectionMatrix * vec4(inPosition.x, inPosition.y, 0, 1);\n"
                "    color = inColor;\n"
                "    texCoord = inTexCoord;\n"
                "}";
            fragmentShaderSource =
                "#version 310 es\n"
                "precision mediump float;\n"
                "uniform sampler2D uTexture;\n"
                "in vec4 color;\n"
                "in vec2 texCoord;\n"
                "out vec4 outColor;\n"
                "void main() {\n"
                "    outColor = texture(uTexture, texCoord) * color;\n"
                "}";
        }
        else if (TGUI_GLAD_GL_ES_VERSION_3_0)
        {
            vertexShaderSource =
                "#version 300 es\n"
                "uniform mat4 projectionMatrix;\n"
                "layout(location=0) in vec2 inPosition;\n"
                "layout(location=1) in vec4 inColor;\n"
                "layout(location=2) in vec2 inTexCoord;\n"
                "out vec4 color;\n"
                "out vec2 texCoord;\n"
                "void main() {\n"
                "    gl_Position = projectionMatrix * vec4(inPosition.x, inPosition.y, 0, 1);\n"
                "    color = inColor;\n"
                "    texCoord = inTexCoord;\n"
                "}";
            fragmentShaderSource =
                "#version 300 es\n"
                "precision mediump float;\n"
                "uniform sampler2D uTexture;\n"
                "in vec4 color;\n"
                "in vec2 texCoord;\n"
                "out vec4 outColor;\n"
                "void main() {\n"
                "    outColor = texture(uTexture, texCoord) * color;\n"
                "}";
        }
        else // No GLES 3 support
        {
            vertexShaderSource =
                "#version 100\n"
                "uniform mat4 projectionMatrix;\n"
                "attribute vec2 inPosition;\n"
                "attribute vec4 inColor;\n"
                "attribute vec2 inTexCoord;\n"
                "varying vec4 color;\n"
                "varying vec2 texCoord;\n"
                "void main() {\n"
                "    gl_Position = projectionMatrix * vec4(inPosition.x, inPosition.y, 0, 1);\n"
                "    color = inColor;\n"
                "    texCoord = inTexCoord;\n"
                "}";
            fragmentShaderSource =
                "#version 100\n"
                "precision mediump float;\n"
                "uniform sampler2D uTexture;\n"
                "varying vec4 color;\n"
                "varying vec2 texCoord;\n"
                "void main() {\n"
                "    gl_FragColor = texture2D(uTexture, texCoord) * color;\n"
                "}";
        }

        // Create the vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        if (vertexShader == 0)
            throw Exception{U"Failed to create shaders in BackendRenderTargetGLES2. glCreateShader(GL_VERTEX_SHADER) returned 0."};

        TGUI_GL_CHECK(glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr));
        TGUI_GL_CHECK(glCompileShader(vertexShader));

        GLint vertexShaderCompiled = GL_FALSE;
        TGUI_GL_CHECK(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexShaderCompiled));
        if (vertexShaderCompiled != GL_TRUE)
            throw Exception{U"Failed to create shaders in BackendRenderTargetGLES2. Failed to compile vertex shader."};

        // Create the fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        if (fragmentShader == 0)
            throw Exception{U"Failed to create shaders in BackendRenderTargetGLES2. glCreateShader(GL_FRAGMENT_SHADER) returned 0."};

        TGUI_GL_CHECK(glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr));
        TGUI_GL_CHECK(glCompileShader(fragmentShader));

        GLint fragmentShaderCompiled = GL_FALSE;
        TGUI_GL_CHECK(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentShaderCompiled));
        if (fragmentShaderCompiled != GL_TRUE)
            throw Exception{U"Failed to create shaders in BackendRenderTargetGLES2. Failed to compile fragment shader."};

        // Link the vertex and fragment shader into a program
        GLuint programId = glCreateProgram();
        TGUI_GL_CHECK(glAttachShader(programId, vertexShader));
        TGUI_GL_CHECK(glAttachShader(programId, fragmentShader));
        TGUI_GL_CHECK(glLinkProgram(programId));

        GLint programLinked = GL_TRUE;
        TGUI_GL_CHECK(glGetProgramiv(programId, GL_LINK_STATUS, &programLinked));
        if (programLinked != GL_TRUE)
        {
            GLchar errorMessage[512];
            GLsizei errorMessageLength = 0;
            glGetProgramInfoLog(programId, 512, &errorMessageLength, static_cast<GLchar*>(errorMessage));
            const String errorMessageStr{static_cast<const char*>(errorMessage), static_cast<std::size_t>(errorMessageLength)};
            throw Exception{U"Failed to create shaders in BackendRenderTargetGLES2. Failed to link the shaders. Error: '" + errorMessageStr + "'"};
        }

        return programId;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendRenderTargetGLES2::BackendRenderTargetGLES2() :
        m_shaderProgram(createShaderProgram())
    {
        TGUI_ASSERT(isBackendSet(), "BackendRenderTargetGLES2 can't be created when there is no system backend initialized (was a gui created yet?)");
        TGUI_ASSERT(getBackend()->getRenderer(), "BackendRenderTargetGLES2 can't be created when there is no backend renderer (was a gui attached to a window yet?)");

        // If our OpenGL version didn't support the layout qualifier in GLSL then we need to query the location
        if (!TGUI_GLAD_GL_ES_VERSION_3_1)
        {
            m_projectionMatrixShaderUniformLocation = glGetUniformLocation(m_shaderProgram, "projectionMatrix");
            if (m_projectionMatrixShaderUniformLocation < 0)
                throw Exception{U"Failed to initialize BackendRenderTargetGLES2: projectionMatrix uniform wasn't found in shader program"};
        }

        if (!TGUI_GLAD_GL_ES_VERSION_3_0)
        {
            const int positionShaderLocation = glGetAttribLocation(m_shaderProgram, "inPosition");
            const int colorShaderLocation = glGetAttribLocation(m_shaderProgram, "inColor");
            const int texCoordShaderLocation = glGetAttribLocation(m_shaderProgram, "inTexCoord");
            if (positionShaderLocation < 0)
                throw Exception{U"Failed to initialize BackendRenderTargetGLES2: inPosition wasn't found in shader program"};
            if (colorShaderLocation < 0)
                throw Exception{U"Failed to initialize BackendRenderTargetGLES2: inColor wasn't found in shader program"};
            if (texCoordShaderLocation < 0)
                throw Exception{U"Failed to initialize BackendRenderTargetGLES2: inTexCoord wasn't found in shader program"};

            m_positionShaderLocation = static_cast<GLuint>(positionShaderLocation);
            m_colorShaderLocation = static_cast<GLuint>(colorShaderLocation);
            m_texCoordShaderLocation = static_cast<GLuint>(texCoordShaderLocation);
        }

        createBuffers();

        // Create a solid white 1x1 texture to pass to the shader when we aren't drawing a texture
        m_emptyTexture = std::make_unique<BackendTextureGLES2>();
        auto pixels = MakeUniqueForOverwrite<std::uint8_t[]>(4); // 4 bytes to store RGBA values
        for (unsigned int i = 0; i < 4; ++i)
            pixels[i] = 255;
        m_emptyTexture->load({1,1}, std::move(pixels), false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendRenderTargetGLES2::~BackendRenderTargetGLES2()
    {
        TGUI_GL_CHECK(glDeleteBuffers(1, &m_vertexBuffer));
        TGUI_GL_CHECK(glDeleteBuffers(1, &m_indexBuffer));

        if (TGUI_GLAD_GL_ES_VERSION_3_0)
            TGUI_GL_CHECK(glDeleteVertexArrays(1, &m_vertexArray));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetGLES2::setClearColor(const Color& color)
    {
        glClearColor(color.getRed() / 255.f, color.getGreen() / 255.f, color.getBlue() / 255.f, color.getAlpha() / 255.f);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetGLES2::clearScreen()
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetGLES2::setView(FloatRect view, FloatRect viewport, Vector2f targetSize)
    {
        BackendRenderTarget::setView(view, viewport, targetSize);

        m_projectionTransform = Transform();
        m_projectionTransform.translate({-1 - (2.f * (view.left / view.width)), 1 + (2.f * (view.top / view.height))});
        m_projectionTransform.scale({2.f / view.width, -2.f / view.height});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetGLES2::drawGui(const std::shared_ptr<RootContainer>& root)
    {
        if ((m_targetSize.x == 0) || (m_targetSize.y == 0) || (m_viewRect.width <= 0) || (m_viewRect.height <= 0))
            return;

        // Get some values from the current state so that we can restore them when we are done drawing
        const GLboolean oldBlendEnabled = glIsEnabled(GL_BLEND);
        GLint oldBlendSrc = GL_SRC_ALPHA;
        GLint oldBlendDst = GL_ONE_MINUS_SRC_ALPHA;
        if (oldBlendEnabled)
        {
            TGUI_GL_CHECK(glGetIntegerv(GL_BLEND_SRC_ALPHA, &oldBlendSrc));
            TGUI_GL_CHECK(glGetIntegerv(GL_BLEND_DST_ALPHA, &oldBlendDst));
            if ((oldBlendSrc != GL_SRC_ALPHA) || (oldBlendDst != GL_ONE_MINUS_SRC_ALPHA))
                TGUI_GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        }
        else // Blend was disabled
        {
            TGUI_GL_CHECK(glEnable(GL_BLEND));
            TGUI_GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        }

        const GLboolean oldDepthEnabled = glIsEnabled(GL_DEPTH_TEST);
        if (oldDepthEnabled)
            TGUI_GL_CHECK(glDisable(GL_DEPTH_TEST));

        const GLboolean oldCullingEnabled = glIsEnabled(GL_CULL_FACE);
        if (oldCullingEnabled)
            TGUI_GL_CHECK(glDisable(GL_CULL_FACE));

        GLint oldClipRect[4];
        const GLboolean oldScissorEnabled = glIsEnabled(GL_SCISSOR_TEST);
        if (oldScissorEnabled)
            glGetIntegerv(GL_SCISSOR_BOX, static_cast<GLint*>(oldClipRect));
        else
            glEnable(GL_SCISSOR_TEST);

        GLint oldViewport[4];
        TGUI_GL_CHECK(glGetIntegerv(GL_VIEWPORT, static_cast<GLint*>(oldViewport)));

        m_pixelsPerPoint = {m_viewport.width / m_viewRect.width, m_viewport.height / m_viewRect.height};

        // Change the state that we need while drawing the gui
        const std::array<int, 4> viewportGL = {static_cast<int>(m_viewport.left), static_cast<int>(m_targetSize.y - m_viewport.top - m_viewport.height),
                                               static_cast<int>(m_viewport.width), static_cast<int>(m_viewport.height)};
        TGUI_GL_CHECK(glViewport(viewportGL[0], viewportGL[1], viewportGL[2], viewportGL[3]));
        TGUI_GL_CHECK(glScissor(viewportGL[0], viewportGL[1], viewportGL[2], viewportGL[3]));
        TGUI_GL_CHECK(glUseProgram(m_shaderProgram));
        if (TGUI_GLAD_GL_ES_VERSION_3_0)
            TGUI_GL_CHECK(glBindVertexArray(m_vertexArray));
        TGUI_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer));
        TGUI_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer));

        // Don't make any assumptions about the currently set texture
        m_currentTexture = nullptr;
        TGUI_GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_emptyTexture->getInternalTexture()));

        // Draw the widgets
        root->draw(*this, {});

        m_currentTexture = nullptr;

        // Restore the old state
        if (TGUI_GLAD_GL_ES_VERSION_3_0)
            TGUI_GL_CHECK(glBindVertexArray(0));
        TGUI_GL_CHECK(glUseProgram(0));
        TGUI_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
        TGUI_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        TGUI_GL_CHECK(glViewport(oldViewport[0], oldViewport[1], static_cast<GLsizei>(oldViewport[2]), static_cast<GLsizei>(oldViewport[3])));

        if (oldScissorEnabled)
            TGUI_GL_CHECK(glScissor(oldClipRect[0], oldClipRect[1], oldClipRect[2], oldClipRect[3]));
        else
            TGUI_GL_CHECK(glDisable(GL_SCISSOR_TEST));

        if (oldCullingEnabled)
            TGUI_GL_CHECK(glEnable(GL_CULL_FACE));
        if (oldDepthEnabled)
            TGUI_GL_CHECK(glEnable(GL_DEPTH_TEST));

        if (oldBlendEnabled)
        {
            if ((oldBlendSrc != GL_SRC_ALPHA) || (oldBlendDst != GL_ONE_MINUS_SRC_ALPHA))
                TGUI_GL_CHECK(glBlendFunc(static_cast<GLenum>(oldBlendSrc), static_cast<GLenum>(oldBlendDst)));
        }
        else
            TGUI_GL_CHECK(glDisable(GL_BLEND));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetGLES2::drawVertexArray(const RenderStates& states, const Vertex* vertices,
        std::size_t vertexCount, const unsigned int* indices, std::size_t indexCount, const std::shared_ptr<BackendTexture>& texture)
    {
        // Change the bound texture if it changed
        if (m_currentTexture != texture)
        {
            if (texture)
            {
                TGUI_ASSERT(std::dynamic_pointer_cast<BackendTextureGLES2>(texture), "BackendRenderTargetGLES2 requires textures of type BackendTextureGLES2");
                m_currentTexture = std::static_pointer_cast<BackendTextureGLES2>(texture);

                TGUI_GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_currentTexture->getInternalTexture()));
            }
            else
            {
                m_currentTexture = nullptr;
                TGUI_GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_emptyTexture->getInternalTexture()));
            }
        }

        // Load the data into the vertex buffer. After some experimenting, orphaning the buffer and allocating a new one each time
        // was (suprisingly) faster than creating a larger buffer and only writing to non-overlapping ranges within a frame.
        // Batch rendering (and re-arranging draw calls to be better batchable) would be much faster though.
        TGUI_GL_CHECK(glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertexCount * sizeof(Vertex)), vertices, GL_DYNAMIC_DRAW));

        const Transform finalTransform = m_projectionTransform * states.transform;
        glUniformMatrix4fv(m_projectionMatrixShaderUniformLocation, 1, GL_FALSE, finalTransform.getMatrix().data());

        if (indices)
        {
            // Load the data into the index buffer
            TGUI_GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indexCount * sizeof(GLuint)), indices, GL_STREAM_DRAW));

            TGUI_GL_CHECK(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr));
        }
        else // No indices were given, all vertices need to be drawn in the order they were provided
            TGUI_GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertexCount)));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetGLES2::updateClipping(FloatRect clipRect, FloatRect clipViewport)
    {
        if ((clipViewport.width > 0) && (clipViewport.height > 0) && (clipRect.width > 0) && (clipRect.height > 0))
        {
            m_pixelsPerPoint = {clipViewport.width / clipRect.width, clipViewport.height / clipRect.height};

            TGUI_GL_CHECK(glScissor(static_cast<int>(clipViewport.left), static_cast<int>(m_targetSize.y - clipViewport.top - clipViewport.height),
                                    static_cast<int>(clipViewport.width), static_cast<int>(clipViewport.height)));
        }
        else // Clip the entire window
        {
            m_pixelsPerPoint = {1, 1};

            TGUI_GL_CHECK(glScissor(0, 0, 0, 0));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetGLES2::setVertexAttribs()
    {
        TGUI_GL_CHECK(glEnableVertexAttribArray(m_positionShaderLocation));
        TGUI_GL_CHECK(glEnableVertexAttribArray(m_colorShaderLocation));
        TGUI_GL_CHECK(glEnableVertexAttribArray(m_texCoordShaderLocation));

        // Position is stored as x,y in the first 2 floats
        // Color is stored as r,g,b,a in the next 4 bytes
        // Texture coordinate is stored as u,v in the last 2 floats
        static_assert(sizeof(Vertex) == 8 + 4 + 8, "Size of tgui::Vertex has to match the data");
        TGUI_GL_CHECK(glVertexAttribPointer(m_positionShaderLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(0)));
        TGUI_GL_CHECK(glVertexAttribPointer(m_colorShaderLocation, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), reinterpret_cast<GLvoid*>(8)));
        TGUI_GL_CHECK(glVertexAttribPointer(m_texCoordShaderLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(8 + 4)));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetGLES2::createBuffers()
    {
        if (TGUI_GLAD_GL_ES_VERSION_3_0)
        {
            TGUI_GL_CHECK(glGenVertexArrays(1, &m_vertexArray));
            TGUI_GL_CHECK(glBindVertexArray(m_vertexArray));
        }

        // Create the vertex buffer
        TGUI_GL_CHECK(glGenBuffers(1, &m_vertexBuffer));
        TGUI_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer));

        // Create the index buffer
        TGUI_GL_CHECK(glGenBuffers(1, &m_indexBuffer));
        TGUI_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer));

        setVertexAttribs();

        if (TGUI_GLAD_GL_ES_VERSION_3_0)
            TGUI_GL_CHECK(glBindVertexArray(0));

        TGUI_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
        TGUI_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
