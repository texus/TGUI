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

#ifndef TGUI_BACKEND_SDL_GPU_INCLUDE_HPP
#define TGUI_BACKEND_SDL_GPU_INCLUDE_HPP

#include <TGUI/Config.hpp>
#if !TGUI_HAS_BACKEND_SDL_GPU
    #error "TGUI wasn't build with the SDL_GPU backend"
#endif

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/Backend/Window/SDL/BackendSDL.hpp>
    #include <TGUI/Backend/Renderer/SDL_GPU/BackendRendererSDLGPU.hpp>
    #include <TGUI/Backend/Font/SDL_ttf/BackendFontSDLttf.hpp>
#endif

TGUI_MODULE_EXPORT namespace tgui
{
    inline namespace SDL_GPU
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gui class for SDL + SDL_ttf, using SDL's GPU API (https://wiki.libsdl.org/SDL3/CategoryGPU) for rendering
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class TGUI_API Gui : public BackendGuiSDL
        {
        public:

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Default constructor
            ///
            /// @warning You must still call setWindow on the Gui before using any TGUI functions
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            Gui() = default;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Constructor that immediately sets the window and renderer on which the gui should be drawn
            ///
            /// @param window  The SDL window that will be used by the gui
            /// @param device  The SDL device that will be used for rendering the gui on the window
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            Gui(SDL_Window* window, SDL_GPUDevice* device)
            {
                setWindow(window, device);
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the window and renderer on which the gui should be drawn
            ///
            /// @param window  The SDL window that will be used by the gui
            /// @param device  The SDL device that will be used for rendering the gui on the window
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void setWindow(SDL_Window* window, SDL_GPUDevice* device);

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Returns the renderer that was provided to the gui
            ///
            /// @return Renderer that was set via the constructor or setWindow
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            SDL_GPUDevice* getDevice() const;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief This function is not used by this backend, use draw(SDL_GPUCommandBuffer*, SDL_GPURenderPass*) instead
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void draw() override
            {
                TGUI_ASSERT(false, "The SDL_GPU backend requires that you pass SDL_GPUCommandBuffer and SDL_GPURenderPass pointers to the draw function");
                throw Exception("The SDL_GPU backend requires that you pass SDL_GPUCommandBuffer and SDL_GPURenderPass pointers to the draw function");
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Prepares drawing the gui and all of its widgets
            ///
            /// @param cmdBuffer   Command buffer to add rendering instructions to
            /// @param copyPass    Optional active copy pass
            ///
            /// @warning If copyPass is a nullptr then this function uses a new SDL_GPUCopyPass internally,
            ///          there should be no active pass when calling this function in this case.
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void prepareDraw(SDL_GPUCommandBuffer* cmdBuffer, SDL_GPUCopyPass* copyPass = nullptr);

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Draws all the widgets that were added to the gui
            ///
            /// @param renderPass  Active render pass
            ///
            /// @warning prepareDrawGui must have been called before calling this function
            /// @warning Bound GraphicsPipeline, VertexBuffers, IndexBuffer and FragmentSamplers will be changed by this function
            /// @warning Scissor rectangle is changed by this function to contain the entire window
            /// @warning Viewport is changed by this function
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void draw(SDL_GPURenderPass* renderPass);

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        protected:

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Clears the screen, draws the gui and then presents the frame
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void renderFrame(Color clearColor) override;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Not used by this backend
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void presentScreen() override;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        protected:

            SDL_GPUDevice* m_device = nullptr;
        };
    }
}

#endif // TGUI_BACKEND_SDL_GPU_INCLUDE_HPP
