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

TEST_CASE("[Slider]")
{
    tgui::Slider::Ptr slider = tgui::Slider::create();
    slider->getRenderer()->setFont("resources/DejaVuSans.ttf");
    slider->setMinimum(10);
    slider->setMaximum(20);
    slider->setValue(15);

    SECTION("Signals")
    {
        slider->onValueChange([](){});
        slider->onValueChange([](float){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(slider)->getSignal("ValueChanged").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(slider->getWidgetType() == "Slider");
    }

    SECTION("Position and Size")
    {
        slider->setPosition(40, 30);
        slider->getRenderer()->setBorders(2);

        SECTION("Horizontal")
        {
            slider->setSize(150, 25);

            float thumbHeight = slider->getSize().y * 1.6f;
            float thumbWidth = thumbHeight / 2.f;
            REQUIRE(slider->getSize() == tgui::Vector2f(150, 25));
            REQUIRE(slider->getFullSize() == tgui::Vector2f(slider->getSize().x + thumbWidth, thumbHeight));
            REQUIRE(slider->getWidgetOffset() == -tgui::Vector2f(thumbWidth / 2.f, (thumbHeight - slider->getSize().y) / 2.f));

            slider->getSharedRenderer()->setThumbWithinTrack(true);
            REQUIRE(slider->getFullSize() == tgui::Vector2f(slider->getSize().x, thumbHeight));
            REQUIRE(slider->getWidgetOffset() == -tgui::Vector2f(0, (thumbHeight - slider->getSize().y) / 2.f));
        }

        SECTION("Vertical")
        {
            slider->setSize(20, 140);

            float thumbWidth = slider->getSize().x * 1.6f;
            float thumbHeight = thumbWidth / 2.f;

            REQUIRE(slider->getSize() == tgui::Vector2f(20, 140));
            REQUIRE(slider->getFullSize() == tgui::Vector2f(thumbWidth, slider->getSize().y + thumbHeight));
            REQUIRE(slider->getWidgetOffset() == -tgui::Vector2f((thumbWidth - slider->getSize().x) / 2.f, thumbHeight / 2.f));

            slider->getSharedRenderer()->setThumbWithinTrack(true);
            REQUIRE(slider->getFullSize() == tgui::Vector2f(thumbWidth, slider->getSize().y));
            REQUIRE(slider->getWidgetOffset() == -tgui::Vector2f((thumbWidth - slider->getSize().x) / 2.f, 0));
        }

        REQUIRE(slider->getPosition() == tgui::Vector2f(40, 30));
    }

    SECTION("Minimum")
    {
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

    SECTION("Maximum")
    {
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

    SECTION("Value")
    {
        REQUIRE(slider->getValue() == 15);
        
        slider->setValue(14);
        REQUIRE(slider->getValue() == 14);
        
        slider->setValue(7);
        REQUIRE(slider->getValue() == 10);
        
        slider->setValue(23);
        REQUIRE(slider->getValue() == 20);
    }

    SECTION("Step")
    {
        slider->setStep(5);
        REQUIRE(slider->getStep() == 5);

        slider->setMinimum(20.5f);
        slider->setMaximum(50.5f);
        slider->setStep(3.0f);
        slider->setValue(26.5f);
        REQUIRE(slider->getValue() == 26.5f);

        slider->setValue(25.5f);
        REQUIRE(((slider->getValue() > 26.4f) && (slider->getValue() < 26.6f)));

        slider->setValue(24.5f);
        REQUIRE(((slider->getValue() > 23.4f) && (slider->getValue() < 23.6f)));
    }

    SECTION("Orientation")
    {
        REQUIRE(slider->getOrientation() == tgui::Orientation::Horizontal);

        SECTION("Deprecated setVerticalScroll method")
        {
            REQUIRE(!slider->getVerticalScroll());

            slider->setSize(100, 20);
            REQUIRE(!slider->getVerticalScroll());
            REQUIRE(slider->getOrientation() == tgui::Orientation::Horizontal);

            slider->setSize(20, 100);
            REQUIRE(slider->getVerticalScroll());
            REQUIRE(slider->getOrientation() == tgui::Orientation::Vertical);

            slider->setSize(10, 40);
            slider->setVerticalScroll(false);
            REQUIRE(!slider->getVerticalScroll());
            REQUIRE(slider->getOrientation() == tgui::Orientation::Horizontal);
            REQUIRE(slider->getSize() == tgui::Vector2f(40, 10)); // setVerticalScroll flips size
            slider->setVerticalScroll(true);
            REQUIRE(slider->getVerticalScroll());
            REQUIRE(slider->getOrientation() == tgui::Orientation::Vertical);
            REQUIRE(slider->getSize() == tgui::Vector2f(10, 40)); // setVerticalScroll flips size

            // Orientation isn't locked, calling setSize can still alter the orientation
            slider->setSize(100, 20);
            REQUIRE(!slider->getVerticalScroll());
            REQUIRE(slider->getOrientation() == tgui::Orientation::Horizontal);
        }

        SECTION("New setOrientation method")
        {
            slider->setSize(100, 20);
            REQUIRE(slider->getOrientation() == tgui::Orientation::Horizontal);

            slider->setSize(20, 100);
            REQUIRE(slider->getOrientation() == tgui::Orientation::Vertical);

            slider->setSize(10, 40);
            slider->setOrientation(tgui::Orientation::Horizontal);
            REQUIRE(slider->getOrientation() == tgui::Orientation::Horizontal);
            REQUIRE(slider->getSize() == tgui::Vector2f(10, 40)); // setOrientation does not alter size
            slider->setOrientation(tgui::Orientation::Vertical);
            REQUIRE(slider->getOrientation() == tgui::Orientation::Vertical);
            REQUIRE(slider->getSize() == tgui::Vector2f(10, 40)); // setOrientation does not alter size

            slider->setSize(100, 20);
            slider->setOrientation(tgui::Orientation::Horizontal);

            // Orientation is locked, setSize no longer alters the orientation
            slider->setSize(20, 100);
            REQUIRE(slider->getOrientation() == tgui::Orientation::Horizontal);
        }
    }

    SECTION("InvertedDirection")
    {
        REQUIRE(!slider->getInvertedDirection());

        slider->setInvertedDirection(true);
        REQUIRE(slider->getInvertedDirection());
        REQUIRE(slider->getValue() == 15);

        slider->setInvertedDirection(false);
        REQUIRE(!slider->getInvertedDirection());
    }

    SECTION("ChangeValueOnScroll")
    {
        REQUIRE(slider->getChangeValueOnScroll());
        slider->setChangeValueOnScroll(false);
        REQUIRE(!slider->getChangeValueOnScroll());
        slider->setChangeValueOnScroll(true);
        REQUIRE(slider->getChangeValueOnScroll());
    }

    SECTION("Events / Signals")
    {
        SECTION("Widget")
        {
            testWidgetSignals(slider);
        }

        SECTION("ValueChanged")
        {
            slider->setStep(2);
            slider->setInvertedDirection(true);
            slider->setValue(10);

            unsigned int valueChangedCount = 0;
            slider->onValueChange(&genericCallback, std::ref(valueChangedCount));

            SECTION("setValue")
            {
                slider->setValue(14);
                REQUIRE(valueChangedCount == 1);

                slider->setValue(14);
                REQUIRE(valueChangedCount == 1);
            }

            SECTION("Horizontal slider")
            {
                slider->setSize(140, 20);

                SECTION("Click on track")
                {
                    REQUIRE(valueChangedCount == 0);
                    slider->leftMousePressed({71, 10});
                    REQUIRE(slider->getValue() == 14);
                    REQUIRE(valueChangedCount == 1);
                    slider->leftMouseReleased({71, 10});
                    slider->leftMouseButtonNoLongerDown();
                    REQUIRE(valueChangedCount == 1);
                }

                SECTION("Dragging thumb")
                {
                    slider->leftMousePressed({134, 10});
                    REQUIRE(valueChangedCount == 0);
                    slider->mouseMoved({10, 10});
                    REQUIRE(slider->getValue() == 18);
                    REQUIRE(valueChangedCount == 1);
                }

                SECTION("Mouse wheel scroll")
                {
                    slider->scrolled(-2, {}, false);
                    REQUIRE(slider->getValue() == 14);
                    REQUIRE(valueChangedCount == 1);
                    slider->scrolled(1, {}, false);
                    REQUIRE(slider->getValue() == 12);
                    REQUIRE(valueChangedCount == 2);

                    // If the scroll event originated from touch events (two finger scrolling) then it has no effect
                    slider->scrolled(-1, {}, true);
                    REQUIRE(valueChangedCount == 2);
                    slider->scrolled(1, {}, true);
                    REQUIRE(valueChangedCount == 2);
                }
            }

            SECTION("Vertical slider")
            {
                slider->setSize(20, 140);

                SECTION("Click on track")
                {
                    REQUIRE(valueChangedCount == 0);
                    slider->leftMousePressed({10, 71});
                    REQUIRE(slider->getValue() == 16);
                    REQUIRE(valueChangedCount == 1);
                    slider->leftMouseReleased({10, 71});
                    slider->leftMouseButtonNoLongerDown();
                    REQUIRE(valueChangedCount == 1);
                }

                SECTION("Dragging thumb")
                {
                    slider->leftMousePressed({10, 6});
                    REQUIRE(valueChangedCount == 0);
                    slider->mouseMoved({10, 130});
                    REQUIRE(slider->getValue() == 18);
                    REQUIRE(valueChangedCount == 1);
                }

                SECTION("Mouse wheel scroll")
                {
                    slider->scrolled(-2, {}, false);
                    REQUIRE(slider->getValue() == 14);
                    REQUIRE(valueChangedCount == 1);
                    slider->scrolled(1, {}, false);
                    REQUIRE(slider->getValue() == 12);
                    REQUIRE(valueChangedCount == 2);

                    // If the scroll event originated from touch events (two finger scrolling) then it has no effect
                    slider->scrolled(-1, {}, true);
                    REQUIRE(valueChangedCount == 2);
                    slider->scrolled(1, {}, true);
                    REQUIRE(valueChangedCount == 2);
                }
            }
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
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColor", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColorHover", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorHover", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbWithinTrack", "true"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TrackColor", tgui::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("TrackColorHover", tgui::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColor", tgui::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbColorHover", tgui::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", tgui::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorHover", tgui::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("ThumbWithinTrack", true));
            }

            SECTION("functions")
            {
                renderer->setTrackColor({30, 40, 50});
                renderer->setTrackColorHover({40, 50, 60});
                renderer->setThumbColor({50, 60, 70});
                renderer->setThumbColorHover({60, 70, 80});
                renderer->setBorderColor({70, 80, 90});
                renderer->setBorderColorHover({80, 90, 100});
                renderer->setBorders({1, 2, 3, 4});
                renderer->setThumbWithinTrack(true);
            }

            REQUIRE(renderer->getProperty("TrackColor").getColor() == tgui::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("TrackColorHover").getColor() == tgui::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("ThumbColor").getColor() == tgui::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("ThumbColorHover").getColor() == tgui::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == tgui::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("BorderColorHover").getColor() == tgui::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("ThumbWithinTrack").getBool());

            REQUIRE(renderer->getTrackColor() == tgui::Color(30, 40, 50));
            REQUIRE(renderer->getTrackColorHover() == tgui::Color(40, 50, 60));
            REQUIRE(renderer->getThumbColor() == tgui::Color(50, 60, 70));
            REQUIRE(renderer->getThumbColorHover() == tgui::Color(60, 70, 80));
            REQUIRE(renderer->getBorderColor() == tgui::Color(70, 80, 90));
            REQUIRE(renderer->getBorderColorHover() == tgui::Color(80, 90, 100));
            REQUIRE(renderer->getBorders() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getThumbWithinTrack());
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
        slider->setValue(20);
        slider->setStep(5);
        slider->setInvertedDirection(true);

        testSavingWidget("Slider", slider);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(120, 60, slider)

        slider->setEnabled(true);
        slider->setPosition({10, 15});
        slider->setSize({100, 30});
        slider->setMinimum(1000);
        slider->setMaximum(2000);
        slider->setValue(1600);

        tgui::SliderRenderer renderer = tgui::RendererData::create();
        renderer.setTrackColor(tgui::Color::Green);
        renderer.setThumbColor(tgui::Color::Red);
        renderer.setBorderColor(tgui::Color::Blue);
        renderer.setBorders({1, 2, 3, 4});
        renderer.setOpacity(0.7f);
        slider->setRenderer(renderer.getData());

        auto setHoverRenderer = [&](bool textured){
                                        renderer.setTrackColorHover(tgui::Color::Cyan);
                                        renderer.setThumbColorHover(tgui::Color::Magenta);
                                        renderer.setBorderColorHover(tgui::Color::Yellow);
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
                TEST_DRAW("Slider_Normal_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("Slider_Normal_HoverSet.png")
                }
            }

            SECTION("HoverState")
            {
                slider->mouseMoved(mousePos);

                TEST_DRAW("Slider_Hover_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("Slider_Hover_HoverSet.png")
                }
            }

            SECTION("ThumbWithinTrack")
            {
                slider->setValue(slider->getMinimum());
                renderer.setThumbWithinTrack(false);
                TEST_DRAW("Slider_ThumbOutsideTrack.png")

                renderer.setThumbWithinTrack(true);
                TEST_DRAW("Slider_ThumbInsideTrack.png")
            }
        }

        SECTION("Textured")
        {
            renderer.setTextureTrack("resources/Texture1.png");
            renderer.setTextureThumb("resources/Texture2.png");

            SECTION("NormalState")
            {
                TEST_DRAW("Slider_Normal_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("Slider_Normal_TextureHoverSet.png")
                }
            }

            SECTION("HoverState")
            {
                slider->mouseMoved(mousePos);

                TEST_DRAW("Slider_Hover_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("Slider_Hover_TextureHoverSet.png")
                }
            }

            SECTION("ThumbWithinTrack")
            {
                slider->setValue(slider->getMaximum());
                renderer.setThumbWithinTrack(false);
                TEST_DRAW("Slider_TextureThumbOutsideTrack.png")

                renderer.setThumbWithinTrack(true);
                TEST_DRAW("Slider_TextureThumbInsideTrack.png")
            }

            SECTION("Non-square thumb texture")
            {
                tgui::Texture texture("resources/TextureRect1.png");

                renderer.setTextureThumb(texture);
                TEST_DRAW("Slider_NonSquareThumbTexture.png")

                const auto imageSize = texture.getImageSize();
                const auto pixels = texture.getData()->backendTexture->getPixels();

                auto rotatedImagePixels = tgui::MakeUniqueForOverwrite<std::uint8_t[]>(imageSize.x * imageSize.y * 4);
                for (unsigned y = 0; y < imageSize.y; ++y)
                {
                    for (unsigned x = 0; x < imageSize.x; ++x)
                        std::memcpy(&rotatedImagePixels[(x * imageSize.y + y) * 4], &pixels[(y * imageSize.x + x) * 4], 4);
                }

                tgui::Texture rotatedTexture;
                rotatedTexture.loadFromPixelData({imageSize.y, imageSize.x}, rotatedImagePixels.get());

                renderer.setTextureThumb(rotatedTexture);
                TEST_DRAW("Slider_NonSquareThumbTexture_Rotated.png")
            }
        }
    }
}
