/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Widgets/ClickableWidget.hpp>

TEST_CASE("[Panel]")
{
    tgui::Panel::Ptr panel = std::make_shared<tgui::Panel>();
    panel->getRenderer()->setFont("resources/DroidSansArmenian.ttf");

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(panel->connect("MousePressed", [](){}));
        REQUIRE_NOTHROW(panel->connect("MouseReleased", [](){}));
        REQUIRE_NOTHROW(panel->connect("Clicked", [](){}));

        REQUIRE_NOTHROW(panel->connect("MousePressed", [](sf::Vector2f){}));
        REQUIRE_NOTHROW(panel->connect("MouseReleased", [](sf::Vector2f){}));
        REQUIRE_NOTHROW(panel->connect("Clicked", [](sf::Vector2f){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(panel->getWidgetType() == "Panel");
    }

    SECTION("Position and Size")
    {
        panel->setPosition(40, 30);
        panel->setSize(150, 100);

        REQUIRE(panel->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(panel->getSize() == sf::Vector2f(150, 100));
        REQUIRE(panel->getFullSize() == panel->getSize());
        REQUIRE(panel->getWidgetOffset() == sf::Vector2f(0, 0));

        SECTION("Child widgets")
        {
            auto childWidget = std::make_shared<tgui::ClickableWidget>();
            childWidget->setPosition(60, 50);
            panel->add(childWidget);

            REQUIRE(childWidget->getPosition() == sf::Vector2f(60, 50));
            REQUIRE(childWidget->getAbsolutePosition() == sf::Vector2f(100, 80));

            panel->getRenderer()->setBorders({1, 2, 3, 4});
            REQUIRE(childWidget->getPosition() == sf::Vector2f(60, 50));
            REQUIRE(childWidget->getAbsolutePosition() == sf::Vector2f(101, 82));
        }
    }

    SECTION("Events")
    {
        unsigned int mousePressedCount = 0;
        unsigned int mouseReleasedCount = 0;
        unsigned int clickedCount = 0;

        panel->setPosition(40, 30);
        panel->setSize(150, 100);

        panel->connect("MousePressed", mouseCallback, std::ref(mousePressedCount));
        panel->connect("MouseReleased", mouseCallback, std::ref(mouseReleasedCount));
        panel->connect("Clicked", mouseCallback, std::ref(clickedCount));

        SECTION("mouseOnWidget")
        {
            REQUIRE(!panel->mouseOnWidget(10, 15));
            REQUIRE(panel->mouseOnWidget(40, 30));
            REQUIRE(panel->mouseOnWidget(115, 80));
            REQUIRE(panel->mouseOnWidget(189, 129));
            REQUIRE(!panel->mouseOnWidget(190, 130));

            REQUIRE(mousePressedCount == 0);
            REQUIRE(mouseReleasedCount == 0);
            REQUIRE(clickedCount == 0);
        }

        SECTION("mouse move")
        {
            unsigned int mouseEnteredCount = 0;
            unsigned int mouseLeftCount = 0;

            panel->connect("MouseEntered", genericCallback, std::ref(mouseEnteredCount));
            panel->connect("MouseLeft", genericCallback, std::ref(mouseLeftCount));

            auto parent = std::make_shared<tgui::Panel>(300, 200);
            parent->add(panel);

            parent->mouseMoved(10, 15);
            REQUIRE(mouseEnteredCount == 0);
            REQUIRE(mouseLeftCount == 0);

            parent->mouseMoved(40, 30);
            REQUIRE(mouseEnteredCount == 1);
            REQUIRE(mouseLeftCount == 0);

            parent->mouseMoved(189, 129);
            REQUIRE(mouseEnteredCount == 1);
            REQUIRE(mouseLeftCount == 0);

            parent->mouseMoved(190, 130);
            REQUIRE(mouseEnteredCount == 1);
            REQUIRE(mouseLeftCount == 1);
        }

        SECTION("mouse click")
        {
            panel->leftMouseReleased(115, 80);

            REQUIRE(mouseReleasedCount == 1);
            REQUIRE(clickedCount == 0);

            SECTION("mouse press")
            {
                panel->leftMousePressed(115, 80);

                REQUIRE(mousePressedCount == 1);
                REQUIRE(mouseReleasedCount == 1);
                REQUIRE(clickedCount == 0);
            }

            panel->leftMouseReleased(115, 80);

            REQUIRE(mousePressedCount == 1);
            REQUIRE(mouseReleasedCount == 2);
            REQUIRE(clickedCount == 1);
        }
    }

    testWidgetRenderer(panel->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = panel->getRenderer();

        SECTION("set serialized property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(10, 20, 30)"));
            REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(40, 50, 60)"));
            REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
        }

        SECTION("set object property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{10, 20, 30}));
            REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{40, 50, 60}));
            REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
        }

        SECTION("functions")
        {
            renderer->setBackgroundColor({10, 20, 30});
            renderer->setBorderColor({40, 50, 60});
            renderer->setBorders({1, 2, 3, 4});
        }

        REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(10, 20, 30));
        REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(40, 50, 60));
        REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));

        REQUIRE(renderer->getBackgroundColor() == sf::Color(10, 20, 30));
        REQUIRE(renderer->getBorderColor() == sf::Color(40, 50, 60));
        REQUIRE(renderer->getBorders() == tgui::Borders(1, 2, 3, 4));
    }

    SECTION("Saving and loading from file")
    {
        panel = std::make_shared<tgui::Panel>(400.f, 300.f);

        SECTION("independent panel")
        {
            REQUIRE_NOTHROW(panel->saveWidgetsToFile("WidgetFilePanel1.txt"));
            
            panel->setSize(200, 100);
            REQUIRE_NOTHROW(panel->loadWidgetsFromFile("WidgetFilePanel1.txt"));
            REQUIRE(panel->getSize() == sf::Vector2f(200, 100)); // The Panel itself is not saved, only its children

            REQUIRE_NOTHROW(panel->saveWidgetsToFile("WidgetFilePanel2.txt"));
            REQUIRE(compareFiles("WidgetFilePanel1.txt", "WidgetFilePanel2.txt"));
        }

        SECTION("panel inside gui")
        {
            auto parent = std::make_shared<tgui::GuiContainer>();
            parent->add(panel);

            auto widget = std::make_shared<tgui::ClickableWidget>();
            widget->setPosition(40, 20);
            panel->add(widget);

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFilePanel3.txt"));

            parent->removeAllWidgets();
            REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFilePanel3.txt"));

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFilePanel4.txt"));
            REQUIRE(compareFiles("WidgetFilePanel3.txt", "WidgetFilePanel4.txt"));

            SECTION("Copying widget")
            {
                copy(parent, panel);

                REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFilePanel4.txt"));
                REQUIRE(compareFiles("WidgetFilePanel3.txt", "WidgetFilePanel4.txt"));
            }
        }
    }
}
