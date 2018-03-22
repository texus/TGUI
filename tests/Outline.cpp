/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Outline.hpp>

TEST_CASE("[Outline]")
{
    SECTION("Constructors")
    {
        SECTION("Constant")
        {
            auto outline = tgui::Outline();
            REQUIRE(outline.getLeft() == 0);
            REQUIRE(outline.getTop() == 0);
            REQUIRE(outline.getRight() == 0);
            REQUIRE(outline.getBottom() == 0);

            outline = tgui::Outline(20);
            REQUIRE(outline.getLeft() == 20);
            REQUIRE(outline.getTop() == 20);
            REQUIRE(outline.getRight() == 20);
            REQUIRE(outline.getBottom() == 20);

            outline = tgui::Outline(10, 5);
            REQUIRE(outline.getLeft() == 10);
            REQUIRE(outline.getTop() == 5);
            REQUIRE(outline.getRight() == 10);
            REQUIRE(outline.getBottom() == 5);

            outline = tgui::Outline(1, 2, 3, 4);
            REQUIRE(outline.getLeft() == 1);
            REQUIRE(outline.getTop() == 2);
            REQUIRE(outline.getRight() == 3);
            REQUIRE(outline.getBottom() == 4);
        }

        SECTION("Relative")
        {
            auto outline1 = tgui::Outline("10%");
            auto outline2 = tgui::Outline("1%", "2%", "3%", "4%");
            auto outline3 = tgui::Outline(15, "20%");

            SECTION("Before updating parent size")
            {
                REQUIRE(outline1.getLeft() == 0);
                REQUIRE(outline1.getTop() == 0);
                REQUIRE(outline1.getRight() == 0);
                REQUIRE(outline1.getBottom() == 0);

                REQUIRE(outline2.getLeft() == 0);
                REQUIRE(outline2.getTop() == 0);
                REQUIRE(outline2.getRight() == 0);
                REQUIRE(outline2.getBottom() == 0);

                REQUIRE(outline3.getLeft() == 15);
                REQUIRE(outline3.getTop() == 0);
                REQUIRE(outline3.getRight() == 15);
                REQUIRE(outline3.getBottom() == 0);
            }

            outline1.updateParentSize({200, 100});
            outline2.updateParentSize({200, 100});
            outline3.updateParentSize({200, 100});

            SECTION("After updating parent size")
            {
                REQUIRE(outline1.getLeft() == 20);
                REQUIRE(outline1.getTop() == 10);
                REQUIRE(outline1.getRight() == 20);
                REQUIRE(outline1.getBottom() == 10);

                REQUIRE(outline2.getLeft() == 2);
                REQUIRE(outline2.getTop() == 2);
                REQUIRE(outline2.getRight() == 6);
                REQUIRE(outline2.getBottom() == 4);

                REQUIRE(outline3.getLeft() == 15);
                REQUIRE(outline3.getTop() == 20);
                REQUIRE(outline3.getRight() == 15);
                REQUIRE(outline3.getBottom() == 20);
            }
        }
    }

    SECTION("Operators")
    {
        auto outline1 = tgui::Outline(1, 2, 3, 4);
        auto outline2 = tgui::Outline(1, 2, 3, 5);
        auto outline3 = tgui::Outline(6, 7, 8, 9);
        auto outline4 = tgui::Outline(1, 2, 3, 4);

        SECTION("==")
        {
            REQUIRE(!(outline1 == outline2));
            REQUIRE(!(outline1 == outline3));
            REQUIRE(!(outline2 == outline3));
            REQUIRE(outline1 == outline1);
            REQUIRE(outline1 == outline4);
            REQUIRE(outline4 == outline4);
        }

        SECTION("!=")
        {
            REQUIRE(outline1 != outline2);
            REQUIRE(outline1 != outline3);
            REQUIRE(outline2 != outline3);
            REQUIRE(!(outline1 != outline1));
            REQUIRE(!(outline1 != outline4));
            REQUIRE(!(outline4 != outline4));
        }
    }

    SECTION("toString")
    {
        auto outline = tgui::Outline();
        REQUIRE(outline.toString() == "(0, 0, 0, 0)");

        outline = tgui::Outline(20);
        REQUIRE(outline.toString() == "(20, 20, 20, 20)");

        outline = tgui::Outline(10, 5);
        REQUIRE(outline.toString() == "(10, 5, 10, 5)");

        outline = tgui::Outline(1, 2, 3, 4);
        REQUIRE(outline.toString() == "(1, 2, 3, 4)");

        outline = tgui::Outline("10%");
        REQUIRE(outline.toString() == "(10%, 10%, 10%, 10%)");

        outline = tgui::Outline("1%", "2%", "3%", "4%");
        REQUIRE(outline.toString() == "(1%, 2%, 3%, 4%)");

        outline = tgui::Outline(15, "20%");
        REQUIRE(outline.toString() == "(15, 20%, 15, 20%)");
    }
}
