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
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/ComboBox.hpp>
#include <TGUI/Widgets/Group.hpp>

TEST_CASE("[ComboBox]")
{
    tgui::ComboBox::Ptr comboBox = tgui::ComboBox::create();
    comboBox->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(comboBox->connect("ItemSelected", [](){}));
        REQUIRE_NOTHROW(comboBox->connect("ItemSelected", [](sf::String){}));
        REQUIRE_NOTHROW(comboBox->connect("ItemSelected", [](std::string){}));
        REQUIRE_NOTHROW(comboBox->connect("ItemSelected", [](sf::String, sf::String){}));
        REQUIRE_NOTHROW(comboBox->connect("ItemSelected", [](std::string, std::string){}));
        REQUIRE_NOTHROW(comboBox->connect("ItemSelected", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(comboBox->connect("ItemSelected", [](tgui::Widget::Ptr, std::string, sf::String){}));
        REQUIRE_NOTHROW(comboBox->connect("ItemSelected", [](tgui::Widget::Ptr, std::string, std::string){}));
        REQUIRE_NOTHROW(comboBox->connect("ItemSelected", [](tgui::Widget::Ptr, std::string, sf::String, sf::String){}));
        REQUIRE_NOTHROW(comboBox->connect("ItemSelected", [](tgui::Widget::Ptr, std::string, std::string, std::string){}));
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

        REQUIRE(comboBox->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(comboBox->getSize() == sf::Vector2f(150, 35));
        REQUIRE(comboBox->getFullSize() == comboBox->getSize());
        REQUIRE(comboBox->getWidgetOffset() == sf::Vector2f(0, 0));
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
    }

    SECTION("TextSize")
    {
        comboBox->setTextSize(25);
        REQUIRE(comboBox->getTextSize() == 25);
    }

    SECTION("ExpandDirection")
    {
        REQUIRE(comboBox->getExpandDirection() == tgui::ComboBox::ExpandDirection::Down);
        comboBox->setExpandDirection(tgui::ComboBox::ExpandDirection::Up);
        REQUIRE(comboBox->getExpandDirection() == tgui::ComboBox::ExpandDirection::Up);
        comboBox->setExpandDirection(tgui::ComboBox::ExpandDirection::Down);
        REQUIRE(comboBox->getExpandDirection() == tgui::ComboBox::ExpandDirection::Down);
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

            auto mouseClick = [container](sf::Vector2f pos) {
                container->mouseMoved(pos);
                container->leftMousePressed(pos);
                container->leftMouseReleased(pos);
            };

            unsigned int itemSelectedCount = 0;
            comboBox->connect("ItemSelected", &genericCallback, std::ref(itemSelectedCount));

            comboBox->setPosition(25, 6);
            comboBox->setSize(150, 24);
            comboBox->getRenderer()->setBorders(2);
            comboBox->addItem("1");
            comboBox->addItem("2");
            comboBox->addItem("3");

            const sf::Vector2f mousePosOnComboBox = {100, 15};
            const sf::Vector2f mousePosOnItem1 = {100, 40};
            const sf::Vector2f mousePosOnItem2 = {100, 60};

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
                REQUIRE(itemSelectedCount == 0);
                container->mouseWheelScrolled(-1, mousePosOnComboBox);
                REQUIRE(comboBox->getSelectedItemIndex() == 0);
                REQUIRE(itemSelectedCount == 1);
                container->mouseWheelScrolled(-1, mousePosOnComboBox);
                REQUIRE(comboBox->getSelectedItemIndex() == 1);
                REQUIRE(itemSelectedCount == 2);
                container->mouseWheelScrolled(1, mousePosOnComboBox);
                REQUIRE(comboBox->getSelectedItemIndex() == 0);
                REQUIRE(itemSelectedCount == 3);

                // Scrolling on the combo box has no effect when the list is open
                mouseClick(mousePosOnComboBox);
                container->mouseWheelScrolled(-1, mousePosOnComboBox);
                REQUIRE(comboBox->getSelectedItemIndex() == 0);
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
            listBoxRenderer.setBackgroundColor(sf::Color::Red);
            listBoxRenderer.setTextColor(sf::Color::Blue);

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(20, 30, 40)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColor", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColorHover", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColor", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorHover", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", "(5, 6, 7, 8)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyle", "Bold"));
                REQUIRE_NOTHROW(renderer->setProperty("ListBox", "{ BackgroundColor = Red; TextColor = Blue; }"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{20, 30, 40}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColor", sf::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColorHover", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColor", sf::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorHover", sf::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Borders{5, 6, 7, 8}));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyle", sf::Text::Bold));
                REQUIRE_NOTHROW(renderer->setProperty("ListBox", listBoxRenderer.getData()));
            }

            SECTION("functions")
            {
                renderer->setBackgroundColor({20, 30, 40});
                renderer->setArrowBackgroundColor({30, 40, 50});
                renderer->setArrowBackgroundColorHover({40, 50, 60});
                renderer->setArrowColor({50, 60, 70});
                renderer->setArrowColorHover({60, 70, 80});
                renderer->setTextColor({70, 80, 90});
                renderer->setBorderColor({80, 90, 100});
                renderer->setBorders({1, 2, 3, 4});
                renderer->setPadding({5, 6, 7, 8});
                renderer->setTextStyle(sf::Text::Bold);
                renderer->setListBox(listBoxRenderer.getData());
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(20, 30, 40));
            REQUIRE(renderer->getProperty("ArrowBackgroundColor").getColor() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("ArrowBackgroundColorHover").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("ArrowColor").getColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("ArrowColorHover").getColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("Padding").getOutline() == tgui::Borders(5, 6, 7, 8));
            REQUIRE(renderer->getProperty("TextStyle").getTextStyle() == sf::Text::Bold);

            REQUIRE(renderer->getListBox()->propertyValuePairs.size() == 2);
            REQUIRE(renderer->getListBox()->propertyValuePairs["backgroundcolor"].getColor() == sf::Color::Red);
            REQUIRE(renderer->getListBox()->propertyValuePairs["textcolor"].getColor() == sf::Color::Blue);
        }

        SECTION("textured")
        {
            tgui::Texture textureBackground("resources/Black.png", {0, 154, 48, 48}, {16, 16, 16, 16});
            tgui::Texture textureArrow("resources/Black.png", {92,  0, 32, 32});
            tgui::Texture textureArrowHover("resources/Black.png", {92, 32, 32, 32});

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", tgui::Serializer::serialize(textureBackground)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrow", tgui::Serializer::serialize(textureArrow)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowHover", tgui::Serializer::serialize(textureArrowHover)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", textureBackground));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrow", textureArrow));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowHover", textureArrowHover));
            }

            SECTION("functions")
            {
                renderer->setTextureBackground(textureBackground);
                renderer->setTextureArrow(textureArrow);
                renderer->setTextureArrowHover(textureArrowHover);
            }

            REQUIRE(renderer->getProperty("TextureBackground").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureArrow").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureArrowHover").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTextureBackground().getData() == textureBackground.getData());
            REQUIRE(renderer->getTextureArrow().getData() == textureArrow.getData());
            REQUIRE(renderer->getTextureArrowHover().getData() == textureArrowHover.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        comboBox->addItem("Item 1", "1");
        comboBox->addItem("Item 2");
        comboBox->addItem("Item 3", "3");
        comboBox->setItemsToDisplay(3);
        comboBox->setMaximumItems(5);
        comboBox->setExpandDirection(tgui::ComboBox::ExpandDirection::Up);

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
        renderer.setBackgroundColor(sf::Color::Green);
        renderer.setTextColor(sf::Color::Red);
        renderer.setBorderColor(sf::Color::Blue);
        renderer.setArrowBackgroundColor(sf::Color::Cyan);
        renderer.setArrowColor(sf::Color::Magenta);
        renderer.setBorders({1, 2, 3, 4});
        renderer.setTextStyle(sf::Text::Italic);
        renderer.setOpacity(0.7f);
        comboBox->setRenderer(renderer.getData());

        auto setHoverRenderer = [&](bool textured){
            renderer.setArrowBackgroundColorHover(sf::Color::Yellow);
            renderer.setArrowColorHover(sf::Color::Black);
            if (textured)
                renderer.setTextureArrowHover("resources/Texture3.png");
        };

        comboBox->addItem("1");
        comboBox->addItem("2");
        comboBox->addItem("3");

        const sf::Vector2f mousePos{60, 15};

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
            }
        }
    }
}
