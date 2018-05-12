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
#include <TGUI/Widgets/BitmapButton.hpp>

TEST_CASE("[BitmapButton]")
{
    tgui::BitmapButton::Ptr button = tgui::BitmapButton::create();
    button->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(button->connect("Pressed", [](){}));
        REQUIRE_NOTHROW(button->connect("Pressed", [](sf::String){}));
        REQUIRE_NOTHROW(button->connect("Pressed", [](std::string){}));
        REQUIRE_NOTHROW(button->connect("Pressed", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(button->connect("Pressed", [](tgui::Widget::Ptr, std::string, sf::String){}));
        REQUIRE_NOTHROW(button->connect("Pressed", [](tgui::Widget::Ptr, std::string, std::string){}));
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

        REQUIRE(button->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(button->getSize() == sf::Vector2f(150, 100));
        REQUIRE(button->getFullSize() == button->getSize());
        REQUIRE(button->getWidgetOffset() == sf::Vector2f(0, 0));
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
            button->connect("Pressed", &genericCallback, std::ref(pressedCount));

            SECTION("mouse click")
            {
                button->leftMouseReleased({115, 80});
                REQUIRE(pressedCount == 0);

                SECTION("mouse press")
                {
                    button->leftMousePressed({115, 80});
                    REQUIRE(pressedCount == 0);
                }

                button->leftMouseReleased({115, 80});
                REQUIRE(pressedCount == 1);
            }

            SECTION("key pressed")
            {
                sf::Event::KeyEvent keyEvent;
                keyEvent.alt = false;
                keyEvent.control = false;
                keyEvent.shift = false;
                keyEvent.system = false;

                keyEvent.code = sf::Keyboard::Space;
                button->keyPressed(keyEvent);
                REQUIRE(pressedCount == 1);

                keyEvent.code = sf::Keyboard::Return;
                button->keyPressed(keyEvent);
                REQUIRE(pressedCount == 2);
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
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{20, 30, 40}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", sf::Color{30, 40, 50}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDown", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDisabled", sf::Color{90, 100, 110}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", sf::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDown", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDisabled", sf::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorHover", sf::Color{110, 120, 130}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDown", sf::Color{120, 130, 140}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDisabled", sf::Color{130, 140, 150}));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyle", sf::Text::Italic));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleHover", sf::Text::Underlined));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleDown", tgui::TextStyle(sf::Text::Bold | sf::Text::Underlined)));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleDisabled", sf::Text::StrikeThrough));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
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
                renderer->setTextStyle(sf::Text::Italic);
                renderer->setTextStyleHover(sf::Text::Underlined);
                renderer->setTextStyleDown(sf::Text::Bold | sf::Text::Underlined);
                renderer->setTextStyleDisabled(sf::Text::StrikeThrough);
                renderer->setBorders({1, 2, 3, 4});
            }

            REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(20, 30, 40));
            REQUIRE(renderer->getProperty("TextColorHover").getColor() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getProperty("TextColorDown").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("TextColorDisabled").getColor() == sf::Color(90, 100, 110));
            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("BackgroundColorDown").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("BackgroundColorDisabled").getColor() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("BorderColorHover").getColor() == sf::Color(110, 120, 130));
            REQUIRE(renderer->getProperty("BorderColorDown").getColor() == sf::Color(120, 130, 140));
            REQUIRE(renderer->getProperty("BorderColorDisabled").getColor() == sf::Color(130, 140, 150));
            REQUIRE(renderer->getProperty("TextStyle").getTextStyle() == sf::Text::Italic);
            REQUIRE(renderer->getProperty("TextStyleHover").getTextStyle() == sf::Text::Underlined);
            REQUIRE(renderer->getProperty("TextStyleDown").getTextStyle() == (sf::Text::Bold | sf::Text::Underlined));
            REQUIRE(renderer->getProperty("TextStyleDisabled").getTextStyle() == sf::Text::StrikeThrough);
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));

            REQUIRE(renderer->getTextColor() == sf::Color(20, 30, 40));
            REQUIRE(renderer->getTextColorHover() == sf::Color(30, 40, 50));
            REQUIRE(renderer->getTextColorDown() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getTextColorDisabled() == sf::Color(90, 100, 110));
            REQUIRE(renderer->getBackgroundColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getBackgroundColorHover() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getBackgroundColorDown() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getBackgroundColorDisabled() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getBorderColor() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getBorderColorHover() == sf::Color(110, 120, 130));
            REQUIRE(renderer->getBorderColorDown() == sf::Color(120, 130, 140));
            REQUIRE(renderer->getBorderColorDisabled() == sf::Color(130, 140, 150));
            REQUIRE(renderer->getTextStyle() == sf::Text::Italic);
            REQUIRE(renderer->getTextStyleHover() == sf::Text::Underlined);
            REQUIRE(renderer->getTextStyleDown() == (sf::Text::Bold | sf::Text::Underlined));
            REQUIRE(renderer->getTextStyleDisabled() == sf::Text::StrikeThrough);
            REQUIRE(renderer->getBorders() == tgui::Borders(1, 2, 3, 4));
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

        testSavingWidget("BitmapButton", button, false);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(120, 35, button)

        button->setEnabled(true);
        button->setPosition(10, 5);
        button->setSize(100, 26);
        button->setTextSize(16);

        tgui::ButtonRenderer renderer = tgui::RendererData::create();
        renderer.setTextColor(sf::Color::Red);
        renderer.setBackgroundColor(sf::Color::Green);
        renderer.setBorderColor(sf::Color::Blue);
        renderer.setTextStyle(sf::Text::Style::Italic);
        renderer.setBorders({1, 2, 3, 4});
        renderer.setOpacity(0.7f);
        button->setRenderer(renderer.getData());

        auto setHoverRenderer = [&](bool textured){
                                        renderer.setTextColorHover(sf::Color::Magenta);
                                        renderer.setBackgroundColorHover(sf::Color::Cyan);
                                        renderer.setBorderColorHover(sf::Color::Yellow);
                                        renderer.setTextStyleHover(sf::Text::Style::Bold);
                                        if (textured)
                                            renderer.setTextureHover("resources/Texture2.png");
                                     };

        auto setDownRenderer = [&](bool textured){
                                        renderer.setTextColorDown(sf::Color::Black);
                                        renderer.setBackgroundColorDown(sf::Color::White);
                                        renderer.setBorderColorDown({128, 128, 128});
                                        renderer.setTextStyleDown(sf::Text::Style::Underlined);
                                        renderer.setTextColorDisabled({128, 128, 0});
                                        renderer.setBackgroundColorDisabled({0, 128, 128});
                                        renderer.setBorderColorDisabled({128, 0, 128});
                                        renderer.setTextStyleDisabled(sf::Text::Style::StrikeThrough);
                                        if (textured)
                                            renderer.setTextureDown("resources/Texture3.png");
                                    };

        auto setDisabledRenderer = [&](bool textured){
                                        renderer.setTextColorDisabled({128, 128, 0});
                                        renderer.setBackgroundColorDisabled({0, 128, 128});
                                        renderer.setBorderColorDisabled({128, 0, 128});
                                        renderer.setTextStyleDisabled(sf::Text::Style::StrikeThrough);
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
    }
}
