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

#include <TGUI/Backend/Renderer/SDL_GPU/BackendTextureSDLGPU.hpp>

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/Backend/Window/Backend.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    BackendTextureSDLGPU::BackendTextureSDLGPU(SDL_GPUDevice* device, std::uint32_t usageFlags) :
        m_device{device},
        m_usageFlags{usageFlags}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendTextureSDLGPU::~BackendTextureSDLGPU()
    {
        if (m_texture)
            SDL_ReleaseGPUTexture(m_device, m_texture);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendTextureSDLGPU::loadTextureOnly(Vector2u size, const std::uint8_t* pixels, bool smooth)
    {
        const bool reuseTexture = (m_texture && (size.x == m_imageSize.x) && (size.y == m_imageSize.y) && (smooth == m_isSmooth));

        BackendTexture::loadTextureOnly(size, pixels, smooth);

        if (!reuseTexture)
        {
            if (m_texture)
                SDL_ReleaseGPUTexture(m_device, m_texture);

            SDL_GPUTextureCreateInfo textureInfo = {};
            textureInfo.type = SDL_GPU_TEXTURETYPE_2D;
            textureInfo.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
            textureInfo.usage = m_usageFlags;
            textureInfo.width = size.x;
            textureInfo.height = size.y;
            textureInfo.layer_count_or_depth = 1;
            textureInfo.num_levels = 1;
            m_texture = SDL_CreateGPUTexture(m_device, &textureInfo);
        }

        SDL_GPUTransferBufferCreateInfo transferBufferInfo = {};
        transferBufferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        transferBufferInfo.size = size.x * size.y * 4;
        SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(m_device, &transferBufferInfo);

        void* bufferData = SDL_MapGPUTransferBuffer(m_device, transferBuffer, false);
        std::memcpy(bufferData, pixels, size.x * size.y * 4);
        SDL_UnmapGPUTransferBuffer(m_device, transferBuffer);

        SDL_GPUCommandBuffer* uploadCmdBuffer = SDL_AcquireGPUCommandBuffer(m_device);
        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuffer);

        SDL_GPUTextureTransferInfo transferInfo = {};
        transferInfo.transfer_buffer = transferBuffer;
        transferInfo.offset = 0;
        transferInfo.pixels_per_row = 0;
        transferInfo.rows_per_layer = 0;

        SDL_GPUTextureRegion textureRegion = {};
        textureRegion.texture = m_texture;
        textureRegion.w = size.x;
        textureRegion.h = size.y;
        textureRegion.d = 1;

        SDL_UploadToGPUTexture(copyPass, &transferInfo, &textureRegion, false);

        SDL_EndGPUCopyPass(copyPass);
        SDL_SubmitGPUCommandBuffer(uploadCmdBuffer);
        SDL_ReleaseGPUTransferBuffer(m_device, transferBuffer);

        return (m_texture != nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendTextureSDLGPU::replaceInternalTexture(SDL_GPUTexture* texture, Vector2u imageSize)
    {
        if (m_texture)
            SDL_ReleaseGPUTexture(m_device, m_texture);

        m_texture = texture;
        m_pixels = nullptr;
        m_imageSize = imageSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
