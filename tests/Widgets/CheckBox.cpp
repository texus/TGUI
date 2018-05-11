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
#include <TGUI/Widgets/CheckBox.hpp>

TEST_CASE("[CheckBox]")
{
    tgui::CheckBox::Ptr checkBox = tgui::CheckBox::create();
    checkBox->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(checkBox->connect("Checked", [](){}));
        REQUIRE_NOTHROW(checkBox->connect("Checked", [](bool){}));
        REQUIRE_NOTHROW(checkBox->connect("Checked", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(checkBox->connect("Checked", [](tgui::Widget::Ptr, std::string, bool){}));

        REQUIRE_NOTHROW(checkBox->connect("Unchecked", [](){}));
        REQUIRE_NOTHROW(checkBox->connect("Unchecked", [](bool){}));
        REQUIRE_NOTHROW(checkBox->connect("Unchecked", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(checkBox->connect("Unchecked", [](tgui::Widget::Ptr, std::string, bool){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(checkBox->getWidgetType() == "CheckBox");
    }

    SECTION("Position and Size")
    {
        checkBox->setPosition(40, 30);
        checkBox->setSize(36, 36);
        checkBox->getRenderer()->setBorders(2);

        SECTION("Colored")
        {
            REQUIRE(checkBox->getPosition() == sf::Vector2f(40, 30));
            REQUIRE(checkBox->getSize() == sf::Vector2f(36, 36));
            REQUIRE(checkBox->getFullSize() == checkBox->getSize());
            REQUIRE(checkBox->getWidgetOffset() == sf::Vector2f(0, 0));

            SECTION("With text")
            {
                checkBox->setText("Test");

                SECTION("Small text")
                {
                    checkBox->setTextSize(20);
                    REQUIRE(checkBox->getSize() == sf::Vector2f(36, 36));
                    REQUIRE(checkBox->getFullSize().x > checkBox->getSize().x);
                    REQUIRE(checkBox->getFullSize().y == checkBox->getSize().y);
                    REQUIRE(checkBox->getWidgetOffset() == sf::Vector2f(0, 0));
                }

                SECTION("Large text")
                {
                    checkBox->setTextSize(50);
                    REQUIRE(checkBox->getSize() == sf::Vector2f(36, 36));
                    REQUIRE(checkBox->getFullSize().x > checkBox->getSize().x);
                    REQUIRE(checkBox->getFullSize().y > checkBox->getSize().y);
                    REQUIRE(checkBox->getWidgetOffset().x == 0);
                    REQUIRE(checkBox->getWidgetOffset().y < 0);
                }
            }
        }

        SECTION("Textured")
        {
            SECTION("Unchecked and Checked images of same size")
            {
                checkBox->getRenderer()->setTextureUnchecked({"resources/CheckBox1.png", {0, 0, 32, 32}});
                checkBox->getRenderer()->setTextureChecked({"resources/CheckBox1.png", {32, 0, 32, 32}});

                REQUIRE(checkBox->getPosition() == sf::Vector2f(40, 30));
                REQUIRE(checkBox->getSize() == sf::Vector2f(36, 36));
                REQUIRE(checkBox->getFullSize() == checkBox->getSize());
                REQUIRE(checkBox->getWidgetOffset() == sf::Vector2f(0, 0));

                SECTION("With text")
                {
                    checkBox->setText("Test");

                    SECTION("Small text")
                    {
                        checkBox->setTextSize(20);
                        REQUIRE(checkBox->getSize() == sf::Vector2f(36, 36));
                        REQUIRE(checkBox->getFullSize().x > checkBox->getSize().x);
                        REQUIRE(checkBox->getFullSize().y == checkBox->getSize().y);
                        REQUIRE(checkBox->getWidgetOffset() == sf::Vector2f(0, 0));
                    }

                    SECTION("Large text")
                    {
                        checkBox->setTextSize(50);
                        REQUIRE(checkBox->getSize() == sf::Vector2f(36, 36));
                        REQUIRE(checkBox->getFullSize().x > checkBox->getSize().x);
                        REQUIRE(checkBox->getFullSize().y > checkBox->getSize().y);
                        REQUIRE(checkBox->getWidgetOffset().x == 0);
                        REQUIRE(checkBox->getWidgetOffset().y < 0);
                    }
                }
            }

            SECTION("Unchecked and Checked images of different sizes")
            {
                checkBox->getRenderer()->setTextureUnchecked({"resources/CheckBox2.png", {0, 8, 32, 32}});
                checkBox->getRenderer()->setTextureChecked({"resources/CheckBox2.png", {32, 0, 38, 40}});

                REQUIRE(checkBox->getPosition() == sf::Vector2f(40, 30));
                REQUIRE(checkBox->getSize() == sf::Vector2f(36, 36));
                REQUIRE(checkBox->getFullSize() == sf::Vector2f(40, 42));
                REQUIRE(checkBox->getWidgetOffset() == sf::Vector2f(0, -6));

                SECTION("With text")
                {
                    checkBox->setText("Test");

                    SECTION("Small text")
                    {
                        checkBox->setTextSize(20);
                        REQUIRE(checkBox->getSize() == sf::Vector2f(36, 36));
                        REQUIRE(checkBox->getFullSize().x > 40);
                        REQUIRE(checkBox->getFullSize().y == 42);
                        REQUIRE(checkBox->getWidgetOffset() == sf::Vector2f(0, -6));
                    }

                    SECTION("Large text")
                    {
                        checkBox->setTextSize(50);
                        REQUIRE(checkBox->getSize() == sf::Vector2f(36, 36));
                        REQUIRE(checkBox->getFullSize().x > 40);
                        REQUIRE(checkBox->getFullSize().y > 42);
                        REQUIRE(checkBox->getWidgetOffset().x == 0);
                        REQUIRE(checkBox->getWidgetOffset().y < -6);
                    }
                }
            }
        }
    }

    SECTION("Checked")
    {
        REQUIRE(!checkBox->isChecked());
        checkBox->setChecked(true);
        REQUIRE(checkBox->isChecked());
        checkBox->setChecked(false);
        REQUIRE(!checkBox->isChecked());
    }

    SECTION("Events / Signals")
    {
        SECTION("ClickableWidget")
        {
            testClickableWidgetSignals(checkBox);
        }

        SECTION("Check / Uncheck")
        {
            checkBox->setPosition(40, 30);
            checkBox->setSize(50, 60);

            unsigned int checkCount = 0;
            unsigned int uncheckCount = 0;
            checkBox->connect("Checked", &genericCallback, std::ref(checkCount));
            checkBox->connect("Unchecked", &genericCallback, std::ref(uncheckCount));

            checkBox->setChecked(true);
            REQUIRE(checkCount == 1);
            REQUIRE(uncheckCount == 0);

            checkBox->setChecked(false);
            REQUIRE(checkCount == 1);
            REQUIRE(uncheckCount == 1);

            checkBox->leftMousePressed({65, 60});
            REQUIRE(checkCount == 1);
            REQUIRE(uncheckCount == 1);

            checkBox->leftMouseReleased({65, 60});
            REQUIRE(checkCount == 2);
            REQUIRE(uncheckCount == 1);

            checkBox->leftMousePressed({65, 60});
            checkBox->leftMouseReleased({65, 60});
            REQUIRE(checkCount == 2);
            REQUIRE(uncheckCount == 2);

            SECTION("Key pressed")
            {
                sf::Event::KeyEvent keyEvent;
                keyEvent.alt = false;
                keyEvent.control = false;
                keyEvent.shift = false;
                keyEvent.system = false;

                keyEvent.code = sf::Keyboard::Space;
                checkBox->keyPressed(keyEvent);
                REQUIRE(checkCount == 3);
                REQUIRE(uncheckCount == 2);

                keyEvent.code = sf::Keyboard::Return;
                checkBox->keyPressed(keyEvent);
                REQUIRE(checkCount == 3);
                REQUIRE(uncheckCount == 3);
            }
        }
    }

    SECTION("Saving and loading from file")
    {
        checkBox->setChecked(true);
        checkBox->setText("SomeText");
        checkBox->setTextSize(25);
        checkBox->setTextClickable(false);

        testSavingWidget("CheckBox", checkBox);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(150, 35, checkBox)

        checkBox->setEnabled(true);
        checkBox->setPosition(10, 5);
        checkBox->setSize(25, 25);
        checkBox->setText("Check me!");
        checkBox->setTextSize(16);

        tgui::RadioButtonRenderer renderer = tgui::RendererData::create();
        renderer.setTextColor(sf::Color::Red);
        renderer.setTextColorChecked({128, 0, 0});
        renderer.setBackgroundColor(sf::Color::Green);
        renderer.setBackgroundColorChecked({0, 128, 0});
        renderer.setBorderColor(sf::Color::Blue);
        renderer.setBorderColorChecked({0, 0, 128});
        renderer.setTextStyle(sf::Text::Style::Italic);
        renderer.setTextStyleChecked(tgui::TextStyle{sf::Text::Style::Bold | sf::Text::Style::StrikeThrough});
        renderer.setBorders({2});
        renderer.setOpacity(0.7f);
        checkBox->setRenderer(renderer.getData());

        auto setHoverRenderer = [&](bool textured){
                                        renderer.setTextColorHover(sf::Color::Magenta);
                                        renderer.setBackgroundColorHover(sf::Color::Cyan);
                                        renderer.setBorderColorHover(sf::Color::Yellow);
                                        renderer.setTextStyle(sf::Text::Style::Bold);
                                        if (textured)
                                            renderer.setTextureUncheckedHover("resources/Texture3.png");
                                     };

        auto setDisabledRenderer = [&](bool textured){
                                        renderer.setTextColorDisabled({128, 128, 0});
                                        renderer.setBackgroundColorDisabled({0, 128, 128});
                                        renderer.setBorderColorDisabled({128, 0, 128});
                                        renderer.setTextStyle(sf::Text::Style::Italic);
                                        if (textured)
                                            renderer.setTextureUncheckedDisabled("resources/Texture5.png");
                                    };

        auto setCheckedHoverRenderer = [&](bool textured){
                                            renderer.setTextColorCheckedHover({192, 64, 0});
                                            renderer.setBackgroundColorCheckedHover({0, 192, 64});
                                            renderer.setBorderColorCheckedHover({64, 0, 192});
                                            renderer.setTextStyleChecked(tgui::TextStyle{sf::Text::Style::Bold | sf::Text::Style::Italic});
                                            if (textured)
                                                renderer.setTextureCheckedHover("resources/Texture4.png");
                                         };

        auto setCheckedDisabledRenderer = [&](bool textured){
                                            renderer.setTextColorCheckedDisabled({64, 192, 0});
                                            renderer.setBackgroundColorCheckedDisabled({0, 64, 192});
                                            renderer.setBorderColorCheckedDisabled({192, 0, 64});
                                            renderer.setTextStyleChecked(tgui::TextStyle{sf::Text::Style::Italic | sf::Text::Style::StrikeThrough});
                                            if (textured)
                                                renderer.setTextureCheckedDisabled("resources/Texture6.png");
                                        };

        const auto mousePos = checkBox->getPosition() + (checkBox->getSize() / 2.f);

        SECTION("Colored")
        {
            SECTION("NormalState")
            {
                TEST_DRAW("CheckBox_Normal_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("CheckBox_Normal_HoverSet.png")
                }
            }

            SECTION("CheckedNormalState")
            {
                checkBox->setChecked(true);

                TEST_DRAW("CheckBox_CheckedNormal_NormalSet.png")

                SECTION("HoverSet")
                {
                    setCheckedHoverRenderer(false);
                    TEST_DRAW("CheckBox_CheckedNormal_HoverSet.png")
                }
            }

            SECTION("HoverState")
            {
                checkBox->mouseMoved(mousePos);

                TEST_DRAW("CheckBox_Hover_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("CheckBox_Hover_HoverSet.png")
                }
            }

            SECTION("CheckedHoverState")
            {
                checkBox->setChecked(true);
                checkBox->mouseMoved(mousePos);

                TEST_DRAW("CheckBox_CheckedHover_NormalSet.png")

                SECTION("HoverSet")
                {
                    setCheckedHoverRenderer(false);
                    TEST_DRAW("CheckBox_CheckedHover_HoverSet.png")
                }
            }

            SECTION("DisabledState")
            {
                checkBox->setEnabled(false);

                TEST_DRAW("CheckBox_Disabled_NormalSet.png")

                SECTION("DisabledSet")
                {
                    setDisabledRenderer(false);
                    TEST_DRAW("CheckBox_Disabled_DisabledSet.png")
                }
            }

            SECTION("CheckedDisabledState")
            {
                checkBox->setChecked(true);
                checkBox->setEnabled(false);

                TEST_DRAW("CheckBox_CheckedDisabled_NormalSet.png")

                SECTION("DisabledSet")
                {
                    setCheckedDisabledRenderer(false);
                    TEST_DRAW("CheckBox_CheckedDisabled_DisabledSet.png")
                }
            }
        }

        SECTION("Textured")
        {
            renderer.setTextureUnchecked("resources/Texture1.png");
            renderer.setTextureChecked("resources/Texture2.png");

            SECTION("NormalState")
            {
                TEST_DRAW("CheckBox_Normal_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("CheckBox_Normal_TextureHoverSet.png")
                }
            }

            SECTION("CheckedNormalState")
            {
                checkBox->setChecked(true);

                TEST_DRAW("CheckBox_CheckedNormal_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setCheckedHoverRenderer(true);
                    TEST_DRAW("CheckBox_CheckedNormal_TextureHoverSet.png")
                }
            }

            SECTION("HoverState")
            {
                checkBox->mouseMoved(mousePos);

                TEST_DRAW("CheckBox_Hover_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("CheckBox_Hover_TextureHoverSet.png")
                }
            }

            SECTION("CheckedHoverState")
            {
                checkBox->setChecked(true);
                checkBox->mouseMoved(mousePos);

                TEST_DRAW("CheckBox_CheckedHover_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setCheckedHoverRenderer(true);
                    TEST_DRAW("CheckBox_CheckedHover_TextureHoverSet.png")
                }
            }

            SECTION("DisabledState")
            {
                checkBox->setEnabled(false);

                TEST_DRAW("CheckBox_Disabled_TextureNormalSet.png")

                SECTION("DisabledSet")
                {
                    setDisabledRenderer(true);
                    TEST_DRAW("CheckBox_Disabled_TextureDisabledSet.png")
                }
            }

            SECTION("CheckedDisabledState")
            {
                checkBox->setChecked(true);
                checkBox->setEnabled(false);

                TEST_DRAW("CheckBox_CheckedDisabled_TextureNormalSet.png")

                SECTION("DisabledSet")
                {
                    setCheckedDisabledRenderer(true);
                    TEST_DRAW("CheckBox_CheckedDisabled_TextureDisabledSet.png")
                }
            }
        }
    }
}
