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
#include <TGUI/Widgets/Scrollbar.hpp>

TEST_CASE("[Scrollbar]")
{
    tgui::Scrollbar::Ptr scrollbar = tgui::Scrollbar::create();
    scrollbar->getRenderer()->setFont("resources/DejaVuSans.ttf");
    scrollbar->setMaximum(20);
    scrollbar->setViewportSize(5);
    scrollbar->setValue(10);

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(scrollbar->connect("ValueChanged", [](){}));
        REQUIRE_NOTHROW(scrollbar->connect("ValueChanged", [](unsigned int){}));
        REQUIRE_NOTHROW(scrollbar->connect("ValueChanged", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(scrollbar->connect("ValueChanged", [](tgui::Widget::Ptr, std::string, unsigned int){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(scrollbar->getWidgetType() == "Scrollbar");
    }

    SECTION("Position and Size")
    {
        scrollbar->setPosition(40, 30);
        scrollbar->setSize(150, 25);

        REQUIRE(scrollbar->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(scrollbar->getSize() == sf::Vector2f(150, 25));
        REQUIRE(scrollbar->getFullSize() == scrollbar->getSize());
        REQUIRE(scrollbar->getWidgetOffset() == sf::Vector2f(0, 0));
    }

    SECTION("ViewportSize")
    {
        REQUIRE(scrollbar->getViewportSize() == 5);

        scrollbar->setViewportSize(7);
        REQUIRE(scrollbar->getViewportSize() == 7);
        REQUIRE(scrollbar->getValue() == 10);
        REQUIRE(scrollbar->getMaximum() == 20);

        scrollbar->setViewportSize(16);
        REQUIRE(scrollbar->getViewportSize() == 16);
        REQUIRE(scrollbar->getValue() == 4);
        REQUIRE(scrollbar->getMaximum() == 20);

        scrollbar->setViewportSize(22);
        REQUIRE(scrollbar->getViewportSize() == 22);
        REQUIRE(scrollbar->getValue() == 0);
        REQUIRE(scrollbar->getMaximum() == 20);
    }

    SECTION("Maximum")
    {
        REQUIRE(scrollbar->getMaximum() == 20);

        scrollbar->setMaximum(17);
        REQUIRE(scrollbar->getViewportSize() == 5);
        REQUIRE(scrollbar->getValue() == 10);
        REQUIRE(scrollbar->getMaximum() == 17);

        scrollbar->setMaximum(12);
        REQUIRE(scrollbar->getViewportSize() == 5);
        REQUIRE(scrollbar->getValue() == 7);
        REQUIRE(scrollbar->getMaximum() == 12);

        scrollbar->setMaximum(4);
        REQUIRE(scrollbar->getViewportSize() == 5);
        REQUIRE(scrollbar->getValue() == 0);
        REQUIRE(scrollbar->getMaximum() == 4);
    }

    SECTION("Value")
    {
        REQUIRE(scrollbar->getValue() == 10);
        
        scrollbar->setValue(13);
        REQUIRE(scrollbar->getValue() == 13);
        
        scrollbar->setValue(18);
        REQUIRE(scrollbar->getValue() == 15);
    }

    SECTION("ScrollAmount")
    {
        REQUIRE(scrollbar->getScrollAmount() == 1);
        scrollbar->setScrollAmount(50);
        REQUIRE(scrollbar->getScrollAmount() == 50);
    }

    SECTION("AutoHide")
    {
        REQUIRE(scrollbar->getAutoHide());
        scrollbar->setAutoHide(false);
        REQUIRE(!scrollbar->getAutoHide());
        scrollbar->setAutoHide(true);
        REQUIRE(scrollbar->getAutoHide());
    }

    SECTION("Events / Signals")
    {
        SECTION("Widget")
        {
            testWidgetSignals(scrollbar);
        }

        // TODO
    }

    testWidgetRenderer(scrollbar->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = scrollbar->getRenderer();

        SECTION("colored")
        {
            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TrackColor", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("TrackColorHover", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColorHover", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColor", "rgb(90, 100, 110)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColorHover", "rgb(100, 110, 120)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColor", "rgb(110, 120, 130)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorHover", "rgb(120, 130, 140)"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TrackColor", sf::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("TrackColorHover", sf::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColor", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColorHover", sf::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColor", sf::Color{90, 100, 110}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColorHover", sf::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColor", sf::Color{110, 120, 130}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorHover", sf::Color{120, 130, 140}));
            }

            SECTION("functions")
            {
                renderer->setTrackColor({50, 60, 70});
                renderer->setTrackColorHover({60, 70, 80});
                renderer->setThumbColor({70, 80, 90});
                renderer->setThumbColorHover({80, 90, 100});
                renderer->setArrowBackgroundColor({90, 100, 110});
                renderer->setArrowBackgroundColorHover({100, 110, 120});
                renderer->setArrowColor({110, 120, 130});
                renderer->setArrowColorHover({120, 130, 140});
            }

            REQUIRE(renderer->getProperty("TrackColor").getColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("TrackColorHover").getColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("ThumbColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("ThumbColorHover").getColor() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("ArrowBackgroundColor").getColor() == sf::Color(90, 100, 110));
            REQUIRE(renderer->getProperty("ArrowBackgroundColorHover").getColor() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("ArrowColor").getColor() == sf::Color(110, 120, 130));
            REQUIRE(renderer->getProperty("ArrowColorHover").getColor() == sf::Color(120, 130, 140));

            REQUIRE(renderer->getTrackColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getTrackColorHover() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getThumbColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getThumbColorHover() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getArrowBackgroundColor() == sf::Color(90, 100, 110));
            REQUIRE(renderer->getArrowBackgroundColorHover() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getArrowColor() == sf::Color(110, 120, 130));
            REQUIRE(renderer->getArrowColorHover() == sf::Color(120, 130, 140));
        }

        SECTION("textured")
        {
            tgui::Texture textureTrackNormal("resources/Black.png", {123, 154, 20, 20});
            tgui::Texture textureTrackHover("resources/Black.png", {123, 174, 20, 20});
            tgui::Texture textureThumbNormal("resources/Black.png", {143, 154, 20, 20});
            tgui::Texture textureThumbHover("resources/Black.png", {143, 174, 20, 20});
            tgui::Texture textureArrowUpNormal("resources/Black.png", {163, 154, 20, 20}, {0, 0, 20, 19});
            tgui::Texture textureArrowUpHover("resources/Black.png", {183, 154, 20, 20}, {0, 0, 20, 19});
            tgui::Texture textureArrowDownNormal("resources/Black.png", {163, 174, 20, 20}, {0, 1, 20, 19});
            tgui::Texture textureArrowDownHover("resources/Black.png", {183, 174, 20, 20}, {0, 1, 20, 19});

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureTrack", tgui::Serializer::serialize(textureTrackNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureTrackHover", tgui::Serializer::serialize(textureTrackHover)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureThumb", tgui::Serializer::serialize(textureThumbNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureThumbHover", tgui::Serializer::serialize(textureThumbHover)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowUp", tgui::Serializer::serialize(textureArrowUpNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowUpHover", tgui::Serializer::serialize(textureArrowUpHover)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowDown", tgui::Serializer::serialize(textureArrowDownNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowDownHover", tgui::Serializer::serialize(textureArrowDownHover)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureTrack", textureTrackNormal));
                REQUIRE_NOTHROW(renderer->setProperty("TextureTrackHover", textureTrackHover));
                REQUIRE_NOTHROW(renderer->setProperty("TextureThumb", textureThumbNormal));
                REQUIRE_NOTHROW(renderer->setProperty("TextureThumbHover", textureThumbHover));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowUp", textureArrowUpNormal));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowUpHover", textureArrowUpHover));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowDown", textureArrowDownNormal));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowDownHover", textureArrowDownHover));
            }

            SECTION("functions")
            {
                renderer->setTextureTrack(textureTrackNormal);
                renderer->setTextureTrackHover(textureTrackHover);
                renderer->setTextureThumb(textureThumbNormal);
                renderer->setTextureThumbHover(textureThumbHover);
                renderer->setTextureArrowUp(textureArrowUpNormal);
                renderer->setTextureArrowUpHover(textureArrowUpHover);
                renderer->setTextureArrowDown(textureArrowDownNormal);
                renderer->setTextureArrowDownHover(textureArrowDownHover);
            }

            REQUIRE(renderer->getProperty("TextureTrack").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureTrackHover").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureThumb").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureThumbHover").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureArrowUp").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureArrowUpHover").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureArrowDown").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureArrowDownHover").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTextureTrack().getData() == textureTrackNormal.getData());
            REQUIRE(renderer->getTextureTrackHover().getData() == textureTrackHover.getData());
            REQUIRE(renderer->getTextureThumb().getData() == textureThumbNormal.getData());
            REQUIRE(renderer->getTextureThumbHover().getData() == textureThumbHover.getData());
            REQUIRE(renderer->getTextureArrowUp().getData() == textureArrowUpNormal.getData());
            REQUIRE(renderer->getTextureArrowUpHover().getData() == textureArrowUpHover.getData());
            REQUIRE(renderer->getTextureArrowDown().getData() == textureArrowDownNormal.getData());
            REQUIRE(renderer->getTextureArrowDownHover().getData() == textureArrowDownHover.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        scrollbar->setMaximum(50);
        scrollbar->setViewportSize(10);
        scrollbar->setValue(20);
        scrollbar->setScrollAmount(5);
        scrollbar->setAutoHide(false);

        testSavingWidget("Scrollbar", scrollbar);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(120, 60, scrollbar)

        scrollbar->setEnabled(true);
        scrollbar->setPosition({10, 15});
        scrollbar->setSize({100, 30});
        scrollbar->setViewportSize(120);
        scrollbar->setMaximum(200);
        scrollbar->setValue(30);

        tgui::ScrollbarRenderer renderer = tgui::RendererData::create();
        renderer.setTrackColor(sf::Color::Green);
        renderer.setThumbColor(sf::Color::Red);
        renderer.setArrowBackgroundColor(sf::Color::Blue);
        renderer.setArrowColor(sf::Color::White);
        renderer.setOpacity(0.7f);
        scrollbar->setRenderer(renderer.getData());

        auto setHoverRenderer = [&](bool textured){
                                        renderer.setTrackColorHover(sf::Color::Cyan);
                                        renderer.setThumbColorHover(sf::Color::Magenta);
                                        renderer.setArrowBackgroundColorHover(sf::Color::Yellow);
                                        renderer.setArrowColorHover(sf::Color::Black);
                                        if (textured)
                                        {
                                            renderer.setTextureTrackHover("resources/Texture5.png");
                                            renderer.setTextureThumbHover("resources/Texture6.png");
                                            renderer.setTextureArrowUpHover("resources/Texture7.png");
                                            renderer.setTextureArrowDownHover("resources/Texture8.png");
                                        }
                                     };

        auto mousePos = scrollbar->getPosition();

        SECTION("Colored")
        {
            SECTION("NormalState")
            {
                TEST_DRAW("Scrollbar_Normal_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("Scrollbar_Normal_HoverSet.png")
                }
            }

            SECTION("HoverState")
            {
                scrollbar->mouseMoved(mousePos);

                TEST_DRAW("Scrollbar_Hover_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("Scrollbar_HoverArrowLeft_HoverSet.png")

                    scrollbar->mouseMoved({mousePos.x + 32, mousePos.y});
                    TEST_DRAW("Scrollbar_HoverTrackLeft_HoverSet.png");

                    scrollbar->mouseMoved({mousePos.x + 48, mousePos.y});
                    TEST_DRAW("Scrollbar_HoverThumb_HoverSet.png");

                    scrollbar->mouseMoved({mousePos.x + 65, mousePos.y});
                    TEST_DRAW("Scrollbar_HoverTrackRight_HoverSet.png");

                    scrollbar->mouseMoved({mousePos.x + 80, mousePos.y});
                    TEST_DRAW("Scrollbar_HoverArrowRight_HoverSet.png");
                }
            }
        }

        SECTION("Textured")
        {
            renderer.setTextureTrack("resources/Texture1.png");
            renderer.setTextureThumb("resources/Texture2.png");
            renderer.setTextureArrowUp("resources/Texture3.png");
            renderer.setTextureArrowDown("resources/Texture4.png");

            SECTION("NormalState")
            {
                TEST_DRAW("Scrollbar_Normal_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("Scrollbar_Normal_TextureHoverSet.png")
                }
            }

            SECTION("HoverState")
            {
                scrollbar->mouseMoved(mousePos);

                TEST_DRAW("Scrollbar_Hover_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("Scrollbar_HoverArrowLeft_TextureHoverSet.png")

                    scrollbar->mouseMoved({mousePos.x + 32, mousePos.y});
                    TEST_DRAW("Scrollbar_HoverTrackLeft_TextureHoverSet.png");

                    scrollbar->mouseMoved({mousePos.x + 48, mousePos.y});
                    TEST_DRAW("Scrollbar_HoverThumb_TextureHoverSet.png");

                    scrollbar->mouseMoved({mousePos.x + 65, mousePos.y});
                    TEST_DRAW("Scrollbar_HoverTrackRight_TextureHoverSet.png");

                    scrollbar->mouseMoved({mousePos.x + 80, mousePos.y});
                    TEST_DRAW("Scrollbar_HoverArrowRight_TextureHoverSet.png");
                }
            }
        }
    }
}
