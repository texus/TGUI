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
#include <TGUI/Widgets/MessageBox.hpp>

TEST_CASE("[MessageBox]")
{
    tgui::MessageBox::Ptr messageBox = tgui::MessageBox::create();
    messageBox->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(messageBox->connect("ButtonPressed", [](){}));
        REQUIRE_NOTHROW(messageBox->connect("ButtonPressed", [](sf::String){}));
        REQUIRE_NOTHROW(messageBox->connect("ButtonPressed", [](std::string){}));
        REQUIRE_NOTHROW(messageBox->connect("ButtonPressed", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(messageBox->connect("ButtonPressed", [](tgui::Widget::Ptr, std::string, sf::String){}));
        REQUIRE_NOTHROW(messageBox->connect("ButtonPressed", [](tgui::Widget::Ptr, std::string, std::string){}));
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

    testWidgetRenderer(messageBox->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = messageBox->getRenderer();

        tgui::ButtonRenderer buttonsRenderer;
        buttonsRenderer.setBackgroundColor(sf::Color::Cyan);

        SECTION("set serialized property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(10, 20, 30)"));
            REQUIRE_NOTHROW(renderer->setProperty("Button", "{ BackgroundColor = Cyan; }"));
        }

        SECTION("set object property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{10, 20, 30}));
            REQUIRE_NOTHROW(renderer->setProperty("Button", buttonsRenderer.getData()));
        }

        SECTION("functions")
        {
            renderer->setTextColor({10, 20, 30});
            renderer->setButton(buttonsRenderer.getData());
        }

        REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(10, 20, 30));

        REQUIRE(renderer->getButton()->propertyValuePairs.size() == 1);
        REQUIRE(renderer->getButton()->propertyValuePairs["backgroundcolor"].getColor() == sf::Color::Cyan);
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
