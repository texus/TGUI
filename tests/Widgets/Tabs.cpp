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

TEST_CASE("[Tabs]")
{
    tgui::Tabs::Ptr tabs = tgui::Tabs::create();
    tabs->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("Signals")
    {
        tabs->onTabSelect([](){});
        tabs->onTabSelect([](const tgui::String&){});

        REQUIRE_NOTHROW(tgui::Widget::Ptr(tabs)->getSignal("TabSelected").connect([]{}));
    }

    SECTION("WidgetType")
    {
        REQUIRE(tabs->getWidgetType() == "Tabs");
    }

    SECTION("Position and Size")
    {
        tabs->setPosition(40, 30);
        tabs->setTabHeight(24);
        tabs->getRenderer()->setBorders(2);

        REQUIRE(tabs->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(tabs->getSize() == tgui::Vector2f(4, 24));
        REQUIRE(tabs->getFullSize() == tabs->getSize());
        REQUIRE(tabs->getWidgetOffset() == tgui::Vector2f(0, 0));

        tabs->add("TabText");
        REQUIRE(tabs->getSize().x > 4);
        REQUIRE(tabs->getFullSize() == tabs->getSize());
    }

    SECTION("Adding items")
    {
        REQUIRE(tabs->getTabsCount() == 0);
        tabs->add("Item 1");
        REQUIRE(tabs->getTabsCount() == 1);
        tabs->add("Item 2");
        tabs->add("Item 4");
        REQUIRE(tabs->getTabsCount() == 3);

        tabs->insert(2, "Item 3");
        REQUIRE(tabs->getTabsCount() == 4);

        REQUIRE(tabs->getText(0) == "Item 1");
        REQUIRE(tabs->getText(1) == "Item 2");
        REQUIRE(tabs->getText(2) == "Item 3");
        REQUIRE(tabs->getText(3) == "Item 4");
    }

    SECTION("Removing items")
    {
        tabs->add("Item 1");
        tabs->add("Item 2");
        tabs->add("Item 3");
        REQUIRE(tabs->getTabsCount() == 3);

        REQUIRE(!tabs->remove("Item 0"));
        REQUIRE(tabs->getTabsCount() == 3);
        REQUIRE(tabs->remove("Item 2"));
        REQUIRE(tabs->getTabsCount() == 2);

        tabs->add("Item 4");
        REQUIRE(tabs->getTabsCount() == 3);
        REQUIRE(tabs->getText(0) == "Item 1");
        REQUIRE(tabs->getText(1) == "Item 3");
        REQUIRE(tabs->getText(2) == "Item 4");

        REQUIRE(!tabs->remove(3));
        REQUIRE(tabs->getTabsCount() == 3);
        REQUIRE(tabs->remove(0));
        REQUIRE(tabs->getTabsCount() == 2);
        REQUIRE(tabs->getText(0) == "Item 3");
        REQUIRE(tabs->getText(1) == "Item 4");

        tabs->add("Item 5");
        tabs->add("Item 5");
        tabs->add("Item 6");
        REQUIRE(tabs->getTabsCount() == 5);
        tabs->removeAll();
        REQUIRE(tabs->getTabsCount() == 0);
    }

    SECTION("Changing items")
    {
        tabs->add("Item 1");
        tabs->add("Item 2");
        tabs->add("Item 3");

        REQUIRE(!tabs->changeText(3, "Item 00"));
        REQUIRE(tabs->getTabsCount() == 3);
        REQUIRE(tabs->getText(0) == "Item 1");
        REQUIRE(tabs->getText(1) == "Item 2");
        REQUIRE(tabs->getText(2) == "Item 3");
        REQUIRE(tabs->changeText(1, "Item 20"));
        REQUIRE(tabs->getTabsCount() == 3);
        REQUIRE(tabs->getText(0) == "Item 1");
        REQUIRE(tabs->getText(1) == "Item 20");
        REQUIRE(tabs->getText(2) == "Item 3");
    }

    SECTION("Selecting items")
    {
        tabs->add("Item 1", false);
        REQUIRE(tabs->getSelected() == "");
        REQUIRE(tabs->getSelectedIndex() == -1);

        tabs->add("Item 2", true);
        REQUIRE(tabs->getSelected() == "Item 2");
        REQUIRE(tabs->getSelectedIndex() == 1);

        tabs->add("Item 3", false);
        REQUIRE(tabs->getSelected() == "Item 2");
        REQUIRE(tabs->getSelectedIndex() == 1);

        tabs->add("Item 4");
        REQUIRE(tabs->getSelected() == "Item 4");
        REQUIRE(tabs->getSelectedIndex() == 3);

        REQUIRE(!tabs->select("Item 0"));
        REQUIRE(tabs->getSelected() == "");
        REQUIRE(tabs->getSelectedIndex() == -1);

        REQUIRE(tabs->select("Item 1"));
        REQUIRE(tabs->getSelected() == "Item 1");
        REQUIRE(tabs->getSelectedIndex() == 0);

        REQUIRE(!tabs->select(4));
        REQUIRE(tabs->getSelected() == "");
        REQUIRE(tabs->getSelectedIndex() == -1);

        REQUIRE(tabs->select(2));
        REQUIRE(tabs->getSelected() == "Item 3");
        REQUIRE(tabs->getSelectedIndex() == 2);

        tabs->deselect();
        REQUIRE(tabs->getSelected() == "");
        REQUIRE(tabs->getSelectedIndex() == -1);
    }

    SECTION("AutoSize")
    {
        REQUIRE(tabs->getAutoSize());
        tabs->setAutoSize(false);
        REQUIRE(!tabs->getAutoSize());
        tabs->setAutoSize(true);
        REQUIRE(tabs->getAutoSize());
    }

    SECTION("TabVisible")
    {
        REQUIRE(!tabs->getTabVisible(0)); // Tab that doesn't exist can't be visible

        tabs->add("0");
        tabs->add("1");

        REQUIRE(tabs->getTabVisible(0));
        REQUIRE(tabs->getTabVisible(1));
        REQUIRE(!tabs->getTabVisible(2)); // Tab that doesn't exist can't be visible

        tabs->setTabVisible(0, false);
        tabs->setTabVisible(2, true);  // Showing tab that doesn't exist has no effect

        REQUIRE(!tabs->getTabVisible(0));
        REQUIRE(tabs->getTabVisible(1));
        REQUIRE(!tabs->getTabVisible(2)); // Tab that doesn't exist can't be visible
    }

    SECTION("TabEnabled")
    {
        REQUIRE(!tabs->getTabEnabled(0)); // Tab that doesn't exist can't be enabled

        tabs->add("0");
        tabs->add("1");

        REQUIRE(tabs->getTabEnabled(0));
        REQUIRE(tabs->getTabEnabled(1));
        REQUIRE(!tabs->getTabEnabled(2)); // Tab that doesn't exist can't be enabled

        tabs->setTabEnabled(0, false);
        tabs->setTabEnabled(2, true);  // Enabling tab that doesn't exist has no effect

        REQUIRE(!tabs->getTabEnabled(0));
        REQUIRE(tabs->getTabEnabled(1));
        REQUIRE(!tabs->getTabEnabled(2)); // Tab that doesn't exist can't be enabled
    }

    SECTION("TextSize")
    {
        tabs->setTextSize(12);
        REQUIRE(tabs->getTextSize() == 12);
    }

    SECTION("TabHeight")
    {
        tabs->setTabHeight(20);
        REQUIRE(tabs->getSize().y == 20);
    }

    SECTION("MinimumTabWidth")
    {
        REQUIRE(tabs->getMinimumTabWidth() == 0);
        tabs->setMinimumTabWidth(30);
        REQUIRE(tabs->getMinimumTabWidth() == 30);
    }

    SECTION("MaximumTabWidth")
    {
        REQUIRE(tabs->getMaximumTabWidth() == 0);
        tabs->setMaximumTabWidth(60);
        REQUIRE(tabs->getMaximumTabWidth() == 60);
    }

    SECTION("Events / Signals")
    {
        SECTION("Widget")
        {
            testWidgetSignals(tabs);
        }

        SECTION("TabSelected")
        {
            tabs->setSize({300, 20});

            unsigned int tabSelectedCount = 0;
            tabs->onTabSelect(&genericCallback, std::ref(tabSelectedCount));

            tabs->add("1");
            tabs->add("2");
            REQUIRE(tabSelectedCount == 2);

            tabs->add("3", false);
            REQUIRE(tabSelectedCount == 2);

            tabs->select("3");
            REQUIRE(tabSelectedCount == 3);

            tabs->select(0);
            REQUIRE(tabSelectedCount == 4);

            REQUIRE(tabs->getHoveredIndex() == -1);

            const tgui::Vector2f mousePos1{200, 10};
            tabs->mouseMoved(mousePos1);
            REQUIRE(tabs->getHoveredIndex() == 2);
            tabs->leftMousePressed(mousePos1);
            tabs->leftMouseReleased(mousePos1);
            REQUIRE(tabs->getSelected() == "3");
            REQUIRE(tabSelectedCount == 5);

            const tgui::Vector2f mousePos2{199, 10};
            tabs->mouseMoved(mousePos2);
            REQUIRE(tabs->getHoveredIndex() == 1);
            tabs->leftMousePressed(mousePos2);
            tabs->leftMouseReleased(mousePos2);
            REQUIRE(tabs->getSelected() == "2");
            REQUIRE(tabSelectedCount == 6);

            tabs->leftMousePressed(mousePos2);
            tabs->leftMouseReleased(mousePos2);
            REQUIRE(tabSelectedCount == 6);
        }
    }

    testWidgetRenderer(tabs->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = tabs->getRenderer();

        SECTION("colored")
        {
            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(10, 20, 30)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColor", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", "rgb(70, 80, 90)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", "rgb(100, 110, 120)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(130, 140, 150)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorHover", "rgb(140, 150, 160)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBorderColor", "rgb(150, 160, 170)"));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBorderColorHover", "rgb(160, 170, 180)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("DistanceToSide", "2"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", tgui::Color{10, 20, 30}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColor", tgui::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", tgui::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", tgui::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", tgui::Color{130, 140, 150}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColorHover", tgui::Color{140, 150, 160}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBorderColor", tgui::Color{150, 160, 170}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBorderColorHover", tgui::Color{160, 170, 180}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("DistanceToSide", 2));
            }

            SECTION("functions")
            {
                renderer->setBackgroundColor({10, 20, 30});
                renderer->setSelectedBackgroundColor({40, 50, 60});
                renderer->setTextColor({70, 80, 90});
                renderer->setSelectedTextColor({100, 110, 120});
                renderer->setBorderColor({130, 140, 150});
                renderer->setBorderColorHover({140, 150, 160});
                renderer->setSelectedBorderColor({150, 160, 170});
                renderer->setSelectedBorderColorHover({160, 170, 180});
                renderer->setBorders({1, 2, 3, 4});
                renderer->setDistanceToSide(2);
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == tgui::Color(10, 20, 30));
            REQUIRE(renderer->getProperty("SelectedBackgroundColor").getColor() == tgui::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("TextColor").getColor() == tgui::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("SelectedTextColor").getColor() == tgui::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == tgui::Color(130, 140, 150));
            REQUIRE(renderer->getProperty("BorderColorHover").getColor() == tgui::Color(140, 150, 160));
            REQUIRE(renderer->getProperty("SelectedBorderColor").getColor() == tgui::Color(150, 160, 170));
            REQUIRE(renderer->getProperty("SelectedBorderColorHover").getColor() == tgui::Color(160, 170, 180));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("DistanceToSide").getNumber() == 2);
        }

        SECTION("textured")
        {
            tgui::Texture textureNormal{"resources/Black.png", {0, 0, 60, 32}, {16, 0, 28, 32}};
            tgui::Texture textureHover{"resources/Black.png", {0, 0, 60, 32}, {16, 0, 28, 32}};
            tgui::Texture textureSelected{"resources/Black.png", {0, 32, 60, 32}, {16, 0, 28, 32}};
            tgui::Texture textureSelectedHover{"resources/Black.png", {0, 32, 60, 32}, {16, 0, 28, 32}};
            tgui::Texture textureDisabled{"resources/Texture1.png"};

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureTab", tgui::Serializer::serialize(textureNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureTabHover", tgui::Serializer::serialize(textureHover)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureSelectedTab", tgui::Serializer::serialize(textureSelected)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureSelectedTabHover", tgui::Serializer::serialize(textureSelectedHover)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureDisabledTab", tgui::Serializer::serialize(textureDisabled)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureTab", textureNormal));
                REQUIRE_NOTHROW(renderer->setProperty("TextureTabHover", textureHover));
                REQUIRE_NOTHROW(renderer->setProperty("TextureSelectedTab", textureSelected));
                REQUIRE_NOTHROW(renderer->setProperty("TextureSelectedTabHover", textureSelectedHover));
                REQUIRE_NOTHROW(renderer->setProperty("TextureDisabledTab", textureDisabled));
            }

            SECTION("functions")
            {
                renderer->setTextureTab(textureNormal);
                renderer->setTextureTabHover(textureHover);
                renderer->setTextureSelectedTab(textureSelected);
                renderer->setTextureSelectedTabHover(textureSelectedHover);
                renderer->setTextureDisabledTab(textureDisabled);
            }

            REQUIRE(renderer->getProperty("TextureTab").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureTabHover").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureSelectedTab").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureSelectedTabHover").getTexture().getData() != nullptr);
            REQUIRE(renderer->getProperty("TextureDisabledTab").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTextureTab().getData() == textureNormal.getData());
            REQUIRE(renderer->getTextureTabHover().getData() == textureHover.getData());
            REQUIRE(renderer->getTextureSelectedTab().getData() == textureSelected.getData());
            REQUIRE(renderer->getTextureSelectedTabHover().getData() == textureSelectedHover.getData());
            REQUIRE(renderer->getTextureDisabledTab().getData() == textureDisabled.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        tabs->add("1");
        tabs->add("2");
        tabs->add("invisible");
        tabs->add("3");
        tabs->select("2");
        tabs->setTextSize(20);
        tabs->setTabHeight(26);
        tabs->setMaximumTabWidth(100);
        tabs->setTabVisible(2, false);
        tabs->setTabEnabled(3, false);

        testSavingWidget("Tabs", tabs);
    }

    SECTION("Draw")
    {
        TEST_DRAW_INIT(120, 40, tabs)

        tabs->setEnabled(true);
        tabs->setPosition({10, 5});
        tabs->setSize({100, 30});
        tabs->setTextSize(16);
        tabs->add("1");
        tabs->add("2");
        tabs->add("3");

        tgui::TabsRenderer renderer = tgui::RendererData::create();
        renderer.setBackgroundColor(tgui::Color::Green);
        renderer.setSelectedBackgroundColor(tgui::Color::Black);
        renderer.setTextColor(tgui::Color::Red);
        renderer.setSelectedTextColor(tgui::Color::White);
        renderer.setBorderColor({tgui::Color::White});
        renderer.setBorderColorHover({tgui::Color::Red});
        renderer.setSelectedBorderColor({tgui::Color::Yellow});
        renderer.setSelectedBorderColorHover({tgui::Color::Magenta});
        renderer.setBorders({1, 2, 3, 4});
        renderer.setOpacity(0.7f);
        tabs->setRenderer(renderer.getData());

        auto setHoverRenderer = [&](bool textured){
                                        renderer.setBackgroundColorHover(tgui::Color::Cyan);
                                        renderer.setSelectedBackgroundColorHover(tgui::Color::Blue);
                                        renderer.setTextColorHover(tgui::Color::Magenta);
                                        renderer.setSelectedTextColorHover(tgui::Color::Yellow);
                                        if (textured)
                                        {
                                            renderer.setTextureTabHover("resources/Texture3.png");
                                            renderer.setTextureSelectedTabHover("resources/Texture4.png");
                                        }
                                     };

        SECTION("Colored")
        {
            SECTION("NormalState")
            {
                TEST_DRAW("Tabs_Normal_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("Tabs_Normal_HoverSet.png")
                }
            }

            SECTION("HoverState (mouse not over selected tab)")
            {
                tabs->mouseMoved(tabs->getPosition() + (tabs->getSize() / 2.f));

                TEST_DRAW("Tabs_Hover_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("Tabs_Hover_HoverSet.png")
                }
            }

            SECTION("HoverState (mouse on selected tab)")
            {
                tabs->mouseMoved(tabs->getPosition() + (tabs->getSize() * (2.f / 3.f)));

                TEST_DRAW("Tabs_HoverSelected_NormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(false);
                    TEST_DRAW("Tabs_HoverSelected_HoverSet.png")
                }
            }
        }

        SECTION("Textured")
        {
            renderer.setTextureTab("resources/Texture1.png");
            renderer.setTextureSelectedTab("resources/Texture2.png");

            SECTION("NormalState")
            {
                TEST_DRAW("Tabs_Normal_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("Tabs_Normal_TextureHoverSet.png")
                }
            }

            SECTION("HoverState (mouse not over selected tab)")
            {
                tabs->mouseMoved(tabs->getPosition() + (tabs->getSize() / 2.f));

                TEST_DRAW("Tabs_Hover_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("Tabs_Hover_TextureHoverSet.png")
                }
            }

            SECTION("HoverState (mouse on selected tab)")
            {
                tabs->mouseMoved(tabs->getPosition() + (tabs->getSize() * (2.f / 3.f)));

                TEST_DRAW("Tabs_HoverSelected_TextureNormalSet.png")

                SECTION("HoverSet")
                {
                    setHoverRenderer(true);
                    TEST_DRAW("Tabs_HoverSelected_TextureHoverSet.png")
                }
            }
        }
    }
}
