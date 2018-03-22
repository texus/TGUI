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
#include <TGUI/Text.hpp>

TEST_CASE("[Text]")
{
    tgui::Text text;

    SECTION("String")
    {
        REQUIRE(text.getString() == "");
        text.setString("MyString");
        REQUIRE(text.getString() == "MyString");
    }

    SECTION("CharacterSize")
    {
        text.setCharacterSize(20);
        REQUIRE(text.getCharacterSize() == 20);
    }

    SECTION("Color")
    {
        text.setColor(sf::Color::Blue);
        REQUIRE(text.getColor() == sf::Color::Blue);
    }

    SECTION("Opacity")
    {
        REQUIRE(text.getOpacity() == 1);
        text.setOpacity(0.75);
        REQUIRE(text.getOpacity() == 0.75);
    }

    SECTION("Font")
    {
        std::shared_ptr<sf::Font> font = std::make_shared<sf::Font>();

        REQUIRE(text.getFont() == nullptr);
        text.setFont(font);
        REQUIRE(text.getFont().getFont() == font);
        text.setFont(nullptr);
        REQUIRE(text.getFont() == nullptr);
    }

    SECTION("Style")
    {
        REQUIRE(text.getStyle() == sf::Text::Regular);
        text.setStyle(sf::Text::Italic);
        REQUIRE(text.getStyle() == sf::Text::Italic);
    }

    SECTION("Size")
    {
        text.setFont("resources/DejaVuSans.ttf");

        SECTION("Calculation")
        {
            text.setCharacterSize(40);
            text.setString("a\nb\nc");
            float width = text.getSize().x;

            std::shared_ptr<sf::Font> font = text.getFont();
            REQUIRE(text.getSize().y == 3 * font->getLineSpacing(40) + tgui::Text::calculateExtraVerticalSpace(font, 40));

            text.setString("xy");
            REQUIRE(text.getSize().y == 1 * font->getLineSpacing(40) + tgui::Text::calculateExtraVerticalSpace(font, 40));
            REQUIRE(text.getSize().x > width);
        }

        SECTION("Without a font the size is (0,0)")
        {
            tgui::Text text2;
            REQUIRE(text2.getSize() == sf::Vector2f(0, 0));

            text2.setString("xyz");
            REQUIRE(text2.getSize() == sf::Vector2f(0, 0));
        }

        SECTION("Tab is 4x wider than space")
        {
            text.setString(" ");
            float width = text.getSize().x;

            text.setString("\t");
            REQUIRE(text.getSize().x == 4 * width);
        }

        SECTION("Updated when string changes")
        {
            float height = text.getSize().y;

            text.setString("a\nb\nc");
            REQUIRE(height < text.getSize().y);
            height = text.getSize().y;

            text.setString("x");
            REQUIRE(height > text.getSize().y);
        }

        SECTION("Independent of individual character height")
        {
            text.setString(L"Êg");

            tgui::Text text2;
            text2.setFont("resources/DejaVuSans.ttf");
            text2.setString(".");
            REQUIRE(text.getSize().y == text2.getSize().y);

            tgui::Text text3;
            text3.setFont("resources/DejaVuSans.ttf");
            REQUIRE(text.getSize().y == text3.getSize().y);
        }
    }
}
