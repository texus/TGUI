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

TEST_CASE("[Panel]")
{
    tgui::Panel::Ptr panel = tgui::Panel::create();
    panel->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        panel->onMousePress([](){});
        panel->onMousePress([](tgui::Vector2f){});

        panel->onMouseRelease([](){});
        panel->onMouseRelease([](tgui::Vector2f){});

        panel->onClick([](){});
        panel->onClick([](tgui::Vector2f){});

        panel->onDoubleClick([](){});
        panel->onDoubleClick([](tgui::Vector2f){});

        panel->onRightMousePress([](){});
        panel->onRightMousePress([](tgui::Vector2f){});

        panel->onRightMouseRelease([](){});
        panel->onRightMouseRelease([](tgui::Vector2f){});

        panel->onRightClick([](){});
        panel->onRightClick([](tgui::Vector2f){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(panel)->getSignal("MousePressed").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(panel)->getSignal("MouseReleased").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(panel)->getSignal("Clicked").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(panel)->getSignal("DoubleClicked").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(panel)->getSignal("RightMousePressed").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(panel)->getSignal("RightMouseReleased").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(panel)->getSignal("RightClicked").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(panel->getWidgetType() == "Panel");
    }

    SECTION("Position and Size")
    {
        panel->setPosition(40, 30);
        panel->setSize(150, 100);

        SECTION("Without outline")
        {
            REQUIRE(panel->getPosition() == tgui::Vector2f(40, 30));
            REQUIRE(panel->getSize() == tgui::Vector2f(150, 100));
            REQUIRE(panel->getFullSize() == panel->getSize());
            REQUIRE(panel->getWidgetOffset() == tgui::Vector2f(0, 0));
            REQUIRE(panel->getChildWidgetsOffset() == tgui::Vector2f(0, 0));
        }

        SECTION("With outline")
        {
            panel->getRenderer()->setBorders({1, 2, 3, 4});
            panel->getRenderer()->setPadding({5, 6, 7, 8});

            REQUIRE(panel->getPosition() == tgui::Vector2f(40, 30));
            REQUIRE(panel->getSize() == tgui::Vector2f(150, 100));
            REQUIRE(panel->getFullSize() == panel->getSize());
            REQUIRE(panel->getWidgetOffset() == tgui::Vector2f(0, 0));
            REQUIRE(panel->getChildWidgetsOffset() == tgui::Vector2f(6, 8));
        }

        SECTION("Child widgets")
        {
            auto childWidget = tgui::ClickableWidget::create();
            childWidget->setPosition(60, 50);
            panel->add(childWidget);

            REQUIRE(childWidget->getPosition() == tgui::Vector2f(60, 50));
            REQUIRE(childWidget->getAbsolutePosition() == tgui::Vector2f(100, 80));

            panel->getRenderer()->setBorders({1, 2, 3, 4});
            REQUIRE(childWidget->getPosition() == tgui::Vector2f(60, 50));
            REQUIRE(childWidget->getAbsolutePosition() == tgui::Vector2f(101, 82));
        }
    }

    SECTION("Events / Signals")
    {
        SECTION("mouse clicks")
        {
            testClickableWidgetSignals(panel);
        }

        SECTION("mouse move")
        {
            panel->setPosition(40, 30);
            panel->setSize(150, 100);

            unsigned int mouseEnteredCount = 0;
            unsigned int mouseLeftCount = 0;

            panel->onMouseEnter(&genericCallback, std::ref(mouseEnteredCount));
            panel->onMouseLeave(&genericCallback, std::ref(mouseLeftCount));

            auto parent = tgui::Panel::create({300, 200});
            parent->setPosition({30, 25});
            parent->add(panel);

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

    testWidgetRenderer(panel->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = panel->getRenderer();

        SECTION("colored")
        {
            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(10, 20, 30)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", "(5, 6, 7, 8)"));
                REQUIRE_NOTHROW(renderer->setProperty("RoundedBorderRadius", 5));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", tgui::Color{10, 20, 30}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", tgui::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Padding{5, 6, 7, 8}));
                REQUIRE_NOTHROW(renderer->setProperty("RoundedBorderRadius", 5));
            }

            SECTION("functions")
            {
                renderer->setBackgroundColor({10, 20, 30});
                renderer->setBorderColor({40, 50, 60});
                renderer->setBorders({1, 2, 3, 4});
                renderer->setPadding({5, 6, 7, 8});
                renderer->setRoundedBorderRadius(5);
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == tgui::Color(10, 20, 30));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == tgui::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("Padding").getOutline() == tgui::Padding(5, 6, 7, 8));
            REQUIRE(renderer->getProperty("RoundedBorderRadius").getNumber() == 5);

            REQUIRE(renderer->getBackgroundColor() == tgui::Color(10, 20, 30));
            REQUIRE(renderer->getBorderColor() == tgui::Color(40, 50, 60));
            REQUIRE(renderer->getBorders() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getPadding() == tgui::Padding(5, 6, 7, 8));
            REQUIRE(renderer->getRoundedBorderRadius() == 5);
        }

        SECTION("textured")
        {
            tgui::Texture textureBackground("resources/Black.png", {0, 154, 48, 48}, {16, 16, 16, 16});

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", tgui::Serializer::serialize(textureBackground)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", textureBackground));
            }

            SECTION("functions")
            {
                renderer->setTextureBackground(textureBackground);
            }

            REQUIRE(renderer->getProperty("TextureBackground").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTextureBackground().getData() == textureBackground.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        panel = tgui::Panel::create({400, 300});

        auto widget = tgui::ClickableWidget::create();
        widget->setPosition({20, 10});
        widget->setSize({150, 100});
        panel->add(widget);

        SECTION("Only save contents")
        {
            REQUIRE_NOTHROW(panel->saveWidgetsToFile("PanelWidgetFile1.txt"));
            
            panel->setSize(200, 100);
            REQUIRE_NOTHROW(panel->loadWidgetsFromFile("PanelWidgetFile1.txt"));
            REQUIRE(panel->getSize() == tgui::Vector2f(200, 100)); // The Panel itself is not saved, only its children

            REQUIRE_NOTHROW(panel->saveWidgetsToFile("PanelWidgetFile2.txt"));
            REQUIRE(compareFiles("PanelWidgetFile1.txt", "PanelWidgetFile2.txt"));
        }

        SECTION("Save entire panel")
        {
            testSavingWidget("Panel", panel);
        }
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(200, 150, panel)

        tgui::PanelRenderer renderer = tgui::RendererData::create();
        renderer.setBackgroundColor(tgui::Color::Yellow);
        renderer.setBorderColor(tgui::Color::Red);
        renderer.setBorders({1, 2, 3, 4});
        renderer.setPadding({5, 6, 7, 8});
        renderer.setOpacity(0.7f);
        panel->setRenderer(renderer.getData());

        panel->setSize({180, 140});
        panel->setPosition({10, 5});

        auto picture = tgui::Picture::create("resources/image.png");
        picture->setSize({150, 100});
        picture->setPosition({50, 55});
        panel->add(picture);

        TEST_DRAW("Panel.png")
    }
}
