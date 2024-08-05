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

TEST_CASE("[PanelListBox]")
{
    tgui::PanelListBox::Ptr panelListBox = tgui::PanelListBox::create();

    SECTION("Signals")
    {
        panelListBox->onItemSelect([]{});
        panelListBox->onItemSelect([](int){});
        panelListBox->onItemSelect([](const tgui::Panel::Ptr&){});
        panelListBox->onItemSelect([](const tgui::String&){});
        panelListBox->onItemSelect([](int, const tgui::Panel::Ptr&){});
        panelListBox->onItemSelect([](const tgui::Panel::Ptr&, const tgui::String&){});
        panelListBox->onItemSelect([](int, const tgui::Panel::Ptr&, const tgui::String&){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(panelListBox)->getSignal("ItemSelected").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(panelListBox->getWidgetType() == "PanelListBox");
    }

    SECTION("Position and Size")
    {
        panelListBox->setPosition(50, 30);
        panelListBox->setSize(150, 100);

        REQUIRE(panelListBox->getPosition() == tgui::Vector2f(50, 30));
        REQUIRE(panelListBox->getSize() == tgui::Vector2f(150, 100));
        REQUIRE(panelListBox->getFullSize() == panelListBox->getSize());
        REQUIRE(panelListBox->getWidgetOffset() == tgui::Vector2f(0, 0));
    }

    SECTION("Adding items")
    {
        REQUIRE(panelListBox->getItemCount() == 0);
        const auto item1 = panelListBox->addItem("1");
        REQUIRE(panelListBox->getItemCount() == 1);
        const auto item2 = panelListBox->addItem("2");
        const auto item3 = panelListBox->addItem("3");
        REQUIRE(panelListBox->getItemCount() == 3);

        REQUIRE(panelListBox->getItemByIndex(0) == item1);
        REQUIRE(panelListBox->getItemByIndex(1) == item2);
        REQUIRE(panelListBox->getItemByIndex(2) == item3);

        REQUIRE(panelListBox->getItemById("1") == item1);
        REQUIRE(panelListBox->getItemById("2") == item2);
        REQUIRE(panelListBox->getItemById("3") == item3);

        const auto item4 = panelListBox->addItem("4", 1);
        REQUIRE(panelListBox->getItemCount() == 4);
        REQUIRE(panelListBox->getItemByIndex(0) == item1);
        REQUIRE(panelListBox->getItemByIndex(1) == item4);
        REQUIRE(panelListBox->getItemByIndex(2) == item2);
        REQUIRE(panelListBox->getItemByIndex(3) == item3);
    }

    SECTION("Getters")
    {
        const auto item1 = panelListBox->addItem("1");
        const auto item2 = panelListBox->addItem("2");

        REQUIRE(panelListBox->getItems().size() == 2);
        REQUIRE(panelListBox->getItems()[0] == item1);
        REQUIRE(panelListBox->getItems()[1] == item2);

        REQUIRE(panelListBox->getItemByIndex(0) == item1);

        REQUIRE(panelListBox->getItemById("1") == item1);
        REQUIRE(panelListBox->getItemById("3") == nullptr);

        REQUIRE(panelListBox->getItemByIndex(1) == item2);
        REQUIRE(panelListBox->getItemByIndex(2) == nullptr);

        REQUIRE(panelListBox->getIndexById("1") == 0);
        REQUIRE(panelListBox->getIndexById("3") == -1);

        REQUIRE(panelListBox->getIdByIndex(1) == "2");
        REQUIRE(panelListBox->getIdByIndex(2) == "");
    }

    SECTION("Removing items")
    {
        const auto item0 = tgui::Panel::create();
        const auto item1 = panelListBox->addItem("1");
        const auto item2 = panelListBox->addItem("2");
        const auto item3 = panelListBox->addItem("3");
        REQUIRE(panelListBox->getItemCount() == 3);

        REQUIRE(!panelListBox->removeItem(item0));
        REQUIRE(panelListBox->getItemCount() == 3);
        REQUIRE(panelListBox->removeItem(item2));
        REQUIRE(panelListBox->getItemCount() == 2);

        const auto item4 = panelListBox->addItem("4");
        REQUIRE(panelListBox->getItemCount() == 3);
        REQUIRE(panelListBox->getItemByIndex(0) == item1);
        REQUIRE(panelListBox->getItemByIndex(1) == item3);
        REQUIRE(panelListBox->getItemByIndex(2) == item4);

        REQUIRE(!panelListBox->removeItemByIndex(3));
        REQUIRE(panelListBox->getItemCount() == 3);
        REQUIRE(panelListBox->removeItemByIndex(0));
        REQUIRE(panelListBox->getItemCount() == 2);
        REQUIRE(panelListBox->getItemByIndex(0) == item3);
        REQUIRE(panelListBox->getItemByIndex(1) == item4);

        REQUIRE(!panelListBox->removeItemById("2"));
        REQUIRE(panelListBox->getItemCount() == 2);
        REQUIRE(panelListBox->removeItemById("4"));
        REQUIRE(panelListBox->getItemCount() == 1);
        REQUIRE(panelListBox->getItemByIndex(0) == item3);

        const auto item5 = panelListBox->addItem("5");
        const auto item5_second = panelListBox->addItem("5");
        const auto item6 = panelListBox->addItem("6");
        REQUIRE(panelListBox->getItemCount() == 4);
        panelListBox->removeAllItems();
        REQUIRE(panelListBox->getItemCount() == 0);
    }

    SECTION("Changing template")
    {
        const auto item1 = panelListBox->addItem("1");

        const auto panelTemplate = panelListBox->getPanelTemplate();
        const auto button = tgui::Button::create("Some button");
        panelTemplate->add(button, "Template button");

        const auto item2 = panelListBox->addItem("2");

        REQUIRE(item1->getWidgets().empty());
        REQUIRE(item2->getWidgets().size() == 1);
        REQUIRE(item2->getWidgets()[0]->getWidgetName() == "Template button");

    }

    SECTION("Selecting items")
    {
        const auto item0 = tgui::Panel::create();
        const auto item1 = panelListBox->addItem("1");
        const auto item2 = panelListBox->addItem("2");
        const auto item3 = panelListBox->addItem("3");

        REQUIRE(panelListBox->getSelectedItem() == nullptr);
        REQUIRE(panelListBox->getSelectedItemId() == "");
        REQUIRE(panelListBox->getSelectedItemIndex() == -1);

        REQUIRE(!panelListBox->setSelectedItem(item0));
        REQUIRE(panelListBox->getSelectedItemIndex() == -1);
        REQUIRE(panelListBox->setSelectedItem(item1));
        REQUIRE(panelListBox->getSelectedItem() == item1);
        REQUIRE(panelListBox->getSelectedItemId() == "1");
        REQUIRE(panelListBox->getSelectedItemIndex() == 0);

        REQUIRE(!panelListBox->setSelectedItemById("0"));
        REQUIRE(panelListBox->getSelectedItemIndex() == -1);
        REQUIRE(panelListBox->setSelectedItemById("2"));
        REQUIRE(panelListBox->getSelectedItem() == item2);
        REQUIRE(panelListBox->getSelectedItemId() == "2");
        REQUIRE(panelListBox->getSelectedItemIndex() == 1);

        REQUIRE(!panelListBox->setSelectedItemByIndex(3));
        REQUIRE(panelListBox->getSelectedItemIndex() == -1);
        REQUIRE(panelListBox->setSelectedItemByIndex(2));
        REQUIRE(panelListBox->getSelectedItem() == item3);
        REQUIRE(panelListBox->getSelectedItemId() == "3");
        REQUIRE(panelListBox->getSelectedItemIndex() == 2);

        panelListBox->deselectItem();
        REQUIRE(panelListBox->getSelectedItem() == nullptr);
        REQUIRE(panelListBox->getSelectedItemId() == "");
        REQUIRE(panelListBox->getSelectedItemIndex() == -1);
    }

    SECTION("Contains")
    {
        const auto item0 = tgui::Panel::create();

        REQUIRE(!panelListBox->contains(item0));
        REQUIRE(!panelListBox->containsId("1"));

        const auto item1 = panelListBox->addItem("1");
        REQUIRE(panelListBox->contains(item1));
        REQUIRE(panelListBox->containsId("1"));

        const auto item2 = panelListBox->addItem();
        REQUIRE(panelListBox->contains(item2));
        REQUIRE(!panelListBox->containsId("2"));

        panelListBox->removeItem(item1);
        REQUIRE(!panelListBox->contains(item1));
        REQUIRE(!panelListBox->containsId("1"));
    }

    SECTION("ItemsHeight")
    {
        panelListBox->setItemsHeight(20);
        REQUIRE(panelListBox->getItemsHeight().getValue() == 20);
    }

    SECTION("MaximumItems")
    {
        const auto panel1 = panelListBox->addItem("1");
        panelListBox->addItem("2");
        const auto panel3 = panelListBox->addItem("3");
        panelListBox->addItem("4");
        panelListBox->addItem("5");
        panelListBox->setSelectedItemByIndex(4);
        REQUIRE(panelListBox->getItemCount() == 5);
        panelListBox->setMaximumItems(3);
        REQUIRE(panelListBox->getSelectedItemIndex() == -1);
        REQUIRE(panelListBox->getItemCount() == 3);

        panelListBox->addItem("6");
        REQUIRE(panelListBox->getItemCount() == 3);
        REQUIRE(panelListBox->getItemByIndex(0) == panel1);
        REQUIRE(panelListBox->getItemByIndex(2) == panel3);
    }

    SECTION("Scrollbar access")
    {
        panelListBox->setSize(200, 100);
        panelListBox->setItemsHeight(25);
        for (unsigned int i = 0; i < 6; ++i)
            panelListBox->addItem();
        testScrollbarAccess(panelListBox->getVerticalScrollbar());
        REQUIRE(panelListBox->getHorizontalScrollbar()->getPolicy() == tgui::Scrollbar::Policy::Never);
    }

    testWidgetSignals(panelListBox);
    SECTION("Events / Signals")
    {
        auto container = tgui::Group::create({400.f, 300.f});
        container->add(panelListBox);

        auto mouseMoved = [container](const tgui::Vector2f pos){
            container->processMouseMoveEvent(pos);
        };
        auto mousePressed = [container](const tgui::Vector2f pos){
            container->processMousePressEvent(tgui::Event::MouseButton::Left, pos);
        };
        auto mouseReleased = [container](const tgui::Vector2f pos){
            container->processMouseReleaseEvent(tgui::Event::MouseButton::Left, pos);
        };

        panelListBox->setPosition(10, 20);
        panelListBox->setSize(120, 64);
        panelListBox->setItemsHeight(20);
        panelListBox->getRenderer()->setBorders(2);
        panelListBox->getRenderer()->setScrollbarWidth(10);

        panelListBox->addItem("1");
        panelListBox->addItem("2");
        panelListBox->addItem("3");

        unsigned int itemSelectedCount = 0;
        // unsigned int doubleClickedCount = 0;
        panelListBox->onItemSelect(&genericCallback, std::ref(itemSelectedCount));
        // panelListBox->onDoubleClick(&genericCallback, std::ref(doubleClickedCount));

        SECTION("Click on item")
        {
            // Clicking the border does nothing
            mouseMoved({11, 21});
            REQUIRE(panelListBox->getHoveredItemIndex() == -1);
            mousePressed({11, 21});
            mouseReleased({11, 21});
            REQUIRE(itemSelectedCount == 0);
            REQUIRE(panelListBox->getSelectedItemIndex() == -1);

            // Select the first item
            mouseMoved({12, 22});
            REQUIRE(panelListBox->getHoveredItemIndex() == 0);
            mousePressed({12, 22});
            mouseReleased({12, 22});
            REQUIRE(itemSelectedCount == 1);
            REQUIRE(panelListBox->getSelectedItemIndex() == 0);

            // Select the third item
            mousePressed({40, 70});
            mouseReleased({40, 70});
            REQUIRE(itemSelectedCount == 2);
            REQUIRE(panelListBox->getSelectedItemIndex() == 2);

            // SECTION("Double click")
            // {
            //     // If a click happens a long time after the previous click then it isn't a double click
            //     container->updateTime(std::chrono::milliseconds(10000));
            //     mousePressed({40, 70});
            //     mouseReleased({40, 70});
            //     REQUIRE(doubleClickedCount == 0);
            //
            //     container->updateTime(std::chrono::milliseconds(5));
            //     mousePressed({40, 70});
            //     mouseReleased({40, 70});
            //     REQUIRE(doubleClickedCount == 1);
            //     REQUIRE(itemSelectedCount == 2);
            //     REQUIRE(panelListBox->getSelectedItemIndex() == 2);
            //
            //     // Third click is not registered as another double click
            //     mousePressed({40, 70});
            //     mouseReleased({40, 70});
            //     REQUIRE(doubleClickedCount == 1);
            // }
        }

        // SECTION("Arrow keys")
        // {
        //     tgui::Event::KeyEvent downKeyEvent;
        //     downKeyEvent.code = tgui::Event::KeyboardKey::Down;
        //     downKeyEvent.alt = false;
        //     downKeyEvent.control = false;
        //     downKeyEvent.shift = false;
        //     downKeyEvent.system = false;
        //
        //     tgui::Event::KeyEvent upKeyEvent;
        //     upKeyEvent.code = tgui::Event::KeyboardKey::Up;
        //
        //     panelListBox->setSelectedItemByIndex(2);
        //     panelListBox->setAutoScroll(false);
        //     panelListBox->addItem("Item 4");
        //
        //     REQUIRE(panelListBox->getScrollbarValue() == 0);
        //     panelListBox->keyPressed(downKeyEvent);
        //     REQUIRE(panelListBox->getScrollbarValue() > 0);
        //     REQUIRE(panelListBox->getSelectedItemIndex() == 3);
        //
        //     panelListBox->keyPressed(upKeyEvent);
        //     panelListBox->keyPressed(upKeyEvent);
        //     REQUIRE(panelListBox->getScrollbarValue() > 0);
        //     REQUIRE(panelListBox->getSelectedItemIndex() == 1);
        //
        //     panelListBox->keyPressed(upKeyEvent);
        //     REQUIRE(panelListBox->getScrollbarValue() == 0);
        //     REQUIRE(panelListBox->getSelectedItemIndex() == 0);
        //
        //     // Arrow up while already at the top doesn't change anything
        //     panelListBox->keyPressed(upKeyEvent);
        //     REQUIRE(panelListBox->getSelectedItemIndex() == 0);
        // }
    }

    testWidgetRenderer(panelListBox->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = panelListBox->getRenderer();

        SECTION("colored")
        {
            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("ItemsBackgroundColor", "rgb(20, 30, 40)"));
                REQUIRE_NOTHROW(renderer->setProperty("ItemsBackgroundColorHover", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedItemsBackgroundColor", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedItemsBackgroundColorHover", "rgb(40, 50, 60)"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("ItemsBackgroundColor", tgui::Color{20, 30, 40}));
                REQUIRE_NOTHROW(renderer->setProperty("ItemsBackgroundColorHover", tgui::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedItemsBackgroundColor", tgui::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedItemsBackgroundColorHover", tgui::Color{40, 50, 60}));
            }

            SECTION("functions")
            {
                renderer->setItemsBackgroundColor({20, 30, 40});
                renderer->setItemsBackgroundColorHover({50, 60, 70});
                renderer->setSelectedItemsBackgroundColor({30, 40, 50});
                renderer->setSelectedItemsBackgroundColorHover({40, 50, 60});
            }

            REQUIRE(renderer->getProperty("ItemsBackgroundColor").getColor() == tgui::Color(20, 30, 40));
            REQUIRE(renderer->getProperty("ItemsBackgroundColorHover").getColor() == tgui::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("SelectedItemsBackgroundColor").getColor() == tgui::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("SelectedItemsBackgroundColorHover").getColor() == tgui::Color(40, 50, 60));
        }
    }

    SECTION("Saving and loading from file")
    {
        panelListBox->setSize(200, 100);
        panelListBox->addItem("1");
        panelListBox->addItem("2");
        panelListBox->addItem("3");
        panelListBox->setItemsHeight(25);
        panelListBox->setMaximumItems(5);
        panelListBox->setSelectedItemById("2");
        panelListBox->setTextSize(20);
        panelListBox->setMaximumItems(5);

        testSavingWidget("PanelListBox", panelListBox, false);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(80, 70, panelListBox)

        panelListBox->setEnabled(true);
        panelListBox->setPosition(10, 5);
        panelListBox->setSize(60, 60);
        panelListBox->setTextSize(14);
        panelListBox->setItemsHeight(18);

        tgui::PanelListBoxRenderer renderer = tgui::RendererData::create();
        renderer.setBackgroundColor(tgui::Color::Green);
        renderer.setSelectedItemsBackgroundColor(tgui::Color::Red);
        renderer.setItemsBackgroundColor(tgui::Color::Yellow);
        renderer.setBorderColor(tgui::Color::Blue);
        renderer.setBorders({1, 2, 3, 4});
        renderer.setPadding({4, 3, 2, 1});
        renderer.setScrollbarWidth(14);
        renderer.setOpacity(0.7f);
        panelListBox->setRenderer(renderer.getData());

        auto setHoverRenderer = [&]{
                                    renderer.setItemsBackgroundColorHover(tgui::Color::Magenta);
                                    renderer.setSelectedItemsBackgroundColorHover("#808080");
                                 };

        panelListBox->addItem("1");
        panelListBox->addItem("2");
        panelListBox->addItem("3");
        panelListBox->addItem("4");
        panelListBox->addItem("5");
        panelListBox->addItem("6");

        constexpr tgui::Vector2f mousePos1{30, 20};
        constexpr tgui::Vector2f mousePos2{30, 35};

        SECTION("Colored")
        {
            SECTION("No selected item")
            {
                SECTION("No hover")
                {
                    TEST_DRAW("PanelListBox_NoSelectedNoHover.png")
                }

                SECTION("Hover")
                {
                    panelListBox->mouseMoved(mousePos2);

                    SECTION("No hover properties set")
                    {
                        TEST_DRAW("PanelListBox_NoSelectedHover_NoHoverSet.png")
                    }
                    SECTION("Hover properties set")
                    {
                        setHoverRenderer();
                        TEST_DRAW("PanelListBox_NoSelectedHover_HoverSet.png")
                    }
                }
            }

            SECTION("Selected item")
            {
                panelListBox->setSelectedItemByIndex(0);

                SECTION("No hover")
                {
                    TEST_DRAW("PanelListBox_SelectedNoHover.png")
                }

                SECTION("Hover selected")
                {
                    panelListBox->mouseMoved(mousePos1);

                    SECTION("No hover properties set")
                    {
                        TEST_DRAW("PanelListBox_SelectedHoverSelected_NoHoverSet.png")
                    }
                    SECTION("Hover properties set")
                    {
                        setHoverRenderer();
                        TEST_DRAW("PanelListBox_SelectedHoverSelected_HoverSet.png")
                    }
                }

                SECTION("Hover other")
                {
                    panelListBox->mouseMoved(mousePos2);

                    SECTION("No hover properties set")
                    {
                        TEST_DRAW("PanelListBox_SelectedHoverOther_NoHoverSet.png")
                    }
                    SECTION("Hover properties set")
                    {
                        setHoverRenderer();
                        TEST_DRAW("PanelListBox_SelectedHoverOther_HoverSet.png")
                    }
                }
            }
        }
    }
}
