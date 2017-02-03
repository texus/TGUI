/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/ComboBox.hpp>

TEST_CASE("[ComboBox]") {
    tgui::ComboBox::Ptr comboBox = std::make_shared<tgui::ComboBox>();
    comboBox->setFont("resources/DroidSansArmenian.ttf");

    SECTION("Signals") {
        REQUIRE_NOTHROW(comboBox->connect("ItemSelected", [](){}));
        REQUIRE_NOTHROW(comboBox->connect("ItemSelected", [](sf::String){}));
        REQUIRE_NOTHROW(comboBox->connect("ItemSelected", [](sf::String, sf::String){}));
    }

    SECTION("WidgetType") {
        REQUIRE(comboBox->getWidgetType() == "ComboBox");
    }

    SECTION("adding items") {
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
    
    SECTION("removing items") {
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
    
    SECTION("changing items") {
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

    SECTION("selecting items") {
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
    
    SECTION("ItemsToDisplay") {
        comboBox->setItemsToDisplay(5);
        REQUIRE(comboBox->getItemsToDisplay() == 5);
    }

    SECTION("MaximumItems") {
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

    SECTION("Scrollbar") {
        tgui::Scrollbar::Ptr scrollbar = std::make_shared<tgui::Theme>()->load("scrollbar");
    
        REQUIRE(comboBox->getScrollbar() != nullptr);
        REQUIRE(comboBox->getScrollbar() != scrollbar);
        comboBox->setScrollbar(nullptr);
        REQUIRE(comboBox->getScrollbar() == nullptr);
        comboBox->setScrollbar(scrollbar);
        REQUIRE(comboBox->getScrollbar() != nullptr);
        REQUIRE(comboBox->getScrollbar() == scrollbar);
    }

    SECTION("Renderer") {
        auto renderer = comboBox->getRenderer();

        SECTION("colored") {
            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColor", "rgb(10, 20, 30)"));
                REQUIRE(renderer->getProperty("ArrowColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("ArrowColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("ArrowColorHover").getColor() == sf::Color(10, 20, 30));
                
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColor", "rgb(20, 30, 40)"));
                REQUIRE(renderer->getProperty("ArrowBackgroundColor").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("ArrowBackgroundColorNormal").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("ArrowBackgroundColorHover").getColor() == sf::Color(20, 30, 40));
                
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(20, 30, 40)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColorNormal", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColorHover", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorNormal", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorHover", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", "(5, 6, 7, 8)"));
            }
            
            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColor", sf::Color{10, 20, 30}));
                REQUIRE(renderer->getProperty("ArrowColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("ArrowColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("ArrowColorHover").getColor() == sf::Color(10, 20, 30));
                
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColor", sf::Color{20, 30, 40}));
                REQUIRE(renderer->getProperty("ArrowBackgroundColor").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("ArrowBackgroundColorNormal").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("ArrowBackgroundColorHover").getColor() == sf::Color(20, 30, 40));
                
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{20, 30, 40}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColorNormal", sf::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColorHover", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorNormal", sf::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorHover", sf::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Borders{5, 6, 7, 8}));
            }

            SECTION("functions") {
                renderer->setArrowColor({10, 20, 30});
                REQUIRE(renderer->getProperty("ArrowColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("ArrowColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("ArrowColorHover").getColor() == sf::Color(10, 20, 30));
                
                renderer->setArrowBackgroundColor({20, 30, 40});
                REQUIRE(renderer->getProperty("ArrowBackgroundColor").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("ArrowBackgroundColorNormal").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("ArrowBackgroundColorHover").getColor() == sf::Color(20, 30, 40));

                renderer->setBackgroundColor({20, 30, 40});
                renderer->setArrowBackgroundColorNormal({30, 40, 50});
                renderer->setArrowBackgroundColorHover({40, 50, 60});
                renderer->setArrowColorNormal({50, 60, 70});
                renderer->setArrowColorHover({60, 70, 80});
                renderer->setTextColor({70, 80, 90});
                renderer->setBorderColor({80, 90, 100});
                renderer->setBorders({1, 2, 3, 4});
                renderer->setPadding({5, 6, 7, 8});

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 9);
                    REQUIRE(pairs["BackgroundColor"].getColor() == sf::Color(20, 30, 40));
                    REQUIRE(pairs["ArrowBackgroundColorNormal"].getColor() == sf::Color(30, 40, 50));
                    REQUIRE(pairs["ArrowBackgroundColorHover"].getColor() == sf::Color(40, 50, 60));
                    REQUIRE(pairs["ArrowColorNormal"].getColor() == sf::Color(50, 60, 70));
                    REQUIRE(pairs["ArrowColorHover"].getColor() == sf::Color(60, 70, 80));
                    REQUIRE(pairs["TextColor"].getColor() == sf::Color(70, 80, 90));
                    REQUIRE(pairs["BorderColor"].getColor() == sf::Color(80, 90, 100));
                    REQUIRE(pairs["Borders"].getBorders() == tgui::Borders(1, 2, 3, 4));
                    REQUIRE(pairs["Padding"].getBorders() == tgui::Borders(5, 6, 7, 8));
                }
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(20, 30, 40));
            REQUIRE(renderer->getProperty("ArrowBackgroundColorNormal").getColor() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("ArrowBackgroundColorHover").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("ArrowColorNormal").getColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("ArrowColorHover").getColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("Borders").getBorders() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("Padding").getBorders() == tgui::Borders(5, 6, 7, 8));
        }

        SECTION("textured") {
            tgui::Texture textureBackground("resources/Black.png", {0, 154, 48, 48}, {16, 16, 16, 16});
            tgui::Texture textureArrowUp("resources/Black.png", {60,  0, 32, 32});
            tgui::Texture textureArrowUpHover("resources/Black.png", {60, 32, 32, 32});
            tgui::Texture textureArrowDown("resources/Black.png", {92,  0, 32, 32});
            tgui::Texture textureArrowDownHover("resources/Black.png", {92, 32, 32, 32});

            REQUIRE(!renderer->getProperty("BackgroundImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("ArrowUpImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("ArrowUpHoverImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("ArrowDownImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("ArrowDownHoverImage").getTexture().isLoaded());
            
            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundImage", tgui::Serializer::serialize(textureBackground)));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowUpImage", tgui::Serializer::serialize(textureArrowUp)));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowUpHoverImage", tgui::Serializer::serialize(textureArrowUpHover)));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowDownImage", tgui::Serializer::serialize(textureArrowDown)));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowDownHoverImage", tgui::Serializer::serialize(textureArrowDownHover)));
            }

            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundImage", textureBackground));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowUpImage", textureArrowUp));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowUpHoverImage", textureArrowUpHover));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowDownImage", textureArrowDown));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowDownHoverImage", textureArrowDownHover));
            }

            SECTION("functions") {
                renderer->setBackgroundTexture(textureBackground);
                renderer->setArrowUpTexture(textureArrowUp);
                renderer->setArrowUpHoverTexture(textureArrowUpHover);
                renderer->setArrowDownTexture(textureArrowDown);
                renderer->setArrowDownHoverTexture(textureArrowDownHover);

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 9);
                    REQUIRE(pairs["BackgroundImage"].getTexture().getData() == textureBackground.getData());
                    REQUIRE(pairs["ArrowUpImage"].getTexture().getData() == textureArrowUp.getData());
                    REQUIRE(pairs["ArrowUpHoverImage"].getTexture().getData() == textureArrowUpHover.getData());
                    REQUIRE(pairs["ArrowDownImage"].getTexture().getData() == textureArrowDown.getData());
                    REQUIRE(pairs["ArrowDownHoverImage"].getTexture().getData() == textureArrowDownHover.getData());
                }
            }

            REQUIRE(renderer->getProperty("BackgroundImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("ArrowUpImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("ArrowUpHoverImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("ArrowDownImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("ArrowDownHoverImage").getTexture().isLoaded());

            REQUIRE(renderer->getProperty("BackgroundImage").getTexture().getData() == textureBackground.getData());
            REQUIRE(renderer->getProperty("ArrowUpImage").getTexture().getData() == textureArrowUp.getData());
            REQUIRE(renderer->getProperty("ArrowUpHoverImage").getTexture().getData() == textureArrowUpHover.getData());
            REQUIRE(renderer->getProperty("ArrowDownImage").getTexture().getData() == textureArrowDown.getData());
            REQUIRE(renderer->getProperty("ArrowDownHoverImage").getTexture().getData() == textureArrowDownHover.getData());
        }
    }

    SECTION("Saving and loading from file") {
        REQUIRE_NOTHROW(comboBox = std::make_shared<tgui::Theme>()->load("ComboBox"));

        auto theme = std::make_shared<tgui::Theme>("resources/Black.txt");
        REQUIRE_NOTHROW(comboBox = theme->load("ComboBox"));
        REQUIRE(comboBox->getPrimaryLoadingParameter() == "resources/Black.txt");
        REQUIRE(comboBox->getSecondaryLoadingParameter() == "combobox");

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(comboBox);

        comboBox->setOpacity(0.8f);
        comboBox->addItem("Item 1", "1");
        comboBox->addItem("Item 2");
        comboBox->addItem("Item 3", "3");
        comboBox->setItemsToDisplay(3);
        comboBox->setMaximumItems(5);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileComboBox1.txt"));
        
        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileComboBox1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileComboBox2.txt"));
        REQUIRE(compareFiles("WidgetFileComboBox1.txt", "WidgetFileComboBox2.txt"));

        SECTION("Copying widget") {
            tgui::ComboBox temp;
            temp = *comboBox;

            parent->removeAllWidgets();
            parent->add(tgui::ComboBox::copy(std::make_shared<tgui::ComboBox>(temp)));

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileComboBox2.txt"));
            REQUIRE(compareFiles("WidgetFileComboBox1.txt", "WidgetFileComboBox2.txt"));
        }
    }
}
