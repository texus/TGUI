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
#include <TGUI/Widgets/RadioButtonGroup.hpp>
#include <TGUI/Widgets/RadioButton.hpp>

TEST_CASE("[RadioButtonGroup]")
{
    tgui::RadioButtonGroup::Ptr group = tgui::RadioButtonGroup::create();
    group->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("WidgetType")
    {
        REQUIRE(group->getWidgetType() == "RadioButtonGroup");
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
        group = tgui::RadioButtonGroup::create();
        group->setSize(400, 300);

        auto radioButton = tgui::RadioButton::create();
        radioButton->setPosition({20, 10});
        radioButton->setSize({30, 30});
        radioButton->setText("Number one");
        group->add(radioButton);

        radioButton = tgui::RadioButton::copy(radioButton);
        radioButton->setPosition({20, 60});
        radioButton->setText("Number two");
        radioButton->setChecked(true);
        group->add(radioButton);

        SECTION("Only save contents")
        {
            REQUIRE_NOTHROW(group->saveWidgetsToFile("RadioButtonGroupWidgetFile1.txt"));
            
            group->setSize(200, 100);
            REQUIRE_NOTHROW(group->loadWidgetsFromFile("RadioButtonGroupWidgetFile1.txt"));
            REQUIRE(group->getSize() == sf::Vector2f(200, 100)); // The Group itself is not saved, only its children

            REQUIRE_NOTHROW(group->saveWidgetsToFile("RadioButtonGroupWidgetFile2.txt"));
            REQUIRE(compareFiles("RadioButtonGroupWidgetFile1.txt", "RadioButtonGroupWidgetFile2.txt"));
        }

        SECTION("Save entire group")
        {
            testSavingWidget("RadioButtonGroup", group, false);
        }
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(200, 110, group)

        group->setPosition({10, 5});
        group->setSize({4, 3}); // RadioButtonGroup ignores its size

        tgui::GroupRenderer renderer = tgui::RendererData::create();
        renderer.setPadding({1, 2, 3, 4});
        renderer.setOpacity(0.7f);
        group->setRenderer(renderer.getData());

        auto radioButton = tgui::RadioButton::create();
        radioButton->setPosition({20, 10});
        radioButton->setSize({30, 30});
        radioButton->setText("Number one");
        group->add(radioButton);

        radioButton = tgui::RadioButton::copy(radioButton);
        radioButton->setPosition({20, 60});
        radioButton->setText("Number two");
        radioButton->setChecked(true);
        group->add(radioButton);

        TEST_DRAW("RadioButtonGroup.png")
    }
}
