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

#include "Tests.hpp"

TEST_CASE("[Rect]")
{
    SECTION("FloatRect")
    {
        tgui::FloatRect rect(0.5f, 1.0f, 1.5f, 2.0f);
        REQUIRE(rect.getPosition() == tgui::Vector2f(0.5f, 1.0f));
        REQUIRE(rect.getSize() == tgui::Vector2f(1.5f, 2.0f));

        REQUIRE(tgui::FloatRect({0.5f, 1.0f}, {1.5f, 2.0f}) == rect);
        REQUIRE(tgui::FloatRect({1.5f, 2.0f}, {0.5f, 1.0f}) != rect);

        REQUIRE(tgui::FloatRect(tgui::IntRect({10, 20}, {30, 40})) == tgui::FloatRect(10, 20, 30, 40));
        REQUIRE(tgui::FloatRect(tgui::UIntRect({10, 20}, {30, 40})) == tgui::FloatRect(10, 20, 30, 40));

#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS && !TGUI_DISABLE_SFML_CONVERSIONS
        REQUIRE(tgui::FloatRect(sf::FloatRect(rect)) == rect);
#endif

        rect.setPosition({100, 200});
        REQUIRE(rect.getPosition() == tgui::Vector2f(100, 200));
        REQUIRE(rect.getSize() == tgui::Vector2f(1.5f, 2.0f));

        rect.setSize({300, 400});
        REQUIRE(rect.getPosition() == tgui::Vector2f(100, 200));
        REQUIRE(rect.getSize() == tgui::Vector2f(300, 400));

        REQUIRE(rect.contains({100, 200}));
        REQUIRE(rect.contains({250, 400}));
        REQUIRE(!rect.contains({400, 600}));
        REQUIRE(!rect.contains({250, 50}));
        REQUIRE(!rect.contains({50, 400}));
        REQUIRE(!rect.contains({250, 1000}));
        REQUIRE(!rect.contains({1000, 400}));

        REQUIRE(rect.intersects(rect));
        REQUIRE(rect.intersects({0, 0, 110, 210}));
        REQUIRE(rect.intersects({390, 590, 100, 100}));
        REQUIRE(rect.intersects({50, 300, 100, 100}));
        REQUIRE(rect.intersects({350, 300, 100, 100}));
        REQUIRE(rect.intersects({150, 150, 100, 100}));
        REQUIRE(rect.intersects({150, 550, 100, 100}));
        REQUIRE(!rect.intersects({0, 0, 100, 200}));
        REQUIRE(!rect.intersects({400, 600, 100, 100}));
    }

    SECTION("IntRect")
    {
        tgui::IntRect rect(10, 20, 30, 40);
        REQUIRE(rect.getPosition() == tgui::Vector2i(10, 20));
        REQUIRE(rect.getSize() == tgui::Vector2i(30, 40));

        REQUIRE(tgui::IntRect({10, 20}, {30, 40}) == rect);
        REQUIRE(tgui::IntRect({30, 40}, {10, 20}) != rect);

        REQUIRE(tgui::IntRect(tgui::FloatRect({10, 20}, {30, 40})) == tgui::IntRect(10, 20, 30, 40));
        REQUIRE(tgui::IntRect(tgui::UIntRect({10, 20}, {30, 40})) == tgui::IntRect(10, 20, 30, 40));

#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS && !TGUI_DISABLE_SFML_CONVERSIONS
        REQUIRE(tgui::IntRect(sf::IntRect(rect)) == rect);
#endif

        rect.setPosition({100, 200});
        REQUIRE(rect.getPosition() == tgui::Vector2i(100, 200));
        REQUIRE(rect.getSize() == tgui::Vector2i(30, 40));

        rect.setSize({300, 400});
        REQUIRE(rect.getPosition() == tgui::Vector2i(100, 200));
        REQUIRE(rect.getSize() == tgui::Vector2i(300, 400));

        REQUIRE(rect.contains({100, 200}));
        REQUIRE(rect.contains({250, 400}));
        REQUIRE(!rect.contains({400, 600}));
        REQUIRE(!rect.contains({250, 50}));
        REQUIRE(!rect.contains({50, 400}));
        REQUIRE(!rect.contains({250, 1000}));
        REQUIRE(!rect.contains({1000, 400}));

        REQUIRE(rect.intersects(rect));
        REQUIRE(rect.intersects({0, 0, 110, 210}));
        REQUIRE(rect.intersects({390, 590, 100, 100}));
        REQUIRE(rect.intersects({50, 300, 100, 100}));
        REQUIRE(rect.intersects({350, 300, 100, 100}));
        REQUIRE(rect.intersects({150, 150, 100, 100}));
        REQUIRE(rect.intersects({150, 550, 100, 100}));
        REQUIRE(!rect.intersects({0, 0, 100, 200}));
        REQUIRE(!rect.intersects({400, 600, 100, 100}));
    }

    SECTION("UIntRect")
    {
        tgui::UIntRect rect(10, 20, 30, 40);
        REQUIRE(rect.getPosition() == tgui::Vector2u(10, 20));
        REQUIRE(rect.getSize() == tgui::Vector2u(30, 40));

        REQUIRE(tgui::UIntRect({10, 20}, {30, 40}) == rect);
        REQUIRE(tgui::UIntRect({30, 40}, {10, 20}) != rect);

        REQUIRE(tgui::UIntRect(tgui::FloatRect({10, 20}, {30, 40})) == tgui::UIntRect(10, 20, 30, 40));
        REQUIRE(tgui::UIntRect(tgui::IntRect({10, 20}, {30, 40})) == tgui::UIntRect(10, 20, 30, 40));

#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS && !TGUI_DISABLE_SFML_CONVERSIONS
        REQUIRE(tgui::UIntRect(sf::Rect<unsigned int>(rect)) == rect);
#endif

        rect.setPosition({100, 200});
        REQUIRE(rect.getPosition() == tgui::Vector2u(100, 200));
        REQUIRE(rect.getSize() == tgui::Vector2u(30, 40));

        rect.setSize({300, 400});
        REQUIRE(rect.getPosition() == tgui::Vector2u(100, 200));
        REQUIRE(rect.getSize() == tgui::Vector2u(300, 400));

        REQUIRE(rect.contains({100, 200}));
        REQUIRE(rect.contains({250, 400}));
        REQUIRE(!rect.contains({400, 600}));
        REQUIRE(!rect.contains({250, 50}));
        REQUIRE(!rect.contains({50, 400}));
        REQUIRE(!rect.contains({250, 1000}));
        REQUIRE(!rect.contains({1000, 400}));

        REQUIRE(rect.intersects(rect));
        REQUIRE(rect.intersects({0, 0, 110, 210}));
        REQUIRE(rect.intersects({390, 590, 100, 100}));
        REQUIRE(rect.intersects({50, 300, 100, 100}));
        REQUIRE(rect.intersects({350, 300, 100, 100}));
        REQUIRE(rect.intersects({150, 150, 100, 100}));
        REQUIRE(rect.intersects({150, 550, 100, 100}));
        REQUIRE(!rect.intersects({0, 0, 100, 200}));
        REQUIRE(!rect.intersects({400, 600, 100, 100}));
    }
}
