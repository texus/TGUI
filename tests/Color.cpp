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

#include "Tests.hpp"
#include <TGUI/Color.hpp>

TEST_CASE("[Color]")
{
    sf::Color color{16, 32, 64, 128};
    REQUIRE(tgui::Color(color).getRed() == 16);
    REQUIRE(tgui::Color(color).getGreen() == 32);
    REQUIRE(tgui::Color(color).getBlue() == 64);
    REQUIRE(tgui::Color(color).getAlpha() == 128);
    REQUIRE(sf::Color(tgui::Color(color)) == color);
    REQUIRE(sf::Color(tgui::Color(16, 32, 64, 128)) == color);
    REQUIRE(sf::Color(tgui::Color("rgba(16, 32, 64, 128)")) == color);
    REQUIRE(sf::Color(tgui::Color("#10204080")) == color);
    REQUIRE(sf::Color(tgui::Color(std::string("#10204080"))) == color);
    REQUIRE(sf::Color(tgui::Color(std::string("Red"))) == sf::Color::Red);
}
