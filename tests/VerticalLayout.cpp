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

#include "Tests.hpp"
#include <TGUI/VerticalLayout.hpp>
#include <TGUI/Widgets/Button.hpp>

TEST_CASE("[VerticalLayout]") {
    auto layout = std::make_shared<tgui::VerticalLayout>();
    layout->setSize(1000, 800);
    layout->setPosition(50, 40);

    auto button1 = std::make_shared<tgui::Button>();
    layout->add(button1);

    REQUIRE(button1->getFullSize() == sf::Vector2f(1000, 800));

    auto button2 = std::make_shared<tgui::Button>();
    layout->add(button2);

    REQUIRE(button1->getFullSize() == sf::Vector2f(1000, 400));
    REQUIRE(button2->getFullSize() == sf::Vector2f(1000, 400));

    layout->setRatio(button2, 3);
    REQUIRE(button1->getFullSize() == sf::Vector2f(1000, 200));
    REQUIRE(button2->getFullSize() == sf::Vector2f(1000, 600));
    
    layout->insertSpace(1, 1.5);
    layout->setRatio(0, 3.5);
    REQUIRE(button1->getFullSize() == sf::Vector2f(1000, 350));
    REQUIRE(button2->getFullSize() == sf::Vector2f(1000, 300));

    auto button3 = std::make_shared<tgui::Button>();
    layout->add(button3);

    REQUIRE(layout->getRatio(button1) == 3.5);
    REQUIRE(layout->getRatio(button2) == 3);
    REQUIRE(layout->getRatio(button3) == 1);
    REQUIRE(layout->getRatio(nullptr) == 0);
    REQUIRE(layout->getRatio(std::make_shared<tgui::Button>()) == 0);
    REQUIRE(layout->getRatio(0) == 3.5);
    REQUIRE(layout->getRatio(1) == 1.5);
    REQUIRE(layout->getRatio(2) == 3);
    REQUIRE(layout->getRatio(3) == 1);
    REQUIRE(layout->getRatio(4) == 0);

    // TODO: Add fixed size tests
}
