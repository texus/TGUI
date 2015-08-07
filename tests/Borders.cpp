/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Borders.hpp>

TEST_CASE("[Borders]") {
    SECTION("Constructors") {
        auto borders = tgui::Borders();
        REQUIRE(borders.left == 0);
        REQUIRE(borders.top == 0);
        REQUIRE(borders.right == 0);
        REQUIRE(borders.bottom == 0);

        borders = tgui::Borders(20);
        REQUIRE(borders.left == 20);
        REQUIRE(borders.top == 20);
        REQUIRE(borders.right == 20);
        REQUIRE(borders.bottom == 20);

        borders = tgui::Borders(10, 5);
        REQUIRE(borders.left == 10);
        REQUIRE(borders.top == 5);
        REQUIRE(borders.right == 10);
        REQUIRE(borders.bottom == 5);

        borders = tgui::Borders(1, 2, 3, 4);
        REQUIRE(borders.left == 1);
        REQUIRE(borders.top == 2);
        REQUIRE(borders.right == 3);
        REQUIRE(borders.bottom == 4);
    }

    SECTION("WidgetBorders") {
        tgui::WidgetBorders widgetBorders;
        REQUIRE(widgetBorders.getBorders().left == 0);
        REQUIRE(widgetBorders.getBorders().top == 0);
        REQUIRE(widgetBorders.getBorders().right == 0);
        REQUIRE(widgetBorders.getBorders().bottom == 0);
        
        widgetBorders.setBorders({1, 2, 3, 4});
        REQUIRE(widgetBorders.getBorders().left == 1);
        REQUIRE(widgetBorders.getBorders().top == 2);
        REQUIRE(widgetBorders.getBorders().right == 3);
        REQUIRE(widgetBorders.getBorders().bottom == 4);
    }
    
    SECTION("WidgetPadding") {
        tgui::WidgetPadding widgetPadding;
        REQUIRE(widgetPadding.getPadding().left == 0);
        REQUIRE(widgetPadding.getPadding().top == 0);
        REQUIRE(widgetPadding.getPadding().right == 0);
        REQUIRE(widgetPadding.getPadding().bottom == 0);
        
        widgetPadding.setPadding({1, 2, 3, 4});
        REQUIRE(widgetPadding.getPadding().left == 1);
        REQUIRE(widgetPadding.getPadding().top == 2);
        REQUIRE(widgetPadding.getPadding().right == 3);
        REQUIRE(widgetPadding.getPadding().bottom == 4);
    }
}
