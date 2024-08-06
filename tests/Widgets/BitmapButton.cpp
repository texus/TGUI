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

TEST_CASE("[BitmapButton]")
{
    tgui::BitmapButton::Ptr button = tgui::BitmapButton::create();
    button->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        button->onPress([](){});
        button->onPress([](const tgui::String&){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(button)->getSignal("Pressed").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(button->getWidgetType() == "BitmapButton");
    }

    SECTION("Position and Size")
    {
        button->setPosition(40, 30);
        button->setSize(150, 100);
        button->getRenderer()->setBorders(2);

        REQUIRE(button->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(button->getSize() == tgui::Vector2f(150, 100));
        REQUIRE(button->getFullSize() == button->getSize());
        REQUIRE(button->getWidgetOffset() == tgui::Vector2f(0, 0));
    }

    SECTION("Text")
    {
        REQUIRE(button->getText() == "");
        button->setText("SomeText");
        REQUIRE(button->getText() == "SomeText");
    }

    SECTION("TextSize")
    {
        button->setTextSize(25);
        REQUIRE(button->getTextSize() == 25);
    }

    SECTION("Image")
    {
        REQUIRE(button->getImage().getData() == nullptr);
        button->setImage("resources/image.png");
        REQUIRE(button->getImage().getData() != nullptr);
    }

    SECTION("ImageScaling")
    {
        REQUIRE(button->getImageScaling() == 0);
        button->setImageScaling(0.5f);
        REQUIRE(button->getImageScaling() == 0.5f);
    }

    SECTION("IgnoreKeyEvents")
    {
        REQUIRE(!button->getIgnoreKeyEvents());
        button->setIgnoreKeyEvents(true);
        REQUIRE(button->getIgnoreKeyEvents());
        button->setIgnoreKeyEvents(false);
        REQUIRE(!button->getIgnoreKeyEvents());
    }

    SECTION("Events / Signals")
    {
        SECTION("ClickableWidget")
        {
            testClickableWidgetSignals(button);
        }

        SECTION("Pressed signal")
        {
            button->setPosition(40, 30);
            button->setSize(150, 100);

            unsigned int pressedCount = 0;
            button->onPress(&genericCallback, std::ref(pressedCount));

            SECTION("mouse click")
            {
                button->leftMouseReleased({115, 80});
                REQUIRE(pressedCount == 0);

                button->leftMousePressed({115, 80});
                REQUIRE(pressedCount == 0);

                button->leftMouseReleased({115, 80});
                REQUIRE(pressedCount == 1);
            }

            SECTION("key pressed")
            {
                tgui::Event::KeyEvent keyEvent;
                keyEvent.alt = false;
                keyEvent.control = false;
                keyEvent.shift = false;
                keyEvent.system = false;

                keyEvent.code = tgui::Event::KeyboardKey::Space;
                button->keyPressed(keyEvent);
                REQUIRE(pressedCount == 1);

                keyEvent.code = tgui::Event::KeyboardKey::Enter;
                button->keyPressed(keyEvent);
                REQUIRE(pressedCount == 2);

                SECTION("ignored")
                {
                    button->setIgnoreKeyEvents(true);

                    keyEvent.code = tgui::Event::KeyboardKey::Space;
                    button->keyPressed(keyEvent);
                    REQUIRE(pressedCount == 2);

                    keyEvent.code = tgui::Event::KeyboardKey::Enter;
                    button->keyPressed(keyEvent);
                    REQUIRE(pressedCount == 2);
                }
            }
        }
    }

    testWidgetRenderer(button->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = button->getRenderer();

        SECTION("colored")
        {
            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(20, 30, 40)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", "rgb(30, 40, 50)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDown", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDisabled", "rgb(90, 100, 110)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDown", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDisabled", "rgb(100, 110, 120)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorHover", "rgb(110, 120, 130)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDown", "rgb(120, 130, 140)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDisabled", "rgb(130, 140, 150)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyle", "Italic"));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleHover", "Underlined"));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleDown", "Bold | Underlined"));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleDisabled", "StrikeThrough"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("RoundedBorderRadius", 5));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", tgui::Color{20, 30, 40}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", tgui::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDown", tgui::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDisabled", tgui::Color{90, 100, 110}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", tgui::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", tgui::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDown", tgui::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDisabled", tgui::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", tgui::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorHover", tgui::Color{110, 120, 130}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDown", tgui::Color{120, 130, 140}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDisabled", tgui::Color{130, 140, 150}));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyle", tgui::TextStyle::Italic));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleHover", tgui::TextStyle::Underlined));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleDown", tgui::TextStyles(tgui::TextStyle::Bold | tgui::TextStyle::Underlined)));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleDisabled", tgui::TextStyle::StrikeThrough));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("RoundedBorderRadius", 5));
            }

            SECTION("functions")
            {
                renderer->setTextColor({20, 30, 40});
                renderer->setTextColorHover({30, 40, 50});
                renderer->setTextColorDown({40, 50, 60});
                renderer->setTextColorDisabled({90, 100, 110});
                renderer->setBackgroundColor({50, 60, 70});
                renderer->setBackgroundColorHover({60, 70, 80});
                renderer->setBackgroundColorDown({70, 80, 90});
                renderer->setBackgroundColorDisabled({100, 110, 120});
                renderer->setBorderColor({80, 90, 100});
                renderer->setBorderColorHover({110, 120, 130});
                renderer->setBorderColorDown({120, 130, 140});
                renderer->setBorderColorDisabled({130, 140, 150});
                renderer->setTextStyle(tgui::TextStyle::Italic);
                renderer->setTextStyleHover(tgui::TextStyle::Underlined);
                renderer->setTextStyleDown(tgui::TextStyle::Bold | tgui::TextStyle::Underlined);
                renderer->setTextStyleDisabled(tgui::TextStyle::StrikeThrough);
                renderer->setBorders({1, 2, 3, 4});
                renderer->setRoundedBorderRadius(5);
            }

            REQUIRE(renderer->getProperty("TextColor").getColor() == tgui::Color(20, 30, 40));
            REQUIRE(renderer->getProperty("TextColorHover").getColor() == tgui::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("TextColorDown").getColor() == tgui::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("TextColorDisabled").getColor() == tgui::Color(90, 100, 110));
            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == tgui::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == tgui::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("BackgroundColorDown").getColor() == tgui::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("BackgroundColorDisabled").getColor() == tgui::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == tgui::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("BorderColorHover").getColor() == tgui::Color(110, 120, 130));
            REQUIRE(renderer->getProperty("BorderColorDown").getColor() == tgui::Color(120, 130, 140));
            REQUIRE(renderer->getProperty("BorderColorDisabled").getColor() == tgui::Color(130, 140, 150));
            REQUIRE(renderer->getProperty("TextStyle").getTextStyle() == tgui::TextStyle::Italic);
            REQUIRE(renderer->getProperty("TextStyleHover").getTextStyle() == tgui::TextStyle::Underlined);
            REQUIRE(renderer->getProperty("TextStyleDown").getTextStyle() == (tgui::TextStyle::Bold | tgui::TextStyle::Underlined));
            REQUIRE(renderer->getProperty("TextStyleDisabled").getTextStyle() == tgui::TextStyle::StrikeThrough);
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("RoundedBorderRadius").getNumber() == 5);

            REQUIRE(renderer->getTextColor() == tgui::Color(20, 30, 40));
            REQUIRE(renderer->getTextColorHover() == tgui::Color(30, 40, 50));
            REQUIRE(renderer->getTextColorDown() == tgui::Color(40, 50, 60));
            REQUIRE(renderer->getTextColorDisabled() == tgui::Color(90, 100, 110));
            REQUIRE(renderer->getBackgroundColor() == tgui::Color(50, 60, 70));
            REQUIRE(renderer->getBackgroundColorHover() == tgui::Color(60, 70, 80));
            REQUIRE(renderer->getBackgroundColorDown() == tgui::Color(70, 80, 90));
            REQUIRE(renderer->getBackgroundColorDisabled() == tgui::Color(100, 110, 120));
            REQUIRE(renderer->getBorderColor() == tgui::Color(80, 90, 100));
            REQUIRE(renderer->getBorderColorHover() == tgui::Color(110, 120, 130));
            REQUIRE(renderer->getBorderColorDown() == tgui::Color(120, 130, 140));
            REQUIRE(renderer->getBorderColorDisabled() == tgui::Color(130, 140, 150));
            REQUIRE(renderer->getTextStyle() == tgui::TextStyle::Italic);
            REQUIRE(renderer->getTextStyleHover() == tgui::TextStyle::Underlined);
            REQUIRE(renderer->getTextStyleDown() == (tgui::TextStyle::Bold | tgui::TextStyle::Underlined));
            REQUIRE(renderer->getTextStyleDisabled() == tgui::TextStyle::StrikeThrough);
            REQUIRE(renderer->getBorders() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getRoundedBorderRadius() == 5);
        }

        SECTION("textured")
        {
            tgui::Texture textureNormal("resources/Texture1.png");
            tgui::Texture textureHover("resources/Texture2.png");
            tgui::Texture textureDown("resources/Texture3.png");
            tgui::Texture textureDisabled("resources/Texture4.png");
            tgui::Texture textureFocused("resources/Texture5.png");

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("Texture", tgui::Serializer::serialize(textureNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureHover", tgui::Serializer::serialize(textureHover)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureDown", tgui::Serializer::serialize(textureDown)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureDisabled", tgui::Serializer::serialize(textureDisabled)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureFocused", tgui::Serializer::serialize(textureFocused)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("Texture", textureNormal));
                REQUIRE_NOTHROW(renderer->setProperty("TextureHover", textureHover));
                REQUIRE_NOTHROW(renderer->setProperty("TextureDown", textureDown));
                REQUIRE_NOTHROW(renderer->setProperty("TextureDisabled", textureDisabled));
                REQUIRE_NOTHROW(renderer->setProperty("TextureFocused", textureFocused));
            }

            SECTION("functions")
            {
                renderer->setTexture(textureNormal);
                renderer->setTextureHover(textureHover);
                renderer->setTextureDown(textureDown);
                renderer->setTextureDisabled(textureDisabled);
                renderer->setTextureFocused(textureFocused);
            }

            REQUIRE(renderer->getProperty("Texture").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureHover").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureDown").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureDisabled").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureFocused").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTexture().getData() == textureNormal.getData());
            REQUIRE(renderer->getTextureHover().getData() == textureHover.getData());
            REQUIRE(renderer->getTextureDown().getData() == textureDown.getData());
            REQUIRE(renderer->getTextureDisabled().getData() == textureDisabled.getData());
            REQUIRE(renderer->getTextureFocused().getData() == textureFocused.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        button->setText("SomeText");
        button->setTextSize(25);
        button->setImage("resources/image.png");
        button->setImageScaling(0.8f);

        testSavingWidget("BitmapButton", button);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(120, 35, button)

        button->setEnabled(true);
        button->setPosition(10, 5);
        button->setSize(100, 26);
        button->setTextSize(16);

        tgui::ButtonRenderer renderer = tgui::RendererData::create();
        renderer.setTextColor(tgui::Color::Red);
        renderer.setBackgroundColor(tgui::Color::Green);
        renderer.setBorderColor(tgui::Color::Blue);
        renderer.setTextStyle(tgui::TextStyle::Italic);
        renderer.setBorders({1, 2, 3, 4});
        renderer.setOpacity(0.7f);
        button->setRenderer(renderer.getData());

        auto setHoverRenderer = [&](bool textured){
                                        renderer.setTextColorHover(tgui::Color::Magenta);
                                        renderer.setBackgroundColorHover(tgui::Color::Cyan);
                                        renderer.setBorderColorHover(tgui::Color::Yellow);
                                        renderer.setTextStyleHover(tgui::TextStyle::Bold);
                                        if (textured)
                                            renderer.setTextureHover("resources/Texture2.png");
                                     };

        auto setDownRenderer = [&](bool textured){
                                        renderer.setTextColorDown(tgui::Color::Black);
                                        renderer.setBackgroundColorDown(tgui::Color::White);
                                        renderer.setBorderColorDown({128, 128, 128});
                                        renderer.setTextStyleDown(tgui::TextStyle::Underlined);
                                        renderer.setTextColorDisabled({128, 128, 0});
                                        renderer.setBackgroundColorDisabled({0, 128, 128});
                                        renderer.setBorderColorDisabled({128, 0, 128});
                                        renderer.setTextStyleDisabled(tgui::TextStyle::StrikeThrough);
                                        if (textured)
                                            renderer.setTextureDown("resources/Texture3.png");
                                    };

        auto setDisabledRenderer = [&](bool textured){
                                        renderer.setTextColorDisabled({128, 128, 0});
                                        renderer.setBackgroundColorDisabled({0, 128, 128});
                                        renderer.setBorderColorDisabled({128, 0, 128});
                                        renderer.setTextStyleDisabled(tgui::TextStyle::StrikeThrough);
                                        if (textured)
                                            renderer.setTextureDisabled("resources/Texture4.png");
                                    };

        const auto mousePos = button->getPosition() + (button->getSize() / 2.f);

        SECTION("No image or text")
        {
            TEST_DRAW("BitmapButton_NoImageNoText.png")
        }

        SECTION("No image")
        {
            button->setText("Caption");
            TEST_DRAW("BitmapButton_NoImage.png")
        }

        button->setImage("resources/image.png");
        button->setImageScaling(0.8f);

        SECTION("No text")
        {
            TEST_DRAW("BitmapButton_NoText.png")
        }

        button->setText("Caption");

        SECTION("Colored")
        {
            SECTION("NormalState")
            {
                TEST_DRAW("BitmapButton_Normal_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("BitmapButton_Normal_HoverSet.png")

                    SECTION("DownSet")
                    {
                        setDownRenderer(false);
                        TEST_DRAW("BitmapButton_Normal_DownSet.png")
                    }
                }
            }

            SECTION("HoverState")
            {
                button->mouseMoved(mousePos);

                TEST_DRAW("BitmapButton_Hover_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("BitmapButton_Hover_HoverSet.png")

                    SECTION("DownSet")
                    {
                        setDownRenderer(false);
                        TEST_DRAW("BitmapButton_Hover_DownSet.png")
                    }
                }
            }

            SECTION("DownState")
            {
                button->mouseMoved(mousePos);
                button->leftMousePressed(mousePos);

                TEST_DRAW("BitmapButton_Down_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("BitmapButton_Down_HoverSet.png")

                    SECTION("DownSet")
                    {
                        setDownRenderer(false);
                        TEST_DRAW("BitmapButton_Down_DownSet.png")
                    }
                }
            }

            SECTION("DisabledState")
            {
                button->setEnabled(false);

                TEST_DRAW("BitmapButton_Disabled_NormalSet.png")

                SECTION("DisabledSet")
                {
                    setDisabledRenderer(false);
                    TEST_DRAW("BitmapButton_Disabled_DisabledSet.png")
                }
            }
        }

        SECTION("Textured")
        {
            renderer.setTexture("resources/Texture1.png");

            SECTION("NormalState")
            {
                TEST_DRAW("BitmapButton_Normal_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("BitmapButton_Normal_TextureHoverSet.png")

                    SECTION("DownSet")
                    {
                        setDownRenderer(true);
                        TEST_DRAW("BitmapButton_Normal_TextureDownSet.png")
                    }
                }
            }

            SECTION("HoverState")
            {
                button->mouseMoved(mousePos);

                TEST_DRAW("BitmapButton_Hover_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("BitmapButton_Hover_TextureHoverSet.png")

                    SECTION("DownSet")
                    {
                        setDownRenderer(true);
                        TEST_DRAW("BitmapButton_Hover_TextureDownSet.png")
                    }
                }
            }

            SECTION("DownState")
            {
                button->mouseMoved(mousePos);
                button->leftMousePressed(mousePos);

                TEST_DRAW("BitmapButton_Down_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("BitmapButton_Down_TextureHoverSet.png")

                    SECTION("DownSet")
                    {
                        setDownRenderer(true);
                        TEST_DRAW("BitmapButton_Down_TextureDownSet.png")
                    }
                }
            }

            SECTION("DisabledState")
            {
                button->setEnabled(false);

                TEST_DRAW("BitmapButton_Disabled_TextureNormalSet.png")

                SECTION("DisabledSet")
                {
                    setDisabledRenderer(true);
                    TEST_DRAW("BitmapButton_Disabled_TextureDisabledSet.png")
                }
            }
        }

        SECTION("Non-square image")
        {
            button->setImage("resources/TextureRect3.png");
            TEST_DRAW("BitmapButton_NonSquareImage.png")

            button->setText("");
            TEST_DRAW("BitmapButton_NonSquareImage_NoText.png")
        }
    }
}
