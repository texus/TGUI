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
#include <TGUI/Widgets/RangeSlider.hpp>

TEST_CASE("[RangeSlider]")
{
    tgui::RangeSlider::Ptr slider = tgui::RangeSlider::create();
    slider->getRenderer()->setFont("resources/DejaVuSans.ttf");
    slider->setMinimum(10);
    slider->setMaximum(20);
    slider->setSelectionStart(12);
    slider->setSelectionEnd(17);

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(slider->connect("RangeChanged", [](){}));
        REQUIRE_NOTHROW(slider->connect("RangeChanged", [](float, float){}));
        REQUIRE_NOTHROW(slider->connect("RangeChanged", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(slider->connect("RangeChanged", [](tgui::Widget::Ptr, std::string, float, float){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(slider->getWidgetType() == "RangeSlider");
    }

    SECTION("Position and Size")
    {
        slider->setPosition(40, 30);
        slider->setSize(150, 25);
        slider->getRenderer()->setBorders(2);

        REQUIRE(slider->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(slider->getSize() == sf::Vector2f(150, 25));

        // TODO: Test getFullSize and getWidgetOffset
    }

    SECTION("Minimum")
    {
        REQUIRE(slider->getMinimum() == 10);

        slider->setMinimum(12);
        REQUIRE(slider->getMinimum() == 12);
        REQUIRE(slider->getSelectionStart() == 12);
        REQUIRE(slider->getSelectionEnd() == 17);
        REQUIRE(slider->getMaximum() == 20);

        slider->setMinimum(16);
        REQUIRE(slider->getMinimum() == 16);
        REQUIRE(slider->getSelectionStart() == 16);
        REQUIRE(slider->getSelectionEnd() == 17);
        REQUIRE(slider->getMaximum() == 20);

        slider->setMinimum(22);
        REQUIRE(slider->getMinimum() == 22);
        REQUIRE(slider->getSelectionStart() == 22);
        REQUIRE(slider->getSelectionEnd() == 22);
        REQUIRE(slider->getMaximum() == 22);
    }

    SECTION("Maximum")
    {
        REQUIRE(slider->getMaximum() == 20);

        slider->setMaximum(17);
        REQUIRE(slider->getMinimum() == 10);
        REQUIRE(slider->getSelectionStart() == 12);
        REQUIRE(slider->getSelectionEnd() == 17);
        REQUIRE(slider->getMaximum() == 17);

        slider->setMaximum(11);
        REQUIRE(slider->getMinimum() == 10);
        REQUIRE(slider->getSelectionStart() == 11);
        REQUIRE(slider->getSelectionEnd() == 11);
        REQUIRE(slider->getMaximum() == 11);

        slider->setMaximum(9);
        REQUIRE(slider->getMinimum() == 9);
        REQUIRE(slider->getSelectionStart() == 9);
        REQUIRE(slider->getSelectionEnd() == 9);
        REQUIRE(slider->getMaximum() == 9);
    }

    SECTION("SelectionStart")
    {
        REQUIRE(slider->getSelectionStart() == 12);

        slider->setSelectionStart(11);
        REQUIRE(slider->getSelectionStart() == 11);

        slider->setSelectionStart(7);
        REQUIRE(slider->getSelectionStart() == 10);
        REQUIRE(slider->getSelectionEnd() == 17);

        slider->setSelectionStart(18);
        REQUIRE(slider->getSelectionStart() == 18);
        REQUIRE(slider->getSelectionEnd() == 18);

        slider->setSelectionStart(23);
        REQUIRE(slider->getSelectionStart() == 20);
        REQUIRE(slider->getSelectionEnd() == 20);
    }

    SECTION("SelectionEnd")
    {
        REQUIRE(slider->getSelectionEnd() == 17);

        slider->setSelectionEnd(19);
        REQUIRE(slider->getSelectionEnd() == 19);

        slider->setSelectionEnd(24);
        REQUIRE(slider->getSelectionEnd() == 20);
        REQUIRE(slider->getSelectionStart() == 12);

        slider->setSelectionEnd(11);
        REQUIRE(slider->getSelectionEnd() == 11);
        REQUIRE(slider->getSelectionStart() == 11);

        slider->setSelectionEnd(9);
        REQUIRE(slider->getSelectionEnd() == 10);
        REQUIRE(slider->getSelectionStart() == 10);
    }

    SECTION("Step")
    {
        slider->setStep(5);
        REQUIRE(slider->getStep() == 5);

        slider->setMinimum(20.5f);
        slider->setMaximum(50.5f);
        slider->setStep(3.0f);
        slider->setSelectionEnd(29.5f);
        REQUIRE(slider->getSelectionEnd() == 29.5f);

        slider->setSelectionEnd(25.5f);
        REQUIRE(((slider->getSelectionEnd() > 26.4f) && (slider->getSelectionStart() < 26.6f)));

        slider->setSelectionStart(24.5f);
        REQUIRE(((slider->getSelectionStart() > 23.4f) && (slider->getSelectionStart() < 23.6f)));
    }

    SECTION("Events / Signals")
    {
        SECTION("Widget")
        {
            testWidgetSignals(slider);
        }

        SECTION("RangeChanged")
        {
            unsigned int rangeChangedCount = 0;
            slider->connect("RangeChanged", &genericCallback, std::ref(rangeChangedCount));

            slider->setSelectionStart(14);
            REQUIRE(rangeChangedCount == 1);

            slider->setSelectionEnd(16);
            REQUIRE(rangeChangedCount == 2);

            slider->setSelectionStart(14);
            REQUIRE(rangeChangedCount == 2);

            slider->setSelectionEnd(16);
            REQUIRE(rangeChangedCount == 2);

            // TODO: Test value changes on mouse events
        }
    }

    testWidgetRenderer(slider->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = slider->getRenderer();

        SECTION("colored")
        {
            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TrackColor", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("TrackColorHover", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTrackColor", "rgb(90, 100, 110)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTrackColorHover", "rgb(100, 110, 120)"));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColor", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColorHover", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorHover", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TrackColor", sf::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("TrackColorHover", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTrackColor", sf::Color{90, 100, 110}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTrackColorHover", sf::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColor", sf::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColorHover", sf::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorHover", sf::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
            }

            SECTION("functions")
            {
                renderer->setTrackColor({30, 40, 50});
                renderer->setTrackColorHover({40, 50, 60});
                renderer->setSelectedTrackColor({90, 100, 110});
                renderer->setSelectedTrackColorHover({100, 110, 120});
                renderer->setThumbColor({50, 60, 70});
                renderer->setThumbColorHover({60, 70, 80});
                renderer->setBorderColor({70, 80, 90});
                renderer->setBorderColorHover({80, 90, 100});
                renderer->setBorders({1, 2, 3, 4});
            }

            REQUIRE(renderer->getProperty("TrackColor").getColor() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("TrackColorHover").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("SelectedTrackColor").getColor() == sf::Color(90, 100, 110));
            REQUIRE(renderer->getProperty("SelectedTrackColorHover").getColor() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("ThumbColor").getColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("ThumbColorHover").getColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("BorderColorHover").getColor() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));

            REQUIRE(renderer->getTrackColor() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getTrackColorHover() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getSelectedTrackColor() == sf::Color(90, 100, 110));
            REQUIRE(renderer->getSelectedTrackColorHover() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getThumbColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getThumbColorHover() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getBorderColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getBorderColorHover() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getBorders() == tgui::Borders(1, 2, 3, 4));
        }

        SECTION("textured")
        {
            tgui::Texture textureTrack("resources/Black.png", {203, 150, 20, 45}, {0, 15, 20, 15});
            tgui::Texture textureTrackHover("resources/Black.png", {223, 150, 20, 45}, {0, 15, 20, 15});
            tgui::Texture textureThumb("resources/Black.png", {243, 150, 30, 30});
            tgui::Texture textureThumbHover("resources/Black.png", {243, 150, 30, 30});

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureTrack", tgui::Serializer::serialize(textureTrack)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureTrackHover", tgui::Serializer::serialize(textureTrackHover)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureThumb", tgui::Serializer::serialize(textureThumb)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureThumbHover", tgui::Serializer::serialize(textureThumbHover)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureTrack", textureTrack));
                REQUIRE_NOTHROW(renderer->setProperty("TextureTrackHover", textureTrackHover));
                REQUIRE_NOTHROW(renderer->setProperty("TextureThumb", textureThumb));
                REQUIRE_NOTHROW(renderer->setProperty("TextureThumbHover", textureThumbHover));
            }

            SECTION("functions")
            {
                renderer->setTextureTrack(textureTrack);
                renderer->setTextureTrackHover(textureTrackHover);
                renderer->setTextureThumb(textureThumb);
                renderer->setTextureThumbHover(textureThumbHover);
            }

            REQUIRE(renderer->getProperty("TextureTrack").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureTrackHover").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureThumb").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureThumbHover").getTexture().getData() != nullptr);

            REQUIRE(renderer->getProperty("TextureTrack").getTexture().getData() == textureTrack.getData());
            REQUIRE(renderer->getProperty("TextureTrackHover").getTexture().getData() == textureTrackHover.getData());
            REQUIRE(renderer->getProperty("TextureThumb").getTexture().getData() == textureThumb.getData());
            REQUIRE(renderer->getProperty("TextureThumbHover").getTexture().getData() == textureThumbHover.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        slider->setMinimum(10);
        slider->setMaximum(50);
        slider->setSelectionStart(20);
        slider->setSelectionEnd(35);
        slider->setStep(5);

        testSavingWidget("RangeSlider", slider, false);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(120, 60, slider)

        slider->setEnabled(true);
        slider->setPosition({10, 15});
        slider->setSize({100, 30});
        slider->setMinimum(1000);
        slider->setMaximum(2000);
        slider->setSelectionStart(1200);
        slider->setSelectionEnd(1650);

        tgui::RangeSliderRenderer renderer = tgui::RendererData::create();
        renderer.setTrackColor(sf::Color::Green);
        renderer.setThumbColor(sf::Color::Red);
        renderer.setBorderColor(sf::Color::Blue);
        renderer.setSelectedTrackColor(sf::Color::White);
        renderer.setBorders({1, 2, 3, 4});
        renderer.setOpacity(0.7f);
        slider->setRenderer(renderer.getData());

        auto setHoverRenderer = [&](bool textured){
                                        renderer.setTrackColorHover(sf::Color::Cyan);
                                        renderer.setThumbColorHover(sf::Color::Magenta);
                                        renderer.setBorderColorHover(sf::Color::Yellow);
                                        renderer.setSelectedTrackColorHover(sf::Color::Black);
                                        if (textured)
                                        {
                                            renderer.setTextureTrackHover("resources/Texture3.png");
                                            renderer.setTextureThumbHover("resources/Texture4.png");
                                        }
                                     };

        const auto mousePos = slider->getPosition() + (slider->getSize() / 2.f);

        SECTION("Colored")
        {
            SECTION("NormalState")
            {
                TEST_DRAW("RangeSlider_Normal_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("RangeSlider_Normal_HoverSet.png")
                }
            }

            SECTION("HoverState")
            {
                slider->mouseMoved(mousePos);

                TEST_DRAW("RangeSlider_Hover_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("RangeSlider_Hover_HoverSet.png")
                }
            }
        }

        SECTION("Textured")
        {
            renderer.setTextureTrack("resources/Texture1.png");
            renderer.setTextureThumb("resources/Texture2.png");

            SECTION("NormalState")
            {
                TEST_DRAW("RangeSlider_Normal_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("RangeSlider_Normal_TextureHoverSet.png")
                }
            }

            SECTION("HoverState")
            {
                slider->mouseMoved(mousePos);

                TEST_DRAW("RangeSlider_Hover_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("RangeSlider_Hover_TextureHoverSet.png")
                }
            }
        }
    }
}
