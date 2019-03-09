/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2019 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/SvgImage.hpp>
#include <TGUI/Widgets/Picture.hpp>

TEST_CASE("[SvgImage]")
{
    SECTION("Empty image")
    {
        tgui::SvgImage svgImage;

        REQUIRE(!svgImage.isSet());
        REQUIRE(svgImage.getSize() == sf::Vector2f{0, 0});

        sf::Texture texture;
        svgImage.rasterize(texture, sf::Vector2u{100, 100});
        REQUIRE(texture.getSize() == sf::Vector2u{0, 0});
    }

    SECTION("Loading svg")
    {
        tgui::SvgImage svgImage{"resources/SFML.svg"};

        REQUIRE(svgImage.isSet());
        REQUIRE(svgImage.getSize() == sf::Vector2f{130, 130});

        sf::Texture texture;
        svgImage.rasterize(texture, sf::Vector2u{100, 100});
        REQUIRE(texture.getSize() == sf::Vector2u{100, 100});
    }

    SECTION("Drawing svg")
    {
        auto picture = tgui::Picture::create("resources/SFML.svg");

        TEST_DRAW_INIT(130, 130, picture)

        SECTION("Without transformations")
        {
            TEST_DRAW("Svg.png")
        }

        SECTION("Moved, scaled and colorized")
        {
            tgui::Texture texture = picture->getRenderer()->getTexture();
            texture.setColor(sf::Color::Red);
            picture->getRenderer()->setTexture(texture);

            picture->setPosition(40, 10);
            picture->setSize(80, 80);

            TEST_DRAW("Svg_TransformedAndColored.png")
        }
    }
}
