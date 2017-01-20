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

#include "../Tests.hpp"
#include <TGUI/Widgets/HorizontalLayout.hpp>
#include <TGUI/Widgets/Button.hpp>

TEST_CASE("[HorizontalLayout]")
{
    auto layout = tgui::HorizontalLayout::create();
    layout->setSize(800, 100);
    layout->setPosition(50, 40);

    auto button1 = tgui::Button::create();
    layout->add(button1);

    REQUIRE(button1->getFullSize() == sf::Vector2f(800, 100));

    auto button2 = tgui::Button::create();
    layout->add(button2);

    REQUIRE(button1->getFullSize() == sf::Vector2f(400, 100));
    REQUIRE(button2->getFullSize() == sf::Vector2f(400, 100));

    layout->setRatio(button2, 3);
    REQUIRE(button1->getFullSize() == sf::Vector2f(200, 100));
    REQUIRE(button2->getFullSize() == sf::Vector2f(600, 100));

    layout->insertSpace(1, 1.5);
    layout->setRatio(0, 3.5);
    REQUIRE(button1->getFullSize() == sf::Vector2f(350, 100));
    REQUIRE(button2->getFullSize() == sf::Vector2f(300, 100));

    auto button3 = tgui::Button::create();
    layout->add(button3);

    REQUIRE(layout->getRatio(button1) == 3.5);
    REQUIRE(layout->getRatio(button2) == 3);
    REQUIRE(layout->getRatio(button3) == 1);
    REQUIRE(layout->getRatio(nullptr) == 0);
    REQUIRE(layout->getRatio(tgui::Button::create()) == 0);
    REQUIRE(layout->getRatio(0) == 3.5);
    REQUIRE(layout->getRatio(1) == 1.5);
    REQUIRE(layout->getRatio(2) == 3);
    REQUIRE(layout->getRatio(3) == 1);
    REQUIRE(layout->getRatio(4) == 0);

    // TODO: Add fixed size tests
}
