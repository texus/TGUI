/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2019 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Widgets/ListView.hpp>

TEST_CASE("[ListView]")
{
    tgui::ListView::Ptr listView = tgui::ListView::create();
    listView->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(listView->connect("ItemSelected", [](){}));
        REQUIRE_NOTHROW(listView->connect("ItemSelected", [](int){}));
        REQUIRE_NOTHROW(listView->connect("ItemSelected", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(listView->connect("ItemSelected", [](tgui::Widget::Ptr, std::string, int){}));

        REQUIRE_NOTHROW(listView->connect("DoubleClicked", [](){}));
        REQUIRE_NOTHROW(listView->connect("DoubleClicked", [](int){}));
        REQUIRE_NOTHROW(listView->connect("DoubleClicked", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(listView->connect("DoubleClicked", [](tgui::Widget::Ptr, std::string, int){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(listView->getWidgetType() == "ListView");
    }

    SECTION("Position and Size")
    {
        listView->setPosition(40, 30);
        listView->setSize(150, 100);
        listView->getRenderer()->setBorders(2);

        REQUIRE(listView->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(listView->getSize() == sf::Vector2f(150, 100));
        REQUIRE(listView->getFullSize() == listView->getSize());
        REQUIRE(listView->getWidgetOffset() == sf::Vector2f(0, 0));
    }

    // TODO
}
