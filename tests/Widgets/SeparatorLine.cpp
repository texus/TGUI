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

TEST_CASE("[SeparatorLine]")
{
    tgui::SeparatorLine::Ptr separator = tgui::SeparatorLine::create();
    separator->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        separator->onClick([](){});
        separator->onClick([](tgui::Vector2f){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(separator)->getSignal("Clicked").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(separator->getWidgetType() == "SeparatorLine");
    }

    SECTION("Position and Size")
    {
        separator->setPosition(40, 30);
        separator->setSize(150, 3);

        REQUIRE(separator->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(separator->getSize() == tgui::Vector2f(150, 3));
        REQUIRE(separator->getFullSize() == separator->getSize());
        REQUIRE(separator->getWidgetOffset() == tgui::Vector2f(0, 0));
    }

    SECTION("Renderer")
    {
        auto renderer = separator->getRenderer();

        SECTION("set serialized property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("Color", "rgb(10, 20, 30)"));
        }

        SECTION("set object property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("Color", tgui::Color{10, 20, 30}));
        }

        SECTION("functions")
        {
            renderer->setColor(tgui::Color{10, 20, 30});
        }

        REQUIRE(renderer->getProperty("Color").getColor() == tgui::Color{10, 20, 30});
        REQUIRE(renderer->getColor() == tgui::Color{10, 20, 30});
    }

    SECTION("Saving and loading from file")
    {
        separator->setSize({2, 100});

        testSavingWidget("SeparatorLine", separator);
    }
}
