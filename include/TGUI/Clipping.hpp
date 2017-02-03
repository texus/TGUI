/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_CLIPPING_HPP
#define TGUI_CLIPPING_HPP

#include <TGUI/Global.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/OpenGL.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API Clipping
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates a clipping object which will define a clipping region until the object is destroyed
        ///
        /// @param target  Target to which we are drawing
        /// @param topLeft Position of the top left corner of the clipping area relative to the view
        /// @param size    Size of the clipping area relative to the view
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Clipping(const sf::RenderTarget& target, const sf::RenderStates& states, sf::Vector2f topLeft, sf::Vector2f size)
        {
            const sf::View& view = target.getView();
            sf::Vector2f bottomRight = topLeft + size;

            // Calculate the scale factor of the view
            float scaleViewX = target.getSize().x / view.getSize().x;
            float scaleViewY = target.getSize().y / view.getSize().y;

            // Get the global position
            sf::Vector2f topLeftPosition = {((topLeft.x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                            ((topLeft.y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};
            sf::Vector2f bottomRightPosition = {(bottomRight.x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                                (bottomRight.y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top)};

            topLeftPosition = states.transform.transformPoint(topLeftPosition);
            bottomRightPosition = states.transform.transformPoint(bottomRightPosition);

            // Get the old clipping area
            glGetIntegerv(GL_SCISSOR_BOX, m_scissor);

            // Calculate the clipping area
            GLint scissorLeft = std::max(static_cast<GLint>(topLeftPosition.x * scaleViewX), m_scissor[0]);
            GLint scissorTop = std::max(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - m_scissor[1] - m_scissor[3]);
            GLint scissorRight = std::min(static_cast<GLint>(bottomRightPosition.x * scaleViewX), m_scissor[0] + m_scissor[2]);
            GLint scissorBottom = std::min(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - m_scissor[1]);

            // If the object outside the window then don't draw anything
            if (scissorRight < scissorLeft)
                scissorRight = scissorLeft;
            else if (scissorBottom < scissorTop)
                scissorTop = scissorBottom;

            // Set the clipping area
            glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // The clipping object cannot be copied
        Clipping(const Clipping& copy) = delete;
        Clipping& operator=(const Clipping& right) = delete;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // When the clipping object is destroyed, the old clipping is restored
        ~Clipping()
        {
            glScissor(m_scissor[0], m_scissor[1], m_scissor[2], m_scissor[3]);
        };


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        GLint m_scissor[4];
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_CLIPPING_HPP
