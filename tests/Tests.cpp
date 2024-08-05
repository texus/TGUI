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

#include <TGUI/Config.hpp>
#ifdef TGUI_SYSTEM_WINDOWS
    #include <TGUI/extlibs/IncludeWindows.hpp>
#endif

#include "Tests.hpp"

tgui::String getClipboardContents()
{
#ifdef TGUI_SYSTEM_WINDOWS
    // Wait a moment before accessing the clipboard.
    // The data wasn't always there yet when requesting it immediately after changing it when using the SDL backend on Windows.
    // The problem might be with setting instead of getting though: setting two times quickly resulted in access denied error.
    // There were still occational failures when only waiting 1ms.
    Sleep(5);
#endif

    return tgui::getBackend()->getClipboard();
}

void mouseCallback(unsigned int& count, tgui::Vector2f pos)
{
    count++;
    REQUIRE(pos == tgui::Vector2f(75, 50));
}

void genericCallback(unsigned int& count)
{
    count++;
}

void testWidgetSignals(const tgui::Widget::Ptr& widget)
{
    // TODO: Test other signals than MouseEntered and MouseLeft

    SECTION("mouse move")
    {
        unsigned int mouseEnteredCount = 0;
        unsigned int mouseLeftCount = 0;

        widget->onMouseEnter([&]{ genericCallback(mouseEnteredCount); });
        widget->onMouseLeave([&]{ genericCallback(mouseLeftCount); });

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

        parent->remove(widget);
    }
}

template <typename T>
void testClickableWidgetSignalsImpl(T widget)
{
    testWidgetSignals(widget);

    unsigned int mousePressedCount = 0;
    unsigned int mouseReleasedCount = 0;
    unsigned int clickedCount = 0;
    unsigned int rightMousePressedCount = 0;
    unsigned int rightMouseReleasedCount = 0;
    unsigned int rightClickedCount = 0;

    widget->setPosition({40, 30});
    widget->setSize({150, 100});

    widget->onMousePress([&](tgui::Vector2f pos){ mouseCallback(mousePressedCount, pos); });
    widget->onMouseRelease([&](tgui::Vector2f pos){ mouseCallback(mouseReleasedCount, pos); });
    widget->onClick([&](tgui::Vector2f pos){ mouseCallback(clickedCount, pos); });
    widget->onRightMousePress([&](tgui::Vector2f pos){ mouseCallback(rightMousePressedCount, pos); });
    widget->onRightMouseRelease([&](tgui::Vector2f pos){ mouseCallback(rightMouseReleasedCount, pos); });
    widget->onRightClick([&](tgui::Vector2f pos){ mouseCallback(rightClickedCount, pos); });

    SECTION("isMouseOnWidget")
    {
        REQUIRE(!widget->isMouseOnWidget({39, 29}));
        REQUIRE(widget->isMouseOnWidget({40, 30}));
        REQUIRE(widget->isMouseOnWidget({115, 80}));
        REQUIRE(widget->isMouseOnWidget({189, 129}));
        REQUIRE(!widget->isMouseOnWidget({190, 130}));

        REQUIRE(mousePressedCount == 0);
        REQUIRE(mouseReleasedCount == 0);
        REQUIRE(clickedCount == 0);
        REQUIRE(rightMousePressedCount == 0);
        REQUIRE(rightMouseReleasedCount == 0);
        REQUIRE(rightClickedCount == 0);
    }

    auto parent = tgui::Panel::create({300, 200});
    parent->setPosition({60, 55});
    parent->add(widget);

    SECTION("left mouse click")
    {
        parent->leftMouseReleased({175, 135});

        REQUIRE(mousePressedCount == 0);
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
        parent->leftMouseButtonNoLongerDown();

        REQUIRE(mousePressedCount == 1);
        REQUIRE(mouseReleasedCount == 2);
        REQUIRE(clickedCount == 1);
        REQUIRE(rightMousePressedCount == 0);
        REQUIRE(rightMouseReleasedCount == 0);
        REQUIRE(rightClickedCount == 0);
    }

    SECTION("right mouse click")
    {
        parent->rightMouseReleased({175, 135});

        REQUIRE(rightMousePressedCount == 0);
        REQUIRE(rightMouseReleasedCount == 1);
        REQUIRE(rightClickedCount == 0);

        SECTION("mouse press")
        {
            parent->rightMousePressed({175, 135});

            REQUIRE(rightMousePressedCount == 1);
            REQUIRE(rightMouseReleasedCount == 1);
            REQUIRE(rightClickedCount == 0);
        }

        parent->rightMouseReleased({175, 135});
        parent->rightMouseButtonNoLongerDown();

        REQUIRE(rightMousePressedCount == 1);
        REQUIRE(rightMouseReleasedCount == 2);
        REQUIRE(rightClickedCount == 1);
        REQUIRE(mousePressedCount == 0);
        REQUIRE(mouseReleasedCount == 0);
        REQUIRE(clickedCount == 0);
    }
}

void testClickableWidgetSignals(const tgui::ClickableWidget::Ptr& widget)
{
    testClickableWidgetSignalsImpl(widget);
}

void testClickableWidgetSignals(const tgui::Panel::Ptr& widget)
{
    testClickableWidgetSignalsImpl(widget);
}

void testScrollbarAccess(tgui::ScrollbarAccessor *scrollbar)
{
    scrollbar->setValue(10);
    REQUIRE(scrollbar->getValue() == 10);

    scrollbar->setScrollAmount(25);
    REQUIRE(scrollbar->getScrollAmount() == 25);

    scrollbar->setPolicy(tgui::Scrollbar::Policy::Always);
    REQUIRE(scrollbar->getPolicy() == tgui::Scrollbar::Policy::Always);

    REQUIRE(scrollbar->getMaximum() > scrollbar->getViewportSize());
    REQUIRE(scrollbar->getMaxValue() == scrollbar->getMaximum() - scrollbar->getViewportSize());

    REQUIRE(scrollbar->isShown());
    REQUIRE(scrollbar->getWidth() > 0);
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
