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
#include <TGUI/Widgets/Knob.hpp>

TEST_CASE("[Knob]") {
    tgui::Knob::Ptr knob = std::make_shared<tgui::Knob>();
    knob->setMinimum(10);
    knob->setMaximum(20);
    knob->setValue(15);

    SECTION("Signals") {
        REQUIRE_NOTHROW(knob->connect("ValueChanged", [](){}));
        REQUIRE_NOTHROW(knob->connect("ValueChanged", [](int){}));
    }

    SECTION("WidgetType") {
        REQUIRE(knob->getWidgetType() == "Knob");
    }

    SECTION("StartRotation") {
        knob->setStartRotation(50);
        REQUIRE(knob->getStartRotation() == 50);
        
        knob->setStartRotation(740);
        REQUIRE(knob->getStartRotation() == 20);
        
        knob->setStartRotation(-30);
        REQUIRE(knob->getStartRotation() == 330);
    }
    
    SECTION("EndRotation") {
        knob->setEndRotation(50);
        REQUIRE(knob->getEndRotation() == 50);
        
        knob->setEndRotation(750);
        REQUIRE(knob->getEndRotation() == 30);
        
        knob->setEndRotation(-25);
        REQUIRE(knob->getEndRotation() == 335);
    }

    SECTION("Minimum") {
        REQUIRE(knob->getMinimum() == 10);

        knob->setMinimum(12);
        REQUIRE(knob->getMinimum() == 12);
        REQUIRE(knob->getValue() == 15);
        REQUIRE(knob->getMaximum() == 20);

        knob->setMinimum(16);
        REQUIRE(knob->getMinimum() == 16);
        REQUIRE(knob->getValue() == 16);
        REQUIRE(knob->getMaximum() == 20);

        knob->setMinimum(22);
        REQUIRE(knob->getMinimum() == 22);
        REQUIRE(knob->getValue() == 22);
        REQUIRE(knob->getMaximum() == 22);
    }

    SECTION("Maximum") {
        REQUIRE(knob->getMaximum() == 20);

        knob->setMaximum(17);
        REQUIRE(knob->getMinimum() == 10);
        REQUIRE(knob->getValue() == 15);
        REQUIRE(knob->getMaximum() == 17);

        knob->setMaximum(11);
        REQUIRE(knob->getMinimum() == 10);
        REQUIRE(knob->getValue() == 11);
        REQUIRE(knob->getMaximum() == 11);

        knob->setMaximum(9);
        REQUIRE(knob->getMinimum() == 9);
        REQUIRE(knob->getValue() == 9);
        REQUIRE(knob->getMaximum() == 9);
    }

    SECTION("Value") {
        REQUIRE(knob->getValue() == 15);
        
        knob->setValue(14);
        REQUIRE(knob->getValue() == 14);
        
        knob->setValue(7);
        REQUIRE(knob->getValue() == 10);
        
        knob->setValue(23);
        REQUIRE(knob->getValue() == 20);
    }

    SECTION("ClockwiseTurning") {
        REQUIRE(knob->getClockwiseTurning());
        knob->setClockwiseTurning(false);
        REQUIRE(!knob->getClockwiseTurning());
        knob->setClockwiseTurning(true);
        REQUIRE(knob->getClockwiseTurning());
    }

    SECTION("Renderer") {
        auto renderer = knob->getRenderer();

        SECTION("colored") {
            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(10, 20, 30)"));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColor", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
            }

            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{10, 20, 30}));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColor", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
            }

            SECTION("functions") {
                renderer->setBackgroundColor({10, 20, 30});
                renderer->setThumbColor({40, 50, 60});
                renderer->setBorderColor({70, 80, 90});
                renderer->setBorders({1, 2, 3, 4});

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 4);
                    REQUIRE(pairs["BackgroundColor"].getColor() == sf::Color(10, 20, 30));
                    REQUIRE(pairs["ThumbColor"].getColor() == sf::Color(40, 50, 60));
                    REQUIRE(pairs["BorderColor"].getColor() == sf::Color(70, 80, 90));
                    REQUIRE(pairs["Borders"].getBorders() == tgui::Borders(1, 2, 3, 4));
                }
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(10, 20, 30));
            REQUIRE(renderer->getProperty("ThumbColor").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("Borders").getBorders() == tgui::Borders(1, 2, 3, 4));
        }

        SECTION("textured") {
            tgui::Texture textureBackground("resources/Knob/Back.png");
            tgui::Texture textureForeground("resources/Knob/Front.png");

            REQUIRE(!renderer->getProperty("BackgroundImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("ForegroundImage").getTexture().isLoaded());

            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundImage", tgui::Serializer::serialize(textureBackground)));
                REQUIRE_NOTHROW(renderer->setProperty("ForegroundImage", tgui::Serializer::serialize(textureForeground)));
                REQUIRE_NOTHROW(renderer->setProperty("ImageRotation", "90"));
            }

            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundImage", textureBackground));
                REQUIRE_NOTHROW(renderer->setProperty("ForegroundImage", textureForeground));
                REQUIRE_NOTHROW(renderer->setProperty("ImageRotation", 90));
            }

            SECTION("functions") {
                renderer->setBackgroundTexture(textureBackground);
                renderer->setForegroundTexture(textureForeground);
                renderer->setImageRotation(90);

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 5);
                    REQUIRE(pairs["BackgroundImage"].getTexture().getData() == textureBackground.getData());
                    REQUIRE(pairs["ForegroundImage"].getTexture().getData() == textureForeground.getData());
                    REQUIRE(pairs["ImageRotation"].getNumber() == 90);
                }
            }

            REQUIRE(renderer->getProperty("BackgroundImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("ForegroundImage").getTexture().isLoaded());

            REQUIRE(renderer->getProperty("BackgroundImage").getTexture().getData() == textureBackground.getData());
            REQUIRE(renderer->getProperty("ForegroundImage").getTexture().getData() == textureForeground.getData());
            REQUIRE(renderer->getProperty("ImageRotation").getNumber() == 90);
        }
    }

    SECTION("Saving and loading from file") {
        REQUIRE_NOTHROW(knob = std::make_shared<tgui::Theme>()->load("Knob"));

        auto theme = std::make_shared<tgui::Theme>("resources/Knob/Black.txt");
        REQUIRE_NOTHROW(knob = theme->load("Knob"));
        REQUIRE(knob->getPrimaryLoadingParameter() == "resources/Knob/Black.txt");
        REQUIRE(knob->getSecondaryLoadingParameter() == "knob");

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(knob);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileKnob1.txt"));
        
        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileKnob1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileKnob2.txt"));
        REQUIRE(compareFiles("WidgetFileKnob1.txt", "WidgetFileKnob2.txt"));
    }
}
