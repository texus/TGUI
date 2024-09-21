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

#include <TGUI/extlibs/IncludeSDL.hpp>
#include <TGUI/Backend/Renderer/SDL_Renderer/BackendRenderTargetSDL.hpp>

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/Backend/Renderer/BackendText.hpp>
    #include <TGUI/Backend/Window/Backend.hpp>
    #include <TGUI/Container.hpp>
#endif

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <numeric>
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    BackendRenderTargetSDL::BackendRenderTargetSDL(SDL_Renderer* renderer) :
        m_renderer{renderer}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSDL::setClearColor(const Color& color)
    {
        SDL_SetRenderDrawColor(m_renderer, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSDL::clearScreen()
    {
        SDL_RenderClear(m_renderer);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSDL::setView(FloatRect view, FloatRect viewport, Vector2f targetSize)
    {
        BackendRenderTarget::setView(view, viewport, targetSize);

        m_projectionTransform = Transform();
        m_projectionTransform.translate({(-view.left / view.width) * viewport.width, (-view.top / view.height) * viewport.height});
        m_projectionTransform.scale({viewport.width / view.width, viewport.height / view.height});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSDL::drawGui(const std::shared_ptr<RootContainer>& root)
    {
        if (!m_renderer || (m_targetSize.x == 0) || (m_targetSize.y == 0) || (m_viewRect.width <= 0) || (m_viewRect.height <= 0))
            return;

        // Store the current blend mode, in case we need to change it
        SDL_BlendMode oldBlendMode = SDL_BLENDMODE_BLEND;
        SDL_GetRenderDrawBlendMode(m_renderer, &oldBlendMode);

        // Store the current clipping settings, in case we need to change it
        SDL_Rect oldClipRect;
        const bool oldClipEnabled = SDL_RenderClipEnabled(m_renderer);
        if (oldClipEnabled)
            SDL_GetRenderClipRect(m_renderer, &oldClipRect);

        // Store the current viewport, in case we need to change it
        SDL_Rect oldViewport;
        SDL_GetRenderViewport(m_renderer, &oldViewport);

        SDL_Rect newViewport;
        newViewport.x = static_cast<int>(std::floor(m_viewport.left));
        newViewport.y = static_cast<int>(std::floor(m_viewport.top));
        newViewport.w = static_cast<int>(std::ceil(m_viewport.width));
        newViewport.h = static_cast<int>(std::ceil(m_viewport.height));

        // Change the viewport if needed
        const bool viewportNeedsUpdate = (oldViewport.x != newViewport.x) || (oldViewport.y != newViewport.y)
                                      || (oldViewport.w != newViewport.w) || (oldViewport.h != newViewport.h);
        if (viewportNeedsUpdate)
            SDL_SetRenderViewport(m_renderer, &newViewport);

        m_pixelsPerPoint = {m_viewport.width / m_viewRect.width, m_viewport.height / m_viewRect.height};

        // Blend mode has to be SDL_BLENDMODE_BLEND in order to use transparency for colored objects that don't use a texture
        if (oldBlendMode != SDL_BLENDMODE_BLEND)
            SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

        // Draw the widgets
        root->draw(*this, {});

        // Restore the old blend mode
        if (oldBlendMode != SDL_BLENDMODE_BLEND)
            SDL_SetRenderDrawBlendMode(m_renderer, oldBlendMode);

        // Restore the old viewport
        if (viewportNeedsUpdate)
            SDL_SetRenderViewport(m_renderer, &oldViewport);

        // Restore the old clipping rect
        if (oldClipEnabled)
            SDL_SetRenderClipRect(m_renderer, &oldClipRect);
        else
            SDL_SetRenderClipRect(m_renderer, nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSDL::drawVertexArray(const RenderStates& states, const Vertex* vertices,
        std::size_t vertexCount, const unsigned int* indices, std::size_t indexCount, const std::shared_ptr<BackendTexture>& texture)
    {
        if (vertexCount == 0)
            return;

        SDL_Texture* textureSDL = nullptr;
        if (texture)
        {
            TGUI_ASSERT(std::dynamic_pointer_cast<BackendTextureSDL>(texture), "BackendRenderTargetSDL requires textures of type BackendTextureSDL");
            textureSDL = std::static_pointer_cast<BackendTextureSDL>(texture)->getInternalTexture();
        }

        const Transform finalTransform = m_projectionTransform * states.transform;
#if SDL_MAJOR_VERSION >= 3
        std::vector<SDL_Vertex> verticesSDL;
        verticesSDL.reserve(vertexCount);
        for (std::size_t i = 0; i < vertexCount; ++i)
        {
            const Vertex& vertex = vertices[i];
            const Vector2f transformedPosition = finalTransform.transformPoint(vertex.position);
            verticesSDL.push_back({SDL_FPoint{transformedPosition.x, transformedPosition.y},
                                   SDL_FColor{vertex.color.red / 255.f, vertex.color.green / 255.f, vertex.color.blue / 255.f, vertex.color.alpha / 255.f},
                                   SDL_FPoint{vertex.texCoords.x, vertex.texCoords.y}});
        }

        SDL_RenderGeometryRaw(m_renderer, textureSDL,
                              &verticesSDL.data()->position.x, sizeof(SDL_Vertex),
                              &verticesSDL.data()->color, sizeof(SDL_Vertex),
                              &verticesSDL.data()->tex_coord.x, sizeof(SDL_Vertex),
                              static_cast<int>(vertexCount), indices, static_cast<int>(indexCount), sizeof(unsigned int));
#else
        std::vector<Vertex> verticesSDL(vertices, vertices + vertexCount);
        for (std::size_t i = 0; i < vertexCount; ++i)
        {
            const Vector2f transformedPosition = finalTransform.transformPoint(vertices[i].position);
            verticesSDL[i].position.x = transformedPosition.x;
            verticesSDL[i].position.y = transformedPosition.y;
        }

        // We use SDL_RenderGeometry instead of SDL_RenderGeometryRaw because it's easier and because the signature of
        // the SDL_RenderGeometryRaw function is different in SDL 2.0.18 and SDL >= 2.0.20
        static_assert(sizeof(int) == sizeof(unsigned int), "Size of 'int' and 'unsigned int' must be identical for cast to work");
        static_assert(sizeof(Vertex) == sizeof(SDL_Vertex), "SDL_Vertex requires same memory layout as tgui::Vertex for cast to work");
        SDL_RenderGeometry(m_renderer, textureSDL,
                           reinterpret_cast<const SDL_Vertex*>(verticesSDL.data()), static_cast<int>(vertexCount),
                           reinterpret_cast<const int*>(indices), static_cast<int>(indexCount));
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetSDL::updateClipping(FloatRect clipRect, FloatRect clipViewport)
    {
        if ((clipViewport.width > 0) && (clipViewport.height > 0) && (clipRect.width > 0) && (clipRect.height > 0))
        {
            m_pixelsPerPoint = {clipViewport.width / clipRect.width, clipViewport.height / clipRect.height};

            SDL_Rect clipRectSDL;
            clipRectSDL.x = static_cast<int>(std::floor(clipViewport.left - m_viewport.left));
            clipRectSDL.y = static_cast<int>(std::floor(clipViewport.top - m_viewport.top));
            clipRectSDL.w = static_cast<int>(std::ceil(clipViewport.width));
            clipRectSDL.h = static_cast<int>(std::ceil(clipViewport.height));
            SDL_SetRenderClipRect(m_renderer, &clipRectSDL);
        }
        else // Clip the entire window
        {
            m_pixelsPerPoint = {1, 1};

            SDL_SetRenderClipRect(m_renderer, nullptr);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
