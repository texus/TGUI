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
#include <TGUI/AbsoluteOrRelativeValue.hpp>

TEST_CASE("[AbsoluteOrRelativeValue]")
{
    SECTION("constants")
    {
        tgui::AbsoluteOrRelativeValue v1;
        REQUIRE(v1.toString() == "0");

        tgui::AbsoluteOrRelativeValue v2(-20.3f);
        REQUIRE(v2.toString() == "-20.3");

        tgui::AbsoluteOrRelativeValue v3{"60"};
        REQUIRE(v3.toString() == "60");

        tgui::AbsoluteOrRelativeValue v4{""};
        REQUIRE(v4.getValue() == 0);

        SECTION("updating parent size")
        {
            v1.updateParentSize(110);
            v2.updateParentSize(120);
            v3.updateParentSize(130);
            v4.updateParentSize(140);

            REQUIRE(v1.getValue() == 0);
            REQUIRE(v2.getValue() == -20.3f);
            REQUIRE(v3.getValue() == 60);
            REQUIRE(v4.getValue() == 0);
        }
    }

    SECTION("relative values")
    {
        tgui::AbsoluteOrRelativeValue v1{"60%"};
        REQUIRE(v1.toString() == "60%");

        tgui::AbsoluteOrRelativeValue v2{tgui::RelativeValue{0.35f}};
        REQUIRE(v2.toString() == "35%");

        SECTION("updating parent size")
        {
            REQUIRE(v1.getValue() == 0);
            v1.updateParentSize(250);
            REQUIRE(v1.getValue() == 250*0.6f);
            v1.updateParentSize(100);
            REQUIRE(v1.getValue() == 100*0.6f);
            REQUIRE(v1.toString() == "60%");

            v2.updateParentSize(40);
            REQUIRE(v2.getValue() == 14);
            REQUIRE(v2.toString() == "35%");
        }
    }
}
