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
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <TGUI/Widgets/Panel.hpp>

TEST_CASE("[ChildWindow]")
{
    tgui::ChildWindow::Ptr childWindow = tgui::ChildWindow::create();
    childWindow->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(childWindow->connect("MousePressed", [](){}));
        REQUIRE_NOTHROW(childWindow->connect("MousePressed", [](tgui::Widget::Ptr, std::string){}));

        REQUIRE_NOTHROW(childWindow->connect("Closed", [](){}));
        REQUIRE_NOTHROW(childWindow->connect("Closed", [](tgui::ChildWindow::Ptr){}));
        REQUIRE_NOTHROW(childWindow->connect("Closed", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(childWindow->connect("Closed", [](tgui::Widget::Ptr, std::string, tgui::ChildWindow::Ptr){}));

        REQUIRE_NOTHROW(childWindow->connect("Maximized", [](){}));
        REQUIRE_NOTHROW(childWindow->connect("Maximized", [](tgui::ChildWindow::Ptr){}));
        REQUIRE_NOTHROW(childWindow->connect("Maximized", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(childWindow->connect("Maximized", [](tgui::Widget::Ptr, std::string, tgui::ChildWindow::Ptr){}));

        REQUIRE_NOTHROW(childWindow->connect("Minimized", [](){}));
        REQUIRE_NOTHROW(childWindow->connect("Minimized", [](tgui::ChildWindow::Ptr){}));
        REQUIRE_NOTHROW(childWindow->connect("Minimized", [](tgui::Widget::Ptr, std::string){}));
        REQUIRE_NOTHROW(childWindow->connect("Minimized", [](tgui::Widget::Ptr, std::string, tgui::ChildWindow::Ptr){}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(childWindow->getWidgetType() == "ChildWindow");
    }

    SECTION("Position and Size")
    {
        childWindow->setPosition(40, 30);
        childWindow->setSize(150, 100);
        childWindow->getRenderer()->setTitleBarHeight(15);
        childWindow->getRenderer()->setBorderBelowTitleBar(1);
        childWindow->getRenderer()->setBorders({1, 2, 3, 4});

        REQUIRE(childWindow->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(childWindow->getSize() == sf::Vector2f(150, 100));
        REQUIRE(childWindow->getFullSize() == sf::Vector2f(154, 122));
        REQUIRE(childWindow->getWidgetOffset() == sf::Vector2f(0, 0));

        SECTION("Child widgets")
        {
            REQUIRE(childWindow->getChildWidgetsOffset() == sf::Vector2f(1, 18));

            auto childWidget = tgui::ClickableWidget::create();
            childWidget->setPosition(60, 50);
            childWindow->add(childWidget);

            REQUIRE(childWidget->getPosition() == sf::Vector2f(60, 50));
            REQUIRE(childWidget->getAbsolutePosition() == sf::Vector2f(101, 98));
        }
    }

    SECTION("MinimumSize")
    {
        childWindow->setSize(25, 25);

        REQUIRE(childWindow->getMinimumSize() == sf::Vector2f(0, 0));
        childWindow->setMinimumSize({50, 100});
        REQUIRE(childWindow->getMinimumSize() == sf::Vector2f(50, 100));

        // Window is resized if needed
        REQUIRE(childWindow->getSize() == sf::Vector2f(50, 100));

        // Size limits are for user interaction and setSize ignores this property
        childWindow->setSize({40, 30});
        REQUIRE(childWindow->getSize() == sf::Vector2f(40, 30));
    }

    SECTION("MaximumSize")
    {
        childWindow->setSize(250, 250);

        REQUIRE(childWindow->getMaximumSize() == sf::Vector2f(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()));
        childWindow->setMaximumSize({100, 50});
        REQUIRE(childWindow->getMaximumSize() == sf::Vector2f(100, 50));

        // Window is resized if needed
        REQUIRE(childWindow->getSize() == sf::Vector2f(100, 50));

        // Size limits are for user interaction and setSize ignores this property
        childWindow->setSize({400, 300});
        REQUIRE(childWindow->getSize() == sf::Vector2f(400, 300));
    }

    SECTION("Title")
    {
        REQUIRE(childWindow->getTitle() == "");
        childWindow->setTitle("Title Text");
        REQUIRE(childWindow->getTitle() == "Title Text");
    }

    SECTION("TitleAlignment")
    {
        REQUIRE(childWindow->getTitleAlignment() == tgui::ChildWindow::TitleAlignment::Center);

        childWindow->setTitleAlignment(tgui::ChildWindow::TitleAlignment::Left);
        REQUIRE(childWindow->getTitleAlignment() == tgui::ChildWindow::TitleAlignment::Left);

        childWindow->setTitleAlignment(tgui::ChildWindow::TitleAlignment::Right);
        REQUIRE(childWindow->getTitleAlignment() == tgui::ChildWindow::TitleAlignment::Right);

        childWindow->setTitleAlignment(tgui::ChildWindow::TitleAlignment::Center);
        REQUIRE(childWindow->getTitleAlignment() == tgui::ChildWindow::TitleAlignment::Center);
    }

    SECTION("TitleButtons")
    {
        REQUIRE(childWindow->getTitleButtons() == tgui::ChildWindow::TitleButton::Close);

        childWindow->setTitleButtons(tgui::ChildWindow::TitleButton::None);
        REQUIRE(childWindow->getTitleButtons() == tgui::ChildWindow::TitleButton::None);

        int buttons = tgui::ChildWindow::TitleButton::Close | tgui::ChildWindow::TitleButton::Maximize | tgui::ChildWindow::TitleButton::Minimize;
        childWindow->setTitleButtons(buttons);
        REQUIRE(childWindow->getTitleButtons() == buttons);
    }

    SECTION("Resizable")
    {
        REQUIRE(childWindow->isResizable() == false);

        childWindow->setResizable(true);
        REQUIRE(childWindow->isResizable() == true);

        childWindow->setResizable(false);
        REQUIRE(childWindow->isResizable() == false);
    }

    SECTION("KeepInParent")
    {
        REQUIRE(childWindow->isKeptInParent() == false);

        childWindow->setKeepInParent(true);
        REQUIRE(childWindow->isKeptInParent() == true);

        childWindow->setKeepInParent(false);
        REQUIRE(childWindow->isKeptInParent() == false);
    }

    SECTION("Events / Signals")
    {
        unsigned int mousePressedCount = 0;
        unsigned int mouseReleasedCount = 0;
        unsigned int clickedCount = 0;

        childWindow->setPosition(40, 30);
        childWindow->setSize(150, 100);
        childWindow->getRenderer()->setTitleBarHeight(20);
        childWindow->getRenderer()->setBorders({2});

        SECTION("mouseOnWidget")
        {
            REQUIRE(!childWindow->mouseOnWidget({39, 29}));
            REQUIRE(childWindow->mouseOnWidget({40, 30}));
            REQUIRE(childWindow->mouseOnWidget({115, 80}));
            REQUIRE(childWindow->mouseOnWidget({193, 153}));
            REQUIRE(!childWindow->mouseOnWidget({194, 154}));
        }

        SECTION("mouse move")
        {
            unsigned int mouseEnteredCount = 0;
            unsigned int mouseLeftCount = 0;

            childWindow->connect("MouseEntered", &genericCallback, std::ref(mouseEnteredCount));
            childWindow->connect("MouseLeft", &genericCallback, std::ref(mouseLeftCount));

            auto parent = tgui::Panel::create({300, 200});
            parent->setPosition({30, 25});
            parent->add(childWindow);

            parent->mouseMoved({40, 40});
            REQUIRE(mouseEnteredCount == 0);
            REQUIRE(mouseLeftCount == 0);

            parent->mouseMoved({70, 55});
            REQUIRE(mouseEnteredCount == 1);
            REQUIRE(mouseLeftCount == 0);

            parent->mouseMoved({223, 178});
            REQUIRE(mouseEnteredCount == 1);
            REQUIRE(mouseLeftCount == 0);

            parent->mouseMoved({224, 179});
            REQUIRE(mouseEnteredCount == 1);
            REQUIRE(mouseLeftCount == 1);
        }

        /// TODO: Test title button events and dragging and resizing window
    }

    testWidgetRenderer(childWindow->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = childWindow->getRenderer();

        childWindow->setTitleButtons(tgui::ChildWindow::TitleButton::None);

        SECTION("colored")
        {
            tgui::ButtonRenderer closeButtonRenderer;
            tgui::ButtonRenderer maximizeButtonRenderer;
            tgui::ButtonRenderer minimizeButtonRenderer;
            closeButtonRenderer.setBackgroundColor(sf::Color::Red);
            maximizeButtonRenderer.setBackgroundColor(sf::Color::Green);
            minimizeButtonRenderer.setBackgroundColor(sf::Color::Blue);

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(10, 20, 30)"));
                REQUIRE_NOTHROW(renderer->setProperty("TitleColor", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("TitleBarColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(100, 110, 120)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderBelowTitleBar", "3"));
                REQUIRE_NOTHROW(renderer->setProperty("DistanceToSide", "2"));
                REQUIRE_NOTHROW(renderer->setProperty("PaddingBetweenButtons", "1"));
                REQUIRE_NOTHROW(renderer->setProperty("TitleBarHeight", "25"));
                REQUIRE_NOTHROW(renderer->setProperty("MinimumResizableBorderWidth", "4"));
                REQUIRE_NOTHROW(renderer->setProperty("CloseButton", "{ BackgroundColor = Red; }"));
                REQUIRE_NOTHROW(renderer->setProperty("MaximizeButton", "{ BackgroundColor = Green; }"));
                REQUIRE_NOTHROW(renderer->setProperty("MinimizeButton", "{ BackgroundColor = Blue; }"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{10, 20, 30}));
                REQUIRE_NOTHROW(renderer->setProperty("TitleColor", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("TitleBarColor", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderBelowTitleBar", 3));
                REQUIRE_NOTHROW(renderer->setProperty("DistanceToSide", 2));
                REQUIRE_NOTHROW(renderer->setProperty("PaddingBetweenButtons", 1));
                REQUIRE_NOTHROW(renderer->setProperty("TitleBarHeight", 25));
                REQUIRE_NOTHROW(renderer->setProperty("MinimumResizableBorderWidth", 4));
                REQUIRE_NOTHROW(renderer->setProperty("CloseButton", closeButtonRenderer.getData()));
                REQUIRE_NOTHROW(renderer->setProperty("MaximizeButton", maximizeButtonRenderer.getData()));
                REQUIRE_NOTHROW(renderer->setProperty("MinimizeButton", minimizeButtonRenderer.getData()));
            }

            SECTION("functions")
            {
                renderer->setBackgroundColor({10, 20, 30});
                renderer->setTitleColor({40, 50, 60});
                renderer->setTitleBarColor({70, 80, 90});
                renderer->setBorderColor({100, 110, 120});
                renderer->setBorders({1, 2, 3, 4});
                renderer->setBorderBelowTitleBar(3);
                renderer->setDistanceToSide(2);
                renderer->setPaddingBetweenButtons(1);
                renderer->setTitleBarHeight(25);
                renderer->setMinimumResizableBorderWidth(4);

                renderer->setCloseButton(closeButtonRenderer.getData());
                renderer->setMaximizeButton(maximizeButtonRenderer.getData());
                renderer->setMinimizeButton(minimizeButtonRenderer.getData());
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(10, 20, 30));
            REQUIRE(renderer->getProperty("TitleColor").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("TitleBarColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("BorderBelowTitleBar").getNumber() == 3);
            REQUIRE(renderer->getProperty("DistanceToSide").getNumber() == 2);
            REQUIRE(renderer->getProperty("PaddingBetweenButtons").getNumber() == 1);
            REQUIRE(renderer->getProperty("TitleBarHeight").getNumber() == 25);
            REQUIRE(renderer->getProperty("MinimumResizableBorderWidth").getNumber() == 4);

            REQUIRE(renderer->getCloseButton()->propertyValuePairs["backgroundcolor"].getColor() == sf::Color::Red);
            REQUIRE(renderer->getMaximizeButton()->propertyValuePairs["backgroundcolor"].getColor() == sf::Color::Green);
            REQUIRE(renderer->getMinimizeButton()->propertyValuePairs["backgroundcolor"].getColor() == sf::Color::Blue);
        }

        SECTION("textured")
        {
            tgui::Texture textureTitleBar("resources/Black.png", {48, 154, 75, 25}, {16, 16, 16, 16});

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureTitleBar", tgui::Serializer::serialize(textureTitleBar)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureTitleBar", textureTitleBar));
            }

            SECTION("functions")
            {
                renderer->setTextureTitleBar(textureTitleBar);
            }

            REQUIRE(renderer->getProperty("TextureTitleBar").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTextureTitleBar().getData() == textureTitleBar.getData());
        }

        SECTION("TitleBarHeight value")
        {
            SECTION("TitleBarHeight property set")
            {
                renderer->setTitleBarHeight(15);
                renderer->setTextureTitleBar({"resources/TitleBar.png"});
                REQUIRE(renderer->getTitleBarHeight() == 15);
            }

            SECTION("Fallback to texture height")
            {
                renderer->setTextureTitleBar({"resources/TitleBar.png"});
                REQUIRE(renderer->getTitleBarHeight() == 25);
            }

            SECTION("Fallback to default value")
            {
                REQUIRE(renderer->getTitleBarHeight() == 20);

                renderer->setTextureTitleBar({"resources/TitleBar.png"});
                renderer->setTextureTitleBar({});
                REQUIRE(renderer->getTitleBarHeight() == 20);
            }
        }
    }

    SECTION("Saving and loading from file")
    {
        childWindow = tgui::ChildWindow::create();
        childWindow->setSize(400, 300);

        SECTION("Only save contents")
        {
            REQUIRE_NOTHROW(childWindow->saveWidgetsToFile("ChildWindowWidgetFile1.txt"));

            childWindow->setSize(200, 100);
            REQUIRE_NOTHROW(childWindow->loadWidgetsFromFile("ChildWindowWidgetFile1.txt"));
            REQUIRE(childWindow->getSize() == sf::Vector2f(200, 100)); // The child window itself is not saved, only its children

            REQUIRE_NOTHROW(childWindow->saveWidgetsToFile("ChildWindowWidgetFile2.txt"));
            REQUIRE(compareFiles("ChildWindowWidgetFile1.txt", "ChildWindowWidgetFile2.txt"));
        }

        SECTION("Save entire child window")
        {
            childWindow->setMinimumSize({40, 30});
            childWindow->setMaximumSize({200, 150});
            childWindow->setTitle("Title");
            childWindow->setTitleAlignment(tgui::ChildWindow::TitleAlignment::Left);
            childWindow->setTitleButtons(tgui::ChildWindow::TitleButton::Close | tgui::ChildWindow::TitleButton::Minimize);
            childWindow->setResizable();
            childWindow->setKeepInParent(true);

            auto widget = tgui::ClickableWidget::create();
            widget->setPosition(40, 20);
            childWindow->add(widget);

            testSavingWidget("ChildWindow", childWindow);
        }
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(200, 185, childWindow)

        childWindow->setSize({180, 140});
        childWindow->setPosition({10, 5});
        childWindow->setTitle("Window");
        childWindow->setTitleTextSize(18);
        childWindow->setTitleButtons(tgui::ChildWindow::TitleButton::Maximize | tgui::ChildWindow::TitleButton::Minimize);
        childWindow->setTitleAlignment(tgui::ChildWindow::TitleAlignment::Center);

        tgui::ChildWindowRenderer renderer = tgui::RendererData::create();
        renderer.setTitleBarColor(sf::Color::Blue);
        renderer.setTitleColor(sf::Color::Magenta);
        renderer.setBackgroundColor(sf::Color::Green);
        renderer.setBorderColor(sf::Color::Red);
        renderer.setDistanceToSide(10);
        renderer.setPaddingBetweenButtons(5);
        renderer.setShowTextOnTitleButtons(false);
        renderer.setTitleBarHeight(30);
        renderer.setBorderBelowTitleBar(1);
        renderer.setBorders({2, 3, 4, 5});
        renderer.setOpacity(0.7f);
        childWindow->setRenderer(renderer.getData());

        tgui::ButtonRenderer buttonRenderer = tgui::RendererData::create();
        buttonRenderer.setBackgroundColor(sf::Color::Red);
        buttonRenderer.setBorderColor(sf::Color::Green);
        buttonRenderer.setBorders({2});
        renderer.setCloseButton(buttonRenderer.getData());
        renderer.setMaximizeButton(buttonRenderer.getData());
        renderer.setMinimizeButton(buttonRenderer.getData());

        auto picture = tgui::Picture::create("resources/image.png");
        picture->setSize({150, 100});
        picture->setPosition({50, 55});
        childWindow->add(picture);

        SECTION("Colored")
        {
            TEST_DRAW("ChildWindow.png")

            childWindow->setTitleAlignment(tgui::ChildWindow::TitleAlignment::Left);

            renderer.setShowTextOnTitleButtons(true);
            TEST_DRAW("ChildWindow_TitleButtonText.png")
        }

        SECTION("Textured")
        {
            childWindow->setTitleAlignment(tgui::ChildWindow::TitleAlignment::Right);

            renderer.setTextureTitleBar("resources/Texture1.png");
            TEST_DRAW("ChildWindow_Textured.png")
        }
    }
}
