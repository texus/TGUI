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
#include <TGUI/Widgets/Tabs.hpp>

TEST_CASE("[Tabs]")
{
    tgui::Tabs::Ptr tabs = std::make_shared<tgui::Tabs>();
    tabs->getRenderer()->setFont("resources/DroidSansArmenian.ttf");

    SECTION("Signals")
    {
        REQUIRE_NOTHROW(tabs->connect("TabSelected", [](){}));
        REQUIRE_NOTHROW(tabs->connect("TabSelected", [](sf::String){}));
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

        REQUIRE(tabs->getPosition() == sf::Vector2f(40, 30));
        REQUIRE(tabs->getSize() == sf::Vector2f(4, 24));
        REQUIRE(tabs->getFullSize() == tabs->getSize());
        REQUIRE(tabs->getWidgetOffset() == sf::Vector2f(0, 0));

        tabs->add("TabText");
        REQUIRE(tabs->getSize().x > 4);
        REQUIRE(tabs->getFullSize() == tabs->getSize());
    }

    /// TODO: Test the functions in the Tab class

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
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("DistanceToSide", "2"));
            }
            
            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", sf::Color{10, 20, 30}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedBackgroundColor", sf::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("TextColor", sf::Color{70, 80, 90}));
                REQUIRE_NOTHROW(renderer->setProperty("SelectedTextColor", sf::Color{100, 110, 120}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", sf::Color{130, 140, 150}));
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
                renderer->setBorders({1, 2, 3, 4});
                renderer->setDistanceToSide(2);
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == sf::Color(10, 20, 30));
            REQUIRE(renderer->getProperty("SelectedBackgroundColor").getColor() == sf::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("TextColor").getColor() == sf::Color(70, 80, 90));
            REQUIRE(renderer->getProperty("SelectedTextColor").getColor() == sf::Color(100, 110, 120));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == sf::Color(130, 140, 150));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("DistanceToSide").getNumber() == 2);
        }

        SECTION("textured")
        {
            tgui::Texture textureNormal("resources/Black.png", {0, 0, 60, 32}, {16, 0, 28, 32});
            tgui::Texture textureSelected("resources/Black.png", {0, 32, 60, 32}, {16, 0, 28, 32});

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureTab", tgui::Serializer::serialize(textureNormal)));
                REQUIRE_NOTHROW(renderer->setProperty("TextureSelectedTab", tgui::Serializer::serialize(textureSelected)));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("TextureTab", textureNormal));
                REQUIRE_NOTHROW(renderer->setProperty("TextureSelectedTab", textureSelected));
            }

            SECTION("functions")
            {
                renderer->setTextureTab(textureNormal);
                renderer->setTextureSelectedTab(textureSelected);
            }

            REQUIRE(renderer->getProperty("TextureTab").getTexture().isLoaded());
            REQUIRE(renderer->getProperty("TextureSelectedTab").getTexture().isLoaded());

            REQUIRE(renderer->getTextureTab().getData() == textureNormal.getData());
            REQUIRE(renderer->getTextureSelectedTab().getData() == textureSelected.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        tabs->add("1");
        tabs->add("2");
        tabs->add("3");
        tabs->select("2");
        tabs->setTextSize(20);
        tabs->setTabHeight(26);
        tabs->setMaximumTabWidth(100);

        testSavingWidget("Tabs", tabs);
    }
}
