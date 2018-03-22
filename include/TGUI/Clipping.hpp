/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Vector2f.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/View.hpp>

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
        /// @param states  Current render states
        /// @param topLeft Position of the top left corner of the clipping area relative to the view
        /// @param size    Size of the clipping area relative to the view
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Clipping(sf::RenderTarget& target, const sf::RenderStates& states, Vector2f topLeft, Vector2f size);


        // The clipping object cannot be copied
        Clipping(const Clipping& copy) = delete;
        Clipping& operator=(const Clipping& right) = delete;


        // When the clipping object is destroyed, the old clipping is restored
        ~Clipping();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        // Sets the view used by the gui, which the calculations have to take into account when changing the view for clipping
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void setGuiView(const sf::View& view);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        sf::RenderTarget& m_target;
        sf::View m_oldView;

        static sf::View m_originalView;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_CLIPPING_HPP
