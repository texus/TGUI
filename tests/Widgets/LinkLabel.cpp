/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2026 Bruno Van de Velde (vdv_b@tgui.eu)
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

TEST_CASE("[LinkLabel]")
{
    tgui::LinkLabel::Ptr label = tgui::LinkLabel::create();
    label->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("WidgetType")
    {
        REQUIRE(label->getWidgetType() == "LinkLabel");
    }

    SECTION("Create")
    {
        REQUIRE(label->getText().empty());
        REQUIRE(label->getUrl().empty());

        label = tgui::LinkLabel::create("Text", "https://tgui.eu");
        REQUIRE(label->getText() == "Text");
        REQUIRE(label->getUrl() == "https://tgui.eu");
    }

    SECTION("Url")
    {
        label->setUrl("https://tgui.eu");
        REQUIRE(label->getUrl() == "https://tgui.eu");
        label->setUrl("");
        REQUIRE(label->getUrl().empty());
    }

    SECTION("MouseCursor")
    {
        REQUIRE(label->getMouseCursor() == tgui::Cursor::Type::Hand);
    }

    // Note that the url is left empty in these tests, because otherwise clicking the label would open a web browser
    SECTION("Events / Signals")
    {
        SECTION("ClickableWidget")
        {
            testClickableWidgetSignals(label);
        }
    }

    testWidgetRenderer(label->getRenderer());
    SECTION("Renderer")
    {
        auto* renderer = label->getRenderer();

        SECTION("set serialized property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(100, 50, 150)"));
            REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", "rgb(150, 100, 50)"));
        }

        SECTION("set object property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("TextColor", tgui::Color{100, 50, 150}));
            REQUIRE_NOTHROW(renderer->setProperty("TextColorHover", tgui::Color{150, 100, 50}));
        }

        SECTION("functions")
        {
            renderer->setTextColor({100, 50, 150});
            renderer->setTextColorHover({150, 100, 50});
        }

        REQUIRE(renderer->getProperty("TextColor").getColor() == tgui::Color(100, 50, 150));
        REQUIRE(renderer->getProperty("TextColorHover").getColor() == tgui::Color(150, 100, 50));

        REQUIRE(renderer->getTextColor() == tgui::Color(100, 50, 150));
        REQUIRE(renderer->getTextColorHover() == tgui::Color(150, 100, 50));
    }

    SECTION("Saving and loading from file")
    {
        label->setText("SomeText");
        label->setUrl("https://tgui.eu");
        label->setTextSize(25);

        testSavingWidget("LinkLabel", label);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(75, 40, label)

        label->setPosition(10, 5);
        label->setText("Link");
        label->setTextSize(24);

        // The default theme sets a hover color, we start without it to test that TextColor is used instead
        auto* renderer = label->getRenderer();
        renderer->setTextColor(tgui::Color::Blue);
        renderer->setTextColorHover({});
        renderer->setTextStyle(tgui::TextStyle::Underlined);

        const tgui::Vector2f mousePos = {30, 20};

        SECTION("Normal")
        {
            TEST_DRAW("LinkLabel_Normal.png")

            // The hover color is only used when the mouse is on top of the label
            renderer->setTextColorHover(tgui::Color::Red);
            TEST_DRAW("LinkLabel_Normal.png")
        }

        SECTION("Hover")
        {
            label->mouseMoved(mousePos);
            TEST_DRAW("LinkLabel_Normal.png") // TextColor is used as long as TextColorHover isn't set

            renderer->setTextColorHover(tgui::Color::Red);
            TEST_DRAW("LinkLabel_Hover.png")

            label->mouseNoLongerOnWidget();
            TEST_DRAW("LinkLabel_Normal.png")
        }
    }
}
