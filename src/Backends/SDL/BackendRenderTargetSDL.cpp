/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Backends/SDL/BackendRenderTargetSDL.hpp>
#include <TGUI/Backends/SDL/BackendTextSDL.hpp>
#include <TGUI/Backends/SDL/BackendTextureSDL.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/OpenGL.hpp>

#include <SDL.h>

#include <algorithm>
#include <numeric>
#include <vector>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    static const GLsizei NrVertexElements = 8; // position (2) + color (4) + texture coordinate (2)

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static GLuint createShaderProgram()
    {
        const GLchar* vertexShaderSource[] =
        {
            "#version 430 core\n"
            "layout(location=0) uniform mat4 projectionMatrix;"
            "layout(location=0) in vec2 inPosition;\n"
            "layout(location=1) in vec4 inColor;\n"
            "layout(location=2) in vec2 inTexCoord;\n"
            "out vec4 color;\n"
            "out vec2 texCoord;\n"
            "void main() {\n"
            "    gl_Position = projectionMatrix * vec4(inPosition.x, inPosition.y, 0, 1);\n"
            "    color = inColor;\n"
            "    texCoord = inTexCoord;\n"
            "}"
        };

        const GLchar* fragmentShaderSource[] =
        {
            "#version 430 core\n"
            "uniform sampler2D uTexture;"
            "in vec4 color;"
            "in vec2 texCoord;"
            "out vec4 outColor;\n"
            "void main() {\n"
            "    outColor = texture(uTexture, texCoord) * color;\n"
            "}"
        };

        // Create the vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        if (vertexShader == 0)
            throw Exception{"Failed to create shaders in BackendRenderTargetSDL. glCreateShader(GL_VERTEX_SHADER) returned 0."};

        TGUI_GL_CHECK(glShaderSource(vertexShader, 1, vertexShaderSource, NULL));
        TGUI_GL_CHECK(glCompileShader(vertexShader));

        GLint vertexShaderCompiled = GL_FALSE;
        TGUI_GL_CHECK(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexShaderCompiled));
        if (vertexShaderCompiled != GL_TRUE)
            throw Exception{"Failed to create shaders in BackendRenderTargetSDL. Failed to compile vertex shader."};

        // Create the fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        if (fragmentShader == 0)
            throw Exception{"Failed to create shaders in BackendRenderTargetSDL. glCreateShader(GL_FRAGMENT_SHADER) returned 0."};

        TGUI_GL_CHECK(glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL));
        TGUI_GL_CHECK(glCompileShader(fragmentShader));

        GLint fragmentShaderCompiled = GL_FALSE;
        TGUI_GL_CHECK(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentShaderCompiled));
        if (fragmentShaderCompiled != GL_TRUE)
            throw Exception{"Failed to create shaders in BackendRenderTargetSDL. Failed to compile fragment shader."};

        // Link the vertex and fragment shader into a program
        GLuint programId = glCreateProgram();
        TGUI_GL_CHECK(glAttachShader(programId, vertexShader));
        TGUI_GL_CHECK(glAttachShader(programId, fragmentShader));
        TGUI_GL_CHECK(glLinkProgram(programId));

        GLint programLinked = GL_TRUE;
        TGUI_GL_CHECK(glGetProgramiv(programId, GL_LINK_STATUS, &programLinked));
        if (programLinked != GL_TRUE)
            throw Exception{"Failed to create shaders in BackendRenderTargetSDL. Failed to link the shaders."};

        return programId;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static void createBuffers(GLuint& vertexArray, GLuint& vertexBuffer, GLuint& indexBuffer)
    {
        TGUI_GL_CHECK(glGenVertexArrays(1, &vertexArray));
        TGUI_GL_CHECK(glBindVertexArray(vertexArray));

        glEnableVertexAttribArray(0); // inPosition
        glEnableVertexAttribArray(1); // inColor
        glEnableVertexAttribArray(2); // inTexCoord

        // Create the vertex buffer
        // Position is stored as x,y in the first 2 floats
        // Color is stored as r,g,b,a in the next 4 floats
        // Texture coordinate is stored as u,v in the last 2 floats
        TGUI_GL_CHECK(glGenBuffers(1, &vertexBuffer));
        TGUI_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer));
        TGUI_GL_CHECK(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, NrVertexElements * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0)));
        TGUI_GL_CHECK(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, NrVertexElements * sizeof(GLfloat), reinterpret_cast<GLvoid*>(2 * sizeof(GLfloat))));
        TGUI_GL_CHECK(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, NrVertexElements * sizeof(GLfloat), reinterpret_cast<GLvoid*>(6 * sizeof(GLfloat))));

        // Create the index buffer
        TGUI_GL_CHECK(glGenBuffers(1, &indexBuffer));
        TGUI_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer));

        TGUI_GL_CHECK(glBindVertexArray(0));

        TGUI_GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
        TGUI_GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendRenderTargetSDL::BackendRenderTargetSDL(SDL_Window* window) :
        m_shaderProgram(createShaderProgram()),
        m_window(window)
    {
        createBuffers(m_vertexArray, m_vertexBuffer, m_indexBuffer);

        // Create a solid white 1x1 texture to pass to the shader when we aren't drawing a texture
        std::uint32_t pixelData = 0xFFFFFFFF;
        TGUI_GL_CHECK(glGenTextures(1, &m_emptyTexture));
        TGUI_GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_emptyTexture));
        TGUI_GL_CHECK(glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1, 1));
        TGUI_GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixelData));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendRenderTargetSDL::~BackendRenderTargetSDL()
    {
        TGUI_GL_CHECK(glDeleteTextures(1, &m_emptyTexture));
        TGUI_GL_CHECK(glDeleteBuffers(1, &m_vertexBuffer));
        TGUI_GL_CHECK(glDeleteBuffers(1, &m_indexBuffer));
        TGUI_GL_CHECK(glDeleteVertexArrays(1, &m_vertexArray));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SDL_Window* BackendRenderTargetSDL::getWindow() const
    {
        return m_window;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSDL::setView(FloatRect view, FloatRect viewport)
    {
        if (!m_window)
            return;

        SDL_GetWindowSize(m_window, &m_windowWidth, &m_windowHeight);

        m_projectionTransform = Transform();
        m_projectionTransform.translate({-1 - (2.f * (view.left / view.width) * (viewport.width / view.width)),
                                         1 + (2.f * (view.top / view.height) * (viewport.height / view.height))});
        m_projectionTransform.scale({2.f / m_windowWidth * (viewport.width / view.width),
                                     -2.f / m_windowHeight * (viewport.height / view.height)});

        m_viewRect = view;
        m_viewport = {static_cast<int>(viewport.left), static_cast<int>(viewport.top),
                      static_cast<int>(viewport.width), static_cast<int>(viewport.height)};

        m_viewportGL = {m_viewport.left, m_windowHeight - m_viewport.top - m_viewport.height, m_viewport.width, m_viewport.height};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSDL::drawGui(const std::shared_ptr<RootContainer>& root)
    {
        if (!m_window)
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
        TGUI_GL_CHECK(glViewport(m_viewportGL[0], m_viewportGL[1], m_viewportGL[2], m_viewportGL[3]));
        TGUI_GL_CHECK(glScissor(m_viewportGL[0], m_viewportGL[1], m_viewportGL[2], m_viewportGL[3]));
        TGUI_GL_CHECK(glUseProgram(m_shaderProgram));
        TGUI_GL_CHECK(glBindVertexArray(m_vertexArray));

        // Don't make any assumptions about the currently set texture
        glBindTexture(GL_TEXTURE_2D, m_emptyTexture);
        m_currentTexture = m_emptyTexture;

        // Draw the widgets
        root->draw(*this, {});

        // Restore the old state
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

    void BackendRenderTargetSDL::drawWidget(const RenderStates& states, const std::shared_ptr<Widget>& widget)
    {
        // If the widget lies outside of the clip rect then we can skip drawing it
        const FloatRect& clipRect = m_clippingLayers.empty() ? m_viewRect : m_clippingLayers.back().first;
        const Vector2f widgetBottomRight{states.transform.transformPoint(widget->getWidgetOffset() + widget->getFullSize())};
        const Vector2f widgetTopLeft = states.transform.transformPoint(widget->getWidgetOffset());
        if ((widgetTopLeft.x > clipRect.left + clipRect.width) || (widgetTopLeft.y > clipRect.top + clipRect.height)
         || (widgetBottomRight.x < clipRect.left) || (widgetBottomRight.y < clipRect.top))
            return;

        widget->draw(*this, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSDL::addClippingLayer(const RenderStates& states, FloatRect rect)
    {
        const FloatRect& oldClipRect = m_clippingLayers.empty() ? m_viewRect : m_clippingLayers.back().first;
        const std::array<int, 4>& oldClipRectGL = m_clippingLayers.empty() ? m_viewportGL : m_clippingLayers.back().second;

        /// TODO: We currently can't clip rotated objects
        const float* transformMatrix = states.transform.getMatrix();
        if ((std::abs(transformMatrix[1]) > 0.00001f) || (std::abs(transformMatrix[4]) > 0.00001f))
        {
            m_clippingLayers.push_back({oldClipRect, oldClipRectGL});
            return;
        }

        const Vector2f bottomRight{states.transform.transformPoint(rect.getPosition() + rect.getSize())};
        const Vector2f topLeft = states.transform.transformPoint(rect.getPosition());

        const int clipLeft = std::max(static_cast<int>(topLeft.x), oldClipRectGL[0]);
        const int clipRight = std::min(static_cast<int>(bottomRight.x), oldClipRectGL[0] + oldClipRectGL[2]);
        const int clipBottom = std::max(static_cast<int>(m_windowHeight - bottomRight.y), oldClipRectGL[1]);
        const int clipTop = std::min(static_cast<int>(m_windowHeight - topLeft.y), oldClipRectGL[1] + oldClipRectGL[3]);

        const FloatRect clipRect = {topLeft, bottomRight - topLeft};
        const std::array<int, 4> clipRectGL = {clipLeft, clipBottom, clipRight - clipLeft, clipTop - clipBottom};
        m_clippingLayers.push_back({clipRect, clipRectGL});

        glScissor(clipRectGL[0], clipRectGL[1], clipRectGL[2], clipRectGL[3]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSDL::removeClippingLayer()
    {
        TGUI_ASSERT(!m_clippingLayers.empty(), "BackendRenderTargetSDL::removeClippingLayer was called when there were no clipping layers");
        m_clippingLayers.pop_back();

        const std::array<int, 4>& clipRectGL = m_clippingLayers.empty() ? m_viewportGL : m_clippingLayers.back().second;
        glScissor(clipRectGL[0], clipRectGL[1], clipRectGL[2], clipRectGL[3]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSDL::drawSprite(const RenderStates& states, const Sprite& sprite)
    {
        if (!sprite.isSet())
            return;

        RenderStates transformedStates = states;
        if (sprite.getRotation() != 0)
        {
            // A rotation can cause the image to be shifted, so we move it upfront so that it ends at the correct location
            transformedStates.transform.translate(-Transform().rotate(sprite.getRotation()).transformRect({{}, sprite.getSize()}).getPosition());
            transformedStates.transform.rotate(sprite.getRotation());
        }

        transformedStates.transform.translate(sprite.getPosition());

        const FloatRect& visibleRect = sprite.getVisibleRect();
        const bool clippingRequired = (visibleRect != FloatRect{});
        if (clippingRequired)
            addClippingLayer(transformedStates, {{visibleRect.left, visibleRect.top}, {visibleRect.width, visibleRect.height}});

        std::shared_ptr<BackendTextureSDL> backendTexture;
        if (sprite.getTexture().getData()->svgImage)
        {
            TGUI_ASSERT(std::dynamic_pointer_cast<BackendTextureSDL>(sprite.getSvgTexture()), "BackendRenderTargetSDL::drawSprite requires SVG texture of type BackendTextureSDL");
            backendTexture = std::static_pointer_cast<BackendTextureSDL>(sprite.getSvgTexture());
        }
        else
        {
            TGUI_ASSERT(std::dynamic_pointer_cast<BackendTextureSDL>(sprite.getTexture().getData()->backendTexture),
                        "BackendRenderTargetSDL::drawSprite requires backend texture of type BackendTextureSDL");
            backendTexture = std::static_pointer_cast<BackendTextureSDL>(sprite.getTexture().getData()->backendTexture);
        }

        changeTexture(backendTexture->getInternalTexture());
        prepareVerticesAndIndices(sprite);
        updateTransformation(transformedStates.transform);

        TGUI_GL_CHECK(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(sprite.getIndices().size()), GL_UNSIGNED_INT, NULL));

        if (clippingRequired)
            removeClippingLayer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSDL::drawText(const RenderStates& states, const Text& text)
    {
        RenderStates movedStates = states;
        movedStates.transform.translate(text.getPosition());
        updateTransformation(movedStates.transform);

        TGUI_ASSERT(std::dynamic_pointer_cast<BackendTextSDL>(text.getBackendText()), "BackendRenderTargetSDL::drawText requires backend text of type BackendTextSDL");

        if (text.getOutlineThickness() != 0)
        {
            const auto& linesOutline = std::static_pointer_cast<BackendTextSDL>(text.getBackendText())->getInternalTextures();
            for (const auto& line : linesOutline)
                drawTextLine(line.bounding, line.textureId);
        }

        const auto& lines = std::static_pointer_cast<BackendTextSDL>(text.getBackendText())->getInternalTextures();
        for (const auto& line : lines)
            drawTextLine(line.bounding, line.textureId);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSDL::drawTriangles(const RenderStates& states, const Vertex* vertices, std::size_t vertexCount, const int* indices, std::size_t indexCount)
    {
        if (!indices)
            indexCount = vertexCount;

        changeTexture(m_emptyTexture);
        prepareVerticesAndIndices(vertices, vertexCount, indices, indexCount);
        updateTransformation(states.transform);

        TGUI_GL_CHECK(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, NULL));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSDL::drawCircle(const RenderStates& states, float size, const Color& backgroundColor, float borderThickness, const Color& borderColor)
    {
        changeTexture(m_emptyTexture);

        const float radius = size / 2.f;
        if (borderThickness > 0)
        {
            drawCircleHelper(states, radius + borderThickness, borderColor, -borderThickness);
            drawCircleHelper(states, radius, backgroundColor, 0);
        }
        else if (borderThickness < 0)
        {
            drawCircleHelper(states, radius, borderColor, 0);
            drawCircleHelper(states, radius + borderThickness, backgroundColor, -borderThickness);
        }
        else // No outline
            drawCircleHelper(states, radius, backgroundColor, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSDL::prepareVerticesAndIndices(const Sprite& sprite)
    {
        const std::vector<tgui::Vertex>& vertices = sprite.getVertices();
        const std::vector<int>& indices = sprite.getIndices();

        Vector2u textureSize;
        if (sprite.getTexture().getData()->svgImage)
            textureSize = sprite.getSvgTexture()->getSize();
        else
            textureSize = sprite.getTexture().getData()->backendTexture->getSize();

        prepareVerticesAndIndices(vertices.data(), vertices.size(), indices.data(), indices.size(), textureSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSDL::prepareVerticesAndIndices(const Vertex* vertices, std::size_t vertexCount, const int* indices, std::size_t indexCount, Vector2u textureSize)
    {
        // Construct the array of vertices
        std::vector<GLfloat> vertexData;
        vertexData.reserve(NrVertexElements * vertexCount);
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
        auto indexData = std::make_unique<GLuint[]>(indexCount);
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
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSDL::changeTexture(GLuint textureId)
    {
        if (m_currentTexture == textureId)
            return;

        glBindTexture(GL_TEXTURE_2D, textureId);
        m_currentTexture = textureId;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSDL::updateTransformation(const Transform& transform)
    {
        glUniformMatrix4fv(0, 1, GL_FALSE, (m_projectionTransform * transform).getMatrix());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSDL::drawTextLine(const SDL_Rect& bounding, GLuint textureId)
    {
        const std::array<Vertex, 4> vertices = {{
            {
                {static_cast<float>(bounding.x), static_cast<float>(bounding.y)},
                {255, 255, 255, 255},
                {0, 0}
            },
            {
                {static_cast<float>(bounding.x) + bounding.w, static_cast<float>(bounding.y)},
                {255, 255, 255, 255},
                {static_cast<float>(bounding.w), 0}
            },
            {
                {static_cast<float>(bounding.x), static_cast<float>(bounding.y + bounding.h)},
                {255, 255, 255, 255},
                {0, static_cast<float>(bounding.h)}
            },
            {
                {static_cast<float>(bounding.x + bounding.w), static_cast<float>(bounding.y + bounding.h)},
                {255, 255, 255, 255},
                {static_cast<float>(bounding.w), static_cast<float>(bounding.h)}
            },
        }};
        const std::array<int, 6> indices = {{
            0, 2, 1,
            1, 2, 3
        }};

        changeTexture(textureId);
        prepareVerticesAndIndices(vertices.data(), vertices.size(), indices.data(), indices.size(),
                                  {static_cast<unsigned int>(bounding.w), static_cast<unsigned int>(bounding.h)});

        TGUI_GL_CHECK(glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, NULL));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSDL::drawCircleHelper(const RenderStates& states, float radius, const Color& color, float offset)
    {
        if (offset != 0)
        {
            RenderStates movedStates = states;
            movedStates.transform.translate({offset, offset});
            updateTransformation(movedStates.transform);
        }
        else
            updateTransformation(states.transform);

        const int nrPoints = 1 + static_cast<int>(std::ceil(radius * 4));
        const Vector2f center = {radius, radius};

        std::vector<Vertex> vertices(nrPoints, {{}, Vertex::Color(color), {0, 0}});
        vertices[0].position = center;

        const float pi = 3.14159265358979f;
        const float twoPi = 2.f * pi;
        const int nrTriangles = nrPoints - 2;
        for (int i = 0; i <= nrTriangles; ++i)
        {
            vertices[i].position = {center.x + (radius * std::cos(twoPi * i / nrTriangles)),
                                    center.y + (radius * std::sin(twoPi * i / nrTriangles))};
        }

        std::vector<int> indices(nrPoints);
        for (int i = 0; i < nrPoints; ++i)
            indices[i] = i;

        prepareVerticesAndIndices(vertices.data(), vertices.size(), indices.data(), indices.size());
        TGUI_GL_CHECK(glDrawElements(GL_TRIANGLE_FAN, nrPoints, GL_UNSIGNED_INT, NULL));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
