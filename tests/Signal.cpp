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

#include "Tests.hpp"
#include <TGUI/Widgets/Button.hpp>

// Ignore warning "C4800: 'const int': forcing value to bool 'true' or 'false'" in Visual Studio
#if defined _MSC_VER
    #pragma warning(disable : 4800)
#endif

TEST_CASE("[Signal]")
{
    tgui::Widget::Ptr widget = tgui::ClickableWidget::create();

/*
TODO: More tests:
    void f1(int){}

    struct A {
        void f2(int) {}
        void f3(int) const {}
    };

    struct B {
        void operator()(int) {}
    };

    struct C {
        void operator()(int) const {}
    };

    template <typename T>
    struct D {
        void operator()(const T&) const {}
    };

    int retInt() { return 0; }

    binder([](){}, 0);
    binder([](int){}, 0, 50);
    binder<int>([](int){}, 0);
    binder<int>([](int, int){}, 0, 50);
    binder<int, int>([](int, int){}, 0);
    binder<int, int>([](int, int, int){}, 0, 50);
    binder<int, int, int>([](int, int, int){}, 0);
    binder<int, int, int>([](int, int, int, int){}, 0, 50);

    connect(f1, 0);
    connect(&f1, 0);
    connect(&A::f2, &a, 0);
    connect(&A::f3, &a, 0);
    connect(b, 0);
    connect(c, 0);
    connect(std::function<void(int)>(f1), 0);
    connect(f1, std::bind(retInt));
    connect([&](int){ s+=1; }, 0);
    connect([&](auto){ s+=1; }, 0);

    connect(f1);
    connect(&f1);
    connect(&A::f2, &a);
    connect(&A::f3, &a);
    connect(b);
    connect(c);
    connect(std::function<void(int)>(f1));
    connect([&](int){ s+=1; });
    connect([&](auto, sf::String){ s+=1; }, 0);

    // std::ref as parameter
    // also try binding a generic lambda with variadic parameters
    // try testing with multiple operator() overloads
*/

    SECTION("connect")
    {
        unsigned int id = widget->connect("PositionChanged", [](){});
        REQUIRE(widget->connect("SizeChanged", [](){}) == ++id);
        REQUIRE(widget->connect("Focused", [](){}) == ++id);
        REQUIRE(widget->connect("Unfocused", [](){}) == ++id);
        REQUIRE(widget->connect("MouseEntered", [](){}) == ++id);
        REQUIRE(widget->connect("MouseLeft", [](){}) == ++id);

        REQUIRE(widget->connect("PositionChanged", [](sf::Vector2f){}) == ++id);
        REQUIRE(widget->connect("SizeChanged", [](sf::Vector2f){}) == ++id);

        tgui::Widget::Ptr widget2 = tgui::Button::create();
        REQUIRE(widget2->connect("PositionChanged", [](sf::Vector2f, sf::Vector2f){}, widget2->getPosition()) == ++id);
        REQUIRE(widget2->connect("SizeChanged", [](sf::Vector2f, sf::Vector2f){}, std::bind(&tgui::Widget::getSize, widget2)) == ++id);
        REQUIRE(widget2->connect("Pressed", [](std::string){}, std::bind(&tgui::Button::getText, std::static_pointer_cast<tgui::Button>(widget2))) == ++id);
        REQUIRE(widget2->connect("Pressed", [](auto, sf::String){}, 5) == ++id);

        REQUIRE_THROWS_AS(widget->connect("", [](){}), tgui::Exception);
        REQUIRE_THROWS_AS(widget->connect("    ", [](){}), tgui::Exception);
        REQUIRE_THROWS_AS(widget->connect("SomeWrongSignal", [](){}), tgui::Exception);
        REQUIRE_THROWS_AS(widget->connect("PositionChanged", [](bool){}), tgui::Exception);

        REQUIRE(widget->connect("Focused Unfocused", [](){}) == id+2);
        REQUIRE(widget->connect("All", [](){}) > id+3);
    }

    SECTION("connectEx")
    {
        unsigned int id = widget->connectEx("PositionChanged", [](const tgui::Callback&){});
        REQUIRE(widget->connectEx("SizeChanged", [](const tgui::Callback&){}) == ++id);
        REQUIRE(widget->connectEx("Focused", [](const tgui::Callback&){}) == ++id);
        REQUIRE(widget->connectEx("Unfocused", [](const tgui::Callback&){}) == ++id);
        REQUIRE(widget->connectEx("MouseEntered", [](const tgui::Callback&){}) == ++id);
        REQUIRE(widget->connectEx("MouseLeft", [](const tgui::Callback&){}) == ++id);

        tgui::Widget::Ptr widget2 = tgui::ClickableWidget::create();
        REQUIRE(widget->connectEx("PositionChanged", [](sf::Vector2f, const tgui::Callback&){}, widget2->getPosition()) == ++id);
        REQUIRE(widget->connectEx("SizeChanged", [](sf::Vector2f, const tgui::Callback&){}, std::bind(&tgui::Widget::getSize, widget2)) == ++id);

        REQUIRE_THROWS_AS(widget->connectEx("", [](const tgui::Callback&){}), tgui::Exception);
        REQUIRE_THROWS_AS(widget->connectEx("    ", [](const tgui::Callback&){}), tgui::Exception);
        REQUIRE_THROWS_AS(widget->connectEx("SomeWrongSignal", [](const tgui::Callback&){}), tgui::Exception);

        REQUIRE(widget->connectEx("Focused Unfocused MouseEntered MouseLeft", [](const tgui::Callback&){}) == id+4);
        REQUIRE(widget->connectEx("All", [](const tgui::Callback&){}) > id+5);
    }

    SECTION("disconnect")
    {
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
