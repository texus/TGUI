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
#include <TGUI/Widgets/RadioButton.hpp>
#include <TGUI/Widgets/RadioButtonGroup.hpp>

TEST_CASE("[RadioButton]")
{
    tgui::RadioButton::Ptr radioButton = tgui::RadioButton::create();
    radioButton->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(radioButton->connect("Checked", [](){}));
        REQUIRE_NOTHROW(radioButton->connect("Checked", [](bool){}));
        REQUIRE_NOTHROW(radioButton->connect("Checked", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(radioButton->connect("Checked", [](tgui::Widget::Ptr, std::string, bool){}));

        REQUIRE_NOTHROW(radioButton->connect("Unchecked", [](){}));
        REQUIRE_NOTHROW(radioButton->connect("Unchecked", [](bool){}));
        REQUIRE_NOTHROW(radioButton->connect("Unchecked", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(radioButton->connect("Unchecked", [](tgui::Widget::Ptr, std::string, bool){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(radioButton->getWidgetType() == "RadioButton");
    }

    SECTION("Position and Size")
    {
        radioButton->setPosition(40, 30);
        radioButton->setSize(50, 60);
        radioButton->getRenderer()->setBorders(2);

        REQUIRE(radioButton->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(radioButton->getSize() == sf::Vector2f(50, 60));
        REQUIRE(radioButton->getFullSize() == radioButton->getSize());
        REQUIRE(radioButton->getWidgetOffset() == sf::Vector2f(0, 0));
    }

    SECTION("Checked")
    {
        REQUIRE(!radioButton->isChecked());
        radioButton->setChecked(true);
        REQUIRE(radioButton->isChecked());
        radioButton->setChecked(false);
        REQUIRE(!radioButton->isChecked());

        // Checking a radio button will uncheck all radio buttons with the same parent
        auto parent1 = tgui::RadioButtonGroup::create();
        auto parent2 = tgui::RadioButtonGroup::create();
        auto radioButton1 = tgui::RadioButton::create();
        auto radioButton2 = tgui::RadioButton::create();
        auto radioButton3 = tgui::RadioButton::create();
        parent1->add(radioButton1);
        parent1->add(radioButton2);
        parent2->add(radioButton3);
        parent2->add(parent1);
        REQUIRE(!radioButton1->isChecked());
        REQUIRE(!radioButton2->isChecked());
        REQUIRE(!radioButton3->isChecked());
        radioButton1->setChecked(true);
        REQUIRE(radioButton1->isChecked());
        REQUIRE(!radioButton2->isChecked());
        REQUIRE(!radioButton3->isChecked());
        radioButton2->setChecked(true);
        REQUIRE(!radioButton1->isChecked());
        REQUIRE(radioButton2->isChecked());
        REQUIRE(!radioButton3->isChecked());
        radioButton3->setChecked(true);
        REQUIRE(!radioButton1->isChecked());
        REQUIRE(radioButton2->isChecked());
        REQUIRE(radioButton3->isChecked());

        parent1->uncheckRadioButtons();
        REQUIRE(!radioButton1->isChecked());
        REQUIRE(!radioButton2->isChecked());
        REQUIRE(radioButton3->isChecked());
    }
    
    SECTION("Text")
    {
        REQUIRE(radioButton->getText() == "");
        radioButton->setText("SomeText");
        REQUIRE(radioButton->getText() == "SomeText");
    }

    SECTION("TextSize")
    {
        radioButton->setTextSize(25);
        REQUIRE(radioButton->getTextSize() == 25);
    }

    SECTION("TextClickable")
    {
        REQUIRE(radioButton->isTextClickable());

        radioButton->setPosition(40, 30);
        radioButton->setSize(50, 60);

        SECTION("With text")
        {
            radioButton->setText("Test");

            radioButton->setTextClickable(false);
            REQUIRE(!radioButton->isTextClickable());
            REQUIRE(!radioButton->mouseOnWidget({110, 60}));

            radioButton->setTextClickable(true);
            REQUIRE(radioButton->isTextClickable());
            REQUIRE(radioButton->mouseOnWidget({110, 60}));
        }

        SECTION("Without text")
        {
            radioButton->setTextClickable(false);
            REQUIRE(!radioButton->isTextClickable());
            REQUIRE(!radioButton->mouseOnWidget({110, 60}));

            radioButton->setTextClickable(true);
            REQUIRE(radioButton->isTextClickable());
            REQUIRE(!radioButton->mouseOnWidget({110, 60}));
        }
    }

    SECTION("Events / Signals")
    {
        SECTION("ClickableWidget")
        {
            testClickableWidgetSignals(radioButton);
        }

        SECTION("Check / Uncheck")
        {
            radioButton->setPosition(40, 30);
            radioButton->setSize(50, 60);

            unsigned int checkCount = 0;
            unsigned int uncheckCount = 0;
            radioButton->connect("Checked", &genericCallback, std::ref(checkCount));
            radioButton->connect("Unchecked", &genericCallback, std::ref(uncheckCount));

            radioButton->leftMousePressed({105, 90});
            REQUIRE(checkCount == 0);
            REQUIRE(uncheckCount == 0);

            radioButton->leftMouseReleased({105, 90});
            REQUIRE(checkCount == 1);
            REQUIRE(uncheckCount == 0);

            // Radio buttons can't be unchecked by user interaction
            radioButton->leftMousePressed({105, 90});
            radioButton->leftMouseReleased({105, 90});
            REQUIRE(checkCount == 1);
            REQUIRE(uncheckCount == 0);

            // Programmably unchecking is however possible
            radioButton->setChecked(false);
            REQUIRE(checkCount == 1);
            REQUIRE(uncheckCount == 1);

            SECTION("Key pressed")
            {
                sf::Event::KeyEvent keyEvent;
                keyEvent.alt = false;
                keyEvent.control = false;
                keyEvent.shift = false;
                keyEvent.system = false;

                keyEvent.code = sf::Keyboard::Space;
                radioButton->keyPressed(keyEvent);
                REQUIRE(checkCount == 2);

                radioButton->setChecked(false);

                keyEvent.code = sf::Keyboard::Return;
                radioButton->keyPressed(keyEvent);
                REQUIRE(checkCount == 3);
            }
        }
    }

    testWidgetRenderer(radioButton->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = radioButton->getRenderer();

        SECTION("colored")
        {
            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(50, 60, 70)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", "rgb(60, 70, 80)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDisabled", "rgb(130, 140, 150)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorChecked", "rgb(140, 150, 160)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorCheckedHover", "rgb(150, 160, 170)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorCheckedDisabled", "rgb(160, 170, 180)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", "rgb(80, 90, 100)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDisabled", "rgb(170, 180, 190)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorChecked", "rgb(180, 190, 200)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorCheckedHover", "rgb(190, 200, 210)"));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorCheckedDisabled", "rgb(200, 210, 220)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(90, 100, 110)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorHover", "rgb(100, 110, 120)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDisabled", "rgb(210, 220, 230)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorChecked", "rgb(220, 230, 240)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorCheckedHover", "rgb(230, 240, 250)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorCheckedDisabled", "rgb(250, 240, 230)"));
                REQUIRE_NOTHROW(renderer->setProperty("CheckColor", "rgb(110, 120, 130)"));
                REQUIRE_NOTHROW(renderer->setProperty("CheckColorHover", "rgb(120, 130, 140)"));
                REQUIRE_NOTHROW(renderer->setProperty("CheckColorDisabled", "rgb(240, 230, 220)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyle", "Italic"));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleChecked", "Bold"));
                REQUIRE_NOTHROW(renderer->setProperty("TextDistanceRatio", "0.5"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", sf::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDisabled", sf::Color{130, 140, 150}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorChecked", sf::Color{140, 150, 160}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorCheckedHover", sf::Color{150, 160, 170}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorCheckedDisabled", sf::Color{160, 170, 180}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", sf::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDisabled", sf::Color{170, 180, 190}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorChecked", sf::Color{180, 190, 200}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorCheckedHover", sf::Color{190, 200, 210}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorCheckedDisabled", sf::Color{200, 210, 220}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{90, 100, 110}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorHover", sf::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDisabled", sf::Color{210, 220, 230}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorChecked", sf::Color{220, 230, 240}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorCheckedHover", sf::Color{230, 240, 250}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorCheckedDisabled", sf::Color{250, 240, 230}));
                REQUIRE_NOTHROW(renderer->setProperty("CheckColor", sf::Color{110, 120, 130}));
                REQUIRE_NOTHROW(renderer->setProperty("CheckColorHover", sf::Color{120, 130, 140}));
                REQUIRE_NOTHROW(renderer->setProperty("CheckColorDisabled", sf::Color{240, 230, 220}));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyle", sf::Text::Italic));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleChecked", sf::Text::Bold));
                REQUIRE_NOTHROW(renderer->setProperty("TextDistanceRatio", 0.5));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
            }

            SECTION("functions")
            {
                renderer->setTextColor({50, 60, 70});
                renderer->setTextColorHover({60, 70, 80});
                renderer->setTextColorDisabled({130, 140, 150});
                renderer->setTextColorChecked({140, 150, 160});
                renderer->setTextColorCheckedHover({150, 160, 170});
                renderer->setTextColorCheckedDisabled({160, 170, 180});
                renderer->setBackgroundColor({70, 80, 90});
                renderer->setBackgroundColorHover({80, 90, 100});
                renderer->setBackgroundColorDisabled({170, 180, 190});
                renderer->setBackgroundColorChecked({180, 190, 200});
                renderer->setBackgroundColorCheckedHover({190, 200, 210});
                renderer->setBackgroundColorCheckedDisabled({200, 210, 220});
                renderer->setBorderColor({90, 100, 110});
                renderer->setBorderColorHover({100, 110, 120});
                renderer->setBorderColorDisabled({210, 220, 230});
                renderer->setBorderColorChecked({220, 230, 240});
                renderer->setBorderColorCheckedHover({230, 240, 250});
                renderer->setBorderColorCheckedDisabled({250, 240, 230});
                renderer->setCheckColor({110, 120, 130});
                renderer->setCheckColorHover({120, 130, 140});
                renderer->setCheckColorDisabled({240, 230, 220});
                renderer->setTextStyle(sf::Text::Italic);
                renderer->setTextStyleChecked(sf::Text::Bold);
                renderer->setTextDistanceRatio(0.5);
                renderer->setBorders({1, 2, 3, 4});
            }

            REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("TextColorHover").getColor() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("TextColorDisabled").getColor() == sf::Color(130, 140, 150));
            REQUIRE(renderer->getProperty("TextColorChecked").getColor() == sf::Color(140, 150, 160));
            REQUIRE(renderer->getProperty("TextColorCheckedHover").getColor() == sf::Color(150, 160, 170));
            REQUIRE(renderer->getProperty("TextColorCheckedDisabled").getColor() == sf::Color(160, 170, 180));
            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("BackgroundColorDisabled").getColor() == sf::Color(170, 180, 190));
            REQUIRE(renderer->getProperty("BackgroundColorChecked").getColor() == sf::Color(180, 190, 200));
            REQUIRE(renderer->getProperty("BackgroundColorCheckedHover").getColor() == sf::Color(190, 200, 210));
            REQUIRE(renderer->getProperty("BackgroundColorCheckedDisabled").getColor() == sf::Color(200, 210, 220));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(90, 100, 110));
            REQUIRE(renderer->getProperty("BorderColorHover").getColor() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("BorderColorDisabled").getColor() == sf::Color(210, 220, 230));
            REQUIRE(renderer->getProperty("BorderColorChecked").getColor() == sf::Color(220, 230, 240));
            REQUIRE(renderer->getProperty("BorderColorCheckedHover").getColor() == sf::Color(230, 240, 250));
            REQUIRE(renderer->getProperty("BorderColorCheckedDisabled").getColor() == sf::Color(250, 240, 230));
            REQUIRE(renderer->getProperty("CheckColor").getColor() == sf::Color(110, 120, 130));
            REQUIRE(renderer->getProperty("CheckColorHover").getColor() == sf::Color(120, 130, 140));
            REQUIRE(renderer->getProperty("CheckColorDisabled").getColor() == sf::Color(240, 230, 220));
            REQUIRE(renderer->getProperty("TextStyle").getTextStyle() == sf::Text::Italic);
            REQUIRE(renderer->getProperty("TextStyleChecked").getTextStyle() == sf::Text::Bold);
            REQUIRE(renderer->getProperty("TextDistanceRatio").getNumber() == 0.5);
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));

            REQUIRE(renderer->getTextColor() == sf::Color(50, 60, 70));
            REQUIRE(renderer->getTextColorHover() == sf::Color(60, 70, 80));
            REQUIRE(renderer->getTextColorDisabled() == sf::Color(130, 140, 150));
            REQUIRE(renderer->getTextColorChecked() == sf::Color(140, 150, 160));
            REQUIRE(renderer->getTextColorCheckedHover() == sf::Color(150, 160, 170));
            REQUIRE(renderer->getTextColorCheckedDisabled() == sf::Color(160, 170, 180));
            REQUIRE(renderer->getBackgroundColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getBackgroundColorHover() == sf::Color(80, 90, 100));
            REQUIRE(renderer->getBackgroundColorDisabled() == sf::Color(170, 180, 190));
            REQUIRE(renderer->getBackgroundColorChecked() == sf::Color(180, 190, 200));
            REQUIRE(renderer->getBackgroundColorCheckedHover() == sf::Color(190, 200, 210));
            REQUIRE(renderer->getBackgroundColorCheckedDisabled() == sf::Color(200, 210, 220));
            REQUIRE(renderer->getBorderColor() == sf::Color(90, 100, 110));
            REQUIRE(renderer->getBorderColorHover() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getBorderColorDisabled() == sf::Color(210, 220, 230));
            REQUIRE(renderer->getBorderColorChecked() == sf::Color(220, 230, 240));
            REQUIRE(renderer->getBorderColorCheckedHover() == sf::Color(230, 240, 250));
            REQUIRE(renderer->getBorderColorCheckedDisabled() == sf::Color(250, 240, 230));
            REQUIRE(renderer->getCheckColor() == sf::Color(110, 120, 130));
            REQUIRE(renderer->getCheckColorHover() == sf::Color(120, 130, 140));
            REQUIRE(renderer->getCheckColorDisabled() == sf::Color(240, 230, 220));
            REQUIRE(renderer->getTextStyle() == sf::Text::Italic);
            REQUIRE(renderer->getTextStyleChecked() == sf::Text::Bold);
            REQUIRE(renderer->getTextDistanceRatio() == 0.5);
            REQUIRE(renderer->getBorders() == tgui::Borders(1, 2, 3, 4));
        }

        SECTION("textured")
        {
            tgui::Texture textureUncheckedNormal("resources/Black.png", {124, 32, 32, 32});
            tgui::Texture textureUncheckedHover("resources/Black.png", {188, 32, 32, 32});
            tgui::Texture textureUncheckedDisabled("resources/Black.png", {124, 32, 32, 32});
            tgui::Texture textureCheckedNormal("resources/Black.png", {156, 32, 32, 32});
            tgui::Texture textureCheckedHover("resources/Black.png", {220, 32, 32, 32});
            tgui::Texture textureCheckedDisabled("resources/Black.png", {156, 32, 32, 32});

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureUnchecked", tgui::Serializer::serialize(textureUncheckedNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureUncheckedHover", tgui::Serializer::serialize(textureUncheckedHover)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureUncheckedDisabled", tgui::Serializer::serialize(textureUncheckedDisabled)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureChecked", tgui::Serializer::serialize(textureCheckedNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureCheckedHover", tgui::Serializer::serialize(textureCheckedHover)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureCheckedDisabled", tgui::Serializer::serialize(textureCheckedDisabled)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureUnchecked", textureUncheckedNormal));
                REQUIRE_NOTHROW(renderer->setProperty("TextureUncheckedHover", textureUncheckedHover));
                REQUIRE_NOTHROW(renderer->setProperty("TextureUncheckedDisabled", textureUncheckedDisabled));
                REQUIRE_NOTHROW(renderer->setProperty("TextureChecked", textureCheckedNormal));
                REQUIRE_NOTHROW(renderer->setProperty("TextureCheckedHover", textureCheckedHover));
                REQUIRE_NOTHROW(renderer->setProperty("TextureCheckedDisabled", textureCheckedDisabled));
            }

            SECTION("functions")
            {
                renderer->setTextureUnchecked(textureUncheckedNormal);
                renderer->setTextureUncheckedHover(textureUncheckedHover);
                renderer->setTextureUncheckedDisabled(textureUncheckedDisabled);
                renderer->setTextureChecked(textureCheckedNormal);
                renderer->setTextureCheckedHover(textureCheckedHover);
                renderer->setTextureCheckedDisabled(textureCheckedDisabled);
            }

            REQUIRE(renderer->getProperty("TextureUnchecked").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureUncheckedHover").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureUncheckedDisabled").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureChecked").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureCheckedHover").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureCheckedDisabled").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTextureUnchecked().getData() == textureUncheckedNormal.getData());
            REQUIRE(renderer->getTextureUncheckedHover().getData() == textureUncheckedHover.getData());
            REQUIRE(renderer->getTextureUncheckedDisabled().getData() == textureUncheckedDisabled.getData());
            REQUIRE(renderer->getTextureChecked().getData() == textureCheckedNormal.getData());
            REQUIRE(renderer->getTextureCheckedHover().getData() == textureCheckedHover.getData());
            REQUIRE(renderer->getTextureCheckedDisabled().getData() == textureCheckedDisabled.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        radioButton->setChecked(true);
        radioButton->setText("SomeText");
        radioButton->setTextSize(25);
        radioButton->setTextClickable(false);

        testSavingWidget("RadioButton", radioButton);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(150, 35, radioButton)

        radioButton->setEnabled(true);
        radioButton->setPosition(10, 5);
        radioButton->setSize(25, 25);
        radioButton->setText("Check me!");
        radioButton->setTextSize(16);

        tgui::RadioButtonRenderer renderer = tgui::RendererData::create();
        renderer.setTextColor(sf::Color::Red);
        renderer.setTextColorChecked({128, 0, 0});
        renderer.setBackgroundColor(sf::Color::Green);
        renderer.setBackgroundColorChecked({0, 128, 0});
        renderer.setBorderColor(sf::Color::Blue);
        renderer.setBorderColorChecked({0, 0, 128});
        renderer.setTextStyle(sf::Text::Style::Italic);
        renderer.setTextStyleChecked(tgui::TextStyle{sf::Text::Style::Bold | sf::Text::Style::StrikeThrough});
        renderer.setOpacity(0.7f);
        radioButton->setRenderer(renderer.getData());

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

        const auto mousePos = radioButton->getPosition() + (radioButton->getSize() / 2.f);

        SECTION("Colored")
        {
            renderer.setBorders({2});

            SECTION("NormalState")
            {
                TEST_DRAW("RadioButton_Normal_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("RadioButton_Normal_HoverSet.png")
                }
            }

            SECTION("CheckedNormalState")
            {
                radioButton->setChecked(true);

                TEST_DRAW("RadioButton_CheckedNormal_NormalSet.png")

                SECTION("HoverSet")
                {
                    setCheckedHoverRenderer(false);
                    TEST_DRAW("RadioButton_CheckedNormal_HoverSet.png")
                }
            }

            SECTION("HoverState")
            {
                radioButton->mouseMoved(mousePos);

                TEST_DRAW("RadioButton_Hover_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("RadioButton_Hover_HoverSet.png")
                }
            }

            SECTION("CheckedHoverState")
            {
                radioButton->setChecked(true);
                radioButton->mouseMoved(mousePos);

                TEST_DRAW("RadioButton_CheckedHover_NormalSet.png")

                SECTION("HoverSet")
                {
                    setCheckedHoverRenderer(false);
                    TEST_DRAW("RadioButton_CheckedHover_HoverSet.png")
                }
            }

            SECTION("DisabledState")
            {
                radioButton->setEnabled(false);

                TEST_DRAW("RadioButton_Disabled_NormalSet.png")

                SECTION("DisabledSet")
                {
                    setDisabledRenderer(false);
                    TEST_DRAW("RadioButton_Disabled_DisabledSet.png")
                }
            }

            SECTION("CheckedDisabledState")
            {
                radioButton->setChecked(true);
                radioButton->setEnabled(false);

                TEST_DRAW("RadioButton_CheckedDisabled_NormalSet.png")

                SECTION("DisabledSet")
                {
                    setCheckedDisabledRenderer(false);
                    TEST_DRAW("RadioButton_CheckedDisabled_DisabledSet.png")
                }
            }
        }

        SECTION("Textured")
        {
            renderer.setTextureUnchecked("resources/Texture1.png");
            renderer.setTextureChecked("resources/Texture2.png");

            SECTION("NormalState")
            {
                TEST_DRAW("RadioButton_Normal_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("RadioButton_Normal_TextureHoverSet.png")
                }
            }

            SECTION("CheckedNormalState")
            {
                radioButton->setChecked(true);

                TEST_DRAW("RadioButton_CheckedNormal_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setCheckedHoverRenderer(true);
                    TEST_DRAW("RadioButton_CheckedNormal_TextureHoverSet.png")
                }
            }

            SECTION("HoverState")
            {
                radioButton->mouseMoved(mousePos);

                TEST_DRAW("RadioButton_Hover_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("RadioButton_Hover_TextureHoverSet.png")
                }
            }

            SECTION("CheckedHoverState")
            {
                radioButton->setChecked(true);
                radioButton->mouseMoved(mousePos);

                TEST_DRAW("RadioButton_CheckedHover_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setCheckedHoverRenderer(true);
                    TEST_DRAW("RadioButton_CheckedHover_TextureHoverSet.png")
                }
            }

            SECTION("DisabledState")
            {
                radioButton->setEnabled(false);

                TEST_DRAW("RadioButton_Disabled_TextureNormalSet.png")

                SECTION("DisabledSet")
                {
                    setDisabledRenderer(true);
                    TEST_DRAW("RadioButton_Disabled_TextureDisabledSet.png")
                }
            }

            SECTION("CheckedDisabledState")
            {
                radioButton->setChecked(true);
                radioButton->setEnabled(false);

                TEST_DRAW("RadioButton_CheckedDisabled_TextureNormalSet.png")

                SECTION("DisabledSet")
                {
                    setCheckedDisabledRenderer(true);
                    TEST_DRAW("RadioButton_CheckedDisabled_TextureDisabledSet.png")
                }
            }
        }
    }
}
