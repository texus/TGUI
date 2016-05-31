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

#include "../Tests.hpp"
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/Button.hpp>

TEST_CASE("[Panel]") {
    tgui::Panel::Ptr panel = std::make_shared<tgui::Panel>();
    panel->setFont("resources/DroidSansArmenian.ttf");

    SECTION("Signals") {
        REQUIRE_NOTHROW(panel->connect("MousePressed", [](){}));
        REQUIRE_NOTHROW(panel->connect("MouseReleased", [](){}));
        REQUIRE_NOTHROW(panel->connect("Clicked", [](){}));

        REQUIRE_NOTHROW(panel->connect("MousePressed", [](sf::Vector2f){}));
        REQUIRE_NOTHROW(panel->connect("MouseReleased", [](sf::Vector2f){}));
        REQUIRE_NOTHROW(panel->connect("Clicked", [](sf::Vector2f){}));
    }

    SECTION("WidgetType") {
        REQUIRE(panel->getWidgetType() == "Panel");
    }

    SECTION("BackgroundColor") {
        panel->setBackgroundColor(sf::Color(10, 20, 30));
        REQUIRE(panel->getBackgroundColor() == sf::Color(10, 20, 30));
    }

    SECTION("Renderer") {
        auto renderer = panel->getRenderer();

        SECTION("set serialized property") {
            REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(10, 20, 30)"));
            REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(40, 50, 60)"));
            REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
        }

        SECTION("set object property") {
            REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{10, 20, 30}));
            REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{40, 50, 60}));
            REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
        }

        SECTION("functions") {
            renderer->setBackgroundColor({10, 20, 30});
            renderer->setBorderColor({40, 50, 60});
            renderer->setBorders({1, 2, 3, 4});

            SECTION("getPropertyValuePairs") {
                auto pairs = renderer->getPropertyValuePairs();
                REQUIRE(pairs.size() == 3);
                REQUIRE(pairs["BackgroundColor"].getColor() == sf::Color(10, 20, 30));
                REQUIRE(pairs["BorderColor"].getColor() == sf::Color(40, 50, 60));
                REQUIRE(pairs["Borders"].getBorders() == tgui::Borders(1, 2, 3, 4));
            }
        }

        REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(10, 20, 30));
        REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(40, 50, 60));
        REQUIRE(renderer->getProperty("Borders").getBorders() == tgui::Borders(1, 2, 3, 4));
    }

    SECTION("Saving and loading from file") {
        panel = std::make_shared<tgui::Panel>(400.f, 300.f);

        SECTION("independent panel") {
            REQUIRE_NOTHROW(panel->saveWidgetsToFile("WidgetFilePanel1.txt"));
            
            panel->setSize(200, 100);
            REQUIRE_NOTHROW(panel->loadWidgetsFromFile("WidgetFilePanel1.txt"));
            REQUIRE(panel->getSize() == sf::Vector2f(200, 100)); // The Panel itself is not saved, only its children

            REQUIRE_NOTHROW(panel->saveWidgetsToFile("WidgetFilePanel2.txt"));
            REQUIRE(compareFiles("WidgetFilePanel1.txt", "WidgetFilePanel2.txt"));
        }

        SECTION("panel inside gui") {
            auto parent = std::make_shared<tgui::GuiContainer>();
            parent->add(panel);

            tgui::Button::Ptr button = std::make_shared<tgui::Button>();
            button->setPosition(40, 20);
            panel->add(button);

            panel->setOpacity(0.8f);
            panel->setBackgroundColor(sf::Color::Green);

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFilePanel3.txt"));

            parent->removeAllWidgets();
            REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFilePanel3.txt"));

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFilePanel4.txt"));
            REQUIRE(compareFiles("WidgetFilePanel3.txt", "WidgetFilePanel4.txt"));

            SECTION("Copying widget") {
                tgui::Panel temp;
                temp = *panel;

                parent->removeAllWidgets();
                parent->add(tgui::Panel::copy(std::make_shared<tgui::Panel>(temp)));

                REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFilePanel4.txt"));
                REQUIRE(compareFiles("WidgetFilePanel3.txt", "WidgetFilePanel4.txt"));
            }
        }
    }
}
