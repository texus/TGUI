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
#include <TGUI/Loading/ThemeLoader.hpp>

// Ignore warning "C4503: decorated name length exceeded, name was truncated" in Visual Studio
#if defined _MSC_VER
    #pragma warning(disable : 4503)
#endif

namespace tgui
{
    struct DefaultThemeLoaderTest
    {
        static auto& getPropertiesCache(std::shared_ptr<DefaultThemeLoader> loader) { return loader->m_propertiesCache; }
    };
}

TEST_CASE("[ThemeLoader]")
{
    tgui::DefaultThemeLoader::flushCache();

    auto loader = std::make_shared<tgui::DefaultThemeLoader>();

    SECTION("load black theme")
    {
        REQUIRE(loader->load("resources/Black.txt", "EditBox").size() > 0);
    }

    SECTION("load nonexistent theme")
    {
        REQUIRE_THROWS_AS(loader->load("resources/nonexistent.txt", ""), tgui::Exception);
    }

    SECTION("load nonexistent class name")
    {
        REQUIRE_THROWS_AS(loader->load("resources/Black.txt", "NonexistentClassName"), tgui::Exception);
    }

    SECTION("load theme with comments")
    {
        std::map<sf::String, sf::String> properties = loader->load("resources/ThemeComments.txt", "Button1");
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["textcolor"] == "Green");

        properties = loader->load("resources/ThemeComments.txt", "Correct/*Comment*/Name");
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["textcolor"] == "rgb(0, 255, 0 )");

        properties = loader->load("resources/ThemeComments.txt", "CorrectName");
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["textcolor"] == "#ABCDEF");
    }

    SECTION("load theme with nested sections")
    {
        std::map<sf::String, sf::String> properties = loader->load("resources/ThemeNested.txt", "ListBox1");
        REQUIRE(properties.size() == 2);
        REQUIRE(properties["backgroundcolor"] == "White");
        REQUIRE(properties["scrollbar"] == "{\nthumbcolor = Green;\ntrackcolor = Red;\n}");

        properties = loader->load("resources/ThemeNested.txt", "ListBox2");
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["scrollbar"] == "{\nthumbcolor = Blue;\n}");

        properties = loader->load("resources/ThemeNested.txt", "ListBox3");
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["scrollbar"] == "{\ntrackcolor = Yellow;\n}");

        properties = loader->load("resources/ThemeNested.txt", "ComboBox1");
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["listbox"] == "{\nbackgroundcolor = White;\nscrollbar = {\nthumbcolor = Green;\ntrackcolor = Red;\n};\n}");

        properties = loader->load("resources/ThemeNested.txt", "ComboBox2");
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["listbox"] == "{\nScrollbar {\n    thumbcolor = Blue;\n}\n}");

        properties = loader->load("resources/ThemeNested.txt", "ComboBox3");
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["listbox"] == "{\nScrollbar {\n    trackcolor = Yellow;\n}\n}");

        properties = loader->load("resources/ThemeNested.txt", "ComboBox4");
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["listbox"] == "{\nbackgroundcolor = Magenta;\nscrollbar = {\nthumbcolor = Green;\ntrackcolor = Red;\n};\n}");

        properties = loader->load("resources/ThemeNested.txt", "ComboBox5");
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["listbox"] == "{\nbackgroundcolor = Cyan;\n\nScrollbar {\n    trackcolor = Black;\n}\n}");
    }

    SECTION("cache")
    {
        REQUIRE(tgui::DefaultThemeLoaderTest::getPropertiesCache(loader).size() == 0);

        SECTION("with preload")
        {
            loader->preload("resources/ThemeSpecialCases.txt");
            loader->preload("resources/ThemeButton1.txt");

            auto& propertyCache = tgui::DefaultThemeLoaderTest::getPropertiesCache(loader);
            REQUIRE(propertyCache.size() == 2);

            auto& cache1 = propertyCache["resources/ThemeSpecialCases.txt"];
            auto& cache2 = propertyCache["resources/ThemeButton1.txt"];
            REQUIRE(cache1.size() == 4);
            REQUIRE(cache1["button1"].size() == 1);
            REQUIRE(cache1["button1"]["textcolor"] == "rgb(255, 0, 0)");
            REQUIRE(cache1["name.with.dots"].size() == 2);
            REQUIRE(cache1["name.with.dots"]["textcolor"] == "rgb(0, 255, 0)");
            REQUIRE(cache1["name.with.dots"]["backgroundcolor"] == "rgb(255, 255, 255)");
            REQUIRE(cache1["specialchars.{}=:;/*#//\t\\\""].size() == 1);
            REQUIRE(cache1["specialchars.{}=:;/*#//\t\\\""]["textcolor"] == "rgba(,,,)");
            REQUIRE(cache1["label"].size() == 1);
            REQUIRE(cache1["label"]["textcolor"] == "rgb(0, 0, 255)");
            REQUIRE(cache2.size() == 1);
            REQUIRE(cache2["button1"].size() == 1);
            REQUIRE(cache2["button1"]["textcolor"] == "rgb(255, 255, 0)");

            auto properties = loader->load("resources/ThemeSpecialCases.txt", "name.WITH.dots");
            REQUIRE(properties.size() == 2);
            REQUIRE(properties["textcolor"] == "rgb(0, 255, 0)");
            REQUIRE(properties["backgroundcolor"] == "rgb(255, 255, 255)");
            REQUIRE(propertyCache.size() == 2);

            properties = loader->load("resources/ThemeButton1.txt", "Button1");
            REQUIRE(propertyCache.size() == 2);

            properties = loader->load("resources/Black.txt", "EditBox");
            REQUIRE(propertyCache.size() == 3);

            tgui::DefaultThemeLoader::flushCache("resources/ThemeButton1.txt");
            REQUIRE(propertyCache.size() == 2);

            tgui::DefaultThemeLoader::flushCache();
            REQUIRE(propertyCache.size() == 0);
        }

        SECTION("without preload")
        {
            auto properties = loader->load("resources/ThemeSpecialCases.txt", "name.WITH.dots");
            REQUIRE(properties.size() == 2);
            REQUIRE(properties["textcolor"] == "rgb(0, 255, 0)");
            REQUIRE(properties["backgroundcolor"] == "rgb(255, 255, 255)");
            REQUIRE(tgui::DefaultThemeLoaderTest::getPropertiesCache(loader).size() == 1);

            auto& cache = tgui::DefaultThemeLoaderTest::getPropertiesCache(loader)["resources/ThemeSpecialCases.txt"];
            REQUIRE(cache.size() == 4);
            REQUIRE(cache["button1"].size() == 1);
            REQUIRE(cache["button1"]["textcolor"] == "rgb(255, 0, 0)");
            REQUIRE(cache["name.with.dots"].size() == 2);
            REQUIRE(cache["name.with.dots"]["textcolor"] == "rgb(0, 255, 0)");
            REQUIRE(cache["name.with.dots"]["backgroundcolor"] == "rgb(255, 255, 255)");
            REQUIRE(cache["specialchars.{}=:;/*#//\t\\\""].size() == 1);
            REQUIRE(cache["specialchars.{}=:;/*#//\t\\\""]["textcolor"] == "rgba(,,,)");
            REQUIRE(cache["label"].size() == 1);
            REQUIRE(cache["label"]["textcolor"] == "rgb(0, 0, 255)");

            properties = loader->load("resources/Black.txt", "EditBox");
            REQUIRE(tgui::DefaultThemeLoaderTest::getPropertiesCache(loader).size() == 2);
            properties = loader->load("resources/ThemeButton1.txt", "Button1");
            REQUIRE(tgui::DefaultThemeLoaderTest::getPropertiesCache(loader).size() == 3);

            tgui::DefaultThemeLoader::flushCache("resources/Black.txt");
            REQUIRE(tgui::DefaultThemeLoaderTest::getPropertiesCache(loader).size() == 2);

            tgui::DefaultThemeLoader::flushCache();
            REQUIRE(tgui::DefaultThemeLoaderTest::getPropertiesCache(loader).size() == 0);
        }
    }
}
