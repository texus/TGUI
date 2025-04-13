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

#include <TGUI/extlibs/IncludeSDL.hpp>
#include <TGUI/Backend/Renderer/SDL_GPU/BackendRendererSDLGPU.hpp>
#include <TGUI/Backend/Renderer/SDL_GPU/CanvasSDLGPU.hpp>

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/Loading/WidgetFactory.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendRendererSDLGPU::BackendRendererSDLGPU(SDL_GPUDevice* device) :
        m_device{device}
    {
        TGUI_ASSERT(m_device, "device passed to BackendRendererSDLGPU can't be a nullptr");

        if (!WidgetFactory::getConstructFunction(U"CanvasSDLGPU"))
            WidgetFactory::setConstructFunction(U"CanvasSDLGPU", std::make_shared<CanvasSDLGPU>);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendTexture> BackendRendererSDLGPU::createTexture()
    {
        return std::make_shared<BackendTextureSDLGPU>(m_device);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int BackendRendererSDLGPU::getMaximumTextureSize()
    {
        // SDL's GPU API currently provides no way to query this value
        return 16384;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SDL_GPUDevice* BackendRendererSDLGPU::getInternalDevice() const
    {
        return m_device;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
