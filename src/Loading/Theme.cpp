/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Global.hpp>

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
                                                       {"BackgroundColor", Color{245, 245, 245}},
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
            }
        };
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<Theme> Theme::m_defaultTheme = nullptr;
    std::shared_ptr<BaseThemeLoader> Theme::m_themeLoader = std::make_shared<DefaultThemeLoader>();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme::Theme(const String& primary) :
        m_primary(primary)
    {
        if (!primary.empty())
            m_themeLoader->preload(primary);
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

        // Update the existing renderers
        for (auto& pair : m_renderers)
        {
            auto& renderer = pair.second;
            const auto oldData = renderer;

            if (!m_themeLoader->canLoad(m_primary, pair.first))
                continue;

            auto& properties = m_themeLoader->load(m_primary, pair.first);

            // Update the property-value pairs of the renderer
            renderer->propertyValuePairs = std::map<String, ObjectConverter>{};
            for (const auto& property : properties)
                renderer->propertyValuePairs[property.first] = ObjectConverter(property.second);

            // Tell the widgets that were using this renderer about all the updated properties, both new ones and old ones that were now reset to their default value
            auto oldIt = oldData->propertyValuePairs.begin();
            auto newIt = renderer->propertyValuePairs.begin();
            while (oldIt != oldData->propertyValuePairs.end() && newIt != renderer->propertyValuePairs.end())
            {
                if (oldIt->first < newIt->first)
                {
                    // Update values that no longer exist in the new renderer and are now reset to the default value
                    for (const auto& observer : renderer->observers)
                        observer.second(oldIt->first);

                    ++oldIt;
                }
                else
                {
                    // Update changed and new properties
                    for (const auto& observer : renderer->observers)
                        observer.second(newIt->first);

                    if (newIt->first < oldIt->first)
                        ++newIt;
                    else
                    {
                        ++oldIt;
                        ++newIt;
                    }
                }
            }
            while (oldIt != oldData->propertyValuePairs.end())
            {
                for (const auto& observer : renderer->observers)
                    observer.second(oldIt->first);

                ++oldIt;
            }
            while (newIt != renderer->propertyValuePairs.end())
            {
                for (const auto& observer : renderer->observers)
                    observer.second(newIt->first);

                ++newIt;
            }
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

        m_renderers[id] = RendererData::create();

        if (!m_themeLoader->canLoad(m_primary, id))
            return nullptr;

        auto& properties = m_themeLoader->load(m_primary, id);
        for (const auto& property : properties)
            m_renderers[id]->propertyValuePairs[property.first] = ObjectConverter(property.second);

        return m_renderers[id];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::addRenderer(const String& id, std::shared_ptr<RendererData> renderer)
    {
        m_renderers[id] = renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Theme::removeRenderer(const String& id)
    {
        auto it = m_renderers.find(id);
        if (it != m_renderers.end())
        {
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
        m_themeLoader = themeLoader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BaseThemeLoader> Theme::getThemeLoader()
    {
        return m_themeLoader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::setDefault(const String& primary)
    {
        setDefault(tgui::Theme::create(primary));
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
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
