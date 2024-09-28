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

TEST_CASE("[SpinButton]")
{
    tgui::SpinButton::Ptr spinButton = tgui::SpinButton::create();
    spinButton->getRenderer()->setFont("resources/DejaVuSans.ttf");
    spinButton->setMinimum(10);
    spinButton->setMaximum(20);
    spinButton->setValue(15);

    SECTION("Signals")
    {
        spinButton->onValueChange([](){});
        spinButton->onValueChange([](float){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(spinButton)->getSignal("ValueChanged").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(spinButton->getWidgetType() == "SpinButton");
    }

    SECTION("Position and Size")
    {
        spinButton->setPosition(40, 30);
        spinButton->setSize(25, 60);
        spinButton->getRenderer()->setBorders(2);

        REQUIRE(spinButton->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(spinButton->getSize() == tgui::Vector2f(25, 60));
        REQUIRE(spinButton->getFullSize() == spinButton->getSize());
        REQUIRE(spinButton->getWidgetOffset() == tgui::Vector2f(0, 0));
    }

    SECTION("Minimum")
    {
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

    SECTION("Maximum")
    {
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

    SECTION("Value")
    {
        REQUIRE(spinButton->getValue() == 15);
        
        spinButton->setValue(14);
        REQUIRE(spinButton->getValue() == 14);
        
        spinButton->setValue(7);
        REQUIRE(spinButton->getValue() == 10);
        
        spinButton->setValue(23);
        REQUIRE(spinButton->getValue() == 20);
    }

    SECTION("Step")
    {
        spinButton->setStep(5);
        REQUIRE(spinButton->getStep() == 5);

        spinButton->setMinimum(20.5f);
        spinButton->setMaximum(50.5f);
        spinButton->setStep(3.0f);
        spinButton->setValue(26.5f);
        REQUIRE(spinButton->getValue() == 26.5f);

        spinButton->setValue(25.5f);
        REQUIRE(((spinButton->getValue() > 26.4f) && (spinButton->getValue() < 26.6f)));

        spinButton->setValue(24.5f);
        REQUIRE(((spinButton->getValue() > 23.4f) && (spinButton->getValue() < 23.6f)));
    }

    SECTION("Orientation")
    {
        REQUIRE(spinButton->getOrientation() == tgui::Orientation::Vertical);

        SECTION("Deprecated setVerticalScroll method")
        {
            REQUIRE(spinButton->getVerticalScroll());

            spinButton->setSize(100, 20);
            REQUIRE(!spinButton->getVerticalScroll());
            REQUIRE(spinButton->getOrientation() == tgui::Orientation::Horizontal);

            spinButton->setSize(20, 100);
            REQUIRE(spinButton->getVerticalScroll());
            REQUIRE(spinButton->getOrientation() == tgui::Orientation::Vertical);

            spinButton->setSize(10, 40);
            spinButton->setVerticalScroll(false);
            REQUIRE(!spinButton->getVerticalScroll());
            REQUIRE(spinButton->getOrientation() == tgui::Orientation::Horizontal);
            REQUIRE(spinButton->getSize() == tgui::Vector2f(40, 10)); // setVerticalScroll flips size
            spinButton->setVerticalScroll(true);
            REQUIRE(spinButton->getVerticalScroll());
            REQUIRE(spinButton->getOrientation() == tgui::Orientation::Vertical);
            REQUIRE(spinButton->getSize() == tgui::Vector2f(10, 40)); // setVerticalScroll flips size

            // Orientation isn't locked, calling setSize can still alter the orientation
            spinButton->setSize(100, 20);
            REQUIRE(!spinButton->getVerticalScroll());
            REQUIRE(spinButton->getOrientation() == tgui::Orientation::Horizontal);
        }

        SECTION("New setOrientation method")
        {
            spinButton->setSize(100, 20);
            REQUIRE(spinButton->getOrientation() == tgui::Orientation::Horizontal);

            spinButton->setSize(20, 100);
            REQUIRE(spinButton->getOrientation() == tgui::Orientation::Vertical);

            spinButton->setSize(10, 40);
            spinButton->setOrientation(tgui::Orientation::Horizontal);
            REQUIRE(spinButton->getOrientation() == tgui::Orientation::Horizontal);
            REQUIRE(spinButton->getSize() == tgui::Vector2f(10, 40)); // setOrientation does not alter size
            spinButton->setOrientation(tgui::Orientation::Vertical);
            REQUIRE(spinButton->getOrientation() == tgui::Orientation::Vertical);
            REQUIRE(spinButton->getSize() == tgui::Vector2f(10, 40)); // setOrientation does not alter size

            spinButton->setSize(100, 20);
            spinButton->setOrientation(tgui::Orientation::Horizontal);

            // Orientation is locked, setSize no longer alters the orientation
            spinButton->setSize(20, 100);
            REQUIRE(spinButton->getOrientation() == tgui::Orientation::Horizontal);
        }
    }

    SECTION("Events / Signals")
    {
        SECTION("ClickableWidget")
        {
            testClickableWidgetSignals(spinButton);
        }

        SECTION("ValueChanged")
        {
            spinButton->setPosition(40, 30);
            spinButton->setSize(25, 60);

            unsigned int valueChangedCount = 0;
            spinButton->onValueChange(&genericCallback, std::ref(valueChangedCount));

            spinButton->setValue(10);
            REQUIRE(valueChangedCount == 1);

            spinButton->setValue(10);
            REQUIRE(valueChangedCount == 1);

            auto parent = tgui::Panel::create({300, 200});
            parent->setPosition(60, 55);
            parent->add(spinButton);

            parent->leftMousePressed({110, 100});
            REQUIRE(valueChangedCount == 2);
            REQUIRE(spinButton->getValue() == 11);
            parent->leftMouseReleased({110, 100});

            parent->leftMousePressed({110, 135});
            parent->leftMouseReleased({110, 135});
            REQUIRE(valueChangedCount == 3);
            REQUIRE(spinButton->getValue() == 10);

            parent->leftMousePressed({110, 135});
            parent->leftMouseReleased({110, 135});
            REQUIRE(valueChangedCount == 3);
            REQUIRE(spinButton->getValue() == 10);
        }
    }

    testWidgetRenderer(spinButton->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = spinButton->getRenderer();

        SECTION("Colored")
        {
            SECTION("Set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColor", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorHover", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderBetweenArrows", "2"));
            }

            SECTION("Set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", tgui::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", tgui::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColor", tgui::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("ArrowColorHover", tgui::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", tgui::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderBetweenArrows", 2));
            }

            SECTION("Functions")
            {
                renderer->setBackgroundColor({30, 40, 50});
                renderer->setBackgroundColorHover({40, 50, 60});
                renderer->setArrowColor({50, 60, 70});
                renderer->setArrowColorHover({60, 70, 80});
                renderer->setBorderColor({70, 80, 90});
                renderer->setBorders({1, 2, 3, 4});
                renderer->setBorderBetweenArrows(2);
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == tgui::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == tgui::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("ArrowColor").getColor() == tgui::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("ArrowColorHover").getColor() == tgui::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == tgui::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("BorderBetweenArrows").getNumber() == 2);

            REQUIRE(renderer->getBackgroundColor() == tgui::Color(30, 40, 50));
            REQUIRE(renderer->getBackgroundColorHover() == tgui::Color(40, 50, 60));
            REQUIRE(renderer->getArrowColor() == tgui::Color(50, 60, 70));
            REQUIRE(renderer->getArrowColorHover() == tgui::Color(60, 70, 80));
            REQUIRE(renderer->getBorderColor() == tgui::Color(70, 80, 90));
            REQUIRE(renderer->getBorders() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getBorderBetweenArrows() == 2);
        }

        SECTION("textured")
        {
            tgui::Texture textureArrowUpNormal("resources/Black.png", {163, 154, 20, 20}, {0, 0, 20, 19});
            tgui::Texture textureArrowUpHover("resources/Black.png", {183, 154, 20, 20}, {0, 0, 20, 19});
            tgui::Texture textureArrowDownNormal("resources/Black.png", {163, 174, 20, 20}, {0, 1, 20, 19});
            tgui::Texture textureArrowDownHover("resources/Black.png", {183, 174, 20, 20}, {0, 1, 20, 19});

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowUp", tgui::Serializer::serialize(textureArrowUpNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowUpHover", tgui::Serializer::serialize(textureArrowUpHover)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowDown", tgui::Serializer::serialize(textureArrowDownNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowDownHover", tgui::Serializer::serialize(textureArrowDownHover)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowUp", textureArrowUpNormal));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowUpHover", textureArrowUpHover));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowDown", textureArrowDownNormal));
                REQUIRE_NOTHROW(renderer->setProperty("TextureArrowDownHover", textureArrowDownHover));
            }

            SECTION("functions")
            {
                renderer->setTextureArrowUp(textureArrowUpNormal);
                renderer->setTextureArrowUpHover(textureArrowUpHover);
                renderer->setTextureArrowDown(textureArrowDownNormal);
                renderer->setTextureArrowDownHover(textureArrowDownHover);
            }

            REQUIRE(renderer->getProperty("TextureArrowUp").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureArrowUpHover").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureArrowDown").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureArrowDownHover").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTextureArrowUp().getData() == textureArrowUpNormal.getData());
            REQUIRE(renderer->getTextureArrowUpHover().getData() == textureArrowUpHover.getData());
            REQUIRE(renderer->getTextureArrowDown().getData() == textureArrowDownNormal.getData());
            REQUIRE(renderer->getTextureArrowDownHover().getData() == textureArrowDownHover.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        spinButton->setMinimum(10);
        spinButton->setMaximum(50);
        spinButton->setValue(20);
        spinButton->setStep(5);

        testSavingWidget("SpinButton", spinButton);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(64, 35, spinButton)

        spinButton->setEnabled(true);
        spinButton->setPosition(10, 5);
        spinButton->setSize(49, 26);
        spinButton->setMinimum(7);
        spinButton->setMaximum(12);
        spinButton->setValue(11);

        tgui::SpinButtonRenderer renderer = tgui::RendererData::create();
        renderer.setBackgroundColor(tgui::Color::Green);
        renderer.setArrowColor(tgui::Color::Red);
        renderer.setBorderColor(tgui::Color::Blue);
        renderer.setBorderBetweenArrows(5);
        renderer.setBorders({1, 2, 3, 4});
        renderer.setOpacity(0.7f);
        spinButton->setRenderer(renderer.getData());

        auto setHoverRenderer = [&](bool textured){
                                        renderer.setBackgroundColorHover(tgui::Color::Cyan);
                                        renderer.setArrowColorHover(tgui::Color::Magenta);
                                        if (textured)
                                        {
                                            renderer.setTextureArrowUpHover("resources/Texture3.png");
                                            renderer.setTextureArrowDownHover("resources/Texture4.png");
                                        }
                                     };

        const auto mousePosLeft = spinButton->getPosition() + (spinButton->getSize() * (1.f / 4.f));
        const auto mousePosRight = spinButton->getPosition() + (spinButton->getSize() * (3.f / 4.f));

        SECTION("Colored")
        {
            SECTION("NormalState")
            {
                TEST_DRAW("SpinButton_Normal_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("SpinButton_Normal_HoverSet.png")
                }

                SECTION("Vertical")
                {
                    renderer.setBorders({1});
                    renderer.setBorderBetweenArrows(1);
                    spinButton->setPosition({5, 5});
                    spinButton->setSize(26, 26);
                    spinButton->setOrientation(tgui::Orientation::Vertical);
                    TEST_DRAW("SpinButton_Vertical.png")
                }

                SECTION("Horizontal")
                {
                    renderer.setBorders({1});
                    renderer.setBorderBetweenArrows(1);
                    spinButton->setPosition({5, 5});
                    spinButton->setSize(26, 26);
                    spinButton->setOrientation(tgui::Orientation::Horizontal);
                    TEST_DRAW("SpinButton_Horizontal_Flat.png")

                    spinButton->setSize(49, 13);
                    TEST_DRAW("SpinButton_Horizontal_Long.png")
                }
            }

            SECTION("HoverState (mouse on left arrow)")
            {
                spinButton->mouseMoved(mousePosLeft);

                TEST_DRAW("SpinButton_HoverLeft_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("SpinButton_HoverLeft_HoverSet.png")
                }
            }

            SECTION("HoverState (mouse on right arrow)")
            {
                spinButton->mouseMoved(mousePosRight);

                TEST_DRAW("SpinButton_HoverRight_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("SpinButton_HoverRight_HoverSet.png")
                }
            }
        }

        SECTION("Textured")
        {
            renderer.setTextureArrowUp("resources/Texture1.png");
            renderer.setTextureArrowDown("resources/Texture2.png");

            SECTION("NormalState")
            {
                TEST_DRAW("SpinButton_Normal_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("SpinButton_Normal_TextureHoverSet.png")
                }
            }

            SECTION("HoverState (mouse on left arrow)")
            {
                spinButton->mouseMoved(mousePosLeft);

                TEST_DRAW("SpinButton_HoverLeft_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("SpinButton_HoverLeft_TextureHoverSet.png")
                }
            }

            SECTION("HoverState (mouse on right arrow)")
            {
                spinButton->mouseMoved(mousePosRight);

                TEST_DRAW("SpinButton_HoverRight_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("SpinButton_HoverRight_TextureHoverSet.png")
                }
            }
        }
    }
}
