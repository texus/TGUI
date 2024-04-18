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

#include <TGUI/Config.hpp>
#if TGUI_HAS_WINDOW_BACKEND_SFML
    #include <SFML/System/Time.hpp>
#endif

#include "Tests.hpp"

TEST_CASE("[Duration]")
{
    SECTION("Constructor")
    {
        SECTION("Default initialized to 0")
        {
            REQUIRE(tgui::Duration().asSeconds() == 0);
        }

        SECTION("Constructors")
        {
            REQUIRE(tgui::Duration(std::chrono::microseconds(10)) == std::chrono::nanoseconds(10000));
            REQUIRE(tgui::Duration(std::chrono::milliseconds(500) / 2) == std::chrono::nanoseconds(250000000));
            REQUIRE(tgui::Duration(std::chrono::microseconds(10) / 5.0) == std::chrono::nanoseconds(2000));
            REQUIRE(tgui::Duration(0) == std::chrono::nanoseconds(0));
            REQUIRE(tgui::Duration(15) == std::chrono::milliseconds(15));

#if TGUI_HAS_WINDOW_BACKEND_SFML
            REQUIRE(tgui::Duration(sf::milliseconds(20)) == std::chrono::milliseconds(20));
#endif
        }
    }

    SECTION("Conversions")
    {
        REQUIRE(std::chrono::duration<int>(tgui::Duration(10000)).count() == 10);
        REQUIRE(std::chrono::nanoseconds(tgui::Duration(std::chrono::microseconds(3))).count() == 3000);
        REQUIRE(std::chrono::microseconds(tgui::Duration(5)).count() == 5000);
        REQUIRE(std::chrono::milliseconds(tgui::Duration(10)).count() == 10);

#if TGUI_HAS_WINDOW_BACKEND_SFML
        REQUIRE(sf::Time(tgui::Duration(std::chrono::milliseconds(50))).asMilliseconds() == 50);
#endif
    }

    SECTION("Comparison operators")
    {
        REQUIRE(tgui::Duration(std::chrono::microseconds(20000)) == tgui::Duration(20));
        REQUIRE(tgui::Duration(std::chrono::microseconds(30000)) != tgui::Duration(20));
        REQUIRE(tgui::Duration(std::chrono::microseconds(19000)) < tgui::Duration(20));
        REQUIRE(tgui::Duration(std::chrono::microseconds(19000)) <= tgui::Duration(20));
        REQUIRE(tgui::Duration(std::chrono::microseconds(21000)) >= tgui::Duration(20));
        REQUIRE(tgui::Duration(std::chrono::microseconds(21000)) >= tgui::Duration(20));
    }

    SECTION("Arithmetic operators")
    {
        REQUIRE(tgui::Duration(std::chrono::microseconds(20000)) + tgui::Duration(30) == tgui::Duration(std::chrono::milliseconds(50)));
        REQUIRE(tgui::Duration(std::chrono::microseconds(20000)) - tgui::Duration(5) == tgui::Duration(std::chrono::milliseconds(15)));
        REQUIRE(tgui::Duration(30) * 20 == tgui::Duration(std::chrono::milliseconds(600)));
        REQUIRE(10.0 * tgui::Duration(30) == tgui::Duration(std::chrono::milliseconds(300)));
        REQUIRE(tgui::Duration(30) / 2.f == tgui::Duration(std::chrono::milliseconds(15)));
        REQUIRE(tgui::Duration(5000) / tgui::Duration(std::chrono::microseconds(2000000)) == 2.5f);
        REQUIRE(tgui::Duration(std::chrono::nanoseconds(100)) % 3 == tgui::Duration(std::chrono::nanoseconds(1)));
        REQUIRE(tgui::Duration(std::chrono::nanoseconds(100)) % std::chrono::nanoseconds(6) == tgui::Duration(std::chrono::nanoseconds(4)));

        tgui::Duration d;
        d = tgui::Duration(std::chrono::microseconds(20000)); REQUIRE((d += tgui::Duration(30)) == tgui::Duration(std::chrono::milliseconds(50)));
        d = tgui::Duration(std::chrono::microseconds(20000)); REQUIRE((d -= tgui::Duration(5)) == tgui::Duration(std::chrono::milliseconds(15)));
        d = tgui::Duration(30); REQUIRE((d *= 20) == tgui::Duration(std::chrono::milliseconds(600)));
        d = tgui::Duration(30); REQUIRE((d /= 2.f) == tgui::Duration(std::chrono::milliseconds(15)));
        d = tgui::Duration(std::chrono::nanoseconds(100)); REQUIRE((d %= 3) == tgui::Duration(std::chrono::nanoseconds(1)));
        d = tgui::Duration(std::chrono::nanoseconds(100)); REQUIRE((d %= std::chrono::nanoseconds(6)) == tgui::Duration(std::chrono::nanoseconds(4)));
    }
}
