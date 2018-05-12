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
#include <TGUI/Widgets/HorizontalLayout.hpp>
#include <TGUI/Widgets/Button.hpp>

TEST_CASE("[HorizontalLayout]")
{
    auto layout = tgui::HorizontalLayout::create();
    layout->setSize(800, 1000);
    layout->setPosition(40, 50);

    SECTION("Positions and sizes")
    {
        auto button1 = tgui::Button::create();
        layout->add(button1);

        REQUIRE(button1->getPosition() == sf::Vector2f(0, 0));
        REQUIRE(button1->getFullSize() == sf::Vector2f(800, 1000));

        auto button2 = tgui::Button::create();
        layout->add(button2);

        REQUIRE(button2->getPosition() == sf::Vector2f(400, 0));
        REQUIRE(button1->getFullSize() == sf::Vector2f(400, 1000));
        REQUIRE(button2->getFullSize() == sf::Vector2f(400, 1000));

        layout->getRenderer()->setPadding({10});
        REQUIRE(button1->getPosition() == sf::Vector2f(0, 0));
        REQUIRE(button2->getPosition() == sf::Vector2f(395, 0));
        REQUIRE(button1->getFullSize() == sf::Vector2f(385, 980));
        REQUIRE(button2->getFullSize() == sf::Vector2f(385, 980));

        layout->getRenderer()->setSpaceBetweenWidgets(30);
        REQUIRE(button1->getPosition() == sf::Vector2f(0, 0));
        REQUIRE(button2->getPosition() == sf::Vector2f(405, 0));
        REQUIRE(button1->getFullSize() == sf::Vector2f(375, 980));
        REQUIRE(button2->getFullSize() == sf::Vector2f(375, 980));

        SECTION("Spaces and ratios")
        {
            layout->addSpace(0.5f);

            auto button3 = tgui::Button::create();
            layout->add(button3, 20);

            REQUIRE(layout->getRatio(0) == 1.f);
            REQUIRE(layout->getRatio(button1) == 1.f);
            REQUIRE(layout->getRatio(2) == 0.5f);
            REQUIRE(layout->getRatio(3) == 20);
            REQUIRE(layout->getRatio(button3) == 20);
            REQUIRE(layout->setRatio(button3, 2));
            REQUIRE(layout->getRatio(button3) == 2);

            REQUIRE(!layout->setRatio(10, 1));
            REQUIRE(layout->getRatio(10) == 0);
            REQUIRE(!layout->setRatio(nullptr, 1));
            REQUIRE(layout->getRatio(nullptr) == 0);

            REQUIRE(layout->get(0)->getFullSize() == sf::Vector2f((800 - 20 - 3*30) * (1 / 4.5f), 980));
            REQUIRE(layout->get(1)->getFullSize() == sf::Vector2f((800 - 20 - 3*30) * (1 / 4.5f), 980));
            REQUIRE(layout->get(2)->getFullSize() == sf::Vector2f((800 - 20 - 3*30) * (0.5f / 4.5f), 980));
            REQUIRE(layout->get(3)->getFullSize() == sf::Vector2f((800 - 20 - 3*30) * (2 / 4.5f), 980));
            REQUIRE(layout->get(0)->getPosition() == sf::Vector2f(0, 0));
            REQUIRE(layout->get(1)->getPosition() == sf::Vector2f(layout->get(0)->getPosition().x + layout->get(0)->getFullSize().x + 30, 0));
            REQUIRE(layout->get(2)->getPosition() == sf::Vector2f(layout->get(1)->getPosition().x + layout->get(1)->getFullSize().x + 30, 0));
            REQUIRE(layout->get(3)->getPosition() == sf::Vector2f(layout->get(2)->getPosition().x + layout->get(2)->getFullSize().x + 30, 0));

            layout->insertSpace(1, 1.5f);
            layout->insertSpace(3, 2.5f);
            layout->remove(4);

            auto button4 = tgui::Button::create();
            layout->insert(2, button4);
            layout->remove(2);

            REQUIRE(layout->get(0)->getFullSize() == sf::Vector2f((800 - 20 - 4*30) * (1 / 8.f), 980));
            REQUIRE(layout->get(1)->getFullSize() == sf::Vector2f((800 - 20 - 4*30) * (1.5f / 8.f), 980));
            REQUIRE(layout->get(2)->getFullSize() == sf::Vector2f((800 - 20 - 4*30) * (1 / 8.f), 980));
            REQUIRE(layout->get(3)->getFullSize() == sf::Vector2f((800 - 20 - 4*30) * (2.5f / 8.f), 980));
            REQUIRE(layout->get(4)->getFullSize() == sf::Vector2f((800 - 20 - 4*30) * (2 / 8.f), 980));
            REQUIRE(layout->get(0)->getPosition() == sf::Vector2f(0, 0));
            REQUIRE(layout->get(1)->getPosition() == sf::Vector2f(layout->get(0)->getPosition().x + layout->get(0)->getFullSize().x + 30, 0));
            REQUIRE(layout->get(2)->getPosition() == sf::Vector2f(layout->get(1)->getPosition().x + layout->get(1)->getFullSize().x + 30, 0));
            REQUIRE(layout->get(3)->getPosition() == sf::Vector2f(layout->get(2)->getPosition().x + layout->get(2)->getFullSize().x + 30, 0));
            REQUIRE(layout->get(4)->getPosition() == sf::Vector2f(layout->get(3)->getPosition().x + layout->get(3)->getFullSize().x + 30, 0));
        }
    }

    SECTION("Order")
    {
        REQUIRE(layout->get(0) == nullptr);

        layout->add(tgui::Button::create(), "1");
        layout->add(tgui::Button::create(), "2");
        layout->insert(1, tgui::Button::create(), "3");
        layout->insert(0, tgui::Button::create(), "4");
        layout->insert(4, tgui::Button::create(), "5");

        REQUIRE(layout->getWidgetName(layout->get(0)) == "4");
        REQUIRE(layout->getWidgetName(layout->get(1)) == "1");
        REQUIRE(layout->getWidgetName(layout->get(2)) == "3");
        REQUIRE(layout->getWidgetName(layout->get(3)) == "2");
        REQUIRE(layout->getWidgetName(layout->get(4)) == "5");
        REQUIRE(layout->get(5) == nullptr);

        REQUIRE(layout->remove(layout->get("2")) == true);
        REQUIRE(layout->remove(1) == true);
        REQUIRE(layout->remove(5) == false);
        REQUIRE(layout->remove(tgui::Button::create()) == false);

        REQUIRE(layout->getWidgetName(layout->get(0)) == "4");
        REQUIRE(layout->getWidgetName(layout->get(1)) == "3");
        REQUIRE(layout->getWidgetName(layout->get(2)) == "5");
        REQUIRE(layout->get(5) == nullptr);

        layout->removeAllWidgets();
        REQUIRE(layout->get(0) == nullptr);
    }

    SECTION("Saving and loading from file")
    {
        tgui::Theme::getDefault()->addRenderer("Button", std::make_shared<tgui::RendererData>());

        layout->add(tgui::Button::create("Hello"));
        layout->addSpace(0.5);
        layout->add(tgui::Button::create("Hi"), 3, "Button2");

        testSavingWidget("HorizontalLayout", layout, false);
    }
}
