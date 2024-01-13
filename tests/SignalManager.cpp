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

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/SignalManager.hpp>
#endif

TEST_CASE("[SignalManager]")
{
    auto manager = tgui::SignalManager::getSignalManager();

    SECTION("connect")
    {
        unsigned int id = manager->connect("NameOfWidget", "PositionChanged", [](){});
        REQUIRE(manager->connect("NameOfWidget", "PositionChanged", [](const tgui::Widget::Ptr&, const tgui::String&){}) == ++id);
        REQUIRE(manager->connect("NameOfWidget", "PositionChanged", [](const tgui::Widget::Ptr&, const tgui::String&){}) == ++id);
        REQUIRE(manager->connect("NameOfWidget", "PositionChanged", [](){}) == ++id);

        REQUIRE(manager->disconnect(id));
        REQUIRE(!manager->disconnect(id));


        tgui::Widget::Ptr widget2 = tgui::Button::create();
        REQUIRE(manager->connect("NameOfWidget", "Pressed", [](){}) == ++id);
        REQUIRE(manager->connect("NameOfWidget", "Pressed", [](int){}, 5) == ++id);
        REQUIRE(manager->connect("NameOfWidget", "Pressed", [](auto){}, 10.f) == ++id);
        REQUIRE(manager->connect("NameOfWidget", "Pressed", [](const tgui::Widget::Ptr&, const tgui::String&){}) == ++id);
        REQUIRE(manager->connect("NameOfWidget", "Pressed", [](auto, auto, auto, const tgui::Widget::Ptr&, const tgui::String&){}, "Hey", 15, 3.f) == ++id);
        REQUIRE(manager->connect("NameOfWidget", "Pressed", std::bind([](int){}, 5)) == ++id); // NOLINT(modernize-avoid-bind)

        struct Class
        {
            void signalHandler1() {}
            void signalHandler2(const tgui::Widget::Ptr&, const tgui::String&) {}
            void signalHandler3(int, float, const tgui::Widget::Ptr&, const tgui::String&) {}
            void signalHandler4(int&, const tgui::Widget::Ptr&, const tgui::String&) {}

            void signalHandler7() const {}
            void signalHandler8(const tgui::Widget::Ptr&, const tgui::String&) const {}
            void signalHandler9(int, float, const tgui::Widget::Ptr&, const tgui::String&) const {}
            void signalHandler10(int&, const tgui::Widget::Ptr&, const tgui::String&) const {}
        };

        int i;
        Class instance;
        REQUIRE(manager->connect("NameOfWidget", "Pressed", &Class::signalHandler1, &instance) == ++id);
        REQUIRE(manager->connect("NameOfWidget", "Pressed", &Class::signalHandler2, &instance) == ++id);
        REQUIRE(manager->connect("NameOfWidget", "Pressed", &Class::signalHandler3, &instance, 0, 5.f) == ++id);
        REQUIRE(manager->connect("NameOfWidget", "Pressed", &Class::signalHandler4, &instance, std::ref(i)) == ++id);
        REQUIRE(manager->connect("NameOfWidget", "Pressed", &Class::signalHandler7, &instance) == ++id);
        REQUIRE(manager->connect("NameOfWidget", "Pressed", &Class::signalHandler8, &instance) == ++id);
        REQUIRE(manager->connect("NameOfWidget", "Pressed", &Class::signalHandler9, &instance, 0, 0.5f) == ++id);
        REQUIRE(manager->connect("NameOfWidget", "Pressed", &Class::signalHandler10, &instance, std::ref(i)) == ++id);
    }

    SECTION("work")
    {
        unsigned int mousePressedCount = 0;
        unsigned int mousePressedCount2 = 0;

        manager->connect("NameOfWidget", "MousePressed", [&](){ mousePressedCount++; });

        auto widget = tgui::Button::create();

        widget->setPosition({40, 30});
        widget->setSize({150, 100});

        auto parent = tgui::Panel::create({300, 200});
        parent->setPosition({60, 55});
        parent->add(widget, "NameOfWidget");

        manager->connect("NameOfWidget", "MousePressed", [&](const tgui::Widget::Ptr&, const tgui::String&){ mousePressedCount2++; });

        parent->leftMouseReleased({175, 135});
        REQUIRE(mousePressedCount == 0);
        REQUIRE(mousePressedCount2 == 0);

        parent->leftMousePressed({175, 135});
        REQUIRE(mousePressedCount == 1);
        REQUIRE(mousePressedCount2 == 1);

        parent->leftMouseReleased({175, 135});
        parent->leftMouseButtonNoLongerDown();
        REQUIRE(mousePressedCount == 1);
        REQUIRE(mousePressedCount2 == 1);
    }
}
