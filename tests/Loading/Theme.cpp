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
#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Widgets/Label.hpp>

// TODO: Reloading theme

TEST_CASE("[Theme]")
{
    SECTION("Loading")
    {
        SECTION("Renderers are shared")
        {
            tgui::Theme theme;
            REQUIRE(theme.getPrimary() == "");
            REQUIRE_NOTHROW(theme.getRenderer("Button"));

            REQUIRE_NOTHROW(theme.load("resources/Black.txt"));
            REQUIRE(theme.getPrimary() == "resources/Black.txt");
            REQUIRE_NOTHROW(theme.getRenderer("EditBox"));
        }

        SECTION("nonexistent file")
        {
            REQUIRE_THROWS_AS(tgui::Theme("nonexistent_file"), tgui::Exception);

            tgui::Theme theme;
            REQUIRE_THROWS_AS(theme.load("nonexistent_file"), tgui::Exception);
        }

        SECTION("nonexistent section")
        {
            // White theme is created on the fly as it uses default values
            tgui::Theme theme1;
            REQUIRE(theme1.getRenderer("nonexistent_section")->propertyValuePairs.empty());

            // Other themes on the other hand will let the ThemeLoader handle it
            tgui::Theme theme2("resources/Black.txt");
            REQUIRE_THROWS_AS(theme2.getRenderer("nonexistent_section"), tgui::Exception);
        }
    }

    SECTION("Adding and removing renderers")
    {
        auto data = std::make_shared<tgui::RendererData>();
        data->propertyValuePairs["textcolor"] = {tgui::Color(255, 0, 255, 200)};

        tgui::Theme theme;
        REQUIRE(theme.getRenderer("label1")->propertyValuePairs.empty());
        REQUIRE(theme.getRenderer("label2")->propertyValuePairs.empty());

        theme.addRenderer("label1", data);
        REQUIRE(theme.getRenderer("label1")->propertyValuePairs.size() == 1);
        REQUIRE(theme.getRenderer("label1")->propertyValuePairs["textcolor"].getColor() == sf::Color(255, 0, 255, 200));
        REQUIRE(theme.getRenderer("label2")->propertyValuePairs.empty());

        REQUIRE(theme.removeRenderer("label1"));
        REQUIRE(theme.getRenderer("label1")->propertyValuePairs.empty());

        REQUIRE(!theme.removeRenderer("nonexistent"));
    }

    SECTION("Renderers are shared")
    {
        tgui::Theme theme{"resources/Black.txt"};

        SECTION("With widgets")
        {
            SECTION("Using getSharedRenderer")
            {
                auto label1 = tgui::Label::create();
                label1->getSharedRenderer()->setTextColor("red");
                REQUIRE(label1->getSharedRenderer()->getTextColor() == sf::Color::Red);

                label1->setRenderer(theme.getRenderer("label"));
                REQUIRE(label1->getSharedRenderer()->getTextColor() != sf::Color::Red);

                label1->getSharedRenderer()->setTextColor("green");
                REQUIRE(label1->getSharedRenderer()->getTextColor() == sf::Color::Green);

                auto label2 = tgui::Label::create();
                label2->getSharedRenderer()->setTextColor("blue");
                REQUIRE(label2->getSharedRenderer()->getTextColor() == sf::Color::Blue);

                label2->setRenderer(theme.getRenderer("label"));
                REQUIRE(label1->getSharedRenderer()->getTextColor() == sf::Color::Green);

                // Changing the renderer of one label affects the look of the other one
                label1->getSharedRenderer()->setTextColor("yellow");
                REQUIRE(label2->getSharedRenderer()->getTextColor() == sf::Color::Yellow);
            }

            SECTION("Using getRenderer")
            {
                auto label1 = tgui::Label::create();
                label1->getRenderer()->setTextColor("red");
                REQUIRE(label1->getRenderer()->getTextColor() == sf::Color::Red);

                label1->setRenderer(theme.getRenderer("label"));
                REQUIRE(label1->getRenderer()->getTextColor() != sf::Color::Red);

                label1->getRenderer()->setTextColor("green");
                REQUIRE(label1->getRenderer()->getTextColor() == sf::Color::Green);

                auto label2 = tgui::Label::create();
                label2->getRenderer()->setTextColor("blue");
                REQUIRE(label2->getRenderer()->getTextColor() == sf::Color::Blue);

                label2->setRenderer(theme.getRenderer("label"));
                REQUIRE(label1->getRenderer()->getTextColor() == sf::Color::Green);

                // Changing the renderer of one label does not affect the other one
                label1->getRenderer()->setTextColor("yellow");
                REQUIRE(label2->getRenderer()->getTextColor() != sf::Color::Yellow);
            }
        }

        SECTION("Without widgets")
        {
            REQUIRE(tgui::LabelRenderer(theme.getRenderer("label")).getTextColor() != sf::Color::Cyan);

            tgui::LabelRenderer(theme.getRenderer("label")).setTextColor(sf::Color::Cyan);
            REQUIRE(tgui::LabelRenderer(theme.getRenderer("label")).getTextColor() == sf::Color::Cyan);
            REQUIRE(theme.getRenderer("label")->propertyValuePairs["textcolor"].getColor() == sf::Color::Cyan);
        }
    }

    SECTION("setThemeLoader")
    {
        struct CustomThemeLoader : public tgui::BaseThemeLoader
        {
            void preload(const std::string& one) override
            {
                REQUIRE(one == "resources/Black.txt");
                preloadCount++;
            }

            const std::map<sf::String, sf::String>& load(const std::string& one, const std::string& two) override
            {
                if (one != "")
                {
                    REQUIRE(one == "resources/Black.txt");
                    REQUIRE(two == "editbox");
                }
                else
                    REQUIRE(two == "button");

                loadCount++;
                return retVal;
            }

            bool canLoad(const std::string&, const std::string&) override
            {
                return true;
            }

            std::map<sf::String, sf::String> retVal;
            unsigned int preloadCount = 0;
            unsigned int loadCount = 0;
        };

        auto loader = std::make_shared<CustomThemeLoader>();
        tgui::Theme::setThemeLoader(loader);
        REQUIRE(tgui::Theme::getThemeLoader() == loader);

        tgui::Theme theme1;
        theme1.getRenderer("BUTTON");

        tgui::Theme theme2("resources/Black.txt");
        theme2.getRenderer("EditBox");

        tgui::Theme::setThemeLoader(std::make_shared<tgui::DefaultThemeLoader>());
        REQUIRE(tgui::Theme::getThemeLoader() != loader);

        REQUIRE(loader->preloadCount == 1);
        REQUIRE(loader->loadCount == 2);
    }
}
