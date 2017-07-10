/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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

// Ignore warning "C4800: 'const int': forcing value to bool 'true' or 'false'" in Visual Studio
#if defined _MSC_VER
    #pragma warning(disable : 4800)
#endif

#include "Tests.hpp"
#include <TGUI/Widgets/Button.hpp>

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
        REQUIRE(widget->connect("PositionChanged", [](tgui::Widget::Ptr, std::string){}) == ++id);
        REQUIRE(widget->onPositionChange->connect([](){}) == ++id);
        REQUIRE(widget->onPositionChange->connect([](sf::Vector2f){}) == ++id);
        REQUIRE(widget->onPositionChange->connect([](tgui::Widget::Ptr, std::string){}) == ++id);
        REQUIRE(widget->onPositionChange->connect([](tgui::Widget::Ptr, std::string, sf::Vector2f){}) == ++id);

        REQUIRE(widget->connect("SizeChanged", [](){}) == ++id);
        REQUIRE(widget->connect("SizeChanged", [](tgui::Widget::Ptr, std::string){}) == ++id);
        REQUIRE(widget->onSizeChange->connect([](){}) == ++id);
        REQUIRE(widget->onSizeChange->connect([](sf::Vector2f){}) == ++id);
        REQUIRE(widget->onSizeChange->connect([](tgui::Widget::Ptr, std::string){}) == ++id);
        REQUIRE(widget->onSizeChange->connect([](tgui::Widget::Ptr, std::string, sf::Vector2f){}) == ++id);

        REQUIRE(widget->connect("Focused", [](){}) == ++id);
        REQUIRE(widget->connect("Focused", [](tgui::Widget::Ptr, std::string){}) == ++id);
        REQUIRE(widget->onFocus->connect([](){}) == ++id);
        REQUIRE(widget->onFocus->connect([](tgui::Widget::Ptr, std::string){}) == ++id);

        REQUIRE(widget->connect("Unfocused", [](){}) == ++id);
        REQUIRE(widget->connect("Unfocused", [](tgui::Widget::Ptr, std::string){}) == ++id);
        REQUIRE(widget->onUnfocus->connect([](){}) == ++id);
        REQUIRE(widget->onUnfocus->connect([](tgui::Widget::Ptr, std::string){}) == ++id);

        REQUIRE(widget->connect("MouseEntered", [](){}) == ++id);
        REQUIRE(widget->connect("MouseEntered", [](tgui::Widget::Ptr, std::string){}) == ++id);
        REQUIRE(widget->onMouseEnter->connect([](){}) == ++id);
        REQUIRE(widget->onMouseEnter->connect([](tgui::Widget::Ptr, std::string){}) == ++id);

        REQUIRE(widget->connect("MouseLeft", [](){}) == ++id);
        REQUIRE(widget->connect("MouseLeft", [](tgui::Widget::Ptr, std::string){}) == ++id);
        REQUIRE(widget->onMouseLeave->connect([](){}) == ++id);
        REQUIRE(widget->onMouseLeave->connect([](tgui::Widget::Ptr, std::string){}) == ++id);

        REQUIRE_THROWS_AS(widget->connect("SomeInvalidSignal", [](){}), tgui::Exception);
    }

    SECTION("disconnect")
    {
        unsigned int i = 0;
        unsigned int id = widget->onPositionChange->connect([&](){ i++; });

        widget->setPosition(10, 10);
        REQUIRE(i == 1);

        widget->onPositionChange->disconnect(id);
        widget->setPosition(20, 20);
        REQUIRE(i == 1);

        id = widget->connect("PositionChanged", [&](){ i++; });
        widget->setPosition(10, 10);
        REQUIRE(i == 2);

        widget->disconnect("PositionChanged", id);
        widget->setPosition(20, 20);
        REQUIRE(i == 2);

        widget->onPositionChange->connect([&](){ i++; });
        widget->onSizeChange->connect([&](){ i++; });
        widget->onPositionChange->disconnectAll();
        widget->setPosition(30, 30);
        widget->setSize(100, 25);
        REQUIRE(i == 3);

        widget->onPositionChange->connect([&](){ i++; });
        widget->onSizeChange->connect([&](){ i++; });
        widget->onPositionChange->disconnectAll();
        widget->onSizeChange->disconnectAll();
        widget->setPosition(40, 40);
        widget->setSize(200, 50);
        REQUIRE(i == 3);
    }
}
