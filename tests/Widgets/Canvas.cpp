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

#include "../Tests.hpp"
#include <TGUI/Widgets/Canvas.hpp>

TEST_CASE("[Canvas]") {
    tgui::Canvas::Ptr canvas = std::make_shared<tgui::Canvas>();
    canvas->setFont("resources/DroidSansArmenian.ttf");

    SECTION("WidgetType") {
        REQUIRE(canvas->getWidgetType() == "Canvas");
    }

    SECTION("constructor") {
        canvas = std::make_shared<tgui::Canvas>(200.f, 100.f);
        REQUIRE(canvas->getSize() == sf::Vector2f(200, 100));
    }

    SECTION("Saving and loading from file") {
        REQUIRE_NOTHROW(canvas = std::make_shared<tgui::Canvas>(60.f, 40.f));

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(canvas);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileCanvas1.txt"));
        
        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileCanvas1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileCanvas2.txt"));
        REQUIRE(compareFiles("WidgetFileCanvas1.txt", "WidgetFileCanvas2.txt"));

        SECTION("Copying widget") {
            tgui::Canvas temp;
            temp = *canvas;

            parent->removeAllWidgets();
            parent->add(tgui::Canvas::copy(std::make_shared<tgui::Canvas>(temp)));

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileCanvas2.txt"));
            REQUIRE(compareFiles("WidgetFileCanvas1.txt", "WidgetFileCanvas2.txt"));
        }
    }
}
