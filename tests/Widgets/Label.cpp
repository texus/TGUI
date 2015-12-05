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
#include <TGUI/Widgets/Label.hpp>

TEST_CASE("[Label]") {
    tgui::Label::Ptr label = std::make_shared<tgui::Label>();
    label->setFont("resources/DroidSansArmenian.ttf");

    SECTION("Signals") {
        REQUIRE_NOTHROW(label->connect("DoubleClicked", [](){}));
        REQUIRE_NOTHROW(label->connect("DoubleClicked", [](sf::String){}));
    }

    SECTION("WidgetType") {
        REQUIRE(label->getWidgetType() == "Label");
    }

    SECTION("Text") {
        REQUIRE(label->getText() == "");
        label->setText("SomeText");
        REQUIRE(label->getText() == "SomeText");
    }

    SECTION("TextStyle") {
        REQUIRE(label->getTextStyle() == sf::Text::Regular);
        label->setTextStyle(sf::Text::Bold | sf::Text::Italic);
        REQUIRE(label->getTextStyle() == (sf::Text::Bold | sf::Text::Italic));
    }

    SECTION("TextSize") {
        label->setTextSize(25);
        REQUIRE(label->getTextSize() == 25);
    }

    SECTION("AutoSize") {
        REQUIRE(label->getAutoSize());
        label->setAutoSize(false);
        REQUIRE(!label->getAutoSize());
        label->setAutoSize(true);
        REQUIRE(label->getAutoSize());
        label->setSize(200, 100);
        REQUIRE(!label->getAutoSize());
    }

    SECTION("MaximumTextWidth") {
        REQUIRE(label->getMaximumTextWidth() == 0);
        label->setMaximumTextWidth(300);
        REQUIRE(label->getMaximumTextWidth() == 300);
    }

    SECTION("Renderer") {
        auto renderer = label->getRenderer();

        SECTION("set serialized property") {
            REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(100, 50, 150)"));
            REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(150, 100, 50)"));
            REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(50, 150, 100)"));
            REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
            REQUIRE_NOTHROW(renderer->setProperty("Padding", "(5, 6, 7, 8)"));
        }
        
        SECTION("set object property") {
            REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{100, 50, 150}));
            REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{150, 100, 50}));
            REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{50, 150, 100}));
            REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
            REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Borders{5, 6, 7, 8}));
        }
        
        SECTION("functions") {
            renderer->setTextColor({100, 50, 150});
            renderer->setBackgroundColor({150, 100, 50});
            renderer->setBorderColor({50, 150, 100});
            renderer->setBorders({1, 2, 3, 4});
            renderer->setPadding({5, 6, 7, 8});
            
            SECTION("getPropertyValuePairs") {
                auto pairs = renderer->getPropertyValuePairs();
                REQUIRE(pairs.size() == 5);
                REQUIRE(pairs["TextColor"].getColor() == sf::Color(100, 50, 150));
                REQUIRE(pairs["BackgroundColor"].getColor() == sf::Color(150, 100, 50));
                REQUIRE(pairs["BorderColor"].getColor() == sf::Color(50, 150, 100));
                REQUIRE(pairs["Borders"].getBorders() == tgui::Borders(1, 2, 3, 4));
                REQUIRE(pairs["Padding"].getBorders() == tgui::Borders(5, 6, 7, 8));
            }
        }

        REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(100, 50, 150));
        REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(150, 100, 50));
        REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(50, 150, 100));
        REQUIRE(renderer->getProperty("Borders").getBorders() == tgui::Borders(1, 2, 3, 4));
        REQUIRE(renderer->getProperty("Padding").getBorders() == tgui::Borders(5, 6, 7, 8));
    }

    SECTION("Saving and loading from file") {
        REQUIRE_NOTHROW(label = std::make_shared<tgui::Theme>()->load("Label"));

        auto theme = std::make_shared<tgui::Theme>("resources/Black.txt");
        REQUIRE_NOTHROW(label = theme->load("Label"));
        REQUIRE(label->getPrimaryLoadingParameter() == "resources/Black.txt");
        REQUIRE(label->getSecondaryLoadingParameter() == "label");

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(label);

        label->setOpacity(0.8f);
        label->setText("SomeText");
        label->setTextSize(25);
        label->setTextStyle(sf::Text::Bold | sf::Text::Italic);
        label->setMaximumTextWidth(300);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileLabel1.txt"));

        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileLabel1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileLabel2.txt"));
        REQUIRE(compareFiles("WidgetFileLabel1.txt", "WidgetFileLabel2.txt"));

        SECTION("Copying widget") {
            tgui::Label temp;
            temp = *label;

            parent->removeAllWidgets();
            parent->add(tgui::Label::copy(std::make_shared<tgui::Label>(temp)));

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileLabel2.txt"));
            REQUIRE(compareFiles("WidgetFileLabel1.txt", "WidgetFileLabel2.txt"));
        }
    }
}
