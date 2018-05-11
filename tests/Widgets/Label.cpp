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
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Gui.hpp>

TEST_CASE("[Label]")
{
    tgui::Label::Ptr label = tgui::Label::create();
    label->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(label->connect("DoubleClicked", [](){}));
        REQUIRE_NOTHROW(label->connect("DoubleClicked", [](sf::String){}));
        REQUIRE_NOTHROW(label->connect("DoubleClicked", [](std::string){}));
        REQUIRE_NOTHROW(label->connect("DoubleClicked", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(label->connect("DoubleClicked", [](tgui::Widget::Ptr, std::string, sf::String){}));
        REQUIRE_NOTHROW(label->connect("DoubleClicked", [](tgui::Widget::Ptr, std::string, std::string){}));
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

    SECTION("IgnoreMouseEvents")
    {
        REQUIRE(!label->isIgnoringMouseEvents());
        label->ignoreMouseEvents(true);
        REQUIRE(label->isIgnoringMouseEvents());
        label->ignoreMouseEvents(false);
        REQUIRE(!label->isIgnoringMouseEvents());
    }

    SECTION("Events / Signals")
    {
        SECTION("ClickableWidget")
        {
            testClickableWidgetSignals(label);
        }

        SECTION("Double click")
        {
            unsigned int doubleClickedCount = 0;
            label->connect("DoubleClicked", &genericCallback, std::ref(doubleClickedCount));

            label->setPosition(40, 30);
            label->setSize(150, 100);

            label->leftMousePressed({115, 80});
            label->leftMouseReleased({115, 80});

            tgui::Gui gui;
            gui.add(label);
            gui.updateTime(DOUBLE_CLICK_TIMEOUT);

            label->leftMousePressed({115, 80});
            label->leftMouseReleased({115, 80});
            REQUIRE(doubleClickedCount == 0);

            gui.updateTime(DOUBLE_CLICK_TIMEOUT / 2.f);

            label->leftMousePressed({115, 80});
            label->leftMouseReleased({115, 80});
            REQUIRE(doubleClickedCount == 1);
        }
    }

    testWidgetRenderer(label->getRenderer());
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
            REQUIRE_NOTHROW(renderer->setProperty("TextStyle", "Bold | Italic"));
        }

        SECTION("set object property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{100, 50, 150}));
            REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{150, 100, 50}));
            REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{50, 150, 100}));
            REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
            REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Borders{5, 6, 7, 8}));
            REQUIRE_NOTHROW(renderer->setProperty("TextStyle", tgui::TextStyle{sf::Text::Bold | sf::Text::Italic}));
        }

        SECTION("functions")
        {
            renderer->setTextColor({100, 50, 150});
            renderer->setBackgroundColor({150, 100, 50});
            renderer->setBorderColor({50, 150, 100});
            renderer->setBorders({1, 2, 3, 4});
            renderer->setPadding({5, 6, 7, 8});
            renderer->setTextStyle(sf::Text::Bold | sf::Text::Italic);
        }

        REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(100, 50, 150));
        REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(150, 100, 50));
        REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(50, 150, 100));
        REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
        REQUIRE(renderer->getProperty("Padding").getOutline() == tgui::Padding(5, 6, 7, 8));
        REQUIRE(renderer->getProperty("TextStyle").getTextStyle() == (sf::Text::Bold | sf::Text::Italic));

        REQUIRE(renderer->getTextColor() == sf::Color(100, 50, 150));
        REQUIRE(renderer->getBackgroundColor() == sf::Color(150, 100, 50));
        REQUIRE(renderer->getBorderColor() == sf::Color(50, 150, 100));
        REQUIRE(renderer->getBorders() == tgui::Borders(1, 2, 3, 4));
        REQUIRE(renderer->getPadding() == tgui::Padding(5, 6, 7, 8));
        REQUIRE(renderer->getTextStyle() == (sf::Text::Bold | sf::Text::Italic));
    }

    SECTION("Saving and loading from file")
    {
        label->setText("SomeText");
        label->setTextSize(25);
        label->setHorizontalAlignment(tgui::Label::HorizontalAlignment::Center);
        label->setVerticalAlignment(tgui::Label::VerticalAlignment::Bottom);
        label->setMaximumTextWidth(300);
        label->ignoreMouseEvents(true);

        testSavingWidget("Label", label);
    }

    SECTION("Draw")
    {
        label->setPosition(10, 5);
        label->getRenderer()->setTextColor(sf::Color::Red);

        SECTION("Simple")
        {
            TEST_DRAW_INIT(110, 40, label)
            label->setText(L"Test gÊ");
            label->setTextSize(24);
            label->getRenderer()->setTextStyle(sf::Text::Style::Italic);
            TEST_DRAW("Label_Simple.png")
        }

        SECTION("Complex")
        {
            TEST_DRAW_INIT(420, 215, label)
            label->setText("Bacon ipsum dolor amet alcatra jerky turkey ball tip jowl beef. Shank landjaeger frankfurter, doner burgdoggen strip steak chicken pancetta jowl. Pork loin leberkas meatloaf ham shoulder cow hamburger pancetta. Rump turducken ribeye salami pork chop sirloin. Leberkas alcatra filet mignon jerky pork belly.");
            label->setTextSize(18);
            label->setSize(400, 205);
            label->getRenderer()->setBackgroundColor(sf::Color::Blue);
            label->getRenderer()->setBorderColor(sf::Color::Yellow);
            label->getRenderer()->setBorders({1, 2, 3, 4});
            label->getRenderer()->setPadding({4, 3, 2, 1});
            label->getRenderer()->setOpacity(0.7f);
            TEST_DRAW("Label_Complex.png")
        }
    }
}
