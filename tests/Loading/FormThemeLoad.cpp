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
#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Panel.hpp>

#include <sstream>
#include <string>

TEST_CASE("[FormApplyDefaultLoad]")
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

    SECTION("applyDefaultTheme controls whether widgets without Renderer use global default theme")
    {
        auto theme = std::make_shared<tgui::Theme>();
        auto rd = std::make_shared<tgui::RendererData>();
        rd->propertyValuePairs[U"TextColor"] = tgui::Color::Yellow;
        theme->addRenderer(U"Button", rd);
        tgui::Theme::setDefault(theme);

        const std::string form = "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; }\n";

        tgui::FormLoadOptions optOn;
        optOn.applyDefaultTheme = true;
        tgui::Panel::Ptr on = tgui::Panel::create();
        std::stringstream sOn(form);
        on->loadWidgetsFromStream(sOn, true, optOn);
        REQUIRE(on->get("B1")->getRenderer()->getProperty("TextColor").getColor() == tgui::Color::Yellow);

        tgui::Theme::setDefault(theme);
        tgui::FormLoadOptions optOff;
        optOff.applyDefaultTheme = false;
        tgui::Panel::Ptr off = tgui::Panel::create();
        std::stringstream sOff(form);
        off->loadWidgetsFromStream(sOff, true, optOff);
        REQUIRE(off->get("B1")->getRenderer()->getProperty("TextColor").getColor() != tgui::Color::Yellow);
    }

    SECTION("Default FormLoadOptions matches two-parameter loadWidgetsFromStream")
    {
        const std::string form =
            "Renderer.1 { TextColor = rgb(12, 34, 56); }\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = &1; }\n";

        tgui::Panel::Ptr pStream = tgui::Panel::create();
        std::stringstream s1(form);
        pStream->loadWidgetsFromStream(s1, true, tgui::FormLoadOptions{});

        tgui::Panel::Ptr pOld = tgui::Panel::create();
        std::stringstream s2(form);
        pOld->loadWidgetsFromStream(s2);

        const auto cStream = pStream->get("B1")->getRenderer()->getProperty("TextColor").getColor();
        const auto cOld = pOld->get("B1")->getRenderer()->getProperty("TextColor").getColor();
        REQUIRE(cStream == cOld);
    }

    SECTION("Default FormLoadOptions matches two-parameter loadWidgetsFromFile")
    {
        const std::string form =
            "Renderer.1 { TextColor = rgb(12, 34, 56); }\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = &1; }\n";

        REQUIRE(tgui::writeFile("FormThemeLoadCompatFile.txt", std::stringstream(form)));

        tgui::Panel::Ptr pNew = tgui::Panel::create();
        pNew->loadWidgetsFromFile("FormThemeLoadCompatFile.txt", true, tgui::FormLoadOptions{});

        tgui::Panel::Ptr pOld = tgui::Panel::create();
        pOld->loadWidgetsFromFile("FormThemeLoadCompatFile.txt");

        REQUIRE(pNew->get("B1")->getRenderer()->getProperty("TextColor").getColor()
                == pOld->get("B1")->getRenderer()->getProperty("TextColor").getColor());
    }

    SECTION("applyDefaultTheme applies to buttons nested inside Panel")
    {
        const tgui::Theme::Ptr oldDefault = tgui::Theme::getDefault();
        auto theme = std::make_shared<tgui::Theme>();
        auto rd = std::make_shared<tgui::RendererData>();
        rd->propertyValuePairs[U"TextColor"] = tgui::Color::Yellow;
        theme->addRenderer(U"Button", rd);
        tgui::Theme::setDefault(theme);

        const std::string form =
            "Panel.P1 {\n"
            "    Position = (0, 0);\n"
            "    Size = (400, 300);\n"
            "    Button.B1 { Position = (10, 10); Size = (50, 30); Text = \"X\"; }\n"
            "}\n";

        tgui::FormLoadOptions optOn;
        optOn.applyDefaultTheme = true;

        tgui::Panel::Ptr root = tgui::Panel::create();
        std::stringstream stream(form);
        root->loadWidgetsFromStream(stream, true, optOn);

        const auto p1 = std::dynamic_pointer_cast<tgui::Panel>(root->get("P1"));
        REQUIRE(p1 != nullptr);
        REQUIRE(p1->get("B1")->getRenderer()->getProperty("TextColor").getColor() == tgui::Color::Yellow);

        tgui::Theme::setDefault(oldDefault);
    }

    SECTION("replaceExisting false matches between loadWidgetsFromStream overloads")
    {
        const std::string form1 =
            "Renderer.1 { TextColor = rgb(10, 20, 30); }\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"A\"; Renderer = &1; }\n";
        const std::string form2 =
            "Renderer.2 { TextColor = rgb(40, 50, 60); }\n"
            "Button.B2 { Position = (60, 0); Size = (50, 30); Text = \"B\"; Renderer = &2; }\n";

        tgui::Panel::Ptr pNew = tgui::Panel::create();
        std::stringstream s1a(form1);
        pNew->loadWidgetsFromStream(s1a, true, tgui::FormLoadOptions{});
        std::stringstream s1b(form2);
        pNew->loadWidgetsFromStream(s1b, false, tgui::FormLoadOptions{});

        tgui::Panel::Ptr pOld = tgui::Panel::create();
        std::stringstream s2a(form1);
        pOld->loadWidgetsFromStream(s2a, true);
        std::stringstream s2b(form2);
        pOld->loadWidgetsFromStream(s2b, false);

        REQUIRE(pNew->getWidgets().size() == pOld->getWidgets().size());
        REQUIRE(pNew->get("B1")->getRenderer()->getProperty("TextColor").getColor()
                == pOld->get("B1")->getRenderer()->getProperty("TextColor").getColor());
        REQUIRE(pNew->get("B2")->getRenderer()->getProperty("TextColor").getColor()
                == pOld->get("B2")->getRenderer()->getProperty("TextColor").getColor());
    }
}
