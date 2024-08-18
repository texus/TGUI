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

TEST_CASE("[TreeView]")
{
    auto treeView = tgui::TreeView::create();
    treeView->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        treeView->onItemSelect([](){});
        treeView->onItemSelect([](const tgui::String&){});
        treeView->onItemSelect([](const std::vector<tgui::String>&){});

        treeView->onDoubleClick([](){});
        treeView->onDoubleClick([](const tgui::String&){});
        treeView->onDoubleClick([](const std::vector<tgui::String>&){});

        treeView->onExpand([](){});
        treeView->onExpand([](const tgui::String&){});
        treeView->onExpand([](const std::vector<tgui::String>&){});

        treeView->onCollapse([](){});
        treeView->onCollapse([](const tgui::String&){});
        treeView->onCollapse([](const std::vector<tgui::String>&){});

        treeView->onRightClick([](){});
        treeView->onRightClick([](const tgui::String&){});
        treeView->onRightClick([](const std::vector<tgui::String>&){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(treeView)->getSignal("ItemSelected").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(treeView)->getSignal("DoubleClicked").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(treeView)->getSignal("Expanded").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(treeView)->getSignal("Collapsed").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(treeView)->getSignal("RightClicked").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(treeView->getWidgetType() == "TreeView");
    }

    SECTION("Position and Size")
    {
        treeView->setPosition(40, 30);
        treeView->setSize(150, 100);
        treeView->getRenderer()->setBorders(2);

        REQUIRE(treeView->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(treeView->getSize() == tgui::Vector2f(150, 100));
        REQUIRE(treeView->getFullSize() == treeView->getSize());
        REQUIRE(treeView->getWidgetOffset() == tgui::Vector2f(0, 0));
    }

    SECTION("Adding and removing items")
    {
        SECTION("No depth")
        {
            REQUIRE(treeView->getNodes().empty());
            REQUIRE(treeView->addItem({"One"}));
            REQUIRE(treeView->getNodes().size() == 1);
            REQUIRE(treeView->getNodes()[0].text == "One");
            REQUIRE(treeView->getNodes()[0].nodes.empty());

            REQUIRE(treeView->addItem({"Two"}, false));
            REQUIRE(treeView->getNodes().size() == 2);

            REQUIRE(treeView->changeItem({"Two"}, "_2_"));
            REQUIRE(treeView->getNodes().size() == 2);
            REQUIRE(treeView->getNodes()[1].text == "_2_");

            treeView->removeItem({"One"});
            REQUIRE(treeView->getNodes().size() == 1);
        }

        SECTION("Multiple items")
        {
            REQUIRE(treeView->addItem({"Smilies", "Happy"}));
            REQUIRE(treeView->addItem({"Smilies", "Sad"}));
            REQUIRE(treeView->addItem({"Smilies", "Neither"}));
            REQUIRE(treeView->addItem({"Vehicles", "Parts", "Wheel"}));
            REQUIRE(treeView->addItem({"Vehicles", "Whole", "Truck"}));
            REQUIRE(treeView->addItem({"Vehicles", "Whole", "Car"}));

            SECTION("Verify that nodes were added")
            {
                REQUIRE(treeView->getNodes().size() == 2);
                REQUIRE(treeView->getNodes()[0].text == "Smilies");
                REQUIRE(treeView->getNodes()[0].nodes.size() == 3);
                REQUIRE(treeView->getNodes()[0].nodes[0].text == "Happy");
                REQUIRE(treeView->getNodes()[0].nodes[1].text == "Sad");
                REQUIRE(treeView->getNodes()[0].nodes[2].text == "Neither");
                REQUIRE(treeView->getNodes()[1].text == "Vehicles");
                REQUIRE(treeView->getNodes()[1].nodes.size() == 2);
                REQUIRE(treeView->getNodes()[1].nodes[0].text == "Parts");
                REQUIRE(treeView->getNodes()[1].nodes[0].nodes.size() == 1);
                REQUIRE(treeView->getNodes()[1].nodes[0].nodes[0].text == "Wheel");
                REQUIRE(treeView->getNodes()[1].nodes[1].text == "Whole");
                REQUIRE(treeView->getNodes()[1].nodes[1].nodes.size() == 2);
                REQUIRE(treeView->getNodes()[1].nodes[1].nodes[0].text == "Truck");
                REQUIRE(treeView->getNodes()[1].nodes[1].nodes[1].text == "Car");

                REQUIRE(treeView->getNode({"Smilies"}).text == "Smilies");
                REQUIRE(treeView->getNode({"Smilies"}).nodes.size() == 3);
                REQUIRE(treeView->getNode({"Smilies", "Neither"}).text == "Neither");
                REQUIRE(treeView->getNode({"Smilies", "Neither"}).nodes.empty());
                REQUIRE(treeView->getNode({"Vehicles", "Whole"}).text == "Whole");
                REQUIRE(treeView->getNode({"Vehicles", "Whole"}).nodes.size() == 2);
                REQUIRE(treeView->getNode({"Vehicles", "Whole"}).nodes[0].text == "Truck");
                REQUIRE(treeView->getNode({"Vehicles", "Whole"}).nodes[1].text == "Car");

                // Testing invalid getNode calls
                REQUIRE(treeView->getNode({}).text == "");
                REQUIRE(treeView->getNode({}).nodes.empty());
                REQUIRE(treeView->getNode({"Vehicles", "Train"}).text == "");
                REQUIRE(treeView->getNode({"Vehicles", "Train"}).nodes.empty());
            }

            SECTION("Changing items")
            {
                REQUIRE(!treeView->changeItem({}, "test"));
                REQUIRE(!treeView->changeItem({"Does", "not", "exist"}, "test"));
                REQUIRE(!treeView->changeItem({"Vehicles", "Whole", "something else"}, "test"));

                REQUIRE(treeView->changeItem({"Vehicles", "Whole", "Car"}, "Train"));
                REQUIRE(treeView->getNodes().size() == 2);
                REQUIRE(treeView->getNodes()[1].nodes[1].nodes.size() == 2);
                REQUIRE(treeView->getNodes()[1].nodes[1].nodes[0].text == "Truck");
                REQUIRE(treeView->getNodes()[1].nodes[1].nodes[1].text == "Train");

                // Once changed, the old item won't be found anymore
                REQUIRE(!treeView->changeItem({"Vehicles", "Whole", "Car"}, "Airplane"));
                REQUIRE(treeView->getNodes()[1].nodes[1].nodes[1].text == "Train");

                // The renamed item can be found now
                REQUIRE(treeView->changeItem({"Vehicles", "Whole", "Train"}, "Airplane"));
                REQUIRE(treeView->getNodes()[1].nodes[1].nodes[1].text == "Airplane");
            }

            SECTION("Removing items")
            {
                treeView->removeItem({"Vehicles", "Parts", "Wheel"});
                treeView->removeItem({"Smilies", "Sad"});
                treeView->removeItem({"Vehicles", "Whole", "Car"});

                REQUIRE(treeView->getNodes().size() == 2);
                REQUIRE(treeView->getNodes()[0].text == "Smilies");
                REQUIRE(treeView->getNodes()[0].nodes.size() == 2);
                REQUIRE(treeView->getNodes()[0].nodes[0].text == "Happy");
                REQUIRE(treeView->getNodes()[0].nodes[1].text == "Neither");
                REQUIRE(treeView->getNodes()[1].text == "Vehicles");
                REQUIRE(treeView->getNodes()[1].nodes.size() == 1);
                REQUIRE(treeView->getNodes()[1].nodes[0].text == "Whole");
                REQUIRE(treeView->getNodes()[1].nodes[0].nodes.size() == 1);
                REQUIRE(treeView->getNodes()[1].nodes[0].nodes[0].text == "Truck");

                SECTION("Keep parent after delete")
                {
                    treeView->removeItem({"Vehicles", "Whole", "Truck"}, false);
                    REQUIRE(treeView->getNodes()[1].nodes[0].text == "Whole");
                    REQUIRE(treeView->getNodes()[1].nodes[0].nodes.size() == 0);
                }
            }

            SECTION("Removing all items")
            {
                treeView->removeAllItems();
                REQUIRE(treeView->getNodes().empty());
            }
        }

        SECTION("Invalid calls")
        {
            treeView->addItem({"One"});
            REQUIRE(!treeView->addItem({"Two", "Sub"}, false));
            REQUIRE(!treeView->addItem(std::vector<tgui::String>()));
            REQUIRE(!treeView->removeItem({"One", "Sub"}));
        }
    }

    SECTION("Collapsing and expanding items")
    {
        treeView->addItem({"Smilies", "Neither"});
        treeView->addItem({"Vehicles", "Parts", "Wheel"});

        // Nodes are expanded by default
        REQUIRE(treeView->getNodes()[0].expanded);
        REQUIRE(treeView->getNodes()[1].expanded);
        REQUIRE(treeView->getNodes()[1].nodes[0].expanded);

        treeView->collapse({"Vehicles"});
        REQUIRE(!treeView->getNodes()[1].expanded);
        REQUIRE(treeView->getNodes()[1].nodes[0].expanded); // collapse doesn't affect state of child nodes

        treeView->collapse({"Vehicles", "Parts"});
        REQUIRE(!treeView->getNodes()[1].nodes[0].expanded);

        treeView->expand({"Vehicles"});
        REQUIRE(treeView->getNodes()[1].expanded);
        REQUIRE(!treeView->getNodes()[1].nodes[0].expanded); // expand doesn't affect state of child nodes

        treeView->collapseAll();
        REQUIRE(!treeView->getNodes()[0].expanded);
        REQUIRE(!treeView->getNodes()[1].expanded);
        REQUIRE(!treeView->getNodes()[1].nodes[0].expanded);

        treeView->expandAll();
        REQUIRE(treeView->getNodes()[0].expanded);
        REQUIRE(treeView->getNodes()[1].expanded);
        REQUIRE(treeView->getNodes()[1].nodes[0].expanded);

        // Expanding child will expand all parents
        treeView->collapseAll();
        treeView->expand({"Vehicles", "Parts", "Wheel"});
        REQUIRE(!treeView->getNodes()[0].expanded);
        REQUIRE(treeView->getNodes()[1].expanded);
        REQUIRE(treeView->getNodes()[1].nodes[0].expanded);
        REQUIRE(treeView->getNodes()[1].nodes[0].nodes[0].expanded);
    }

    SECTION("Selecting items")
    {
        treeView->addItem({"Smilies", "Neither"});
        treeView->addItem({"Vehicles", "Parts", "Wheel"});

        REQUIRE(treeView->getSelectedItem().empty());

        REQUIRE(treeView->selectItem({"Vehicles"}));
        REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Vehicles"});

        REQUIRE(!treeView->selectItem({"Unexisting"}));
        REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Vehicles"});

        REQUIRE(treeView->selectItem({"Smilies", "Neither"}));
        REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Smilies", "Neither"});

        REQUIRE(!treeView->selectItem({"Vehicles", "Parts", "Unexisting"}));
        REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Smilies", "Neither"});

        treeView->collapseAll();
        REQUIRE(treeView->selectItem({"Vehicles", "Parts", "Wheel"}));
        REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Vehicles", "Parts", "Wheel"});
        REQUIRE(treeView->selectItem({"Vehicles", "Parts", "Wheel"}));

        treeView->deselectItem();
        REQUIRE(treeView->getSelectedItem().empty());

        REQUIRE(!treeView->selectItem({"Smilies", "NonExistent"}));
        REQUIRE(!treeView->selectItem({"NonExistent"}));
        REQUIRE(!treeView->selectItem({}));
        REQUIRE(treeView->getSelectedItem().empty());
    }

    SECTION("ItemHeight")
    {
        treeView->setItemHeight(30);
        REQUIRE(treeView->getItemHeight() == 30);
    }

    SECTION("TextSize")
    {
        treeView->setTextSize(25);
        REQUIRE(treeView->getTextSize() == 25);
    }

    SECTION("ScrollbarValue + ScrollbarMaxValue")
    {
        REQUIRE(treeView->getVerticalScrollbarValue() == 0);
        treeView->setVerticalScrollbarValue(100);
        REQUIRE(treeView->getVerticalScrollbarValue() == 0);

        REQUIRE(treeView->getHorizontalScrollbarValue() == 0);
        treeView->setHorizontalScrollbarValue(100);
        REQUIRE(treeView->getHorizontalScrollbarValue() == 0);

        REQUIRE(treeView->getVerticalScrollbarMaxValue() == 0);
        REQUIRE(treeView->getHorizontalScrollbarMaxValue() == 0);

        treeView->setSize(70, 45);
        treeView->setItemHeight(20);
        treeView->addItem({"Smilies", "Happy"});
        treeView->addItem({"Smilies", "Sad"});
        treeView->addItem({"Smilies", "Neither"});

        REQUIRE(treeView->getVerticalScrollbarMaxValue() > 37);
        REQUIRE(treeView->getHorizontalScrollbarMaxValue() > 0);

        treeView->setVerticalScrollbarValue(10);
        REQUIRE(treeView->getVerticalScrollbarValue() == 10);

        treeView->setHorizontalScrollbarValue(15);
        REQUIRE(treeView->getHorizontalScrollbarValue() == 15);
    }

    SECTION("Scrollbar access")
    {
        treeView->setSize(70, 70);
        treeView->setItemHeight(20);
        treeView->addItem({"Smilies", "Happy"});
        treeView->addItem({"Smilies", "Sad"});
        treeView->addItem({"Smilies", "Neither"});
        testScrollbarAccess(treeView->getVerticalScrollbar());
        testScrollbarAccess(treeView->getHorizontalScrollbar());
    }

    SECTION("ItemIndexInParent")
    {
        treeView->addItem({"Smilies", "Happy"});
        treeView->addItem({"Smilies", "Sad"});
        treeView->addItem({"Smilies", "Neither"});
        treeView->addItem({"Vehicles", "Parts", "Wheel"});
        treeView->addItem({"Vehicles", "Whole", "Truck"});
        treeView->addItem({"Vehicles", "Whole", "Car"});
        treeView->addItem({"Vehicles", "Whole", "Train"});
        treeView->collapseAll();

        REQUIRE(treeView->getItemIndexInParent({"Smilies"}) == 0);
        REQUIRE(treeView->getItemIndexInParent({"Vehicles"}) == 1);

        REQUIRE(treeView->getItemIndexInParent({"Smilies", "Happy"}) == 0);
        REQUIRE(treeView->getItemIndexInParent({"Smilies", "Sad"}) == 1);
        REQUIRE(treeView->getItemIndexInParent({"Vehicles", "Parts"}) == 0);

        REQUIRE(treeView->getItemIndexInParent({"Vehicles", "Parts", "Wheel"}) == 0);
        REQUIRE(treeView->getItemIndexInParent({"Vehicles", "Whole", "Truck"}) == 0);
        REQUIRE(treeView->getItemIndexInParent({"Vehicles", "Whole", "Train"}) == 2);

        REQUIRE(treeView->getItemIndexInParent({"Vehicles", "Parts", "Wheel", "NonExistent"}) == -1);
        REQUIRE(treeView->getItemIndexInParent({"NonExistent", "Happy"}) == -1);
        REQUIRE(treeView->getItemIndexInParent({"NonExistent"}) == -1);
        REQUIRE(treeView->getItemIndexInParent({}) == -1);

        REQUIRE(!treeView->setItemIndexInParent({}, 0));
        REQUIRE(!treeView->setItemIndexInParent({"NonExistent"}, 0));
        REQUIRE(!treeView->setItemIndexInParent({"NonExistent", "Happy"}, 0));
        REQUIRE(!treeView->setItemIndexInParent({"Vehicles", "Parts", "Wheel", "NonExistent"}, 0));

        REQUIRE(treeView->getNodes()[0].text == "Smilies");
        REQUIRE(treeView->getNodes()[1].text == "Vehicles");
        REQUIRE(treeView->getNodes()[0].nodes[1].text == "Sad");
        REQUIRE(treeView->getNodes()[1].nodes[1].text == "Whole");

        REQUIRE(treeView->setItemIndexInParent({"Smilies"}, 1));
        REQUIRE(treeView->getNodes()[0].text == "Vehicles");
        REQUIRE(treeView->getNodes()[1].text == "Smilies");
        REQUIRE(treeView->getNodes()[1].nodes[1].text == "Sad");

        REQUIRE(treeView->getNodes()[0].nodes[1].nodes[0].text == "Truck");
        REQUIRE(treeView->getNodes()[0].nodes[1].nodes[1].text == "Car");
        REQUIRE(treeView->getNodes()[0].nodes[1].nodes[2].text == "Train");

        REQUIRE(treeView->setItemIndexInParent({"Vehicles", "Whole", "Car"}, 0));
        REQUIRE(treeView->getNodes()[0].nodes[1].nodes[0].text == "Car");
        REQUIRE(treeView->getNodes()[0].nodes[1].nodes[1].text == "Truck");
        REQUIRE(treeView->getNodes()[0].nodes[1].nodes[2].text == "Train");

        REQUIRE(treeView->setItemIndexInParent({"Vehicles", "Whole", "Truck"}, 2));
        REQUIRE(treeView->getNodes()[0].nodes[1].nodes[0].text == "Car");
        REQUIRE(treeView->getNodes()[0].nodes[1].nodes[1].text == "Train");
        REQUIRE(treeView->getNodes()[0].nodes[1].nodes[2].text == "Truck");

        REQUIRE(treeView->setItemIndexInParent({"Vehicles", "Whole", "Train"}, 1));
        REQUIRE(treeView->getNodes()[0].nodes[1].nodes[0].text == "Car");
        REQUIRE(treeView->getNodes()[0].nodes[1].nodes[1].text == "Train");
        REQUIRE(treeView->getNodes()[0].nodes[1].nodes[2].text == "Truck");

        REQUIRE(treeView->setItemIndexInParent({"Vehicles", "Whole", "Truck"}, 25));
        REQUIRE(treeView->getNodes()[0].nodes[1].nodes[0].text == "Car");
        REQUIRE(treeView->getNodes()[0].nodes[1].nodes[1].text == "Train");
        REQUIRE(treeView->getNodes()[0].nodes[1].nodes[2].text == "Truck");

        REQUIRE(treeView->setItemIndexInParent({"Vehicles", "Whole", "Car"}, 25));
        REQUIRE(treeView->getNodes()[0].nodes[1].nodes[0].text == "Train");
        REQUIRE(treeView->getNodes()[0].nodes[1].nodes[1].text == "Truck");
        REQUIRE(treeView->getNodes()[0].nodes[1].nodes[2].text == "Car");
        REQUIRE(treeView->getItemIndexInParent({"Vehicles", "Whole", "Train"}) == 0);
        REQUIRE(treeView->getItemIndexInParent({"Vehicles", "Whole", "Truck"}) == 1);
        REQUIRE(treeView->getItemIndexInParent({"Vehicles", "Whole", "Car"}) == 2);
    }

    SECTION("changeItemHierarchy")
    {
        treeView->addItem({"Smilies", "Happy"});
        treeView->addItem({"Smilies", "Sad"});
        treeView->addItem({"Smilies", "Neither"});
        treeView->addItem({"Vehicles", "Parts", "Wheel"});
        treeView->addItem({"Vehicles", "Whole", "Truck"});
        treeView->addItem({"Vehicles", "Whole", "Car"});
        treeView->addItem({"Vehicles", "Whole", "Train"});

        REQUIRE(!treeView->changeItemHierarchy({"Smilies", "Sad"}, {}));
        REQUIRE(!treeView->changeItemHierarchy({"Smilies", "Sad"}, {"NonExistent", "New"}));
        REQUIRE(!treeView->changeItemHierarchy({"Smilies", "Sad"}, {"Vehicles", "NonExistent", "New"}));
        REQUIRE(!treeView->changeItemHierarchy({"Smilies", "NonExistent"}, {"New"}));
        REQUIRE(!treeView->changeItemHierarchy({"NonExistent"}, {"New"}));
        REQUIRE(!treeView->changeItemHierarchy({}, {"New"}));

        treeView->changeItemHierarchy({"Vehicles", "Whole", "Train"}, {"Vehicles", "Whole", "Train"});
        REQUIRE(treeView->getNodes()[1].nodes[1].nodes[0].text == "Truck");
        REQUIRE(treeView->getNodes()[1].nodes[1].nodes[1].text == "Car");
        REQUIRE(treeView->getNodes()[1].nodes[1].nodes[2].text == "Train");

        treeView->changeItemHierarchy({"Vehicles", "Whole", "Truck"}, {"Vehicles", "Whole", "Truck"});
        REQUIRE(treeView->getNodes()[1].nodes[1].nodes[0].text == "Car");
        REQUIRE(treeView->getNodes()[1].nodes[1].nodes[1].text == "Train");
        REQUIRE(treeView->getNodes()[1].nodes[1].nodes[2].text == "Truck");

        treeView->changeItemHierarchy({"Vehicles", "Whole", "Train"}, {"Vehicles", "Whole", "Bike"});
        REQUIRE(treeView->getNodes()[1].nodes[1].nodes[0].text == "Car");
        REQUIRE(treeView->getNodes()[1].nodes[1].nodes[1].text == "Truck");
        REQUIRE(treeView->getNodes()[1].nodes[1].nodes[2].text == "Bike");

        treeView->changeItemHierarchy({"Vehicles", "Whole", "Car"}, {"Smilies", "MovedCar"});
        REQUIRE(treeView->getNodes()[0].nodes[2].text == "Neither");
        REQUIRE(treeView->getNodes()[0].nodes[3].text == "MovedCar");
        REQUIRE(treeView->getNodes()[1].nodes[1].nodes[0].text == "Truck");
        REQUIRE(treeView->getNodes()[1].nodes[1].nodes[1].text == "Bike");

        treeView->changeItemHierarchy({"Vehicles", "Whole"}, {"MovedWhole"});
        REQUIRE(treeView->getNodes()[1].text == "Vehicles");
        REQUIRE(treeView->getNodes()[2].text == "MovedWhole");
        REQUIRE(treeView->getNodes()[2].nodes[0].text == "Truck");
        REQUIRE(treeView->getNodes()[2].nodes[1].text == "Bike");
        REQUIRE(treeView->getNodes()[1].nodes[0].text == "Parts");
        REQUIRE(treeView->getNodes()[1].nodes.size() == 1);

        treeView->changeItemHierarchy({"Smilies"}, {"Vehicles", "Parts", "MovedSmilies"});
        REQUIRE(treeView->getNodes().size() == 2);
        REQUIRE(treeView->getNodes()[0].text == "Vehicles");
        REQUIRE(treeView->getNodes()[1].text == "MovedWhole");
        REQUIRE(treeView->getNodes()[0].nodes.size() == 1);
        REQUIRE(treeView->getNodes()[0].nodes[0].text == "Parts");
        REQUIRE(treeView->getNodes()[0].nodes[0].nodes.size() == 2);
        REQUIRE(treeView->getNodes()[0].nodes[0].nodes[0].text == "Wheel");
        REQUIRE(treeView->getNodes()[0].nodes[0].nodes[1].text == "MovedSmilies");
        REQUIRE(treeView->getNodes()[0].nodes[0].nodes[1].nodes.size() == 4);
        REQUIRE(treeView->getNodes()[0].nodes[0].nodes[1].nodes[0].text == "Happy");
        REQUIRE(treeView->getNodes()[0].nodes[0].nodes[1].nodes[1].text == "Sad");
        REQUIRE(treeView->getNodes()[0].nodes[0].nodes[1].nodes[2].text == "Neither");
        REQUIRE(treeView->getNodes()[0].nodes[0].nodes[1].nodes[3].text == "MovedCar");
    }

    testWidgetSignals(treeView);
    SECTION("Events / Signals")
    {
        auto root = std::make_shared<tgui::RootContainer>();
        root->setSize(800, 600);

        auto container = tgui::Group::create({400.f, 300.f});
        container->setPosition(100, 50);
        container->add(treeView);
        root->add(container);

        auto mouseMoved = [container](tgui::Vector2f pos){
            container->processMouseMoveEvent(pos);
        };
        auto mousePressed = [container](tgui::Vector2f pos, tgui::Event::MouseButton button = tgui::Event::MouseButton::Left){
            container->processMousePressEvent(button, pos);
        };
        auto mouseReleased = [container](tgui::Vector2f pos, tgui::Event::MouseButton button = tgui::Event::MouseButton::Left){
            container->processMouseReleaseEvent(button, pos);
        };

        treeView->setPosition(10, 20);
        treeView->setSize(120, 64);
        treeView->setItemHeight(20);
        treeView->getRenderer()->setBorders(2);
        treeView->getRenderer()->setScrollbarWidth(10);

        treeView->addItem({"Smilies", "Happy"});
        treeView->addItem({"Smilies", "Sad"});
        treeView->addItem({"Smilies", "Neither"});

        unsigned int itemSelectedCount = 0;
        unsigned int doubleClickedCount = 0;
        unsigned int expandedCount = 0;
        unsigned int collapsedCount = 0;
        treeView->onItemSelect(&genericCallback, std::ref(itemSelectedCount));
        treeView->onDoubleClick(&genericCallback, std::ref(doubleClickedCount));
        treeView->onExpand(&genericCallback, std::ref(expandedCount));
        treeView->onCollapse(&genericCallback, std::ref(collapsedCount));

        SECTION("Click on item")
        {
            // Clicking the border does nothing
            mouseMoved({11, 21});
            REQUIRE(treeView->getHoveredItem().empty());
            mousePressed({11, 21});
            mouseReleased({11, 21});
            REQUIRE(itemSelectedCount == 0);
            REQUIRE(treeView->getSelectedItem().empty());

            // Select the first row
            mouseMoved({12, 22});
            REQUIRE(treeView->getHoveredItem() == std::vector<tgui::String>{"Smilies"});
            mousePressed({12, 22});
            mouseReleased({12, 22});
            REQUIRE(itemSelectedCount == 1);
            REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Smilies"});

            // Select the third row
            mousePressed({60, 70});
            mouseReleased({60, 70});
            REQUIRE(itemSelectedCount == 2);
            REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Smilies", "Sad"});

            SECTION("Double click")
            {
                // If a click happens a long time after the previous click then it isn't a double click
                container->updateTime(std::chrono::milliseconds(10000));
                mousePressed({60, 70});
                mouseReleased({60, 70});
                REQUIRE(doubleClickedCount == 0);

                container->updateTime(std::chrono::milliseconds(5));
                mousePressed({60, 70});
                mouseReleased({60, 70});
                REQUIRE(doubleClickedCount == 1);
                REQUIRE(itemSelectedCount == 2);
                REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Smilies", "Sad"});

                // Third click is not registered as another double click
                mousePressed({60, 70});
                mouseReleased({60, 70});
                REQUIRE(doubleClickedCount == 1);

                // Double clicking on an open node will collapse it
                container->updateTime(std::chrono::milliseconds(2000));
                REQUIRE(collapsedCount == 0);
                mousePressed({60, 30});
                mouseReleased({60, 30});
                mousePressed({60, 30});
                mouseReleased({60, 30});
                REQUIRE(collapsedCount == 1);
                REQUIRE(doubleClickedCount == 1);

                // Double clicking on a closed node will expand it
                container->updateTime(std::chrono::milliseconds(2000));
                REQUIRE(expandedCount == 0);
                mousePressed({60, 30});
                mouseReleased({60, 30});
                mousePressed({60, 30});
                mouseReleased({60, 30});
                REQUIRE(expandedCount == 1);
                REQUIRE(collapsedCount == 1);
                REQUIRE(doubleClickedCount == 1);
            }
        }

        SECTION("Click on collapse/expand icon")
        {
            mousePressed({20, 30});
            mouseReleased({20, 30});
            REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Smilies"});
            REQUIRE(collapsedCount == 1);
            REQUIRE(expandedCount == 0);

            mousePressed({20, 30});
            mouseReleased({20, 30});
            REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Smilies"});
            REQUIRE(expandedCount == 1);
            REQUIRE(collapsedCount == 1);
        }

        SECTION("Right click")
        {
            unsigned int rightClickCount = 0;
            treeView->onRightClick(&genericCallback, std::ref(rightClickCount));

            mousePressed({60, 70}, tgui::Event::MouseButton::Right);
            mouseReleased({60, 70}, tgui::Event::MouseButton::Right);
            REQUIRE(rightClickCount == 1);
            REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Smilies", "Sad"});
        }

        SECTION("Vertical scrollbar interaction")
        {
            // There is no scrollbar yet
            treeView->removeItem({"Smilies", "Neither"});
            mousePressed({127, 81});
            mouseReleased({127, 81});
            REQUIRE(itemSelectedCount == 1);
            REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Smilies", "Sad"});

            treeView->addItem({"Smilies", "Neither"});

            // Clicking on scrollbar arrow down
            mousePressed({127, 81});
            mouseReleased({127, 81});

            // Dragging scrollbar thumb
            mouseMoved({125, 50});
            mousePressed({125, 50});
            mouseMoved({125, 45});
            mouseReleased({125, 45});
            REQUIRE(itemSelectedCount == 1);
            REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Smilies", "Sad"});

            // Select the second and then third row
            mouseMoved({40, 51});
            mousePressed({40, 51});
            mouseReleased({40, 51});
            REQUIRE(itemSelectedCount == 2);
            REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Smilies", "Happy"});
            mouseMoved({40, 52});
            mousePressed({40, 52});
            mouseReleased({40, 52});
            REQUIRE(itemSelectedCount == 3);
            REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Smilies", "Sad"});

            treeView->addItem({"Vehicles", "Parts", "Wheel"});
            treeView->addItem({"Vehicles", "Whole", "Truck"});
            treeView->setVerticalScrollbarValue(0);

            // Scrolling down with mouse wheel
            root->scrolled(-1, {40 + container->getPosition().x, 70 + container->getPosition().y}, false);
            root->scrolled(-1, {40 + container->getPosition().x, 70 + container->getPosition().y}, false);

            mousePressed({40, 52});
            REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Smilies", "Neither"});
            mouseReleased({40, 52});
        }

        SECTION("Horizontal scrollbar interaction")
        {
            mousePressed({70, 80});
            mouseReleased({70, 80});
            REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Smilies", "Sad"});

            treeView->deselectItem();
            treeView->addItem({"Item too long to fit"});

            mousePressed({70, 80});
            mouseReleased({70, 80});
            REQUIRE(treeView->getSelectedItem().empty());
        }

        SECTION("Changing selection with arrow keys")
        {
            treeView->removeItem({"Smilies", "Neither"});
            mousePressed({127, 81});
            mouseReleased({127, 81});
            REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Smilies", "Sad"});

            tgui::Event::KeyEvent event;
            event.alt = false;
            event.shift = false;
            event.control = false;
            event.system = false;

            event.code = tgui::Event::KeyboardKey::Up;
            treeView->keyPressed(event);
            REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Smilies", "Happy"});

            event.code = tgui::Event::KeyboardKey::Down;
            treeView->keyPressed(event);
            REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Smilies", "Sad"});
            treeView->keyPressed(event);
            REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Smilies", "Sad"});

            event.code = tgui::Event::KeyboardKey::Up;
            treeView->keyPressed(event);
            treeView->keyPressed(event);
            REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Smilies"});
            treeView->keyPressed(event);
            REQUIRE(treeView->getSelectedItem() == std::vector<tgui::String>{"Smilies"});
            REQUIRE(treeView->getNodes()[0].expanded);

            event.code = tgui::Event::KeyboardKey::Left;
            treeView->keyPressed(event);
            REQUIRE(!treeView->getNodes()[0].expanded);
            treeView->keyPressed(event);
            REQUIRE(!treeView->getNodes()[0].expanded);

            event.code = tgui::Event::KeyboardKey::Right;
            treeView->keyPressed(event);
            REQUIRE(treeView->getNodes()[0].expanded);
            treeView->keyPressed(event);
            REQUIRE(treeView->getNodes()[0].expanded);
        }
    }

    testWidgetRenderer(treeView->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = treeView->getRenderer();

        SECTION("colored")
        {
            tgui::ScrollbarRenderer scrollbarRenderer;
            scrollbarRenderer.setTrackColor(tgui::Color::Red);
            scrollbarRenderer.setThumbColor(tgui::Color::Blue);

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(20, 30, 40)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColor", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColorHover", "rgb(90, 100, 110)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColorHover", "rgb(100, 110, 120)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", "(5, 6, 7, 8)"));
                REQUIRE_NOTHROW(renderer->setProperty("Scrollbar", "{ TrackColor = Red; ThumbColor = Blue; }"));
                REQUIRE_NOTHROW(renderer->setProperty("ScrollbarWidth", "15"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", tgui::Color{20, 30, 40}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", tgui::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", tgui::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", tgui::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColor", tgui::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColorHover", tgui::Color{90, 100, 110}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", tgui::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColorHover", tgui::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", tgui::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Borders{5, 6, 7, 8}));
                REQUIRE_NOTHROW(renderer->setProperty("Scrollbar", scrollbarRenderer.getData()));
                REQUIRE_NOTHROW(renderer->setProperty("ScrollbarWidth", 15));
            }

            SECTION("functions")
            {
                renderer->setBackgroundColor({20, 30, 40});
                renderer->setBackgroundColorHover({50, 60, 70});
                renderer->setTextColor({30, 40, 50});
                renderer->setTextColorHover({40, 50, 60});
                renderer->setSelectedBackgroundColor({60, 70, 80});
                renderer->setSelectedBackgroundColorHover({90, 100, 110});
                renderer->setSelectedTextColor({70, 80, 90});
                renderer->setSelectedTextColorHover({100, 110, 120});
                renderer->setBorderColor({80, 90, 100});
                renderer->setBorders({1, 2, 3, 4});
                renderer->setPadding({5, 6, 7, 8});
                renderer->setScrollbar(scrollbarRenderer.getData());
                renderer->setScrollbarWidth(15);
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == tgui::Color(20, 30, 40));
            REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == tgui::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("TextColor").getColor() == tgui::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("TextColorHover").getColor() == tgui::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("SelectedBackgroundColor").getColor() == tgui::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("SelectedBackgroundColorHover").getColor() == tgui::Color(90, 100, 110));
            REQUIRE(renderer->getProperty("SelectedTextColor").getColor() == tgui::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("SelectedTextColorHover").getColor() == tgui::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == tgui::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("Padding").getOutline() == tgui::Borders(5, 6, 7, 8));
            REQUIRE(renderer->getProperty("ScrollbarWidth").getNumber() == 15);

            REQUIRE(renderer->getScrollbar()->propertyValuePairs.size() == 2);
            REQUIRE(renderer->getScrollbar()->propertyValuePairs["TrackColor"].getColor() == tgui::Color::Red);
            REQUIRE(renderer->getScrollbar()->propertyValuePairs["ThumbColor"].getColor() == tgui::Color::Blue);
        }

        SECTION("textured")
        {
            tgui::Texture textureBackground("resources/Black.png", {0, 154, 48, 48}, {16, 16, 16, 16});
            tgui::Texture textureBranchExpanded("resources/TreeViewExpanded.png");
            tgui::Texture textureBranchCollapsed("resources/TreeViewCollapsed.png");
            tgui::Texture textureLeaf("resources/TreeViewLeaf.png");

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", tgui::Serializer::serialize(textureBackground)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureBranchExpanded", tgui::Serializer::serialize(textureBranchExpanded)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureBranchCollapsed", tgui::Serializer::serialize(textureBranchCollapsed)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureLeaf", tgui::Serializer::serialize(textureLeaf)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", textureBackground));
                REQUIRE_NOTHROW(renderer->setProperty("TextureBranchExpanded", textureBranchExpanded));
                REQUIRE_NOTHROW(renderer->setProperty("TextureBranchCollapsed", textureBranchCollapsed));
                REQUIRE_NOTHROW(renderer->setProperty("TextureLeaf", textureLeaf));
            }

            SECTION("functions")
            {
                renderer->setTextureBackground(textureBackground);
                renderer->setTextureBranchExpanded(textureBranchExpanded);
                renderer->setTextureBranchCollapsed(textureBranchCollapsed);
                renderer->setTextureLeaf(textureLeaf);
            }

            REQUIRE(renderer->getProperty("TextureBackground").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureBranchExpanded").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureBranchCollapsed").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureLeaf").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTextureBackground().getData() == textureBackground.getData());
            REQUIRE(renderer->getTextureBranchExpanded().getData() == textureBranchExpanded.getData());
            REQUIRE(renderer->getTextureBranchCollapsed().getData() == textureBranchCollapsed.getData());
            REQUIRE(renderer->getTextureLeaf().getData() == textureLeaf.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        treeView->addItem({"Smilies", "Happy"});
        treeView->addItem({"Smilies", "Sad"});
        treeView->addItem({"Smilies", "Neither"});
        treeView->addItem({"Vehicles", "Parts", "Wheel"});
        treeView->addItem({"Vehicles", "Whole", "Truck"});
        treeView->addItem({"Vehicles", "Whole", "Car"});
        treeView->collapse({"Vehicles", "Whole"});

        treeView->setItemHeight(30);
        treeView->setTextSize(25);

        testSavingWidget("TreeView", treeView);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(140, 120, treeView)

        treeView->setPosition(10, 5);
        treeView->setSize(120, 110);
        treeView->setTextSize(14);
        treeView->setItemHeight(18);

        treeView->addItem({"Smilies", "Happy"});
        treeView->addItem({"Smilies", "Sad"});
        treeView->addItem({"Smilies", "Neither"});
        treeView->addItem({"Vehicles", "Parts", "Wheel"});
        treeView->addItem({"Vehicles", "Whole", "Truck"});
        treeView->addItem({"Vehicles", "Whole", "Car"});
        treeView->collapse({"Smilies"});

        tgui::TreeViewRenderer renderer = tgui::RendererData::create();
        renderer.setBackgroundColor(tgui::Color::Green);
        renderer.setTextColor(tgui::Color::Red);
        renderer.setSelectedBackgroundColor(tgui::Color::White);
        renderer.setSelectedTextColor(tgui::Color::Black);
        renderer.setBorderColor(tgui::Color::Blue);
        renderer.setBorders({1, 2, 3, 4});
        renderer.setPadding({4, 3, 2, 1});
        renderer.setScrollbarWidth(14);
        renderer.setOpacity(0.7f);
        treeView->setRenderer(renderer.getData());

        auto setHoverRenderer = [&]{
                                    renderer.setBackgroundColorHover(tgui::Color::Yellow);
                                    renderer.setTextColorHover(tgui::Color::Magenta);
                                    renderer.setSelectedBackgroundColorHover(tgui::Color::Cyan);
                                    renderer.setSelectedTextColorHover("#808080");
                                 };

        const tgui::Vector2f mousePos1{30, 20};
        const tgui::Vector2f mousePos2{30, 35};
        const tgui::Vector2f mousePos3{30, 75};

        SECTION("Colored")
        {
            SECTION("No selected item")
            {
                SECTION("No hover")
                {
                    TEST_DRAW("TreeView_NoSelectedNoHover.png")
                }

                SECTION("Hover")
                {
                    treeView->mouseMoved(mousePos2);

                    SECTION("No hover properties set")
                    {
                        TEST_DRAW("TreeView_NoSelectedHover_NoHoverSet.png")
                    }
                    SECTION("Hover properties set")
                    {
                        setHoverRenderer();
                        TEST_DRAW("TreeView_NoSelectedHover_HoverSet.png")
                    }
                }
            }

            SECTION("Selected item")
            {
                treeView->selectItem({"Smilies"});

                SECTION("No hover")
                {
                    TEST_DRAW("TreeView_SelectedNoHover.png")
                }

                SECTION("Hover selected")
                {
                    treeView->mouseMoved(mousePos1);

                    SECTION("No hover properties set")
                    {
                        TEST_DRAW("TreeView_SelectedHoverSelected_NoHoverSet.png")
                    }
                    SECTION("Hover properties set")
                    {
                        setHoverRenderer();
                        TEST_DRAW("TreeView_SelectedHoverSelected_HoverSet.png")
                    }
                }

                SECTION("Hover other")
                {
                    treeView->mouseMoved(mousePos3);

                    SECTION("No hover properties set")
                    {
                        TEST_DRAW("TreeView_SelectedHoverOther_NoHoverSet.png")
                    }
                    SECTION("Hover properties set")
                    {
                        setHoverRenderer();
                        TEST_DRAW("TreeView_SelectedHoverOther_HoverSet.png")
                    }
                }
            }
        }

        SECTION("Textured")
        {
            renderer.setTextureBackground("resources/Texture1.png");
            renderer.setTextureBranchExpanded({"resources/Texture5.png", {0,0,15,15}});
            renderer.setTextureBranchCollapsed({"resources/Texture7.png", {0,0,15,15}});
            renderer.setTextureLeaf({"resources/Texture8.png", {0,0,15,15}});

            SECTION("No selected item")
            {
                SECTION("No hover")
                {
                    TEST_DRAW("TreeView_NoSelectedNoHover_Texture.png")
                }

                SECTION("Hover")
                {
                    treeView->mouseMoved(mousePos2);

                    SECTION("No hover properties set")
                    {
                        TEST_DRAW("TreeView_NoSelectedHover_NoHoverSet_Texture.png")
                    }
                    SECTION("Hover properties set")
                    {
                        setHoverRenderer();
                        TEST_DRAW("TreeView_NoSelectedHover_HoverSet_Texture.png")
                    }
                }
            }

            SECTION("Selected item")
            {
                treeView->selectItem({"Smilies"});

                SECTION("No hover")
                {
                    TEST_DRAW("TreeView_SelectedNoHover_Texture.png")
                }

                SECTION("Hover selected")
                {
                    treeView->mouseMoved(mousePos1);

                    SECTION("No hover properties set")
                    {
                        TEST_DRAW("TreeView_SelectedHoverSelected_NoHoverSet_Texture.png")
                    }
                    SECTION("Hover properties set")
                    {
                        setHoverRenderer();
                        TEST_DRAW("TreeView_SelectedHoverSelected_HoverSet_Texture.png")
                    }
                }

                SECTION("Hover other")
                {
                    treeView->mouseMoved(mousePos3);

                    SECTION("No hover properties set")
                    {
                        TEST_DRAW("TreeView_SelectedHoverOther_NoHoverSet_Texture.png")
                    }
                    SECTION("Hover properties set")
                    {
                        setHoverRenderer();
                        TEST_DRAW("TreeView_SelectedHoverOther_HoverSet_Texture.png")
                    }
                }
            }
        }
    }
}
