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

#include <TGUI/Global.hpp>

#include <TGUI/FormLoadOptions.hpp>
#include <TGUI/Loading/DataIO.hpp>
#include <TGUI/Loading/Deserializer.hpp>
#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Loading/WidgetFactory.hpp>
#include <TGUI/ObjectConverter.hpp>
#include <TGUI/Texture.hpp>
#include <TGUI/WidgetLoadResources.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/Panel.hpp>

#include <array>
#include <string>

TEST_CASE("[FormThemeLoad]")
{
    tgui::Panel::Ptr panel = tgui::Panel::create();

    SECTION("Renderer section can be empty")
    {
        std::stringstream form(std::string(
            "Renderer.1 {}\n"
            "Button.B1 { Renderer = &1; }\n"));

        REQUIRE_NOTHROW(panel->loadWidgetsFromStream(form));
    }

    SECTION("Reference renderer directly")
    {
        std::stringstream form(std::string(
            "Renderer.1 { TextColor = rgb(10, 20, 30); }\n"
            "Renderer.2 { TextColor = rgb(40, 50, 60); }\n"
            "Theme.MyTheme { Button = &2; }\n"
            "Button.B1 { Renderer = &1; }\n"));

        REQUIRE_NOTHROW(panel->loadWidgetsFromStream(form));
        REQUIRE(panel->get("B1")->getSharedRenderer()->getProperty("TextColor").getColor() == tgui::Color(10, 20, 30));
    }

    SECTION("Renderer = @ThemeName")
    {
        std::string form(
            "Renderer.1 { TextColor = rgb(200, 100, 50); }\n"
            "Theme.ThemeName { Button = &1; }\n"
            "Button.B1 { Renderer = @ThemeName; }\n");

        tgui::ButtonRenderer buttonRenderer;
        buttonRenderer.setTextColor(tgui::Color::Cyan);
        auto runtimeTheme = std::make_shared<tgui::Theme>();
        runtimeTheme->addRenderer(U"Button", buttonRenderer.getData());

        tgui::ButtonRenderer replacementButtonRenderer;
        replacementButtonRenderer.setTextColor(tgui::Color::Yellow);
        auto replacementTheme = std::make_shared<tgui::Theme>();
        replacementTheme->addRenderer(U"Button", replacementButtonRenderer.getData());

        // If no runtime theme is provided then falls back to Theme section in form file
        panel->loadWidgetsFromStream(std::stringstream(form));
        REQUIRE(panel->get("B1")->getSharedRenderer()->getProperty("TextColor").getColor() == tgui::Color(200, 100, 50));

        // If a theme is provided at runtime then it overrides the values in the form file
        tgui::FormLoadOptions loadOptions;
        loadOptions.replaceExistingWidgets = true;
        loadOptions.themesByAlias[U"ThemeName"] = runtimeTheme;
        panel->loadWidgetsFromStream(std::stringstream(form), loadOptions);
        REQUIRE(panel->get("B1")->getSharedRenderer()->getProperty("TextColor").getColor() == tgui::Color::Cyan);

        // The renderer data in the loaded widget is still shared with the runtime theme
        REQUIRE(panel->get("B1")->getSharedRenderer()->getData() == runtimeTheme->getRenderer(U"Button"));

        // Replacing the theme also refreshes the bound widget renderer
        REQUIRE_NOTHROW(runtimeTheme->replace(*replacementTheme));
        REQUIRE(panel->get("B1")->getSharedRenderer()->getProperty("TextColor").getColor() == tgui::Color::Yellow);
    }

    SECTION("Theme references missing renderer id")
    {
        std::stringstream form(std::string(
            "Theme.Main { Button = &missing; }\n"
            "Renderer.1 {}\n"
            "Button.B1 { Renderer = &1; }\n"));

        REQUIRE_THROWS_AS(panel->loadWidgetsFromStream(form), tgui::Exception);
    }

    SECTION("Widget Renderer refers to missing renderer id")
    {
        std::stringstream form(std::string(
            "Renderer.1 {}\n"
            "Button.B1 { Renderer = &missing; }\n"));

        REQUIRE_THROWS_AS(panel->loadWidgetsFromStream(form), tgui::Exception);
    }

    SECTION("Widget Renderer whitespace-only value throws")
    {
        std::stringstream form(std::string(
            "Renderer.1 {}\n"
            "Button.B1 { Renderer =     ; }\n"));

        REQUIRE_THROWS_AS(panel->loadWidgetsFromStream(form), tgui::Exception);
    }

    SECTION("Widget Renderer @ with no alias throws")
    {
        std::stringstream form(std::string(
            "Renderer.1 {}\n"
            "Button.B1 { Renderer = @; }\n"));

        REQUIRE_THROWS_AS(panel->loadWidgetsFromStream(form), tgui::Exception);
    }

    SECTION("Widget Renderer @ alias without Theme or runtime theme throws")
    {
        std::stringstream form(std::string(
            "Renderer.1 {}\n"
            "Button.B1 { Renderer = @NoSuchTheme; }\n"));

        REQUIRE_THROWS_AS(panel->loadWidgetsFromStream(form), tgui::Exception);

        // A fallback is not needed in the form file. It loads with just a runtime theme.
        tgui::FormLoadOptions loadOptions;
        loadOptions.themesByAlias[U"ThemeName"] = std::make_shared<tgui::Theme>();
        REQUIRE_NOTHROW(panel->loadWidgetsFromStream(form));
    }

    SECTION("Widget Renderer @Alias.Section missing in Theme section throws")
    {
        std::stringstream form(std::string(
            "Renderer.1 { TextColor = rgb(1, 2, 3); }\n"
            "Theme.Main { Button = &1; }\n"
            "Button.B1 { Renderer = @Main.Label; }\n"));

        REQUIRE_THROWS_AS(panel->loadWidgetsFromStream(form), tgui::Exception);
    }

    SECTION("Widget Renderer malformed theme binding with missing theme name throws")
    {
        std::stringstream form(std::string(
            "Renderer.1 {}\n"
            "Button.B1 { Renderer = @.Button; }\n"));

        REQUIRE_THROWS_AS(panel->loadWidgetsFromStream(form), tgui::Exception);
    }

    SECTION("Widget Renderer malformed theme binding with missing section name throws")
    {
        std::stringstream form(std::string(
            "Renderer.1 {}\n"
            "Button.B1 { Renderer = @Main.; }\n"));

        REQUIRE_THROWS_AS(panel->loadWidgetsFromStream(form), tgui::Exception);
    }

    SECTION("Widget Renderer value must start with & or @")
    {
        std::stringstream form(std::string(
            "Renderer.1 {}\n"
            "Button.B1 { Renderer = invalid; }\n"));

        REQUIRE_THROWS_AS(panel->loadWidgetsFromStream(form), tgui::Exception);

        // A nested renderer is also valid though
        std::stringstream form2(std::string("Button.B1 { Renderer = {}; }\n"));
        REQUIRE_NOTHROW(panel->loadWidgetsFromStream(form2));
    }

    SECTION("Theme section without alias name throws")
    {
        std::stringstream form(std::string(
            "Renderer.1 {}\n"
            "Theme { Button = &1; }\n"
            "Button.B1 { Renderer = &1; }\n"));

        REQUIRE_THROWS_AS(panel->loadWidgetsFromStream(form), tgui::Exception);
    }

    SECTION("Theme section property must use renderer reference")
    {
        std::stringstream form(std::string(
            "Renderer.1 {}\n"
            "Theme.Main { Button = foo; }\n"
            "Button.B1 { Renderer = &1; }\n"));

        REQUIRE_THROWS_AS(panel->loadWidgetsFromStream(form), tgui::Exception);
    }

    SECTION("Themes still work for nested widgets")
    {
        std::string form(
            "Renderer.1 { TextColor = rgb(200, 100, 50); }\n"
            "Theme.Main { Button = &1; }\n"
            "ChildWindow.Outer {\n"
            "    Position = (0, 0);\n"
            "    Size = (500, 400);\n"
            "    Panel.Inner {\n"
            "        Position = (5, 5);\n"
            "        Size = (200, 150);\n"
            "        Button.B1 { Renderer = @Main; }\n"
            "    }\n"
            "}\n");

        tgui::ButtonRenderer buttonRenderer;
        buttonRenderer.setTextColor(tgui::Color::Green);
        auto runtimeTheme = std::make_shared<tgui::Theme>();
        runtimeTheme->addRenderer(U"Button", buttonRenderer.getData());

        REQUIRE_NOTHROW(panel->loadWidgetsFromStream(std::stringstream(form)));

        const auto outer = std::dynamic_pointer_cast<tgui::ChildWindow>(panel->get("Outer"));
        REQUIRE(outer != nullptr);
        const auto inner = std::dynamic_pointer_cast<tgui::Panel>(outer->get("Inner"));
        REQUIRE(inner != nullptr);
        REQUIRE(inner->get("B1")->getSharedRenderer()->getProperty("TextColor").getColor() == tgui::Color(200, 100, 50));

        tgui::FormLoadOptions loadOptions;
        loadOptions.replaceExistingWidgets = true;
        loadOptions.themesByAlias[U"Main"] = runtimeTheme;
        REQUIRE_NOTHROW(panel->loadWidgetsFromStream(std::stringstream(form), loadOptions));

        REQUIRE(panel->get("B1")->getSharedRenderer()->getProperty("TextColor").getColor() == tgui::Color::Green);
    }

    SECTION("Texture propery of default theme is removed when loading colored theme")
    {
        const tgui::Theme::Ptr oldDefault = tgui::Theme::getDefault();

        tgui::ButtonRenderer defaultButtonRenderer;
        defaultButtonRenderer.setTexture({"resources/Texture1.png"});
        auto defaultTheme = std::make_shared<tgui::Theme>();
        defaultTheme->addRenderer(U"Button", defaultButtonRenderer.getData());
        tgui::Theme::setDefault(defaultTheme);

        std::stringstream form(std::string(
            "Renderer.1 { TextColor = rgb(11, 22, 33); }\n"
            "Theme.Main { Button = &1; }\n"
            "Button.B1 { }\n"
            "Button.B2 { Renderer = @Main; }\n"));

        REQUIRE_NOTHROW(panel->loadWidgetsFromStream(form));

        REQUIRE(panel->get("B1")->getSharedRenderer()->getProperty("Texture").getTexture() == defaultButtonRenderer.getTexture());
        REQUIRE(panel->get("B1")->getSharedRenderer()->getProperty("TextColor").getType() == tgui::ObjectConverter::Type::None);
        REQUIRE(panel->get("B2")->getSharedRenderer()->getProperty("Texture").getTexture().getData() == nullptr);
        REQUIRE(panel->get("B2")->getSharedRenderer()->getProperty("TextColor").getColor() == tgui::Color(11, 22, 33));

        tgui::Theme::setDefault(oldDefault);
    }
}
