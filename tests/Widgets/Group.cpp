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
#include <TGUI/Widgets/Group.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <TGUI/Widgets/ClickableWidget.hpp>

TEST_CASE("[Group]")
{
    tgui::Group::Ptr group = tgui::Group::create();
    group->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("WidgetType")
    {
        REQUIRE(group->getWidgetType() == "Group");
    }

    SECTION("Position and Size")
    {
        group->setPosition(40, 30);
        group->setSize(150, 100);

        SECTION("Without outline")
        {
            REQUIRE(group->getPosition() == sf::Vector2f(40, 30));
            REQUIRE(group->getSize() == sf::Vector2f(150, 100));
            REQUIRE(group->getFullSize() == group->getSize());
            REQUIRE(group->getWidgetOffset() == sf::Vector2f(0, 0));
            REQUIRE(group->getChildWidgetsOffset() == sf::Vector2f(0, 0));
        }

        SECTION("With outline")
        {
            group->getRenderer()->setPadding({1, 2, 3, 4});

            REQUIRE(group->getPosition() == sf::Vector2f(40, 30));
            REQUIRE(group->getSize() == sf::Vector2f(150, 100));
            REQUIRE(group->getFullSize() == group->getSize());
            REQUIRE(group->getWidgetOffset() == sf::Vector2f(0, 0));
            REQUIRE(group->getChildWidgetsOffset() == sf::Vector2f(1, 2));
        }

        SECTION("Child widgets")
        {
            auto childWidget = tgui::ClickableWidget::create();
            childWidget->setPosition(60, 50);
            group->add(childWidget);

            REQUIRE(childWidget->getPosition() == sf::Vector2f(60, 50));
            REQUIRE(childWidget->getAbsolutePosition() == sf::Vector2f(100, 80));

            group->getRenderer()->setPadding({1, 2, 3, 4});
            REQUIRE(childWidget->getPosition() == sf::Vector2f(60, 50));
            REQUIRE(childWidget->getAbsolutePosition() == sf::Vector2f(101, 82));
        }
    }

    testWidgetRenderer(group->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = group->getRenderer();

        SECTION("set serialized property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("Padding", "(1, 2, 3, 4)"));
        }

        SECTION("set object property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Padding{1, 2, 3, 4}));
        }

        SECTION("functions")
        {
            renderer->setPadding({1, 2, 3, 4});
        }

        REQUIRE(renderer->getProperty("Padding").getOutline() == tgui::Padding(1, 2, 3, 4));

        REQUIRE(renderer->getPadding() == tgui::Padding(1, 2, 3, 4));
    }

    SECTION("Saving and loading from file")
    {
        group = tgui::Group::create({400, 300});

        auto widget = tgui::ClickableWidget::create();
        widget->setPosition({20, 10});
        widget->setSize({150, 100});
        group->add(widget);

        SECTION("Only save contents")
        {
            REQUIRE_NOTHROW(group->saveWidgetsToFile("GroupWidgetFile1.txt"));
            
            group->setSize(200, 100);
            REQUIRE_NOTHROW(group->loadWidgetsFromFile("GroupWidgetFile1.txt"));
            REQUIRE(group->getSize() == sf::Vector2f(200, 100)); // The Group itself is not saved, only its children

            REQUIRE_NOTHROW(group->saveWidgetsToFile("GroupWidgetFile2.txt"));
            REQUIRE(compareFiles("GroupWidgetFile1.txt", "GroupWidgetFile2.txt"));
        }

        SECTION("Save entire group")
        {
            testSavingWidget("Group", group, false);
        }
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(200, 150, group)

        tgui::GroupRenderer renderer = tgui::RendererData::create();
        renderer.setPadding({1, 2, 3, 4});
        renderer.setOpacity(0.7f);
        group->setRenderer(renderer.getData());

        group->setSize({180, 140});
        group->setPosition({10, 5});

        auto picture = tgui::Picture::create("resources/image.png");
        picture->setSize({150, 100});
        picture->setPosition({55, 60});
        group->add(picture);

        TEST_DRAW("Group.png")
    }
}
