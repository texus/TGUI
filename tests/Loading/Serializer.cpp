/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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

TEST_CASE("[Serializer]")
{
    SECTION("serialize uninitialized object")
    {
        REQUIRE_THROWS_AS(tgui::Serializer::serialize({}), tgui::Exception);
    }

    SECTION("serialize bool")
    {
        REQUIRE(tgui::Serializer::serialize({false}) == "false");
        REQUIRE(tgui::Serializer::serialize({true}) == "true");
    }

    SECTION("serialize font")
    {
        tgui::Font font{"resources/DejaVuSans.ttf"};
        REQUIRE(tgui::Serializer::serialize(font) == "\"resources/DejaVuSans.ttf\"");

        font = nullptr;
        REQUIRE(tgui::Serializer::serialize(font) == "null");
    }

    SECTION("serialize color")
    {
        REQUIRE(tgui::Serializer::serialize(tgui::Color{100, 0, 50}) == "#640032");

        REQUIRE(tgui::Serializer::serialize(tgui::Color{1, 35, 69, 103}) == "#01234567");
        REQUIRE(tgui::Serializer::serialize(tgui::Color{137, 171, 205, 239}) == "#89ABCDEF");

        REQUIRE(tgui::Serializer::serialize(tgui::Color::Black) == "black");
        REQUIRE(tgui::Serializer::serialize(tgui::Color::White) == "white");
        REQUIRE(tgui::Serializer::serialize(tgui::Color::Red) == "red");
        REQUIRE(tgui::Serializer::serialize(tgui::Color::Green) == "green");
        REQUIRE(tgui::Serializer::serialize(tgui::Color::Blue) == "blue");
        REQUIRE(tgui::Serializer::serialize(tgui::Color::Magenta) == "magenta");
        REQUIRE(tgui::Serializer::serialize(tgui::Color::Yellow) == "yellow");
        REQUIRE(tgui::Serializer::serialize(tgui::Color::Cyan) == "cyan");
        REQUIRE(tgui::Serializer::serialize(tgui::Color::Transparent) == "transparent");
    }

    SECTION("serialize outline")
    {
        REQUIRE(tgui::Serializer::serialize(tgui::Outline{}) == "(0, 0, 0, 0)");
        REQUIRE(tgui::Serializer::serialize(tgui::Outline{10, 2, 50, 300}) == "(10, 2, 50, 300)");
    }

    SECTION("serialize texture")
    {
        tgui::Texture texture;

        texture.load("resources/image.png");
        REQUIRE(tgui::Serializer::serialize(texture) == "\"resources/image.png\" Smooth");

        texture.load("resources/image.png", {0, 0, 25, 25});
        REQUIRE(tgui::Serializer::serialize(texture) == "\"resources/image.png\" Part(0, 0, 25, 25) Smooth");

        texture.load("resources/image.png", {}, {10, 10, 30, 30}, false);
        REQUIRE(tgui::Serializer::serialize(texture) == "\"resources/image.png\" Middle(10, 10, 30, 30) NoSmooth");

        texture.load("resources/image.png", {0, 0, 40, 40}, {10, 10, 20, 20});
        REQUIRE(tgui::Serializer::serialize(texture) == "\"resources/image.png\" Part(0, 0, 40, 40) Middle(10, 10, 20, 20) Smooth");

        std::size_t bytesCount;
        auto bytes = tgui::readFileToMemory("resources/image.png", bytesCount);
        REQUIRE(bytes);
        REQUIRE(bytesCount > 0);

        tgui::Texture textureInMemory;
        textureInMemory.loadFromMemory(bytes.get(), bytesCount);
        REQUIRE(tgui::Serializer::serialize(textureInMemory).starts_with("\"data:image/png;base64,"));
        REQUIRE(tgui::Serializer::serialize(textureInMemory).ends_with("\" Smooth"));
    }

    SECTION("serialize string")
    {
        REQUIRE(tgui::Serializer::serialize(tgui::String{""}) == "\"\"");
        REQUIRE(tgui::Serializer::serialize(tgui::String{"word"}) == "word");
        REQUIRE(tgui::Serializer::serialize(tgui::String{"NoQuotes_Required@089%"}) == "NoQuotes_Required@089%");
        REQUIRE(tgui::Serializer::serialize(tgui::String{"String with spaces"}) == "\"String with spaces\"");
        REQUIRE(tgui::Serializer::serialize(tgui::String{"\\"}) == "\"\\\\\"");
        REQUIRE(tgui::Serializer::serialize(tgui::String{"\n"}) == "\"\\n\"");
        REQUIRE(tgui::Serializer::serialize(tgui::String{"\t"}) == "\"\\t\"");
        REQUIRE(tgui::Serializer::serialize(tgui::String{"\v"}) == "\"\\v\"");
        REQUIRE(tgui::Serializer::serialize(tgui::String{"\""}) == "\"\\\"\"");
        REQUIRE(tgui::Serializer::serialize(tgui::String{"a\t\"string\"?\nYES!"}) == "\"a\\t\\\"string\\\"?\\nYES!\"");
    }

    SECTION("serialize number")
    {
        REQUIRE(tgui::Serializer::serialize({0}) == "0");
        REQUIRE(tgui::Serializer::serialize({1}) == "1");
        REQUIRE(tgui::Serializer::serialize({2.15f}) == "2.15");
    }

    SECTION("serialize text style")
    {
        REQUIRE(tgui::Serializer::serialize(tgui::TextStyles{tgui::TextStyle::Regular}) == "Regular");
        REQUIRE(tgui::Serializer::serialize(tgui::TextStyles{tgui::TextStyle::Bold}) == "Bold");
        REQUIRE(tgui::Serializer::serialize(tgui::TextStyles{tgui::TextStyle::Italic | tgui::TextStyle::Underlined}) == "Italic | Underlined");
        REQUIRE(tgui::Serializer::serialize(tgui::TextStyles{"bold"}) == "Bold");
        REQUIRE(tgui::Serializer::serialize(tgui::TextStyles{"italic | underlined"}) == "Italic | Underlined");

        REQUIRE(tgui::Serializer::serialize(tgui::TextStyles{256}) == "Regular");
    }

    SECTION("serialize renderer")
    {
        auto childRendererData = std::make_shared<tgui::RendererData>();
        childRendererData->propertyValuePairs["Num"] = {5};

        auto rendererData = std::make_shared<tgui::RendererData>();
        rendererData->propertyValuePairs["SomeColor"] = {tgui::Color::Red};
        rendererData->propertyValuePairs["TextStyleProperty"] = {tgui::TextStyle::StrikeThrough};
        rendererData->propertyValuePairs["Nested"] = {childRendererData};

        tgui::String result = "SomeColor = red;\n"
                             "TextStyleProperty = StrikeThrough;\n"
                             "\n"
                             "Nested {\n"
                             "    Num = 5;\n"
                             "}\n";

        REQUIRE(tgui::Serializer::serialize(rendererData) == result);
    }

    SECTION("custom serialize function")
    {
        REQUIRE(tgui::Serializer::serialize(tgui::Color::Blue) == "blue");
        auto oldFunc = tgui::Serializer::getFunction(tgui::ObjectConverter::Type::Color);

        tgui::Serializer::setFunction(tgui::ObjectConverter::Type::Color, [](tgui::ObjectConverter&&){ return "STR"; });
        REQUIRE(tgui::Serializer::serialize(tgui::Color::Blue) == "STR");
        REQUIRE(tgui::Serializer::serialize(tgui::Outline{10, 2, 50, 300}) == "(10, 2, 50, 300)");
        
        tgui::Serializer::setFunction(tgui::ObjectConverter::Type::Color, oldFunc);
        REQUIRE(tgui::Serializer::serialize(tgui::Color::Blue) == "blue");
    }
}
