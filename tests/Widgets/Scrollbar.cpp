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
#include <TGUI/Widgets/Scrollbar.hpp>

TEST_CASE("[Scrollbar]") {
    tgui::Scrollbar::Ptr scrollbar = std::make_shared<tgui::Scrollbar>();
    scrollbar->setFont("resources/DroidSansArmenian.ttf");
    scrollbar->setLowValue(5);
    scrollbar->setMaximum(20);
    scrollbar->setValue(10);

    SECTION("Signals") {
        REQUIRE_NOTHROW(scrollbar->connect("ValueChanged", [](){}));
        REQUIRE_NOTHROW(scrollbar->connect("ValueChanged", [](int){}));
    }

    SECTION("WidgetType") {
        REQUIRE(scrollbar->getWidgetType() == "Scrollbar");
    }

    SECTION("LowValue") {
        REQUIRE(scrollbar->getLowValue() == 5);

        scrollbar->setLowValue(7);
        REQUIRE(scrollbar->getLowValue() == 7);
        REQUIRE(scrollbar->getValue() == 10);
        REQUIRE(scrollbar->getMaximum() == 20);

        scrollbar->setLowValue(16);
        REQUIRE(scrollbar->getLowValue() == 16);
        REQUIRE(scrollbar->getValue() == 4);
        REQUIRE(scrollbar->getMaximum() == 20);

        scrollbar->setLowValue(22);
        REQUIRE(scrollbar->getLowValue() == 22);
        REQUIRE(scrollbar->getValue() == 0);
        REQUIRE(scrollbar->getMaximum() == 20);
    }

    SECTION("Maximum") {
        REQUIRE(scrollbar->getMaximum() == 20);

        scrollbar->setMaximum(17);
        REQUIRE(scrollbar->getLowValue() == 5);
        REQUIRE(scrollbar->getValue() == 10);
        REQUIRE(scrollbar->getMaximum() == 17);

        scrollbar->setMaximum(12);
        REQUIRE(scrollbar->getLowValue() == 5);
        REQUIRE(scrollbar->getValue() == 7);
        REQUIRE(scrollbar->getMaximum() == 12);

        scrollbar->setMaximum(4);
        REQUIRE(scrollbar->getLowValue() == 5);
        REQUIRE(scrollbar->getValue() == 0);
        REQUIRE(scrollbar->getMaximum() == 4);
    }

    SECTION("Value") {
        REQUIRE(scrollbar->getValue() == 10);
        
        scrollbar->setValue(13);
        REQUIRE(scrollbar->getValue() == 13);
        
        scrollbar->setValue(18);
        REQUIRE(scrollbar->getValue() == 15);
    }

    SECTION("ArrowScrollAmount") {
        REQUIRE(scrollbar->getArrowScrollAmount() == 1);
        scrollbar->setArrowScrollAmount(50);
        REQUIRE(scrollbar->getArrowScrollAmount() == 50);
    }

    SECTION("AutoHide") {
        REQUIRE(scrollbar->getAutoHide());
        scrollbar->setAutoHide(false);
        REQUIRE(!scrollbar->getAutoHide());
        scrollbar->setAutoHide(true);
        REQUIRE(scrollbar->getAutoHide());
    }

    SECTION("Renderer") {
        auto renderer = scrollbar->getRenderer();

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

                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColor", "rgb(30, 40, 50)"));
                REQUIRE(renderer->getProperty("ArrowBackgroundColor").getColor() == sf::Color(30, 40, 50));
                REQUIRE(renderer->getProperty("ArrowBackgroundColorNormal").getColor() == sf::Color(30, 40, 50));
                REQUIRE(renderer->getProperty("ArrowBackgroundColorHover").getColor() == sf::Color(30, 40, 50));
                
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColor", "rgb(40, 50, 60)"));
                REQUIRE(renderer->getProperty("ArrowColor").getColor() == sf::Color(40, 50, 60));
                REQUIRE(renderer->getProperty("ArrowColorNormal").getColor() == sf::Color(40, 50, 60));
                REQUIRE(renderer->getProperty("ArrowColorHover").getColor() == sf::Color(40, 50, 60));

                REQUIRE_NOTHROW(renderer->setProperty("TrackColorNormal", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("TrackColorHover", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColorNormal", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColorHover", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColorNormal", "rgb(90, 100, 110)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColorHover", "rgb(100, 110, 120)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorNormal", "rgb(110, 120, 130)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorHover", "rgb(120, 130, 140)"));
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
                
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColor", sf::Color{30, 40, 50}));
                REQUIRE(renderer->getProperty("ArrowBackgroundColor").getColor() == sf::Color(30, 40, 50));
                REQUIRE(renderer->getProperty("ArrowBackgroundColorNormal").getColor() == sf::Color(30, 40, 50));
                REQUIRE(renderer->getProperty("ArrowBackgroundColorHover").getColor() == sf::Color(30, 40, 50));
                
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColor", sf::Color{40, 50, 60}));
                REQUIRE(renderer->getProperty("ArrowColor").getColor() == sf::Color(40, 50, 60));
                REQUIRE(renderer->getProperty("ArrowColorNormal").getColor() == sf::Color(40, 50, 60));
                REQUIRE(renderer->getProperty("ArrowColorHover").getColor() == sf::Color(40, 50, 60));

                REQUIRE_NOTHROW(renderer->setProperty("TrackColorNormal", sf::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("TrackColorHover", sf::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColorNormal", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColorHover", sf::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColorNormal", sf::Color{90, 100, 110}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColorHover", sf::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorNormal", sf::Color{110, 120, 130}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorHover", sf::Color{120, 130, 140}));
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
                
                renderer->setArrowBackgroundColor({30, 40, 50});
                REQUIRE(renderer->getProperty("ArrowBackgroundColor").getColor() == sf::Color(30, 40, 50));
                REQUIRE(renderer->getProperty("ArrowBackgroundColorNormal").getColor() == sf::Color(30, 40, 50));
                REQUIRE(renderer->getProperty("ArrowBackgroundColorHover").getColor() == sf::Color(30, 40, 50));

                renderer->setArrowColor({40, 50, 60});
                REQUIRE(renderer->getProperty("ArrowColor").getColor() == sf::Color(40, 50, 60));
                REQUIRE(renderer->getProperty("ArrowColorNormal").getColor() == sf::Color(40, 50, 60));
                REQUIRE(renderer->getProperty("ArrowColorHover").getColor() == sf::Color(40, 50, 60));

                renderer->setTrackColorNormal({50, 60, 70});
                renderer->setTrackColorHover({60, 70, 80});
                renderer->setThumbColorNormal({70, 80, 90});
                renderer->setThumbColorHover({80, 90, 100});
                renderer->setArrowBackgroundColorNormal({90, 100, 110});
                renderer->setArrowBackgroundColorHover({100, 110, 120});
                renderer->setArrowColorNormal({110, 120, 130});
                renderer->setArrowColorHover({120, 130, 140});

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 8);
                    REQUIRE(pairs["TrackColorNormal"].getColor() == sf::Color(50, 60, 70));
                    REQUIRE(pairs["TrackColorHover"].getColor() == sf::Color(60, 70, 80));
                    REQUIRE(pairs["ThumbColorNormal"].getColor() == sf::Color(70, 80, 90));
                    REQUIRE(pairs["ThumbColorHover"].getColor() == sf::Color(80, 90, 100));
                    REQUIRE(pairs["ArrowBackgroundColorNormal"].getColor() == sf::Color(90, 100, 110));
                    REQUIRE(pairs["ArrowBackgroundColorHover"].getColor() == sf::Color(100, 110, 120));
                    REQUIRE(pairs["ArrowColorNormal"].getColor() == sf::Color(110, 120, 130));
                    REQUIRE(pairs["ArrowColorHover"].getColor() == sf::Color(120, 130, 140));
                }
            }

            REQUIRE(renderer->getProperty("TrackColorNormal").getColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("TrackColorHover").getColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("ThumbColorNormal").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("ThumbColorHover").getColor() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("ArrowBackgroundColorNormal").getColor() == sf::Color(90, 100, 110));
            REQUIRE(renderer->getProperty("ArrowBackgroundColorHover").getColor() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("ArrowColorNormal").getColor() == sf::Color(110, 120, 130));
            REQUIRE(renderer->getProperty("ArrowColorHover").getColor() == sf::Color(120, 130, 140));
        }

        SECTION("textured") {
            tgui::Texture textureTrackNormal("resources/Black.png", {123, 154, 20, 20});
            tgui::Texture textureTrackHover("resources/Black.png", {123, 174, 20, 20});
            tgui::Texture textureThumbNormal("resources/Black.png", {143, 154, 20, 20});
            tgui::Texture textureThumbHover("resources/Black.png", {143, 174, 20, 20});
            tgui::Texture textureArrowUpNormal("resources/Black.png", {163, 154, 20, 20}, {0, 0, 20, 19});
            tgui::Texture textureArrowUpHover("resources/Black.png", {183, 154, 20, 20}, {0, 0, 20, 19});
            tgui::Texture textureArrowDownNormal("resources/Black.png", {163, 174, 20, 20}, {0, 1, 20, 19});
            tgui::Texture textureArrowDownHover("resources/Black.png", {183, 174, 20, 20}, {0, 1, 20, 19});

            REQUIRE(!renderer->getProperty("TrackImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("TrackHoverImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("ThumbImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("ThumbHoverImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("ArrowUpImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("ArrowUpHoverImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("ArrowDownImage").getTexture().isLoaded());
            REQUIRE(!renderer->getProperty("ArrowDownHoverImage").getTexture().isLoaded());

            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("TrackImage", tgui::Serializer::serialize(textureTrackNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("TrackHoverImage", tgui::Serializer::serialize(textureTrackHover)));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbImage", tgui::Serializer::serialize(textureThumbNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbHoverImage", tgui::Serializer::serialize(textureThumbHover)));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowUpImage", tgui::Serializer::serialize(textureArrowUpNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowUpHoverImage", tgui::Serializer::serialize(textureArrowUpHover)));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowDownImage", tgui::Serializer::serialize(textureArrowDownNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowDownHoverImage", tgui::Serializer::serialize(textureArrowDownHover)));
            }

            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("TrackImage", textureTrackNormal));
                REQUIRE_NOTHROW(renderer->setProperty("TrackHoverImage", textureTrackHover));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbImage", textureThumbNormal));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbHoverImage", textureThumbHover));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowUpImage", textureArrowUpNormal));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowUpHoverImage", textureArrowUpHover));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowDownImage", textureArrowDownNormal));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowDownHoverImage", textureArrowDownHover));
            }

            SECTION("functions") {
                renderer->setTrackTexture(textureTrackNormal);
                renderer->setTrackHoverTexture(textureTrackHover);
                renderer->setThumbTexture(textureThumbNormal);
                renderer->setThumbHoverTexture(textureThumbHover);
                renderer->setArrowUpTexture(textureArrowUpNormal);
                renderer->setArrowUpHoverTexture(textureArrowUpHover);
                renderer->setArrowDownTexture(textureArrowDownNormal);
                renderer->setArrowDownHoverTexture(textureArrowDownHover);

                SECTION("getPropertyValuePairs") {
                    auto pairs = renderer->getPropertyValuePairs();
                    REQUIRE(pairs.size() == 8);
                    REQUIRE(pairs["TrackImage"].getTexture().getData() == textureTrackNormal.getData());
                    REQUIRE(pairs["TrackHoverImage"].getTexture().getData() == textureTrackHover.getData());
                    REQUIRE(pairs["ThumbImage"].getTexture().getData() == textureThumbNormal.getData());
                    REQUIRE(pairs["ThumbHoverImage"].getTexture().getData() == textureThumbHover.getData());
                    REQUIRE(pairs["ArrowUpImage"].getTexture().getData() == textureArrowUpNormal.getData());
                    REQUIRE(pairs["ArrowUpHoverImage"].getTexture().getData() == textureArrowUpHover.getData());
                    REQUIRE(pairs["ArrowDownImage"].getTexture().getData() == textureArrowDownNormal.getData());
                    REQUIRE(pairs["ArrowDownHoverImage"].getTexture().getData() == textureArrowDownHover.getData());
                }
            }

            REQUIRE(renderer->getProperty("TrackImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("TrackHoverImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("ThumbImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("ThumbHoverImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("ArrowUpImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("ArrowUpHoverImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("ArrowDownImage").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("ArrowDownHoverImage").getTexture().isLoaded());

            REQUIRE(renderer->getProperty("TrackImage").getTexture().getData() == textureTrackNormal.getData());
            REQUIRE(renderer->getProperty("TrackHoverImage").getTexture().getData() == textureTrackHover.getData());
            REQUIRE(renderer->getProperty("ThumbImage").getTexture().getData() == textureThumbNormal.getData());
            REQUIRE(renderer->getProperty("ThumbHoverImage").getTexture().getData() == textureThumbHover.getData());
            REQUIRE(renderer->getProperty("ArrowUpImage").getTexture().getData() == textureArrowUpNormal.getData());
            REQUIRE(renderer->getProperty("ArrowUpHoverImage").getTexture().getData() == textureArrowUpHover.getData());
            REQUIRE(renderer->getProperty("ArrowDownImage").getTexture().getData() == textureArrowDownNormal.getData());
            REQUIRE(renderer->getProperty("ArrowDownHoverImage").getTexture().getData() == textureArrowDownHover.getData());
        }
    }

    SECTION("Saving and loading from file") {
        REQUIRE_NOTHROW(scrollbar = std::make_shared<tgui::Theme>()->load("Scrollbar"));

        auto theme = std::make_shared<tgui::Theme>("resources/Black.txt");
        REQUIRE_NOTHROW(scrollbar = theme->load("Scrollbar"));
        REQUIRE(scrollbar->getPrimaryLoadingParameter() == "resources/Black.txt");
        REQUIRE(scrollbar->getSecondaryLoadingParameter() == "scrollbar");

        auto parent = std::make_shared<tgui::GuiContainer>();
        parent->add(scrollbar);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileScrollbar1.txt"));
        
        parent->removeAllWidgets();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile("WidgetFileScrollbar1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileScrollbar2.txt"));
        REQUIRE(compareFiles("WidgetFileScrollbar1.txt", "WidgetFileScrollbar2.txt"));

        SECTION("Copying widget") {
            tgui::Scrollbar temp;
            temp = *scrollbar;

            parent->removeAllWidgets();
            parent->add(tgui::Scrollbar::copy(std::make_shared<tgui::Scrollbar>(temp)));

            REQUIRE_NOTHROW(parent->saveWidgetsToFile("WidgetFileScrollbar2.txt"));
            REQUIRE(compareFiles("WidgetFileScrollbar1.txt", "WidgetFileScrollbar2.txt"));
        }
    }
}
