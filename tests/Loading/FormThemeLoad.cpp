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

namespace
{
    /// Calls Widget::load(WidgetLoadResources) explicitly (ButtonBase::load(map) hides the base overload name).
    class TestButton : public tgui::Button
    {
    public:
        using Ptr = std::shared_ptr<TestButton>;

        static Ptr create()
        {
            return std::make_shared<TestButton>();
        }

        void loadExposed(const std::unique_ptr<tgui::DataIO::Node>& node, const tgui::WidgetLoadResources& resources)
        {
            Widget::load(node, resources);
        }
    };

    /// Custom Button that calls Widget::loadUsingResources with a WidgetLoadResources built only from the renderer map,
    /// so it never sees FormLoadOptions runtime themes or Theme.* fallbacks. Matches legacy two-parameter form load when
    /// those features are unused; other widgets on the same form still receive full options.
    class ThemeBlindButton : public tgui::Button
    {
    public:
        ThemeBlindButton() :
            tgui::Button("ThemeBlindButton")
        {
        }

        static std::shared_ptr<ThemeBlindButton> create()
        {
            return std::make_shared<ThemeBlindButton>();
        }

    protected:
        void load(const std::unique_ptr<tgui::DataIO::Node>& node, const LoadingRenderersMap& renderers) override
        {
            tgui::WidgetLoadResources wlr(renderers);
            Widget::loadUsingResources(node, wlr);

            if (node->propertyValuePairs[U"Text"])
                setText(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::String, node->propertyValuePairs[U"Text"]->value)
                            .getString());
        }
    };

    /// Custom Button with no load override: uses ButtonBase::load and the normal theme-aware path (same as stock Button).
    class ThemeAwareCustomButton : public tgui::Button
    {
    public:
        ThemeAwareCustomButton() :
            tgui::Button("ThemeAwareCustomButton")
        {
        }

        static std::shared_ptr<ThemeAwareCustomButton> create()
        {
            return std::make_shared<ThemeAwareCustomButton>();
        }
    };

    void registerFormThemeLoadCustomWidgetTypes()
    {
        static const int once = []
        {
            tgui::WidgetFactory::setConstructFunction(U"ThemeBlindButton", [] { return tgui::Widget::Ptr(ThemeBlindButton::create()); });
            tgui::WidgetFactory::setConstructFunction(U"ThemeAwareCustomButton",
                                                      [] { return tgui::Widget::Ptr(ThemeAwareCustomButton::create()); });
            return 0;
        }();
        (void)once;
    }

    void requireFormLoadThrows(const std::string& form)
    {
        tgui::Panel::Ptr panel = tgui::Panel::create();
        std::stringstream stream(form);
        REQUIRE_THROWS_AS(panel->loadWidgetsFromStream(stream), tgui::Exception);
    }

    /// Property names from TGUI_RENDERER_PROPERTY_* macros in src/Renderers/ButtonRenderer.cpp (keep in sync).
    constexpr std::array<const char*, 44> buttonRendererPropertyNames = {
        "Borders",
        "TextColor",
        "TextColorDown",
        "TextColorHover",
        "TextColorDownHover",
        "TextColorDisabled",
        "TextColorDownDisabled",
        "TextColorFocused",
        "TextColorDownFocused",
        "BackgroundColor",
        "BackgroundColorDown",
        "BackgroundColorHover",
        "BackgroundColorDownHover",
        "BackgroundColorDisabled",
        "BackgroundColorDownDisabled",
        "BackgroundColorFocused",
        "BackgroundColorDownFocused",
        "BorderColor",
        "BorderColorDown",
        "BorderColorHover",
        "BorderColorDownHover",
        "BorderColorDisabled",
        "BorderColorDownDisabled",
        "BorderColorFocused",
        "BorderColorDownFocused",
        "Texture",
        "TextureDown",
        "TextureHover",
        "TextureDownHover",
        "TextureDisabled",
        "TextureDownDisabled",
        "TextureFocused",
        "TextureDownFocused",
        "TextStyle",
        "TextStyleDown",
        "TextStyleHover",
        "TextStyleDownHover",
        "TextStyleDisabled",
        "TextStyleDownDisabled",
        "TextStyleFocused",
        "TextStyleDownFocused",
        "TextOutlineThickness",
        "TextOutlineColor",
        "RoundedBorderRadius",
    };
} // namespace

