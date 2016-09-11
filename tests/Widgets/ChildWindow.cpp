/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include "../Tests.hpp"
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/Panel.hpp>

TEST_CASE("[ChildWindow]")
{
    tgui::ChildWindow::Ptr childWindow = std::make_shared<tgui::ChildWindow>();
    childWindow->getRenderer()->setFont("resources/DroidSansArmenian.ttf");

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(childWindow->connect("MousePressed", [](){}));
        REQUIRE_NOTHROW(childWindow->connect("Closed", [](){}));
        REQUIRE_NOTHROW(childWindow->connect("Maximized", [](){}));
        REQUIRE_NOTHROW(childWindow->connect("Minimized", [](){}));

        REQUIRE_NOTHROW(childWindow->connect("MousePressed", [](sf::Vector2f){}));
        REQUIRE_NOTHROW(childWindow->connect("Closed", [](tgui::ChildWindow::Ptr){}));
        REQUIRE_NOTHROW(childWindow->connect("Maximized", [](tgui::ChildWindow::Ptr){}));
        REQUIRE_NOTHROW(childWindow->connect("Minimized", [](tgui::ChildWindow::Ptr){}));
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
        childWindow->getRenderer()->setBorders({1, 2, 3, 4});

        REQUIRE(childWindow->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(childWindow->getSize() == sf::Vector2f(150, 100));
        REQUIRE(childWindow->getFullSize() == sf::Vector2f(154, 121));
        REQUIRE(childWindow->getWidgetOffset() == sf::Vector2f(0, 0));

        SECTION("Child widgets")
        {
            auto childWidget = std::make_shared<tgui::ClickableWidget>();
            childWidget->setPosition(60, 50);
            childWindow->add(childWidget);

            REQUIRE(childWidget->getPosition() == sf::Vector2f(60, 50));
            REQUIRE(childWidget->getAbsolutePosition() == sf::Vector2f(101, 97));
        }
    }

    /// TODO: Test the functions in the ChildWindow class

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
            REQUIRE(!childWindow->mouseOnWidget({-1, -1}));
            REQUIRE(childWindow->mouseOnWidget({0, 0}));
            REQUIRE(childWindow->mouseOnWidget({75, 50}));
            REQUIRE(childWindow->mouseOnWidget({153, 123}));
            REQUIRE(!childWindow->mouseOnWidget({154, 124}));
        }

        SECTION("mouse move")
        {
            unsigned int mouseEnteredCount = 0;
            unsigned int mouseLeftCount = 0;

            childWindow->connect("MouseEntered", genericCallback, std::ref(mouseEnteredCount));
            childWindow->connect("MouseLeft", genericCallback, std::ref(mouseLeftCount));

            auto parent = std::make_shared<tgui::Panel>(300, 200);
            parent->setPosition({30, 25});
            parent->add(childWindow);

            parent->mouseMoved({10, 15});
            REQUIRE(mouseEnteredCount == 0);
            REQUIRE(mouseLeftCount == 0);

            parent->mouseMoved({40, 30});
            REQUIRE(mouseEnteredCount == 1);
            REQUIRE(mouseLeftCount == 0);

            parent->mouseMoved({193, 153});
            REQUIRE(mouseEnteredCount == 1);
            REQUIRE(mouseLeftCount == 0);

            parent->mouseMoved({194, 154});
            REQUIRE(mouseEnteredCount == 1);
            REQUIRE(mouseLeftCount == 1);
        }

        /// TODO: Test title button events and dragging and resizing window
    }

    testWidgetRenderer(childWindow->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = childWindow->getRenderer();

        childWindow->setTitleButtons(tgui::ChildWindow::TitleButtons::None);

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
                REQUIRE_NOTHROW(renderer->setProperty("DistanceToSide", "2"));
                REQUIRE_NOTHROW(renderer->setProperty("PaddingBetweenButtons", "1"));
                REQUIRE_NOTHROW(renderer->setProperty("TitleBarHeight", "25"));
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
                REQUIRE_NOTHROW(renderer->setProperty("DistanceToSide", 2));
                REQUIRE_NOTHROW(renderer->setProperty("PaddingBetweenButtons", 1));
                REQUIRE_NOTHROW(renderer->setProperty("TitleBarHeight", 25));
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
                renderer->setDistanceToSide(2);
                renderer->setPaddingBetweenButtons(1);
                renderer->setTitleBarHeight(25);

                renderer->setCloseButton(closeButtonRenderer.getData());
                renderer->setMaximizeButton(maximizeButtonRenderer.getData());
                renderer->setMinimizeButton(minimizeButtonRenderer.getData());
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(10, 20, 30));
            REQUIRE(renderer->getProperty("TitleColor").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("TitleBarColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("DistanceToSide").getNumber() == 2);
            REQUIRE(renderer->getProperty("PaddingBetweenButtons").getNumber() == 1);
            REQUIRE(renderer->getProperty("TitleBarHeight").getNumber() == 25);

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

            REQUIRE(renderer->getProperty("TextureTitleBar").getTexture().isLoaded());

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
            }
        }
    }

    SECTION("Saving and loading from file")
    {
        childWindow = std::make_shared<tgui::ChildWindow>();
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
            childWindow->setTitle("Title");
            childWindow->setTitleAlignment(tgui::ChildWindow::TitleAlignment::Left);
            childWindow->setIcon({"resources/image.png"});
            childWindow->keepInParent();

            auto widget = std::make_shared<tgui::ClickableWidget>();
            widget->setPosition(40, 20);
            childWindow->add(widget);

            testSavingWidget("ChildWindow", childWindow);
        }
    }
}
