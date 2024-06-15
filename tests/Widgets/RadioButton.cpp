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

TEST_CASE("[RadioButton]")
{
    tgui::RadioButton::Ptr radioButton = tgui::RadioButton::create();
    radioButton->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        radioButton->onCheck([](){});
        radioButton->onCheck([](bool){});

        radioButton->onUncheck([](){});
        radioButton->onUncheck([](bool){});

        radioButton->onChange([](){});
        radioButton->onChange([](bool){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(radioButton)->getSignal("Checked").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(radioButton)->getSignal("Unchecked").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(radioButton)->getSignal("Changed").connect([]{}));
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

        REQUIRE(radioButton->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(radioButton->getSize() == tgui::Vector2f(50, 60));
        REQUIRE(radioButton->getFullSize() == radioButton->getSize());
        REQUIRE(radioButton->getWidgetOffset() == tgui::Vector2f(0, 0));
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
        REQUIRE(!parent1->getCheckedRadioButton());
        REQUIRE(!parent2->getCheckedRadioButton());
        radioButton1->setChecked(true);
        REQUIRE(radioButton1->isChecked());
        REQUIRE(!radioButton2->isChecked());
        REQUIRE(!radioButton3->isChecked());
        REQUIRE(parent1->getCheckedRadioButton() == radioButton1);
        REQUIRE(!parent2->getCheckedRadioButton());
        radioButton2->setChecked(true);
        REQUIRE(!radioButton1->isChecked());
        REQUIRE(radioButton2->isChecked());
        REQUIRE(!radioButton3->isChecked());
        REQUIRE(parent1->getCheckedRadioButton() == radioButton2);
        REQUIRE(!parent2->getCheckedRadioButton());
        radioButton3->setChecked(true);
        REQUIRE(!radioButton1->isChecked());
        REQUIRE(radioButton2->isChecked());
        REQUIRE(radioButton3->isChecked());
        REQUIRE(parent1->getCheckedRadioButton() == radioButton2);
        REQUIRE(parent2->getCheckedRadioButton() == radioButton3);

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
            REQUIRE(!radioButton->isMouseOnWidget({110, 60}));

            radioButton->setTextClickable(true);
            REQUIRE(radioButton->isTextClickable());
            REQUIRE(radioButton->isMouseOnWidget({110, 60}));
        }

        SECTION("Without text")
        {
            radioButton->setTextClickable(false);
            REQUIRE(!radioButton->isTextClickable());
            REQUIRE(!radioButton->isMouseOnWidget({110, 60}));

            radioButton->setTextClickable(true);
            REQUIRE(radioButton->isTextClickable());
            REQUIRE(!radioButton->isMouseOnWidget({110, 60}));
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
            unsigned int changedCount = 0;
            radioButton->onCheck(&genericCallback, std::ref(checkCount));
            radioButton->onUncheck(&genericCallback, std::ref(uncheckCount));
            radioButton->onChange(&genericCallback, std::ref(changedCount));

            radioButton->leftMousePressed({105, 90});
            REQUIRE(checkCount == 0);
            REQUIRE(uncheckCount == 0);
            REQUIRE(changedCount == 0);

            radioButton->leftMouseReleased({105, 90});
            REQUIRE(checkCount == 1);
            REQUIRE(uncheckCount == 0);
            REQUIRE(changedCount == 1);

            // Radio buttons can't be unchecked by user interaction
            radioButton->leftMousePressed({105, 90});
            radioButton->leftMouseReleased({105, 90});
            REQUIRE(checkCount == 1);
            REQUIRE(uncheckCount == 0);
            REQUIRE(changedCount == 1);

            // Programmably unchecking is however possible
            radioButton->setChecked(false);
            REQUIRE(checkCount == 1);
            REQUIRE(uncheckCount == 1);
            REQUIRE(changedCount == 2);

            SECTION("Key pressed")
            {
                tgui::Event::KeyEvent keyEvent;
                keyEvent.alt = false;
                keyEvent.control = false;
                keyEvent.shift = false;
                keyEvent.system = false;

                keyEvent.code = tgui::Event::KeyboardKey::Space;
                radioButton->keyPressed(keyEvent);
                REQUIRE(checkCount == 2);
                REQUIRE(changedCount == 3);

                radioButton->setChecked(false);
                REQUIRE(changedCount == 4);

                keyEvent.code = tgui::Event::KeyboardKey::Enter;
                radioButton->keyPressed(keyEvent);
                REQUIRE(checkCount == 3);
                REQUIRE(changedCount == 5);
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
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorFocused", "rgb(230, 220, 210)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorChecked", "rgb(220, 230, 240)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorCheckedHover", "rgb(230, 240, 250)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorCheckedDisabled", "rgb(250, 240, 230)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorCheckedFocused", "rgb(220, 210, 200)"));
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
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", tgui::Color{50, 60, 70}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", tgui::Color{60, 70, 80}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDisabled", tgui::Color{130, 140, 150}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorChecked", tgui::Color{140, 150, 160}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorCheckedHover", tgui::Color{150, 160, 170}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorCheckedDisabled", tgui::Color{160, 170, 180}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", tgui::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorHover", tgui::Color{80, 90, 100}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorDisabled", tgui::Color{170, 180, 190}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorChecked", tgui::Color{180, 190, 200}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorCheckedHover", tgui::Color{190, 200, 210}));
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColorCheckedDisabled", tgui::Color{200, 210, 220}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", tgui::Color{90, 100, 110}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorHover", tgui::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorDisabled", tgui::Color{210, 220, 230}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorFocused", tgui::Color{230, 220, 210}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorChecked", tgui::Color{220, 230, 240}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorCheckedHover", tgui::Color{230, 240, 250}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorCheckedDisabled", tgui::Color{250, 240, 230}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorCheckedFocused", tgui::Color{220, 210, 200}));
                REQUIRE_NOTHROW(renderer->setProperty("CheckColor", tgui::Color{110, 120, 130}));
                REQUIRE_NOTHROW(renderer->setProperty("CheckColorHover", tgui::Color{120, 130, 140}));
                REQUIRE_NOTHROW(renderer->setProperty("CheckColorDisabled", tgui::Color{240, 230, 220}));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyle", tgui::TextStyle::Italic));
                REQUIRE_NOTHROW(renderer->setProperty("TextStyleChecked", tgui::TextStyle::Bold));
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
                renderer->setBorderColorFocused({230, 220, 210});
                renderer->setBorderColorChecked({220, 230, 240});
                renderer->setBorderColorCheckedHover({230, 240, 250});
                renderer->setBorderColorCheckedDisabled({250, 240, 230});
                renderer->setBorderColorCheckedFocused({220, 210, 200});
                renderer->setCheckColor({110, 120, 130});
                renderer->setCheckColorHover({120, 130, 140});
                renderer->setCheckColorDisabled({240, 230, 220});
                renderer->setTextStyle(tgui::TextStyle::Italic);
                renderer->setTextStyleChecked(tgui::TextStyle::Bold);
                renderer->setTextDistanceRatio(0.5);
                renderer->setBorders({1, 2, 3, 4});
            }

            REQUIRE(renderer->getProperty("TextColor").getColor() == tgui::Color(50, 60, 70));
            REQUIRE(renderer->getProperty("TextColorHover").getColor() == tgui::Color(60, 70, 80));
            REQUIRE(renderer->getProperty("TextColorDisabled").getColor() == tgui::Color(130, 140, 150));
            REQUIRE(renderer->getProperty("TextColorChecked").getColor() == tgui::Color(140, 150, 160));
            REQUIRE(renderer->getProperty("TextColorCheckedHover").getColor() == tgui::Color(150, 160, 170));
            REQUIRE(renderer->getProperty("TextColorCheckedDisabled").getColor() == tgui::Color(160, 170, 180));
            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == tgui::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("BackgroundColorHover").getColor() == tgui::Color(80, 90, 100));
            REQUIRE(renderer->getProperty("BackgroundColorDisabled").getColor() == tgui::Color(170, 180, 190));
            REQUIRE(renderer->getProperty("BackgroundColorChecked").getColor() == tgui::Color(180, 190, 200));
            REQUIRE(renderer->getProperty("BackgroundColorCheckedHover").getColor() == tgui::Color(190, 200, 210));
            REQUIRE(renderer->getProperty("BackgroundColorCheckedDisabled").getColor() == tgui::Color(200, 210, 220));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == tgui::Color(90, 100, 110));
            REQUIRE(renderer->getProperty("BorderColorHover").getColor() == tgui::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("BorderColorDisabled").getColor() == tgui::Color(210, 220, 230));
            REQUIRE(renderer->getProperty("BorderColorFocused").getColor() == tgui::Color(230, 220, 210));
            REQUIRE(renderer->getProperty("BorderColorChecked").getColor() == tgui::Color(220, 230, 240));
            REQUIRE(renderer->getProperty("BorderColorCheckedHover").getColor() == tgui::Color(230, 240, 250));
            REQUIRE(renderer->getProperty("BorderColorCheckedDisabled").getColor() == tgui::Color(250, 240, 230));
            REQUIRE(renderer->getProperty("BorderColorCheckedFocused").getColor() == tgui::Color(220, 210, 200));
            REQUIRE(renderer->getProperty("CheckColor").getColor() == tgui::Color(110, 120, 130));
            REQUIRE(renderer->getProperty("CheckColorHover").getColor() == tgui::Color(120, 130, 140));
            REQUIRE(renderer->getProperty("CheckColorDisabled").getColor() == tgui::Color(240, 230, 220));
            REQUIRE(renderer->getProperty("TextStyle").getTextStyle() == tgui::TextStyle::Italic);
            REQUIRE(renderer->getProperty("TextStyleChecked").getTextStyle() == tgui::TextStyle::Bold);
            REQUIRE(renderer->getProperty("TextDistanceRatio").getNumber() == 0.5f);
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));

            REQUIRE(renderer->getTextColor() == tgui::Color(50, 60, 70));
            REQUIRE(renderer->getTextColorHover() == tgui::Color(60, 70, 80));
            REQUIRE(renderer->getTextColorDisabled() == tgui::Color(130, 140, 150));
            REQUIRE(renderer->getTextColorChecked() == tgui::Color(140, 150, 160));
            REQUIRE(renderer->getTextColorCheckedHover() == tgui::Color(150, 160, 170));
            REQUIRE(renderer->getTextColorCheckedDisabled() == tgui::Color(160, 170, 180));
            REQUIRE(renderer->getBackgroundColor() == tgui::Color(70, 80, 90));
            REQUIRE(renderer->getBackgroundColorHover() == tgui::Color(80, 90, 100));
            REQUIRE(renderer->getBackgroundColorDisabled() == tgui::Color(170, 180, 190));
            REQUIRE(renderer->getBackgroundColorChecked() == tgui::Color(180, 190, 200));
            REQUIRE(renderer->getBackgroundColorCheckedHover() == tgui::Color(190, 200, 210));
            REQUIRE(renderer->getBackgroundColorCheckedDisabled() == tgui::Color(200, 210, 220));
            REQUIRE(renderer->getBorderColor() == tgui::Color(90, 100, 110));
            REQUIRE(renderer->getBorderColorHover() == tgui::Color(100, 110, 120));
            REQUIRE(renderer->getBorderColorDisabled() == tgui::Color(210, 220, 230));
            REQUIRE(renderer->getBorderColorFocused() == tgui::Color(230, 220, 210));
            REQUIRE(renderer->getBorderColorChecked() == tgui::Color(220, 230, 240));
            REQUIRE(renderer->getBorderColorCheckedHover() == tgui::Color(230, 240, 250));
            REQUIRE(renderer->getBorderColorCheckedDisabled() == tgui::Color(250, 240, 230));
            REQUIRE(renderer->getBorderColorCheckedFocused() == tgui::Color(220, 210, 200));
            REQUIRE(renderer->getCheckColor() == tgui::Color(110, 120, 130));
            REQUIRE(renderer->getCheckColorHover() == tgui::Color(120, 130, 140));
            REQUIRE(renderer->getCheckColorDisabled() == tgui::Color(240, 230, 220));
            REQUIRE(renderer->getTextStyle() == tgui::TextStyle::Italic);
            REQUIRE(renderer->getTextStyleChecked() == tgui::TextStyle::Bold);
            REQUIRE(renderer->getTextDistanceRatio() == 0.5f);
            REQUIRE(renderer->getBorders() == tgui::Borders(1, 2, 3, 4));
        }

        SECTION("textured")
        {
            tgui::Texture textureUncheckedNormal("resources/Black.png", {124, 32, 32, 32});
            tgui::Texture textureUncheckedHover("resources/Black.png", {188, 32, 32, 32});
            tgui::Texture textureUncheckedDisabled("resources/Black.png", {124, 32, 32, 32});
            tgui::Texture textureUncheckedFocused("resources/Texture1.png");
            tgui::Texture textureCheckedNormal("resources/Black.png", {156, 32, 32, 32});
            tgui::Texture textureCheckedHover("resources/Black.png", {220, 32, 32, 32});
            tgui::Texture textureCheckedDisabled("resources/Black.png", {156, 32, 32, 32});
            tgui::Texture textureCheckedFocused("resources/Texture2.png");

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureUnchecked", tgui::Serializer::serialize(textureUncheckedNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureUncheckedHover", tgui::Serializer::serialize(textureUncheckedHover)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureUncheckedDisabled", tgui::Serializer::serialize(textureUncheckedDisabled)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureUncheckedFocused", tgui::Serializer::serialize(textureUncheckedFocused)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureChecked", tgui::Serializer::serialize(textureCheckedNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureCheckedHover", tgui::Serializer::serialize(textureCheckedHover)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureCheckedDisabled", tgui::Serializer::serialize(textureCheckedDisabled)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureCheckedFocused", tgui::Serializer::serialize(textureCheckedFocused)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureUnchecked", textureUncheckedNormal));
                REQUIRE_NOTHROW(renderer->setProperty("TextureUncheckedHover", textureUncheckedHover));
                REQUIRE_NOTHROW(renderer->setProperty("TextureUncheckedDisabled", textureUncheckedDisabled));
                REQUIRE_NOTHROW(renderer->setProperty("TextureUncheckedFocused", textureUncheckedFocused));
                REQUIRE_NOTHROW(renderer->setProperty("TextureChecked", textureCheckedNormal));
                REQUIRE_NOTHROW(renderer->setProperty("TextureCheckedHover", textureCheckedHover));
                REQUIRE_NOTHROW(renderer->setProperty("TextureCheckedDisabled", textureCheckedDisabled));
                REQUIRE_NOTHROW(renderer->setProperty("TextureCheckedFocused", textureCheckedFocused));
            }

            SECTION("functions")
            {
                renderer->setTextureUnchecked(textureUncheckedNormal);
                renderer->setTextureUncheckedHover(textureUncheckedHover);
                renderer->setTextureUncheckedDisabled(textureUncheckedDisabled);
                renderer->setTextureUncheckedFocused(textureUncheckedFocused);
                renderer->setTextureChecked(textureCheckedNormal);
                renderer->setTextureCheckedHover(textureCheckedHover);
                renderer->setTextureCheckedDisabled(textureCheckedDisabled);
                renderer->setTextureCheckedFocused(textureCheckedFocused);
            }

            REQUIRE(renderer->getProperty("TextureUnchecked").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureUncheckedHover").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureUncheckedDisabled").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureUncheckedFocused").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureChecked").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureCheckedHover").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureCheckedDisabled").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureCheckedFocused").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTextureUnchecked().getData() == textureUncheckedNormal.getData());
            REQUIRE(renderer->getTextureUncheckedHover().getData() == textureUncheckedHover.getData());
            REQUIRE(renderer->getTextureUncheckedDisabled().getData() == textureUncheckedDisabled.getData());
            REQUIRE(renderer->getTextureUncheckedFocused().getData() == textureUncheckedFocused.getData());
            REQUIRE(renderer->getTextureChecked().getData() == textureCheckedNormal.getData());
            REQUIRE(renderer->getTextureCheckedHover().getData() == textureCheckedHover.getData());
            REQUIRE(renderer->getTextureCheckedDisabled().getData() == textureCheckedDisabled.getData());
            REQUIRE(renderer->getTextureCheckedFocused().getData() == textureCheckedFocused.getData());
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
        renderer.setTextColor(tgui::Color::Red);
        renderer.setTextColorChecked({128, 0, 0});
        renderer.setBackgroundColor(tgui::Color::Green);
        renderer.setBackgroundColorChecked({0, 128, 0});
        renderer.setBorderColor(tgui::Color::Blue);
        renderer.setBorderColorChecked({0, 0, 128});
        renderer.setTextStyle(tgui::TextStyle::Italic);
        renderer.setTextStyleChecked(tgui::TextStyles{tgui::TextStyle::Bold | tgui::TextStyle::StrikeThrough});
        renderer.setOpacity(0.7f);
        radioButton->setRenderer(renderer.getData());

        auto setHoverRenderer = [&](bool textured){
                                        renderer.setTextColorHover(tgui::Color::Magenta);
                                        renderer.setBackgroundColorHover(tgui::Color::Cyan);
                                        renderer.setBorderColorHover(tgui::Color::Yellow);
                                        renderer.setTextStyle(tgui::TextStyle::Bold);
                                        if (textured)
                                            renderer.setTextureUncheckedHover("resources/Texture3.png");
                                     };

        auto setDisabledRenderer = [&](bool textured){
                                        renderer.setTextColorDisabled({128, 128, 0});
                                        renderer.setBackgroundColorDisabled({0, 128, 128});
                                        renderer.setBorderColorDisabled({128, 0, 128});
                                        renderer.setTextStyle(tgui::TextStyle::Italic);
                                        if (textured)
                                            renderer.setTextureUncheckedDisabled("resources/Texture5.png");
                                    };

        auto setCheckedHoverRenderer = [&](bool textured){
                                            renderer.setTextColorCheckedHover({192, 64, 0});
                                            renderer.setBackgroundColorCheckedHover({0, 192, 64});
                                            renderer.setBorderColorCheckedHover({64, 0, 192});
                                            renderer.setTextStyleChecked(tgui::TextStyles{tgui::TextStyle::Bold | tgui::TextStyle::Italic});
                                            if (textured)
                                                renderer.setTextureCheckedHover("resources/Texture4.png");
                                         };

        auto setCheckedDisabledRenderer = [&](bool textured){
                                            renderer.setTextColorCheckedDisabled({64, 192, 0});
                                            renderer.setBackgroundColorCheckedDisabled({0, 64, 192});
                                            renderer.setBorderColorCheckedDisabled({192, 0, 64});
                                            renderer.setTextStyleChecked(tgui::TextStyles{tgui::TextStyle::Italic | tgui::TextStyle::StrikeThrough});
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
