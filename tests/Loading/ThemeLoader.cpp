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
#include <TGUI/Loading/ThemeLoader.hpp>

namespace tgui
{
    struct DefaultThemeLoaderTest
    {
        static auto& getPropertiesCache(std::shared_ptr<DefaultThemeLoader> loader) { return loader->m_propertiesCache; }
        static auto& getWidgetTypeCache(std::shared_ptr<DefaultThemeLoader> loader) { return loader->m_widgetTypeCache; }
    };
}

TEST_CASE("[ThemeLoader]") {
    tgui::DefaultThemeLoader::flushCache();

    auto loader = std::make_shared<tgui::DefaultThemeLoader>();
    std::map<std::string, std::string> properties;

    SECTION("load black theme") {
        loader->load("resources/Black.txt", "EditBox", properties);
        REQUIRE(properties.size() > 0);
    }

    SECTION("load nonexistent theme") {
        REQUIRE_THROWS_AS(loader->load("resources/nonexistent.txt", "", properties), tgui::Exception);
    }

    SECTION("load nonexistent class name") {
        REQUIRE_THROWS_AS(loader->load("resources/Black.txt", "NonexistentClassName", properties), tgui::Exception);
    }

    SECTION("load theme with missing opening brace") {
        REQUIRE_THROWS_AS(loader->load("resources/ThemeMissingOpeningBrace.txt", "button1", properties), tgui::Exception);
    }

    SECTION("load theme with missing closing brace") {
        REQUIRE_THROWS_AS(loader->load("resources/ThemeMissingClosingBrace.txt", "button1", properties), tgui::Exception);
    }

    SECTION("load theme with missing semi-colon") {
        REQUIRE_THROWS_AS(loader->load("resources/ThemeMissingSemiColon.txt", "button1", properties), tgui::Exception);
    }

    SECTION("load theme with special cases") {
        REQUIRE_NOTHROW(loader->load("resources/ThemeSpecialCases.txt", "name.WITH.dots", properties));
    }

    SECTION("load theme with comments") {
/*
        REQUIRE_NOTHROW(loader->load("resources/ThemeComments.txt", "Button1", properties));
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["textcolor"] == "rgb(255, 0, 0)");

        REQUIRE_NOTHROW(loader->load("resources/ThemeComments.txt", "CorrectName", properties));
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["textcolor"] == "rgb(0, 255, 0)");

        REQUIRE_NOTHROW(loader->load("resources/ThemeComments.txt", "Button3", properties));
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["textcolor"] == "rgb(0, 0, 255)");
*/
    }

    SECTION("cache") {
        REQUIRE(tgui::DefaultThemeLoaderTest::getPropertiesCache(loader).size() == 0);
        REQUIRE(tgui::DefaultThemeLoaderTest::getWidgetTypeCache(loader).size() == 0);

        loader->load("resources/ThemeSpecialCases.txt", "name.WITH.dots", properties);
        REQUIRE(properties.size() == 2);
        REQUIRE(properties["textcolor"] == "rgb(0, 255, 0)");
        REQUIRE(properties["backgroundcolor"] == "rgb(255, 255, 255)");
        REQUIRE(tgui::DefaultThemeLoaderTest::getPropertiesCache(loader).size() == 1);
        REQUIRE(tgui::DefaultThemeLoaderTest::getWidgetTypeCache(loader).size() == 1);
        
        auto& cache = tgui::DefaultThemeLoaderTest::getPropertiesCache(loader)["resources/ThemeSpecialCases.txt"];
        REQUIRE(cache.size() == 3);
        REQUIRE(cache["button1"].size() == 1);
        REQUIRE(cache["button1"]["textcolor"] == "rgb(255, 0, 0)");
        REQUIRE(cache["name.with.dots"].size() == 2);
        REQUIRE(cache["name.with.dots"]["textcolor"] == "rgb(0, 255, 0)");
        REQUIRE(cache["name.with.dots"]["backgroundcolor"] == "rgb(255, 255, 255)");
        REQUIRE(cache["button"].size() == 1);
        REQUIRE(cache["button"]["textcolor"] == "rgb(0, 0, 255)");

        auto& typeCache = tgui::DefaultThemeLoaderTest::getWidgetTypeCache(loader)["resources/ThemeSpecialCases.txt"];
        REQUIRE(typeCache.size() == 3);
        REQUIRE(typeCache["button1"] == "button");
        REQUIRE(typeCache["name.with.dots"] == "button");
        REQUIRE(typeCache["button"] == "button");

        loader->load("resources/Black.txt", "EditBox", properties);
        REQUIRE(tgui::DefaultThemeLoaderTest::getPropertiesCache(loader).size() == 2);

        tgui::DefaultThemeLoader::flushCache();
        REQUIRE(tgui::DefaultThemeLoaderTest::getPropertiesCache(loader).size() == 0);
    }
}
