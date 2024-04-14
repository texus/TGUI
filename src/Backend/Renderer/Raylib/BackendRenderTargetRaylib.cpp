/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/Backend/Renderer/Raylib/BackendRenderTargetRaylib.hpp>

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/Backend/Renderer/BackendText.hpp>
    #include <TGUI/Backend/Window/Backend.hpp>
    #include <TGUI/Container.hpp>
#endif

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <numeric>
#endif

#include <rlgl.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    BackendRenderTargetRaylib::~BackendRenderTargetRaylib()
    {
        SetMaterialTexture(&m_material, MATERIAL_MAP_DIFFUSE, m_defaultTexture);
        UnloadMaterial(m_material);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetRaylib::setClearColor(const Color& color)
    {
        m_clearColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetRaylib::clearScreen()
    {
        ClearBackground({m_clearColor.getRed(), m_clearColor.getGreen(), m_clearColor.getBlue(), m_clearColor.getAlpha()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetRaylib::setView(FloatRect view, FloatRect viewport, Vector2f targetSize)
    {
        BackendRenderTarget::setView(view, viewport, targetSize);

        m_projectionTransform = Transform();
        m_projectionTransform.translate({(-view.left / view.width) * viewport.width, (-view.top / view.height) * viewport.height});
        m_projectionTransform.translate({viewport.left, viewport.top});
        m_projectionTransform.scale({viewport.width / view.width, viewport.height / view.height});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetRaylib::drawGui(const std::shared_ptr<RootContainer>& root)
    {
        if ((m_targetSize.x == 0) || (m_targetSize.y == 0) || (m_viewRect.width <= 0) || (m_viewRect.height <= 0))
            return;

        rlDisableBackfaceCulling();
        BeginBlendMode(BLEND_ALPHA);
        BeginScissorMode(static_cast<int>(m_viewport.left), static_cast<int>(m_viewport.top), static_cast<int>(m_viewport.width), static_cast<int>(m_viewport.height));

        m_pixelsPerPoint = {m_viewport.width / m_viewRect.width, m_viewport.height / m_viewRect.height};

        // Draw the widgets
        root->draw(*this, {});

        // Raylib provides not way to query the modes, so we reset values that TGUI changed to a reasonably default
        EndScissorMode();
        EndBlendMode();
        rlEnableBackfaceCulling();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetRaylib::drawVertexArray(const RenderStates& states, const Vertex* vertices,
        std::size_t vertexCount, const unsigned int* indices, std::size_t indexCount, const std::shared_ptr<BackendTexture>& texture)
    {
        if (vertexCount == 0)
            return;

        Mesh mesh = {};
        mesh.vertexCount = static_cast<int>(vertexCount);

        auto convertedVertices = MakeUniqueForOverwrite<float[]>(vertexCount * 3);
        auto convertedVerticesIt = convertedVertices.get();
        for (std::size_t i = 0; i < vertexCount; ++i)
        {
            *(convertedVerticesIt++) = vertices[i].position.x;
            *(convertedVerticesIt++) = vertices[i].position.y;
            *(convertedVerticesIt++) = 0;
        }
        mesh.vertices = convertedVertices.get();

        auto convertedColors = MakeUniqueForOverwrite<unsigned char[]>(vertexCount * 4);
        auto convertedColorsIt = convertedColors.get();
        for (std::size_t i = 0; i < vertexCount; ++i)
        {
            *(convertedColorsIt++) = vertices[i].color.red;
            *(convertedColorsIt++) = vertices[i].color.green;
            *(convertedColorsIt++) = vertices[i].color.blue;
            *(convertedColorsIt++) = vertices[i].color.alpha;
        }
        mesh.colors = convertedColors.get();

        auto convertedTexCoords = MakeUniqueForOverwrite<float[]>(vertexCount * 2);
        auto convertedTexCoordsIt = convertedTexCoords.get();
        for (std::size_t i = 0; i < vertexCount; ++i)
        {
            *(convertedTexCoordsIt++) = vertices[i].texCoords.x;
            *(convertedTexCoordsIt++) = vertices[i].texCoords.y;
        }
        mesh.texcoords = convertedTexCoords.get();

        std::unique_ptr<unsigned short[]> converedIndices;
        if (indices)
        {
            converedIndices = std::make_unique<unsigned short[]>(indexCount);
            for (std::size_t i = 0; i < indexCount; ++i)
                converedIndices[i] = static_cast<unsigned short>(indices[i]);

            mesh.triangleCount = static_cast<int>(indexCount / 3);
            mesh.indices = converedIndices.get();
        }
        else // There are no indices
        {
            mesh.triangleCount = static_cast<int>(vertexCount / 3);
            mesh.indices = nullptr;
        }

        UploadMesh(&mesh, false);

        const Transform finalTransform = m_projectionTransform * states.transform;
        const float* transformValues = finalTransform.getMatrix().data();
        Matrix convertedTransform = {
            transformValues[0], transformValues[4], transformValues[8], transformValues[12],
            transformValues[1], transformValues[5], transformValues[9], transformValues[13],
            transformValues[2], transformValues[6], transformValues[10], transformValues[14],
            transformValues[3], transformValues[7], transformValues[11], transformValues[15]
        };

        if (texture)
        {
            TGUI_ASSERT(std::dynamic_pointer_cast<BackendTextureRaylib>(texture), "BackendRenderTargetRaylib requires textures of type BackendTextureRaylib");
            const Texture2D& textureRaylib = std::static_pointer_cast<BackendTextureRaylib>(texture)->getInternalTexture();

            SetMaterialTexture(&m_material, MATERIAL_MAP_DIFFUSE, textureRaylib);
        }
        else
            SetMaterialTexture(&m_material, MATERIAL_MAP_DIFFUSE, m_defaultTexture);

        DrawMesh(mesh, m_material, convertedTransform);

        mesh.texcoords = nullptr;
        mesh.vertices = nullptr;
        mesh.indices = nullptr;
        mesh.colors = nullptr;
        UnloadMesh(mesh);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetRaylib::updateClipping(FloatRect clipRect, FloatRect clipViewport)
    {
        if ((clipViewport.width > 0) && (clipViewport.height > 0) && (clipRect.width > 0) && (clipRect.height > 0))
        {
            m_pixelsPerPoint = {clipViewport.width / clipRect.width, clipViewport.height / clipRect.height};

            BeginScissorMode(static_cast<int>(clipViewport.left), static_cast<int>(clipViewport.top), static_cast<int>(clipViewport.width), static_cast<int>(clipViewport.height));
        }
        else // Clip the entire window
        {
            m_pixelsPerPoint = {1, 1};

            BeginScissorMode(0, 0, 0, 0);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
