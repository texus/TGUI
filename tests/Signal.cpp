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

TEST_CASE("[Signal]") {
    tgui::Widget::Ptr widget = std::make_shared<tgui::Button>();

    SECTION("connect") {
        unsigned int id = widget->connect("PositionChanged", [](){});
        REQUIRE(widget->connect("SizeChanged", [](){}) == id+1);
        REQUIRE(widget->connect("Focused", [](){}) == id+2);
        REQUIRE(widget->connect("Unfocused", [](){}) == id+3);
        REQUIRE(widget->connect("MouseEntered", [](){}) == id+4);
        REQUIRE(widget->connect("MouseLeft", [](){}) == id+5);

        REQUIRE(widget->connect("PositionChanged", [](sf::Vector2f){}) == id+6);
        REQUIRE(widget->connect("SizeChanged", [](sf::Vector2f){}) == id+7);

        tgui::Widget::Ptr widget2 = std::make_shared<tgui::Button>();
        REQUIRE(widget->connect("PositionChanged", [](sf::Vector2f, sf::Vector2f){}, widget2->getPosition()) == id+8);
        REQUIRE(widget->connect("SizeChanged", [](sf::Vector2f, sf::Vector2f){}, std::bind(&tgui::Widget::getSize, widget2)) == id+9);

        REQUIRE_THROWS_AS(widget->connect("", [](){}), tgui::Exception);
        REQUIRE_THROWS_AS(widget->connect("    ", [](){}), tgui::Exception);
        REQUIRE_THROWS_AS(widget->connect("SomeWrongSignal", [](){}), tgui::Exception);
        REQUIRE_THROWS_AS(widget->connect("PositionChanged", [](bool){}), tgui::Exception);

        REQUIRE(widget->connect("Focused Unfocused MouseEntered MouseLeft", [](){}) == id+13);
        REQUIRE(widget->connect("All", [](){}) > id+14);
    }

    SECTION("connectEx") {
        unsigned int id = widget->connectEx("PositionChanged", [](const tgui::Callback&){});
        REQUIRE(widget->connectEx("SizeChanged", [](const tgui::Callback&){}) == id+1);
        REQUIRE(widget->connectEx("Focused", [](const tgui::Callback&){}) == id+2);
        REQUIRE(widget->connectEx("Unfocused", [](const tgui::Callback&){}) == id+3);
        REQUIRE(widget->connectEx("MouseEntered", [](const tgui::Callback&){}) == id+4);
        REQUIRE(widget->connectEx("MouseLeft", [](const tgui::Callback&){}) == id+5);

        tgui::Widget::Ptr widget2 = std::make_shared<tgui::Button>();
        REQUIRE(widget->connectEx("PositionChanged", [](sf::Vector2f, const tgui::Callback&){}, widget2->getPosition()) == id+6);
        REQUIRE(widget->connectEx("SizeChanged", [](sf::Vector2f, const tgui::Callback&){}, std::bind(&tgui::Widget::getSize, widget2)) == id+7);

        REQUIRE_THROWS_AS(widget->connectEx("SomeWrongSignal", [](const tgui::Callback&){}), tgui::Exception);

        REQUIRE(widget->connectEx("Focused Unfocused MouseEntered MouseLeft", [](const tgui::Callback&){}) == id+11);
        REQUIRE(widget->connectEx("All", [](const tgui::Callback&){}) > id+12);
    }

    SECTION("disconnect") {
        unsigned int i = 0;
        unsigned int id = widget->connect("PositionChanged", [&](){ i++; });

        widget->setPosition(10, 10);
        REQUIRE(i == 1);

        widget->disconnect(id);
        widget->setPosition(20, 20);
        REQUIRE(i == 1);

        widget->connect("PositionChanged", [&](){ i++; });
        widget->connect("SizeChanged", [&](){ i++; });
        widget->disconnectAll("PositionChanged");
        widget->setPosition(30, 30);
        widget->setSize(100, 25);
        REQUIRE(i == 2);

        widget->connect("PositionChanged", [&](){ i++; });
        widget->connect("SizeChanged", [&](){ i++; });
        widget->disconnectAll();
        widget->setPosition(40, 40);
        widget->setSize(200, 50);
        REQUIRE(i == 2);
    }
}
