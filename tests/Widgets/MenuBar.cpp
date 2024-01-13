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

TEST_CASE("[MenuBar]")
{
    tgui::MenuBar::Ptr menuBar = tgui::MenuBar::create();
    menuBar->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        menuBar->onMenuItemClick([](){});
        menuBar->onMenuItemClick([](const tgui::String&){});
        menuBar->onMenuItemClick([](const std::vector<tgui::String>&){});

        menuBar->connectMenuItem("File", "Save", [](){});
        menuBar->connectMenuItem({"Help", "About", "Version"}, [](){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(menuBar)->getSignal("MenuItemClicked").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(menuBar->getWidgetType() == "MenuBar");
    }

    SECTION("Position and Size")
    {
        SECTION("Manual size")
        {
            menuBar->setPosition(40, 30);
            menuBar->setSize(150, 100);

            REQUIRE(menuBar->getPosition() == tgui::Vector2f(40, 30));
            REQUIRE(menuBar->getSize() == tgui::Vector2f(150, 100));
            REQUIRE(menuBar->getFullSize() == menuBar->getSize());
            REQUIRE(menuBar->getWidgetOffset() == tgui::Vector2f(0, 0));

            SECTION("Width is unchanged when adding to parent")
            {
                auto parent = tgui::Panel::create();
                parent->add(menuBar);

                REQUIRE(menuBar->getSize() == tgui::Vector2f(150, 100));
            }
        }

        SECTION("Width is set when added to parent")
        {
            REQUIRE(menuBar->getSize().x == 0);

            auto parent = tgui::Panel::create();
            parent->setSize(400, 300);
            parent->add(menuBar);

            REQUIRE(menuBar->getSize().x == 400);

            SECTION("Width of parent is bound")
            {
                parent->setSize(50, 100);
                REQUIRE(menuBar->getSize().x == 50);
            }
        }
    }

    SECTION("Adding and removing menus")
    {
        SECTION("Single menu")
        {
            REQUIRE(menuBar->getMenus().empty());
            menuBar->addMenu("Help");
            REQUIRE(menuBar->getMenus().size() == 1);
            REQUIRE(menuBar->getMenus()[0].text == "Help");
            REQUIRE(menuBar->getMenus()[0].menuItems.empty());

            menuBar->addMenuItem("About");
            REQUIRE(menuBar->getMenus().size() == 1);
            REQUIRE(menuBar->getMenus()[0].text == "Help");
            REQUIRE(menuBar->getMenus()[0].menuItems.size() == 1);
            REQUIRE(menuBar->getMenus()[0].menuItems[0].text == "About");
        }

        SECTION("Multiple menus")
        {
            menuBar->addMenu("File");
            menuBar->addMenuItem("Load");
            menuBar->addMenuItem("Save");
            menuBar->addMenu("Edit");
            menuBar->addMenuItem("Undo");
            menuBar->addMenuItem("Redo");
            menuBar->addMenuItem("Copy");
            menuBar->addMenuItem("Paste");
            menuBar->addMenu("Help");
            menuBar->addMenuItem("About");

            SECTION("Verify that menus were added")
            {
                REQUIRE(menuBar->getMenus().size() == 3);
                REQUIRE(menuBar->getMenus()[0].text == "File");
                REQUIRE(menuBar->getMenus()[0].menuItems.size() == 2);
                REQUIRE(menuBar->getMenus()[0].menuItems[0].text == "Load");
                REQUIRE(menuBar->getMenus()[0].menuItems[1].text == "Save");
                REQUIRE(menuBar->getMenus()[1].text == "Edit");
                REQUIRE(menuBar->getMenus()[1].menuItems.size() == 4);
                REQUIRE(menuBar->getMenus()[1].menuItems[0].text == "Undo");
                REQUIRE(menuBar->getMenus()[1].menuItems[1].text == "Redo");
                REQUIRE(menuBar->getMenus()[1].menuItems[2].text == "Copy");
                REQUIRE(menuBar->getMenus()[1].menuItems[3].text == "Paste");
                REQUIRE(menuBar->getMenus()[2].text == "Help");
                REQUIRE(menuBar->getMenus()[2].menuItems.size() == 1);
                REQUIRE(menuBar->getMenus()[2].menuItems[0].text == "About");
            }

            SECTION("Adding menu items to older menu")
            {
                REQUIRE(menuBar->addMenuItem("File", "Quit"));

                REQUIRE(menuBar->getMenus().size() == 3);
                REQUIRE(menuBar->getMenus()[0].text == "File");
                REQUIRE(menuBar->getMenus()[0].menuItems.size() == 3);
                REQUIRE(menuBar->getMenus()[0].menuItems[0].text == "Load");
                REQUIRE(menuBar->getMenus()[0].menuItems[1].text == "Save");
                REQUIRE(menuBar->getMenus()[0].menuItems[2].text == "Quit");
                REQUIRE(menuBar->getMenus()[1].text == "Edit");
                REQUIRE(menuBar->getMenus()[1].menuItems.size() == 4);
                REQUIRE(menuBar->getMenus()[2].text == "Help");
                REQUIRE(menuBar->getMenus()[2].menuItems.size() == 1);
            }

            SECTION("Removing menu items")
            {
                menuBar->removeMenuItem("Edit", "Undo");
                menuBar->removeMenuItem("Edit", "Paste");
                menuBar->removeMenuItem("Help", "About");

                REQUIRE(menuBar->getMenus()[0].text == "File");
                REQUIRE(menuBar->getMenus()[0].menuItems.size() == 2);
                REQUIRE(menuBar->getMenus()[1].text == "Edit");
                REQUIRE(menuBar->getMenus()[1].menuItems.size() == 2);
                REQUIRE(menuBar->getMenus()[1].menuItems[0].text == "Redo");
                REQUIRE(menuBar->getMenus()[1].menuItems[1].text == "Copy");
                REQUIRE(menuBar->getMenus()[2].text == "Help");
                REQUIRE(menuBar->getMenus()[2].menuItems.size() == 0);
            }

            SECTION("Removing menu")
            {
                menuBar->removeMenu("File");

                REQUIRE(menuBar->getMenus().size() == 2);
                REQUIRE(menuBar->getMenus()[0].text == "Edit");
                REQUIRE(menuBar->getMenus()[0].menuItems.size() == 4);
                REQUIRE(menuBar->getMenus()[1].text == "Help");
                REQUIRE(menuBar->getMenus()[1].menuItems.size() == 1);
            }

            SECTION("Removing all menus")
            {
                menuBar->removeAllMenus();
                REQUIRE(menuBar->getMenus().empty());
            }

            SECTION("Submenus")
            {
                REQUIRE(menuBar->addMenuItem({"File", "Other", "Print"}));
                REQUIRE(menuBar->addMenuItem({"File", "Other", "Extra", "Quit"}));

                REQUIRE(menuBar->getMenus()[0].menuItems.size() == 3);
                REQUIRE(menuBar->getMenus()[0].menuItems[2].text == "Other");
                REQUIRE(menuBar->getMenus()[0].menuItems[2].menuItems.size() == 2);

                REQUIRE(menuBar->removeMenuItem({"File", "Other", "Extra", "Quit"}));
                REQUIRE(menuBar->getMenus()[0].menuItems[2].menuItems.size() == 1);

                REQUIRE(menuBar->removeMenuItem({"File", "Other", "Print"}));
                REQUIRE(menuBar->getMenus()[0].menuItems.size() == 2);
            }
        }

        SECTION("Invalid calls")
        {
            REQUIRE(!menuBar->addMenuItem("Item"));

            menuBar->addMenu("File");
            REQUIRE(!menuBar->addMenuItem("Edit", "Item"));

            REQUIRE(!menuBar->addMenuItem(std::vector<tgui::String>()));
            REQUIRE(!menuBar->removeMenuItem({"File", "Other", "Print"}));
            REQUIRE(!menuBar->addMenuItem({"File", "Other", "Extra", "Quit"}, false));
        }
    }

    SECTION("Renaming menus")
    {
        menuBar->addMenu("File");
        menuBar->addMenuItem("Load");
        menuBar->addMenuItem("Save");
        menuBar->addMenu("Edit");
        menuBar->addMenuItem("Undo");
        menuBar->addMenuItem("Redo");
        menuBar->addMenuItem("Copy");
        menuBar->addMenuItem("Paste");
        menuBar->addMenuItem({"This", "is", "an experiment"});
        menuBar->addMenuItem({"This", "is", "a test"});

        // Rename a menu
        REQUIRE(menuBar->getMenus()[1].text == "Edit");
        REQUIRE(menuBar->changeMenuItem({"Edit"}, "EDIT"));
        REQUIRE(menuBar->getMenus()[1].text == "EDIT");

        // We can't access the menu items with the old name
        REQUIRE(!menuBar->changeMenuItem({"Edit", "Redo"}, "REDO"));
        REQUIRE(menuBar->getMenus()[1].menuItems[1].text == "Redo");

        // Rename a menu item
        REQUIRE(menuBar->changeMenuItem({"EDIT", "Copy"}, "COPY"));
        REQUIRE(menuBar->getMenus()[1].menuItems[2].text == "COPY");

        // Rename a menu item in a sub menu
        REQUIRE(menuBar->changeMenuItem({"This", "is", "a test"}, "THE test"));
        REQUIRE(menuBar->getMenus()[2].menuItems[0].menuItems[1].text == "THE test");

        // We can't rename something that doesn't exist
        REQUIRE(!menuBar->changeMenuItem({"This", "won't", "work"}, "Just testing"));
    }

    SECTION("Disabling menus")
    {
        menuBar->addMenu("M1");
        menuBar->addMenuItem("I1");
        menuBar->addMenuItem("I2");
        menuBar->addMenu("M2");
        menuBar->addMenuItem("I3");

        REQUIRE(menuBar->getMenuEnabled("M1"));
        REQUIRE(menuBar->setMenuEnabled("M1", false));
        REQUIRE(!menuBar->getMenuEnabled("M1"));
        REQUIRE(menuBar->getMenuEnabled("M2"));

        REQUIRE(menuBar->getMenuItemEnabled("M1", "I2"));
        REQUIRE(menuBar->setMenuItemEnabled("M1", "I2", false));
        REQUIRE(menuBar->getMenuItemEnabled("M1", "I1"));
        REQUIRE(!menuBar->getMenuItemEnabled("M1", "I2"));
        REQUIRE(menuBar->getMenuItemEnabled("M2", "I3"));

        // Check that getMenuList confirms that the menus are disabled
        REQUIRE(menuBar->getMenus().size() == 2);
        REQUIRE(!menuBar->getMenus()[0].enabled);
        REQUIRE(menuBar->getMenus()[0].menuItems.size() == 2);
        REQUIRE(menuBar->getMenus()[0].menuItems[0].enabled);
        REQUIRE(!menuBar->getMenus()[0].menuItems[1].enabled);
        REQUIRE(menuBar->getMenus()[1].enabled);
        REQUIRE(menuBar->getMenus()[1].menuItems.size() == 1);
        REQUIRE(menuBar->getMenus()[1].menuItems[0].enabled);

        // Test re-enabling menus
        REQUIRE(menuBar->setMenuEnabled("M1", true));
        REQUIRE(menuBar->setMenuItemEnabled("M1", "I2", true));
        REQUIRE(menuBar->getMenuEnabled("M1"));
        REQUIRE(menuBar->getMenuItemEnabled("M1", "I2"));

        REQUIRE(menuBar->setMenuEnabled("M2", true));
        REQUIRE(menuBar->setMenuItemEnabled("M2", "I3", true));
        REQUIRE(menuBar->getMenuEnabled("M2"));
        REQUIRE(menuBar->getMenuItemEnabled("M2", "I3"));

        // Setter returns false if menu doesn't exist
        REQUIRE(!menuBar->setMenuEnabled("M3", false));
        REQUIRE(!menuBar->setMenuItemEnabled("M3", "I1", false));
        REQUIRE(!menuBar->setMenuItemEnabled("M1", "I3", false));

        SECTION("Submenus")
        {
            REQUIRE(!menuBar->setMenuItemEnabled({"M1", "I2", "S1"}, true));
            REQUIRE(!menuBar->getMenuItemEnabled({"M1", "I2", "S1"}));

            REQUIRE(menuBar->addMenuItem({"M1", "I2", "S1"}, false));
            REQUIRE(menuBar->getMenuItemEnabled({"M1", "I2", "S1"}));

            REQUIRE(menuBar->setMenuItemEnabled({"M1", "I2", "S1"}, false));
            REQUIRE(!menuBar->getMenuItemEnabled({"M1", "I2", "S1"}));
        }
    }

    SECTION("TextSize")
    {
        menuBar->setTextSize(25);
        REQUIRE(menuBar->getTextSize() == 25);
    }

    SECTION("MinimumSubMenuWidth")
    {
        menuBar->setMinimumSubMenuWidth(150);
        REQUIRE(menuBar->getMinimumSubMenuWidth() == 150);
    }

    SECTION("InvertedMenuDirection")
    {
        REQUIRE(!menuBar->getInvertedMenuDirection());
        menuBar->setInvertedMenuDirection(true);
        REQUIRE(menuBar->getInvertedMenuDirection());
        menuBar->setInvertedMenuDirection(false);
        REQUIRE(!menuBar->getInvertedMenuDirection());
    }

    SECTION("Events / Signals")
    {
        menuBar->setHeight(25);
        menuBar->addMenu("File");
        menuBar->addMenuItem("Load");
        menuBar->addMenuItem("Save");
        menuBar->addMenu("Edit");
        menuBar->addMenuItem("Undo");
        menuBar->addMenuItem("Redo");
        menuBar->addMenuItem("Copy");
        menuBar->addMenuItem("Paste");
        menuBar->addMenu("Help");
        menuBar->addMenuItem("About");

        SECTION("Widget")
        {
            testWidgetSignals(menuBar);
        }

        // The menu bar needs to be attached to a Gui object as it will create a new widget when a menu opens.
        // All events also need to be send to the gui to determine to which widget the event goes.
        globalGui->add(menuBar);
        auto simulateLeftMouseClick = [](int x, int y){
            tgui::Event event;
            event.type = tgui::Event::Type::MouseMoved;
            event.mouseMove.x = x;
            event.mouseMove.y = y;
            globalGui->handleEvent(event);

            event.type = tgui::Event::Type::MouseButtonPressed;
            event.mouseButton.button = tgui::Event::MouseButton::Left;
            event.mouseButton.x = x;
            event.mouseButton.y = y;
            globalGui->handleEvent(event);

            event.type = tgui::Event::Type::MouseButtonReleased;
            event.mouseButton.button = tgui::Event::MouseButton::Left;
            event.mouseButton.x = x;
            event.mouseButton.y = y;
            globalGui->handleEvent(event);
        };

        SECTION("Opening and closing menu")
        {
            simulateLeftMouseClick(50, 10);

            SECTION("By clicking the menu again")
            {
                simulateLeftMouseClick(50, 10);
            }

            SECTION("By clicking on a menu item")
            {
                simulateLeftMouseClick(50, 90);
            }

            SECTION("By clicking outside the menu bar")
            {
                simulateLeftMouseClick(10, 200);
            }
        }

        /// TODO: more event tests

        globalGui->removeAllWidgets();
    }

    testWidgetRenderer(menuBar->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = menuBar->getRenderer();

        SECTION("colored")
        {
            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(10, 20, 30)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColor", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", "rgb(100, 110, 120)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDisabled", "rgb(130, 140, 150)"));
                REQUIRE_NOTHROW(renderer->setProperty("SeparatorColor", "rgb(160, 170, 180)"));
                REQUIRE_NOTHROW(renderer->setProperty("DistanceToSide", "2"));
                REQUIRE_NOTHROW(renderer->setProperty("SeparatorThickness", "3"));
                REQUIRE_NOTHROW(renderer->setProperty("SeparatorVerticalPadding", "4"));
                REQUIRE_NOTHROW(renderer->setProperty("SeparatorSidePadding", "5"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", tgui::Color{10, 20, 30}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColor", tgui::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", tgui::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", tgui::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColorDisabled", tgui::Color{130, 140, 150}));
                REQUIRE_NOTHROW(renderer->setProperty("SeparatorColor", tgui::Color{160, 170, 180}));
                REQUIRE_NOTHROW(renderer->setProperty("DistanceToSide", 2));
                REQUIRE_NOTHROW(renderer->setProperty("SeparatorThickness", 3));
                REQUIRE_NOTHROW(renderer->setProperty("SeparatorVerticalPadding", 4));
                REQUIRE_NOTHROW(renderer->setProperty("SeparatorSidePadding", 5));
            }

            SECTION("functions")
            {
                renderer->setBackgroundColor({10, 20, 30});
                renderer->setSelectedBackgroundColor({40, 50, 60});
                renderer->setTextColor({70, 80, 90});
                renderer->setSelectedTextColor({100, 110, 120});
                renderer->setTextColorDisabled({130, 140, 150});
                renderer->setSeparatorColor({160, 170, 180});
                renderer->setDistanceToSide(2);
                renderer->setSeparatorThickness(3);
                renderer->setSeparatorVerticalPadding(4);
                renderer->setSeparatorSidePadding(5);
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == tgui::Color(10, 20, 30));
            REQUIRE(renderer->getProperty("SelectedBackgroundColor").getColor() == tgui::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("TextColor").getColor() == tgui::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("SelectedTextColor").getColor() == tgui::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("TextColorDisabled").getColor() == tgui::Color(130, 140, 150));
            REQUIRE(renderer->getProperty("SeparatorColor").getColor() == tgui::Color(160, 170, 180));
            REQUIRE(renderer->getProperty("DistanceToSide").getNumber() == 2);
            REQUIRE(renderer->getProperty("SeparatorThickness").getNumber() == 3);
            REQUIRE(renderer->getProperty("SeparatorVerticalPadding").getNumber() == 4);
            REQUIRE(renderer->getProperty("SeparatorSidePadding").getNumber() == 5);
        }

        SECTION("textured")
        {
            tgui::Texture textureBackground("resources/Black.png", {115, 179, 8, 6}, {2, 2, 4, 2});
            tgui::Texture textureItemBackground("resources/Black.png", {115, 181, 8, 4}, {2, 0, 4, 2});
            tgui::Texture textureSelectedItemBackground("resources/Black.png", {115, 185, 8, 6}, {2, 2, 4, 2});

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", tgui::Serializer::serialize(textureBackground)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureItemBackground", tgui::Serializer::serialize(textureItemBackground)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureSelectedItemBackground", tgui::Serializer::serialize(textureSelectedItemBackground)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", textureBackground));
                REQUIRE_NOTHROW(renderer->setProperty("TextureItemBackground", textureItemBackground));
                REQUIRE_NOTHROW(renderer->setProperty("TextureSelectedItemBackground", textureSelectedItemBackground));
            }

            SECTION("functions")
            {
                renderer->setTextureBackground(textureBackground);
                renderer->setTextureItemBackground(textureItemBackground);
                renderer->setTextureSelectedItemBackground(textureSelectedItemBackground);
            }

            REQUIRE(renderer->getProperty("TextureBackground").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureItemBackground").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureSelectedItemBackground").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTextureBackground().getData() == textureBackground.getData());
            REQUIRE(renderer->getTextureItemBackground().getData() == textureItemBackground.getData());
            REQUIRE(renderer->getTextureSelectedItemBackground().getData() == textureSelectedItemBackground.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        menuBar->addMenu("File");
        menuBar->addMenuItem("Quit");
        menuBar->addMenu("Edit");
        menuBar->addMenuItem("Undo");
        menuBar->addMenuItem("Redo");
        menuBar->addMenu("Help");
        menuBar->addMenuItem({"Help", "About", "Version", "Whatever"});
        menuBar->setMenuItemEnabled({"Help", "About"}, false);

        menuBar->setMinimumSubMenuWidth(100);
        menuBar->setTextSize(25); // TextSize is currently reset when copying (due to setSize calling setTextSize)

        testSavingWidget("MenuBar", menuBar);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(150, 70, menuBar)

        menuBar->setEnabled(true);
        menuBar->setPosition(10, 5);
        menuBar->setSize(130, 20);
        menuBar->setTextSize(16);
        menuBar->setMinimumSubMenuWidth(60);

        menuBar->addMenu("File");
        menuBar->addMenuItem("Quit");
        menuBar->addMenu("Edit");
        menuBar->addMenuItem("Undo");
        menuBar->addMenuItem("Redo");
        menuBar->addMenu("Help");

        menuBar->setMenuEnabled("File", false);
        menuBar->setMenuItemEnabled("Edit", "Redo", false);

        tgui::MenuBarRenderer renderer = tgui::RendererData::create();
        renderer.setTextColor(tgui::Color::Red);
        renderer.setSelectedTextColor(tgui::Color::Blue);
        renderer.setTextColorDisabled(tgui::Color::Black);
        renderer.setBackgroundColor(tgui::Color::Green);
        renderer.setSelectedBackgroundColor(tgui::Color::Yellow);
        renderer.setDistanceToSide(3);
        renderer.setOpacity(0.7f);
        menuBar->setRenderer(renderer.getData());

        auto container = gui.getContainer();

        SECTION("Colored")
        {
            TEST_DRAW("MenuBar.png")

            tgui::Vector2f mousePos = {35, 20};
            container->mouseMoved(mousePos);
            container->leftMousePressed(mousePos);
            container->leftMouseReleased(mousePos);
            TEST_DRAW("MenuBar_MenuDisabled.png")

            mousePos = {62, 20};
            container->mouseMoved(mousePos);
            container->leftMousePressed(mousePos);
            container->leftMouseReleased(mousePos);
            TEST_DRAW("MenuBar_MenuOpen.png")

            mousePos = {62, 40};
            container->mouseMoved(mousePos);
            TEST_DRAW("MenuBar_MenuHover.png")

            mousePos = {62, 60};
            container->mouseMoved(mousePos);
            TEST_DRAW("MenuBar_MenuHoverDisabled.png")

            menuBar->setEnabled(false);
            TEST_DRAW("MenuBar_Disabled.png")
        }

        SECTION("Textured")
        {
            renderer.setTextureBackground("resources/Texture1.png");
            renderer.setTextureItemBackground("resources/Texture2.png");
            renderer.setTextureSelectedItemBackground("resources/Texture3.png");

            TEST_DRAW("MenuBar_Textured.png")

            tgui::Vector2f mousePos = {35, 20};
            container->mouseMoved(mousePos);
            container->leftMousePressed(mousePos);
            container->leftMouseReleased(mousePos);
            TEST_DRAW("MenuBar_MenuDisabled_Textured.png")

            mousePos = {62, 20};
            container->mouseMoved(mousePos);
            container->leftMousePressed(mousePos);
            container->leftMouseReleased(mousePos);
            TEST_DRAW("MenuBar_MenuOpen_Textured.png")

            mousePos = {62, 40};
            container->mouseMoved(mousePos);
            TEST_DRAW("MenuBar_MenuHover_Textured.png")

            mousePos = {62, 60};
            container->mouseMoved(mousePos);
            TEST_DRAW("MenuBar_MenuHoverDisabled_Textured.png")

            menuBar->setEnabled(false);
            TEST_DRAW("MenuBar_Disabled_Textured.png")
        }
    }

    SECTION("Scaling")
    {
        menuBar->setPosition(10, 5);
        menuBar->setSize(130, 20);
        menuBar->setTextSize(16);
        menuBar->setMinimumSubMenuWidth(60);

        menuBar->addMenu("File");
        menuBar->addMenuItem("Quit");
        menuBar->addMenu("Edit");
        menuBar->addMenuItem("Undo");
        menuBar->addMenuItem("Redo");
        menuBar->addMenu("Help");

        tgui::MenuBarRenderer renderer = tgui::RendererData::create();
        renderer.setTextColor(tgui::Color::Red);
        renderer.setSelectedTextColor(tgui::Color::Blue);
        renderer.setBackgroundColor(tgui::Color::Green);
        renderer.setSelectedBackgroundColor(tgui::Color::Yellow);
        renderer.setDistanceToSide(3);
        menuBar->setRenderer(renderer.getData());

        auto outerPanel = tgui::Panel::create({390, 185});
        outerPanel->getRenderer()->setBackgroundColor(tgui::Color::Red);
        outerPanel->getRenderer()->setBorderColor(tgui::Color::Cyan);
        outerPanel->getRenderer()->setBorders({4});
        outerPanel->setPosition({5, 10});

        auto innerPanel = tgui::Panel::create({220, 80});
        innerPanel->getRenderer()->setBackgroundColor(tgui::Color::Blue);
        innerPanel->getRenderer()->setBorderColor(tgui::Color::Magenta);
        innerPanel->getRenderer()->setBorders({5});
        innerPanel->setPosition({20, 15});
        outerPanel->add(innerPanel);

        innerPanel->add(menuBar);

        outerPanel->setScale(1.25f);
        innerPanel->setScale(1.6f);
        menuBar->setScale(1.5f);

        TEST_DRAW_INIT(500, 250, outerPanel)
        TEST_DRAW("MenuBar_Scaling_Normal.png")

        tgui::Event event;
        event.type = tgui::Event::Type::MouseMoved;
        event.mouseMove.x = 190;
        event.mouseMove.y = 70;
        gui.handleEvent(event);

        event.type = tgui::Event::Type::MouseButtonPressed;
        event.mouseButton.button = tgui::Event::MouseButton::Left;
        event.mouseButton.x = 190;
        event.mouseButton.y = 70;
        gui.handleEvent(event);

        event.type = tgui::Event::Type::MouseButtonReleased;
        gui.handleEvent(event);
        TEST_DRAW("MenuBar_Scaling_MenuOpened.png")

        event.type = tgui::Event::Type::MouseMoved;
        event.mouseMove.x = 190;
        event.mouseMove.y = 190;
        gui.handleEvent(event);
        TEST_DRAW("MenuBar_Scaling_HoveredItemChanged.png")

        event.type = tgui::Event::Type::MouseMoved;
        event.mouseMove.x = 150;
        event.mouseMove.y = 70;
        gui.handleEvent(event);
        TEST_DRAW("MenuBar_Scaling_OpenMenuChanged.png")
    }
}
