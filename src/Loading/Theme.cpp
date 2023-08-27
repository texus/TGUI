/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Loading/Serializer.hpp>
#include <TGUI/Widget.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        struct DefaultTheme : public Theme
        {
            DefaultTheme()
            {
                m_renderers = {
                    {"BitmapButton", RendererData::create({{"Borders", Borders{1}},
                                                           {"BorderColor", Color{60, 60, 60}},
                                                           {"BorderColorHover", Color::Black},
                                                           {"BorderColorDown", Color::Black},
                                                           {"BorderColorFocused", Color{30, 30, 180}},
                                                           {"BorderColorDisabled", Color{125, 125, 125}},
                                                           {"TextColor", Color{60, 60, 60}},
                                                           {"TextColorHover", Color::Black},
                                                           {"TextColorDown", Color::Black},
                                                           {"TextColorDisabled", Color{125, 125, 125}},
                                                           {"BackgroundColor", Color{245, 245, 245}},
                                                           {"BackgroundColorHover", Color::White},
                                                           {"BackgroundColorDown", Color{235, 235, 235}},
                                                           {"BackgroundColorDisabled", Color{230, 230, 230}}})},
                    {"Button", RendererData::create({{"Borders", Borders{1}},
                                                     {"BorderColor", Color{60, 60, 60}},
                                                     {"BorderColorHover", Color::Black},
                                                     {"BorderColorDown", Color::Black},
                                                     {"BorderColorFocused", Color{30, 30, 180}},
                                                     {"BorderColorDisabled", Color{125, 125, 125}},
                                                     {"TextColor", Color{60, 60, 60}},
                                                     {"TextColorHover", Color::Black},
                                                     {"TextColorDown", Color::Black},
                                                     {"TextColorDisabled", Color{125, 125, 125}},
                                                     {"BackgroundColor", Color{245, 245, 245}},
                                                     {"BackgroundColorHover", Color::White},
                                                     {"BackgroundColorDown", Color{235, 235, 235}},
                                                     {"BackgroundColorDisabled", Color{230, 230, 230}}})},
                    {"ChatBox", RendererData::create({{"Borders", Borders{1}},
                                                      {"Padding", Padding{0}},
                                                      {"BorderColor", Color::Black},
                                                      {"BackgroundColor", Color{245, 245, 245}}})},
                    {"CheckBox", RendererData::create({{"Borders", Borders{1}},
                                                       {"BorderColor", Color{60, 60, 60}},
                                                       {"BorderColorHover", Color::Black},
                                                       {"BorderColorFocused", Color{30, 30, 180}},
                                                       {"BorderColorDisabled", Color{125, 125, 125}},
                                                       {"TextColor", Color{60, 60, 60}},
                                                       {"TextColorHover", Color::Black},
                                                       {"TextColorDisabled", Color{125, 125, 125}},
                                                       {"BackgroundColor", Color{245, 245, 245}},
                                                       {"BackgroundColorHover", Color::White},
                                                       {"BackgroundColorDisabled", Color{230, 230, 230}},
                                                       {"CheckColor", Color{60, 60, 60}},
                                                       {"CheckColorHover", Color::Black},
                                                       {"CheckColorDisabled", Color{125, 125, 125}},
                                                       {"TextDistanceRatio", 0.2f}})},
                    {"ChildWindow", RendererData::create({{"Borders", Borders{1}},
                                                          {"BorderColor", Color::Black},
                                                          {"TitleColor", Color{60, 60, 60}},
                                                          {"TitleBarColor", Color::White},
                                                          {"BackgroundColor", Color{230, 230, 230}},
                                                          {"DistanceToSide", 3.f},
                                                          {"BorderBelowTitleBar", 1.f},
                                                          {"PaddingBetweenButtons", 1.f},
                                                          {"ShowTextOnTitleButtons", true}})},
                    {"ChildWindowButton", RendererData::create({{"Borders", Borders{1}},
                                                                {"BorderColor", Color{60, 60, 60}},
                                                                {"BorderColorHover", Color::Black},
                                                                {"BorderColorDown", Color::Black},
                                                                {"TextColor", Color{60, 60, 60}},
                                                                {"TextColorHover", Color::Black},
                                                                {"TextColorDown", Color::Black},
                                                                {"BackgroundColor", Color{245, 245, 245}},
                                                                {"BackgroundColorHover", Color::White},
                                                                {"BackgroundColorDown", Color{235, 235, 235}}})},
                    {"ColorPicker", RendererData::create({{"Borders", Borders{1}},
                                                          {"BorderColor", Color::Black},
                                                          {"TitleColor", Color{60, 60, 60}},
                                                          {"TitleBarColor", Color::White},
                                                          {"BackgroundColor", Color{230, 230, 230}},
                                                          {"DistanceToSide", 3.f},
                                                          {"BorderBelowTitleBar", 1.f},
                                                          {"PaddingBetweenButtons", 1.f},
                                                          {"ShowTextOnTitleButtons", true}})},
                    {"ComboBox", RendererData::create({{"Borders", Borders{1}},
                                                       {"Padding", Padding{0}},
                                                       {"BorderColor", Color::Black},
                                                       {"TextColor", Color{60, 60, 60}},
                                                       {"TextColorDisabled", Color{125, 125, 125}},
                                                       {"BackgroundColor", Color{245, 245, 245}},
                                                       {"BackgroundColorDisabled", Color{230, 230, 230}},
                                                       {"ArrowColor", Color{60, 60, 60}},
                                                       {"ArrowColorHover", Color::Black},
                                                       {"ArrowBackgroundColor", Color{245, 245, 245}},
                                                       {"ArrowBackgroundColorHover", Color::White}})},
                    {"EditBox", RendererData::create({{"Borders", Borders{1}},
                                                      {"Padding", Padding{0}},
                                                      {"CaretWidth", 1.f},
                                                      {"CaretColor", Color::Black},
                                                      {"BorderColor", Color{60, 60, 60}},
                                                      {"BorderColorHover", Color::Black},
                                                      {"BorderColorDisabled", Color{125, 125, 125}},
                                                      {"TextColor", Color{60, 60, 60}},
                                                      {"TextColorDisabled", Color{125, 125, 125}},
                                                      {"SelectedTextColor", Color::White},
                                                      {"SelectedTextBackgroundColor", Color{0, 110, 255}},
                                                      {"DefaultTextColor", Color{160, 160, 160}},
                                                      {"BackgroundColor", Color{245, 245, 245}},
                                                      {"BackgroundColorHover", Color::White},
                                                      {"BackgroundColorDisabled", Color{230, 230, 230}}})},
                    {"FileDialog", RendererData::create({{"Borders", Borders{1}},
                                                         {"BorderColor", Color::Black},
                                                         {"TitleColor", Color{60, 60, 60}},
                                                         {"TitleBarColor", Color::White},
                                                         {"BackgroundColor", Color{230, 230, 230}},
                                                         {"DistanceToSide", 3.f},
                                                         {"BorderBelowTitleBar", 1.f},
                                                         {"PaddingBetweenButtons", 1.f},
                                                         {"ShowTextOnTitleButtons", true}})},
                    {"Group", RendererData::create()},
                    {"HorizontalLayout", RendererData::create()},
                    {"HorizontalWrap", RendererData::create()},
                    {"Knob", RendererData::create({{"Borders", Borders{2}},
                                                   {"ImageRotation", 0.f},
                                                   {"BorderColor", Color::Black},
                                                   {"ThumbColor", Color::Black},
                                                   {"BackgroundColor", Color::White}})},
                    {"Label", RendererData::create({{"Borders", Borders{}},
                                                    {"BorderColor", Color{60, 60, 60}},
                                                    {"TextColor", Color{60, 60, 60}},
                                                    {"BackgroundColor", Color::Transparent}})},
                    {"ListBox", RendererData::create({{"Borders", Borders{1}},
                                                      {"Padding", Padding{0}},
                                                      {"BorderColor", Color::Black},
                                                      {"TextColor", Color{60, 60, 60}},
                                                      {"TextColorHover", Color::Black},
                                                      {"SelectedTextColor", Color::White},
                                                      {"BackgroundColor", Color{245, 245, 245}},
                                                      {"BackgroundColorHover", Color::White},
                                                      {"SelectedBackgroundColor", Color{0, 110, 255}},
                                                      {"SelectedBackgroundColorHover", Color{30, 150, 255}}})},
                    {"ListView", RendererData::create({{"Borders", Borders{1}},
                                                       {"Padding", Padding{0}},
                                                       {"BorderColor", Color::Black},
                                                       {"SeparatorColor", Color(200, 200, 200)},
                                                       {"HeaderTextColor", Color::Black},
                                                       {"HeaderBackgroundColor", Color(230, 230, 230)},
                                                       {"TextColor", Color{60, 60, 60}},
                                                       {"TextColorHover", Color::Black},
                                                       {"SelectedTextColor", Color::White},
                                                       {"BackgroundColor", Color{245, 245, 245}},
                                                       {"BackgroundColorHover", Color::White},
                                                       {"SelectedBackgroundColor", Color{0, 110, 255}},
                                                       {"SelectedBackgroundColorHover", Color{30, 150, 255}}})},
                    {"MenuBar", RendererData::create({{"TextColor", Color{60, 60, 60}},
                                                      {"SelectedTextColor", Color::White},
                                                      {"BackgroundColor", Color::White},
                                                      {"SelectedBackgroundColor", Color{0, 110, 255}},
                                                      {"TextColorDisabled", Color{125, 125, 125}},
                                                      {"SeparatorColor", Color::Black},
                                                      {"SeparatorThickness", 1.f},
                                                      {"SeparatorVerticalPadding", 1.f}})},
                    {"MessageBox", RendererData::create({{"Borders", Borders{1}},
                                                         {"BorderColor", Color::Black},
                                                         {"TitleColor", Color{60, 60, 60}},
                                                         {"TitleBarColor", Color::White},
                                                         {"BackgroundColor", Color{230, 230, 230}},
                                                         {"DistanceToSide", 3.f},
                                                         {"PaddingBetweenButtons", 1.f},
                                                         {"TextColor", Color::Black}})},
                    {"Panel", RendererData::create({{"BorderColor", Color::Black},
                                                    {"BackgroundColor", Color::White}})},
                    {"PanelListBox", RendererData::create({{"ItemsBackgroundColor", Color{245, 245, 245}},
                                                           {"ItemsBackgroundColorHover", Color::White},
                                                           {"SelectedItemsBackgroundColor", Color{0, 110, 255}},
                                                           {"SelectedItemsBackgroundColorHover", Color{30, 150, 255}}})},
                    {"Picture", RendererData::create()},
                    {"ProgressBar", RendererData::create({{"Borders", Borders{1}},
                                                          {"BorderColor", Color::Black},
                                                          {"TextColor", Color{60, 60, 60}},
                                                          {"TextColorFilled", Color::White},
                                                          {"BackgroundColor", Color{245, 245, 245}},
                                                          {"FillColor", Color{0, 110, 255}}})},
                    {"RadioButton", RendererData::create({{"Borders", Borders{1}},
                                                          {"BorderColor", Color{60, 60, 60}},
                                                          {"BorderColorHover", Color::Black},
                                                          {"BorderColorFocused", Color{30, 30, 180}},
                                                          {"BorderColorDisabled", Color{125, 125, 125}},
                                                          {"TextColor", Color{60, 60, 60}},
                                                          {"TextColorHover", Color::Black},
                                                          {"TextColorDisabled", Color{125, 125, 125}},
                                                          {"BackgroundColor", Color{245, 245, 245}},
                                                          {"BackgroundColorHover", Color::White},
                                                          {"BackgroundColorDisabled", Color{230, 230, 230}},
                                                          {"CheckColor", Color{60, 60, 60}},
                                                          {"CheckColorHover", Color::Black},
                                                          {"CheckColorDisabled", Color{125, 125, 125}},
                                                          {"TextDistanceRatio", 0.2f}})},
                    {"RangeSlider", RendererData::create({{"Borders", Borders{1}},
                                                          {"BorderColor", Color{60, 60, 60}},
                                                          {"BorderColorHover", Color::Black},
                                                          {"SelectedTrackColor", Color{0, 110, 255}},
                                                          {"TrackColor", Color{245, 245, 245}},
                                                          {"TrackColorHover", Color{255, 255, 255}},
                                                          {"ThumbColor", Color{245, 245, 245}},
                                                          {"ThumbColorHover", Color{255, 255, 255}}})},
                    {"ScrollablePanel", RendererData::create({{"BorderColor", Color::Black},
                                                              {"BackgroundColor", Color::White}})},
                    {"Scrollbar", RendererData::create({{"TrackColor", Color{245, 245, 245}},
                                                        {"ThumbColor", Color{220, 220, 220}},
                                                        {"ThumbColorHover", Color{230, 230, 230}},
                                                        {"ArrowBackgroundColor", Color{245, 245, 245}},
                                                        {"ArrowBackgroundColorHover", Color{255, 255, 255}},
                                                        {"ArrowColor", Color{60, 60, 60}},
                                                        {"ArrowColorHover", Color{0, 0, 0}}})},
                    {"SeparatorLine", RendererData::create({{"Color", Color::Black}})},
                    {"Slider", RendererData::create({{"Borders", Borders{1}},
                                                     {"BorderColor", Color{60, 60, 60}},
                                                     {"BorderColorHover", Color::Black},
                                                     {"TrackColor", Color{245, 245, 245}},
                                                     {"TrackColorHover", Color{255, 255, 255}},
                                                     {"ThumbColor", Color{245, 245, 245}},
                                                     {"ThumbColorHover", Color{255, 255, 255}}})},
                    {"SpinButton", RendererData::create({{"Borders", Borders{1}},
                                                         {"BorderColor", Color::Black},
                                                         {"BackgroundColor", Color{245, 245, 245}},
                                                         {"BackgroundColorHover", Color::White},
                                                         {"ArrowColor", Color{60, 60, 60}},
                                                         {"ArrowColorHover", Color::Black},
                                                         {"BorderBetweenArrows", 2.f}})},
                    {"SpinControl", RendererData::create({})},
                    {"TabContainer", RendererData::create({})},
                    {"Tabs", RendererData::create({{"Borders", Borders{1}},
                                                   {"BorderColor", Color::Black},
                                                   {"TextColor", Color{60, 60, 60}},
                                                   {"TextColorHover", Color::Black},
                                                   {"TextColorDisabled", Color{125, 125, 125}},
                                                   {"SelectedTextColor", Color::White},
                                                   {"BackgroundColor", Color{245, 245, 245}},
                                                   {"BackgroundColorHover", Color::White},
                                                   {"BackgroundColorDisabled", Color{230, 230, 230}},
                                                   {"SelectedBackgroundColor", Color{0, 110, 255}},
                                                   {"SelectedBackgroundColorHover", Color{30, 150, 255}}})},
                    {"TextArea", RendererData::create({{"Borders", Borders{1}},
                                                      {"Padding", Padding{0}},
                                                      {"CaretWidth", 1.f},
                                                      {"CaretColor", Color::Black},
                                                      {"BorderColor", Color::Black},
                                                      {"TextColor", Color{60, 60, 60}},
                                                      {"DefaultTextColor", Color{160, 160, 160}},
                                                      {"SelectedTextColor", Color::White},
                                                      {"SelectedTextBackgroundColor", Color{0, 110, 255}},
                                                      {"BackgroundColor", Color::White}})},
                    {"ToggleButton", RendererData::create({{"Borders", Borders{1}},
                                                           {"BorderColor", Color{60, 60, 60}},
                                                           {"BorderColorHover", Color::Black},
                                                           {"BorderColorDown", Color::Black},
                                                           {"BorderColorFocused", Color{30, 30, 180}},
                                                           {"BorderColorDisabled", Color{125, 125, 125}},
                                                           {"TextColor", Color{60, 60, 60}},
                                                           {"TextColorHover", Color::Black},
                                                           {"TextColorDown", Color::White},
                                                           {"TextColorDisabled", Color{125, 125, 125}},
                                                           {"BackgroundColor", Color{245, 245, 245}},
                                                           {"BackgroundColorHover", Color::White},
                                                           {"BackgroundColorDown", Color{0, 110, 255}},
                                                           {"BackgroundColorDownHover", Color{0, 140, 255}},
                                                           {"BackgroundColorDisabled", Color{230, 230, 230}}})},
                    {"TreeView", RendererData::create({{"Borders", Borders{1}},
                                                       {"Padding", Padding{0}},
                                                       {"BorderColor", Color::Black},
                                                       {"TextColor", Color{60, 60, 60}},
                                                       {"TextColorHover", Color::Black},
                                                       {"SelectedTextColor", Color::White},
                                                       {"BackgroundColor", Color{245, 245, 245}},
                                                       {"BackgroundColorHover", Color::White},
                                                       {"SelectedBackgroundColor", Color{0, 110, 255}},
                                                       {"SelectedBackgroundColorHover", Color{30, 150, 255}}})},
                    {"VerticalLayout", RendererData::create({})}
               };

                for (const auto& pair : m_renderers)
                    pair.second->connectedTheme = this;
            }
        };
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<String, String> Theme::m_rendererInheritanceParents = {
        {"BitmapButton", "Button"},
        {"CheckBox", "RadioButton"},
        {"ColorPicker", "ChildWindow"},
        {"FileDialog", "ChildWindow"},
        {"ListView", "ListBox"},
        {"MessageBox", "ChildWindow"},
        {"RangeSlider", "Slider"},
        {"RichTextLabel", "Label"},
        {"ScrollablePanel", "Panel"},
        {"PanelListBox", "ScrollablePanel"},
        {"ToggleButton", "Button"},
        {"TreeView", "ListBox"},
    };

    std::map<String, std::map<String, String>> Theme::m_rendererDefaultSubwidgets = {
        {"ChatBox", {{"Scrollbar", ""}}},
        {"ChildWindow", {{"CloseButton", "Button"}}},
        {"ColorPicker", {{"Button", ""}, {"Label", ""}, {"Slider", ""}}},
        {"ComboBox", {{"ListBox", ""}}},
        {"FileDialog", {{"Button", ""}, {"EditBox", ""}, {"ListView", ""}, {"FilenameLabel", "Label"}, {"FileTypeComboBox", "ComboBox"}}},
        {"Label", {{"Scrollbar", ""}}},
        {"ListBox", {{"Scrollbar", ""}}},
        {"ListView", {{"Scrollbar", ""}}},
        {"MessageBox", {{"Button", ""}}},
        {"ScrollablePanel", {{"Scrollbar", ""}}},
        {"SpinControl", {{"SpinButton", ""}, {"SpinText", "EditBox"}}},
        {"TextArea", {{"Scrollbar", ""}}},
        {"TreeView", {{"Scrollbar", ""}}},
    };

    std::map<String, std::map<String, String>> Theme::m_rendererInheritedGlobalProperties = {
        {"", {
            {"BorderColor", "TextColor"},
            {"ArrowColor", "TextColor"},
            {"ArrowBackgroundColor", "BackgroundColor"},
        }},
        {"Button", {
            {"TextColor", ""},
            {"TextColorHover", ""},
            {"TextColorDisabled", ""},
            {"BackgroundColor", ""},
            {"BackgroundColorHover", ""},
            {"BackgroundColorDisabled", ""},
            {"BorderColor", ""},
            {"Borders", ""},
        }},
        {"ChatBox", {
            {"BackgroundColor", ""},
            {"BorderColor", ""},
            {"Borders", ""},
            {"ScrollbarWidth", ""},
        }},
        {"ChildWindow", {
            {"BackgroundColor", ""},
            {"TitleColor", "TextColor"},
            {"BorderColor", ""},
            {"Borders", ""},
        }},
        {"ComboBox", {
            {"TextColor", ""},
            {"TextColorDisabled", ""},
            {"BackgroundColor", ""},
            {"BackgroundColorDisabled", ""},
            {"ArrowBackgroundColor", ""},
            {"ArrowBackgroundColorHover", ""},
            {"ArrowBackgroundColorDisabled", ""},
            {"ArrowColor", ""},
            {"ArrowColorHover", ""},
            {"ArrowColorDisabled", ""},
            {"BorderColor", ""},
            {"Borders", ""},
        }},
        {"EditBox", {
            {"TextColor", ""},
            {"TextColorDisabled", ""},
            {"SelectedTextColor", ""},
            {"SelectedTextBackgroundColor", "SelectedBackgroundColor"},
            {"BackgroundColor", ""},
            {"BackgroundColorHover", ""},
            {"BackgroundColorDisabled", ""},
            {"BorderColor", ""},
            {"Borders", ""},
        }},
        {"Knob", {
            {"ThumbColor", "TextColor"},
            {"BackgroundColor", ""},
            {"BorderColor", ""},
            {"Borders", ""},
        }},
        {"Label", {
            {"TextColor", ""},
            {"BorderColor", ""},
            {"ScrollbarWidth", ""},
            // BackgroundColor and Borders are not inherited, because the default label should be simple
        }},
        {"ListBox", {
            {"BackgroundColor", ""},
            {"BackgroundColorHover", ""},
            {"TextColor", ""},
            {"TextColorHover", ""},
            {"SelectedBackgroundColor", ""},
            {"SelectedBackgroundColorHover", ""},
            {"SelectedTextColor", ""},
            {"SelectedTextColorHover", ""},
            {"BorderColor", ""},
            {"Borders", ""},
            {"ScrollbarWidth", ""},
        }},
        {"MenuBar", {
            {"BackgroundColor", ""},
            {"SelectedBackgroundColor", ""},
            {"TextColor", ""},
            {"SelectedTextColor", ""},
            {"SeparatorColor", "BorderColor"},
        }},
        {"MessageBox", {
            {"TextColor", ""},
        }},
        {"Panel", {
            {"BackgroundColor", ""},
            {"BorderColor", ""},
            {"Borders", ""},
        }},
        {"PanelListBox", {
            {"BackgroundColor", ""},
            {"BorderColor", ""},
            {"Borders", ""}
        }},
        {"ProgressBar", {
            {"BackgroundColor", ""},
            {"FillColor", "SelectedBackgroundColor"},
            {"TextColor", ""},
            {"BorderColor", ""},
            {"Borders", ""},
        }},
        {"RadioButton", {
            {"TextColor", ""},
            {"TextColorHover", ""},
            {"TextColorDisabled", ""},
            {"BackgroundColor", ""},
            {"BackgroundColorHover", ""},
            {"BackgroundColorDisabled", ""},
            {"CheckColor", "TextColor"},
            {"BorderColor", ""},
            {"Borders", ""},
        }},
        {"RangeSlider", {
            {"SelectedTrackColor", "SelectedBackgroundColor"},
            {"SelectedTrackColorHover", "SelectedBackgroundColorHover"},
        }},
        {"ScrollablePanel", {
            {"BackgroundColor", ""},
            {"BorderColor", ""},
            {"Borders", ""},
            {"ScrollbarWidth", ""},
        }},
        {"Scrollbar", {
            {"TrackColor", "BackgroundColor"},
            {"ThumbColor", "ArrowColor"},
            {"ArrowBackgroundColor", ""},
            {"ArrowBackgroundColorHover", ""},
            {"ArrowColor", ""},
            {"ArrowColorHover", ""},
        }},
        {"SeparatorLine", {
            {"Color", "BorderColor"},
        }},
        {"Slider", {
            {"TrackColor", "BackgroundColor"},
            {"ThumbColor", "ArrowColor"},
            {"BorderColor", ""},
            {"Borders", ""},
        }},
        {"SpinButton", {
            {"BackgroundColor", ""},
            {"BackgroundColorHover", ""},
            {"ArrowColor", ""},
            {"ArrowColorHover", ""},
            {"BorderColor", ""},
            {"Borders", ""},
        }},
        {"Tabs", {
            {"BackgroundColor", ""},
            {"SelectedBackgroundColor", ""},
            {"TextColor", ""},
            {"SelectedTextColor", ""},
            {"BorderColor", ""},
            {"Borders", ""},
        }},
        {"TextArea", {
            {"BackgroundColor", ""},
            {"TextColor", ""},
            {"SelectedTextColor", ""},
            {"SelectedTextBackgroundColor", "SelectedBackgroundColor"},
            {"CaretColor", "TextColor"},
            {"BorderColor", ""},
            {"Borders", ""},
            {"ScrollbarWidth", ""},
        }},
        {"ToggleButton", {
            {"TextColorDown", "SelectedTextColor"},
            {"BackgroundColorDown", "SelectedBackgroundColor"},
        }},
    };

    std::shared_ptr<Theme> Theme::m_defaultTheme = nullptr;
    std::shared_ptr<BaseThemeLoader> Theme::m_themeLoader = std::make_shared<DefaultThemeLoader>();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme::Theme(const String& primary) :
        m_primary(primary)
    {
        if (primary.empty())
            return;

        m_themeLoader->preload(primary);

        // Load the global properties
        const auto& globalProperties = m_themeLoader->getGlobalProperties(m_primary);
        for (const auto& property : globalProperties)
            m_globalProperties[property.first] = ObjectConverter(property.second);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme::~Theme()
    {
        for (const auto& pair : m_renderers)
        {
            if (pair.second->connectedTheme == this)
                pair.second->connectedTheme = nullptr;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme::Theme(const Theme& other) :
        m_renderers       {},
        m_globalProperties{other.m_globalProperties},
        m_primary         {other.m_primary}
    {
        for (const auto& pair : other.m_renderers)
        {
            auto data = std::make_shared<RendererData>(*pair.second);
            data->observers = {};
            data->connectedTheme = this;
            data->shared = true;
            m_renderers[pair.first] = data;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme::Theme(Theme&& other) noexcept :
        m_renderers       {std::move(other.m_renderers)},
        m_globalProperties{std::move(other.m_globalProperties)},
        m_primary         {std::move(other.m_primary)}
    {
        for (const auto& pair : m_renderers)
            pair.second->connectedTheme = this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme& Theme::operator= (const Theme& other)
    {
        if (this != &other)
        {
            Theme temp(other);

            std::swap(m_renderers,        temp.m_renderers);
            std::swap(m_globalProperties, temp.m_globalProperties);
            std::swap(m_primary,          temp.m_primary);

            for (const auto& pair : m_renderers)
                pair.second->connectedTheme = this;
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme& Theme::operator= (Theme&& other) noexcept
    {
        if (this != &other)
        {
            m_renderers = std::move(other.m_renderers);
            m_globalProperties = std::move(other.m_globalProperties);
            m_primary = std::move(other.m_primary);

            for (const auto& pair : m_renderers)
                pair.second->connectedTheme = this;
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme::Ptr Theme::create(const String& primary)
    {
        return std::make_shared<Theme>(primary);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::load(const String& primary)
    {
        m_primary = primary;
        m_themeLoader->preload(primary);

        // Load the new global properties
        m_globalProperties.clear();
        const auto& globalProperties = m_themeLoader->getGlobalProperties(m_primary);
        for (const auto& property : globalProperties)
            m_globalProperties[property.first] = ObjectConverter(property.second);

        // Update the existing widgets that were using renderers from this theme
        for (auto& pair : m_renderers)
        {
            if (!m_themeLoader->canLoad(m_primary, pair.first))
                continue;

            auto& renderer = pair.second;
            const auto& properties = m_themeLoader->load(m_primary, pair.first);
            auto observers = std::move(renderer->observers);

            renderer = RendererData::create();
            renderer->observers = std::move(observers);
            renderer->connectedTheme = this;
            for (const auto& property : properties)
                renderer->propertyValuePairs[property.first] = ObjectConverter(property.second);

            for (auto& observer : renderer->observers)
                observer->setRenderer(renderer);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<RendererData> Theme::getRenderer(const String& id)
    {
        // If we already have this renderer in cache then just return it
        auto it = m_renderers.find(id);
        if (it != m_renderers.end())
            return it->second;

        m_renderers[id] = RendererData::create();
        m_renderers[id]->connectedTheme = this;
        auto& properties = m_themeLoader->load(m_primary, id);
        for (const auto& property : properties)
            m_renderers[id]->propertyValuePairs[property.first] = ObjectConverter(property.second);

        return m_renderers[id];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<RendererData> Theme::getRendererNoThrow(const String& id)
    {
        // If we already have this renderer in cache then just return it
        auto it = m_renderers.find(id);
        if (it != m_renderers.end())
            return it->second;

        if (!m_themeLoader->canLoad(m_primary, id))
            return nullptr;

        m_renderers[id] = RendererData::create();
        m_renderers[id]->connectedTheme = this;

        const auto& properties = m_themeLoader->load(m_primary, id);
        for (const auto& property : properties)
            m_renderers[id]->propertyValuePairs[property.first] = ObjectConverter(property.second);

        return m_renderers[id];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter Theme::getGlobalProperty(const String& property)
    {
        auto propertyIt = m_globalProperties.find(property);
        if (propertyIt != m_globalProperties.end())
            return propertyIt->second;
        else
            return {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::addRenderer(const String& id, std::shared_ptr<RendererData> renderer)
    {
        if (!renderer)
            renderer = RendererData::create();

        auto existingRendererIt = m_renderers.find(id);
        if (existingRendererIt != m_renderers.end())
        {
            if (existingRendererIt->second->connectedTheme == this)
                existingRendererIt->second->connectedTheme = nullptr;
        }

        m_renderers[id] = renderer;
        m_renderers[id]->connectedTheme = this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Theme::removeRenderer(const String& id)
    {
        auto it = m_renderers.find(id);
        if (it != m_renderers.end())
        {
            if (it->second->connectedTheme == this)
                it->second->connectedTheme = nullptr;

            m_renderers.erase(it);
            return true;
        }
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& Theme::getPrimary() const
    {
        return m_primary;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::setThemeLoader(std::shared_ptr<BaseThemeLoader> themeLoader)
    {
        m_themeLoader = std::move(themeLoader);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BaseThemeLoader> Theme::getThemeLoader()
    {
        return m_themeLoader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::setDefault(const String& primary)
    {
        setDefault(Theme::create(primary));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::setDefault(std::shared_ptr<Theme> theme)
    {
        m_defaultTheme = std::move(theme);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::setDefault(std::nullptr_t)
    {
        m_defaultTheme = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<Theme> Theme::getDefault()
    {
        if (!m_defaultTheme)
            m_defaultTheme = std::make_shared<DefaultTheme>();

        return m_defaultTheme;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::addRendererInheritanceParent(const String& widgetType, const String& parentType)
    {
        m_rendererInheritanceParents[widgetType] = parentType;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String Theme::getRendererInheritanceParent(const String& widgetType)
    {
        return m_rendererInheritanceParents[widgetType];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::addRendererDefaultSubwidget(const String& widgetType, const String& property, const String& propertyWidgetType)
    {
        m_rendererDefaultSubwidgets[widgetType][property] = propertyWidgetType;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<String, String> Theme::getRendererDefaultSubwidgets(const String& widgetType)
    {
        return m_rendererDefaultSubwidgets[widgetType];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::addRendererInheritedGlobalProperty(const String& widgetType, const String& property, const String& globalProperty)
    {
        m_rendererInheritedGlobalProperties[widgetType][property] = globalProperty;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<String, String> Theme::getRendererInheritedGlobalProperties(const String& widgetType)
    {
        return m_rendererInheritedGlobalProperties[widgetType];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
