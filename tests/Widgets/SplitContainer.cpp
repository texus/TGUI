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

TEST_CASE("[SplitContainer]")
{
    tgui::SplitContainer::Ptr splitContainer = tgui::SplitContainer::create();
    splitContainer->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("WidgetType")
    {
        REQUIRE(splitContainer->getWidgetType() == "SplitContainer");
    }

    SECTION("Position and Size")
    {
        splitContainer->setPosition(40, 30);
        splitContainer->setSize(150, 100);

        REQUIRE(splitContainer->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(splitContainer->getSize() == tgui::Vector2f(150, 100));
        REQUIRE(splitContainer->getFullSize() == tgui::Vector2f(150, 100));
        REQUIRE(splitContainer->getWidgetOffset() == tgui::Vector2f(0, 0));
    }

    SECTION("SplitterWidth")
    {
        splitContainer->setSplitterWidth(2);
        REQUIRE(splitContainer->getSplitterWidth() == 2);
    }

    SECTION("MinimumGrabWidth")
    {
        splitContainer->setMinimumGrabWidth(4);
        REQUIRE(splitContainer->getMinimumGrabWidth() == 4);
    }

    SECTION("Orientation")
    {
        REQUIRE(splitContainer->getOrientation() == tgui::Orientation::Horizontal);
        splitContainer->setOrientation(tgui::Orientation::Vertical);
        REQUIRE(splitContainer->getOrientation() == tgui::Orientation::Vertical);
        splitContainer->setOrientation(tgui::Orientation::Horizontal);
        REQUIRE(splitContainer->getOrientation() == tgui::Orientation::Horizontal);
    }

    SECTION("SplitterOffset")
    {
        splitContainer->setSize(200, 60);

        REQUIRE(splitContainer->getMinValidSplitterOffset() == 0);
        REQUIRE(splitContainer->getMaxValidSplitterOffset() == 200);

        splitContainer->setMinValidSplitterOffset(25);
        splitContainer->setMaxValidSplitterOffset(65);
        REQUIRE(splitContainer->getMinValidSplitterOffset() == 25);
        REQUIRE(splitContainer->getMaxValidSplitterOffset() == 65);

        splitContainer->setMinValidSplitterOffset("20%");
        splitContainer->setMaxValidSplitterOffset("70%");
        REQUIRE(splitContainer->getMinValidSplitterOffset() == 40);
        REQUIRE(splitContainer->getMaxValidSplitterOffset() == 140);

        splitContainer->setSplitterOffset(60);
        REQUIRE(splitContainer->getSplitterOffset() == 60);

        splitContainer->setSplitterOffset("40%");
        REQUIRE(splitContainer->getSplitterOffset() == 80);

        splitContainer->setSplitterOffset(10);
        REQUIRE(splitContainer->getSplitterOffset() == 40); // Limited by minimum

        splitContainer->setSplitterOffset(160);
        REQUIRE(splitContainer->getSplitterOffset() == 140); // Limited by maximum
    }

    SECTION("Child widgets")
    {
        splitContainer->setSplitterWidth(5);

        SECTION("Horizontal")
        {
            splitContainer->setSize(200, 60);
            splitContainer->setOrientation(tgui::Orientation::Horizontal);

            auto child1 = tgui::Panel::create();
            splitContainer->add(child1);

            splitContainer->setSplitterOffset(90);
            REQUIRE(child1->getPosition() == tgui::Vector2f{0, 0});
            REQUIRE(child1->getSize() == tgui::Vector2f{90, 60});

            auto child2 = tgui::Panel::create();
            splitContainer->add(child2);
            REQUIRE(child2->getPosition() == tgui::Vector2f{95, 0});
            REQUIRE(child2->getSize() == tgui::Vector2f{105, 60});

            splitContainer->setSplitterOffset(95);
            REQUIRE(child1->getPosition() == tgui::Vector2f{0, 0});
            REQUIRE(child1->getSize() == tgui::Vector2f{95, 60});
            REQUIRE(child2->getPosition() == tgui::Vector2f{100, 0});
            REQUIRE(child2->getSize() == tgui::Vector2f{100, 60});

            splitContainer->setWidgetIndex(child2, 0);
            REQUIRE(child1->getPosition() == tgui::Vector2f{100, 0});
            REQUIRE(child1->getSize() == tgui::Vector2f{100, 60});
            REQUIRE(child2->getPosition() == tgui::Vector2f{0, 0});
            REQUIRE(child2->getSize() == tgui::Vector2f{95, 60});
        }

        SECTION("Vertical")
        {
            splitContainer->setSize(50, 150);
            splitContainer->setOrientation(tgui::Orientation::Vertical);

            auto child1 = tgui::Panel::create();
            splitContainer->add(child1);

            splitContainer->setSplitterOffset(110);
            REQUIRE(child1->getPosition() == tgui::Vector2f{0, 0});
            REQUIRE(child1->getSize() == tgui::Vector2f{50, 110});

            auto child2 = tgui::Panel::create();
            splitContainer->add(child2);
            REQUIRE(child2->getPosition() == tgui::Vector2f{0, 115});
            REQUIRE(child2->getSize() == tgui::Vector2f{50, 35});

            splitContainer->setSplitterOffset(105);
            REQUIRE(child1->getPosition() == tgui::Vector2f{0, 0});
            REQUIRE(child1->getSize() == tgui::Vector2f{50, 105});
            REQUIRE(child2->getPosition() == tgui::Vector2f{0, 110});
            REQUIRE(child2->getSize() == tgui::Vector2f{50, 40});

            splitContainer->setWidgetIndex(child1, 1);
            REQUIRE(child1->getPosition() == tgui::Vector2f{0, 110});
            REQUIRE(child1->getSize() == tgui::Vector2f{50, 40});
            REQUIRE(child2->getPosition() == tgui::Vector2f{0, 0});
            REQUIRE(child2->getSize() == tgui::Vector2f{50, 105});
        }
    }

    SECTION("Events / Signals")
    {
        splitContainer->setPosition(40, 30);
        splitContainer->setSize(150, 100);

        SECTION("isMouseOnWidget")
        {
            REQUIRE(!splitContainer->isMouseOnWidget({39, 29}));
            REQUIRE(splitContainer->isMouseOnWidget({40, 30}));
            REQUIRE(splitContainer->isMouseOnWidget({115, 80}));
            REQUIRE(splitContainer->isMouseOnWidget({189, 129}));
            REQUIRE(!splitContainer->isMouseOnWidget({190, 130}));
        }

        SECTION("Dragging splitter")
        {
            splitContainer->setSize(200, 60);
            splitContainer->setOrientation(tgui::Orientation::Horizontal);
            splitContainer->setSplitterWidth(5);
            splitContainer->setSplitterOffset(80);

            auto child1 = tgui::Panel::create();
            auto child2 = tgui::Panel::create();
            splitContainer->add(child1);
            splitContainer->add(child2);

            splitContainer->mouseMoved({122, 75});
            splitContainer->leftMousePressed({122, 75});
            splitContainer->mouseMoved({92, 70});
            splitContainer->leftMouseReleased({92, 70});

            REQUIRE(child1->getPosition() == tgui::Vector2f{0, 0});
            REQUIRE(child1->getSize() == tgui::Vector2f{50, 60});
            REQUIRE(child2->getPosition() == tgui::Vector2f{55, 0});
            REQUIRE(child2->getSize() == tgui::Vector2f{145, 60});
        }
    }

    testWidgetRenderer(splitContainer->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = splitContainer->getRenderer();

        SECTION("colored")
        {
            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("SplitterColor", "rgb(10, 20, 30)"));
                REQUIRE_NOTHROW(renderer->setProperty("SplitterColorHover", "rgb(40, 50, 60)"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("SplitterColor", tgui::Color{10, 20, 30}));
                REQUIRE_NOTHROW(renderer->setProperty("SplitterColorHover", tgui::Color{40, 50, 60}));
            }

            SECTION("functions")
            {
                renderer->setSplitterColor({10, 20, 30});
                renderer->setSplitterColorHover({40, 50, 60});
            }

            REQUIRE(renderer->getProperty("SplitterColor").getColor() == tgui::Color(10, 20, 30));
            REQUIRE(renderer->getProperty("SplitterColorHover").getColor() == tgui::Color(40, 50, 60));
        }
    }

    SECTION("Saving and loading from file")
    {
        auto child1 = tgui::Panel::create();
        auto child2 = tgui::Panel::create();
        child1->getRenderer()->setBackgroundColor(tgui::Color::Blue);
        child2->getRenderer()->setBackgroundColor(tgui::Color::Red);
        splitContainer->add(child1);
        splitContainer->add(child2);

        splitContainer->getRenderer()->setSplitterColor(tgui::Color::Green);
        splitContainer->getRenderer()->setSplitterColorHover(tgui::Color::Cyan);

        splitContainer->setSplitterWidth(4);
        splitContainer->setSize(150, 150);
        splitContainer->setOrientation(tgui::Orientation::Vertical);
        splitContainer->setMinValidSplitterOffset("20%");
        splitContainer->setMaxValidSplitterOffset(110);
        splitContainer->setSplitterOffset(80);

        testSavingWidget("SplitContainer", splitContainer);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(150, 100, splitContainer)

        splitContainer->setPosition(10, 5);
        splitContainer->setSize(130, 90);
        splitContainer->setSplitterWidth(2);
        splitContainer->setMinValidSplitterOffset("20%");
        splitContainer->setMaxValidSplitterOffset(110);

        auto child1 = tgui::Panel::create();
        auto child2 = tgui::Panel::create();
        child1->getRenderer()->setBackgroundColor(tgui::Color::Yellow);
        child2->getRenderer()->setBackgroundColor(tgui::Color::Green);
        splitContainer->add(child1);
        splitContainer->add(child2);

        tgui::SplitContainerRenderer renderer = tgui::RendererData::create();
        renderer.setSplitterColor(tgui::Color::Blue);
        renderer.setSplitterColorHover(tgui::Color::Red);
        splitContainer->setRenderer(renderer.getData());

        SECTION("Horizontal")
        {
            splitContainer->setOrientation(tgui::Orientation::Horizontal);
            splitContainer->setSplitterOffset(80);

            splitContainer->mouseMoved({80, 40});
            TEST_DRAW("SplitContainer_Horizontal.png")

            splitContainer->mouseMoved({90, 40});
            TEST_DRAW("SplitContainer_Horizontal_SplitterHover.png")
        }

        SECTION("Vertical")
        {
            splitContainer->setOrientation(tgui::Orientation::Vertical);
            splitContainer->setSplitterOffset(40);

            splitContainer->mouseMoved({60, 30});
            TEST_DRAW("SplitContainer_Vertical.png")

            splitContainer->mouseMoved({60, 45});
            TEST_DRAW("SplitContainer_Vertical_SplitterHover.png")
        }
    }
}
