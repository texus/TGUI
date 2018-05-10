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
#include <TGUI/Sprite.hpp>

TEST_CASE("[Sprite]")
{
    tgui::Sprite sprite;

    SECTION("Setting texture")
    {
        SECTION("Empty texture")
        {
            sprite.setTexture({});

            REQUIRE(!sprite.isSet());
            REQUIRE(sprite.getTexture().getData() == nullptr);
            REQUIRE(sprite.getSize() == sf::Vector2f(0, 0));
        }

        SECTION("Loaded texture")
        {
            sprite.setTexture({"resources/image.png"});

            REQUIRE(sprite.isSet());
            REQUIRE(sprite.getTexture().getData() != nullptr);
            REQUIRE(sprite.getSize() == sf::Vector2f(50, 50));
        }

        REQUIRE(sprite.getColor() == sf::Color::White);
        REQUIRE(sprite.getVisibleRect() == sf::FloatRect());
        REQUIRE(sprite.getScalingType() == tgui::Sprite::ScalingType::Normal);
    }

    SECTION("Size")
    {
        sprite.setTexture({});
        REQUIRE(sprite.getSize() == sf::Vector2f());

        tgui::Texture texture{"resources/image.png"};
        sprite.setTexture(texture);
        REQUIRE(sprite.getSize() == sf::Vector2f(50, 50));

        sprite.setSize({80, 60});
        REQUIRE(sprite.getSize() == sf::Vector2f(80, 60));
        REQUIRE(texture.getImageSize() == sf::Vector2f(50, 50));

        sprite.setTexture({"resources/image.png"});
        REQUIRE(sprite.getSize() == sf::Vector2f(80, 60));
    }

    SECTION("Color")
    {
        sprite.setTexture({"resources/image.png"});
        REQUIRE(sprite.getColor() == sf::Color::White);

        sprite.setColor(sf::Color::Blue);
        REQUIRE(sprite.getColor() == sf::Color::Blue);
    }

    SECTION("Opacity")
    {
        sprite.setTexture("resources/image.png");
        REQUIRE(sprite.getOpacity() == 1.f);

        sprite.setOpacity(0.6f);
        REQUIRE(sprite.getOpacity() == 0.6f);
    }

    SECTION("VisibleRect")
    {
        REQUIRE(sprite.getVisibleRect() == sf::FloatRect());

        sprite.setTexture({"resources/image.png"});
        REQUIRE(sprite.getVisibleRect() == sf::FloatRect());

        sprite.setVisibleRect({10, 10, 30, 30});
        REQUIRE(sprite.getVisibleRect() == sf::FloatRect(10, 10, 30, 30));
    }

    SECTION("isTransparentPixel")
    {
        sprite.setPosition({10, 20});

        SECTION("Invalid image")
        {
            tgui::Sprite unloadedSprite;
            REQUIRE(unloadedSprite.isTransparentPixel({0, 0}) == true);
        }

        SECTION("Normal Scaling")
        {
            sprite.setTexture({"resources/TransparentParts.png", {10, 10, 30, 30}});
            sprite.setSize({60, 15});
            REQUIRE(sprite.getScalingType() == tgui::Sprite::ScalingType::Normal);

            REQUIRE(!sprite.isTransparentPixel({21, 22.5f}));
            REQUIRE(sprite.isTransparentPixel({22, 23}));
            REQUIRE(!sprite.isTransparentPixel({21, 27.5f}));
            REQUIRE(sprite.isTransparentPixel({22, 27.5f}));
            REQUIRE(!sprite.isTransparentPixel({21, 32}));
            REQUIRE(sprite.isTransparentPixel({22, 31.5f}));
            REQUIRE(!sprite.isTransparentPixel({40, 32}));
            REQUIRE(sprite.isTransparentPixel({40, 31.5f}));
            REQUIRE(!sprite.isTransparentPixel({58, 32}));
            REQUIRE(sprite.isTransparentPixel({57, 31.5f}));
            REQUIRE(!sprite.isTransparentPixel({58, 27.5f}));
            REQUIRE(sprite.isTransparentPixel({57, 27.5f}));
            REQUIRE(!sprite.isTransparentPixel({58, 22.5f}));
            REQUIRE(sprite.isTransparentPixel({57, 23}));
            REQUIRE(!sprite.isTransparentPixel({40, 22.5f}));
            REQUIRE(sprite.isTransparentPixel({40, 23}));
            REQUIRE(sprite.isTransparentPixel({40, 27.5f}));

            // Corner pixels
            REQUIRE(!sprite.isTransparentPixel({10, 20}));
            REQUIRE(!sprite.isTransparentPixel({10, 34.5f}));
            REQUIRE(!sprite.isTransparentPixel({69, 34.5f}));
            REQUIRE(!sprite.isTransparentPixel({69, 20}));
        }

        SECTION("Horizontal Scaling")
        {
            sprite.setTexture({"resources/TransparentParts.png", {0, 10, 50, 30}, {10, 0, 30, 30}});
            sprite.setSize({70, 15});
            REQUIRE(sprite.getScalingType() == tgui::Sprite::ScalingType::Horizontal);

            // Left part
            REQUIRE(!sprite.isTransparentPixel({10.5f, 24.5f}));
            REQUIRE(sprite.isTransparentPixel({11, 25}));
            REQUIRE(!sprite.isTransparentPixel({10.5f, 27}));
            REQUIRE(sprite.isTransparentPixel({11, 27}));
            REQUIRE(!sprite.isTransparentPixel({10.5f, 30}));
            REQUIRE(sprite.isTransparentPixel({11, 29.5f}));
            REQUIRE(!sprite.isTransparentPixel({12, 30}));
            REQUIRE(sprite.isTransparentPixel({12, 29.5f}));
            REQUIRE(!sprite.isTransparentPixel({14, 30}));
            REQUIRE(sprite.isTransparentPixel({13.5f, 29.5f}));
            REQUIRE(!sprite.isTransparentPixel({14, 27}));
            REQUIRE(sprite.isTransparentPixel({13.5f, 27}));
            REQUIRE(!sprite.isTransparentPixel({14, 24.5}));
            REQUIRE(sprite.isTransparentPixel({13.5f, 25}));
            REQUIRE(!sprite.isTransparentPixel({12, 24.5f}));
            REQUIRE(sprite.isTransparentPixel({12, 25}));
            REQUIRE(sprite.isTransparentPixel({12, 27}));

            // Middle part
            REQUIRE(!sprite.isTransparentPixel({26, 22.5f}));
            REQUIRE(sprite.isTransparentPixel({27, 23}));
            REQUIRE(!sprite.isTransparentPixel({26, 27.5f}));
            REQUIRE(sprite.isTransparentPixel({27, 27.5f}));
            REQUIRE(!sprite.isTransparentPixel({26, 32}));
            REQUIRE(sprite.isTransparentPixel({27, 31.5f}));
            REQUIRE(!sprite.isTransparentPixel({44, 32}));
            REQUIRE(sprite.isTransparentPixel({44, 31.5f}));
            REQUIRE(!sprite.isTransparentPixel({63, 32}));
            REQUIRE(sprite.isTransparentPixel({62, 31.5f}));
            REQUIRE(!sprite.isTransparentPixel({63, 27.5f}));
            REQUIRE(sprite.isTransparentPixel({62, 27.5f}));
            REQUIRE(!sprite.isTransparentPixel({63, 22.5f}));
            REQUIRE(sprite.isTransparentPixel({62, 23}));
            REQUIRE(!sprite.isTransparentPixel({44, 22.5f}));
            REQUIRE(sprite.isTransparentPixel({44, 23}));
            REQUIRE(sprite.isTransparentPixel({44, 27.5f}));

            // Right part
            REQUIRE(!sprite.isTransparentPixel({75.5f, 24.5f}));
            REQUIRE(sprite.isTransparentPixel({76, 25}));
            REQUIRE(!sprite.isTransparentPixel({75.5f, 27}));
            REQUIRE(sprite.isTransparentPixel({76, 27}));
            REQUIRE(!sprite.isTransparentPixel({75.5f, 30}));
            REQUIRE(sprite.isTransparentPixel({76, 29.5f}));
            REQUIRE(!sprite.isTransparentPixel({77, 30}));
            REQUIRE(sprite.isTransparentPixel({77, 29.5f}));
            REQUIRE(!sprite.isTransparentPixel({79, 30}));
            REQUIRE(sprite.isTransparentPixel({78.5f, 29.5f}));
            REQUIRE(!sprite.isTransparentPixel({79, 27}));
            REQUIRE(sprite.isTransparentPixel({78.5f, 27}));
            REQUIRE(!sprite.isTransparentPixel({79, 24.5}));
            REQUIRE(sprite.isTransparentPixel({78.5f, 25}));
            REQUIRE(!sprite.isTransparentPixel({77, 24.5f}));
            REQUIRE(sprite.isTransparentPixel({77, 25}));
            REQUIRE(sprite.isTransparentPixel({77, 27}));

            // Corner pixels
            REQUIRE(!sprite.isTransparentPixel({10, 20}));
            REQUIRE(!sprite.isTransparentPixel({10, 34.5f}));
            REQUIRE(!sprite.isTransparentPixel({79.5f, 34.5f}));
            REQUIRE(!sprite.isTransparentPixel({79.5f, 20}));
        }

        SECTION("Vertical Scaling")
        {
            sprite.setTexture({"resources/TransparentParts.png", {10, 0, 30, 50}, {0, 10, 30, 30}});
            sprite.setSize({15, 70});
            REQUIRE(sprite.getScalingType() == tgui::Sprite::ScalingType::Vertical);

            // Top part
            REQUIRE(!sprite.isTransparentPixel({14.5f, 20.5f}));
            REQUIRE(sprite.isTransparentPixel({15, 21}));
            REQUIRE(!sprite.isTransparentPixel({17, 20.5f}));
            REQUIRE(sprite.isTransparentPixel({17, 21}));
            REQUIRE(!sprite.isTransparentPixel({20, 20.5f}));
            REQUIRE(sprite.isTransparentPixel({19.5f, 21}));
            REQUIRE(!sprite.isTransparentPixel({20, 22}));
            REQUIRE(sprite.isTransparentPixel({19.5f, 22}));
            REQUIRE(!sprite.isTransparentPixel({20, 24}));
            REQUIRE(sprite.isTransparentPixel({19.5f, 23.5f}));
            REQUIRE(!sprite.isTransparentPixel({17, 24}));
            REQUIRE(sprite.isTransparentPixel({17, 23.5f}));
            REQUIRE(!sprite.isTransparentPixel({14.5, 24}));
            REQUIRE(sprite.isTransparentPixel({15, 23.5f}));
            REQUIRE(!sprite.isTransparentPixel({14.5f, 22}));
            REQUIRE(sprite.isTransparentPixel({15, 22}));
            REQUIRE(sprite.isTransparentPixel({17, 22}));

            // Middle part
            REQUIRE(!sprite.isTransparentPixel({12.5f, 36}));
            REQUIRE(sprite.isTransparentPixel({13, 37}));
            REQUIRE(!sprite.isTransparentPixel({17.5f, 36}));
            REQUIRE(sprite.isTransparentPixel({17.5f, 37}));
            REQUIRE(!sprite.isTransparentPixel({22, 36}));
            REQUIRE(sprite.isTransparentPixel({21.5f, 37}));
            REQUIRE(!sprite.isTransparentPixel({22, 54}));
            REQUIRE(sprite.isTransparentPixel({21.5f, 54}));
            REQUIRE(!sprite.isTransparentPixel({22, 73}));
            REQUIRE(sprite.isTransparentPixel({21.5f, 72}));
            REQUIRE(!sprite.isTransparentPixel({17.5f, 73}));
            REQUIRE(sprite.isTransparentPixel({17.5f, 72}));
            REQUIRE(!sprite.isTransparentPixel({12.5f, 73}));
            REQUIRE(sprite.isTransparentPixel({13, 72}));
            REQUIRE(!sprite.isTransparentPixel({12.5f, 54}));
            REQUIRE(sprite.isTransparentPixel({13, 54}));
            REQUIRE(sprite.isTransparentPixel({17.5f, 54}));

            // Bottom part
            REQUIRE(!sprite.isTransparentPixel({14.5f, 85.5f}));
            REQUIRE(sprite.isTransparentPixel({15, 86}));
            REQUIRE(!sprite.isTransparentPixel({17, 85.5f}));
            REQUIRE(sprite.isTransparentPixel({17, 86}));
            REQUIRE(!sprite.isTransparentPixel({20, 85.5f}));
            REQUIRE(sprite.isTransparentPixel({19.5f, 86}));
            REQUIRE(!sprite.isTransparentPixel({20, 87}));
            REQUIRE(sprite.isTransparentPixel({19.5f, 87}));
            REQUIRE(!sprite.isTransparentPixel({20, 89}));
            REQUIRE(sprite.isTransparentPixel({19.5f, 88.5f}));
            REQUIRE(!sprite.isTransparentPixel({17, 89}));
            REQUIRE(sprite.isTransparentPixel({17, 88.5f}));
            REQUIRE(!sprite.isTransparentPixel({14.5, 89}));
            REQUIRE(sprite.isTransparentPixel({15, 88.5f}));
            REQUIRE(!sprite.isTransparentPixel({14.5f, 87}));
            REQUIRE(sprite.isTransparentPixel({15, 87}));
            REQUIRE(sprite.isTransparentPixel({17, 87}));

            // Corner pixels
            REQUIRE(!sprite.isTransparentPixel({10, 20}));
            REQUIRE(!sprite.isTransparentPixel({24.5f, 20}));
            REQUIRE(!sprite.isTransparentPixel({24.5f, 89.5f}));
            REQUIRE(!sprite.isTransparentPixel({10, 89.5f}));
        }

        SECTION("9-Slice Scaling")
        {
            sprite.setTexture({"resources/TransparentParts.png", {}, {10, 10, 30, 30}});
            sprite.setSize({80, 35});
            REQUIRE(sprite.getScalingType() == tgui::Sprite::ScalingType::NineSlice);

            // Top left part
            REQUIRE(!sprite.isTransparentPixel({11, 21}));
            REQUIRE(sprite.isTransparentPixel({12, 22}));
            REQUIRE(!sprite.isTransparentPixel({11, 23.5f}));
            REQUIRE(sprite.isTransparentPixel({12, 23.5f}));
            REQUIRE(!sprite.isTransparentPixel({11, 26}));
            REQUIRE(sprite.isTransparentPixel({12, 25}));
            REQUIRE(!sprite.isTransparentPixel({13.5f, 26}));
            REQUIRE(sprite.isTransparentPixel({13.5f, 25}));
            REQUIRE(!sprite.isTransparentPixel({16, 26}));
            REQUIRE(sprite.isTransparentPixel({15, 25}));
            REQUIRE(!sprite.isTransparentPixel({16, 23.5f}));
            REQUIRE(sprite.isTransparentPixel({15, 23.5f}));
            REQUIRE(!sprite.isTransparentPixel({16, 21}));
            REQUIRE(sprite.isTransparentPixel({15, 22}));
            REQUIRE(!sprite.isTransparentPixel({13.5f, 21}));
            REQUIRE(sprite.isTransparentPixel({13.5f, 22}));
            REQUIRE(sprite.isTransparentPixel({13.5f, 23.5f}));

            // Top right part
            REQUIRE(!sprite.isTransparentPixel({88, 21}));
            REQUIRE(sprite.isTransparentPixel({87, 22}));
            REQUIRE(!sprite.isTransparentPixel({88, 23.5f}));
            REQUIRE(sprite.isTransparentPixel({87, 23.5f}));
            REQUIRE(!sprite.isTransparentPixel({88, 26}));
            REQUIRE(sprite.isTransparentPixel({87, 25}));
            REQUIRE(!sprite.isTransparentPixel({86.5f, 26}));
            REQUIRE(sprite.isTransparentPixel({86.5f, 25}));
            REQUIRE(!sprite.isTransparentPixel({83, 26}));
            REQUIRE(sprite.isTransparentPixel({84, 25}));
            REQUIRE(!sprite.isTransparentPixel({83, 23.5f}));
            REQUIRE(sprite.isTransparentPixel({84, 23.5f}));
            REQUIRE(!sprite.isTransparentPixel({83, 21}));
            REQUIRE(sprite.isTransparentPixel({84, 22}));
            REQUIRE(!sprite.isTransparentPixel({86.5f, 21}));
            REQUIRE(sprite.isTransparentPixel({86.5f, 22}));
            REQUIRE(sprite.isTransparentPixel({86.5f, 23.5f}));

            // Bottom left part
            REQUIRE(!sprite.isTransparentPixel({11, 53}));
            REQUIRE(sprite.isTransparentPixel({12, 52}));
            REQUIRE(!sprite.isTransparentPixel({11, 50.5f}));
            REQUIRE(sprite.isTransparentPixel({12, 50.5f}));
            REQUIRE(!sprite.isTransparentPixel({11, 48}));
            REQUIRE(sprite.isTransparentPixel({12, 49}));
            REQUIRE(!sprite.isTransparentPixel({13.5f, 48}));
            REQUIRE(sprite.isTransparentPixel({13.5f, 49}));
            REQUIRE(!sprite.isTransparentPixel({16, 48}));
            REQUIRE(sprite.isTransparentPixel({15, 49}));
            REQUIRE(!sprite.isTransparentPixel({16, 50.5f}));
            REQUIRE(sprite.isTransparentPixel({15, 50.5f}));
            REQUIRE(!sprite.isTransparentPixel({16, 53}));
            REQUIRE(sprite.isTransparentPixel({15, 52}));
            REQUIRE(!sprite.isTransparentPixel({13.5f, 53}));
            REQUIRE(sprite.isTransparentPixel({13.5f, 52}));
            REQUIRE(sprite.isTransparentPixel({13.5f, 50.5f}));

            // Bottom right part
            REQUIRE(!sprite.isTransparentPixel({88, 53}));
            REQUIRE(sprite.isTransparentPixel({87, 52}));
            REQUIRE(!sprite.isTransparentPixel({88, 50.5f}));
            REQUIRE(sprite.isTransparentPixel({87, 50.5f}));
            REQUIRE(!sprite.isTransparentPixel({88, 48}));
            REQUIRE(sprite.isTransparentPixel({87, 49}));
            REQUIRE(!sprite.isTransparentPixel({86.5f, 48}));
            REQUIRE(sprite.isTransparentPixel({86.5f, 49}));
            REQUIRE(!sprite.isTransparentPixel({83, 48}));
            REQUIRE(sprite.isTransparentPixel({84, 49}));
            REQUIRE(!sprite.isTransparentPixel({83, 50.5f}));
            REQUIRE(sprite.isTransparentPixel({84, 50.5f}));
            REQUIRE(!sprite.isTransparentPixel({83, 53}));
            REQUIRE(sprite.isTransparentPixel({84, 52}));
            REQUIRE(!sprite.isTransparentPixel({86.5f, 53}));
            REQUIRE(sprite.isTransparentPixel({86.5f, 52}));
            REQUIRE(sprite.isTransparentPixel({86.5f, 50.5f}));

            // Left part
            REQUIRE(!sprite.isTransparentPixel({11, 34.5f}));
            REQUIRE(sprite.isTransparentPixel({12, 35}));
            REQUIRE(!sprite.isTransparentPixel({11, 37}));
            REQUIRE(sprite.isTransparentPixel({12, 37}));
            REQUIRE(!sprite.isTransparentPixel({11, 40}));
            REQUIRE(sprite.isTransparentPixel({12, 39.5f}));
            REQUIRE(!sprite.isTransparentPixel({14.5f, 40}));
            REQUIRE(sprite.isTransparentPixel({14.5f, 39.5f}));
            REQUIRE(!sprite.isTransparentPixel({18, 40}));
            REQUIRE(sprite.isTransparentPixel({17, 39.5f}));
            REQUIRE(!sprite.isTransparentPixel({18, 37}));
            REQUIRE(sprite.isTransparentPixel({17, 37}));
            REQUIRE(!sprite.isTransparentPixel({18, 34.5f}));
            REQUIRE(sprite.isTransparentPixel({17, 35}));
            REQUIRE(!sprite.isTransparentPixel({14.5f, 34.5f}));
            REQUIRE(sprite.isTransparentPixel({14.5f, 35}));
            REQUIRE(sprite.isTransparentPixel({14.5f, 37}));

            // Right part
            REQUIRE(!sprite.isTransparentPixel({81, 34.5f}));
            REQUIRE(sprite.isTransparentPixel({82, 35}));
            REQUIRE(!sprite.isTransparentPixel({81, 37}));
            REQUIRE(sprite.isTransparentPixel({82, 37}));
            REQUIRE(!sprite.isTransparentPixel({81, 40}));
            REQUIRE(sprite.isTransparentPixel({82, 39.5f}));
            REQUIRE(!sprite.isTransparentPixel({84.5f, 40}));
            REQUIRE(sprite.isTransparentPixel({84.5f, 39.5f}));
            REQUIRE(!sprite.isTransparentPixel({88, 40}));
            REQUIRE(sprite.isTransparentPixel({87, 39.5f}));
            REQUIRE(!sprite.isTransparentPixel({88, 37}));
            REQUIRE(sprite.isTransparentPixel({87, 37}));
            REQUIRE(!sprite.isTransparentPixel({88, 34.5f}));
            REQUIRE(sprite.isTransparentPixel({87, 35}));
            REQUIRE(!sprite.isTransparentPixel({84.5f, 34.5f}));
            REQUIRE(sprite.isTransparentPixel({84.5f, 35}));
            REQUIRE(sprite.isTransparentPixel({84.5f, 37}));

            // Top part
            REQUIRE(!sprite.isTransparentPixel({39, 21}));
            REQUIRE(sprite.isTransparentPixel({40, 22}));
            REQUIRE(!sprite.isTransparentPixel({39, 24.5f}));
            REQUIRE(sprite.isTransparentPixel({40, 24.5f}));
            REQUIRE(!sprite.isTransparentPixel({39, 28}));
            REQUIRE(sprite.isTransparentPixel({40, 27}));
            REQUIRE(!sprite.isTransparentPixel({49, 28}));
            REQUIRE(sprite.isTransparentPixel({49, 27}));
            REQUIRE(!sprite.isTransparentPixel({60, 28}));
            REQUIRE(sprite.isTransparentPixel({59, 27}));
            REQUIRE(!sprite.isTransparentPixel({60, 24.5f}));
            REQUIRE(sprite.isTransparentPixel({59, 24.5f}));
            REQUIRE(!sprite.isTransparentPixel({60, 21}));
            REQUIRE(sprite.isTransparentPixel({59, 22}));
            REQUIRE(!sprite.isTransparentPixel({49, 21}));
            REQUIRE(sprite.isTransparentPixel({49, 22}));
            REQUIRE(sprite.isTransparentPixel({49, 24.5f}));

            // Bottom part
            REQUIRE(!sprite.isTransparentPixel({39, 46}));
            REQUIRE(sprite.isTransparentPixel({40, 47}));
            REQUIRE(!sprite.isTransparentPixel({39, 49.5f}));
            REQUIRE(sprite.isTransparentPixel({40, 49.5f}));
            REQUIRE(!sprite.isTransparentPixel({39, 53}));
            REQUIRE(sprite.isTransparentPixel({40, 52}));
            REQUIRE(!sprite.isTransparentPixel({49, 53}));
            REQUIRE(sprite.isTransparentPixel({49, 52}));
            REQUIRE(!sprite.isTransparentPixel({60, 53}));
            REQUIRE(sprite.isTransparentPixel({59, 52}));
            REQUIRE(!sprite.isTransparentPixel({60, 49.5f}));
            REQUIRE(sprite.isTransparentPixel({59, 49.5f}));
            REQUIRE(!sprite.isTransparentPixel({60, 46}));
            REQUIRE(sprite.isTransparentPixel({59, 47}));
            REQUIRE(!sprite.isTransparentPixel({49, 46}));
            REQUIRE(sprite.isTransparentPixel({49, 47}));
            REQUIRE(sprite.isTransparentPixel({49, 49.5f}));

            // Middle part
            REQUIRE(!sprite.isTransparentPixel({31, 32.5f}));
            REQUIRE(sprite.isTransparentPixel({32, 33}));
            REQUIRE(!sprite.isTransparentPixel({31, 37.5f}));
            REQUIRE(sprite.isTransparentPixel({32, 37.5f}));
            REQUIRE(!sprite.isTransparentPixel({31, 42}));
            REQUIRE(sprite.isTransparentPixel({32, 41.5f}));
            REQUIRE(!sprite.isTransparentPixel({49.5f, 42}));
            REQUIRE(sprite.isTransparentPixel({49.5f, 41.5f}));
            REQUIRE(!sprite.isTransparentPixel({68, 42}));
            REQUIRE(sprite.isTransparentPixel({67, 41.5f}));
            REQUIRE(!sprite.isTransparentPixel({68, 37.5f}));
            REQUIRE(sprite.isTransparentPixel({67, 37.5f}));
            REQUIRE(!sprite.isTransparentPixel({68, 32.5f}));
            REQUIRE(sprite.isTransparentPixel({67, 33}));
            REQUIRE(!sprite.isTransparentPixel({49.5f, 32.5f}));
            REQUIRE(sprite.isTransparentPixel({49.5f, 33}));
            REQUIRE(sprite.isTransparentPixel({49.5f, 37.5f}));

            // Corner pixels
            REQUIRE(!sprite.isTransparentPixel({10, 20}));
            REQUIRE(!sprite.isTransparentPixel({10, 54.5f}));
            REQUIRE(!sprite.isTransparentPixel({89, 54.5f}));
            REQUIRE(!sprite.isTransparentPixel({89, 20}));
        }
    }

    SECTION("Downgrading scaling type")
    {
        SECTION("Horizontal")
        {
            sprite.setTexture({"resources/image.png", {}, {10, 0, 30, 50}});
            REQUIRE(sprite.getScalingType() == tgui::Sprite::ScalingType::Horizontal);

            sprite.setSize({20, 60});
            REQUIRE(sprite.getScalingType() == tgui::Sprite::ScalingType::Normal);

            sprite.setSize({100, 30});
            REQUIRE(sprite.getScalingType() == tgui::Sprite::ScalingType::Horizontal);
        }

        SECTION("Vertical")
        {
            sprite.setTexture({"resources/image.png", {}, {0, 5, 50, 40}});
            REQUIRE(sprite.getScalingType() == tgui::Sprite::ScalingType::Vertical);

            sprite.setSize({110, 20});
            REQUIRE(sprite.getScalingType() == tgui::Sprite::ScalingType::Normal);

            sprite.setSize({300, 100});
            REQUIRE(sprite.getScalingType() == tgui::Sprite::ScalingType::Vertical);
        }

        SECTION("9-Slice")
        {
            sprite.setTexture({"resources/image.png", {}, {10, 5, 30, 40}});
            REQUIRE(sprite.getScalingType() == tgui::Sprite::ScalingType::NineSlice);

            sprite.setSize({9, 1});
            REQUIRE(sprite.getScalingType() == tgui::Sprite::ScalingType::Normal);

            sprite.setSize({20, 9});
            REQUIRE(sprite.getScalingType() == tgui::Sprite::ScalingType::Horizontal);

            sprite.setSize({19, 10});
            REQUIRE(sprite.getScalingType() == tgui::Sprite::ScalingType::Vertical);

            sprite.setSize({20, 10});
            REQUIRE(sprite.getScalingType() == tgui::Sprite::ScalingType::NineSlice);
        }
    }

    SECTION("getTexture has a version to change the texture and a const version")
    {
        sprite.getTexture().setSmooth(false);

        const tgui::Sprite sprite2{};
        sprite2.getTexture().isSmooth();
    }
}
