/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Widgets/ColorPicker.hpp>
#include <TGUI/Renderers/LabelRenderer.hpp>

TEST_CASE("[ColorPicker]")
{
    tgui::ColorPicker::Ptr colorPicker = tgui::ColorPicker::create();
    colorPicker->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        colorPicker->onColorChange([](){});
        colorPicker->onColorChange([](tgui::Color){});

        colorPicker->onOkPress([](){});
        colorPicker->onOkPress([](tgui::Color){});
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

        // The color picker contains multiple similar widgets which share renderers with each other.
        // This causes the renderers to be saved outside of the ColorPicker section so that the widgets can refer to it.
        // Unfortunately, the order in which the global renderers are saved is undefined and we compare the files
        // literally to check if the output is the same. This is why we make a small change to the internal renderer
        // of several widgets here, this makes sure only the label renderer is stored outside the FileDialog.
        colorPicker->get("#TGUI_INTERNAL$ColorPickerReset#")->getRenderer()->setOpacity(0.99f);
        colorPicker->get("#TGUI_INTERNAL$ColorPickerOK#")->getRenderer()->setOpacity(0.98f);
        colorPicker->get("#TGUI_INTERNAL$ColorPickerCancel#")->getRenderer()->setOpacity(0.97f);
        colorPicker->get("#TGUI_INTERNAL$ColorPickerRedBox#")->getRenderer()->setOpacity(0.96f);
        colorPicker->get("#TGUI_INTERNAL$ColorPickerGreenBox#")->getRenderer()->setOpacity(0.95f);
        colorPicker->get("#TGUI_INTERNAL$ColorPickerBlueBox#")->getRenderer()->setOpacity(0.94f);
        colorPicker->get("#TGUI_INTERNAL$ColorPickerAlphaBox#")->getRenderer()->setOpacity(0.93f);
        colorPicker->get("#TGUI_INTERNAL$ColorPickerRed#")->getRenderer()->setOpacity(0.92f);
        colorPicker->get("#TGUI_INTERNAL$ColorPickerGreen#")->getRenderer()->setOpacity(0.91f);
        colorPicker->get("#TGUI_INTERNAL$ColorPickerBlue#")->getRenderer()->setOpacity(0.90f);
        colorPicker->get("#TGUI_INTERNAL$ColorPickerAlpha#")->getRenderer()->setOpacity(0.89f);
        colorPicker->get("#TGUI_INTERNAL$ColorPickerValue#")->getRenderer()->setOpacity(0.88f);

        colorPicker->setColor(tgui::Color::Blue);
        testSavingWidget("ColorPicker", colorPicker, false);
    }
}
