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

TEST_CASE("[RichTextLabel]")
{
    tgui::RichTextLabel::Ptr label = tgui::RichTextLabel::create();
    label->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        label->onDoubleClick([](){});
        label->onDoubleClick([](const tgui::String&){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(label)->getSignal("DoubleClicked").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(label->getWidgetType() == "RichTextLabel");
    }

    SECTION("Position and Size")
    {
        label->setPosition(40, 30);
        label->setSize(150, 100);
        label->getRenderer()->setBorders(2);

        REQUIRE(label->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(label->getSize() == tgui::Vector2f(150, 100));
        REQUIRE(label->getFullSize() == label->getSize());
        REQUIRE(label->getWidgetOffset() == tgui::Vector2f(0, 0));
    }

    SECTION("Text")
    {
        REQUIRE(label->getText() == "");
        label->setText("SomeText");
        REQUIRE(label->getText() == "SomeText");
    }

    SECTION("TextSize")
    {
        label->setTextSize(25);
        REQUIRE(label->getTextSize() == 25);
    }

    SECTION("Alignment")
    {
        REQUIRE(label->getHorizontalAlignment() == tgui::HorizontalAlignment::Left);
        REQUIRE(label->getVerticalAlignment() == tgui::VerticalAlignment::Top);

        label->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
        REQUIRE(label->getHorizontalAlignment() == tgui::HorizontalAlignment::Center);
        REQUIRE(label->getVerticalAlignment() == tgui::VerticalAlignment::Top);

        label->setHorizontalAlignment(tgui::HorizontalAlignment::Right);
        REQUIRE(label->getHorizontalAlignment() == tgui::HorizontalAlignment::Right);

        label->setHorizontalAlignment(tgui::HorizontalAlignment::Left);
        REQUIRE(label->getHorizontalAlignment() == tgui::HorizontalAlignment::Left);

        label->setVerticalAlignment(tgui::VerticalAlignment::Center);
        REQUIRE(label->getHorizontalAlignment() == tgui::HorizontalAlignment::Left);
        REQUIRE(label->getVerticalAlignment() == tgui::VerticalAlignment::Center);

        label->setVerticalAlignment(tgui::VerticalAlignment::Bottom);
        REQUIRE(label->getVerticalAlignment() == tgui::VerticalAlignment::Bottom);

        label->setVerticalAlignment(tgui::VerticalAlignment::Top);
        REQUIRE(label->getVerticalAlignment() == tgui::VerticalAlignment::Top);
    }

    SECTION("AutoSize")
    {
        REQUIRE(label->getAutoSize());
        label->setAutoSize(false);
        REQUIRE(!label->getAutoSize());
        label->setAutoSize(true);
        REQUIRE(label->getAutoSize());
        label->setSize(200, 100);
        REQUIRE(!label->getAutoSize());
    }

    SECTION("MaximumTextWidth")
    {
        REQUIRE(label->getMaximumTextWidth() == 0);
        label->setMaximumTextWidth(300);
        REQUIRE(label->getMaximumTextWidth() == 300);

        label->setSize(200, 50);
        REQUIRE(label->getMaximumTextWidth() == 200);

        label->setSize(400, 50);
        REQUIRE(label->getMaximumTextWidth() == 400);

        label->setMaximumTextWidth(500);
        REQUIRE(label->getMaximumTextWidth() == 400);

        label->setAutoSize(true);
        REQUIRE(label->getMaximumTextWidth() == 500);
    }

    SECTION("ScrollbarPolicy")
    {
        REQUIRE(label->getScrollbarPolicy() == tgui::Scrollbar::Policy::Automatic);
        label->setScrollbarPolicy(tgui::Scrollbar::Policy::Always);
        REQUIRE(label->getScrollbarPolicy() == tgui::Scrollbar::Policy::Always);
        label->setScrollbarPolicy(tgui::Scrollbar::Policy::Never);
        REQUIRE(label->getScrollbarPolicy() == tgui::Scrollbar::Policy::Never);
        label->setScrollbarPolicy(tgui::Scrollbar::Policy::Automatic);
        REQUIRE(label->getScrollbarPolicy() == tgui::Scrollbar::Policy::Automatic);
    }

    SECTION("IgnoreMouseEvents")
    {
        REQUIRE(!label->isIgnoringMouseEvents());
        label->ignoreMouseEvents(true);
        REQUIRE(label->isIgnoringMouseEvents());
        label->ignoreMouseEvents(false);
        REQUIRE(!label->isIgnoringMouseEvents());
    }

    SECTION("Events / Signals")
    {
        SECTION("ClickableWidget")
        {
            testClickableWidgetSignals(label);
        }

        SECTION("Double click")
        {
            unsigned int doubleClickedCount = 0;
            label->onDoubleClick(&genericCallback, std::ref(doubleClickedCount));

            label->setPosition(40, 30);
            label->setSize(150, 100);

            label->leftMousePressed({115, 80});
            label->leftMouseReleased({115, 80});

            GuiNull gui;
            gui.add(label);
            gui.updateTime(DOUBLE_CLICK_TIMEOUT);

            label->leftMousePressed({115, 80});
            label->leftMouseReleased({115, 80});
            REQUIRE(doubleClickedCount == 0);

            gui.updateTime(DOUBLE_CLICK_TIMEOUT / 2.f);

            label->leftMousePressed({115, 80});
            label->leftMouseReleased({115, 80});
            REQUIRE(doubleClickedCount == 1);
        }

        SECTION("Mouse events pass through label with IgnoreMouseEvents=true")
        {
            unsigned int mousePressedCountBack = 0;
            unsigned int mousePressedCountFront = 0;

            label->setPosition({40, 30});
            label->setSize({150, 100});

            auto backgroundLabel = tgui::RichTextLabel::copy(label);

            auto parent = tgui::Panel::create({300, 200});
            parent->setPosition({30, 25});
            parent->add(backgroundLabel);
            parent->add(label);

            backgroundLabel->onMousePress(&genericCallback, std::ref(mousePressedCountBack));
            label->onMousePress(&genericCallback, std::ref(mousePressedCountFront));

            parent->leftMousePressed({175, 135});
            parent->leftMouseReleased({175, 135});
            parent->leftMouseButtonNoLongerDown();

            REQUIRE(mousePressedCountBack == 0);
            REQUIRE(mousePressedCountFront == 1);

            label->ignoreMouseEvents(true);

            parent->leftMousePressed({175, 135});
            parent->leftMouseReleased({175, 135});
            parent->leftMouseButtonNoLongerDown();

            REQUIRE(mousePressedCountBack == 1);
            REQUIRE(mousePressedCountFront == 1);
        }
    }

    testWidgetRenderer(label->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = label->getRenderer();

        tgui::Texture textureBackground("resources/Black.png", {0, 154, 48, 48}, {16, 16, 16, 16});

        tgui::ScrollbarRenderer scrollbarRenderer;
        scrollbarRenderer.setTrackColor(tgui::Color::Red);
        scrollbarRenderer.setThumbColor(tgui::Color::Blue);

        SECTION("set serialized property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(100, 50, 150)"));
            REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(150, 100, 50)"));
            REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(50, 150, 100)"));
            REQUIRE_NOTHROW(renderer->setProperty("TextOutlineColor", "rgb(10, 20, 30)"));
            REQUIRE_NOTHROW(renderer->setProperty("TextOutlineThickness", "2"));
            REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
            REQUIRE_NOTHROW(renderer->setProperty("Padding", "(5, 6, 7, 8)"));
            REQUIRE_NOTHROW(renderer->setProperty("TextStyle", "Bold | Italic"));
            REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", tgui::Serializer::serialize(textureBackground)));
            REQUIRE_NOTHROW(renderer->setProperty("Scrollbar", "{ TrackColor = Red; ThumbColor = Blue; }"));
            REQUIRE_NOTHROW(renderer->setProperty("ScrollbarWidth", "15"));
        }

        SECTION("set object property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("TextColor", tgui::Color{100, 50, 150}));
            REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", tgui::Color{150, 100, 50}));
            REQUIRE_NOTHROW(renderer->setProperty("BorderColor", tgui::Color{50, 150, 100}));
            REQUIRE_NOTHROW(renderer->setProperty("TextOutlineColor", tgui::Color{10, 20, 30}));
            REQUIRE_NOTHROW(renderer->setProperty("TextOutlineThickness", 2));
            REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
            REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Borders{5, 6, 7, 8}));
            REQUIRE_NOTHROW(renderer->setProperty("TextStyle", tgui::TextStyles{tgui::TextStyle::Bold | tgui::TextStyle::Italic}));
            REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", textureBackground));
            REQUIRE_NOTHROW(renderer->setProperty("Scrollbar", scrollbarRenderer.getData()));
            REQUIRE_NOTHROW(renderer->setProperty("ScrollbarWidth", 15));
        }

        SECTION("functions")
        {
            renderer->setTextColor({100, 50, 150});
            renderer->setBackgroundColor({150, 100, 50});
            renderer->setBorderColor({50, 150, 100});
            renderer->setTextOutlineColor({10, 20, 30});
            renderer->setTextOutlineThickness(2);
            renderer->setBorders({1, 2, 3, 4});
            renderer->setPadding({5, 6, 7, 8});
            renderer->setTextStyle(tgui::TextStyle::Bold | tgui::TextStyle::Italic);
            renderer->setTextureBackground(textureBackground);
            renderer->setScrollbar(scrollbarRenderer.getData());
            renderer->setScrollbarWidth(15);
        }

        REQUIRE(renderer->getProperty("TextColor").getColor() == tgui::Color(100, 50, 150));
        REQUIRE(renderer->getProperty("BackgroundColor").getColor() == tgui::Color(150, 100, 50));
        REQUIRE(renderer->getProperty("BorderColor").getColor() == tgui::Color(50, 150, 100));
        REQUIRE(renderer->getProperty("TextOutlineColor").getColor() == tgui::Color(10, 20, 30));
        REQUIRE(renderer->getProperty("TextOutlineThickness").getNumber() == 2);
        REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
        REQUIRE(renderer->getProperty("Padding").getOutline() == tgui::Padding(5, 6, 7, 8));
        REQUIRE(renderer->getProperty("TextStyle").getTextStyle() == (tgui::TextStyle::Bold | tgui::TextStyle::Italic));
        REQUIRE(renderer->getProperty("TextureBackground").getTexture().getData() != nullptr);
        REQUIRE(renderer->getProperty("ScrollbarWidth").getNumber() == 15);

        REQUIRE(renderer->getTextColor() == tgui::Color(100, 50, 150));
        REQUIRE(renderer->getBackgroundColor() == tgui::Color(150, 100, 50));
        REQUIRE(renderer->getBorderColor() == tgui::Color(50, 150, 100));
        REQUIRE(renderer->getTextOutlineColor() == tgui::Color(10, 20, 30));
        REQUIRE(renderer->getTextOutlineThickness() == 2);
        REQUIRE(renderer->getBorders() == tgui::Borders(1, 2, 3, 4));
        REQUIRE(renderer->getPadding() == tgui::Padding(5, 6, 7, 8));
        REQUIRE(renderer->getTextStyle() == (tgui::TextStyle::Bold | tgui::TextStyle::Italic));
        REQUIRE(renderer->getTextureBackground().getData() == textureBackground.getData());

        REQUIRE(renderer->getScrollbar()->propertyValuePairs.size() == 2);
        REQUIRE(renderer->getScrollbar()->propertyValuePairs["TrackColor"].getColor() == tgui::Color::Red);
        REQUIRE(renderer->getScrollbar()->propertyValuePairs["ThumbColor"].getColor() == tgui::Color::Blue);
    }

    SECTION("Saving and loading from file")
    {
        label->setText("SomeText");
        label->setTextSize(25);
        label->setHorizontalAlignment(tgui::HorizontalAlignment::Center);
        label->setVerticalAlignment(tgui::VerticalAlignment::Bottom);
        label->setScrollbarPolicy(tgui::Scrollbar::Policy::Never);
        label->setMaximumTextWidth(300);
        label->ignoreMouseEvents(true);

        testSavingWidget("Label", label);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(425, 195, label)

        label->setPosition(10, 5);
        label->setSize(405, 185);
        label->setTextSize(13);
        label->getRenderer()->setTextColor("#303030");
        label->getRenderer()->setBackgroundColor("#D0D0D0");
        label->getRenderer()->setBorderColor(tgui::Color::Yellow);
        label->getRenderer()->setBorders({1, 2, 3, 4});
        label->getRenderer()->setPadding({4, 3, 2, 1});
        label->getRenderer()->setOpacity(0.7f);

        label->setText(
            U"The RichTextLabel widget supports formatting text with <b>bold</b>, <i>italics</i>, <u>underlined</u> and even <s>strikethrough</s>. "
            U"Each letter can have a separate <size=15>size</size> or <color=blue>color</color>. This allows for some <b><color=#ff0000>C</color><color=#ffbf00>O</color>"
            U"<color=#80ff00>L</color><color=#00ff40>O</color><color=#00ffff>R</color><color=#0040ff>F</color><color=#7f00ff>U</color><color=#ff00bf>L</color></b> text. "
            U"You can even include icons such as <img=resources/TreeViewCollapsed.png> and images:\n<img=\"resources/image.png\">\n\nLines that are too long will wrap around and a vertical scrollbar "
            U"can be included when there are too many lines!\n\n\n\n\n\n\n\n");

        TEST_DRAW("RichTextLabel.png")

        SECTION("Outline")
        {
            // Outline of one piece should not be rendered on top of another piece, and there should be no additional gap between pieces
            label->getRenderer()->setTextOutlineColor(tgui::Color::Green);
            label->getRenderer()->setTextOutlineThickness(4);
            label->setText("<color=blue>Hello</color><color=red>World</color>");
            TEST_DRAW("RichTextLabel_Outline.png")
        }
    }
}
