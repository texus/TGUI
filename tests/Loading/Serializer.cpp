/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include "../catch.hpp"
#include <TGUI/Loading/Serializer.hpp>

TEST_CASE("[Serializer]") {
    SECTION("serialize uninitialized object") {
        REQUIRE_THROWS_AS(tgui::Serializer::serialize({}), tgui::Exception);
    }

    SECTION("serialize font") {
        auto font = std::make_shared<sf::Font>();
        font->loadFromFile("resources/DroidSansArmenian.ttf");
        REQUIRE(tgui::Serializer::serialize(font) == "null"); // Serializing fonts is not supported yet

        font = nullptr;
        REQUIRE(tgui::Serializer::serialize(font) == "null");
    }

    SECTION("serialize color") {
        REQUIRE(tgui::Serializer::serialize(sf::Color{100, 0, 50}) == "#640032");

        REQUIRE(tgui::Serializer::serialize(sf::Color{1, 35, 69, 103}) == "#01234567");
        REQUIRE(tgui::Serializer::serialize(sf::Color{137, 171, 205, 239}) == "#89ABCDEF");

        REQUIRE(tgui::Serializer::serialize(sf::Color::Black) == "Black");
        REQUIRE(tgui::Serializer::serialize(sf::Color::White) == "White");
        REQUIRE(tgui::Serializer::serialize(sf::Color::Red) == "Red");
        REQUIRE(tgui::Serializer::serialize(sf::Color::Green) == "Green");
        REQUIRE(tgui::Serializer::serialize(sf::Color::Blue) == "Blue");
        REQUIRE(tgui::Serializer::serialize(sf::Color::Magenta) == "Magenta");
        REQUIRE(tgui::Serializer::serialize(sf::Color::Yellow) == "Yellow");
        REQUIRE(tgui::Serializer::serialize(sf::Color::Cyan) == "Cyan");
        REQUIRE(tgui::Serializer::serialize(sf::Color::Transparent) == "Transparent");
    }

    SECTION("serialize borders") {
        REQUIRE(tgui::Serializer::serialize(tgui::Borders{}) == "(0, 0, 0, 0)");
        REQUIRE(tgui::Serializer::serialize(tgui::Borders{10, 2, 50, 300}) == "(10, 2, 50, 300)");
    }

    SECTION("serialize texture") {
        tgui::Texture texture;

        texture.load("resources/image.png");
        REQUIRE(tgui::Serializer::serialize(texture) == "\"resources/image.png\"");

        texture.load("resources/image.png", {0, 0, 25, 25});
        REQUIRE(tgui::Serializer::serialize(texture) == "\"resources/image.png\" Part(0, 0, 25, 25)");

        texture.load("resources/image.png", {}, {10, 10, 30, 30});
        REQUIRE(tgui::Serializer::serialize(texture) == "\"resources/image.png\" Middle(10, 10, 30, 30)");

        texture.load("resources/image.png", {0, 0, 40, 40}, {10, 10, 20, 20});
        REQUIRE(tgui::Serializer::serialize(texture) == "\"resources/image.png\" Part(0, 0, 40, 40) Middle(10, 10, 20, 20)");
    }

    SECTION("serialize string") {
        REQUIRE(tgui::Serializer::serialize({""}) == "\"\"");
        REQUIRE(tgui::Serializer::serialize({"Just a string."}) == "\"Just a string.\"");
        REQUIRE(tgui::Serializer::serialize({"\\"}) == "\"\\\\\"");
        REQUIRE(tgui::Serializer::serialize({"\n"}) == "\"\\n\"");
        REQUIRE(tgui::Serializer::serialize({"\t"}) == "\"\\t\"");
        REQUIRE(tgui::Serializer::serialize({"\v"}) == "\"\\v\"");
        REQUIRE(tgui::Serializer::serialize({"\""}) == "\"\\\"\"");
        REQUIRE(tgui::Serializer::serialize({"a\t\"string\"?\nYES!"}) == "\"a\\t\\\"string\\\"?\\nYES!\"");
    }

    SECTION("serialize number") {
        REQUIRE(tgui::Serializer::serialize({0}) == "0");
        REQUIRE(tgui::Serializer::serialize({1}) == "1");
        REQUIRE(tgui::Serializer::serialize({2.15f}) == "2.15");
    }

    SECTION("custom serialize function") {
        REQUIRE(tgui::Serializer::serialize(sf::Color::Blue) == "Blue");
        auto oldFunc = tgui::Serializer::getFunction(tgui::ObjectConverter::Type::Color);

        tgui::Serializer::setFunction(tgui::ObjectConverter::Type::Color, [](tgui::ObjectConverter&&){ return "STR"; });
        REQUIRE(tgui::Serializer::serialize(sf::Color::Blue) == "STR");
        REQUIRE(tgui::Serializer::serialize(tgui::Borders{10, 2, 50, 300}) == "(10, 2, 50, 300)");
        
        tgui::Serializer::setFunction(tgui::ObjectConverter::Type::Color, oldFunc);
        REQUIRE(tgui::Serializer::serialize(sf::Color::Blue) == "Blue");
    }
}
