/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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

TEST_CASE("[ClickableWidget]")
{
    tgui::ClickableWidget::Ptr widget = tgui::ClickableWidget::create();

    SECTION("Signals")
    {
        widget->onMousePress([](){});
        widget->onMousePress([](tgui::Vector2f){});

        widget->onMouseRelease([](){});
        widget->onMouseRelease([](tgui::Vector2f){});

        widget->onClick([](){});
        widget->onClick([](tgui::Vector2f){});

        widget->onRightMousePress([](){});
        widget->onRightMousePress([](tgui::Vector2f){});

        widget->onRightMouseRelease([](){});
        widget->onRightMouseRelease([](tgui::Vector2f){});

        widget->onRightClick([](){});
        widget->onRightClick([](tgui::Vector2f){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(widget)->getSignal("MousePressed").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(widget)->getSignal("MouseReleased").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(widget)->getSignal("Clicked").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(widget)->getSignal("RightMousePressed").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(widget)->getSignal("RightMouseReleased").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(widget)->getSignal("RightClicked").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(widget->getWidgetType() == "ClickableWidget");
    }

    SECTION("Constructor")
    {
        widget = tgui::ClickableWidget::create({200, 100});
        REQUIRE(widget->getSize() == tgui::Vector2f(200, 100));
    }

    SECTION("Position and Size")
    {
        widget->setPosition(40, 30);
        widget->setSize(150, 100);

        REQUIRE(widget->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(widget->getSize() == tgui::Vector2f(150, 100));
        REQUIRE(widget->getFullSize() == widget->getSize());
        REQUIRE(widget->getWidgetOffset() == tgui::Vector2f(0, 0));
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
