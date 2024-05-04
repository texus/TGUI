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

TEST_CASE("[MessageBox]")
{
    tgui::MessageBox::Ptr messageBox = tgui::MessageBox::create();
    messageBox->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        messageBox->onButtonPress([](){});
        messageBox->onButtonPress([](const tgui::String&){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(messageBox)->getSignal("ButtonPressed").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(messageBox->getWidgetType() == "MessageBox");
    }

    SECTION("Text")
    {
        REQUIRE(messageBox->getText() == "");
        messageBox->setText("Some text");
        REQUIRE(messageBox->getText() == "Some text");
    }

    SECTION("TextSize")
    {
        messageBox->setTextSize(17);
        REQUIRE(messageBox->getTextSize() == 17);
    }

    SECTION("Buttons")
    {
        REQUIRE(messageBox->getButtons().size() == 0);

        messageBox->addButton("First");
        REQUIRE(messageBox->getButtons().size() == 1);
        REQUIRE(messageBox->getButtons()[0] == "First");

        messageBox->addButton("Second");
        REQUIRE(messageBox->getButtons().size() == 2);
        REQUIRE(messageBox->getButtons()[0] == "First");
        REQUIRE(messageBox->getButtons()[1] == "Second");
    }

    SECTION("ChangeButtons")
    {
        REQUIRE(messageBox->getButtons().size() == 0);

        // Empty captions on empty MessageBox does nothing.
        messageBox->changeButtons({});
        REQUIRE(messageBox->getButtons().size() == 0);

        // Adding a single caption from empty.
        messageBox->changeButtons({ "First" });
        REQUIRE(messageBox->getButtons().size() == 1);
        REQUIRE(messageBox->getButtons()[0] == "First");

        // Empty captions on MessageBox removes single buttons.
        messageBox->changeButtons({});
        REQUIRE(messageBox->getButtons().size() == 0);

        // Adding a multiple captions from empty.
        messageBox->changeButtons({ "First", "Second", "Third" });
        REQUIRE(messageBox->getButtons().size() == 3);
        REQUIRE(messageBox->getButtons()[0] == "First");
        REQUIRE(messageBox->getButtons()[1] == "Second");
        REQUIRE(messageBox->getButtons()[2] == "Third");

        // Empty captions on MessageBox also removes mutliple buttons.
        messageBox->changeButtons({});
        REQUIRE(messageBox->getButtons().size() == 0);

        // Changing with a single caption will rename existing button.
        messageBox->addButton("First");
        messageBox->changeButtons({ "Renamed" });
        REQUIRE(messageBox->getButtons().size() == 1);
        REQUIRE(messageBox->getButtons()[0] == "Renamed");

        // Changing with a excess captions will rename existing buttons and add new ones.
        messageBox->addButton("Second");
        messageBox->changeButtons({ "First", "Renamed", "Third", "Fourth" });
        REQUIRE(messageBox->getButtons().size() == 4);
        REQUIRE(messageBox->getButtons()[0] == "First");
        REQUIRE(messageBox->getButtons()[1] == "Renamed");
        REQUIRE(messageBox->getButtons()[2] == "Third");
        REQUIRE(messageBox->getButtons()[3] == "Fourth");

        // Changing with lower number of captions will rename existing buttons and remove excess ones.
        messageBox->changeButtons({ "1", "2" });
        REQUIRE(messageBox->getButtons().size() == 2);
        REQUIRE(messageBox->getButtons()[0] == "1");
        REQUIRE(messageBox->getButtons()[1] == "2");

        // Applying the same button vector will not have any perceived effect.
        messageBox->changeButtons(messageBox->getButtons());
        REQUIRE(messageBox->getButtons().size() == 2);
        REQUIRE(messageBox->getButtons()[0] == "1");
        REQUIRE(messageBox->getButtons()[1] == "2");

        auto currentButtons = messageBox->getButtons();
        currentButtons.resize(10);
        messageBox->changeButtons(currentButtons);
        REQUIRE(messageBox->getButtons().size() == 10);
    }

    SECTION("LabelAlignment")
    {
        REQUIRE(messageBox->getLabelAlignment() == tgui::HorizontalAlignment::Left);
        messageBox->setLabelAlignment(tgui::HorizontalAlignment::Right);
        REQUIRE(messageBox->getLabelAlignment() == tgui::HorizontalAlignment::Right);
        messageBox->setLabelAlignment(tgui::HorizontalAlignment::Center);
        REQUIRE(messageBox->getLabelAlignment() == tgui::HorizontalAlignment::Center);
        messageBox->setLabelAlignment(tgui::HorizontalAlignment::Left);
        REQUIRE(messageBox->getLabelAlignment() == tgui::HorizontalAlignment::Left);
    }

    SECTION("ButtonAlignment")
    {
        REQUIRE(messageBox->getButtonAlignment() == tgui::HorizontalAlignment::Center);
        messageBox->setButtonAlignment(tgui::HorizontalAlignment::Left);
        REQUIRE(messageBox->getButtonAlignment() == tgui::HorizontalAlignment::Left);
        messageBox->setButtonAlignment(tgui::HorizontalAlignment::Right);
        REQUIRE(messageBox->getButtonAlignment() == tgui::HorizontalAlignment::Right);
        messageBox->setButtonAlignment(tgui::HorizontalAlignment::Center);
        REQUIRE(messageBox->getButtonAlignment() == tgui::HorizontalAlignment::Center);
    }

    testWidgetRenderer(messageBox->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = messageBox->getRenderer();

        tgui::ButtonRenderer buttonsRenderer;
        buttonsRenderer.setBackgroundColor(tgui::Color::Cyan);

        SECTION("set serialized property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(10, 20, 30)"));
            REQUIRE_NOTHROW(renderer->setProperty("Button", "{ BackgroundColor = Cyan; }"));
        }

        SECTION("set object property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("TextColor", tgui::Color{10, 20, 30}));
            REQUIRE_NOTHROW(renderer->setProperty("Button", buttonsRenderer.getData()));
        }

        SECTION("functions")
        {
            renderer->setTextColor({10, 20, 30});
            renderer->setButton(buttonsRenderer.getData());
        }

        REQUIRE(renderer->getProperty("TextColor").getColor() == tgui::Color(10, 20, 30));

        REQUIRE(renderer->getButton()->propertyValuePairs.size() == 1);
        REQUIRE(renderer->getButton()->propertyValuePairs["BackgroundColor"].getColor() == tgui::Color::Cyan);
    }

    SECTION("Saving and loading from file")
    {
        messageBox->setTitle("Error occured");
        messageBox->setText("Oh no! Something went wrong.");
        messageBox->addButton("Reboot");
        messageBox->addButton("Retry");
        messageBox->addButton("Ignore");

        testSavingWidget("MessageBox", messageBox);
    }
}
