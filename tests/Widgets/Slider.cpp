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
#include <TGUI/Widgets/Slider.hpp>

TEST_CASE("[Slider]") {
    tgui::Slider::Ptr slider = std::make_shared<tgui::Slider>();
    slider->setMinimum(10);
    slider->setMaximum(20);
    slider->setValue(15);

    SECTION("Signals") {
        REQUIRE_NOTHROW(slider->connect("ValueChanged", [](){}));
        REQUIRE_NOTHROW(slider->connect("ValueChanged", [](int){}));
    }

    SECTION("WidgetType") {
        REQUIRE(slider->getWidgetType() == "Slider");
    }

    SECTION("Minimum") {
        REQUIRE(slider->getMinimum() == 10);

        slider->setMinimum(12);
        REQUIRE(slider->getMinimum() == 12);
        REQUIRE(slider->getValue() == 15);
        REQUIRE(slider->getMaximum() == 20);

        slider->setMinimum(16);
        REQUIRE(slider->getMinimum() == 16);
        REQUIRE(slider->getValue() == 16);
        REQUIRE(slider->getMaximum() == 20);

        slider->setMinimum(22);
        REQUIRE(slider->getMinimum() == 22);
        REQUIRE(slider->getValue() == 22);
        REQUIRE(slider->getMaximum() == 22);
    }

    SECTION("Maximum") {
        REQUIRE(slider->getMaximum() == 20);

        slider->setMaximum(17);
        REQUIRE(slider->getMinimum() == 10);
        REQUIRE(slider->getValue() == 15);
        REQUIRE(slider->getMaximum() == 17);

        slider->setMaximum(11);
        REQUIRE(slider->getMinimum() == 10);
        REQUIRE(slider->getValue() == 11);
        REQUIRE(slider->getMaximum() == 11);

        slider->setMaximum(9);
        REQUIRE(slider->getMinimum() == 9);
        REQUIRE(slider->getValue() == 9);
        REQUIRE(slider->getMaximum() == 9);
    }

    SECTION("Value") {
        REQUIRE(slider->getValue() == 15);
        
        slider->setValue(14);
        REQUIRE(slider->getValue() == 14);
        
        slider->setValue(7);
        REQUIRE(slider->getValue() == 10);
        
        slider->setValue(23);
        REQUIRE(slider->getValue() == 20);
    }

    SECTION("Renderer") {
        auto renderer = slider->getRenderer();

        SECTION("colored") {
            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("TrackColor", "rgb(10, 20, 30)"));
                REQUIRE(renderer->getProperty("TrackColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TrackColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TrackColorHover").getColor() == sf::Color(10, 20, 30));

                REQUIRE_NOTHROW(renderer->setProperty("ThumbColor", "rgb(20, 30, 40)"));
                REQUIRE(renderer->getProperty("ThumbColor").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("ThumbColorNormal").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("ThumbColorHover").getColor() == sf::Color(20, 30, 40));

                REQUIRE_NOTHROW(renderer->setProperty("TrackColorNormal", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("TrackColorHover", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColorNormal", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColorHover", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
            }

            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("TrackColor", sf::Color{10, 20, 30}));
                REQUIRE(renderer->getProperty("TrackColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TrackColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TrackColorHover").getColor() == sf::Color(10, 20, 30));
                
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColor", sf::Color{20, 30, 40}));
                REQUIRE(renderer->getProperty("ThumbColor").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("ThumbColorNormal").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("ThumbColorHover").getColor() == sf::Color(20, 30, 40));

                REQUIRE_NOTHROW(renderer->setProperty("TrackColorNormal", sf::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("TrackColorHover", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColorNormal", sf::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColorHover", sf::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
            }

            SECTION("functions") {
                renderer->setTrackColor({10, 20, 30});
                REQUIRE(renderer->getProperty("TrackColor").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TrackColorNormal").getColor() == sf::Color(10, 20, 30));
                REQUIRE(renderer->getProperty("TrackColorHover").getColor() == sf::Color(10, 20, 30));

                renderer->setThumbColor({20, 30, 40});
                REQUIRE(renderer->getProperty("ThumbColor").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("ThumbColorNormal").getColor() == sf::Color(20, 30, 40));
                REQUIRE(renderer->getProperty("ThumbColorHover").getColor() == sf::Color(20, 30, 40));

                renderer->setTrackColorNormal({30, 40, 50});
                renderer->setTrackColorHover({40, 50, 60});
                renderer->setThumbColorNormal({50, 60, 70});
                renderer->setThumbColorHover({60, 70, 80});
                renderer->setBorderColor({70, 80, 90});
                renderer->setBorders({1, 2, 3, 4});

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 6);
                    REQUIRE(pairs["TrackColorNormal"].getColor() == sf::Color(30, 40, 50));
                    REQUIRE(pairs["TrackColorHover"].getColor() == sf::Color(40, 50, 60));
                    REQUIRE(pairs["ThumbColorNormal"].getColor() == sf::Color(50, 60, 70));
                    REQUIRE(pairs["ThumbColorHover"].getColor() == sf::Color(60, 70, 80));
                    REQUIRE(pairs["BorderColor"].getColor() == sf::Color(70, 80, 90));
                    REQUIRE(pairs["Borders"].getBorders() == tgui::Borders(1, 2, 3, 4));
                }
            }

            REQUIRE(renderer->getProperty("TrackColorNormal").getColor() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("TrackColorHover").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("ThumbColorNormal").getColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("ThumbColorHover").getColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("Borders").getBorders() == tgui::Borders(1, 2, 3, 4));
        }

        SECTION("textured") {
            tgui::Texture textureTrackNormal("resources/Black.png", {203, 150, 20, 45}, {0, 15, 20, 15});
            tgui::Texture textureTrackHover("resources/Black.png", {223, 150, 20, 45}, {0, 15, 20, 15});
            tgui::Texture textureThumbNormal("resources/Black.png", {243, 150, 30, 30});
            tgui::Texture textureThumbHover("resources/Black.png", {243, 150, 30, 30});

            REQUIRE(!renderer->getProperty("TrackImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("TrackHoverImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("ThumbImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("ThumbHoverImage").getTexture().isLoaded());

            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("TrackImage", tgui::Serializer::serialize(textureTrackNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("TrackHoverImage", tgui::Serializer::serialize(textureTrackHover)));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbImage", tgui::Serializer::serialize(textureThumbNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbHoverImage", tgui::Serializer::serialize(textureThumbHover)));
            }

            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("TrackImage", textureTrackNormal));
                REQUIRE_NOTHROW(renderer->setProperty("TrackHoverImage", textureTrackHover));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbImage", textureThumbNormal));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbHoverImage", textureThumbHover));
            }

            SECTION("functions") {
                renderer->setTrackTexture(textureTrackNormal);
                renderer->setTrackHoverTexture(textureTrackHover);
                renderer->setThumbTexture(textureThumbNormal);
                renderer->setThumbHoverTexture(textureThumbHover);

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 6);
                    REQUIRE(pairs["TrackImage"].getTexture().getData() == textureTrackNormal.getData());
                    REQUIRE(pairs["TrackHoverImage"].getTexture().getData() == textureTrackHover.getData());
                    REQUIRE(pairs["ThumbImage"].getTexture().getData() == textureThumbNormal.getData());
                    REQUIRE(pairs["ThumbHoverImage"].getTexture().getData() == textureThumbHover.getData());
                }
            }

            REQUIRE(renderer->getProperty("TrackImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("TrackHoverImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("ThumbImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("ThumbHoverImage").getTexture().isLoaded());

            REQUIRE(renderer->getProperty("TrackImage").getTexture().getData() == textureTrackNormal.getData());
            REQUIRE(renderer->getProperty("TrackHoverImage").getTexture().getData() == textureTrackHover.getData());
            REQUIRE(renderer->getProperty("ThumbImage").getTexture().getData() == textureThumbNormal.getData());
            REQUIRE(renderer->getProperty("ThumbHoverImage").getTexture().getData() == textureThumbHover.getData());
        }
    }

    SECTION("Saving and loading from file") {
        REQUIRE_NOTHROW(slider = std::make_shared<tgui::Theme>()->load("Slider"));

        auto theme = std::make_shared<tgui::Theme>("resources/Black.txt");
        REQUIRE_NOTHROW(slider = theme->load("Slider"));

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(slider);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileSlider1.txt"));
        
        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileSlider1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileSlider2.txt"));
        REQUIRE(compareFiles("WidgetFileSlider1.txt", "WidgetFileSlider2.txt"));
    }
}
