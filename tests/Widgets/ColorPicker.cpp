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

TEST_CASE("[ColorPicker]")
{
    tgui::ColorPicker::Ptr colorPicker = tgui::ColorPicker::create("Select your color");
    colorPicker->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        colorPicker->onColorChange([](){});
        colorPicker->onColorChange([](tgui::Color){});

        colorPicker->onOkPress([](){});
        colorPicker->onOkPress([](tgui::Color){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(colorPicker)->getSignal("ColorChanged").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(colorPicker)->getSignal("OkPressed").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(colorPicker->getWidgetType() == "ColorPicker");
    }

    SECTION("Color")
    {
        colorPicker->setColor(tgui::Color::Red);
        REQUIRE(colorPicker->getColor() == tgui::Color::Red);
    }

    testWidgetRenderer(colorPicker->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = colorPicker->getRenderer();

        tgui::ButtonRenderer buttonRenderer;
        buttonRenderer.setBackgroundColor(tgui::Color::Cyan);

        tgui::LabelRenderer labelRenderer;
        labelRenderer.setTextColor(tgui::Color::Green);

        tgui::SliderRenderer sliderRenderer;
        sliderRenderer.setTrackColor(tgui::Color::Yellow);

        SECTION("set serialized property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("Button", "{ BackgroundColor = Cyan; }"));
            REQUIRE_NOTHROW(renderer->setProperty("Label", "{ TextColor = Green; }"));
            REQUIRE_NOTHROW(renderer->setProperty("Slider", "{ TrackColor = Yellow; }"));
        }

        SECTION("set object property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("Button", buttonRenderer.getData()));
            REQUIRE_NOTHROW(renderer->setProperty("Label", labelRenderer.getData()));
            REQUIRE_NOTHROW(renderer->setProperty("Slider", sliderRenderer.getData()));
        }

        SECTION("functions")
        {
            renderer->setButton(buttonRenderer.getData());
            renderer->setLabel(labelRenderer.getData());
            renderer->setSlider(sliderRenderer.getData());
        }

        REQUIRE(renderer->getButton()->propertyValuePairs["BackgroundColor"].getColor() == tgui::Color::Cyan);
        REQUIRE(renderer->getLabel()->propertyValuePairs["TextColor"].getColor() == tgui::Color::Green);
        REQUIRE(renderer->getSlider()->propertyValuePairs["TrackColor"].getColor() == tgui::Color::Yellow);
    }

    SECTION("Saving and loading from file")
    {
        // The close button renderer isn't parsed when the button is invisible, leading to a slightly different output
        // when saving and loading the file (as the entire renderer is seen as a string and thus not indented).
        // Since we compare saved files byte for byte, we have to make the close button visible to keep them identical.
        colorPicker->setTitleButtons(tgui::ChildWindow::TitleButton::Close);

        colorPicker->setColor(tgui::Color::Blue);
        testSavingWidget("ColorPicker", colorPicker, false);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(560, 270, colorPicker)

        colorPicker->setEnabled(true);
        colorPicker->setPosition(10, 5);
        colorPicker->setColor(tgui::Color::Green);

        tgui::ColorPickerRenderer renderer = tgui::RendererData::create();
        renderer.setTitleBarColor(tgui::Color::Blue);
        renderer.setTitleColor(tgui::Color::Magenta);
        renderer.setBackgroundColor(tgui::Color::White);
        renderer.setBorderColor(tgui::Color::Red);
        renderer.setBorderColorFocused(tgui::Color::Yellow);
        renderer.setDistanceToSide(10);
        renderer.setPaddingBetweenButtons(5);
        renderer.setShowTextOnTitleButtons(false);
        renderer.setTitleBarHeight(30);
        renderer.setBorderBelowTitleBar(1);
        renderer.setBorders({2, 3, 4, 5});
        renderer.setOpacity(0.7f);
        colorPicker->setRenderer(renderer.getData());

        tgui::ButtonRenderer buttonRenderer = tgui::RendererData::create();
        buttonRenderer.setBackgroundColor(tgui::Color::Cyan);
        buttonRenderer.setBorderColor(tgui::Color::Magenta);
        buttonRenderer.setBorders({2});
        renderer.setButton(buttonRenderer.getData());

        tgui::EditBoxRenderer editBoxRenderer = tgui::RendererData::create();
        editBoxRenderer.setBackgroundColor(tgui::Color::Cyan);
        editBoxRenderer.setBorderColor(tgui::Color::Magenta);
        editBoxRenderer.setTextColor(tgui::Color::Blue);
        editBoxRenderer.setBorders({1});
        renderer.setEditBox(editBoxRenderer.getData());

        tgui::LabelRenderer labelRenderer = tgui::RendererData::create();
        labelRenderer.setTextColor(tgui::Color::Blue);
        renderer.setLabel(labelRenderer.getData());

        tgui::SliderRenderer sliderRenderer = tgui::RendererData::create();
        sliderRenderer.setTrackColor(tgui::Color::Cyan);
        sliderRenderer.setThumbColor(tgui::Color::Yellow);
        sliderRenderer.setBorderColor(tgui::Color::Magenta);
        sliderRenderer.setBorders({1});
        renderer.setSlider(sliderRenderer.getData());

        TEST_DRAW("ColorPicker_BeforeColorChange.png")

        // Change the selected color to (0, 130, 200) by interacting with the sliders
        colorPicker->mouseMoved({383, 83});
        colorPicker->leftMousePressed({383, 83});
        colorPicker->leftMouseReleased({383, 83});
        colorPicker->mouseMoved({438, 110});
        colorPicker->leftMousePressed({438, 110});
        colorPicker->leftMouseReleased({438, 110});
        colorPicker->mouseNoLongerOnWidget();
        colorPicker->setFocused(true);

        TEST_DRAW("ColorPicker_AfterColorChange.png")
    }
}
