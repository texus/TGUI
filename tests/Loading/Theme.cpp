/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include "../catch.hpp"
#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/EditBox.hpp>

namespace tgui
{
    class ThemeTest
    {
    public:
        static auto& getFilename(tgui::Theme::Ptr theme) { return theme->m_filename; }
        static auto& getResourcePath(tgui::Theme::Ptr theme) { return theme->m_resourcePath; }
        static auto& getWidgets(tgui::Theme::Ptr theme) { return theme->m_widgets; }
        static auto& getWidgetTypes(tgui::Theme::Ptr theme) { return theme->m_widgetTypes; }
        static auto& getWidgetProperties(tgui::Theme::Ptr theme) { return theme->m_widgetProperties; }
    };
}

TEST_CASE("[Theme]") {
    SECTION("constructing") {
        tgui::Theme::Ptr theme;
        REQUIRE(theme == nullptr);

        theme = std::make_shared<tgui::Theme>();
        REQUIRE(theme != nullptr);
        REQUIRE(tgui::ThemeTest::getFilename(theme) == "");
        REQUIRE(tgui::ThemeTest::getResourcePath(theme) == "");

        theme = std::make_shared<tgui::Theme>("resources/Black.conf");
        REQUIRE(theme != nullptr);
        REQUIRE(tgui::ThemeTest::getFilename(theme) == "resources/Black.conf");
        REQUIRE(tgui::ThemeTest::getResourcePath(theme) == "resources/");
        REQUIRE(tgui::ThemeTest::getWidgets(theme).empty());
        REQUIRE(tgui::ThemeTest::getWidgetTypes(theme).empty());
        REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).empty());
    }

    SECTION("load") {
        SECTION("default white theme") {
            tgui::Theme::Ptr theme = std::make_shared<tgui::Theme>("");
            REQUIRE(tgui::ThemeTest::getWidgets(theme).empty());
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme).empty());
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).empty());

            tgui::Button::Ptr button = theme->load("button");
            REQUIRE(button->getTheme() == theme);
            REQUIRE(tgui::ThemeTest::getWidgets(theme).size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme).size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme).begin()->first == "button");
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme).begin()->second == "button");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).empty());

            button = nullptr;
            REQUIRE(tgui::ThemeTest::getWidgets(theme).size() == 0);
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme).size() == 1);
        }

        SECTION("black theme") {
            tgui::Theme::Ptr theme = std::make_shared<tgui::Theme>("resources/Black.txt");
            REQUIRE(tgui::ThemeTest::getWidgets(theme).empty());
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme).empty());
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).empty());

            tgui::Button::Ptr button = theme->load("button");
            REQUIRE(button->getTheme() == theme);
            REQUIRE(tgui::ThemeTest::getWidgets(theme).size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme).size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme).begin()->first == "button");
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme).begin()->second == "button");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).begin()->first == "button");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).begin()->second.size() > 0);

            button = nullptr;
            REQUIRE(tgui::ThemeTest::getWidgets(theme).size() == 0);
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme).size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).size() == 1);
        }
        
        SECTION("theme is shared") {
            tgui::Theme::Ptr theme = std::make_shared<tgui::Theme>();
            tgui::Button::Ptr button = theme->load("Button");
            tgui::EditBox::Ptr editBox = theme->load("EditBox");
            REQUIRE(button->getTheme() == theme);
            REQUIRE(button->getTheme() == editBox->getTheme());
        }

        SECTION("nonexistent file") {
            tgui::Theme::Ptr theme = std::make_shared<tgui::Theme>("nonexistent_file");
            REQUIRE_THROWS_AS(theme->load("button"), tgui::Exception);
            REQUIRE(tgui::ThemeTest::getWidgets(theme).empty());
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme).empty());
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).begin()->first == "button");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).begin()->second.size() == 0);
        }

        SECTION("nonexistent section") {
            tgui::Theme::Ptr theme = std::make_shared<tgui::Theme>("resources/Black.txt");
            REQUIRE_THROWS_AS(theme->load("nonexistent_section"), tgui::Exception);
            REQUIRE(tgui::ThemeTest::getWidgets(theme).empty());
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme).empty());
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).begin()->first == "nonexistent_section");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).begin()->second.size() == 0);
        }

        SECTION("wrong widget type") {
            auto theme = std::make_shared<tgui::Theme>("resources/Black.txt");
            REQUIRE_THROWS_AS(tgui::EditBox::Ptr widget = theme->load("Button"), tgui::Exception);
        }
    }

    SECTION("reload") {
        SECTION("changing filename") {
            tgui::Theme::Ptr theme1 = std::make_shared<tgui::Theme>("resources/ThemeButton1.txt");
            tgui::Theme::Ptr theme2 = std::make_shared<tgui::Theme>("resources/ThemeButton2.txt");

            tgui::Button::Ptr button1 = theme1->load("button1");
            REQUIRE(button1->getTheme() == theme1);
            REQUIRE(tgui::ThemeTest::getWidgets(theme1).size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme1).size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme1).begin()->first == "button1");
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme1).begin()->second == "button");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme1).size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme1).begin()->first == "button1");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme1).begin()->second.size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme1).begin()->second.begin()->first == "textcolor");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme1).begin()->second.begin()->second == "rgb(255, 255, 0)");

            tgui::Button::Ptr button2 = theme2->load("button2");
            REQUIRE(button2->getTheme() == theme2);
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme2).begin()->first == "button2");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme2).begin()->second.begin()->second == "rgb(0, 255, 255)");
        
            theme1->reload("resources/ThemeMultipleButtons.txt");
            REQUIRE(button1->getTheme() == theme1);
            REQUIRE(tgui::ThemeTest::getWidgets(theme1).size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme1).size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme1).begin()->first == "button1");
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme1).begin()->second == "button");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme1).size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme1).begin()->first == "button1");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme1).begin()->second.size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme1).begin()->second.begin()->first == "textcolor");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme1).begin()->second.begin()->second == "rgb(255, 0, 0)");

            theme2->reload("resources/ThemeMultipleButtons.txt");
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme2).begin()->first == "button2");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme2).begin()->second.begin()->second == "rgb(0, 255, 0)");

            SECTION("nonexistent theme") {
                REQUIRE_THROWS_AS(theme2->reload("nonexistent_file"), tgui::Exception);
            }

            SECTION("nonexistent section") {
                REQUIRE_THROWS_AS(theme2->reload("resources/ThemeButton1.txt"), tgui::Exception);
            }
        }

        SECTION("changing class name") {
            tgui::Theme::Ptr theme = std::make_shared<tgui::Theme>("resources/ThemeMultipleButtons.txt");
            tgui::Button::Ptr button1 = theme->load("button1");
            tgui::Button::Ptr button2 = theme->load("button2");
            tgui::Button::Ptr button3 = theme->load("button3");

            REQUIRE(tgui::ThemeTest::getWidgets(theme).size() == 3);
            REQUIRE(tgui::ThemeTest::getWidgets(theme)[button1.get()] == "button1");
            REQUIRE(tgui::ThemeTest::getWidgets(theme)[button2.get()] == "button2");
            REQUIRE(tgui::ThemeTest::getWidgets(theme)[button3.get()] == "button3");
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme).size() == 3);
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme)["button1"] == "button");
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme)["button2"] == "button");
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme)["button3"] == "button");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).size() == 3);
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button1"].size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button2"].size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button3"].size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button1"]["textcolor"] == "rgb(255, 0, 0)");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button2"]["textcolor"] == "rgb(0, 255, 0)");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button3"]["textcolor"] == "rgb(0, 0, 255)");
            REQUIRE(button1->getRenderer()->getProperty("TextColor").getColor() == sf::Color(255, 0, 0));
            REQUIRE(button2->getRenderer()->getProperty("TextColor").getColor() == sf::Color(0, 255, 0));
            REQUIRE(button3->getRenderer()->getProperty("TextColor").getColor() == sf::Color(0, 0, 255));

            theme->reload("button1", "button2");
            REQUIRE(tgui::ThemeTest::getWidgets(theme).size() == 3);
            REQUIRE(tgui::ThemeTest::getWidgets(theme)[button1.get()] == "button2");
            REQUIRE(tgui::ThemeTest::getWidgets(theme)[button2.get()] == "button2");
            REQUIRE(tgui::ThemeTest::getWidgets(theme)[button3.get()] == "button3");
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme)["button1"] == "button");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button1"]["textcolor"] == "rgb(255, 0, 0)");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button2"]["textcolor"] == "rgb(0, 255, 0)");
            REQUIRE(button1->getRenderer()->getProperty("TextColor").getColor() == sf::Color(0, 255, 0));
            REQUIRE(button2->getRenderer()->getProperty("TextColor").getColor() == sf::Color(0, 255, 0));
            REQUIRE(button3->getRenderer()->getProperty("TextColor").getColor() == sf::Color(0, 0, 255));

            theme->reload("button2", "button1");
            REQUIRE(tgui::ThemeTest::getWidgets(theme).size() == 3);
            REQUIRE(tgui::ThemeTest::getWidgets(theme)[button1.get()] == "button1");
            REQUIRE(tgui::ThemeTest::getWidgets(theme)[button2.get()] == "button1");
            REQUIRE(tgui::ThemeTest::getWidgets(theme)[button3.get()] == "button3");
            REQUIRE(button1->getRenderer()->getProperty("TextColor").getColor() == sf::Color(255, 0, 0));
            REQUIRE(button2->getRenderer()->getProperty("TextColor").getColor() == sf::Color(255, 0, 0));
            REQUIRE(button3->getRenderer()->getProperty("TextColor").getColor() == sf::Color(0, 0, 255));

            button3 = nullptr;
            REQUIRE(tgui::ThemeTest::getWidgets(theme).size() == 2);

            button2 = nullptr;
            button1 = nullptr;
            REQUIRE(tgui::ThemeTest::getWidgets(theme).size() == 0);
        }

        SECTION("changing single widget") {
            tgui::Theme::Ptr theme = std::make_shared<tgui::Theme>("resources/ThemeMultipleButtons.txt");
            tgui::Button::Ptr button1 = theme->load("button1");
            tgui::Button::Ptr button1b = theme->load("button1");
            tgui::Button::Ptr button2 = theme->load("button2");

            REQUIRE(tgui::ThemeTest::getWidgets(theme).size() == 3);
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme).size() == 2);
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).size() == 2);
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button1"].size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button2"].size() == 1);
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button1"]["textcolor"] == "rgb(255, 0, 0)");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button2"]["textcolor"] == "rgb(0, 255, 0)");
            REQUIRE(button1->getRenderer()->getProperty("TextColor").getColor() == sf::Color(255, 0, 0));
            REQUIRE(button1b->getRenderer()->getProperty("TextColor").getColor() == sf::Color(255, 0, 0));
            REQUIRE(button2->getRenderer()->getProperty("TextColor").getColor() == sf::Color(0, 255, 0));

            theme->reload(button1, "button2");
            REQUIRE(tgui::ThemeTest::getWidgets(theme).size() == 3);
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme).size() == 2);
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).size() == 2);
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button1"]["textcolor"] == "rgb(255, 0, 0)");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button2"]["textcolor"] == "rgb(0, 255, 0)");
            REQUIRE(button1->getRenderer()->getProperty("TextColor").getColor() == sf::Color(0, 255, 0));
            REQUIRE(button1b->getRenderer()->getProperty("TextColor").getColor() == sf::Color(255, 0, 0));
            REQUIRE(button2->getRenderer()->getProperty("TextColor").getColor() == sf::Color(0, 255, 0));

            theme->reload(button1, "button3");
            REQUIRE(tgui::ThemeTest::getWidgets(theme).size() == 3);
            REQUIRE(tgui::ThemeTest::getWidgetTypes(theme).size() == 3);
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).size() == 3);
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button1"]["textcolor"] == "rgb(255, 0, 0)");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button2"]["textcolor"] == "rgb(0, 255, 0)");
            REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button3"]["textcolor"] == "rgb(0, 0, 255)");
            REQUIRE(button1->getRenderer()->getProperty("TextColor").getColor() == sf::Color(0, 0, 255));
            REQUIRE(button1b->getRenderer()->getProperty("TextColor").getColor() == sf::Color(255, 0, 0));
            REQUIRE(button2->getRenderer()->getProperty("TextColor").getColor() == sf::Color(0, 255, 0));
        }
    }

    SECTION("setProperty") {
        tgui::Theme::Ptr theme = std::make_shared<tgui::Theme>("resources/ThemeMultipleButtons.txt");
        tgui::Button::Ptr button1 = theme->load("button1");
        tgui::Button::Ptr button2 = theme->load("button2");
        tgui::Button::Ptr button3 = theme->load("button3");

        REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).size() == 3);
        REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button1"]["textcolor"] == "rgb(255, 0, 0)");
        REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button2"]["textcolor"] == "rgb(0, 255, 0)");
        REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button3"]["textcolor"] == "rgb(0, 0, 255)");
        REQUIRE(button1->getRenderer()->getProperty("TextColor").getColor() == sf::Color(255, 0, 0));
        REQUIRE(button2->getRenderer()->getProperty("TextColor").getColor() == sf::Color(0, 255, 0));
        REQUIRE(button3->getRenderer()->getProperty("TextColor").getColor() == sf::Color(0, 0, 255));

        REQUIRE(theme->getProperty("button1", "TextColor") == "rgb(255, 0, 0)");
        REQUIRE(theme->getProperty("Button2", "TextColor") == "rgb(0, 255, 0)");
        REQUIRE(theme->getProperty("BUTTON3", "TextColor") == "rgb(0, 0, 255)");
        REQUIRE(theme->getProperty("Button3", "Something") == "");
        REQUIRE(theme->getProperty("button4", "TextColor") == "");

        REQUIRE(theme->getPropertyValuePairs("button1").size() == 1);
        REQUIRE(theme->getPropertyValuePairs("button1")["textcolor"] == "rgb(255, 0, 0)");
        REQUIRE(theme->getPropertyValuePairs("button2").size() == 1);
        REQUIRE(theme->getPropertyValuePairs("button2")["textcolor"] == "rgb(0, 255, 0)");
        REQUIRE(theme->getPropertyValuePairs("button3").size() == 1);
        REQUIRE(theme->getPropertyValuePairs("button3")["textcolor"] == "rgb(0, 0, 255)");
        REQUIRE(theme->getPropertyValuePairs("button4").size() == 0);

        theme->setProperty("button1", "textcolor", "rgb(0, 255, 0)");
        REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).size() == 3);
        REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button1"]["textcolor"] == "rgb(0, 255, 0)");
        REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button2"]["textcolor"] == "rgb(0, 255, 0)");
        REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button3"]["textcolor"] == "rgb(0, 0, 255)");
        REQUIRE(button1->getRenderer()->getProperty("TextColor").getColor() == sf::Color(0, 255, 0));
        REQUIRE(button2->getRenderer()->getProperty("TextColor").getColor() == sf::Color(0, 255, 0));
        REQUIRE(button3->getRenderer()->getProperty("TextColor").getColor() == sf::Color(0, 0, 255));

        theme->setProperty("button2", "textcolor", sf::Color(0, 0, 255));        
        REQUIRE(tgui::ThemeTest::getWidgetProperties(theme).size() == 3);
        REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button1"]["textcolor"] == "rgb(0, 255, 0)");
        REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button2"]["textcolor"] == "rgb(0, 0, 255)");
        REQUIRE(tgui::ThemeTest::getWidgetProperties(theme)["button3"]["textcolor"] == "rgb(0, 0, 255)");
        REQUIRE(button1->getRenderer()->getProperty("TextColor").getColor() == sf::Color(0, 255, 0));
        REQUIRE(button2->getRenderer()->getProperty("TextColor").getColor() == sf::Color(0, 0, 255));
        REQUIRE(button3->getRenderer()->getProperty("TextColor").getColor() == sf::Color(0, 0, 255));
    }

    SECTION("clone") {
        tgui::Theme::Ptr theme1 = std::make_shared<tgui::Theme>("resources/Black.txt");
        tgui::Button::Ptr button1 = theme1->load("Button");
        theme1->setProperty("Button", "TextColorNormal", sf::Color(255, 0, 0)); // TODO: Load entire file when theme is created so that it works when this line is moved before the theme1->load line

        tgui::Theme::Ptr theme2 = theme1->clone();
        tgui::Button::Ptr button2 = theme2->load("Button");
        REQUIRE(button1->getRenderer()->getProperty("TextColorNormal").getColor() == sf::Color(255, 0, 0));
        REQUIRE(button2->getRenderer()->getProperty("TextColorNormal").getColor() == sf::Color(255, 0, 0));

        theme1->setProperty("Button", "TextColorNormal", sf::Color(0, 255, 0));
        REQUIRE(button1->getRenderer()->getProperty("TextColorNormal").getColor() == sf::Color(0, 255, 0));
        REQUIRE(button2->getRenderer()->getProperty("TextColorNormal").getColor() == sf::Color(255, 0, 0));

        theme2->setProperty("Button", "TextColorNormal", sf::Color(0, 0, 255));
        REQUIRE(button1->getRenderer()->getProperty("TextColorNormal").getColor() == sf::Color(0, 255, 0));
        REQUIRE(button2->getRenderer()->getProperty("TextColorNormal").getColor() == sf::Color(0, 0, 255));
    }

    SECTION("setConstructFunction") {
        tgui::Theme::Ptr theme = std::make_shared<tgui::Theme>();

        REQUIRE_THROWS_AS(theme->load("CustomWidget"), tgui::Exception);

        unsigned int count = 0;
        tgui::Theme::setConstructFunction("CustomWidget", [&](){ count++; return std::make_shared<tgui::EditBox>(); });
        theme->load("CustomWidget");
        REQUIRE(count == 1);
    }

    SECTION("setThemeLoader") {
        struct CustomThemeLoader : public tgui::BaseThemeLoader {
            std::string load(const std::string& one, const std::string& two, std::map<std::string, std::string>&) override {
                REQUIRE(one == "resources/Black.txt");
                REQUIRE(two == "editbox");
                return two;
            }
        };

        auto loader = std::make_shared<CustomThemeLoader>();
        tgui::Theme::setThemeLoader(loader);

        tgui::Theme::Ptr theme = std::make_shared<tgui::Theme>();
        theme->load("BUTTON");

        theme = std::make_shared<tgui::Theme>("resources/Black.txt");
        theme->load("EditBox");

        tgui::Theme::setThemeLoader(std::make_shared<tgui::DefaultThemeLoader>());
    }
}
