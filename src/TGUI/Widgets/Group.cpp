/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Widgets/Group.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group::Group(const Layout2d& size)
    {
        m_callback.widgetType = "Group";
        m_type = "Group";

        m_renderer = aurora::makeCopied<GroupRenderer>();
        setRenderer(RendererData::create());

        setSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group::Ptr Group::create(Layout2d size)
    {
        return std::make_shared<Group>(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group::Ptr Group::copy(Group::ConstPtr group)
    {
        if (group)
            return std::static_pointer_cast<Group>(group->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Group::getChildWidgetsOffset() const
    {
        return {m_paddingCached.left, m_paddingCached.top};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Group::mouseOnWidget(sf::Vector2f pos) const
    {
        for (const auto& widget : m_widgets)
        {
            if (widget->isVisible())
            {
                if (widget->mouseOnWidget(pos - widget->getPosition()))
                    return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::rendererChanged(const std::string& property)
    {
        if (property == "padding")
        {
            m_paddingCached = getRenderer()->getPadding();
        }
        else
            Container::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition().x + m_paddingCached.left, getPosition().y + m_paddingCached.top);

        // Set the clipping for all draw calls that happen until this clipping object goes out of scope
        sf::Vector2f innerSize = {getSize().x - m_paddingCached.left - m_paddingCached.right, getSize().y - m_paddingCached.top - m_paddingCached.bottom};
        Clipping clipping{target, states, {}, innerSize};

        // Draw the child widgets
        drawWidgetContainer(&target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
