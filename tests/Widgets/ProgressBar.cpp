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
#include <TGUI/Widgets/ProgressBar.hpp>

TEST_CASE("[ProgressBar]") {
    tgui::ProgressBar::Ptr progressBar = std::make_shared<tgui::ProgressBar>();
    progressBar->setFont("resources/DroidSansArmenian.ttf");
    progressBar->setMinimum(10);
    progressBar->setMaximum(20);
    progressBar->setValue(15);

    SECTION("Signals") {
        REQUIRE_NOTHROW(progressBar->connect("ValueChanged", [](){}));
        REQUIRE_NOTHROW(progressBar->connect("Full", [](){}));

        REQUIRE_NOTHROW(progressBar->connect("ValueChanged", [](int){}));
        REQUIRE_NOTHROW(progressBar->connect("Full", [](int){}));
    }

    SECTION("WidgetType") {
        REQUIRE(progressBar->getWidgetType() == "ProgressBar");
    }

    SECTION("Minimum") {
        REQUIRE(progressBar->getMinimum() == 10);

        progressBar->setMinimum(12);
        REQUIRE(progressBar->getMinimum() == 12);
        REQUIRE(progressBar->getValue() == 15);
        REQUIRE(progressBar->getMaximum() == 20);

        progressBar->setMinimum(16);
        REQUIRE(progressBar->getMinimum() == 16);
        REQUIRE(progressBar->getValue() == 16);
        REQUIRE(progressBar->getMaximum() == 20);

        progressBar->setMinimum(22);
        REQUIRE(progressBar->getMinimum() == 22);
        REQUIRE(progressBar->getValue() == 22);
        REQUIRE(progressBar->getMaximum() == 22);
    }

    SECTION("Maximum") {
        REQUIRE(progressBar->getMaximum() == 20);

        progressBar->setMaximum(17);
        REQUIRE(progressBar->getMinimum() == 10);
        REQUIRE(progressBar->getValue() == 15);
        REQUIRE(progressBar->getMaximum() == 17);

        progressBar->setMaximum(11);
        REQUIRE(progressBar->getMinimum() == 10);
        REQUIRE(progressBar->getValue() == 11);
        REQUIRE(progressBar->getMaximum() == 11);

        progressBar->setMaximum(9);
        REQUIRE(progressBar->getMinimum() == 9);
        REQUIRE(progressBar->getValue() == 9);
        REQUIRE(progressBar->getMaximum() == 9);
    }

    SECTION("Value") {
        REQUIRE(progressBar->getValue() == 15);
        
        progressBar->setValue(14);
        REQUIRE(progressBar->getValue() == 14);
        
        progressBar->setValue(7);
        REQUIRE(progressBar->getValue() == 10);
        
        progressBar->setValue(23);
        REQUIRE(progressBar->getValue() == 20);
    }

    SECTION("Value") {
        progressBar->setValue(18);
        REQUIRE(progressBar->getValue() == 18);
        REQUIRE(progressBar->incrementValue() == 19);
        REQUIRE(progressBar->getValue() == 19);
        REQUIRE(progressBar->incrementValue() == 20);
        REQUIRE(progressBar->getValue() == 20);
        REQUIRE(progressBar->incrementValue() == 20);
        REQUIRE(progressBar->getValue() == 20);
    }

    SECTION("Text") {
        REQUIRE(progressBar->getText() == "");
        progressBar->setText("SomeText");
        REQUIRE(progressBar->getText() == "SomeText");
    }

    SECTION("TextSize") {
        progressBar->setTextSize(25);
        REQUIRE(progressBar->getTextSize() == 25);
    }
    
    SECTION("FillDirection") {
        REQUIRE(progressBar->getFillDirection() == tgui::ProgressBar::FillDirection::LeftToRight);
        progressBar->setFillDirection(tgui::ProgressBar::FillDirection::RightToLeft);
        REQUIRE(progressBar->getFillDirection() == tgui::ProgressBar::FillDirection::RightToLeft);
        progressBar->setFillDirection(tgui::ProgressBar::FillDirection::TopToBottom);
        REQUIRE(progressBar->getFillDirection() == tgui::ProgressBar::FillDirection::TopToBottom);
        progressBar->setFillDirection(tgui::ProgressBar::FillDirection::BottomToTop);
        REQUIRE(progressBar->getFillDirection() == tgui::ProgressBar::FillDirection::BottomToTop);
        progressBar->setFillDirection(tgui::ProgressBar::FillDirection::LeftToRight);
        REQUIRE(progressBar->getFillDirection() == tgui::ProgressBar::FillDirection::LeftToRight);
    }

    SECTION("Renderer") {
        auto renderer = progressBar->getRenderer();

        SECTION("colored") {
            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(10, 20, 30)"));
                REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorBack").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorFront").getColor() == sf::Color(10, 20, 30));

                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(20, 30, 40)"));
                REQUIRE_NOTHROW(renderer->setProperty("ForegroundColor", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorBack", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorFront", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
            }

            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{10, 20, 30}));
                REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorBack").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorFront").getColor() == sf::Color(10, 20, 30));
                
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{20, 30, 40}));
                REQUIRE_NOTHROW(renderer->setProperty("ForegroundColor", sf::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorBack", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorFront", sf::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
            }

            SECTION("functions") {
                renderer->setTextColor({10, 20, 30});
                REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorBack").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TextColorFront").getColor() == sf::Color(10, 20, 30));

                renderer->setBackgroundColor({20, 30, 40});
                renderer->setForegroundColor({30, 40, 50});
                renderer->setTextColorBack({40, 50, 60});
                renderer->setTextColorFront({50, 60, 70});
                renderer->setBorderColor({60, 70, 80});
                renderer->setBorders({1, 2, 3, 4});

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 6);
                    REQUIRE(pairs["BackgroundColor"].getColor() == sf::Color(20, 30, 40));
                    REQUIRE(pairs["ForegroundColor"].getColor() == sf::Color(30, 40, 50));
                    REQUIRE(pairs["TextColorBack"].getColor() == sf::Color(40, 50, 60));
                    REQUIRE(pairs["TextColorFront"].getColor() == sf::Color(50, 60, 70));
                    REQUIRE(pairs["BorderColor"].getColor() == sf::Color(60, 70, 80));
                    REQUIRE(pairs["Borders"].getBorders() == tgui::Borders(1, 2, 3, 4));
                }
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(20, 30, 40));
            REQUIRE(renderer->getProperty("ForegroundColor").getColor() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("TextColorBack").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("TextColorFront").getColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("Borders").getBorders() == tgui::Borders(1, 2, 3, 4));
        }

        SECTION("textured") {
            tgui::Texture textureBack("resources/Black.png", {180, 64, 90, 40}, {20, 0, 50, 40});
            tgui::Texture textureFront("resources/Black.png", {180, 108, 82, 32}, {16, 0, 50, 32});

            REQUIRE(!renderer->getProperty("BackImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("FrontImage").getTexture().isLoaded());

            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackImage", tgui::Serializer::serialize(textureBack)));
                REQUIRE_NOTHROW(renderer->setProperty("FrontImage", tgui::Serializer::serialize(textureFront)));
            }

            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackImage", textureBack));
                REQUIRE_NOTHROW(renderer->setProperty("FrontImage", textureFront));
            }

            SECTION("functions") {
                renderer->setBackTexture(textureBack);
                renderer->setFrontTexture(textureFront);

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 6);
                    REQUIRE(pairs["BackImage"].getTexture().getData() == textureBack.getData());
                    REQUIRE(pairs["FrontImage"].getTexture().getData() == textureFront.getData());
                }
            }

            REQUIRE(renderer->getProperty("BackImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("FrontImage").getTexture().isLoaded());

            REQUIRE(renderer->getProperty("BackImage").getTexture().getData() == textureBack.getData());
            REQUIRE(renderer->getProperty("FrontImage").getTexture().getData() == textureFront.getData());
        }
    }

    SECTION("Saving and loading from file") {
        REQUIRE_NOTHROW(progressBar = std::make_shared<tgui::Theme>()->load("ProgressBar"));

        auto theme = std::make_shared<tgui::Theme>("resources/Black.txt");
        REQUIRE_NOTHROW(progressBar = theme->load("ProgressBar"));
        REQUIRE(progressBar->getPrimaryLoadingParameter() == "resources/Black.txt");
        REQUIRE(progressBar->getSecondaryLoadingParameter() == "progressbar");

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(progressBar);

        progressBar->setText("SomeText");
        progressBar->setTextSize(25);
        progressBar->setFillDirection(tgui::ProgressBar::FillDirection::RightToLeft);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileProgressBar1.txt"));
        
        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileProgressBar1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileProgressBar2.txt"));
        REQUIRE(compareFiles("WidgetFileProgressBar1.txt", "WidgetFileProgressBar2.txt"));

        SECTION("Copying widget") {
            tgui::ProgressBar temp;
            temp = *progressBar;

            parent->removeAllWidgets();
            parent->add(tgui::ProgressBar::copy(std::make_shared<tgui::ProgressBar>(temp)));

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileProgressBar2.txt"));
            REQUIRE(compareFiles("WidgetFileProgressBar1.txt", "WidgetFileProgressBar2.txt"));
        }
    }
}
