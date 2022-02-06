/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_BACKEND_RENDER_TARGET_HPP
#define TGUI_BACKEND_RENDER_TARGET_HPP

#include <TGUI/Rect.hpp>
#include <TGUI/Color.hpp>
#include <TGUI/Sprite.hpp>
#include <TGUI/Text.hpp>
#include <TGUI/Outline.hpp>
#include <TGUI/RenderStates.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class Widget;
    class RootContainer;


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Base class for render targets
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API BackendRenderTargetBase
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Virtual destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~BackendRenderTargetBase() = default;

#ifndef TGUI_REMOVE_DEPRECATED_CODE
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Informs the render target about which part of the window is used for rendering
        ///
        /// @param view     Defines which part of the gui is being shown
        /// @param viewport Defines which part of the window is being rendered to
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_DEPRECATED("Use setView(view, viewport, targetSize) instead") virtual void setView(FloatRect view, FloatRect viewport);
#endif

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Informs the render target about which part of the window is used for rendering
        ///
        /// @param view        Defines which part of the gui is being shown
        /// @param viewport    Defines which part of the window is being rendered to
        /// @param targetSize  Size of the window
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setView(FloatRect view, FloatRect viewport, Vector2f targetSize);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Draws the gui and all of its widgets
        ///
        /// @param root  Root container that holds all widgets in the gui
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void drawGui(const std::shared_ptr<RootContainer>& root) = 0;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Draws a widget, if the widget is visible
        ///
        /// @param states  Render states to use for drawing
        /// @param widget  The widget to draw
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void drawWidget(const RenderStates& states, const std::shared_ptr<Widget>& widget);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Adds another clipping region
        ///
        /// @param states  Render states to use for drawing
        /// @param rect    The clipping region
        ///
        /// If multiple clipping regions were added then contents is only shown in the intersection of all regions.
        ///
        /// @warning Every call to addClippingLayer must have a matching call to removeClippingLayer.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void addClippingLayer(const RenderStates& states, FloatRect rect);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Removes the last added clipping region
        ///
        /// @warning The addClippingLayer function must have been called before calling this function.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void removeClippingLayer();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Draws borders inside a provided rectangle
        ///
        /// @param states  Render states to use for drawing
        /// @param borders The size of the borders on each side
        /// @param size    The size of the rectangle in which borders will be drawn
        /// @param color   Color of the borders
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void drawBorders(const RenderStates& states, const Borders& borders, Vector2f size, Color color);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Draws a colored rectangle
        ///
        /// @param states  Render states to use for drawing
        /// @param size    Size of the rectangle
        /// @param color   Color of the rectangle
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void drawFilledRect(const RenderStates& states, Vector2f size, Color color);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Draws a texture
        ///
        /// @param states  Render states to use for drawing
        /// @param sprite  Image to draw
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void drawSprite(const RenderStates& states, const Sprite& sprite) = 0;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Draws some text
        ///
        /// @param states  Render states to use for drawing
        /// @param text    Text to draw
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void drawText(const RenderStates& states, const Text& text) = 0;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Draws one or more triangles (using the color that is specified in the vertices)
        ///
        /// @param states       Render states to use for drawing
        /// @param vertices     Pointer to first element in array of vertices
        /// @param vertexCount  Amount of elements in the vertex array
        /// @param indices      Pointer to first element in array of indices
        /// @param indexCount   Amount of elements in the indices array
        ///
        /// If indices is a nullptr then vertexCount must be a multiple of 3 and each set of 3 vertices will be seen as a triangle.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void drawTriangles(const RenderStates& states, const Vertex* vertices, std::size_t vertexCount, const int* indices = nullptr, std::size_t indexCount = 0) = 0;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Draws one or more triangles (using the color that is specified in the vertices)
        ///
        /// @param states        Render states to use for drawing
        /// @param vertices      Array of vertices
        /// @param indices       Optional array of indices
        ///
        /// This function is provided for convenience and just calls the other drawTriangles function.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void drawTriangles(const RenderStates& states, std::initializer_list<Vertex> vertices, std::initializer_list<int> indices = {});


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Draws a circle
        ///
        /// @param states           Render states to use for drawing
        /// @param size             Diameter of the circle
        /// @param backgroundColor  Color to fill the circle with
        /// @param borderThickness  Thickness of the border to draw around the circle (outside given size if thickness is positive)
        /// @param borderColor      Color of the border, if borderThickness differs from 0
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void drawCircle(const RenderStates& states, float size, const Color& backgroundColor, float borderThickness = 0, const Color& borderColor = {});


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Draws a rounded rectangle
        ///
        /// @param states           Render states to use for drawing
        /// @param size             Size of the rectangle (includes borders)
        /// @param backgroundColor  Fill color of the rectangle
        /// @param radius           Radius of the rounded corners
        /// @param borders          Optional borders on the sides of the rectangle (must be the same on all sides)
        /// @param borderColor      Color of the borders
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void drawRoundedRectangle(const RenderStates& states, const Vector2f& size, const Color& backgroundColor, float radius,
                                          const Borders& borders = {0}, const Color& borderColor = Color::Black);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Called from addClippingLayer and removeClippingLayer to apply the clipping
        ///
        /// @param clipRect      View rectangle to apply
        /// @param clipViewport  Viewport to apply
        ///
        /// Both rectangles may be empty when nothing that will be drawn is going to be visible.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef TGUI_NEXT
        virtual void updateClipping(FloatRect clipRect, FloatRect clipViewport) = 0;
#else
        virtual void updateClipping(FloatRect clipRect, FloatRect clipViewport) { (void)clipRect; (void)clipViewport; };
#endif


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        FloatRect m_viewRect;
        FloatRect m_viewport;
        Vector2f  m_targetSize;

        std::vector<std::pair<FloatRect, FloatRect>> m_clipLayers;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_BACKEND_RENDER_TARGET_HPP
