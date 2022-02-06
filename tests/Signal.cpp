/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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

TEST_CASE("[Signal]")
{
    tgui::Widget::Ptr widget = tgui::ClickableWidget::create();

    SECTION("connect")
    {
        unsigned int id = widget->onPositionChange.connect([](){});
        REQUIRE(widget->onPositionChange.connect([](tgui::Vector2f){}) == ++id);
        REQUIRE(widget->onPositionChange.connectEx([](tgui::Widget::Ptr, tgui::String){}) == ++id);

        REQUIRE(widget->onSizeChange([](){}) == ++id);
        REQUIRE(widget->onSizeChange([](tgui::Vector2f){}) == ++id);

        REQUIRE(widget->onFocus.connect([](){}) == ++id);
        REQUIRE(widget->onFocus.connectEx([](tgui::Widget::Ptr, tgui::String){}) == ++id);

        REQUIRE(widget->onUnfocus.connect([](){}) == ++id);
        REQUIRE(widget->onUnfocus.connectEx([](tgui::Widget::Ptr, tgui::String){}) == ++id);

        REQUIRE(widget->onMouseEnter.connect([](){}) == ++id);
        REQUIRE(widget->onMouseEnter.connectEx([](tgui::Widget::Ptr, tgui::String){}) == ++id);

        REQUIRE(widget->onMouseLeave.connect([](){}) == ++id);
        REQUIRE(widget->onMouseLeave.connectEx([](tgui::Widget::Ptr, tgui::String){}) == ++id);

        REQUIRE(widget->onAnimationFinish([](){}) == ++id);
        REQUIRE(widget->onAnimationFinish([](tgui::AnimationType){}) == ++id);

        REQUIRE(widget->onShowEffectFinish([](){}) == ++id);
        REQUIRE(widget->onShowEffectFinish([](tgui::ShowEffectType){}) == ++id);
        REQUIRE(widget->onShowEffectFinish([](bool){}) == ++id);
        REQUIRE(widget->onShowEffectFinish([](tgui::ShowEffectType, bool){}) == ++id);

        REQUIRE_THROWS_AS(widget->getSignal("SomeInvalidSignal"), tgui::Exception);

        tgui::Button::Ptr widget2 = tgui::Button::create();
        REQUIRE(widget2->onPress([](){}) == ++id);
        REQUIRE(widget2->onPress([](int){}, 5) == ++id);
        REQUIRE(widget2->onPress([](auto){}, 10.f) == ++id);
        REQUIRE(widget2->onPress.connectEx([](tgui::Widget::Ptr, tgui::String){}) == ++id);
        REQUIRE(widget2->onPress.connectEx([](auto, auto, auto, tgui::Widget::Ptr, tgui::String){}, "Hey", 15, 3.f) == ++id);

        struct Class
        {
            void signalHandler1() {}
            void signalHandler2(tgui::Widget::Ptr, const tgui::String&) {}
            void signalHandler3(int, float, tgui::Widget::Ptr, const tgui::String&) {}
            void signalHandler4(int&, tgui::Widget::Ptr, const tgui::String&) {}
            void signalHandler5(int&, const tgui::String&) {}
            void signalHandler6(int&, tgui::String) {}

            void signalHandler7() const {}
            void signalHandler8(tgui::Widget::Ptr, const tgui::String&) const {}
            void signalHandler9(int, float, tgui::Widget::Ptr, const tgui::String&) const {}
            void signalHandler10(int&, tgui::Widget::Ptr, const tgui::String&) const {}
            void signalHandler11(int&, const tgui::String&) const {}
            void signalHandler12(int&, tgui::String) const {}
        };

        int i;
        Class instance;
        REQUIRE(widget2->onPress.connect(&Class::signalHandler1, &instance) == ++id);
        REQUIRE(widget2->onPress.connectEx(&Class::signalHandler2, &instance) == ++id);
        REQUIRE(widget2->onPress.connectEx(&Class::signalHandler3, &instance, 0, 5.f) == ++id);
        REQUIRE(widget2->onPress.connectEx(&Class::signalHandler4, &instance, std::ref(i)) == ++id);
        REQUIRE(widget2->onPress.connect(&Class::signalHandler5, &instance, std::ref(i)) == ++id);
        REQUIRE(widget2->onPress.connect(&Class::signalHandler6, &instance, std::ref(i)) == ++id);
        REQUIRE(widget2->onPress.connect(&Class::signalHandler7, &instance) == ++id);
        REQUIRE(widget2->onPress.connectEx(&Class::signalHandler8, &instance) == ++id);
        REQUIRE(widget2->onPress.connectEx(&Class::signalHandler9, &instance, 0, 0.5f) == ++id);
        REQUIRE(widget2->onPress.connectEx(&Class::signalHandler10, &instance, std::ref(i)) == ++id);
        REQUIRE(widget2->onPress.connect(&Class::signalHandler11, &instance, std::ref(i)) == ++id);
        REQUIRE(widget2->onPress.connect(&Class::signalHandler12, &instance, std::ref(i)) == ++id);

        REQUIRE(widget2->onPress(std::function<void(tgui::String)>([](tgui::String){})) == ++id);
    }

    SECTION("disconnect")
    {
        unsigned int i = 0;
        unsigned int id = widget->onPositionChange([&](){ i++; });
        widget->setPosition(10, 10);
        REQUIRE(i == 1);

        REQUIRE(widget->onPositionChange.disconnect(id));
        REQUIRE_FALSE(widget->onPositionChange.disconnect(id));
        widget->setPosition(20, 20);
        REQUIRE(i == 1);

        widget->onPositionChange([&](){ i++; });
        widget->onSizeChange([&](){ i++; });
        widget->onPositionChange.disconnectAll();
        widget->setPosition(30, 30);
        widget->setSize(100, 25);
        REQUIRE(i == 2);

        widget->onPositionChange.disconnectAll();
        widget->onSizeChange.disconnectAll();
        widget->onPositionChange([&](){ i++; });
        widget->onSizeChange([&](){ i++; });
        widget->setPosition(50, 50);
        widget->setSize(300, 75);
        REQUIRE(i == 4);
    }

    SECTION("Reference")
    {
        tgui::Signal signal{"Test"};
        int x = 3;
        signal.connect([](int& y){ y = 5; }, std::ref(x));
        REQUIRE(x == 3);
        signal.emit(nullptr);
        REQUIRE(x == 5);
    }
}
