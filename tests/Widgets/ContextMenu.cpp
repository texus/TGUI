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

TEST_CASE("[ContextMenu]")
{
    const tgui::ContextMenu::Ptr contextMenu = tgui::ContextMenu::create();
    contextMenu->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        contextMenu->onMenuItemClick([]() {});
        contextMenu->onMenuItemClick([](const tgui::String&) {});
        contextMenu->onMenuItemClick([](const std::vector<tgui::String>&) {});

        contextMenu->connectMenuItem("Save", []() {});
        contextMenu->connectMenuItem({"Help", "About", "Version"}, []() {});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(contextMenu)->getSignal("MenuItemClicked").connect([] {}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(contextMenu->getWidgetType() == "ContextMenu");
    }

    SECTION("Position and Size")
    {
        REQUIRE(contextMenu->addMenuItem({"Smilies", "Happy"}));
        REQUIRE(contextMenu->addMenuItem({"Smilies", "Sad"}));
        REQUIRE(contextMenu->addMenuItem({"Smilies", "Neither"}));
        REQUIRE(contextMenu->addMenuItem({"Vehicles", "Parts", "Wheel"}));
        REQUIRE(contextMenu->addMenuItem({"Vehicles", "Whole", "Truck"}));
        REQUIRE(contextMenu->addMenuItem({"Vehicles", "Whole", "Car"}));

        contextMenu->setMinimumMenuWidth(300);
        contextMenu->setItemHeight(20);
        contextMenu->setPosition(40, 30);
        contextMenu->setSize(150, 100); // Has no effect

        REQUIRE(contextMenu->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(contextMenu->getWidgetOffset() == tgui::Vector2f(0, 0));

        // The position can be set when opening the menu
        contextMenu->openMenu({80, 60});
        REQUIRE(contextMenu->getPosition() == tgui::Vector2f(80, 60));
        REQUIRE(contextMenu->getWidgetOffset() == tgui::Vector2f(0, 0));

        // The context menu has no size. It's menu is an overlay that is a separate widget.
        REQUIRE(contextMenu->getSize() == tgui::Vector2f(0, 0));
        REQUIRE(contextMenu->getFullSize() == contextMenu->getSize());
    }

    SECTION("Open / close")
    {
        contextMenu->addMenuItem("Load");
        contextMenu->addMenuItem("Save");

        REQUIRE(!contextMenu->isMenuOpen());

        contextMenu->openMenu();
        REQUIRE(contextMenu->isMenuOpen());

        contextMenu->closeMenu();
        REQUIRE(!contextMenu->isMenuOpen());
    }

    SECTION("ItemHeight")
    {
        contextMenu->setItemHeight(25);
        REQUIRE(contextMenu->getItemHeight() == 25);
    }

    SECTION("MinimumMenuWidth")
    {
        contextMenu->setMinimumMenuWidth(250);
        REQUIRE(contextMenu->getMinimumMenuWidth() == 250);
    }

    SECTION("TextSize")
    {
        contextMenu->setTextSize(20);
        REQUIRE(contextMenu->getTextSize() == 20);
    }

    SECTION("Changing menu items")
    {
        contextMenu->addMenuItem("Cut");
        contextMenu->addMenuItem("Copy");
        contextMenu->addMenuItem("Paste");
        contextMenu->addMenuItem("-");
        contextMenu->addMenuItem({"Edit", "Undo"});
        contextMenu->addMenuItem({"Edit", "Redo"});
        contextMenu->addMenuItem({"Edit", "Delete"});

        contextMenu->changeMenuItem({"Paste"}, "PASTE");
        contextMenu->changeMenuItem({"Edit", "Redo"}, "REDO");

        REQUIRE(contextMenu->getMenuItems().size() == 5);
        REQUIRE(contextMenu->getMenuItems()[0].text == "Cut");
        REQUIRE(contextMenu->getMenuItems()[0].menuItems.empty());
        REQUIRE(contextMenu->getMenuItems()[1].text == "Copy");
        REQUIRE(contextMenu->getMenuItems()[2].text == "PASTE");
        REQUIRE(contextMenu->getMenuItems()[3].text == "-");
        REQUIRE(contextMenu->getMenuItems()[4].text == "Edit");
        REQUIRE(contextMenu->getMenuItems()[4].menuItems.size() == 3);
        REQUIRE(contextMenu->getMenuItems()[4].menuItems[0].text == "Undo");
        REQUIRE(contextMenu->getMenuItems()[4].menuItems[1].text == "REDO");
        REQUIRE(contextMenu->getMenuItems()[4].menuItems[2].text == "Delete");

        contextMenu->removeMenuItem("Copy");
        contextMenu->removeMenuItem({"Edit", "Delete"});
        REQUIRE(contextMenu->getMenuItems().size() == 4);
        REQUIRE(contextMenu->getMenuItems()[0].text == "Cut");
        REQUIRE(contextMenu->getMenuItems()[1].text == "PASTE");
        REQUIRE(contextMenu->getMenuItems()[2].text == "-");
        REQUIRE(contextMenu->getMenuItems()[3].text == "Edit");
        REQUIRE(contextMenu->getMenuItems()[3].menuItems.size() == 2);
        REQUIRE(contextMenu->getMenuItems()[3].menuItems[0].text == "Undo");
        REQUIRE(contextMenu->getMenuItems()[3].menuItems[1].text == "REDO");

        contextMenu->removeSubMenuItems({"Edit"});
        REQUIRE(contextMenu->getMenuItems().size() == 4);
        REQUIRE(contextMenu->getMenuItems()[3].text == "Edit");
        REQUIRE(contextMenu->getMenuItems()[3].menuItems.empty());

        contextMenu->removeAllMenuItems();
        REQUIRE(contextMenu->getMenuItems().empty());
    }

    SECTION("Menu items enabled / disabled")
    {
        contextMenu->addMenuItem("Copy");
        contextMenu->addMenuItem("Paste");
        contextMenu->addMenuItem({"Edit", "Undo"});
        contextMenu->addMenuItem({"Edit", "Redo"});

        contextMenu->setMenuItemEnabled("Paste", false);
        contextMenu->setMenuItemEnabled({"Edit", "Redo"}, false);

        REQUIRE(contextMenu->getMenuItemEnabled("Copy"));
        REQUIRE(!contextMenu->getMenuItemEnabled("Paste"));
        REQUIRE(contextMenu->getMenuItemEnabled({"Edit", "Undo"}));
        REQUIRE(!contextMenu->getMenuItemEnabled({"Edit", "Redo"}));
    }

    SECTION("Events / Signals")
    {
        contextMenu->addMenuItem("Copy");
        contextMenu->addMenuItem("Paste");
        contextMenu->addMenuItem({"Edit", "Undo"});
        contextMenu->addMenuItem({"Edit", "Redo"});
        contextMenu->setMinimumMenuWidth(150);
        contextMenu->setItemHeight(20);

        unsigned int callbackCount = 0;
        std::vector<tgui::String> expectedHierarchy;
        contextMenu->onMenuItemClick(
            [&](const std::vector<tgui::String>& hierarchy)
            {
                ++callbackCount;
                REQUIRE(expectedHierarchy == hierarchy);
            });

        // The context menu needs to be attached to a Gui object as it will create a new widget when the menu opens.
        // All events also need to be send to the gui to determine to which widget the event goes.
        globalGui->add(contextMenu);
        auto simulateMouseMove = [](int x, int y)
        {
            tgui::Event event;
            event.type = tgui::Event::Type::MouseMoved;
            event.mouseMove.x = x;
            event.mouseMove.y = y;
            globalGui->handleEvent(event);
        };
        auto simulateLeftMouseClick = [](int x, int y)
        {
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

        contextMenu->openMenu({100, 50});
        contextMenu->closeMenu();

        // The context menu isn't shown and thus won't react while the menu is closed
        contextMenu->setPosition({100, 50});
        simulateLeftMouseClick(110, 60);
        REQUIRE(callbackCount == 0);

        // Clicking on one of the menu items sends a callback and closes the menu
        expectedHierarchy = {"Copy"};
        contextMenu->openMenu();
        REQUIRE(contextMenu->isMenuOpen());
        simulateLeftMouseClick(110, 60);
        REQUIRE(callbackCount == 1);
        REQUIRE(!contextMenu->isMenuOpen());

        // The submenu isn't open if we don't hover on the parent item first
        contextMenu->openMenu();
        simulateLeftMouseClick(260, 120);
        REQUIRE(callbackCount == 1);

        // Clicking on one of the menu items in a submenu also sends a callback and closes the menu
        expectedHierarchy = {"Edit", "Redo"};
        contextMenu->openMenu();
        simulateMouseMove(110, 100); // Hover on Edit to open the submenu
        simulateLeftMouseClick(260, 120);
        REQUIRE(callbackCount == 2);
        REQUIRE(!contextMenu->isMenuOpen());

        // The menu item no longer sends a callback when it is disabled
        contextMenu->setMenuItemEnabled({"Edit", "Redo"}, false);
        contextMenu->openMenu();
        simulateMouseMove(110, 100); // Hover on Edit to open the submenu
        simulateLeftMouseClick(260, 120);
        REQUIRE(callbackCount == 2);

        // Clicking beside the menu will close it
        contextMenu->openMenu();
        simulateLeftMouseClick(251, 80);
        REQUIRE(callbackCount == 2);
        REQUIRE(!contextMenu->isMenuOpen());

        // Adding a long item makes the menu wider, so clicking on the same spot again will now trigger a callback
        expectedHierarchy = {"Paste"};
        contextMenu->addMenuItem("Some very long item that will make the context menu wider");
        contextMenu->openMenu();
        simulateLeftMouseClick(251, 80);
        REQUIRE(callbackCount == 3);

        globalGui->removeAllWidgets();
    }

    testWidgetRenderer(contextMenu->getRenderer());
    SECTION("Renderer")
    {
        auto* renderer = contextMenu->getRenderer();

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
            const tgui::Texture textureItemBackground("resources/Black.png", {115, 181, 8, 4}, {2, 0, 4, 2});
            const tgui::Texture textureSelectedItemBackground("resources/Black.png", {115, 185, 8, 6}, {2, 2, 4, 2});

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureItemBackground", tgui::Serializer::serialize(textureItemBackground)));
                REQUIRE_NOTHROW(
                    renderer->setProperty("TextureSelectedItemBackground", tgui::Serializer::serialize(textureSelectedItemBackground)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureItemBackground", textureItemBackground));
                REQUIRE_NOTHROW(renderer->setProperty("TextureSelectedItemBackground", textureSelectedItemBackground));
            }

            SECTION("functions")
            {
                renderer->setTextureItemBackground(textureItemBackground);
                renderer->setTextureSelectedItemBackground(textureSelectedItemBackground);
            }

            REQUIRE(renderer->getProperty("TextureItemBackground").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureSelectedItemBackground").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTextureItemBackground().getData() == textureItemBackground.getData());
            REQUIRE(renderer->getTextureSelectedItemBackground().getData() == textureSelectedItemBackground.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        contextMenu->addMenuItem("Copy");
        contextMenu->addMenuItem("Paste");
        contextMenu->addMenuItem({"Edit", "Undo"});
        contextMenu->addMenuItem({"Edit", "Redo"});
        contextMenu->setMenuItemEnabled({"Edit", "Redo"}, false);

        contextMenu->setMinimumMenuWidth(100);
        contextMenu->setItemHeight(30);
        contextMenu->setTextSize(25);

        testSavingWidget("ContextMenu", contextMenu);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(140, 90, contextMenu)

        contextMenu->setEnabled(true);
        contextMenu->setPosition(10, 5);
        contextMenu->setTextSize(16);
        contextMenu->setItemHeight(20);
        contextMenu->setMinimumMenuWidth(60);

        contextMenu->addMenuItem("Copy");
        contextMenu->addMenuItem("Paste");
        contextMenu->addMenuItem({"Edit", "Undo"});
        contextMenu->addMenuItem({"Edit", "Redo"});

        contextMenu->setMenuItemEnabled("Paste", false);
        contextMenu->setMenuItemEnabled({"Edit", "Redo"}, false);

        tgui::ContextMenuRenderer renderer = tgui::RendererData::create();
        renderer.setTextColor(tgui::Color::Red);
        renderer.setSelectedTextColor(tgui::Color::Blue);
        renderer.setTextColorDisabled(tgui::Color::Black);
        renderer.setBackgroundColor(tgui::Color::Green);
        renderer.setSelectedBackgroundColor(tgui::Color::Yellow);
        renderer.setDistanceToSide(3);
        renderer.setOpacity(0.7f);
        contextMenu->setRenderer(renderer.getData());

        auto container = gui.getContainer();

        SECTION("Closed"){TEST_DRAW("ContextMenu_Closed.png")}

        contextMenu->openMenu();

        SECTION("Colored")
        {
            TEST_DRAW("ContextMenu.png")

            tgui::Vector2f mousePos = {20, 35};
            container->mouseMoved(mousePos);
            TEST_DRAW("ContextMenu_HoverDisabled.png")

            mousePos = {20, 55};
            container->mouseMoved(mousePos);
            TEST_DRAW("ContextMenu_SubMenuOpen.png")

            mousePos = {80, 55};
            container->mouseMoved(mousePos);
            TEST_DRAW("ContextMenu_SubMenuItemHover.png")
        }

        SECTION("Textured")
        {
            renderer.setTextureItemBackground("resources/Texture2.png");
            renderer.setTextureSelectedItemBackground("resources/Texture3.png");

            TEST_DRAW("ContextMenu_Textured.png")

            tgui::Vector2f mousePos = {20, 35};
            container->mouseMoved(mousePos);
            TEST_DRAW("ContextMenu_HoverDisabled_Textured.png")

            mousePos = {20, 55};
            container->mouseMoved(mousePos);
            TEST_DRAW("ContextMenu_SubMenuOpen_Textured.png")

            mousePos = {80, 55};
            container->mouseMoved(mousePos);
            TEST_DRAW("ContextMenu_SubMenuItemHover_Textured.png")
        }
    }
}
