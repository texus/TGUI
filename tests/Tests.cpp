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
#include <TGUI/Widgets/Panel.hpp>

void mouseCallback(unsigned int& count, sf::Vector2f pos)
{
    count++;
    REQUIRE(pos == sf::Vector2f(75, 50));
}

void genericCallback(unsigned int& count)
{
    count++;
}

void testWidgetSignals(tgui::Widget::Ptr widget)
{
    // TODO: Test other signals than MouseEntered and MouseLeft

    SECTION("mouse move")
    {
        unsigned int mouseEnteredCount = 0;
        unsigned int mouseLeftCount = 0;

        widget->connect("MouseEntered", [&]{ genericCallback(mouseEnteredCount); });
        widget->connect("MouseLeft", [&]{ genericCallback(mouseLeftCount); });

        auto parent = tgui::Panel::create({300, 200});
        parent->setPosition({30, 25});
        parent->add(widget);

        widget->setPosition({40, 30});
        widget->setSize({150, 100});

        parent->mouseMoved({40, 40});
        REQUIRE(mouseEnteredCount == 0);
        REQUIRE(mouseLeftCount == 0);

        parent->mouseMoved({70, 55});
        REQUIRE(mouseEnteredCount == 1);
        REQUIRE(mouseLeftCount == 0);

        parent->mouseMoved({219, 154});
        REQUIRE(mouseEnteredCount == 1);
        REQUIRE(mouseLeftCount == 0);

        parent->mouseMoved({220, 155});
        REQUIRE(mouseEnteredCount == 1);
        REQUIRE(mouseLeftCount == 1);
    }
}

void testClickableWidgetSignals(tgui::ClickableWidget::Ptr widget)
{
    testWidgetSignals(widget);

    unsigned int mousePressedCount = 0;
    unsigned int mouseReleasedCount = 0;
    unsigned int clickedCount = 0;

    widget->setPosition({40, 30});
    widget->setSize({150, 100});

    widget->connect("MousePressed", [&](sf::Vector2f pos){ mouseCallback(mousePressedCount, pos); });
    widget->connect("MouseReleased", [&](sf::Vector2f pos){ mouseCallback(mouseReleasedCount, pos); });
    widget->connect("Clicked", [&](sf::Vector2f pos){ mouseCallback(clickedCount, pos); });

    SECTION("mouseOnWidget")
    {
        REQUIRE(!widget->mouseOnWidget({39, 29}));
        REQUIRE(widget->mouseOnWidget({40, 30}));
        REQUIRE(widget->mouseOnWidget({115, 80}));
        REQUIRE(widget->mouseOnWidget({189, 129}));
        REQUIRE(!widget->mouseOnWidget({190, 130}));

        REQUIRE(mousePressedCount == 0);
        REQUIRE(mouseReleasedCount == 0);
        REQUIRE(clickedCount == 0);
    }

    SECTION("mouse click")
    {
        auto parent = tgui::Panel::create({300, 200});
        parent->setPosition({60, 55});
        parent->add(widget);

        parent->leftMouseReleased({175, 135});

        REQUIRE(mouseReleasedCount == 1);
        REQUIRE(clickedCount == 0);

        SECTION("mouse press")
        {
            parent->leftMousePressed({175, 135});

            REQUIRE(mousePressedCount == 1);
            REQUIRE(mouseReleasedCount == 1);
            REQUIRE(clickedCount == 0);
        }

        parent->leftMouseReleased({175, 135});
        parent->mouseNoLongerDown();

        REQUIRE(mousePressedCount == 1);
        REQUIRE(mouseReleasedCount == 2);
        REQUIRE(clickedCount == 1);
    }
}

void testWidgetRenderer(tgui::WidgetRenderer* renderer)
{
    SECTION("WidgetRenderer")
    {
        SECTION("set serialized property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("Opacity", "0.8"));
            REQUIRE_NOTHROW(renderer->setProperty("Font", "resources/DejaVuSans.ttf"));
        }

        SECTION("set object property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("Opacity", 0.8f));
            REQUIRE_NOTHROW(renderer->setProperty("Font", tgui::Font{"resources/DejaVuSans.ttf"}));
        }

        SECTION("functions")
        {
            renderer->setOpacity(0.8f);
            renderer->setFont({"resources/DejaVuSans.ttf"});
        }

        REQUIRE(renderer->getProperty("Opacity").getNumber() == 0.8f);
        REQUIRE(renderer->getProperty("Font").getFont() != nullptr);

        REQUIRE(renderer->getOpacity() == 0.8f);
        REQUIRE(renderer->getFont().getId() == "resources/DejaVuSans.ttf");

        REQUIRE_THROWS_AS(renderer->setProperty("NonexistentProperty", ""), tgui::Exception);
    }
}
