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
                    {"button", RendererData::create({{"borders", Borders{1}},
                                                     {"bordercolor", Color{60, 60, 60}},
                                                     {"bordercolorhover", Color::Black},
                                                     {"bordercolordown", Color::Black},
                                                     {"bordercolorfocused", Color{30, 30, 180}},
                                                     {"bordercolordisabled", Color{125, 125, 125}},
                                                     {"textcolor", Color{60, 60, 60}},
                                                     {"textcolorhover", Color::Black},
                                                     {"textcolordown", Color::Black},
                                                     {"textcolordisabled", Color{125, 125, 125}},
                                                     {"backgroundcolor", Color{245, 245, 245}},
                                                     {"backgroundcolorhover", Color::White},
                                                     {"backgroundcolordown", Color{235, 235, 235}},
                                                     {"backgroundcolordisabled", Color{230, 230, 230}}})},
                    {"chatbox", RendererData::create({{"borders", Borders{1}},
                                                      {"padding", Padding{0}},
                                                      {"bordercolor", Color::Black},
                                                      {"backgroundcolor", Color{245, 245, 245}}})},
                    {"checkbox", RendererData::create({{"borders", Borders{1}},
                                                       {"bordercolor", Color{60, 60, 60}},
                                                       {"bordercolorhover", Color::Black},
                                                       {"bordercolorfocused", Color{30, 30, 180}},
                                                       {"bordercolordisabled", Color{125, 125, 125}},
                                                       {"textcolor", Color{60, 60, 60}},
                                                       {"textcolorhover", Color::Black},
                                                       {"textcolordisabled", Color{125, 125, 125}},
                                                       {"backgroundcolor", Color{245, 245, 245}},
                                                       {"backgroundcolorhover", Color::White},
                                                       {"backgroundcolordisabled", Color{230, 230, 230}},
                                                       {"checkcolor", Color{60, 60, 60}},
                                                       {"checkcolorhover", Color::Black},
                                                       {"checkcolordisabled", Color{125, 125, 125}},
                                                       {"textdistanceratio", 0.2f}})},
                    {"childwindow", RendererData::create({{"borders", Borders{1}},
                                                          {"bordercolor", Color::Black},
                                                          {"titlecolor", Color{60, 60, 60}},
                                                          {"titlebarcolor", Color::White},
                                                          {"backgroundcolor", Color{230, 230, 230}},
                                                          {"distancetoside", 3.f},
                                                          {"borderbelowtitlebar", 1.f},
                                                          {"paddingbetweenbuttons", 1.f},
                                                          {"showtextontitlebuttons", true}})},
                    {"childwindowbutton", RendererData::create({{"borders", Borders{1}},
                                                                {"bordercolor", Color{60, 60, 60}},
                                                                {"bordercolorhover", Color::Black},
                                                                {"bordercolordown", Color::Black},
                                                                {"textcolor", Color{60, 60, 60}},
                                                                {"textcolorhover", Color::Black},
                                                                {"textcolordown", Color::Black},
                                                                {"backgroundcolor", Color{245, 245, 245}},
                                                                {"backgroundcolorhover", Color::White},
                                                                {"backgroundcolordown", Color{235, 235, 235}}})},
                    {"combobox", RendererData::create({{"borders", Borders{1}},
                                                       {"padding", Padding{0}},
                                                       {"bordercolor", Color::Black},
                                                       {"textcolor", Color{60, 60, 60}},
                                                       {"backgroundcolor", Color{245, 245, 245}},
                                                       {"arrowcolor", Color{60, 60, 60}},
                                                       {"arrowcolorhover", Color::Black},
                                                       {"arrowbackgroundcolor", Color{245, 245, 245}},
                                                       {"arrowbackgroundcolorhover", Color::White}})},
                    {"editbox", RendererData::create({{"borders", Borders{1}},
                                                      {"padding", Padding{0}},
                                                      {"caretwidth", 1.f},
                                                      {"caretcolor", Color::Black},
                                                      {"bordercolor", Color{60, 60, 60}},
                                                      {"bordercolorhover", Color::Black},
                                                      {"bordercolordisabled", Color{125, 125, 125}},
                                                      {"textcolor", Color{60, 60, 60}},
                                                      {"textcolordisabled", Color{125, 125, 125}},
                                                      {"selectedtextcolor", Color::White},
                                                      {"selectedtextbackgroundcolor", Color{0, 110, 255}},
                                                      {"defaulttextcolor", Color{160, 160, 160}},
                                                      {"backgroundcolor", Color{245, 245, 245}},
                                                      {"backgroundcolorhover", Color::White},
                                                      {"backgroundcolordisabled", Color{230, 230, 230}}})},
                    {"group", RendererData::create()},
                    {"horizontallayout", RendererData::create()},
                    {"horizontalwrap", RendererData::create()},
                    {"knob", RendererData::create({{"borders", Borders{2}},
                                                   {"imagerotation", 0.f},
                                                   {"bordercolor", Color::Black},
                                                   {"thumbcolor", Color::Black},
                                                   {"backgroundcolor", Color::White}})},
                    {"label", RendererData::create({{"borders", Borders{}},
                                                    {"bordercolor", Color{60, 60, 60}},
                                                    {"textcolor", Color{60, 60, 60}},
                                                    {"backgroundcolor", Color::Transparent}})},
                    {"listbox", RendererData::create({{"borders", Borders{1}},
                                                      {"padding", Padding{0}},
                                                      {"bordercolor", Color::Black},
                                                      {"textcolor", Color{60, 60, 60}},
                                                      {"textcolorhover", Color::Black},
                                                      {"selectedtextcolor", Color::White},
                                                      {"backgroundcolor", Color{245, 245, 245}},
                                                      {"backgroundcolorhover", Color::White},
                                                      {"selectedbackgroundcolor", Color{0, 110, 255}},
                                                      {"selectedbackgroundcolorhover", Color{30, 150, 255}}})},
                    {"menubar", RendererData::create({{"textcolor", Color{60, 60, 60}},
                                                      {"selectedtextcolor", Color::White},
                                                      {"backgroundcolor", Color::White},
                                                      {"selectedbackgroundcolor", Color{0, 110, 255}}})},
                    {"messagebox", RendererData::create({{"borders", Borders{1}},
                                                         {"bordercolor", Color::Black},
                                                         {"titlecolor", Color{60, 60, 60}},
                                                         {"titlebarcolor", Color::White},
                                                         {"backgroundcolor", Color{230, 230, 230}},
                                                         {"distancetoside", 3.f},
                                                         {"paddingbetweenbuttons", 1.f},
                                                         {"textcolor", Color::Black}})},
                    {"panel", RendererData::create({{"bordercolor", Color::Black},
                                                    {"backgroundcolor", Color::White}})},
                    {"picture", RendererData::create()},
                    {"progressbar", RendererData::create({{"borders", Borders{1}},
                                                          {"bordercolor", Color::Black},
                                                          {"textcolor", Color{60, 60, 60}},
                                                          {"textcolorfilled", Color::White},
                                                          {"backgroundcolor", Color{245, 245, 245}},
                                                          {"fillcolor", Color{0, 110, 255}}})},
                    {"radiobutton", RendererData::create({{"borders", Borders{1}},
                                                          {"bordercolor", Color{60, 60, 60}},
                                                          {"bordercolorhover", Color::Black},
                                                          {"bordercolorfocused", Color{30, 30, 180}},
                                                          {"bordercolordisabled", Color{125, 125, 125}},
                                                          {"textcolor", Color{60, 60, 60}},
                                                          {"textcolorhover", Color::Black},
                                                          {"textcolordisabled", Color{125, 125, 125}},
                                                          {"backgroundcolor", Color{245, 245, 245}},
                                                          {"backgroundcolorhover", Color::White},
                                                          {"backgroundcolordisabled", Color{230, 230, 230}},
                                                          {"checkcolor", Color{60, 60, 60}},
                                                          {"checkcolorhover", Color::Black},
                                                          {"checkcolordisabled", Color{125, 125, 125}},
                                                          {"textdistanceratio", 0.2f}})},
                    {"rangeslider", RendererData::create({{"borders", Borders{1}},
                                                          {"bordercolor", Color{60, 60, 60}},
                                                          {"bordercolorhover", Color::Black},
                                                          {"trackcolor", Color{245, 245, 245}},
                                                          {"trackcolorhover", Color{255, 255, 255}},
                                                          {"thumbcolor", Color{245, 245, 245}},
                                                          {"thumbcolorhover", Color{255, 255, 255}}})},
                    {"scrollablepanel", RendererData::create({{"bordercolor", Color::Black},
                                                              {"backgroundcolor", Color::White}})},
                    {"scrollbar", RendererData::create({{"trackcolor", Color{245, 245, 245}},
                                                        {"thumbcolor", Color{220, 220, 220}},
                                                        {"thumbcolorhover", Color{230, 230, 230}},
                                                        {"arrowbackgroundcolor", Color{245, 245, 245}},
                                                        {"arrowbackgroundcolorhover", Color{255, 255, 255}},
                                                        {"arrowcolor", Color{60, 60, 60}},
                                                        {"arrowcolorhover", Color{0, 0, 0}}})},
                    {"slider", RendererData::create({{"borders", Borders{1}},
                                                     {"bordercolor", Color{60, 60, 60}},
                                                     {"bordercolorhover", Color::Black},
                                                     {"trackcolor", Color{245, 245, 245}},
                                                     {"trackcolorhover", Color{255, 255, 255}},
                                                     {"thumbcolor", Color{245, 245, 245}},
                                                     {"thumbcolorhover", Color{255, 255, 255}}})},
                    {"spinbutton", RendererData::create({{"borders", Borders{1}},
                                                         {"bordercolor", Color::Black},
                                                         {"backgroundcolor", Color{245, 245, 245}},
                                                         {"backgroundcolorhover", Color::White},
                                                         {"arrowcolor", Color{60, 60, 60}},
                                                         {"arrowcolorhover", Color::Black},
                                                         {"borderbetweenarrows", 2.f}})},
                    {"tabs", RendererData::create({{"borders", Borders{1}},
                                                   {"bordercolor", Color::Black},
                                                   {"textcolor", Color{60, 60, 60}},
                                                   {"textcolorhover", Color::Black},
                                                   {"textcolordisabled", Color{125, 125, 125}},
                                                   {"selectedtextcolor", Color::White},
                                                   {"backgroundcolor", Color{245, 245, 245}},
                                                   {"backgroundcolorhover", Color::White},
                                                   {"backgroundcolordisabled", Color{230, 230, 230}},
                                                   {"selectedbackgroundcolor", Color{0, 110, 255}},
                                                   {"selectedbackgroundcolorhover", Color{30, 150, 255}}})},
                    {"textbox", RendererData::create({{"borders", Borders{1}},
                                                      {"padding", Padding{0}},
                                                      {"caretwidth", 1.f},
                                                      {"caretcolor", Color::Black},
                                                      {"bordercolor", Color::Black},
                                                      {"textcolor", Color{60, 60, 60}},
                                                      {"selectedtextcolor", Color::White},
                                                      {"selectedtextbackgroundcolor", Color{0, 110, 255}},
                                                      {"backgroundcolor", Color::White}})},
                    {"verticallayout", RendererData::create({})}
               };
            }
        };



        DefaultTheme defaultTheme;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme* Theme::m_defaultTheme = &defaultTheme;
    std::shared_ptr<BaseThemeLoader> Theme::m_themeLoader = std::make_shared<DefaultThemeLoader>();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme::Theme(const std::string& primary) :
        m_primary(primary)
    {
        if (!primary.empty())
            m_themeLoader->preload(primary);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme::~Theme()
    {
        if (m_defaultTheme == this)
            setDefault(nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::load(const std::string& primary)
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
            renderer->propertyValuePairs = std::map<std::string, ObjectConverter>{};
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

    std::shared_ptr<RendererData> Theme::getRenderer(const std::string& id)
    {
        std::string lowercaseSecondary = toLower(id);

        // If we already have this renderer in cache then just return it
        auto it = m_renderers.find(lowercaseSecondary);
        if (it != m_renderers.end())
            return it->second;

        m_renderers[lowercaseSecondary] = RendererData::create();
        auto& properties = m_themeLoader->load(m_primary, lowercaseSecondary);
        for (const auto& property : properties)
            m_renderers[lowercaseSecondary]->propertyValuePairs[property.first] = ObjectConverter(property.second);

        return m_renderers[lowercaseSecondary];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<RendererData> Theme::getRendererNoThrow(const std::string& id)
    {
        std::string lowercaseSecondary = toLower(id);

        // If we already have this renderer in cache then just return it
        auto it = m_renderers.find(lowercaseSecondary);
        if (it != m_renderers.end())
            return it->second;

        m_renderers[lowercaseSecondary] = RendererData::create();

        if (!m_themeLoader->canLoad(m_primary, lowercaseSecondary))
            return nullptr;

        auto& properties = m_themeLoader->load(m_primary, lowercaseSecondary);
        for (const auto& property : properties)
            m_renderers[lowercaseSecondary]->propertyValuePairs[property.first] = ObjectConverter(property.second);

        return m_renderers[lowercaseSecondary];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::addRenderer(const std::string& id, std::shared_ptr<RendererData> renderer)
    {
        m_renderers[toLower(id)] = renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Theme::removeRenderer(const std::string& id)
    {
        auto it = m_renderers.find(toLower(id));
        if (it != m_renderers.end())
        {
            m_renderers.erase(it);
            return true;
        }
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& Theme::getPrimary() const
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

    void Theme::setDefault(Theme* theme)
    {
        if (theme)
            m_defaultTheme = theme;
        else
        {
            defaultTheme = {};
            m_defaultTheme = &defaultTheme;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme* Theme::getDefault()
    {
        return m_defaultTheme;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
