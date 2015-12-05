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
#include <TGUI/Widgets/RadioButton.hpp>

TEST_CASE("[RadioButton]") {
    tgui::RadioButton::Ptr radioButton = std::make_shared<tgui::RadioButton>();
    radioButton->setFont("resources/DroidSansArmenian.ttf");

    SECTION("Signals") {
        REQUIRE_NOTHROW(radioButton->connect("Checked", [](){}));
        REQUIRE_NOTHROW(radioButton->connect("Unchecked", [](){}));

        REQUIRE_NOTHROW(radioButton->connect("Checked", [](bool){}));
        REQUIRE_NOTHROW(radioButton->connect("Unchecked", [](bool){}));
    }

    SECTION("WidgetType") {
        REQUIRE(radioButton->getWidgetType() == "RadioButton");
    }

    SECTION("Checked") {
        REQUIRE(!radioButton->isChecked());
        radioButton->check();
        REQUIRE(radioButton->isChecked());
        radioButton->uncheck();
        REQUIRE(!radioButton->isChecked());

        // Checking a radio button will uncheck all radio buttons with the same parent
        auto parent1 = std::make_shared<tgui::GuiContainer>();
        auto parent2 = std::make_shared<tgui::GuiContainer>();
        auto radioButton1 = std::make_shared<tgui::RadioButton>();
        auto radioButton2 = std::make_shared<tgui::RadioButton>();
        auto radioButton3 = std::make_shared<tgui::RadioButton>();
        parent1->add(radioButton1);
        parent1->add(radioButton2);
        parent2->add(radioButton3);
        parent2->add(parent1);
        REQUIRE(!radioButton1->isChecked());
        REQUIRE(!radioButton2->isChecked());
        REQUIRE(!radioButton3->isChecked());
        radioButton1->check();
        REQUIRE(radioButton1->isChecked());
        REQUIRE(!radioButton2->isChecked());
        REQUIRE(!radioButton3->isChecked());
        radioButton2->check();
        REQUIRE(!radioButton1->isChecked());
        REQUIRE(radioButton2->isChecked());
        REQUIRE(!radioButton3->isChecked());
        radioButton3->check();
        REQUIRE(!radioButton1->isChecked());
        REQUIRE(radioButton2->isChecked());
        REQUIRE(radioButton3->isChecked());

        parent1->uncheckRadioButtons();
        REQUIRE(!radioButton1->isChecked());
        REQUIRE(!radioButton2->isChecked());
        REQUIRE(radioButton3->isChecked());
    }
    
    SECTION("Text") {
        REQUIRE(radioButton->getText() == "");
        radioButton->setText("SomeText");
        REQUIRE(radioButton->getText() == "SomeText");
    }

    SECTION("TextSize") {
        radioButton->setTextSize(25);
        REQUIRE(radioButton->getTextSize() == 25);
    }

    SECTION("Renderer") {
        auto renderer = radioButton->getRenderer();

        SECTION("colored") {
            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(10, 20, 30)"));
                REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorHover").getColor() == sf::Color(10, 20, 30));

                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(20, 30, 40)"));
                REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("BackgroundColorNormal").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(20, 30, 40));

                REQUIRE_NOTHROW(renderer->setProperty("ForegroundColor", "rgb(30, 40, 50)"));
                REQUIRE(renderer->getProperty("ForegroundColor").getColor() == sf::Color(30, 40, 50));
                REQUIRE(renderer->getProperty("ForegroundColorNormal").getColor() == sf::Color(30, 40, 50));
                REQUIRE(renderer->getProperty("ForegroundColorHover").getColor() == sf::Color(30, 40, 50));

                REQUIRE_NOTHROW(renderer->setProperty("CheckColor", "rgb(40, 50, 60)"));
                REQUIRE(renderer->getProperty("CheckColor").getColor() == sf::Color(40, 50, 60));
                REQUIRE(renderer->getProperty("CheckColorNormal").getColor() == sf::Color(40, 50, 60));
                REQUIRE(renderer->getProperty("CheckColorHover").getColor() == sf::Color(40, 50, 60));

                REQUIRE_NOTHROW(renderer->setProperty("TextColorNormal", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorNormal", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("ForegroundColorNormal", "rgb(90, 100, 110)"));
                REQUIRE_NOTHROW(renderer->setProperty("ForegroundColorHover", "rgb(100, 110, 120)"));
                REQUIRE_NOTHROW(renderer->setProperty("CheckColorNormal", "rgb(110, 120, 130)"));
                REQUIRE_NOTHROW(renderer->setProperty("CheckColorHover", "rgb(120, 130, 140)"));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", "(1, 2, 3, 4)"));
            }

            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{10, 20, 30}));
                REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorHover").getColor() == sf::Color(10, 20, 30));

                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{20, 30, 40}));
                REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("BackgroundColorNormal").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(20, 30, 40));

                REQUIRE_NOTHROW(renderer->setProperty("ForegroundColor", sf::Color{30, 40, 50}));
                REQUIRE(renderer->getProperty("ForegroundColor").getColor() == sf::Color(30, 40, 50));
                REQUIRE(renderer->getProperty("ForegroundColorNormal").getColor() == sf::Color(30, 40, 50));
                REQUIRE(renderer->getProperty("ForegroundColorHover").getColor() == sf::Color(30, 40, 50));

                REQUIRE_NOTHROW(renderer->setProperty("CheckColor", sf::Color{40, 50, 60}));
                REQUIRE(renderer->getProperty("CheckColor").getColor() == sf::Color(40, 50, 60));
                REQUIRE(renderer->getProperty("CheckColorNormal").getColor() == sf::Color(40, 50, 60));
                REQUIRE(renderer->getProperty("CheckColorHover").getColor() == sf::Color(40, 50, 60));

                REQUIRE_NOTHROW(renderer->setProperty("TextColorNormal", sf::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", sf::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorNormal", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", sf::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("ForegroundColorNormal", sf::Color{90, 100, 110}));
                REQUIRE_NOTHROW(renderer->setProperty("ForegroundColorHover", sf::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("CheckColorNormal", sf::Color{110, 120, 130}));
                REQUIRE_NOTHROW(renderer->setProperty("CheckColorHover", sf::Color{120, 130, 140}));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Borders{1, 2, 3, 4}));
            }

            SECTION("functions") {
                renderer->setTextColor({10, 20, 30});
                REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorHover").getColor() == sf::Color(10, 20, 30));

                renderer->setBackgroundColor({20, 30, 40});
                REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("BackgroundColorNormal").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(20, 30, 40));

                renderer->setForegroundColor({30, 40, 50});
                REQUIRE(renderer->getProperty("ForegroundColor").getColor() == sf::Color(30, 40, 50));
                REQUIRE(renderer->getProperty("ForegroundColorNormal").getColor() == sf::Color(30, 40, 50));
                REQUIRE(renderer->getProperty("ForegroundColorHover").getColor() == sf::Color(30, 40, 50));

                renderer->setCheckColor({40, 50, 60});
                REQUIRE(renderer->getProperty("CheckColor").getColor() == sf::Color(40, 50, 60));
                REQUIRE(renderer->getProperty("CheckColorNormal").getColor() == sf::Color(40, 50, 60));
                REQUIRE(renderer->getProperty("CheckColorHover").getColor() == sf::Color(40, 50, 60));

                renderer->setTextColorNormal({50, 60, 70});
                renderer->setTextColorHover({60, 70, 80});
                renderer->setBackgroundColorNormal({70, 80, 90});
                renderer->setBackgroundColorHover({80, 90, 100});
                renderer->setForegroundColorNormal({90, 100, 110});
                renderer->setForegroundColorHover({100, 110, 120});
                renderer->setCheckColorNormal({110, 120, 130});
                renderer->setCheckColorHover({120, 130, 140});
                renderer->setPadding({1, 2, 3, 4});

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 9);
                    REQUIRE(pairs["TextColorNormal"].getColor() == sf::Color(50, 60, 70));
                    REQUIRE(pairs["TextColorHover"].getColor() == sf::Color(60, 70, 80));
                    REQUIRE(pairs["BackgroundColorNormal"].getColor() == sf::Color(70, 80, 90));
                    REQUIRE(pairs["BackgroundColorHover"].getColor() == sf::Color(80, 90, 100));
                    REQUIRE(pairs["ForegroundColorNormal"].getColor() == sf::Color(90, 100, 110));
                    REQUIRE(pairs["ForegroundColorHover"].getColor() == sf::Color(100, 110, 120));
                    REQUIRE(pairs["CheckColorNormal"].getColor() == sf::Color(110, 120, 130));
                    REQUIRE(pairs["CheckColorHover"].getColor() == sf::Color(120, 130, 140));
                    REQUIRE(pairs["Padding"].getBorders() == tgui::Borders(1, 2, 3, 4));
                }
            }

            REQUIRE(renderer->getProperty("TextColorNormal").getColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("TextColorHover").getColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("BackgroundColorNormal").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("ForegroundColorNormal").getColor() == sf::Color(90, 100, 110));
            REQUIRE(renderer->getProperty("ForegroundColorHover").getColor() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("CheckColorNormal").getColor() == sf::Color(110, 120, 130));
            REQUIRE(renderer->getProperty("CheckColorHover").getColor() == sf::Color(120, 130, 140));
            REQUIRE(renderer->getProperty("Padding").getBorders() == tgui::Borders(1, 2, 3, 4));
        }

        SECTION("textured") {
            tgui::Texture textureUncheckedNormal("resources/Black.png", {124, 32, 32, 32});
            tgui::Texture textureUncheckedHover("resources/Black.png", {188, 32, 32, 32});
            tgui::Texture textureCheckedNormal("resources/Black.png", {156, 32, 32, 32});
            tgui::Texture textureCheckedHover("resources/Black.png", {220, 32, 32, 32});

            REQUIRE(!renderer->getProperty("UncheckedImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("UncheckedHoverImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("CheckedImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("CheckedHoverImage").getTexture().isLoaded());

            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("UncheckedImage", tgui::Serializer::serialize(textureUncheckedNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("UncheckedHoverImage", tgui::Serializer::serialize(textureUncheckedHover)));
                REQUIRE_NOTHROW(renderer->setProperty("CheckedImage", tgui::Serializer::serialize(textureCheckedNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("CheckedHoverImage", tgui::Serializer::serialize(textureCheckedHover)));
            }

            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("UncheckedImage", textureUncheckedNormal));
                REQUIRE_NOTHROW(renderer->setProperty("UncheckedHoverImage", textureUncheckedHover));
                REQUIRE_NOTHROW(renderer->setProperty("CheckedImage", textureCheckedNormal));
                REQUIRE_NOTHROW(renderer->setProperty("CheckedHoverImage", textureCheckedHover));
            }

            SECTION("functions") {
                renderer->setUncheckedTexture(textureUncheckedNormal);
                renderer->setUncheckedHoverTexture(textureUncheckedHover);
                renderer->setCheckedTexture(textureCheckedNormal);
                renderer->setCheckedHoverTexture(textureCheckedHover);
                renderer->setFocusedTexture({});

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 7);
                    REQUIRE(pairs["UncheckedImage"].getTexture().getData() == textureUncheckedNormal.getData());
                    REQUIRE(pairs["UncheckedHoverImage"].getTexture().getData() == textureUncheckedHover.getData());
                    REQUIRE(pairs["CheckedImage"].getTexture().getData() == textureCheckedNormal.getData());
                    REQUIRE(pairs["CheckedHoverImage"].getTexture().getData() == textureCheckedHover.getData());
                }
            }

            REQUIRE(renderer->getProperty("UncheckedImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("UncheckedHoverImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("CheckedImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("CheckedHoverImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("FocusedImage").getTexture().isLoaded());

            REQUIRE(renderer->getProperty("UncheckedImage").getTexture().getData() == textureUncheckedNormal.getData());
            REQUIRE(renderer->getProperty("UncheckedHoverImage").getTexture().getData() == textureUncheckedHover.getData());
            REQUIRE(renderer->getProperty("CheckedImage").getTexture().getData() == textureCheckedNormal.getData());
            REQUIRE(renderer->getProperty("CheckedHoverImage").getTexture().getData() == textureCheckedHover.getData());
        }
    }

    SECTION("Saving and loading from file") {
        REQUIRE_NOTHROW(radioButton = std::make_shared<tgui::Theme>()->load("RadioButton"));

        auto theme = std::make_shared<tgui::Theme>("resources/Black.txt");
        REQUIRE_NOTHROW(radioButton = theme->load("RadioButton"));
        REQUIRE(radioButton->getPrimaryLoadingParameter() == "resources/Black.txt");
        REQUIRE(radioButton->getSecondaryLoadingParameter() == "radiobutton");

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(radioButton);

        radioButton->setOpacity(0.8f);
        radioButton->check();
        radioButton->setText("SomeText");
        radioButton->setTextSize(25);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileRadioButton1.txt"));
        
        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileRadioButton1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileRadioButton2.txt"));
        REQUIRE(compareFiles("WidgetFileRadioButton1.txt", "WidgetFileRadioButton2.txt"));

        SECTION("Copying widget") {
            tgui::RadioButton temp;
            temp = *radioButton;

            parent->removeAllWidgets();
            parent->add(tgui::RadioButton::copy(std::make_shared<tgui::RadioButton>(temp)));

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileRadioButton2.txt"));
            REQUIRE(compareFiles("WidgetFileRadioButton1.txt", "WidgetFileRadioButton2.txt"));
        }
    }
}
