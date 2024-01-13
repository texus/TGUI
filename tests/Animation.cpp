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

static void compareVector2f(tgui::Vector2f left, tgui::Vector2f right)
{
    REQUIRE(left.x == Approx(right.x).margin(0.000001f));
    REQUIRE(left.y == Approx(right.y).margin(0.000001f));
}

TEST_CASE("[Animation]")
{
    tgui::Widget::Ptr widget = tgui::ClickableWidget::create();
    widget->setPosition(30, 15);
    widget->setSize(120, 30);
    widget->setInheritedOpacity(0.9f);
    widget->getRenderer()->setOpacity(0.5f);

    tgui::Panel::Ptr parent = tgui::Panel::create();
    parent->setSize(480, 360);
    parent->add(widget);

    SECTION("Show/Hide with effects")
    {
        SECTION("Widget becomes visible when effect starts")
        {
            widget->setVisible(false);
            REQUIRE(!widget->isVisible());
            widget->showWithEffect(tgui::ShowEffectType::Scale, std::chrono::milliseconds(250));
            REQUIRE(widget->isVisible());

            // Widget remains visible after animation has finished
            widget->updateTime(std::chrono::milliseconds(250));
            REQUIRE(widget->isVisible());
        }

        SECTION("Widget is still visible when effect starts")
        {
            REQUIRE(widget->isVisible());
            widget->hideWithEffect(tgui::ShowEffectType::Fade, std::chrono::milliseconds(250));
            REQUIRE(widget->isVisible());

            // Widget gets hidden after animation has finished
            widget->updateTime(std::chrono::milliseconds(250));
            REQUIRE(!widget->isVisible());
        }

        SECTION("Time can go past the animation end")
        {
            widget->showWithEffect(tgui::ShowEffectType::SlideFromLeft, std::chrono::milliseconds(250));
            widget->updateTime(std::chrono::milliseconds(500));
            REQUIRE(widget->getPosition() == tgui::Vector2f(30, 15));
            REQUIRE(widget->getSize() == tgui::Vector2f(120, 30));
        }

        SECTION("showWithEffect")
        {
            SECTION("Fade")
            {
                widget->showWithEffect(tgui::ShowEffectType::Fade, std::chrono::milliseconds(300));
                REQUIRE(widget->getInheritedOpacity() == 0);
                widget->updateTime(std::chrono::milliseconds(100));
                REQUIRE(widget->getInheritedOpacity() == Approx(0.3f));
                widget->updateTime(std::chrono::milliseconds(200));
                REQUIRE(widget->getInheritedOpacity() == 0.9f);
            }

            SECTION("Scale")
            {
                widget->showWithEffect(tgui::ShowEffectType::Scale, std::chrono::milliseconds(300));
                REQUIRE(widget->getPosition() == tgui::Vector2f(90, 30));
                REQUIRE(widget->getSize() == tgui::Vector2f(0, 0));
                widget->updateTime(std::chrono::milliseconds(100));
                compareVector2f(widget->getPosition(), {70, 25});
                compareVector2f(widget->getSize(), {40, 10});
                widget->updateTime(std::chrono::milliseconds(200));
                REQUIRE(widget->getPosition() == tgui::Vector2f(30, 15));
                REQUIRE(widget->getSize() == tgui::Vector2f(120, 30));
            }

            SECTION("SlideFromLeft")
            {
                widget->showWithEffect(tgui::ShowEffectType::SlideFromLeft, std::chrono::milliseconds(300));
                REQUIRE(widget->getPosition() == tgui::Vector2f(-120, 15));
                widget->updateTime(std::chrono::milliseconds(100));
                compareVector2f(widget->getPosition(), {-120.f+((120.f+30.f)/3.f), 15});
                widget->updateTime(std::chrono::milliseconds(200));
                REQUIRE(widget->getPosition() == tgui::Vector2f(30, 15));
            }

            SECTION("SlideFromTop")
            {
                widget->showWithEffect(tgui::ShowEffectType::SlideFromTop, std::chrono::milliseconds(300));
                REQUIRE(widget->getPosition() == tgui::Vector2f(30, -30));
                widget->updateTime(std::chrono::milliseconds(100));
                compareVector2f(widget->getPosition(), {30, -30.f+((30.f+15.f)/3.f)});
                widget->updateTime(std::chrono::milliseconds(200));
                REQUIRE(widget->getPosition() == tgui::Vector2f(30, 15));
            }

            SECTION("SlideFromRight")
            {
                widget->showWithEffect(tgui::ShowEffectType::SlideFromRight, std::chrono::milliseconds(300));
                REQUIRE(widget->getPosition() == tgui::Vector2f(480, 15));
                widget->updateTime(std::chrono::milliseconds(100));
                compareVector2f(widget->getPosition(), {480-((480-30)/3.f), 15});
                widget->updateTime(std::chrono::milliseconds(200));
                REQUIRE(widget->getPosition() == tgui::Vector2f(30, 15));
            }

            SECTION("SlideFromBottom")
            {
                widget->showWithEffect(tgui::ShowEffectType::SlideFromBottom, std::chrono::milliseconds(300));
                REQUIRE(widget->getPosition() == tgui::Vector2f(30, 360));
                widget->updateTime(std::chrono::milliseconds(100));
                compareVector2f(widget->getPosition(), {30, 360-((360-15)/3.f)});
                widget->updateTime(std::chrono::milliseconds(200));
                REQUIRE(widget->getPosition() == tgui::Vector2f(30, 15));
            }

            // The widget no longer changes after the animation is over
            widget->updateTime(std::chrono::milliseconds(100));
            REQUIRE(widget->getPosition() == tgui::Vector2f(30, 15));
            REQUIRE(widget->getSize() == tgui::Vector2f(120, 30));
            REQUIRE(widget->getInheritedOpacity() == 0.9f);
        }

        SECTION("hideWithEffect")
        {
            SECTION("Fade")
            {
                widget->hideWithEffect(tgui::ShowEffectType::Fade, std::chrono::milliseconds(300));
                REQUIRE(widget->getInheritedOpacity() == 0.9f);
                widget->updateTime(std::chrono::milliseconds(100));
                REQUIRE(widget->getInheritedOpacity() == Approx(0.6f));
            }

            SECTION("Scale")
            {
                widget->hideWithEffect(tgui::ShowEffectType::Scale, std::chrono::milliseconds(300));
                REQUIRE(widget->getPosition() == tgui::Vector2f(30, 15));
                REQUIRE(widget->getSize() == tgui::Vector2f(120, 30));
                widget->updateTime(std::chrono::milliseconds(100));
                compareVector2f(widget->getPosition(), {50, 20});
                compareVector2f(widget->getSize(), {80, 20});
            }

            SECTION("SlideToRight")
            {
                widget->hideWithEffect(tgui::ShowEffectType::SlideToRight, std::chrono::milliseconds(300));
                REQUIRE(widget->getPosition() == tgui::Vector2f(30, 15));
                widget->updateTime(std::chrono::milliseconds(100));
                compareVector2f(widget->getPosition(), {30+((480-30)/3.f), 15});
            }

            SECTION("SlideToBottom")
            {
                widget->hideWithEffect(tgui::ShowEffectType::SlideToBottom, std::chrono::milliseconds(300));
                REQUIRE(widget->getPosition() == tgui::Vector2f(30, 15));
                widget->updateTime(std::chrono::milliseconds(100));
                compareVector2f(widget->getPosition(), {30, 15+((360-15)/3.f)});
            }

            SECTION("SlideToLeft")
            {
                widget->hideWithEffect(tgui::ShowEffectType::SlideToLeft, std::chrono::milliseconds(300));
                REQUIRE(widget->getPosition() == tgui::Vector2f(30, 15));
                widget->updateTime(std::chrono::milliseconds(100));
                compareVector2f(widget->getPosition(), {30.f-((120.f+30.f)/3.f), 15});
            }

            SECTION("SlideToTop")
            {
                widget->hideWithEffect(tgui::ShowEffectType::SlideToTop, std::chrono::milliseconds(300));
                REQUIRE(widget->getPosition() == tgui::Vector2f(30, 15));
                widget->updateTime(std::chrono::milliseconds(100));
                compareVector2f(widget->getPosition(), {30, 15.f-((30.f+15.f)/3.f)});
            }

            // The widget is hidden but reset to its original values at the end of the animation
            REQUIRE(widget->isVisible());
            widget->updateTime(std::chrono::milliseconds(200));
            REQUIRE(widget->getPosition() == tgui::Vector2f(30, 15));
            REQUIRE(widget->getSize() == tgui::Vector2f(120, 30));
            REQUIRE(widget->getInheritedOpacity() == 0.9f);
            REQUIRE(!widget->isVisible());

            // The widget no longer changes after the animation is over
            widget->updateTime(std::chrono::milliseconds(100));
            REQUIRE(widget->getPosition() == tgui::Vector2f(30, 15));
            REQUIRE(widget->getSize() == tgui::Vector2f(120, 30));
            REQUIRE(widget->getInheritedOpacity() == 0.9f);
            REQUIRE(!widget->isVisible());
        }

        SECTION("moveWithAnimation")
        {
            widget->moveWithAnimation({230, 25}, 300);
            REQUIRE(widget->getPosition() == tgui::Vector2f{30, 15}); // Hasn't moved yet
            widget->updateTime(std::chrono::milliseconds(150));
            REQUIRE(widget->getPosition() == tgui::Vector2f{130, 20}); // Halfway
            widget->updateTime(std::chrono::milliseconds(150));
            REQUIRE(widget->getPosition() == tgui::Vector2f{230, 25}); // Finished
        }

        SECTION("resizeWithAnimation")
        {
            widget->resizeWithAnimation({60, 60}, 300);
            REQUIRE(widget->getSize() == tgui::Vector2f{120, 30}); // Hasn't resized yet
            widget->updateTime(std::chrono::milliseconds(150));
            REQUIRE(widget->getSize() == tgui::Vector2f{90, 45}); // Halfway
            widget->updateTime(std::chrono::milliseconds(150));
            REQUIRE(widget->getSize() == tgui::Vector2f{60, 60}); // Finished
        }

        SECTION("isAnimationPlaying")
        {
            REQUIRE(!widget->isAnimationPlaying());

            widget->hideWithEffect(tgui::ShowEffectType::Fade, tgui::Duration(300));
            REQUIRE(widget->isAnimationPlaying());

            widget->updateTime(tgui::Duration(200));
            REQUIRE(widget->isAnimationPlaying());

            widget->updateTime(tgui::Duration(200));
            REQUIRE(!widget->isAnimationPlaying());

            widget->showWithEffect(tgui::ShowEffectType::SlideFromLeft, tgui::Duration(300));
            REQUIRE(widget->isAnimationPlaying());

            widget->updateTime(tgui::Duration(200));
            REQUIRE(widget->isAnimationPlaying());

            widget->updateTime(tgui::Duration(200));
            REQUIRE(!widget->isAnimationPlaying());
        }

        // TODO: Add tests for simultaneous animations (tests for both same type and different types)
    }
}
