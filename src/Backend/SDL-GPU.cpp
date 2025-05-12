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

#include <TGUI/Backend/SDL-GPU.hpp>

#include <TGUI/extlibs/IncludeSDL.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    inline namespace SDL_GPU
    {
        void Gui::setWindow(SDL_Window* window, SDL_GPUDevice* device)
        {
            if (!isBackendSet())
            {
                auto backend = std::make_shared<BackendSDL>();
                backend->setFontBackend(std::make_shared<BackendFontFactoryImpl<BackendFontSDLttf>>());
                backend->setRenderer(std::make_shared<BackendRendererSDLGPU>(device));
                backend->setDestroyOnLastGuiDetatch(true);
                setBackend(backend);
            }

            m_backendRenderTarget = std::make_shared<BackendRenderTargetSDLGPU>(device, SDL_GetGPUSwapchainTextureFormat(device, window));

            m_device = device;
            setGuiWindow(window);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        SDL_GPUDevice* Gui::getDevice() const
        {
            return m_device;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Gui::presentScreen()
        {
            TGUI_ASSERT(false, "The SDL_GPU backend does not use the presentScreen function");
            throw Exception("The SDL_GPU backend does not use the presentScreen function");
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Gui::prepareDraw(SDL_GPUCommandBuffer* cmdBuffer, SDL_GPUCopyPass* copyPass)
        {
            if (m_drawUpdatesTime)
                updateTime();

            downcast<BackendRenderTargetSDLGPU>(m_backendRenderTarget)->prepareDrawGui(m_container, cmdBuffer, copyPass);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Gui::draw(SDL_GPURenderPass* renderPass)
        {
            downcast<BackendRenderTargetSDLGPU>(m_backendRenderTarget)->drawGui(renderPass);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Gui::renderFrame(Color clearColor)
        {
            SDL_GPUCommandBuffer* cmdBuffer = SDL_AcquireGPUCommandBuffer(m_device);
            if (!cmdBuffer)
                return;

            SDL_GPUTexture* swapchainTexture;
            if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmdBuffer, m_window, &swapchainTexture, NULL, NULL))
                return;

            if (swapchainTexture)
            {
                prepareDraw(cmdBuffer);

                SDL_GPUColorTargetInfo colorTargetInfo = {};
                colorTargetInfo.texture = swapchainTexture;
                colorTargetInfo.clear_color = {clearColor.getRed() / 255.f, clearColor.getGreen() / 255.f, clearColor.getBlue() / 255.f, clearColor.getAlpha() / 255.f};
                colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
                colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

                SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(cmdBuffer, &colorTargetInfo, 1, NULL);
                draw(renderPass);
                SDL_EndGPURenderPass(renderPass);
            }

            SDL_SubmitGPUCommandBuffer(cmdBuffer);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
