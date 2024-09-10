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

TEST_CASE("[ChildWindow]")
{
    tgui::ChildWindow::Ptr childWindow = tgui::ChildWindow::create();
    childWindow->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        childWindow->onMousePress([](){});

        childWindow->onClose([](){});
        childWindow->onClose([](const tgui::ChildWindow::Ptr&){});

        childWindow->onClosing([](){});
        childWindow->onClosing([](bool*){});

        childWindow->onMaximize([](){});
        childWindow->onMaximize([](const tgui::ChildWindow::Ptr&){});

        childWindow->onMinimize([](){});
        childWindow->onMinimize([](const tgui::ChildWindow::Ptr&){});

        childWindow->onEscapeKeyPress([](){});
        childWindow->onEscapeKeyPress([](const tgui::ChildWindow::Ptr&){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(childWindow)->getSignal("MousePressed").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(childWindow)->getSignal("Closed").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(childWindow)->getSignal("Closing").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(childWindow)->getSignal("Maximized").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(childWindow)->getSignal("Minimized").connect([]{}));
        REQUIRE_NOTHROW(tgui::Widget::Ptr(childWindow)->getSignal("EscapeKeyPressed").connect([]{}));
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

        REQUIRE(childWindow->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(childWindow->getSize() == tgui::Vector2f(150, 100));
        REQUIRE(childWindow->getFullSize() == tgui::Vector2f(150, 100));
        REQUIRE(childWindow->getWidgetOffset() == tgui::Vector2f(0, 0));

        SECTION("Child widgets")
        {
            REQUIRE(childWindow->getChildWidgetsOffset() == tgui::Vector2f(1, 18));

            auto childWidget = tgui::ClickableWidget::create();
            childWidget->setPosition(60, 50);
            childWindow->add(childWidget);

            REQUIRE(childWidget->getPosition() == tgui::Vector2f(60, 50));
            REQUIRE(childWidget->getAbsolutePosition() == tgui::Vector2f(101, 98));
        }
    }

    SECTION("MinimumSize")
    {
        childWindow->setSize(25, 25);

        REQUIRE(childWindow->getMinimumSize() == tgui::Vector2f(0, 0));
        childWindow->setMinimumSize({50, 100});
        REQUIRE(childWindow->getMinimumSize() == tgui::Vector2f(50, 100));

        // Window is resized if needed
        REQUIRE(childWindow->getSize() == tgui::Vector2f(50, 100));

        // Size limits are for user interaction and setSize ignores this property
        childWindow->setSize({40, 30});
        REQUIRE(childWindow->getSize() == tgui::Vector2f(40, 30));
    }

    SECTION("MaximumSize")
    {
        childWindow->setSize(250, 250);

        REQUIRE(childWindow->getMaximumSize() == tgui::Vector2f(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()));
        childWindow->setMaximumSize({100, 50});
        REQUIRE(childWindow->getMaximumSize() == tgui::Vector2f(100, 50));

        // Window is resized if needed
        REQUIRE(childWindow->getSize() == tgui::Vector2f(100, 50));

        // Size limits are for user interaction and setSize ignores this property
        childWindow->setSize({400, 300});
        REQUIRE(childWindow->getSize() == tgui::Vector2f(400, 300));
    }

    SECTION("ClientSize")
    {
        childWindow->getRenderer()->setTitleBarHeight(20);
        childWindow->getRenderer()->setBorders({1, 2, 4, 8});
        childWindow->getRenderer()->setBorderBelowTitleBar(2);

        SECTION("setClientSize")
        {
            childWindow->setClientSize({300, 250});
            REQUIRE(childWindow->getClientSize() == tgui::Vector2f(300, 250));
            REQUIRE(childWindow->getSize() == tgui::Vector2f(305, 282));

            // Client size remains fixed when setClientSize was used
            childWindow->getRenderer()->setTitleBarHeight(15);
            childWindow->getRenderer()->setBorders({8, 4, 2, 1});
            childWindow->getRenderer()->setBorderBelowTitleBar(1);
            REQUIRE(childWindow->getClientSize() == tgui::Vector2f(300, 250));
            REQUIRE(childWindow->getSize() == tgui::Vector2f(310, 271));
        }

        SECTION("setSize")
        {
            childWindow->setSize({300, 250});
            REQUIRE(childWindow->getClientSize() == tgui::Vector2f(295, 218));
            REQUIRE(childWindow->getSize() == tgui::Vector2f(300, 250));

            // Client size changes
            childWindow->getRenderer()->setTitleBarHeight(15);
            childWindow->getRenderer()->setBorders({8, 4, 2, 1});
            childWindow->getRenderer()->setBorderBelowTitleBar(1);
            REQUIRE(childWindow->getClientSize() == tgui::Vector2f(290, 229));
            REQUIRE(childWindow->getSize() == tgui::Vector2f(300, 250));
        }
    }

    SECTION("Title")
    {
        REQUIRE(childWindow->getTitle() == "");
        childWindow->setTitle("Title Text");
        REQUIRE(childWindow->getTitle() == "Title Text");
    }

    SECTION("TitleAlignment")
    {
        REQUIRE(childWindow->getTitleAlignment() == tgui::HorizontalAlignment::Center);

        childWindow->setTitleAlignment(tgui::HorizontalAlignment::Left);
        REQUIRE(childWindow->getTitleAlignment() == tgui::HorizontalAlignment::Left);

        childWindow->setTitleAlignment(tgui::HorizontalAlignment::Right);
        REQUIRE(childWindow->getTitleAlignment() == tgui::HorizontalAlignment::Right);

        childWindow->setTitleAlignment(tgui::HorizontalAlignment::Center);
        REQUIRE(childWindow->getTitleAlignment() == tgui::HorizontalAlignment::Center);
    }

    SECTION("TitleButtons")
    {
        REQUIRE(childWindow->getTitleButtons() == tgui::ChildWindow::TitleButton::Close);

        childWindow->setTitleButtons(tgui::ChildWindow::TitleButton::None);
        REQUIRE(childWindow->getTitleButtons() == tgui::ChildWindow::TitleButton::None);

        unsigned int buttons = tgui::ChildWindow::TitleButton::Close | tgui::ChildWindow::TitleButton::Maximize | tgui::ChildWindow::TitleButton::Minimize;
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

    SECTION("PositionLocked")
    {
        REQUIRE(childWindow->isPositionLocked() == false);

        childWindow->setPositionLocked(true);
        REQUIRE(childWindow->isPositionLocked() == true);

        childWindow->setPositionLocked(false);
        REQUIRE(childWindow->isPositionLocked() == false);
    }

    SECTION("KeepInParent")
    {
        REQUIRE(childWindow->getKeepInParent() == false);

        childWindow->setKeepInParent(true);
        REQUIRE(childWindow->getKeepInParent() == true);

        childWindow->setKeepInParent(false);
        REQUIRE(childWindow->getKeepInParent() == false);
    }

    SECTION("CloseBehavior")
    {
        auto parent = tgui::Panel::create({300, 200});
        parent->add(childWindow);

        unsigned int onCloseCount = 0;
        unsigned int onClosingCount = 0;
        childWindow->onClose(&genericCallback, std::ref(onCloseCount));
        childWindow->onClosing(&genericCallback, std::ref(onClosingCount));

        REQUIRE(parent->getWidgets().size() == 1);
        REQUIRE(childWindow->isVisible());

        childWindow->setCloseBehavior(tgui::ChildWindow::CloseBehavior::Remove);
        REQUIRE(childWindow->getCloseBehavior() == tgui::ChildWindow::CloseBehavior::Remove);
        childWindow->close();
        REQUIRE(parent->getWidgets().size() == 0);
        REQUIRE(childWindow->isVisible());
        parent->add(childWindow);

        childWindow->setCloseBehavior(tgui::ChildWindow::CloseBehavior::Hide);
        REQUIRE(childWindow->getCloseBehavior() == tgui::ChildWindow::CloseBehavior::Hide);
        childWindow->close();
        REQUIRE(parent->getWidgets().size() == 1);
        REQUIRE(!childWindow->isVisible());
        childWindow->setVisible(true);

        childWindow->setCloseBehavior(tgui::ChildWindow::CloseBehavior::None);
        REQUIRE(childWindow->getCloseBehavior() == tgui::ChildWindow::CloseBehavior::None);
        childWindow->close();
        REQUIRE(parent->getWidgets().size() == 1);
        REQUIRE(childWindow->isVisible());

        REQUIRE(onClosingCount == 3);
        REQUIRE(onCloseCount == 3);
    }

    SECTION("Events / Signals")
    {
        childWindow->setPosition(40, 30);
        childWindow->setSize(150, 100);
        childWindow->getRenderer()->setTitleBarHeight(20);
        childWindow->getRenderer()->setBorders({2});

        SECTION("isMouseOnWidget")
        {
            REQUIRE(!childWindow->isMouseOnWidget({39, 29}));
            REQUIRE(childWindow->isMouseOnWidget({40, 30}));
            REQUIRE(childWindow->isMouseOnWidget({115, 80}));
            REQUIRE(childWindow->isMouseOnWidget({189, 129}));
            REQUIRE(!childWindow->isMouseOnWidget({190, 130}));
        }

        SECTION("mouse move")
        {
            unsigned int mouseEnteredCount = 0;
            unsigned int mouseLeftCount = 0;

            childWindow->onMouseEnter(&genericCallback, std::ref(mouseEnteredCount));
            childWindow->onMouseLeave(&genericCallback, std::ref(mouseLeftCount));

            auto parent = tgui::Panel::create({300, 200});
            parent->setPosition({30, 25});
            parent->add(childWindow);

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

        SECTION("EscapeKeyPressed")
        {
            unsigned int escapeKeyPressedCount = 0;
            childWindow->onEscapeKeyPress(&genericCallback, std::ref(escapeKeyPressedCount));

            tgui::Event::KeyEvent event;
            event.code = tgui::Event::KeyboardKey::Escape;
            event.alt = false;
            event.control = false;
            event.shift = false;
            event.system = false;
            childWindow->keyPressed(event);
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
            closeButtonRenderer.setBackgroundColor(tgui::Color::Red);
            maximizeButtonRenderer.setBackgroundColor(tgui::Color::Green);
            minimizeButtonRenderer.setBackgroundColor(tgui::Color::Blue);

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(10, 20, 30)"));
                REQUIRE_NOTHROW(renderer->setProperty("TitleColor", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("TitleBarColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(100, 110, 120)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorFocused", "rgb(130, 140, 150)"));
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
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", tgui::Color{10, 20, 30}));
                REQUIRE_NOTHROW(renderer->setProperty("TitleColor", tgui::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("TitleBarColor", tgui::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", tgui::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorFocused", tgui::Color{130, 140, 150}));
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
                renderer->setBorderColorFocused({130, 140, 150});
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

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == tgui::Color(10, 20, 30));
            REQUIRE(renderer->getProperty("TitleColor").getColor() == tgui::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("TitleBarColor").getColor() == tgui::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == tgui::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("BorderColorFocused").getColor() == tgui::Color(130, 140, 150));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("BorderBelowTitleBar").getNumber() == 3);
            REQUIRE(renderer->getProperty("DistanceToSide").getNumber() == 2);
            REQUIRE(renderer->getProperty("PaddingBetweenButtons").getNumber() == 1);
            REQUIRE(renderer->getProperty("TitleBarHeight").getNumber() == 25);
            REQUIRE(renderer->getProperty("MinimumResizableBorderWidth").getNumber() == 4);

            REQUIRE(renderer->getCloseButton()->propertyValuePairs["BackgroundColor"].getColor() == tgui::Color::Red);
            REQUIRE(renderer->getMaximizeButton()->propertyValuePairs["BackgroundColor"].getColor() == tgui::Color::Green);
            REQUIRE(renderer->getMinimizeButton()->propertyValuePairs["BackgroundColor"].getColor() == tgui::Color::Blue);
        }

        SECTION("textured")
        {
            tgui::Texture textureTitleBar("resources/Black.png", {48, 154, 75, 25}, {16, 16, 16, 16});
            tgui::Texture textureBackground("resources/Black.png", {0, 154, 48, 48}, {16, 16, 16, 16});

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureTitleBar", tgui::Serializer::serialize(textureTitleBar)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", tgui::Serializer::serialize(textureBackground)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureTitleBar", textureTitleBar));
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", textureBackground));
            }

            SECTION("functions")
            {
                renderer->setTextureTitleBar(textureTitleBar);
                renderer->setTextureBackground(textureBackground);
            }

            REQUIRE(renderer->getProperty("TextureTitleBar").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureBackground").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTextureTitleBar().getData() == textureTitleBar.getData());
            REQUIRE(renderer->getTextureBackground().getData() == textureBackground.getData());
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

        auto widget = tgui::ClickableWidget::create();
        widget->setPosition(40, 20);
        childWindow->add(widget);

        SECTION("Save entire child window")
        {
            childWindow->setMinimumSize({40, 30});
            childWindow->setMaximumSize({200, 150});
            childWindow->setTitle("Title");
            childWindow->setTitleAlignment(tgui::HorizontalAlignment::Left);
            childWindow->setTitleButtons(tgui::ChildWindow::TitleButton::Close | tgui::ChildWindow::TitleButton::Minimize);
            childWindow->setCloseBehavior(tgui::ChildWindow::CloseBehavior::Hide);
            childWindow->setResizable();
            childWindow->setPositionLocked();
            childWindow->setKeepInParent(true);

            testSavingWidget("ChildWindow", childWindow);
        }

        SECTION("Only save contents")
        {
            REQUIRE_NOTHROW(childWindow->saveWidgetsToFile("ChildWindowWidgetFile4.txt"));

            childWindow->setSize(200, 100);
            REQUIRE_NOTHROW(childWindow->loadWidgetsFromFile("ChildWindowWidgetFile4.txt"));
            REQUIRE(childWindow->getSize() == tgui::Vector2f(200, 100)); // The child window itself is not saved, only its children

            REQUIRE_NOTHROW(childWindow->saveWidgetsToFile("ChildWindowWidgetFile5.txt"));
            REQUIRE(compareFiles("ChildWindowWidgetFile4.txt", "ChildWindowWidgetFile5.txt"));
        }
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(200, 185, childWindow)

        childWindow->setSize({186, 179});
        childWindow->setPosition({10, 5});
        childWindow->setTitle("Window");
        childWindow->setTitleTextSize(18);
        childWindow->setTitleButtons(tgui::ChildWindow::TitleButton::Maximize | tgui::ChildWindow::TitleButton::Minimize);
        childWindow->setTitleAlignment(tgui::HorizontalAlignment::Center);

        tgui::ChildWindowRenderer renderer = tgui::RendererData::create();
        renderer.setTitleBarColor(tgui::Color::Blue);
        renderer.setTitleColor(tgui::Color::Magenta);
        renderer.setBackgroundColor(tgui::Color::Green);
        renderer.setBorderColor(tgui::Color::Red);
        renderer.setBorderColorFocused(tgui::Color::Yellow);
        renderer.setDistanceToSide(10);
        renderer.setPaddingBetweenButtons(5);
        renderer.setShowTextOnTitleButtons(false);
        renderer.setTitleBarHeight(30);
        renderer.setBorderBelowTitleBar(1);
        renderer.setBorders({2, 3, 4, 5});
        renderer.setOpacity(0.7f);
        childWindow->setRenderer(renderer.getData());

        tgui::ButtonRenderer buttonRenderer = tgui::RendererData::create();
        buttonRenderer.setBackgroundColor(tgui::Color::Red);
        buttonRenderer.setBorderColor(tgui::Color::Green);
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

            childWindow->setTitleAlignment(tgui::HorizontalAlignment::Left);

            renderer.setShowTextOnTitleButtons(true);
            TEST_DRAW("ChildWindow_TitleButtonText.png")

            childWindow->setFocused(true);
            TEST_DRAW("ChildWindow_Focused.png")
        }

        SECTION("Textured")
        {
            childWindow->setTitleAlignment(tgui::HorizontalAlignment::Right);

            renderer.setTextureTitleBar("resources/Texture1.png");
            TEST_DRAW("ChildWindow_Textured.png")
        }
    }

    SECTION("Bug Fixes")
    {
        SECTION("setPosition doesn't work with KeepInParent (https://forum.tgui.eu/index.php?topic=1082.0)")
        {
            auto parent = tgui::Panel::create({300, 200});
            parent->setPosition({30, 25});
            parent->add(childWindow);

            childWindow->setSize({100, 80});
            childWindow->setPosition({10, 5});
            childWindow->setKeepInParent(true);

            childWindow->setPosition({15, 20});
            REQUIRE(childWindow->getPosition() == tgui::Vector2f{15, 20});
        }
    }
}
