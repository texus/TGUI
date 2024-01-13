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

// Ignore warning "C4503: decorated name length exceeded, name was truncated" in Visual Studio
#if defined _MSC_VER
    #pragma warning(disable : 4503)
#endif

class CustomThemeLoader : public tgui::DefaultThemeLoader
{
public:
    static auto& getPropertiesCache()
    {
        return m_propertiesCache;
    }
};

TEST_CASE("[ThemeLoader]")
{
    tgui::DefaultThemeLoader::flushCache(); // Clear static data that could be cached from other tests

    auto loader = std::make_shared<CustomThemeLoader>();

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
        std::map<tgui::String, tgui::String> properties = loader->load("resources/ThemeComments.txt", "Button1");
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["TextColor"] == "Green");

        properties = loader->load("resources/ThemeComments.txt", "Correct/*Comment*/Name");
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["TextColor"] == "rgb(0, 255, 0 )");

        properties = loader->load("resources/ThemeComments.txt", "CorrectName");
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["TextColor"] == "#ABCDEF");
    }

    SECTION("load theme with nested sections")
    {
        std::map<tgui::String, tgui::String> properties = loader->load("resources/ThemeNested.txt", "ListBox1");
        REQUIRE(properties.size() == 2);
        REQUIRE(properties["BackgroundColor"] == "White");
        REQUIRE(properties["Scrollbar"] == "{\nThumbColor = Green;\nTrackColor = Red;\n}");

        properties = loader->load("resources/ThemeNested.txt", "ListBox2");
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["Scrollbar"] == "{\nThumbColor = Blue;\n}");

        properties = loader->load("resources/ThemeNested.txt", "ListBox3");
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["Scrollbar"] == "{\nTrackColor = Yellow;\n}");

        properties = loader->load("resources/ThemeNested.txt", "ComboBox1");
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["ListBox"] == "{\nBackgroundColor = White;\nScrollbar = {\nThumbColor = Green;\nTrackColor = Red;\n};\n}");

        properties = loader->load("resources/ThemeNested.txt", "ComboBox2");
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["ListBox"] == "{\nScrollbar {\n    ThumbColor = Blue;\n}\n}");

        properties = loader->load("resources/ThemeNested.txt", "ComboBox3");
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["ListBox"] == "{\nScrollbar {\n    TrackColor = Yellow;\n}\n}");

        properties = loader->load("resources/ThemeNested.txt", "ComboBox4");
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["ListBox"] == "{\nBackgroundColor = Magenta;\nScrollbar = {\nThumbColor = Green;\nTrackColor = Red;\n};\n}");

        properties = loader->load("resources/ThemeNested.txt", "ComboBox5");
        REQUIRE(properties.size() == 1);
        REQUIRE(properties["ListBox"] == "{\nBackgroundColor = Cyan;\n\nScrollbar {\n    TrackColor = Black;\n}\n}");
    }

    SECTION("cache")
    {
        REQUIRE(loader->getPropertiesCache().size() == 0);

        SECTION("with preload")
        {
            loader->preload("resources/ThemeSpecialCases.txt");
            loader->preload("resources/ThemeButton1.txt");

            auto& propertyCache = loader->getPropertiesCache();
            REQUIRE(propertyCache.size() == 2);

            auto& cache1 = propertyCache["resources/ThemeSpecialCases.txt"];
            auto& cache2 = propertyCache["resources/ThemeButton1.txt"];
            REQUIRE(cache1["Button1"].size() == 1);
            REQUIRE(cache1["Button1"]["TextColor"] == "rgb(255, 0, 0)");
            REQUIRE(cache1["Name.With.Dots"].size() == 2);
            REQUIRE(cache1["Name.With.Dots"]["TextColor"] == "rgb(0, 255, 0)");
            REQUIRE(cache1["Name.With.Dots"]["BackgroundColor"] == "rgb(255, 255, 255)");
            REQUIRE(cache1["SpecialChars.{}=:;/*#//\t\\\""].size() == 1);
            REQUIRE(cache1["SpecialChars.{}=:;/*#//\t\\\""]["TextColor"] == "rgba(,,,)");
            REQUIRE(cache1["label"].size() == 1);
            REQUIRE(cache1["label"]["TextColor"] == "rgb(0, 0, 255)");
            REQUIRE(cache2["Button1"].size() == 1);
            REQUIRE(cache2["Button1"]["TextColor"] == "rgb(255, 255, 0)");

            auto properties = loader->load("resources/ThemeSpecialCases.txt", "Name.With.Dots");
            REQUIRE(properties.size() == 2);
            REQUIRE(properties["TextColor"] == "rgb(0, 255, 0)");
            REQUIRE(properties["BackgroundColor"] == "rgb(255, 255, 255)");
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
            auto properties = loader->load("resources/ThemeSpecialCases.txt", "Name.With.Dots");
            REQUIRE(properties.size() == 2);
            REQUIRE(properties["TextColor"] == "rgb(0, 255, 0)");
            REQUIRE(properties["BackgroundColor"] == "rgb(255, 255, 255)");
            REQUIRE(loader->getPropertiesCache().size() == 1);

            auto& cache = loader->getPropertiesCache()["resources/ThemeSpecialCases.txt"];
            REQUIRE(cache["Button1"].size() == 1);
            REQUIRE(cache["Button1"]["TextColor"] == "rgb(255, 0, 0)");
            REQUIRE(cache["Name.With.Dots"].size() == 2);
            REQUIRE(cache["Name.With.Dots"]["TextColor"] == "rgb(0, 255, 0)");
            REQUIRE(cache["Name.With.Dots"]["BackgroundColor"] == "rgb(255, 255, 255)");
            REQUIRE(cache["SpecialChars.{}=:;/*#//\t\\\""].size() == 1);
            REQUIRE(cache["SpecialChars.{}=:;/*#//\t\\\""]["TextColor"] == "rgba(,,,)");
            REQUIRE(cache["label"].size() == 1);
            REQUIRE(cache["label"]["TextColor"] == "rgb(0, 0, 255)");

            properties = loader->load("resources/Black.txt", "EditBox");
            REQUIRE(loader->getPropertiesCache().size() == 2);
            properties = loader->load("resources/ThemeButton1.txt", "Button1");
            REQUIRE(loader->getPropertiesCache().size() == 3);

            tgui::DefaultThemeLoader::flushCache("resources/Black.txt");
            REQUIRE(loader->getPropertiesCache().size() == 2);

            tgui::DefaultThemeLoader::flushCache();
            REQUIRE(loader->getPropertiesCache().size() == 0);
        }
    }
}
