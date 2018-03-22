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
#include <TGUI/Widgets/ClickableWidget.hpp>
#include <TGUI/Widgets/Panel.hpp>

TEST_CASE("[ClickableWidget]")
{
    tgui::ClickableWidget::Ptr widget = tgui::ClickableWidget::create();

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(widget->connect("MousePressed", [](){}));
        REQUIRE_NOTHROW(widget->connect("MousePressed", [](sf::Vector2f){}));
        REQUIRE_NOTHROW(widget->connect("MousePressed", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(widget->connect("MousePressed", [](tgui::Widget::Ptr, std::string, sf::Vector2f){}));

        REQUIRE_NOTHROW(widget->connect("MouseReleased", [](){}));
        REQUIRE_NOTHROW(widget->connect("MouseReleased", [](sf::Vector2f){}));
        REQUIRE_NOTHROW(widget->connect("MouseReleased", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(widget->connect("MouseReleased", [](tgui::Widget::Ptr, std::string, sf::Vector2f){}));

        REQUIRE_NOTHROW(widget->connect("Clicked", [](){}));
        REQUIRE_NOTHROW(widget->connect("Clicked", [](sf::Vector2f){}));
        REQUIRE_NOTHROW(widget->connect("Clicked", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(widget->connect("Clicked", [](tgui::Widget::Ptr, std::string, sf::Vector2f){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(widget->getWidgetType() == "ClickableWidget");
    }

    SECTION("Constructor")
    {
        widget = tgui::ClickableWidget::create({200, 100});
        REQUIRE(widget->getSize() == sf::Vector2f(200, 100));
    }

    SECTION("Position and Size")
    {
        widget->setPosition(40, 30);
        widget->setSize(150, 100);

        REQUIRE(widget->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(widget->getSize() == sf::Vector2f(150, 100));
        REQUIRE(widget->getFullSize() == widget->getSize());
        REQUIRE(widget->getWidgetOffset() == sf::Vector2f(0, 0));
    }

    SECTION("Events / Signals")
    {
        testClickableWidgetSignals(widget);
    }

    SECTION("Renderer")
    {
        auto renderer = widget->getRenderer();

        SECTION("set serialized property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("Opacity", "0.8"));
        }

        SECTION("set object property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("Opacity", 0.8f));
        }

        SECTION("functions")
        {
            renderer->setOpacity(0.8f);
        }

        REQUIRE(renderer->getProperty("Opacity").getNumber() == 0.8f);
        REQUIRE(renderer->getOpacity() == 0.8f);

        REQUIRE_THROWS_AS(renderer->setProperty("NonexistentProperty", ""), tgui::Exception);
    }

    SECTION("Saving and loading from file")
    {
        testSavingWidget("ClickableWidget", widget, false);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(60, 50, widget)
        widget->setPosition(15, 10);
        widget->setSize(25, 20);
        TEST_DRAW("ClickableWidget.png")
    }
}
