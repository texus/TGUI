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

#include <TGUI/Widgets/Panel.hpp>

TEST_CASE("[FormThemeLoad]")
{
    SECTION("Default theme pointer restored after loadWidgetsFromFile and loadWidgetsFromStream")
    {
        auto userTheme = tgui::Theme::create();
        tgui::Theme::setDefault(userTheme);

        const std::string form =
            "Renderer.1 {}\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = &1; }\n";

        tgui::Panel::Ptr panel = tgui::Panel::create();
        std::stringstream stream(form);
        panel->loadWidgetsFromStream(stream);
        REQUIRE(tgui::Theme::getDefault() == userTheme);

        REQUIRE(tgui::writeFile("FormThemeLoadMinimal.txt", std::stringstream(form)));
        panel = tgui::Panel::create();
        panel->loadWidgetsFromFile("FormThemeLoadMinimal.txt");
        REQUIRE(tgui::Theme::getDefault() == userTheme);
    }

    SECTION("File and stream produce same Button colors when default theme is set before load")
    {
        auto theme = std::make_shared<tgui::Theme>();
        auto rd = std::make_shared<tgui::RendererData>();
        rd->propertyValuePairs[U"TextColor"] = tgui::Color{99, 88, 77};
        theme->addRenderer(U"Button", rd);
        tgui::Theme::setDefault(theme);

        const std::string form = "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; }\n";

        tgui::Panel::Ptr pFile = tgui::Panel::create();
        REQUIRE(tgui::writeFile("FormThemeLoadButtonOnly.txt", std::stringstream(form)));
        pFile->loadWidgetsFromFile("FormThemeLoadButtonOnly.txt");

        tgui::Theme::setDefault(theme);
        tgui::Panel::Ptr pStream = tgui::Panel::create();
        std::stringstream stream(form);
        pStream->loadWidgetsFromStream(stream);

        REQUIRE(pFile->get("B1")->getRenderer()->getProperty("TextColor").getColor()
                == pStream->get("B1")->getRenderer()->getProperty("TextColor").getColor());
    }
}
