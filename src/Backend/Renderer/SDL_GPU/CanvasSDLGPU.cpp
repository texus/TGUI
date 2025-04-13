/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2025 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/Backend/Renderer/SDL_GPU/CanvasSDLGPU.hpp>
#include <TGUI/Backend/Renderer/SDL_GPU/BackendRendererSDLGPU.hpp>

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/Backend/Window/Backend.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char CanvasSDLGPU::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSDLGPU::CanvasSDLGPU(const char* typeName, bool initRenderer) :
        CanvasBase{typeName, initRenderer}
    {
        TGUI_ASSERT(isBackendSet() && getBackend()->hasRenderer() && std::dynamic_pointer_cast<BackendRendererSDLGPU>(getBackend()->getRenderer()),
                    "CanvasSDLGPU can only be created when using the SDL_GPU backend renderer");

        SDL_GPUDevice* sdlDevice = std::static_pointer_cast<BackendRendererSDLGPU>(getBackend()->getRenderer())->getInternalDevice();
        m_backendTexture = std::make_shared<BackendTextureSDLGPU>(sdlDevice, SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSDLGPU::CanvasSDLGPU(const CanvasSDLGPU& other) :
        CanvasBase{other}
    {
        TGUI_ASSERT(isBackendSet() && getBackend()->hasRenderer() && std::dynamic_pointer_cast<BackendRendererSDLGPU>(getBackend()->getRenderer()),
                    "CanvasSDLGPU can only be used when using the SDL_GPU backend renderer");

        SDL_GPUDevice* sdlDevice = std::static_pointer_cast<BackendRendererSDLGPU>(getBackend()->getRenderer())->getInternalDevice();
        m_backendTexture = std::make_shared<BackendTextureSDLGPU>(sdlDevice,  SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET);

        setSize(other.getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSDLGPU& CanvasSDLGPU::operator= (const CanvasSDLGPU& right)
    {
        if (this != &right)
        {
            ClickableWidget::operator=(right);
            setSize(right.getSize());
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSDLGPU::Ptr CanvasSDLGPU::create(const Layout2d& size)
    {
        auto canvas = std::make_shared<CanvasSDLGPU>();
        canvas->setSize(size);
        return canvas;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CanvasSDLGPU::Ptr CanvasSDLGPU::copy(const CanvasSDLGPU::ConstPtr& canvas)
    {
        if (canvas)
            return std::static_pointer_cast<CanvasSDLGPU>(canvas->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasSDLGPU::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        const Vector2f newSize = getSize();

        if ((newSize.x > 0) && (newSize.y > 0))
        {
            const Vector2u newTextureSize{newSize};
            if ((m_textureSize.x != newTextureSize.x) || (m_textureSize.y != newTextureSize.y))
            {
                TGUI_ASSERT(isBackendSet() && getBackend()->hasRenderer() && std::dynamic_pointer_cast<BackendRendererSDLGPU>(getBackend()->getRenderer()),
                    "CanvasSDLGPU can only be used when using the SDL_GPU backend renderer");
                SDL_GPUDevice* device = std::static_pointer_cast<BackendRendererSDLGPU>(getBackend()->getRenderer())->getInternalDevice();

                SDL_GPUTextureCreateInfo textureInfo = {};
                textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
                textureInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
                textureInfo.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;
                textureInfo.width = newTextureSize.x;
                textureInfo.height = newTextureSize.y;
                textureInfo.layer_count_or_depth = 1;
                textureInfo.num_levels = 1;
                SDL_GPUTexture* texture = SDL_CreateGPUTexture(device, &textureInfo);

                // Move the ownership of the texture to our backend texture (we thus don't need to call SDL_ReleaseGPUTexture ourselves)
                m_backendTexture->replaceInternalTexture(texture, newTextureSize);

                if (texture)
                    m_textureSize = newTextureSize;
                else
                    m_textureSize = {};
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CanvasSDLGPU::draw(BackendRenderTarget& target, RenderStates states) const
    {
        const Vector2f size = getSize();
        if ((size.x <= 0) || (size.y <= 0) || (m_textureSize.x <= 0) || (m_textureSize.y <= 0))
            return;

        const Vertex::Color vertexColor(Color::applyOpacity(Color::White, m_opacityCached));
        const std::array<Vertex, 4> vertices = {{
            {{0, 0}, vertexColor, {0, 0}},
            {{size.x, 0}, vertexColor, {1, 0}},
            {{0, size.y}, vertexColor, {0, 1}},
            {{size.x, size.y}, vertexColor, {1, 1}},
        }};
        const std::array<unsigned int, 6> indices = {{
            0, 2, 1,
            1, 2, 3
        }};
        target.drawVertexArray(states, vertices.data(), vertices.size(), indices.data(), indices.size(), m_backendTexture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr CanvasSDLGPU::clone() const
    {
        return std::make_shared<CanvasSDLGPU>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
