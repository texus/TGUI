/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Exception.hpp>
#include <TGUI/Loading/Serializer.hpp>
#include <TGUI/Global.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BaseThemeLoader> Theme::m_themeLoader = std::make_shared<DefaultThemeLoader>();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme::Theme(const std::string& primary) :
        m_primary(primary)
    {
        if (!primary.empty())
            m_themeLoader->preload(primary);
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

            // Try to load the new renderer
            const std::map<sf::String, sf::String>* properties;
            try
            {
                properties = &m_themeLoader->load(m_primary, pair.first);
            }
            catch (const Exception&)
            {
                // Exceptions are ignored. When a renderer was loaded from the old theme but is no longer found
                // in the new theme then just ignore it and keep the old renderer.
                continue;
            }

            // Keep track of the old font
            Font oldFont;
            if (renderer->propertyValuePairs.find("font") != pair.second->propertyValuePairs.end())
                oldFont = renderer->propertyValuePairs["font"].getFont();

            // Update the property-value pairs of the renderer
            renderer->propertyValuePairs = std::map<std::string, ObjectConverter>{};
            for (const auto& property : *properties)
                renderer->propertyValuePairs[property.first] = ObjectConverter(property.second);

            // If there used to be a font but no new font was set then keep the old font
            if ((properties->find("font") == properties->end()) && (oldFont != nullptr))
                renderer->propertyValuePairs["font"] = ObjectConverter(oldFont);

            // Reload the widgets that were using the renderer
            for (const auto& observer : renderer->observers)
            {
                for (auto& property : renderer->propertyValuePairs)
                    observer.second(property.first, property.second);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<RendererData> Theme::getRenderer(const std::string& id)
    {
        std::string lowercaseSecondart = toLower(id);

        // If we already have this renderer in cache then just return it
        auto it = m_renderers.find(lowercaseSecondart);
        if (it != m_renderers.end())
            return it->second;

        m_renderers[lowercaseSecondart] = std::make_shared<RendererData>();
        auto& properties = m_themeLoader->load(m_primary, lowercaseSecondart);
        for (const auto& property : properties)
            m_renderers[lowercaseSecondart]->propertyValuePairs[property.first] = ObjectConverter(property.second);

        return m_renderers[lowercaseSecondart];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::addRenderer(const std::string& id, std::shared_ptr<RendererData> renderer)
    {
        m_renderers[id] = renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Theme::removeRenderer(const std::string& id)
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
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