TEST_CASE("[FormThemeLoad]")
{
    SECTION("Theme.MyTheme block with Renderer = &1 still loads")
    {
        const std::string form =
            "Renderer.1 { TextColor = rgb(10, 20, 30); }\n"
            "Theme.MyTheme { Button = &1; }\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = &1; }\n";

        tgui::Panel::Ptr panel = tgui::Panel::create();
        std::stringstream stream(form);
        REQUIRE_NOTHROW(panel->loadWidgetsFromStream(stream));
        REQUIRE(panel->get("B1")->getRenderer()->getProperty("TextColor").getColor() == tgui::Color(10, 20, 30));
    }

    SECTION("Theme section without widget construct error is skipped")
    {
        const std::string form =
            "Renderer.1 {}\n"
            "Theme.X { Button = &1; }\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = &1; }\n";

        tgui::Panel::Ptr panel = tgui::Panel::create();
        std::stringstream stream(form);
        REQUIRE_NOTHROW(panel->loadWidgetsFromStream(stream));
    }

    SECTION("Renderer = @Main uses Theme.Main fallback when no runtime theme")
    {
        const std::string form =
            "Renderer.1 { TextColor = rgb(200, 100, 50); }\n"
            "Theme.Main { Button = &1; }\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = @Main; }\n";

        tgui::Panel::Ptr panel = tgui::Panel::create();
        std::stringstream stream(form);
        panel->loadWidgetsFromStream(stream);
        REQUIRE(panel->get("B1")->getRenderer()->getProperty("TextColor").getColor() == tgui::Color(200, 100, 50));
    }

    SECTION("Runtime theme map overrides Theme section fallback")
    {
        const std::string form =
            "Renderer.1 { TextColor = rgb(200, 100, 50); }\n"
            "Theme.Main { Button = &1; }\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = @Main; }\n";

        auto runtime = std::make_shared<tgui::Theme>();
        auto rd = std::make_shared<tgui::RendererData>();
        rd->propertyValuePairs[U"TextColor"] = tgui::Color::Cyan;
        runtime->addRenderer(U"Button", rd);

        tgui::FormLoadOptions opt;
        opt.themesByAlias[U"Main"] = runtime;

        tgui::Panel::Ptr panel = tgui::Panel::create();
        std::stringstream stream(form);
        panel->loadWidgetsFromStream(stream, true, opt);
        REQUIRE(panel->get("B1")->getRenderer()->getProperty("TextColor").getColor() == tgui::Color::Cyan);
    }

    SECTION("Runtime theme renderer data is shared with loaded widget")
    {
        const std::string form =
            "Renderer.1 { TextColor = rgb(1, 2, 3); }\n"
            "Theme.Main { Button = &1; }\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = @Main; }\n";

        auto runtime = std::make_shared<tgui::Theme>();
        auto rd = std::make_shared<tgui::RendererData>();
        rd->propertyValuePairs[U"TextColor"] = tgui::Color::Magenta;
        runtime->addRenderer(U"Button", rd);

        tgui::FormLoadOptions opt;
        opt.themesByAlias[U"Main"] = runtime;

        tgui::Panel::Ptr panel = tgui::Panel::create();
        std::stringstream stream(form);
        panel->loadWidgetsFromStream(stream, true, opt);

        REQUIRE(panel->get("B1")->getSharedRenderer()->getData() == runtime->getRenderer(U"Button"));
    }

    SECTION("Updating runtime theme after form load refreshes bound widget renderer")
    {
        const std::string form =
            "Renderer.1 { TextColor = rgb(1, 2, 3); }\n"
            "Theme.Main { Button = &1; }\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = @Main; }\n";

        auto runtime = std::make_shared<tgui::Theme>();
        auto rdBefore = std::make_shared<tgui::RendererData>();
        rdBefore->propertyValuePairs[U"TextColor"] = tgui::Color(11, 22, 33);
        runtime->addRenderer(U"Button", rdBefore);

        tgui::FormLoadOptions opt;
        opt.themesByAlias[U"Main"] = runtime;

        tgui::Panel::Ptr panel = tgui::Panel::create();
        std::stringstream stream(form);
        panel->loadWidgetsFromStream(stream, true, opt);

        // Theme renderer data is shared (setRenderer marks it shared). getRenderer() would clone and unsubscribe
        // from the theme, so Theme::replace would no longer refresh this widget — use getSharedRenderer() to read.
        REQUIRE(panel->get("B1")->getSharedRenderer()->getProperty("TextColor").getColor() == tgui::Color(11, 22, 33));

        tgui::Theme replacement;
        auto rdAfter = std::make_shared<tgui::RendererData>();
        rdAfter->propertyValuePairs[U"TextColor"] = tgui::Color(44, 55, 66);
        replacement.addRenderer(U"Button", rdAfter);

        REQUIRE_NOTHROW(runtime->replace(replacement));

        REQUIRE(panel->get("B1")->getSharedRenderer()->getProperty("TextColor").getColor() == tgui::Color(44, 55, 66));
    }

    SECTION("Theme references missing renderer id")
    {
        const std::string form =
            "Theme.Main { Button = &missing; }\n"
            "Renderer.1 {}\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = &1; }\n";

        requireFormLoadThrows(form);
    }

    SECTION("Widget Renderer refers to missing renderer id")
    {
        const std::string form =
            "Renderer.1 {}\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = &missing; }\n";

        requireFormLoadThrows(form);
    }

    SECTION("Widget Renderer whitespace-only value throws")
    {
        const std::string form =
            "Renderer.1 {}\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer =     ; }\n";

        requireFormLoadThrows(form);
    }

    SECTION("Widget Renderer @ with no alias throws")
    {
        const std::string form =
            "Renderer.1 {}\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = @; }\n";

        requireFormLoadThrows(form);
    }

    SECTION("Widget Renderer @ alias without Theme or runtime theme throws")
    {
        const std::string form =
            "Renderer.1 {}\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = @NoSuchTheme; }\n";

        requireFormLoadThrows(form);
    }

    SECTION("Widget Renderer @Alias.Section missing in Theme section throws")
    {
        const std::string form =
            "Renderer.1 { TextColor = rgb(1, 2, 3); }\n"
            "Theme.Main { Button = &1; }\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = @Main.Label; }\n";

        requireFormLoadThrows(form);
    }

    SECTION("Widget Renderer malformed theme binding throws")
    {
        requireFormLoadThrows(
            "Renderer.1 {}\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = @.Button; }\n");

        requireFormLoadThrows(
            "Renderer.1 {}\n"
            "Theme.Main { Button = &1; }\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = @Main.; }\n");
    }

    SECTION("Widget Renderer value must start with & or @")
    {
        const std::string form =
            "Renderer.1 {}\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = invalid; }\n";

        requireFormLoadThrows(form);
    }

    SECTION("Theme section without alias name throws")
    {
        const std::string form =
            "Renderer.1 {}\n"
            "Theme { Button = &1; }\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = &1; }\n";

        requireFormLoadThrows(form);
    }

    SECTION("Theme section property must use renderer reference")
    {
        const std::string form =
            "Renderer.1 {}\n"
            "Theme.Main { Button = foo; }\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = &1; }\n";

        requireFormLoadThrows(form);
    }

    SECTION("Panel forwards Theme.* fallbacks to nested buttons (two-parameter load)")
    {
        const std::string form =
            "Renderer.1 { TextColor = rgb(55, 66, 77); }\n"
            "Theme.Main { Button = &1; }\n"
            "Panel.P1 {\n"
            "    Position = (0, 0);\n"
            "    Size = (400, 300);\n"
            "    Button.B1 { Position = (10, 10); Size = (50, 30); Text = \"X\"; Renderer = @Main; }\n"
            "}\n";

        tgui::Panel::Ptr root = tgui::Panel::create();
        std::stringstream stream(form);
        root->loadWidgetsFromStream(stream);

        const auto p1 = std::dynamic_pointer_cast<tgui::Panel>(root->get("P1"));
        REQUIRE(p1 != nullptr);
        REQUIRE(p1->get("B1")->getRenderer()->getProperty("TextColor").getColor() == tgui::Color(55, 66, 77));
    }

    SECTION("Panel forwards themesByAlias to nested buttons")
    {
        const std::string form =
            "Renderer.1 { TextColor = rgb(200, 100, 50); }\n"
            "Theme.Main { Button = &1; }\n"
            "Panel.P1 {\n"
            "    Position = (0, 0);\n"
            "    Size = (400, 300);\n"
            "    Button.B1 { Position = (10, 10); Size = (50, 30); Text = \"X\"; Renderer = @Main; }\n"
            "}\n";

        auto runtime = std::make_shared<tgui::Theme>();
        auto rd = std::make_shared<tgui::RendererData>();
        rd->propertyValuePairs[U"TextColor"] = tgui::Color::Cyan;
        runtime->addRenderer(U"Button", rd);

        tgui::FormLoadOptions opt;
        opt.themesByAlias[U"Main"] = runtime;

        tgui::Panel::Ptr root = tgui::Panel::create();
        std::stringstream stream(form);
        root->loadWidgetsFromStream(stream, true, opt);

        const auto p1 = std::dynamic_pointer_cast<tgui::Panel>(root->get("P1"));
        REQUIRE(p1 != nullptr);
        REQUIRE(p1->get("B1")->getRenderer()->getProperty("TextColor").getColor() == tgui::Color::Cyan);
    }

    SECTION("Nested Panel forwards themesByAlias to deeply nested button")
    {
        const std::string form =
            "Renderer.1 { TextColor = rgb(200, 100, 50); }\n"
            "Theme.Main { Button = &1; }\n"
            "Panel.Outer {\n"
            "    Position = (0, 0);\n"
            "    Size = (500, 400);\n"
            "    Panel.Inner {\n"
            "        Position = (5, 5);\n"
            "        Size = (200, 150);\n"
            "        Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = @Main; }\n"
            "    }\n"
            "}\n";

        auto runtime = std::make_shared<tgui::Theme>();
        auto rd = std::make_shared<tgui::RendererData>();
        rd->propertyValuePairs[U"TextColor"] = tgui::Color::Green;
        runtime->addRenderer(U"Button", rd);

        tgui::FormLoadOptions opt;
        opt.themesByAlias[U"Main"] = runtime;

        tgui::Panel::Ptr root = tgui::Panel::create();
        std::stringstream stream(form);
        root->loadWidgetsFromStream(stream, true, opt);

        const auto outer = std::dynamic_pointer_cast<tgui::Panel>(root->get("Outer"));
        REQUIRE(outer != nullptr);
        const auto inner = std::dynamic_pointer_cast<tgui::Panel>(outer->get("Inner"));
        REQUIRE(inner != nullptr);
        REQUIRE(inner->get("B1")->getRenderer()->getProperty("TextColor").getColor() == tgui::Color::Green);
    }

    SECTION("ChildWindow forwards themesByAlias to nested buttons")
    {
        const std::string form =
            "Renderer.1 { TextColor = rgb(200, 100, 50); }\n"
            "Theme.Main { Button = &1; }\n"
            "ChildWindow.CW {\n"
            "    Position = (0, 0);\n"
            "    Size = (400, 300);\n"
            "    Title = \"T\";\n"
            "    Button.B1 { Position = (10, 10); Size = (50, 30); Text = \"X\"; Renderer = @Main; }\n"
            "}\n";

        auto runtime = std::make_shared<tgui::Theme>();
        auto rd = std::make_shared<tgui::RendererData>();
        rd->propertyValuePairs[U"TextColor"] = tgui::Color::Magenta;
        runtime->addRenderer(U"Button", rd);

        tgui::FormLoadOptions opt;
        opt.themesByAlias[U"Main"] = runtime;

        tgui::Panel::Ptr root = tgui::Panel::create();
        std::stringstream stream(form);
        root->loadWidgetsFromStream(stream, true, opt);

        const auto cw = std::dynamic_pointer_cast<tgui::ChildWindow>(root->get("CW"));
        REQUIRE(cw != nullptr);
        REQUIRE(cw->get("B1")->getRenderer()->getProperty("TextColor").getColor() == tgui::Color::Magenta);
    }

    SECTION("Theme-blind custom widget matches two-parameter load when FormLoadOptions are default")
    {
        registerFormThemeLoadCustomWidgetTypes();

        const std::string form =
            "Renderer.1 { TextColor = rgb(81, 82, 83); }\n"
            "ThemeBlindButton.TB { Position = (10, 20); Size = (50, 30); Text = \"X\"; Renderer = &1; }\n";

        tgui::Panel::Ptr pOld = tgui::Panel::create();
        std::stringstream sOld(form);
        pOld->loadWidgetsFromStream(sOld);

        tgui::Panel::Ptr pNew = tgui::Panel::create();
        std::stringstream sNew(form);
        pNew->loadWidgetsFromStream(sNew, true, tgui::FormLoadOptions{});

        REQUIRE(pOld->get("TB")->getPosition() == pNew->get("TB")->getPosition());
        REQUIRE(pOld->get("TB")->getSize() == pNew->get("TB")->getSize());
        REQUIRE(pOld->get("TB")->getRenderer()->getProperty("TextColor").getColor()
                == pNew->get("TB")->getRenderer()->getProperty("TextColor").getColor());
    }

    SECTION("Theme-blind custom widget ignores themesByAlias; other widgets on the same form still use it")
    {
        registerFormThemeLoadCustomWidgetTypes();

        // TB uses &1: a theme-blind load has no runtime/fallback theme context, so Renderer = @Main would throw.
        const std::string form =
            "Renderer.1 { TextColor = rgb(200, 100, 50); }\n"
            "Theme.Main { Button = &1; ThemeAwareCustomButton = &1; }\n"
            "ThemeBlindButton.TB { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = &1; }\n"
            "ThemeAwareCustomButton.TAC { Position = (0, 40); Size = (50, 30); Text = \"Z\"; Renderer = @Main; }\n"
            "Button.B1 { Position = (60, 0); Size = (50, 30); Text = \"Y\"; Renderer = @Main; }\n";

        auto runtime = std::make_shared<tgui::Theme>();
        auto rd = std::make_shared<tgui::RendererData>();
        rd->propertyValuePairs[U"TextColor"] = tgui::Color::Cyan;
        runtime->addRenderer(U"Button", rd);
        runtime->addRenderer(U"ThemeAwareCustomButton", rd);

        tgui::FormLoadOptions opt;
        opt.themesByAlias[U"Main"] = runtime;

        tgui::Panel::Ptr panel = tgui::Panel::create();
        std::stringstream stream(form);
        panel->loadWidgetsFromStream(stream, true, opt);

        REQUIRE(panel->get("TB")->getRenderer()->getProperty("TextColor").getColor() == tgui::Color(200, 100, 50));
        REQUIRE(panel->get("TAC")->getRenderer()->getProperty("TextColor").getColor() == tgui::Color::Cyan);
        REQUIRE(panel->get("B1")->getRenderer()->getProperty("TextColor").getColor() == tgui::Color::Cyan);
    }

    SECTION("Theme-blind custom widget ignores applyDefaultTheme; stock Button on the same form still uses default theme")
    {
        registerFormThemeLoadCustomWidgetTypes();

        const tgui::Theme::Ptr oldDefault = tgui::Theme::getDefault();
        auto theme = std::make_shared<tgui::Theme>();
        auto rd = std::make_shared<tgui::RendererData>();
        rd->propertyValuePairs[U"TextColor"] = tgui::Color::Yellow;
        theme->addRenderer(U"Button", rd);
        tgui::Theme::setDefault(theme);

        // Default theme has no ThemeBlindButton section; without Renderer in the form, TB has no TextColor (getColor asserts).
        const std::string form =
            "Renderer.1 { TextColor = rgb(90, 91, 92); }\n"
            "ThemeBlindButton.TB { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = &1; }\n"
            "Button.B1 { Position = (60, 0); Size = (50, 30); Text = \"Y\"; }\n";

        tgui::FormLoadOptions optOn;
        optOn.applyDefaultTheme = true;

        tgui::Panel::Ptr panel = tgui::Panel::create();
        std::stringstream stream(form);
        panel->loadWidgetsFromStream(stream, true, optOn);

        REQUIRE(panel->get("B1")->getRenderer()->getProperty("TextColor").getColor() == tgui::Color::Yellow);
        REQUIRE(panel->get("TB")->getRenderer()->getProperty("TextColor").getColor() == tgui::Color(90, 91, 92));

        tgui::Theme::setDefault(oldDefault);
    }

    SECTION("applyDefaultTheme true: sparse form renderer does not keep default theme texture image")
    {
        const tgui::Theme::Ptr oldDefault = tgui::Theme::getDefault();
        auto theme = std::make_shared<tgui::Theme>();
        auto rd = std::make_shared<tgui::RendererData>();
        rd->propertyValuePairs[U"Texture"] = tgui::Texture{"resources/Texture1.png"};
        theme->addRenderer(U"Button", rd);
        tgui::Theme::setDefault(theme);
        REQUIRE(rd->propertyValuePairs[U"Texture"].getTexture().getData() != nullptr);

        const std::string form =
            "Renderer.1 { TextColor = rgb(11, 22, 33); }\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = &1; }\n";

        tgui::FormLoadOptions opt;
        opt.applyDefaultTheme = true;

        tgui::Panel::Ptr panel = tgui::Panel::create();
        std::stringstream stream(form);
        panel->loadWidgetsFromStream(stream, true, opt);

        const tgui::Button::Ptr btn = panel->get<tgui::Button>(U"B1");
        REQUIRE(btn->getSharedRenderer()->getProperty(U"TextColor").getColor() == tgui::Color(11, 22, 33));
        // Inline renderer has no Texture; setRenderer notifies removal of old Texture and ButtonBase::rendererChanged
        // calls getTexture(), whose generated getter inserts Texture{} when the key is missing (RendererDefines.hpp).
        REQUIRE(btn->getSharedRenderer()->getProperty(U"Texture").getType() == tgui::ObjectConverter::Type::Texture);
        REQUIRE(btn->getSharedRenderer()->getProperty(U"Texture").getTexture().getData() == nullptr);

        tgui::Theme::setDefault(oldDefault);
    }

    SECTION("applyDefaultTheme false: sparse inline renderer has no Texture key (built-in default Button has no Texture)")
    {
        const tgui::Theme::Ptr oldDefault = tgui::Theme::getDefault();
        auto theme = std::make_shared<tgui::Theme>();
        auto rd = std::make_shared<tgui::RendererData>();
        rd->propertyValuePairs[U"Texture"] = tgui::Texture{"resources/Texture1.png"};
        theme->addRenderer(U"Button", rd);
        tgui::Theme::setDefault(theme);

        const std::string form =
            "Renderer.1 { TextColor = rgb(11, 22, 33); }\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = &1; }\n";

        tgui::FormLoadOptions opt;
        opt.applyDefaultTheme = false;

        tgui::Panel::Ptr panel = tgui::Panel::create();
        std::stringstream stream(form);
        panel->loadWidgetsFromStream(stream, true, opt);

        const tgui::Button::Ptr btn = panel->get<tgui::Button>(U"B1");
        REQUIRE(btn->getSharedRenderer()->getProperty(U"TextColor").getColor() == tgui::Color(11, 22, 33));
        REQUIRE(btn->getSharedRenderer()->getPropertyValuePairs().count(U"Texture") == 0);
        REQUIRE(btn->getSharedRenderer()->getProperty(U"Texture").getType() == tgui::ObjectConverter::Type::None);

        tgui::Theme::setDefault(oldDefault);
    }

    SECTION("applyDefaultTheme true: button without Renderer keeps default theme Texture")
    {
        const tgui::Theme::Ptr oldDefault = tgui::Theme::getDefault();
        auto theme = std::make_shared<tgui::Theme>();
        auto rd = std::make_shared<tgui::RendererData>();
        rd->propertyValuePairs[U"Texture"] = tgui::Texture{"resources/Texture1.png"};
        theme->addRenderer(U"Button", rd);
        tgui::Theme::setDefault(theme);

        const std::string form = "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; }\n";

        tgui::FormLoadOptions opt;
        opt.applyDefaultTheme = true;

        tgui::Panel::Ptr panel = tgui::Panel::create();
        std::stringstream stream(form);
        panel->loadWidgetsFromStream(stream, true, opt);

        const tgui::Button::Ptr btn = panel->get<tgui::Button>(U"B1");
        REQUIRE(btn->getSharedRenderer()->getProperty(U"Texture").getType() != tgui::ObjectConverter::Type::None);
        REQUIRE(btn->getSharedRenderer()->getProperty(U"Texture").getTexture().getData() != nullptr);

        tgui::Theme::setDefault(oldDefault);
    }

    SECTION("applyDefaultTheme true vs false: sparse load matches on form fields; Texture key differs when default Button had Texture")
    {
        const tgui::Theme::Ptr oldDefault = tgui::Theme::getDefault();
        auto theme = std::make_shared<tgui::Theme>();
        auto rd = std::make_shared<tgui::RendererData>();
        rd->propertyValuePairs[U"Texture"] = tgui::Texture{"resources/Texture1.png"};
        theme->addRenderer(U"Button", rd);
        tgui::Theme::setDefault(theme);

        const std::string form =
            "Renderer.1 { TextColor = rgb(44, 55, 66); }\n"
            "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = &1; }\n";

        tgui::Panel::Ptr panelTrue = tgui::Panel::create();
        {
            tgui::FormLoadOptions opt;
            opt.applyDefaultTheme = true;
            std::stringstream stream(form);
            panelTrue->loadWidgetsFromStream(stream, true, opt);
        }

        tgui::Theme::setDefault(theme);

        tgui::Panel::Ptr panelFalse = tgui::Panel::create();
        {
            tgui::FormLoadOptions opt;
            opt.applyDefaultTheme = false;
            std::stringstream stream(form);
            panelFalse->loadWidgetsFromStream(stream, true, opt);
        }

        const tgui::Button::Ptr btnTrue = panelTrue->get<tgui::Button>(U"B1");
        const tgui::Button::Ptr btnFalse = panelFalse->get<tgui::Button>(U"B1");

        REQUIRE(btnTrue->getSharedRenderer()->getProperty(U"TextColor") == btnFalse->getSharedRenderer()->getProperty(U"TextColor"));
        REQUIRE(btnTrue->getSharedRenderer()->getPropertyValuePairs().count(U"Texture") == 1);
        REQUIRE(btnFalse->getSharedRenderer()->getPropertyValuePairs().count(U"Texture") == 0);

        for (const char* propName : buttonRendererPropertyNames)
        {
            const tgui::String name(propName);
            auto pTrue = btnTrue->getSharedRenderer()->getProperty(name);
            auto pFalse = btnFalse->getSharedRenderer()->getProperty(name);
            if (name == U"Texture")
            {
                REQUIRE(pFalse.getType() == tgui::ObjectConverter::Type::None);
                REQUIRE(pTrue.getType() == tgui::ObjectConverter::Type::Texture);
                REQUIRE(pTrue.getTexture().getData() == nullptr);
            }
            else
                REQUIRE(pTrue == pFalse);
        }

        tgui::Theme::setDefault(oldDefault);
    }

    SECTION("ButtonRenderer::getTexture const getter can insert empty Texture when map key is missing")
    {
        tgui::Button::Ptr btn = tgui::Button::create();
        auto data = tgui::RendererData::create();
        data->propertyValuePairs[U"TextColor"] = tgui::Color::Red;
        btn->setRenderer(data);

        REQUIRE(btn->getSharedRenderer()->getPropertyValuePairs().count(U"Texture") == 0);
        (void)btn->getSharedRenderer()->getTexture();
        REQUIRE(btn->getSharedRenderer()->getPropertyValuePairs().count(U"Texture") == 1);
        auto texProp = btn->getSharedRenderer()->getProperty(U"Texture");
        REQUIRE(texProp.getTexture().getData() == nullptr);
    }

    SECTION("Widget::load with WidgetLoadResources matches former renderers-map-only behavior")
    {
        const std::string fragment = "Button.B1 { Position = (0, 0); Size = (50, 30); Text = \"X\"; Renderer = &1; }\n";
        auto rd = std::make_shared<tgui::RendererData>();
        rd->propertyValuePairs[U"TextColor"] = tgui::Color(201, 202, 203);

        auto parseNode = [&fragment]
        {
            std::stringstream ss(fragment);
            auto root = tgui::DataIO::parse(ss);
            REQUIRE(root->children.size() == 1);
            return root;
        };

        std::map<tgui::String, std::shared_ptr<tgui::RendererData>> renderersMap;
        renderersMap[U"1"] = rd;

        auto root1 = parseNode();
        TestButton::Ptr wLocal = TestButton::create();
        tgui::WidgetLoadResources resLocal(renderersMap);
        REQUIRE_NOTHROW(wLocal->loadExposed(root1->children[0], resLocal));

        auto root2 = parseNode();
        TestButton::Ptr wTempRes = TestButton::create();
        REQUIRE_NOTHROW(wTempRes->loadExposed(root2->children[0], tgui::WidgetLoadResources(renderersMap)));

        auto root3 = parseNode();
        TestButton::Ptr wTempMap = TestButton::create();
        REQUIRE_NOTHROW(
            wTempMap->loadExposed(root3->children[0],
                                  tgui::WidgetLoadResources(std::map<tgui::String, std::shared_ptr<tgui::RendererData>>{{U"1", rd}})));

        const tgui::Color expected = tgui::Color(201, 202, 203);
        REQUIRE(wLocal->getSharedRenderer()->getProperty("TextColor").getColor() == expected);
        REQUIRE(wTempRes->getSharedRenderer()->getProperty("TextColor").getColor() == expected);
        REQUIRE(wTempMap->getSharedRenderer()->getProperty("TextColor").getColor() == expected);
    }

    SECTION("WidgetLoadResources can be constructed explicitly from renderers map")
    {
        std::map<tgui::String, std::shared_ptr<tgui::RendererData>> map;
        auto data = tgui::RendererData::create();
        map[U"1"] = data;
        tgui::WidgetLoadResources res(map);
        REQUIRE(&res.renderers == &map);
        REQUIRE(res.renderers.find(U"1")->second == data);
    }
}
