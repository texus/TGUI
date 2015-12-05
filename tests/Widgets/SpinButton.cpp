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
#include <TGUI/Widgets/SpinButton.hpp>

TEST_CASE("[SpinButton]") {
    tgui::SpinButton::Ptr spinButton = std::make_shared<tgui::SpinButton>();
    spinButton->setFont("resources/DroidSansArmenian.ttf");
    spinButton->setMinimum(10);
    spinButton->setMaximum(20);
    spinButton->setValue(15);

    SECTION("Signals") {
        REQUIRE_NOTHROW(spinButton->connect("ValueChanged", [](){}));
        REQUIRE_NOTHROW(spinButton->connect("ValueChanged", [](int){}));
    }

    SECTION("WidgetType") {
        REQUIRE(spinButton->getWidgetType() == "SpinButton");
    }

    SECTION("Minimum") {
        REQUIRE(spinButton->getMinimum() == 10);

        spinButton->setMinimum(12);
        REQUIRE(spinButton->getMinimum() == 12);
        REQUIRE(spinButton->getValue() == 15);
        REQUIRE(spinButton->getMaximum() == 20);

        spinButton->setMinimum(16);
        REQUIRE(spinButton->getMinimum() == 16);
        REQUIRE(spinButton->getValue() == 16);
        REQUIRE(spinButton->getMaximum() == 20);

        spinButton->setMinimum(22);
        REQUIRE(spinButton->getMinimum() == 22);
        REQUIRE(spinButton->getValue() == 22);
        REQUIRE(spinButton->getMaximum() == 22);
    }

    SECTION("Maximum") {
        REQUIRE(spinButton->getMaximum() == 20);

        spinButton->setMaximum(17);
        REQUIRE(spinButton->getMinimum() == 10);
        REQUIRE(spinButton->getValue() == 15);
        REQUIRE(spinButton->getMaximum() == 17);

        spinButton->setMaximum(11);
        REQUIRE(spinButton->getMinimum() == 10);
        REQUIRE(spinButton->getValue() == 11);
        REQUIRE(spinButton->getMaximum() == 11);

        spinButton->setMaximum(9);
        REQUIRE(spinButton->getMinimum() == 9);
        REQUIRE(spinButton->getValue() == 9);
        REQUIRE(spinButton->getMaximum() == 9);
    }

    SECTION("Value") {
        REQUIRE(spinButton->getValue() == 15);
        
        spinButton->setValue(14);
        REQUIRE(spinButton->getValue() == 14);
        
        spinButton->setValue(7);
        REQUIRE(spinButton->getValue() == 10);
        
        spinButton->setValue(23);
        REQUIRE(spinButton->getValue() == 20);
    }

    SECTION("VerticalScroll") {
        spinButton->setVerticalScroll(false);
        REQUIRE(!spinButton->getVerticalScroll());
        spinButton->setVerticalScroll(true);
        REQUIRE(spinButton->getVerticalScroll());
    }

    SECTION("Renderer") {
        auto renderer = spinButton->getRenderer();

        SECTION("colored") {
            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(10, 20, 30)"));
                REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("BackgroundColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(10, 20, 30));

                REQUIRE_NOTHROW(renderer->setProperty("ArrowColor", "rgb(20, 30, 40)"));
                REQUIRE(renderer->getProperty("ArrowColor").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("ArrowColorNormal").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("ArrowColorHover").getColor() == sf::Color(20, 30, 40));

                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorNormal", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorNormal", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorHover", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("SpaceBetweenArrows", "2"));
            }

            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{10, 20, 30}));
                REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("BackgroundColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(10, 20, 30));
                
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColor", sf::Color{20, 30, 40}));
                REQUIRE(renderer->getProperty("ArrowColor").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("ArrowColorNormal").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("ArrowColorHover").getColor() == sf::Color(20, 30, 40));

                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorNormal", sf::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorNormal", sf::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorHover", sf::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("SpaceBetweenArrows", 2));
            }

            SECTION("functions") {
                renderer->setBackgroundColor({10, 20, 30});
                REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("BackgroundColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(10, 20, 30));

                renderer->setArrowColor({20, 30, 40});
                REQUIRE(renderer->getProperty("ArrowColor").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("ArrowColorNormal").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("ArrowColorHover").getColor() == sf::Color(20, 30, 40));

                renderer->setBackgroundColorNormal({30, 40, 50});
                renderer->setBackgroundColorHover({40, 50, 60});
                renderer->setArrowColorNormal({50, 60, 70});
                renderer->setArrowColorHover({60, 70, 80});
                renderer->setBorderColor({70, 80, 90});
                renderer->setBorders({1, 2, 3, 4});
                renderer->setSpaceBetweenArrows(2);

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 7);
                    REQUIRE(pairs["BackgroundColorNormal"].getColor() == sf::Color(30, 40, 50));
                    REQUIRE(pairs["BackgroundColorHover"].getColor() == sf::Color(40, 50, 60));
                    REQUIRE(pairs["ArrowColorNormal"].getColor() == sf::Color(50, 60, 70));
                    REQUIRE(pairs["ArrowColorHover"].getColor() == sf::Color(60, 70, 80));
                    REQUIRE(pairs["BorderColor"].getColor() == sf::Color(70, 80, 90));
                    REQUIRE(pairs["Borders"].getBorders() == tgui::Borders(1, 2, 3, 4));
                    REQUIRE(pairs["SpaceBetweenArrows"].getNumber() == 2);
                }
            }

            REQUIRE(renderer->getProperty("BackgroundColorNormal").getColor() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("ArrowColorNormal").getColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("ArrowColorHover").getColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("Borders").getBorders() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("SpaceBetweenArrows").getNumber() == 2);
        }

        SECTION("textured") {
            tgui::Texture textureArrowUpNormal("resources/Black.png", {163, 154, 20, 20}, {0, 0, 20, 19});
            tgui::Texture textureArrowUpHover("resources/Black.png", {183, 154, 20, 20}, {0, 0, 20, 19});
            tgui::Texture textureArrowDownNormal("resources/Black.png", {163, 174, 20, 20}, {0, 1, 20, 19});
            tgui::Texture textureArrowDownHover("resources/Black.png", {183, 174, 20, 20}, {0, 1, 20, 19});

            REQUIRE(!renderer->getProperty("ArrowUpImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("ArrowUpHoverImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("ArrowDownImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("ArrowDownHoverImage").getTexture().isLoaded());

            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("ArrowUpImage", tgui::Serializer::serialize(textureArrowUpNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowUpHoverImage", tgui::Serializer::serialize(textureArrowUpHover)));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowDownImage", tgui::Serializer::serialize(textureArrowDownNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowDownHoverImage", tgui::Serializer::serialize(textureArrowDownHover)));
            }

            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("ArrowUpImage", textureArrowUpNormal));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowUpHoverImage", textureArrowUpHover));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowDownImage", textureArrowDownNormal));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowDownHoverImage", textureArrowDownHover));
            }

            SECTION("functions") {
                renderer->setArrowUpTexture(textureArrowUpNormal);
                renderer->setArrowUpHoverTexture(textureArrowUpHover);
                renderer->setArrowDownTexture(textureArrowDownNormal);
                renderer->setArrowDownHoverTexture(textureArrowDownHover);

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 7);
                    REQUIRE(pairs["ArrowUpImage"].getTexture().getData() == textureArrowUpNormal.getData());
                    REQUIRE(pairs["ArrowUpHoverImage"].getTexture().getData() == textureArrowUpHover.getData());
                    REQUIRE(pairs["ArrowDownImage"].getTexture().getData() == textureArrowDownNormal.getData());
                    REQUIRE(pairs["ArrowDownHoverImage"].getTexture().getData() == textureArrowDownHover.getData());
                }
            }

            REQUIRE(renderer->getProperty("ArrowUpImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("ArrowUpHoverImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("ArrowDownImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("ArrowDownHoverImage").getTexture().isLoaded());

            REQUIRE(renderer->getProperty("ArrowUpImage").getTexture().getData() == textureArrowUpNormal.getData());
            REQUIRE(renderer->getProperty("ArrowUpHoverImage").getTexture().getData() == textureArrowUpHover.getData());
            REQUIRE(renderer->getProperty("ArrowDownImage").getTexture().getData() == textureArrowDownNormal.getData());
            REQUIRE(renderer->getProperty("ArrowDownHoverImage").getTexture().getData() == textureArrowDownHover.getData());
        }
    }

    SECTION("Saving and loading from file") {
        REQUIRE_NOTHROW(spinButton = std::make_shared<tgui::Theme>()->load("SpinButton"));

        auto theme = std::make_shared<tgui::Theme>("resources/Black.txt");
        REQUIRE_NOTHROW(spinButton = theme->load("SpinButton"));
        REQUIRE(spinButton->getPrimaryLoadingParameter() == "resources/Black.txt");
        REQUIRE(spinButton->getSecondaryLoadingParameter() == "spinbutton");

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(spinButton);

        spinButton->setOpacity(0.8f);
        spinButton->setMinimum(10);
        spinButton->setMaximum(50);
        spinButton->setValue(20);
        spinButton->setVerticalScroll(false);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileSpinButton1.txt"));
        
        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileSpinButton1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileSpinButton2.txt"));
        REQUIRE(compareFiles("WidgetFileSpinButton1.txt", "WidgetFileSpinButton2.txt"));

        SECTION("Copying widget") {
            tgui::SpinButton temp;
            temp = *spinButton;

            parent->removeAllWidgets();
            parent->add(tgui::SpinButton::copy(std::make_shared<tgui::SpinButton>(temp)));

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileSpinButton2.txt"));
            REQUIRE(compareFiles("WidgetFileSpinButton1.txt", "WidgetFileSpinButton2.txt"));
        }
    }
}
