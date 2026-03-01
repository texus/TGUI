/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2026 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include "TGUI/Color.hpp"
#include "TGUI/TextStyle.hpp"
#include "Tests.hpp"

TEST_CASE("[Text]")
{
    tgui::Text text;

    SECTION("String")
    {
        REQUIRE(text.getString().empty());
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
        text.setColor(tgui::Color::Blue);
        REQUIRE(text.getColor() == tgui::Color::Blue);
    }

    SECTION("Opacity")
    {
        REQUIRE(text.getOpacity() == 1);
        text.setOpacity(0.75f);
        REQUIRE(text.getOpacity() == 0.75f);
    }

    SECTION("Font")
    {
        const tgui::Font font("resources/DejaVuSans.ttf");

        REQUIRE(text.getFont() == nullptr);
        text.setFont(font);
        REQUIRE(text.getFont() != nullptr);
        REQUIRE(text.getFont() == font);
        text.setFont(nullptr);
        REQUIRE(text.getFont() == nullptr);
    }

    SECTION("Style")
    {
        REQUIRE(text.getStyle() == tgui::TextStyle::Regular);
        text.setStyle(tgui::TextStyle::Italic);
        REQUIRE(text.getStyle() == tgui::TextStyle::Italic);
    }

    SECTION("Outline")
    {
        REQUIRE(text.getOutlineColor() == tgui::Color());
        text.setOutlineColor(tgui::Color::Black);
        REQUIRE(text.getOutlineColor() == tgui::Color::Black);
        text.setOutlineColor(tgui::Color::White);
        REQUIRE(text.getOutlineColor() == tgui::Color::White);
        text.setOutlineColor(tgui::Color::Red);
        REQUIRE(text.getOutlineColor() == tgui::Color::Red);
        text.setOutlineColor(tgui::Color::Green);
        REQUIRE(text.getOutlineColor() == tgui::Color::Green);
        text.setOutlineColor(tgui::Color::Blue);
        REQUIRE(text.getOutlineColor() == tgui::Color::Blue);
        text.setOutlineColor(tgui::Color::Yellow);
        REQUIRE(text.getOutlineColor() == tgui::Color::Yellow);
        text.setOutlineColor(tgui::Color::Magenta);
        REQUIRE(text.getOutlineColor() == tgui::Color::Magenta);
        text.setOutlineColor(tgui::Color::Cyan);
        REQUIRE(text.getOutlineColor() == tgui::Color::Cyan);
        text.setOutlineColor(tgui::Color::Transparent);
        REQUIRE(text.getOutlineColor() == tgui::Color::Transparent);
        text.setOutlineColor(tgui::Color());
        REQUIRE(text.getOutlineColor() == tgui::Color());
        text.setOutlineColor(tgui::Color(1, 2, 3, 4));
        REQUIRE(text.getOutlineColor() == tgui::Color{1, 2, 3, 4});
    }

    SECTION("Size")
    {
        text.setCharacterSize(30);
        text.setFont("resources/DejaVuSans.ttf");

        SECTION("Calculation")
        {
            text.setCharacterSize(40);
            text.setString("a\nb\nc");
            const float width = text.getSize().x;

            const tgui::Font& font = text.getFont();
            REQUIRE(text.getSize().y == (2 * font.getLineSpacing(40)) + std::max(font.getFontHeight(40), font.getLineSpacing(40)));

            text.setString("xy");
            REQUIRE(text.getSize().y == std::max(font.getFontHeight(40), font.getLineSpacing(40)));
            REQUIRE(text.getSize().x > width);
        }

        SECTION("Without a font the size is (0,0)")
        {
            tgui::Text text2;
            REQUIRE(text2.getSize() == tgui::Vector2f(0, 0));

            text2.setString("xyz");
            REQUIRE(text2.getSize() == tgui::Vector2f(0, 0));
        }

        SECTION("Tab is 4x wider than space")
        {
            text.setString(" ");
            const float width = text.getSize().x;

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
            text.setString(L"\u00CAg");

            tgui::Text text2;
            text2.setCharacterSize(text.getCharacterSize());
            text2.setFont("resources/DejaVuSans.ttf");
            text2.setString(".");
            REQUIRE(text.getSize().y == text2.getSize().y);

            tgui::Text text3;
            text3.setCharacterSize(text.getCharacterSize());
            text3.setFont("resources/DejaVuSans.ttf");
            REQUIRE(text.getSize().y == text3.getSize().y);
        }

        SECTION("Line width without font")
        {
            REQUIRE(text.getLineWidth("Foo", nullptr, 0, {}) == 0.0f);
            REQUIRE(text.getLineWidth("Bar", nullptr, 1, tgui::TextStyle::Underlined) == 0.0f);
            REQUIRE(text.getLineWidth("Baz", nullptr, 42, {}) == 0.0f);
            ;
            REQUIRE(text.getLineWidth("FooBar", nullptr, 666, tgui::TextStyle::StrikeThrough) == 0.0f);
        }
    }
}
