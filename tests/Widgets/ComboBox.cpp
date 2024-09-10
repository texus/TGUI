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

TEST_CASE("[ComboBox]")
{
    tgui::ComboBox::Ptr comboBox = tgui::ComboBox::create();
    comboBox->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        comboBox->onItemSelect([](){});
        comboBox->onItemSelect([](int){});
        comboBox->onItemSelect([](const tgui::String&){});
        comboBox->onItemSelect([](const tgui::String&, const tgui::String&){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(comboBox)->getSignal("ItemSelected").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(comboBox->getWidgetType() == "ComboBox");
    }

    SECTION("Position and Size")
    {
        comboBox->setPosition(40, 30);
        comboBox->setSize(150, 35);
        comboBox->getRenderer()->setBorders(2);

        REQUIRE(comboBox->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(comboBox->getSize() == tgui::Vector2f(150, 35));
        REQUIRE(comboBox->getFullSize() == comboBox->getSize());
        REQUIRE(comboBox->getWidgetOffset() == tgui::Vector2f(0, 0));
    }

    SECTION("Adding items")
    {
        REQUIRE(comboBox->getItemCount() == 0);
        comboBox->addItem("Item 1", "1");
        REQUIRE(comboBox->getItemCount() == 1);
        comboBox->addItem("Item 2", "2");
        comboBox->addItem("Item 3");
        REQUIRE(comboBox->getItemCount() == 3);

        REQUIRE(comboBox->getItems()[0] == "Item 1");
        REQUIRE(comboBox->getItems()[1] == "Item 2");
        REQUIRE(comboBox->getItems()[2] == "Item 3");

        REQUIRE(comboBox->getItemById("1") == "Item 1");
        REQUIRE(comboBox->getItemById("2") == "Item 2");
        REQUIRE(comboBox->getItemById("3") == "");

        comboBox->addMultipleItems({"Item 4", "Item 5", "Item 6", "Item 7"});
        REQUIRE(comboBox->getItemCount() == 7);
        REQUIRE(comboBox->getItems()[3] == "Item 4");
        REQUIRE(comboBox->getItems()[6] == "Item 7");
        REQUIRE(comboBox->getIdByIndex(6) == "");

        comboBox->addMultipleItems({});
        REQUIRE(comboBox->getItemCount() == 7);
    }

    SECTION("Getters")
    {
        comboBox->addItem("Item 1", "1");
        comboBox->addItem("Item 2", "2");

        REQUIRE(comboBox->getItems()[0] == "Item 1");

        REQUIRE(comboBox->getItemById("1") == "Item 1");
        REQUIRE(comboBox->getItemById("3") == "");

        REQUIRE(comboBox->getItemByIndex(1) == "Item 2");
        REQUIRE(comboBox->getItemByIndex(2) == "");

        REQUIRE(comboBox->getIndexById("1") == 0);
        REQUIRE(comboBox->getIndexById("3") == -1);

        REQUIRE(comboBox->getIdByIndex(1) == "2");
        REQUIRE(comboBox->getIdByIndex(2) == "");
    }
    
    SECTION("Removing items")
    {
        comboBox->addItem("Item 1", "1");
        comboBox->addItem("Item 2", "2");
        comboBox->addItem("Item 3", "3");
        REQUIRE(comboBox->getItemCount() == 3);

        REQUIRE(!comboBox->removeItem("Item 0"));
        REQUIRE(comboBox->getItemCount() == 3);
        REQUIRE(comboBox->removeItem("Item 2"));
        REQUIRE(comboBox->getItemCount() == 2);

        comboBox->addItem("Item 4", "4");
        REQUIRE(comboBox->getItemCount() == 3);
        REQUIRE(comboBox->getItems()[0] == "Item 1");
        REQUIRE(comboBox->getItems()[1] == "Item 3");
        REQUIRE(comboBox->getItems()[2] == "Item 4");

        REQUIRE(!comboBox->removeItemByIndex(3));
        REQUIRE(comboBox->getItemCount() == 3);
        REQUIRE(comboBox->removeItemByIndex(0));
        REQUIRE(comboBox->getItemCount() == 2);
        REQUIRE(comboBox->getItems()[0] == "Item 3");
        REQUIRE(comboBox->getItems()[1] == "Item 4");

        REQUIRE(!comboBox->removeItemById("2"));
        REQUIRE(comboBox->getItemCount() == 2);
        REQUIRE(comboBox->removeItemById("4"));
        REQUIRE(comboBox->getItemCount() == 1);
        REQUIRE(comboBox->getItems()[0] == "Item 3");
        
        comboBox->addItem("Item 5");
        comboBox->addItem("Item 5");
        comboBox->addItem("Item 6");
        REQUIRE(comboBox->getItemCount() == 4);
        comboBox->removeAllItems();
        REQUIRE(comboBox->getItemCount() == 0);
    }
    
    SECTION("Changing items")
    {
        comboBox->addItem("Item 1", "1");
        comboBox->addItem("Item 2", "2");
        comboBox->addItem("Item 3", "3");

        REQUIRE(!comboBox->changeItem("Item 0", "Item 00"));
        REQUIRE(comboBox->getItemCount() == 3);
        REQUIRE(comboBox->getItems()[0] == "Item 1");
        REQUIRE(comboBox->getItems()[1] == "Item 2");
        REQUIRE(comboBox->getItems()[2] == "Item 3");
        REQUIRE(comboBox->changeItem("Item 1", "Item 10"));
        REQUIRE(comboBox->getItemCount() == 3);
        REQUIRE(comboBox->getItems()[0] == "Item 10");
        REQUIRE(comboBox->getItems()[1] == "Item 2");
        REQUIRE(comboBox->getItems()[2] == "Item 3");
        
        REQUIRE(!comboBox->changeItemById("0", "Item 00"));
        REQUIRE(comboBox->getItemCount() == 3);
        REQUIRE(comboBox->getItems()[0] == "Item 10");
        REQUIRE(comboBox->getItems()[1] == "Item 2");
        REQUIRE(comboBox->getItems()[2] == "Item 3");
        REQUIRE(comboBox->changeItemById("3", "Item 30"));
        REQUIRE(comboBox->getItemCount() == 3);
        REQUIRE(comboBox->getItems()[0] == "Item 10");
        REQUIRE(comboBox->getItems()[1] == "Item 2");
        REQUIRE(comboBox->getItems()[2] == "Item 30");
        
        REQUIRE(!comboBox->changeItemByIndex(3, "Item 00"));
        REQUIRE(comboBox->getItemCount() == 3);
        REQUIRE(comboBox->getItems()[0] == "Item 10");
        REQUIRE(comboBox->getItems()[1] == "Item 2");
        REQUIRE(comboBox->getItems()[2] == "Item 30");
        REQUIRE(comboBox->changeItemByIndex(1, "Item 20"));
        REQUIRE(comboBox->getItemCount() == 3);
        REQUIRE(comboBox->getItems()[0] == "Item 10");
        REQUIRE(comboBox->getItems()[1] == "Item 20");
        REQUIRE(comboBox->getItems()[2] == "Item 30");
    }

    SECTION("Selecting items")
    {
        comboBox->addItem("Item 1", "1");
        comboBox->addItem("Item 2", "2");
        comboBox->addItem("Item 3", "3");

        REQUIRE(comboBox->getSelectedItem() == "");
        REQUIRE(comboBox->getSelectedItemId() == "");        
        REQUIRE(comboBox->getSelectedItemIndex() == -1);
        
        REQUIRE(!comboBox->setSelectedItem("Item 0"));
        REQUIRE(comboBox->getSelectedItemIndex() == -1);
        REQUIRE(comboBox->setSelectedItem("Item 1"));
        REQUIRE(comboBox->getSelectedItem() == "Item 1");
        REQUIRE(comboBox->getSelectedItemId() == "1");        
        REQUIRE(comboBox->getSelectedItemIndex() == 0);

        REQUIRE(!comboBox->setSelectedItemById("0"));
        REQUIRE(comboBox->getSelectedItemIndex() == -1);
        REQUIRE(comboBox->setSelectedItemById("2"));
        REQUIRE(comboBox->getSelectedItem() == "Item 2");
        REQUIRE(comboBox->getSelectedItemId() == "2");        
        REQUIRE(comboBox->getSelectedItemIndex() == 1);

        REQUIRE(!comboBox->setSelectedItemByIndex(3));
        REQUIRE(comboBox->getSelectedItemIndex() == -1);
        REQUIRE(comboBox->setSelectedItemByIndex(2));
        REQUIRE(comboBox->getSelectedItem() == "Item 3");
        REQUIRE(comboBox->getSelectedItemId() == "3");        
        REQUIRE(comboBox->getSelectedItemIndex() == 2);

        comboBox->deselectItem();
        REQUIRE(comboBox->getSelectedItem() == "");
        REQUIRE(comboBox->getSelectedItemId() == "");        
        REQUIRE(comboBox->getSelectedItemIndex() == -1);
    }

    SECTION("Contains")
    {
        REQUIRE(!comboBox->contains("Item 1"));
        REQUIRE(!comboBox->containsId("1"));

        comboBox->addItem("Item 1", "1");
        REQUIRE(comboBox->contains("Item 1"));
        REQUIRE(comboBox->containsId("1"));

        comboBox->addItem("Item 2");
        REQUIRE(comboBox->contains("Item 2"));
        REQUIRE(!comboBox->containsId("2"));

        comboBox->removeItem("Item 1");
        REQUIRE(!comboBox->contains("Item 1"));
        REQUIRE(!comboBox->containsId("1"));
    }

    SECTION("Data")
    {
        comboBox->addItem("Item 1", "1");
        comboBox->addItem("Item 2", "2");
        comboBox->addItem("Item 3", "3");

        comboBox->setItemData(0, "Test");
        comboBox->setItemData(1, 5);
        comboBox->setItemData(2, tgui::String("x"));

        REQUIRE(tgui::String(comboBox->getItemData<const char*>(0)) == "Test");
        REQUIRE(comboBox->getItemData<int>(1) == 5);
        REQUIRE(comboBox->getItemData<tgui::String>(2) == "x");

        // Wrong type results in std::bad_cast
        REQUIRE_THROWS_AS(comboBox->getItemData<int>(2), std::bad_cast);

        // Using an index beyond the added items also results in std::bad_cast
        REQUIRE_THROWS_AS(comboBox->getItemData<tgui::String>(3), std::bad_cast);

        // Re-assigning data is allowed to change the type
        comboBox->setItemData(0, 3);
        REQUIRE(comboBox->getItemData<int>(0) == 3);
    }
    
    SECTION("ItemsToDisplay")
    {
        comboBox->setItemsToDisplay(5);
        REQUIRE(comboBox->getItemsToDisplay() == 5);
    }

    SECTION("MaximumItems")
    {
        comboBox->addItem("Item 1");
        comboBox->addItem("Item 2");
        comboBox->addItem("Item 3");
        comboBox->addItem("Item 4");
        comboBox->addItem("Item 5");
        REQUIRE(comboBox->getItemCount() == 5);
        comboBox->setMaximumItems(3);
        REQUIRE(comboBox->getItemCount() == 3);
        REQUIRE(comboBox->getItems()[0] == "Item 1");
        REQUIRE(comboBox->getItems()[2] == "Item 3");
        
        comboBox->addItem("Item 6");
        REQUIRE(comboBox->getItemCount() == 3);
        REQUIRE(comboBox->getItems()[0] == "Item 1");
        REQUIRE(comboBox->getItems()[2] == "Item 3");

        comboBox->setMaximumItems(5);
        comboBox->addMultipleItems({"Item 4", "Item 5", "Item 6", "Item 7"});
        REQUIRE(comboBox->getItemCount() == 5);
        REQUIRE(comboBox->getItems()[3] == "Item 4");
        REQUIRE(comboBox->getItems()[4] == "Item 5");
    }

    SECTION("TextSize")
    {
        comboBox->setTextSize(25);
        REQUIRE(comboBox->getTextSize() == 25);
    }

    SECTION("DefaultText")
    {
        REQUIRE(comboBox->getDefaultText().empty());
        comboBox->setDefaultText("Hello");
        REQUIRE(comboBox->getDefaultText() == "Hello");
    }

    SECTION("ExpandDirection")
    {
        REQUIRE(comboBox->getExpandDirection() == tgui::ComboBox::ExpandDirection::Automatic);
        comboBox->setExpandDirection(tgui::ComboBox::ExpandDirection::Up);
        REQUIRE(comboBox->getExpandDirection() == tgui::ComboBox::ExpandDirection::Up);
        comboBox->setExpandDirection(tgui::ComboBox::ExpandDirection::Down);
        REQUIRE(comboBox->getExpandDirection() == tgui::ComboBox::ExpandDirection::Down);
        comboBox->setExpandDirection(tgui::ComboBox::ExpandDirection::Automatic);
        REQUIRE(comboBox->getExpandDirection() == tgui::ComboBox::ExpandDirection::Automatic);
    }

    SECTION("ChangeItemOnScroll")
    {
        REQUIRE(!comboBox->getChangeItemOnScroll());
        comboBox->setChangeItemOnScroll(true);
        REQUIRE(comboBox->getChangeItemOnScroll());
        comboBox->setChangeItemOnScroll(false);
        REQUIRE(!comboBox->getChangeItemOnScroll());
    }

    SECTION("Events / Signals")
    {
        SECTION("Widget")
        {
            testWidgetSignals(comboBox);
        }

        SECTION("ItemSelected")
        {
            auto container = tgui::Group::create({400.f, 300.f});
            container->add(comboBox);

            auto mouseClick = [container](tgui::Vector2f pos) {
                container->mouseMoved(pos);
                container->leftMousePressed(pos);
                container->leftMouseReleased(pos);
            };

            unsigned int itemSelectedCount = 0;
            comboBox->onItemSelect(&genericCallback, std::ref(itemSelectedCount));

            comboBox->setPosition(25, 6);
            comboBox->setSize(150, 24);
            comboBox->getRenderer()->setBorders(2);
            comboBox->addItem("1");
            comboBox->addItem("2", "id2");
            comboBox->addItem("3");
            comboBox->setExpandDirection(tgui::ComboBox::ExpandDirection::Automatic);
            comboBox->setChangeItemOnScroll(true);

            const tgui::Vector2f mousePosOnComboBox = {100, 15};
            const tgui::Vector2f mousePosOnItem1 = {100, 40};
            const tgui::Vector2f mousePosOnItem2 = {100, 60};

            SECTION("Click")
            {
                // Selecting second item
                mouseClick(mousePosOnComboBox);
                REQUIRE(itemSelectedCount == 0);
                mouseClick(mousePosOnItem2);
                REQUIRE(itemSelectedCount == 1);
                REQUIRE(comboBox->getSelectedItemIndex() == 1);

                // List was closed when item was clicked
                mouseClick(mousePosOnItem1);
                REQUIRE(comboBox->getSelectedItemIndex() == 1);

                // Clicking on combo box when list is open also closes list
                mouseClick(mousePosOnComboBox);
                mouseClick(mousePosOnComboBox);
                mouseClick(mousePosOnItem1);
                REQUIRE(comboBox->getSelectedItemIndex() == 1);

                REQUIRE(itemSelectedCount == 1);
            }

            SECTION("Mouse wheel scroll")
            {
                container->scrolled(-1, mousePosOnComboBox, false);
                REQUIRE(comboBox->getSelectedItemIndex() == 0);
                REQUIRE(itemSelectedCount == 1);
                container->scrolled(-1, mousePosOnComboBox, false);
                REQUIRE(comboBox->getSelectedItemIndex() == 1);
                REQUIRE(itemSelectedCount == 2);
                container->scrolled(1, mousePosOnComboBox, false);
                REQUIRE(comboBox->getSelectedItemIndex() == 0);
                REQUIRE(itemSelectedCount == 3);

                // If the scroll event originated from touch events (two finger scrolling) then it has no effect
                container->scrolled(-1, mousePosOnComboBox, true);
                REQUIRE(comboBox->getSelectedItemIndex() == 0);
                container->scrolled(1, mousePosOnComboBox, true);
                REQUIRE(comboBox->getSelectedItemIndex() == 0);

                // Changing item by scrolling can be disabled
                comboBox->setChangeItemOnScroll(false);
                container->scrolled(-1, mousePosOnComboBox, false);
                REQUIRE(comboBox->getSelectedItemIndex() == 0);
                comboBox->setChangeItemOnScroll(true);

                // Scrolling on the combo box has no effect when the list is open
                mouseClick(mousePosOnComboBox);
                container->scrolled(-1, mousePosOnComboBox, false);
                REQUIRE(comboBox->getSelectedItemIndex() == 0);
                REQUIRE(itemSelectedCount == 3);
            }

            SECTION("Programmatically")
            {
                comboBox->setSelectedItem("1");
                REQUIRE(itemSelectedCount == 1);
                comboBox->setSelectedItem("1");
                REQUIRE(itemSelectedCount == 1);

                comboBox->setSelectedItemById("id2");
                REQUIRE(itemSelectedCount == 2);
                comboBox->setSelectedItemById("id2");
                REQUIRE(itemSelectedCount == 2);

                comboBox->setSelectedItemByIndex(0);
                REQUIRE(itemSelectedCount == 3);
                comboBox->setSelectedItemByIndex(0);
                REQUIRE(itemSelectedCount == 3);
            }
        }
    }

    testWidgetRenderer(comboBox->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = comboBox->getRenderer();

        SECTION("colored")
        {
            tgui::ListBoxRenderer listBoxRenderer;
            listBoxRenderer.setBackgroundColor(tgui::Color::Red);
            listBoxRenderer.setTextColor(tgui::Color::Blue);

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(20, 30, 40)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDisabled", "rgb(120, 130, 140)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColor", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColorHover", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColorDisabled", "rgb(130, 140, 150)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColor", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorHover", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorDisabled", "rgb(140, 150, 160)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDisabled", "rgb(150, 160, 170)"));
                REQUIRE_NOTHROW(renderer->setProperty("DefaultTextColor", "rgb(110, 120, 130)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", "(5, 6, 7, 8)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyle", "Bold"));
                REQUIRE_NOTHROW(renderer->setProperty("DefaultTextStyle", "Italic"));
                REQUIRE_NOTHROW(renderer->setProperty("ListBox", "{ BackgroundColor = Red; TextColor = Blue; }"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", tgui::Color{20, 30, 40}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDisabled", tgui::Color{120, 130, 140}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColor", tgui::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColorHover", tgui::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColorDisabled", tgui::Color{130, 140, 150}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColor", tgui::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorHover", tgui::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorDisabled", tgui::Color{140, 150, 160}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", tgui::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDisabled", tgui::Color{150, 160, 170}));
                REQUIRE_NOTHROW(renderer->setProperty("DefaultTextColor", tgui::Color{110, 120, 130}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", tgui::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Borders{5, 6, 7, 8}));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyle", tgui::TextStyle::Bold));
                REQUIRE_NOTHROW(renderer->setProperty("DefaultTextStyle", tgui::TextStyle::Italic));
                REQUIRE_NOTHROW(renderer->setProperty("ListBox", listBoxRenderer.getData()));
            }

            SECTION("functions")
            {
                renderer->setBackgroundColor({20, 30, 40});
                renderer->setBackgroundColorDisabled({120, 130, 140});
                renderer->setArrowBackgroundColor({30, 40, 50});
                renderer->setArrowBackgroundColorHover({40, 50, 60});
                renderer->setArrowBackgroundColorDisabled({130, 140, 150});
                renderer->setArrowColor({50, 60, 70});
                renderer->setArrowColorHover({60, 70, 80});
                renderer->setArrowColorDisabled({140, 150, 160});
                renderer->setTextColor({70, 80, 90});
                renderer->setTextColorDisabled({150, 160, 170});
                renderer->setDefaultTextColor({110, 120, 130});
                renderer->setBorderColor({80, 90, 100});
                renderer->setBorders({1, 2, 3, 4});
                renderer->setPadding({5, 6, 7, 8});
                renderer->setTextStyle(tgui::TextStyle::Bold);
                renderer->setDefaultTextStyle(tgui::TextStyle::Italic);
                renderer->setListBox(listBoxRenderer.getData());
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == tgui::Color(20, 30, 40));
            REQUIRE(renderer->getProperty("BackgroundColorDisabled").getColor() == tgui::Color(120, 130, 140));
            REQUIRE(renderer->getProperty("ArrowBackgroundColor").getColor() == tgui::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("ArrowBackgroundColorHover").getColor() == tgui::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("ArrowBackgroundColorDisabled").getColor() == tgui::Color(130, 140, 150));
            REQUIRE(renderer->getProperty("ArrowColor").getColor() == tgui::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("ArrowColorHover").getColor() == tgui::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("ArrowColorDisabled").getColor() == tgui::Color(140, 150, 160));
            REQUIRE(renderer->getProperty("TextColor").getColor() == tgui::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("TextColorDisabled").getColor() == tgui::Color(150, 160, 170));
            REQUIRE(renderer->getProperty("DefaultTextColor").getColor() == tgui::Color(110, 120, 130));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == tgui::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("Padding").getOutline() == tgui::Borders(5, 6, 7, 8));
            REQUIRE(renderer->getProperty("TextStyle").getTextStyle() == tgui::TextStyle::Bold);
            REQUIRE(renderer->getProperty("DefaultTextStyle").getTextStyle() == tgui::TextStyle::Italic);

            REQUIRE(renderer->getListBox()->propertyValuePairs.size() >= 2);  // Also contains Scrollbar from default White theme
            REQUIRE(renderer->getListBox()->propertyValuePairs["BackgroundColor"].getColor() == tgui::Color::Red);
            REQUIRE(renderer->getListBox()->propertyValuePairs["TextColor"].getColor() == tgui::Color::Blue);
        }

        SECTION("textured")
        {
            tgui::Texture textureBackground("resources/Black.png", {0, 154, 48, 48}, {16, 16, 16, 16});
            tgui::Texture textureBackgroundDisabled("resources/Black.png", {0, 154, 48, 48}, {16, 16, 16, 16});
            tgui::Texture textureArrow("resources/Black.png", {92,  0, 32, 32});
            tgui::Texture textureArrowHover("resources/Black.png", {92, 32, 32, 32});
            tgui::Texture textureArrowDisabled("resources/Black.png", {92,  0, 32, 32});

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", tgui::Serializer::serialize(textureBackground)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackgroundDisabled", tgui::Serializer::serialize(textureBackgroundDisabled)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrow", tgui::Serializer::serialize(textureArrow)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowHover", tgui::Serializer::serialize(textureArrowHover)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowDisabled", tgui::Serializer::serialize(textureArrowDisabled)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", textureBackground));
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackgroundDisabled", textureBackgroundDisabled));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrow", textureArrow));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowHover", textureArrowHover));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowDisabled", textureArrowDisabled));
            }

            SECTION("functions")
            {
                renderer->setTextureBackground(textureBackground);
                renderer->setTextureBackgroundDisabled(textureBackgroundDisabled);
                renderer->setTextureArrow(textureArrow);
                renderer->setTextureArrowHover(textureArrowHover);
                renderer->setTextureArrowDisabled(textureArrowDisabled);
            }

            REQUIRE(renderer->getProperty("TextureBackground").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureBackgroundDisabled").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureArrow").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureArrowHover").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureArrowDisabled").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTextureBackground().getData() == textureBackground.getData());
            REQUIRE(renderer->getTextureBackgroundDisabled().getData() == textureBackgroundDisabled.getData());
            REQUIRE(renderer->getTextureArrow().getData() == textureArrow.getData());
            REQUIRE(renderer->getTextureArrowHover().getData() == textureArrowHover.getData());
            REQUIRE(renderer->getTextureArrowDisabled().getData() == textureArrowDisabled.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        comboBox->addItem("Item 1", "1");
        comboBox->addItem("Item 2");
        comboBox->addItem("Item 3", "3");
        comboBox->setItemsToDisplay(3);
        comboBox->setMaximumItems(5);
        comboBox->setSelectedItem("Item 2");
        comboBox->setExpandDirection(tgui::ComboBox::ExpandDirection::Up);
        comboBox->setChangeItemOnScroll(false);

        testSavingWidget("ComboBox", comboBox);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(80, 35, comboBox)

        comboBox->setEnabled(true);
        comboBox->setPosition(10, 5);
        comboBox->setSize(60, 24);
        comboBox->setTextSize(14);

        tgui::ComboBoxRenderer renderer = tgui::RendererData::create();
        renderer.setBackgroundColor(tgui::Color::Green);
        renderer.setTextColor(tgui::Color::Red);
        renderer.setDefaultTextColor(tgui::Color::White);
        renderer.setBorderColor(tgui::Color::Blue);
        renderer.setArrowBackgroundColor(tgui::Color::Cyan);
        renderer.setArrowColor(tgui::Color::Magenta);
        renderer.setBorders({1, 2, 3, 4});
        renderer.setTextStyle(tgui::TextStyle::Italic);
        renderer.setDefaultTextStyle(tgui::TextStyle::Italic);
        renderer.setOpacity(0.7f);
        comboBox->setRenderer(renderer.getData());

        auto setHoverRenderer = [&](bool textured){
            renderer.setArrowBackgroundColorHover(tgui::Color::Yellow);
            renderer.setArrowColorHover(tgui::Color::Black);
            if (textured)
                renderer.setTextureArrowHover("resources/Texture3.png");
        };

        auto setDisabledRenderer = [&](bool textured){
            renderer.setTextColorDisabled({128, 128, 0});
            renderer.setBackgroundColorDisabled({0, 128, 128});
            renderer.setArrowBackgroundColorDisabled({128, 0, 128});
            renderer.setArrowColorDisabled({160, 200, 0});
            if (textured)
            {
                renderer.setTextureBackgroundDisabled("resources/Texture4.png");
                renderer.setTextureArrowDisabled("resources/Texture5.png");
            }
        };

        comboBox->addItem("1");
        comboBox->addItem("2");
        comboBox->addItem("3");

        const tgui::Vector2f mousePos{60, 15};

        SECTION("Colored")
        {
            SECTION("No selected item")
            {
                SECTION("No hover")
                {
                    TEST_DRAW("ComboBox_NoSelectedNoHover.png")
                }

                SECTION("Hover")
                {
                    comboBox->mouseMoved(mousePos);

                    SECTION("No hover properties set")
                    {
                        TEST_DRAW("ComboBox_NoSelectedHover_NoHoverSet.png")
                    }
                    SECTION("Hover properties set")
                    {
                        setHoverRenderer(false);
                        TEST_DRAW("ComboBox_NoSelectedHover_HoverSet.png")
                    }
                }
            }

            SECTION("Selected item")
            {
                comboBox->setSelectedItem("2");

                SECTION("No hover")
                {
                    TEST_DRAW("ComboBox_SelectedNoHover.png")
                }

                SECTION("Hover selected")
                {
                    comboBox->mouseMoved(mousePos);

                    SECTION("No hover properties set")
                    {
                        TEST_DRAW("ComboBox_SelectedHoverSelected_NoHoverSet.png")
                    }
                    SECTION("Hover properties set")
                    {
                        setHoverRenderer(false);
                        TEST_DRAW("ComboBox_SelectedHoverSelected_HoverSet.png")
                    }
                }

                SECTION("Disabled")
                {
                    comboBox->setEnabled(false);
                    TEST_DRAW("ComboBox_Disabled_NormalSet.png")

                    SECTION("DisabledSet")
                    {
                        setDisabledRenderer(true);
                        TEST_DRAW("ComboBox_Disabled_DisabledSet.png")
                    }
                }
            }
        }

        SECTION("Textured")
        {
            renderer.setTextureBackground("resources/Texture1.png");
            renderer.setTextureArrow("resources/Texture2.png");

            SECTION("No selected item")
            {
                SECTION("No hover")
                {
                    TEST_DRAW("ComboBox_NoSelectedNoHover_Textured.png")
                }

                SECTION("Hover")
                {
                    comboBox->mouseMoved(mousePos);

                    SECTION("No hover properties set")
                    {
                        TEST_DRAW("ComboBox_NoSelectedHover_NoHoverSet_Textured.png")
                    }
                    SECTION("Hover properties set")
                    {
                        setHoverRenderer(true);
                        TEST_DRAW("ComboBox_NoSelectedHover_HoverSet_Textured.png")
                    }
                }
            }

            SECTION("Selected item")
            {
                comboBox->setSelectedItem("2");

                SECTION("No hover")
                {
                    TEST_DRAW("ComboBox_SelectedNoHover_Textured.png")
                }

                SECTION("Hover selected")
                {
                    comboBox->mouseMoved(mousePos);

                    SECTION("No hover properties set")
                    {
                        TEST_DRAW("ComboBox_SelectedHoverSelected_NoHoverSet_Textured.png")
                    }
                    SECTION("Hover properties set")
                    {
                        setHoverRenderer(true);
                        TEST_DRAW("ComboBox_SelectedHoverSelected_HoverSet_Textured.png")
                    }
                }

                SECTION("Disabled")
                {
                    comboBox->setEnabled(false);
                    TEST_DRAW("ComboBox_Disabled_TextureNormalSet.png")

                    SECTION("DisabledSet")
                    {
                        setDisabledRenderer(true);
                        TEST_DRAW("ComboBox_Disabled_TextureDisabledSet.png")
                    }
                }
            }
        }

        SECTION("Default text")
        {
            comboBox->setDefaultText("Test");
            TEST_DRAW("ComboBox_DefaultText.png")
        }
    }

    SECTION("Scaling")
    {
        comboBox->setPosition(10, 5);
        comboBox->setSize(60, 24);
        comboBox->setTextSize(14);
        comboBox->addItem("1");
        comboBox->addItem("2");
        comboBox->setSelectedItemByIndex(0);

        tgui::ComboBoxRenderer renderer = tgui::RendererData::create();
        renderer.setBackgroundColor(tgui::Color::Green);
        renderer.setTextColor(tgui::Color::Red);
        renderer.setBorderColor(tgui::Color::Blue);
        renderer.setArrowBackgroundColor(tgui::Color::Cyan);
        renderer.setArrowColor(tgui::Color::Magenta);
        renderer.setBorders({2});
        renderer.setTextStyle(tgui::TextStyle::Italic);
        comboBox->setRenderer(renderer.getData());

        auto outerPanel = tgui::Panel::create({214, 200});
        outerPanel->getRenderer()->setBackgroundColor(tgui::Color::Blue);
        outerPanel->getRenderer()->setBorderColor(tgui::Color::Cyan);
        outerPanel->getRenderer()->setBorders({4});
        outerPanel->setPosition({5, 10});

        auto innerPanel = tgui::Panel::create({114, 80});
        innerPanel->getRenderer()->setBackgroundColor(tgui::Color::Yellow);
        innerPanel->getRenderer()->setBorderColor(tgui::Color::Magenta);
        innerPanel->getRenderer()->setBorders({5});
        innerPanel->setPosition({20, 15});
        outerPanel->add(innerPanel);

        innerPanel->add(comboBox);

        outerPanel->setScale(1.25f);
        innerPanel->setScale(1.6f);
        comboBox->setScale(1.5f);

        TEST_DRAW_INIT(275, 275, outerPanel)
        TEST_DRAW("ComboBox_Scaling_Normal.png")

        tgui::Event event;
        event.type = tgui::Event::Type::MouseMoved;
        event.mouseMove.x = 80;
        event.mouseMove.y = 65;
        gui.handleEvent(event);

        event.type = tgui::Event::Type::MouseButtonPressed;
        event.mouseButton.button = tgui::Event::MouseButton::Left;
        event.mouseButton.x = 80;
        event.mouseButton.y = 65;
        gui.handleEvent(event);

        event.type = tgui::Event::Type::MouseButtonReleased;
        gui.handleEvent(event);

        event.type = tgui::Event::Type::MouseMoved;
        event.mouseMove.x = 230;
        event.mouseMove.y = 230;
        gui.handleEvent(event);

        event.type = tgui::Event::Type::MouseButtonPressed;
        event.mouseButton.button = tgui::Event::MouseButton::Left;
        event.mouseButton.x = 230;
        event.mouseButton.y = 230;
        gui.handleEvent(event);

        TEST_DRAW("ComboBox_Scaling_SelectingItem.png")

        event.type = tgui::Event::Type::MouseButtonReleased;
        gui.handleEvent(event);
    }
}
