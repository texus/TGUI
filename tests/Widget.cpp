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
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/ToolTip.hpp>

TEST_CASE("[Widget]") {
    tgui::Widget::Ptr widget = std::make_shared<tgui::Button>();

    SECTION("Signals") {
        tgui::Widget::Ptr widget2 = std::make_shared<tgui::Button>();

        REQUIRE_NOTHROW(widget->connect("PositionChanged", [](){}));
        REQUIRE_NOTHROW(widget->connect("SizeChanged", [](){}));
        REQUIRE_NOTHROW(widget->connect("Focused", [](){}));
        REQUIRE_NOTHROW(widget->connect("Unfocused", [](){}));
        REQUIRE_NOTHROW(widget->connect("MouseEntered", [](){}));
        REQUIRE_NOTHROW(widget->connect("MouseLeft", [](){}));
        
        REQUIRE_NOTHROW(widget->connect("PositionChanged", [](sf::Vector2f){}));
        REQUIRE_NOTHROW(widget->connect("SizeChanged", [](sf::Vector2f){}));

        REQUIRE_NOTHROW(widget->connect("PositionChanged", [](sf::Vector2f, sf::Vector2f){}, widget2->getPosition()));
        REQUIRE_NOTHROW(widget->connect("SizeChanged", [](sf::Vector2f, sf::Vector2f){}, std::bind(&tgui::Widget::getSize, widget2)));

        REQUIRE_THROWS_AS(widget->connect("SomeWrongSignal", [](){}), tgui::Exception);
        REQUIRE_THROWS_AS(widget->connect("PositionChanged", [](bool){}), tgui::Exception);
    }

    SECTION("Visibile") {
        REQUIRE(widget->isVisible());
        widget->hide();
        REQUIRE(!widget->isVisible());
        widget->show();
        REQUIRE(widget->isVisible());
    }
    
    SECTION("Enabled") {
        REQUIRE(widget->isEnabled());
        widget->disable();
        REQUIRE(!widget->isEnabled());
        widget->enable();
        REQUIRE(widget->isEnabled());
    }

    SECTION("Parent") {
        tgui::Panel::Ptr panel1 = std::make_shared<tgui::Panel>();
        tgui::Panel::Ptr panel2 = std::make_shared<tgui::Panel>();
        
        REQUIRE(widget->getParent() == nullptr);
        panel1->add(widget);
        REQUIRE(widget->getParent() == panel1.get());
        widget->initialize(panel2.get());
        REQUIRE(widget->getParent() == panel2.get());
    }
    
    SECTION("Transparency") {
        REQUIRE(widget->getTransparency() == 255);
        widget->setTransparency(100);
        REQUIRE(widget->getTransparency() == 100);
    }

    SECTION("Tooltip") {
        // Tooltip gets created when trying to access it when not not yet loaded
        REQUIRE(widget->getToolTip() != nullptr);

        widget->getToolTip()->setText("text");
        REQUIRE(widget->getToolTip()->getText() == "text");

        auto tooltip1 = std::make_shared<tgui::ToolTip>();
        tooltip1->setText("some text");
        widget->setToolTip(tooltip1);
        REQUIRE(widget->getToolTip()->getText() == "some text");
        
        // If new tooltip has an empty text, the current one is copied to it
        auto tooltip2 = std::make_shared<tgui::ToolTip>();
        widget->setToolTip(tooltip2);
        REQUIRE(widget->getToolTip()->getText() == "some text");
    }
}
