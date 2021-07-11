/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2021 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Backend/Renderer/BackendText.hpp>
#include <TGUI/Backend/Renderer/OpenGL.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/Backend/Window/Backend.hpp>
#include <numeric>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    static const GLsizei NrVertexElements = 8; // position (2) + color (4) + texture coordinate (2)

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static GLuint createShaderProgram()
    {
        // Select the vertex shader based on which GLES version is available
        const GLchar* vertexShaderSource;
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
        }

        // Select the fragment shader based on which GLES version is available
        const GLchar* fragmentShaderSource;
        if (TGUI_GLAD_GL_ES_VERSION_3_0)
        {
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
        else // No GLES 3 support
        {
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

        // Create the vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        if (vertexShader == 0)
            throw Exception{"Failed to create shaders in BackendRenderTargetGLES2. glCreateShader(GL_VERTEX_SHADER) returned 0."};

        TGUI_GL_CHECK(glShaderSource(vertexShader, 1, &vertexShaderSource, NULL));
        TGUI_GL_CHECK(glCompileShader(vertexShader));

        GLint vertexShaderCompiled = GL_FALSE;
        TGUI_GL_CHECK(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexShaderCompiled));
        if (vertexShaderCompiled != GL_TRUE)
            throw Exception{"Failed to create shaders in BackendRenderTargetGLES2. Failed to compile vertex shader."};

        // Create the fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        if (fragmentShader == 0)
            throw Exception{"Failed to create shaders in BackendRenderTargetGLES2. glCreateShader(GL_FRAGMENT_SHADER) returned 0."};

        TGUI_GL_CHECK(glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL));
        TGUI_GL_CHECK(glCompileShader(fragmentShader));

        GLint fragmentShaderCompiled = GL_FALSE;
        TGUI_GL_CHECK(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentShaderCompiled));
        if (fragmentShaderCompiled != GL_TRUE)
            throw Exception{"Failed to create shaders in BackendRenderTargetGLES2. Failed to compile fragment shader."};

        // Link the vertex and fragment shader into a program
        GLuint programId = glCreateProgram();
        TGUI_GL_CHECK(glAttachShader(programId, vertexShader));
        TGUI_GL_CHECK(glAttachShader(programId, fragmentShader));
        TGUI_GL_CHECK(glLinkProgram(programId));

        GLint programLinked = GL_TRUE;
        TGUI_GL_CHECK(glGetProgramiv(programId, GL_LINK_STATUS, &programLinked));
        if (programLinked != GL_TRUE)
            throw Exception{"Failed to create shaders in BackendRenderTargetGLES2. Failed to link the shaders."};

        return programId;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendRenderTargetGLES2::BackendRenderTargetGLES2() :
        m_shaderProgram(createShaderProgram())
    {
        TGUI_ASSERT(getBackend(), "BackendRenderTargetGLES2 can't be created when there is no system backend initialized (was a gui created yet?)");
        TGUI_ASSERT(getBackend()->getRenderer(), "BackendRenderTargetGLES2 can't be created when there is no backend renderer (was a gui attached to a window yet?)");

        // If our OpenGL version didn't support the layout qualifier in GLSL then we need to query the location
        if (!TGUI_GLAD_GL_ES_VERSION_3_1)
            m_projectionMatrixShaderUniformLocation = glGetUniformLocation(m_shaderProgram, "projectionMatrix");

        if (!TGUI_GLAD_GL_ES_VERSION_3_0)
        {
            m_positionShaderLocation = glGetAttribLocation(m_shaderProgram, "inPosition");
            m_colorShaderLocation = glGetAttribLocation(m_shaderProgram, "inColor");
            m_texCoordShaderLocation = glGetAttribLocation(m_shaderProgram, "inTexCoord");
        }

        createBuffers();

        // Create a solid white 1x1 texture to pass to the shader when we aren't drawing a texture
        m_emptyTexture = std::make_unique<tgui::BackendTextureGLES2>();
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
        m_projectionTransform.scale({2.f / (m_targetSize.x * (viewport.width / m_targetSize.x)) * (viewport.width / view.width),
                                     -2.f / (m_targetSize.y * (viewport.height / m_targetSize.y)) * (viewport.height / view.height)});
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

        GLint oldClipRect[4];
        const GLboolean oldScissorEnabled = glIsEnabled(GL_SCISSOR_TEST);
        if (oldScissorEnabled)
            glGetIntegerv(GL_SCISSOR_BOX, oldClipRect);
        else
            glEnable(GL_SCISSOR_TEST);

        GLint oldViewport[4];
        TGUI_GL_CHECK(glGetIntegerv(GL_VIEWPORT, oldViewport));

        // Change the state that we need while drawing the gui
        const std::array<int, 4> viewportGL = {static_cast<int>(m_viewport.left), static_cast<int>(m_targetSize.y - m_viewport.top - m_viewport.height),
                                               static_cast<int>(m_viewport.width), static_cast<int>(m_viewport.height)};
        TGUI_GL_CHECK(glViewport(viewportGL[0], viewportGL[1], viewportGL[2], viewportGL[3]));
        TGUI_GL_CHECK(glScissor(viewportGL[0], viewportGL[1], viewportGL[2], viewportGL[3]));
        TGUI_GL_CHECK(glUseProgram(m_shaderProgram));
        if (TGUI_GLAD_GL_ES_VERSION_3_0)
            TGUI_GL_CHECK(glBindVertexArray(m_vertexArray));

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
        TGUI_GL_CHECK(glViewport(oldViewport[0], oldViewport[1], static_cast<GLsizei>(oldViewport[2]), static_cast<GLsizei>(oldViewport[3])));

        if (oldScissorEnabled)
            TGUI_GL_CHECK(glScissor(oldClipRect[0], oldClipRect[1], oldClipRect[2], oldClipRect[3]));
        else
            TGUI_GL_CHECK(glDisable(GL_SCISSOR_TEST));

        if (oldBlendEnabled)
        {
            if ((oldBlendSrc != GL_SRC_ALPHA) || (oldBlendDst != GL_ONE_MINUS_SRC_ALPHA))
                TGUI_GL_CHECK(glBlendFunc(oldBlendSrc, oldBlendDst));
        }
        else
            TGUI_GL_CHECK(glDisable(GL_BLEND));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetGLES2::drawVertexArray(const RenderStates& states, const Vertex* vertices, std::size_t vertexCount,
                                                     const int* indices, std::size_t indexCount, const std::shared_ptr<BackendTexture>& texture)
    {
        if (!indices)
            indexCount = vertexCount;

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

        /// TODO: We would be able to use the "verices" member directly (or bulk-memcpy when batching) if we didn't have to rescale here.
        ///       Color should be passed as GL_UNSIGNED_BYTE instead of GL_FLOAT (which also reduces vertex data size).
        ///       Texture coordinates would need to be stored as normalized (but this prevents memcpy in SFML backend). SFML seems to use legacy texture matrix to auto-convert coords.
        // Construct the array of vertices
        std::vector<GLfloat> vertexData;
        vertexData.reserve(NrVertexElements * vertexCount);
        const Vector2f textureSize = texture ? Vector2f{texture->getSize()} : Vector2f{1,1};
        for (std::size_t i = 0; i < vertexCount; ++i)
        {
            const auto& vertex = vertices[i];
            vertexData.push_back(vertex.position.x);
            vertexData.push_back(vertex.position.y);
            vertexData.push_back(vertex.color.red / 255.f);
            vertexData.push_back(vertex.color.green / 255.f);
            vertexData.push_back(vertex.color.blue / 255.f);
            vertexData.push_back(vertex.color.alpha / 255.f);
            vertexData.push_back(vertex.texCoords.x / textureSize.x);
            vertexData.push_back(vertex.texCoords.y / textureSize.y);
        }

        // Create the array of indices
        auto indexData = MakeUniqueForOverwrite<GLuint[]>(indexCount);
        if (indices)
        {
            for (std::size_t i = 0; i < indexCount; ++i)
                indexData[i] = static_cast<GLuint>(indices[i]);
        }
        else // Generate index buffer with sequential values 0, 1, 2, 3, ...
            std::iota(&indexData[0], &indexData[0] + indexCount, 0);

        // Load the data into the vertex buffer
        TGUI_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer));
        if (vertexData.size() > m_vertexBufferSize)
        {
            TGUI_GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(GLfloat), vertexData.data(), GL_STREAM_DRAW));
            m_vertexBufferSize = vertexData.size();
        }
        else
            TGUI_GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, vertexData.size() * sizeof(GLfloat), vertexData.data()));

        // Load the data into the index buffer
        TGUI_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer));
        if (indexCount > m_indexBufferSize)
        {
            TGUI_GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLuint), indexData.get(), GL_STREAM_DRAW));
            m_indexBufferSize = indexCount;
        }
        else
            TGUI_GL_CHECK(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexCount * sizeof(GLuint), indexData.get()));

        if (!TGUI_GLAD_GL_ES_VERSION_3_0)
            setVertexAttribs();

        Transform finalTransform = states.transform;
        finalTransform.roundPosition(); // Avoid blurry texts
        finalTransform = m_projectionTransform * finalTransform;

        glUniformMatrix4fv(m_projectionMatrixShaderUniformLocation, 1, GL_FALSE, finalTransform.getMatrix());

        TGUI_GL_CHECK(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, NULL));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetGLES2::updateClipping(FloatRect, FloatRect clipViewport)
    {
        if ((clipViewport.width > 0) && (clipViewport.height > 0))
        {
            TGUI_GL_CHECK(glScissor(static_cast<int>(clipViewport.left), static_cast<int>(m_targetSize.y - clipViewport.top - clipViewport.height),
                                    static_cast<int>(clipViewport.width), static_cast<int>(clipViewport.height)));
        }
        else // Clip the entire window
        {
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
        // Color is stored as r,g,b,a in the next 4 floats
        // Texture coordinate is stored as u,v in the last 2 floats
        TGUI_GL_CHECK(glVertexAttribPointer(m_positionShaderLocation, 2, GL_FLOAT, GL_FALSE, NrVertexElements * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0)));
        TGUI_GL_CHECK(glVertexAttribPointer(m_colorShaderLocation, 4, GL_FLOAT, GL_FALSE, NrVertexElements * sizeof(GLfloat), reinterpret_cast<GLvoid*>(2 * sizeof(GLfloat))));
        TGUI_GL_CHECK(glVertexAttribPointer(m_texCoordShaderLocation, 2, GL_FLOAT, GL_FALSE, NrVertexElements * sizeof(GLfloat), reinterpret_cast<GLvoid*>(6 * sizeof(GLfloat))));
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
