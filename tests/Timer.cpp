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

TEST_CASE("[Timer]")
{
    unsigned int count = 0;
    const auto callback = [&count]{ ++count; };

    SECTION("Creating timers")
    {
        auto timer1 = tgui::Timer::create(callback, std::chrono::milliseconds(400));
        auto timer2 = tgui::Timer::create(callback, 500);
        auto timer3 = tgui::Timer::create(callback, 500, false);

        tgui::Timer::updateTime(std::chrono::milliseconds(250));
        REQUIRE(count == 0); // No timers triggered yet
        tgui::Timer::updateTime(std::chrono::milliseconds(200));
        REQUIRE(count == 1); // timer1 was triggered
        tgui::Timer::updateTime(std::chrono::milliseconds(200));
        REQUIRE(count == 2); // timer2 was triggered
        tgui::Timer::updateTime(std::chrono::milliseconds(250));
        REQUIRE(count == 3); // timer1 was triggered again

        timer1->setEnabled(false);
        timer2->setEnabled(false);
    }

    SECTION("Editing timers")
    {
        SECTION("Enabled")
        {
            auto timer = tgui::Timer::create(callback, 500, false);
            REQUIRE(!timer->isEnabled());

            tgui::Timer::updateTime(std::chrono::milliseconds(750));
            REQUIRE(count == 0); // Timer was disabled

            timer->setEnabled(true);
            REQUIRE(timer->isEnabled());
            REQUIRE(count == 0);
            tgui::Timer::updateTime(std::chrono::milliseconds(750));
            REQUIRE(count == 1);

            timer->setEnabled(false);
            REQUIRE(!timer->isEnabled());
            tgui::Timer::updateTime(std::chrono::milliseconds(750));
            timer->setEnabled(true);
            tgui::Timer::updateTime(std::chrono::milliseconds(400));
            REQUIRE(count == 1); // Timer was restarted, so not elapsed yet

            tgui::Timer::updateTime(std::chrono::milliseconds(150));
            REQUIRE(count == 2);

            timer->setEnabled(false);
        }

        SECTION("Interval")
        {
            auto timer = tgui::Timer::create(callback, 250);
            REQUIRE(timer->getInterval() == std::chrono::milliseconds(250));

            tgui::Timer::updateTime(std::chrono::milliseconds(240));
            timer->setInterval(350);

            tgui::Timer::updateTime(std::chrono::milliseconds(300));
            REQUIRE(count == 0); // Timer was restarted when interval changed

            timer->setInterval(350);
            tgui::Timer::updateTime(std::chrono::milliseconds(100));
            REQUIRE(count == 1); // Timer wasn't restarted when interval was set to same value

            REQUIRE(timer->getInterval() == std::chrono::milliseconds(350));

            timer->setEnabled(false);
        }

        SECTION("Callback")
        {
            unsigned int otherCount = 0;
            const auto otherCallback = [&otherCount]{ ++otherCount; };

            auto timer = tgui::Timer::create(callback, 250);
            timer->setCallback(otherCallback);

            tgui::Timer::updateTime(std::chrono::milliseconds(300));
            REQUIRE(count == 0);
            REQUIRE(otherCount == 1);

            timer->setEnabled(false);
        }
    }

    SECTION("scheduleCallback")
    {
        SECTION("With time")
        {
            tgui::Timer::scheduleCallback(callback, 250);

            tgui::Timer::updateTime(std::chrono::milliseconds(300));
            REQUIRE(count == 1);

            tgui::Timer::updateTime(std::chrono::milliseconds(300));
            REQUIRE(count == 1); // Only triggered once
        }

        SECTION("Without time")
        {
            tgui::Timer::scheduleCallback(callback);
            tgui::Timer::updateTime(std::chrono::nanoseconds(1));
            REQUIRE(count == 1);

            tgui::Timer::scheduleCallback(callback);
            REQUIRE(count == 1);
            tgui::Timer::updateTime(tgui::Duration());
            REQUIRE(count == 2);
        }
    }

    SECTION("getNextScheduledTime")
    {
        tgui::Optional<tgui::Duration> duration;
        duration = tgui::Timer::getNextScheduledTime();
        REQUIRE(!duration); // There are no timers yet

        tgui::Timer::scheduleCallback(callback, 250);
        auto timer = tgui::Timer::create(callback, 400);

        duration = tgui::Timer::getNextScheduledTime();
        REQUIRE(duration);
        REQUIRE(*duration == std::chrono::milliseconds(250));

        tgui::Timer::updateTime(std::chrono::milliseconds(150));
        duration = tgui::Timer::getNextScheduledTime();
        REQUIRE(*duration == std::chrono::milliseconds(100));

        tgui::Timer::updateTime(std::chrono::milliseconds(150));
        duration = tgui::Timer::getNextScheduledTime();
        REQUIRE(*duration == std::chrono::milliseconds(100));

        tgui::Timer::updateTime(std::chrono::milliseconds(200));
        duration = tgui::Timer::getNextScheduledTime();
        REQUIRE(*duration == std::chrono::milliseconds(400)); // Timer was just restarted

        timer->setEnabled(false);
        duration = tgui::Timer::getNextScheduledTime();
        REQUIRE(!duration); // No active timers
    }
}
