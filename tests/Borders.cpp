/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
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

#include "catch.hpp"
#include <TGUI/Outline.hpp>

TEST_CASE("[Outline]") {
    SECTION("Constructors") {
        auto outline = tgui::Outline();
        REQUIRE(outline.left == 0);
        REQUIRE(outline.top == 0);
        REQUIRE(outline.right == 0);
        REQUIRE(outline.bottom == 0);

        outline = tgui::Outline(20);
        REQUIRE(outline.left == 20);
        REQUIRE(outline.top == 20);
        REQUIRE(outline.right == 20);
        REQUIRE(outline.bottom == 20);

        outline = tgui::Outline(10, 5);
        REQUIRE(outline.left == 10);
        REQUIRE(outline.top == 5);
        REQUIRE(outline.right == 10);
        REQUIRE(outline.bottom == 5);

        outline = tgui::Outline(1, 2, 3, 4);
        REQUIRE(outline.left == 1);
        REQUIRE(outline.top == 2);
        REQUIRE(outline.right == 3);
        REQUIRE(outline.bottom == 4);
    }

    SECTION("Operators") {
        auto outline0 = tgui::Outline();
        auto outline1 = tgui::Outline(1, 2, 3, 4);
        auto outline2 = tgui::Outline(1, 2, 3, 5);
        auto outline3 = tgui::Outline(6, 7, 8, 9);
        auto outline4 = tgui::Outline(1, 2, 3, 4);

        SECTION("==") {
            REQUIRE(!(outline1 == outline2));
            REQUIRE(!(outline1 == outline3));
            REQUIRE(!(outline2 == outline3));
            REQUIRE(outline1 == outline1);
            REQUIRE(outline1 == outline4);
            REQUIRE(outline4 == outline4);
        }

        SECTION("!=") {
            REQUIRE(outline1 != outline2);
            REQUIRE(outline1 != outline3);
            REQUIRE(outline2 != outline3);
            REQUIRE(!(outline1 != outline1));
            REQUIRE(!(outline1 != outline4));
            REQUIRE(!(outline4 != outline4));
        }

        SECTION("+") {
            REQUIRE((outline1 + outline3) == tgui::Outline(7, 9, 11, 13));
            REQUIRE((outline2 + outline0) == outline2);
        }

        SECTION("-") {
            REQUIRE((outline3 - outline1) == tgui::Outline(5, 5, 5, 5));
            REQUIRE((outline2 - outline0) == outline2);
        }
    }
}
