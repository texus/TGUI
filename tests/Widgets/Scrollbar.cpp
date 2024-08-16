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

TEST_CASE("[Scrollbar]")
{
    tgui::Scrollbar::Ptr scrollbar = tgui::Scrollbar::create();
    scrollbar->getRenderer()->setFont("resources/DejaVuSans.ttf");
    scrollbar->setMaximum(20);
    scrollbar->setViewportSize(5);
    scrollbar->setValue(10);

    SECTION("Signals")
    {
        scrollbar->onValueChange([](){});
        scrollbar->onValueChange([](unsigned int){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(scrollbar)->getSignal("ValueChanged").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(scrollbar->getWidgetType() == "Scrollbar");
    }

    SECTION("Position and Size")
    {
        scrollbar->setPosition(40, 30);
        scrollbar->setSize(150, 25);

        REQUIRE(scrollbar->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(scrollbar->getSize() == tgui::Vector2f(150, 25));
        REQUIRE(scrollbar->getFullSize() == scrollbar->getSize());
        REQUIRE(scrollbar->getWidgetOffset() == tgui::Vector2f(0, 0));
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

    SECTION("MaxValue")
    {
        REQUIRE(scrollbar->getMaxValue() == 15);

        scrollbar->setViewportSize(8);
        scrollbar->setMaximum(10);
        REQUIRE(scrollbar->getMaxValue() == 2);

        scrollbar->setViewportSize(5);
        scrollbar->setMaximum(5);
        REQUIRE(scrollbar->getMaxValue() == 0);

        scrollbar->setViewportSize(10);
        scrollbar->setMaximum(7);
        REQUIRE(scrollbar->getMaxValue() == 0);
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

    SECTION("Policy")
    {
        REQUIRE(scrollbar->getPolicy() == tgui::Scrollbar::Policy::Automatic);
        scrollbar->setPolicy(tgui::Scrollbar::Policy::Always);
        REQUIRE(scrollbar->getPolicy() == tgui::Scrollbar::Policy::Always);
        scrollbar->setPolicy(tgui::Scrollbar::Policy::Never);
        REQUIRE(scrollbar->getPolicy() == tgui::Scrollbar::Policy::Never);
        scrollbar->setPolicy(tgui::Scrollbar::Policy::Automatic);
        REQUIRE(scrollbar->getPolicy() == tgui::Scrollbar::Policy::Automatic);
    }

    SECTION("isShown")
    {
        scrollbar->setViewportSize(5);
        scrollbar->setMaximum(5);
        scrollbar->setPolicy(tgui::Scrollbar::Policy::Always);
        REQUIRE(scrollbar->isShown());

        scrollbar->setPolicy(tgui::Scrollbar::Policy::Automatic);
        REQUIRE(!scrollbar->isShown());

        scrollbar->setMaximum(10);
        REQUIRE(scrollbar->isShown());

        scrollbar->setPolicy(tgui::Scrollbar::Policy::Never);
        REQUIRE(!scrollbar->isShown());

        scrollbar->setPolicy(tgui::Scrollbar::Policy::Always);
        REQUIRE(scrollbar->isShown());

        scrollbar->setVisible(false);
        REQUIRE(!scrollbar->isShown());
    }

    SECTION("Orientation")
    {
        REQUIRE(scrollbar->getOrientation() == tgui::Orientation::Vertical);

        SECTION("Deprecated setVerticalScroll method")
        {
            REQUIRE(scrollbar->getVerticalScroll());

            scrollbar->setSize(100, 20);
            REQUIRE(!scrollbar->getVerticalScroll());
            REQUIRE(scrollbar->getOrientation() == tgui::Orientation::Horizontal);

            scrollbar->setSize(20, 100);
            REQUIRE(scrollbar->getVerticalScroll());
            REQUIRE(scrollbar->getOrientation() == tgui::Orientation::Vertical);

            scrollbar->setSize(10, 40);
            scrollbar->setVerticalScroll(false);
            REQUIRE(!scrollbar->getVerticalScroll());
            REQUIRE(scrollbar->getOrientation() == tgui::Orientation::Horizontal);
            REQUIRE(scrollbar->getSize() == tgui::Vector2f(40, 10)); // setVerticalScroll flips size
            scrollbar->setVerticalScroll(true);
            REQUIRE(scrollbar->getVerticalScroll());
            REQUIRE(scrollbar->getOrientation() == tgui::Orientation::Vertical);
            REQUIRE(scrollbar->getSize() == tgui::Vector2f(10, 40)); // setVerticalScroll flips size

            // Orientation isn't locked, calling setSize can still alter the orientation
            scrollbar->setSize(100, 20);
            REQUIRE(!scrollbar->getVerticalScroll());
            REQUIRE(scrollbar->getOrientation() == tgui::Orientation::Horizontal);
        }

        SECTION("New setOrientation method")
        {
            scrollbar->setSize(100, 20);
            REQUIRE(scrollbar->getOrientation() == tgui::Orientation::Horizontal);

            scrollbar->setSize(20, 100);
            REQUIRE(scrollbar->getOrientation() == tgui::Orientation::Vertical);

            scrollbar->setSize(10, 40);
            scrollbar->setOrientation(tgui::Orientation::Horizontal);
            REQUIRE(scrollbar->getOrientation() == tgui::Orientation::Horizontal);
            REQUIRE(scrollbar->getSize() == tgui::Vector2f(10, 40)); // setOrientation does not alter size
            scrollbar->setOrientation(tgui::Orientation::Vertical);
            REQUIRE(scrollbar->getOrientation() == tgui::Orientation::Vertical);
            REQUIRE(scrollbar->getSize() == tgui::Vector2f(10, 40)); // setOrientation does not alter size

            scrollbar->setSize(100, 20);
            scrollbar->setOrientation(tgui::Orientation::Horizontal);

            // Orientation is locked, setSize no longer alters the orientation
            scrollbar->setSize(20, 100);
            REQUIRE(scrollbar->getOrientation() == tgui::Orientation::Horizontal);
        }
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
                REQUIRE_NOTHROW(renderer->setProperty("TrackColor", tgui::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("TrackColorHover", tgui::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColor", tgui::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColorHover", tgui::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColor", tgui::Color{90, 100, 110}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowBackgroundColorHover", tgui::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColor", tgui::Color{110, 120, 130}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorHover", tgui::Color{120, 130, 140}));
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

            REQUIRE(renderer->getProperty("TrackColor").getColor() == tgui::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("TrackColorHover").getColor() == tgui::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("ThumbColor").getColor() == tgui::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("ThumbColorHover").getColor() == tgui::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("ArrowBackgroundColor").getColor() == tgui::Color(90, 100, 110));
            REQUIRE(renderer->getProperty("ArrowBackgroundColorHover").getColor() == tgui::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("ArrowColor").getColor() == tgui::Color(110, 120, 130));
            REQUIRE(renderer->getProperty("ArrowColorHover").getColor() == tgui::Color(120, 130, 140));

            REQUIRE(renderer->getTrackColor() == tgui::Color(50, 60, 70));
            REQUIRE(renderer->getTrackColorHover() == tgui::Color(60, 70, 80));
            REQUIRE(renderer->getThumbColor() == tgui::Color(70, 80, 90));
            REQUIRE(renderer->getThumbColorHover() == tgui::Color(80, 90, 100));
            REQUIRE(renderer->getArrowBackgroundColor() == tgui::Color(90, 100, 110));
            REQUIRE(renderer->getArrowBackgroundColorHover() == tgui::Color(100, 110, 120));
            REQUIRE(renderer->getArrowColor() == tgui::Color(110, 120, 130));
            REQUIRE(renderer->getArrowColorHover() == tgui::Color(120, 130, 140));
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
        scrollbar->setPolicy(tgui::Scrollbar::Policy::Always);

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
        renderer.setTrackColor(tgui::Color::Green);
        renderer.setThumbColor(tgui::Color::Red);
        renderer.setArrowBackgroundColor(tgui::Color::Blue);
        renderer.setArrowColor(tgui::Color::White);
        renderer.setOpacity(0.7f);
        scrollbar->setRenderer(renderer.getData());

        auto setHoverRenderer = [&](bool textured){
                                        renderer.setTrackColorHover(tgui::Color::Cyan);
                                        renderer.setThumbColorHover(tgui::Color::Magenta);
                                        renderer.setArrowBackgroundColorHover(tgui::Color::Yellow);
                                        renderer.setArrowColorHover(tgui::Color::Black);
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
