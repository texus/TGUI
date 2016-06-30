/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
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
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Gui.hpp>

TEST_CASE("[Label]") {
    tgui::Label::Ptr label = std::make_shared<tgui::Label>();
    label->setFont("resources/DroidSansArmenian.ttf");

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(label->connect("DoubleClicked", [](){}));
        REQUIRE_NOTHROW(label->connect("DoubleClicked", [](sf::String){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(label->getWidgetType() == "Label");
    }

    SECTION("Position and Size")
    {
        label->setPosition(40, 30);
        label->setSize(150, 100);
        label->getRenderer()->setBorders(2);

        REQUIRE(label->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(label->getSize() == sf::Vector2f(150, 100));
        REQUIRE(label->getFullSize() == label->getSize());
        REQUIRE(label->getWidgetOffset() == sf::Vector2f(0, 0));
    }

    SECTION("Text")
    {
        REQUIRE(label->getText() == "");
        label->setText("SomeText");
        REQUIRE(label->getText() == "SomeText");
    }

    SECTION("TextStyle")
    {
        REQUIRE(label->getTextStyle() == sf::Text::Regular);
        label->setTextStyle(sf::Text::Bold | sf::Text::Italic);
        REQUIRE(label->getTextStyle() == (sf::Text::Bold | sf::Text::Italic));
    }

    SECTION("TextSize")
    {
        label->setTextSize(25);
        REQUIRE(label->getTextSize() == 25);
    }

    SECTION("Alignment")
    {
        REQUIRE(label->getHorizontalAlignment() == tgui::Label::HorizontalAlignment::Left);
        REQUIRE(label->getVerticalAlignment() == tgui::Label::VerticalAlignment::Top);

        label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
        REQUIRE(label->getHorizontalAlignment() == tgui::Label::HorizontalAlignment::Center);
        REQUIRE(label->getVerticalAlignment() == tgui::Label::VerticalAlignment::Top);

        label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Right);
        REQUIRE(label->getHorizontalAlignment() == tgui::Label::HorizontalAlignment::Right);

        label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Left);
        REQUIRE(label->getHorizontalAlignment() == tgui::Label::HorizontalAlignment::Left);

        label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
        REQUIRE(label->getHorizontalAlignment() == tgui::Label::HorizontalAlignment::Left);
        REQUIRE(label->getVerticalAlignment() == tgui::Label::VerticalAlignment::Center);

        label->setVerticalAlignment(tgui::Label::VerticalAlignment::Bottom);
        REQUIRE(label->getVerticalAlignment() == tgui::Label::VerticalAlignment::Bottom);

        label->setVerticalAlignment(tgui::Label::VerticalAlignment::Top);
        REQUIRE(label->getVerticalAlignment() == tgui::Label::VerticalAlignment::Top);
    }

    SECTION("AutoSize")
    {
        REQUIRE(label->getAutoSize());
        label->setAutoSize(false);
        REQUIRE(!label->getAutoSize());
        label->setAutoSize(true);
        REQUIRE(label->getAutoSize());
        label->setSize(200, 100);
        REQUIRE(!label->getAutoSize());
    }

    SECTION("MaximumTextWidth")
    {
        REQUIRE(label->getMaximumTextWidth() == 0);
        label->setMaximumTextWidth(300);
        REQUIRE(label->getMaximumTextWidth() == 300);

        label->setSize(200, 50);
        REQUIRE(label->getMaximumTextWidth() == 200);

        label->setSize(400, 50);
        REQUIRE(label->getMaximumTextWidth() == 400);

        label->setMaximumTextWidth(500);
        REQUIRE(label->getMaximumTextWidth() == 400);

        label->setAutoSize(true);
        REQUIRE(label->getMaximumTextWidth() == 500);
    }

    SECTION("Events")
    {
        unsigned int mousePressedCount = 0;
        unsigned int mouseReleasedCount = 0;
        unsigned int clickedCount = 0;
        unsigned int doubleClickedCount = 0;

        label->setPosition(40, 30);
        label->setSize(150, 100);

        label->connect("MousePressed", mouseCallback, std::ref(mousePressedCount));
        label->connect("MouseReleased", mouseCallback, std::ref(mouseReleasedCount));
        label->connect("Clicked", mouseCallback, std::ref(clickedCount));
        label->connect("DoubleClicked", genericCallback, std::ref(doubleClickedCount));

        SECTION("mouseOnWidget")
        {
            REQUIRE(!label->mouseOnWidget(10, 15));
            REQUIRE(label->mouseOnWidget(40, 30));
            REQUIRE(label->mouseOnWidget(115, 80));
            REQUIRE(label->mouseOnWidget(189, 129));
            REQUIRE(!label->mouseOnWidget(190, 130));

            REQUIRE(mousePressedCount == 0);
            REQUIRE(mouseReleasedCount == 0);
            REQUIRE(clickedCount == 0);
            REQUIRE(doubleClickedCount == 0);
        }

        SECTION("mouse move")
        {
            unsigned int mouseEnteredCount = 0;
            unsigned int mouseLeftCount = 0;

            label->connect("MouseEntered", genericCallback, std::ref(mouseEnteredCount));
            label->connect("MouseLeft", genericCallback, std::ref(mouseLeftCount));

            auto parent = std::make_shared<tgui::Panel>(300, 200);
            parent->add(label);

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
            label->leftMouseReleased(115, 80);

            REQUIRE(mouseReleasedCount == 1);
            REQUIRE(clickedCount == 0);

            SECTION("mouse press")
            {
                label->leftMousePressed(115, 80);

                REQUIRE(mousePressedCount == 1);
                REQUIRE(mouseReleasedCount == 1);
                REQUIRE(clickedCount == 0);
            }

            label->leftMouseReleased(115, 80);

            REQUIRE(mousePressedCount == 1);
            REQUIRE(mouseReleasedCount == 2);
            REQUIRE(clickedCount == 1);
            REQUIRE(doubleClickedCount == 0);
        }

        SECTION("double click")
        {
            label->leftMousePressed(115, 80);
            label->leftMouseReleased(115, 80);

            tgui::Gui gui;
            gui.add(label);
            gui.updateTime(DOUBLE_CLICK_TIMEOUT);

            label->leftMousePressed(115, 80);
            label->leftMouseReleased(115, 80);

            REQUIRE(mousePressedCount == 2);
            REQUIRE(mouseReleasedCount == 2);
            REQUIRE(clickedCount == 2);
            REQUIRE(doubleClickedCount == 0);

            gui.updateTime(DOUBLE_CLICK_TIMEOUT / 2.f);

            label->leftMousePressed(115, 80);
            label->leftMouseReleased(115, 80);

            REQUIRE(doubleClickedCount == 1);
        }
    }

    SECTION("Renderer")
    {
        auto renderer = label->getRenderer();

        SECTION("set serialized property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(100, 50, 150)"));
            REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(150, 100, 50)"));
            REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(50, 150, 100)"));
            REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
            REQUIRE_NOTHROW(renderer->setProperty("Padding", "(5, 6, 7, 8)"));
            REQUIRE_NOTHROW(renderer->setProperty("Opacity", "0.8"));
        }

        SECTION("set object property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{100, 50, 150}));
            REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{150, 100, 50}));
            REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{50, 150, 100}));
            REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
            REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Borders{5, 6, 7, 8}));
            REQUIRE_NOTHROW(renderer->setProperty("Opacity", 0.8f));
        }

        SECTION("functions")
        {
            renderer->setTextColor({100, 50, 150});
            renderer->setBackgroundColor({150, 100, 50});
            renderer->setBorderColor({50, 150, 100});
            renderer->setBorders({1, 2, 3, 4});
            renderer->setPadding({5, 6, 7, 8});
            renderer->setOpacity(0.8f);
        }

        REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(100, 50, 150));
        REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(150, 100, 50));
        REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(50, 150, 100));
        REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
        REQUIRE(renderer->getProperty("Padding").getOutline() == tgui::Padding(5, 6, 7, 8));
        REQUIRE(renderer->getProperty("Opacity").getNumber() == 0.8f);

        REQUIRE(renderer->getTextColor() == sf::Color(100, 50, 150));
        REQUIRE(renderer->getBackgroundColor() == sf::Color(150, 100, 50));
        REQUIRE(renderer->getBorderColor() == sf::Color(50, 150, 100));
        REQUIRE(renderer->getBorders() == tgui::Borders(1, 2, 3, 4));
        REQUIRE(renderer->getPadding() == tgui::Padding(5, 6, 7, 8));
        REQUIRE(renderer->getOpacity() == 0.8f);

        REQUIRE_THROWS_AS(renderer->setProperty("NonexistentProperty", ""), tgui::Exception);
    }

    SECTION("Saving and loading from file")
    {
        tgui::Theme theme{"resources/Black.txt"};
        label->setRenderer(theme.getRenderer("label"));

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(label);

        label->setText("SomeText");
        label->setTextSize(25);
        label->setTextStyle(sf::Text::Bold | sf::Text::Italic);
        label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
        label->setVerticalAlignment(tgui::Label::VerticalAlignment::Bottom);
        label->setMaximumTextWidth(300);
        label->getRenderer()->setTextColor({100, 50, 150});
        label->getRenderer()->setBackgroundColor({150, 100, 50});
        label->getRenderer()->setBorderColor({50, 150, 100});
        label->getRenderer()->setBorders({1, 2, 3, 4});
        label->getRenderer()->setPadding({5, 6, 7, 8});
        label->getRenderer()->setOpacity(0.8f);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileLabel1.txt"));

        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileLabel1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileLabel2.txt"));
        REQUIRE(compareFiles("WidgetFileLabel1.txt", "WidgetFileLabel2.txt"));

        SECTION("Copying widget")
        {
            copy(parent, label);

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileLabel2.txt"));
            REQUIRE(compareFiles("WidgetFileLabel1.txt", "WidgetFileLabel2.txt"));
        }
    }
}
