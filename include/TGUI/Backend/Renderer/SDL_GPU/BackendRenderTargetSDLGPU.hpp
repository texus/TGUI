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

#ifndef TGUI_BACKEND_RENDER_TARGET_SDL_GPU_HPP
#define TGUI_BACKEND_RENDER_TARGET_SDL_GPU_HPP

#include <TGUI/Backend/Renderer/SDL_GPU/BackendTextureSDLGPU.hpp>

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/Backend/Renderer/BackendRenderTarget.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Render target implementation that uses SDL3's GPU API
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API BackendRenderTargetSDLGPU : public BackendRenderTarget
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        ///
        /// @param device  Device used for rendering
        /// @param swapchainTextureFormat  Texture format returned by SDL_GetGPUSwapchainTextureFormat(device, window)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        BackendRenderTargetSDLGPU(SDL_GPUDevice* device, SDL_GPUTextureFormat swapchainTextureFormat);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Descructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ~BackendRenderTargetSDLGPU();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Select the color that will be used by clearScreen
        /// @param color  Background color of the window
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setClearColor(const Color& color) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Clears the screen, called at the beginning of each frame when gui.mainLoop() is called
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void clearScreen() override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Informs the render target about which part of the window is used for rendering
        ///
        /// @param view        Defines which part of the gui is being shown
        /// @param viewport    Defines which part of the window is being rendered to
        /// @param targetSize  Size of the window
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setView(FloatRect view, FloatRect viewport, Vector2f targetSize) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief This function is not used by this backend, use drawGui(SDL_GPURenderPass*) instead
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void drawGui(const std::shared_ptr<RootContainer>&) override
        {
            TGUI_ASSERT(false, "The SDL_GPU backend requires that you pass SDL_GPUCommandBuffer and SDL_GPURenderPass pointers to the draw function");
            throw Exception("The SDL_GPU backend requires that you pass SDL_GPUCommandBuffer and SDL_GPURenderPass pointers to the draw function");
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Prepares drawing the gui and all of its widgets
        ///
        /// @param root        Root container that holds all widgets in the gui
        /// @param cmdBuffer   Command buffer to add rendering instructions to
        /// @param copyPass    Optional active copy pass
        ///
        /// @return True if there weren't any errors, false if something went wrong (e.g. could not create or upload buffer)
        ///
        /// @warning If copyPass is a nullptr then this function uses a new SDL_GPUCopyPass internally,
        ///          there should be no active pass when calling this function in this case.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool prepareDrawGui(const std::shared_ptr<RootContainer>& root, SDL_GPUCommandBuffer* cmdBuffer, SDL_GPUCopyPass* copyPass = nullptr);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Draws the gui and all of its widgets
        ///
        /// @param root        Root container that holds all widgets in the gui
        /// @param cmdBuffer   Command buffer to add rendering instructions to
        /// @param renderPass  Active render pass
        ///
        /// @warning prepareDrawGui must have been called before calling this function
        /// @warning Bound GraphicsPipeline, VertexBuffers, IndexBuffer and FragmentSamplers will be changed by this function
        /// @warning Scissor rectangle is changed by this function to contain the entire window
        /// @warning Viewport is changed by this function
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void drawGui(SDL_GPURenderPass* renderPass);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Draws a vertex array. This is called by the other draw functions.
        ///
        /// @param states       Render states to use for drawing
        /// @param vertices     Pointer to first element in array of vertices
        /// @param vertexCount  Amount of elements in the vertex array
        /// @param indices      Pointer to first element in array of indices
        /// @param indexCount   Amount of elements in the indices array
        /// @param texture      Texture to use, or nullptr when drawing colored triangles
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void drawVertexArray(const RenderStates& states, const Vertex* vertices, std::size_t vertexCount,
                             const unsigned int* indices, std::size_t indexCount, const std::shared_ptr<BackendTexture>& texture) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Called from addClippingLayer and removeClippingLayer to apply the clipping
        ///
        /// @param clipRect      View rectangle to apply
        /// @param clipViewport  Viewport to apply
        ///
        /// Both rectangles may be empty when nothing that will be drawn is going to be visible.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateClipping(FloatRect clipRect, FloatRect clipViewport) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        struct DrawCommand
        {
            std::size_t indexCount;
            std::shared_ptr<BackendTexture> texture;
            SDL_Rect clipRect;
        };

        SDL_GPUDevice* m_device = nullptr;
        SDL_GPUGraphicsPipeline* m_pipeline = nullptr;
        SDL_GPUSampler* m_samplerNearest = nullptr;
        SDL_GPUSampler* m_samplerLinear = nullptr;
        std::unique_ptr<BackendTextureSDLGPU> m_emptyTexture;

        Transform m_projectionTransform;

        // Used while prepateDrawGui is being executed
        std::vector<Vertex> m_vertices;
        std::vector<unsigned int> m_indices;
        SDL_Rect m_clipRect;

        // Filled duing prepateDrawGui and used during drawGui
        std::vector<DrawCommand> m_drawCommands;
        SDL_GPUBuffer* m_vertexBuffer = nullptr;
        SDL_GPUBuffer* m_indexBuffer = nullptr;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_BACKEND_RENDER_TARGET_SDL_GPU_HPP
