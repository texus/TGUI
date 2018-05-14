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
#include <TGUI/Widgets/Grid.hpp>
#include <TGUI/Widgets/ClickableWidget.hpp>

TEST_CASE("[Grid]")
{
    tgui::Grid::Ptr grid = tgui::Grid::create();

    SECTION("WidgetType")
    {
        REQUIRE(grid->getWidgetType() == "Grid");
    }

    SECTION("Adding widgets")
    {
        auto widget1 = tgui::ClickableWidget::create({100, 50});
        grid->addWidget(widget1, 3, 2);

        REQUIRE(grid->getWidgets().size() == 1);
        REQUIRE(grid->getWidget(3, 2) == widget1);
        REQUIRE(grid->getWidgetPadding(3, 2) ==  tgui::Padding(0));
        REQUIRE(grid->getWidgetAlignment(3, 2) == tgui::Grid::Alignment::Center);

        // Accessing empty cell
        REQUIRE(grid->getWidget(0, 0) == nullptr);
        REQUIRE(grid->getWidgetPadding(0, 0) ==  tgui::Padding{0});
        REQUIRE(grid->getWidgetAlignment(0, 0) == tgui::Grid::Alignment::Center);

        auto widget2 = tgui::ClickableWidget::create({20, 10});
        grid->add(widget2); // Widget is added before calling addWidget here
        grid->addWidget(widget2, 0, 0, {1, 2, 3, 4}, tgui::Grid::Alignment::UpperLeft);

        REQUIRE(grid->getWidgets().size() == 2); // addWidget did not add a duplicate or widget2
        REQUIRE(grid->getWidget(0, 0) == widget2);
        REQUIRE(grid->getWidgetPadding(0, 0) ==  tgui::Padding(1, 2, 3, 4));
        REQUIRE(grid->getWidgetAlignment(0, 0) == tgui::Grid::Alignment::UpperLeft);
    }

    SECTION("Removing widgets")
    {
        auto widget = tgui::ClickableWidget::create();
        grid->addWidget(widget, 3, 2);

        REQUIRE(grid->getWidget(3, 2) == widget);
        grid->remove(widget);
        REQUIRE(grid->getWidget(3, 2) == nullptr);

        grid->addWidget(widget, 1, 5);

        REQUIRE(grid->getWidget(1, 5) == widget);
        grid->removeAllWidgets();
        REQUIRE(grid->getWidget(1, 5) == nullptr);
    }

    SECTION("Borders")
    {
        auto widget = tgui::ClickableWidget::create({40, 30});
        grid->addWidget(widget, 3, 2, {1, 2, 3, 4});

        REQUIRE(grid->getWidgetPadding(3, 2) ==  tgui::Padding(1, 2, 3, 4));
        grid->setWidgetPadding(widget, {});
        REQUIRE(grid->getWidgetPadding(3, 2) ==  tgui::Padding(0, 0, 0, 0));
        grid->setWidgetPadding(3, 2, {5, 6, 7, 8});
        REQUIRE(grid->getWidgetPadding(3, 2) ==  tgui::Padding(5, 6, 7, 8));

        // You can't change the borders of an empty cell
        grid->setWidgetPadding(0, 0, {1, 2, 3, 4});
        REQUIRE(grid->getWidgetPadding(0, 0) ==  tgui::Padding(0, 0, 0, 0));
    }

    SECTION("Alignment")
    {
        auto widget = tgui::ClickableWidget::create();
        grid->addWidget(widget, 3, 2, {}, tgui::Grid::Alignment::BottomRight);

        REQUIRE(grid->getWidgetAlignment(3, 2) == tgui::Grid::Alignment::BottomRight);
        grid->setWidgetAlignment(widget, tgui::Grid::Alignment::Center);
        REQUIRE(grid->getWidgetAlignment(3, 2) == tgui::Grid::Alignment::Center);
        grid->setWidgetAlignment(3, 2, tgui::Grid::Alignment::Up);
        REQUIRE(grid->getWidgetAlignment(3, 2) == tgui::Grid::Alignment::Up);

        // You can't change the alignment of an empty cell
        grid->setWidgetAlignment(0, 0, tgui::Grid::Alignment::Left);
        REQUIRE(grid->getWidgetAlignment(0, 0) == tgui::Grid::Alignment::Center);
    }

    SECTION("Saving and loading from file")
    {
        grid->setSize({800, 600});
        grid->addWidget(tgui::ClickableWidget::create({40, 30}), 5, 4);
        grid->addWidget(tgui::ClickableWidget::create({40, 30}), 2, 7, {1, 2, 3, 4});
        grid->addWidget(tgui::ClickableWidget::create({40, 30}), 0, 0, {}, tgui::Grid::Alignment::Center);
        grid->addWidget(tgui::ClickableWidget::create({40, 30}), 0, 1, {}, tgui::Grid::Alignment::Left);
        grid->addWidget(tgui::ClickableWidget::create({40, 30}), 0, 2, {}, tgui::Grid::Alignment::UpperLeft);
        grid->addWidget(tgui::ClickableWidget::create({40, 30}), 1, 0, {}, tgui::Grid::Alignment::Up);
        grid->addWidget(tgui::ClickableWidget::create({40, 30}), 1, 1, {}, tgui::Grid::Alignment::UpperRight);
        grid->addWidget(tgui::ClickableWidget::create({40, 30}), 1, 2, {}, tgui::Grid::Alignment::Right);
        grid->addWidget(tgui::ClickableWidget::create({40, 30}), 2, 0, {}, tgui::Grid::Alignment::BottomRight);
        grid->addWidget(tgui::ClickableWidget::create({40, 30}), 2, 1, {}, tgui::Grid::Alignment::Bottom);
        grid->addWidget(tgui::ClickableWidget::create({40, 30}), 2, 2, {}, tgui::Grid::Alignment::BottomLeft);
        grid->add(tgui::ClickableWidget::create({40, 30}));

        testSavingWidget("Grid", grid, false);
    }
}
