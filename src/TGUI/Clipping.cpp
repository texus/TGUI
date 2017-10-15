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


#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    sf::View Clipping::m_originalView;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Clipping::Clipping(sf::RenderTarget& target, const sf::RenderStates& states, sf::Vector2f topLeft, sf::Vector2f size) :
        m_target {target},
        m_oldView{target.getView()}
    {
        sf::Vector2f bottomRight = sf::Vector2f(states.transform.transformPoint(topLeft + size));
        topLeft = sf::Vector2f(states.transform.transformPoint(topLeft));

        const sf::Vector2f viewTopLeft = topLeft;
        size = bottomRight - topLeft;

        topLeft.x -= m_originalView.getCenter().x - (m_originalView.getSize().x / 2.f);
        topLeft.y -= m_originalView.getCenter().y - (m_originalView.getSize().y / 2.f);
        bottomRight.x -= m_originalView.getCenter().x - (m_originalView.getSize().x / 2.f);
        bottomRight.y -= m_originalView.getCenter().y - (m_originalView.getSize().y / 2.f);

        topLeft.x *= m_originalView.getViewport().width / m_originalView.getSize().x;
        topLeft.y *= m_originalView.getViewport().height / m_originalView.getSize().y;
        size.x *= m_originalView.getViewport().width / m_originalView.getSize().x;
        size.y *= m_originalView.getViewport().height / m_originalView.getSize().y;

        topLeft.x += m_originalView.getViewport().left;
        topLeft.y += m_originalView.getViewport().top;

        if (topLeft.x < m_originalView.getViewport().left)
        {
            size.x -= m_originalView.getViewport().left - topLeft.x;
            topLeft.x = m_originalView.getViewport().left;
        }
        if (topLeft.y < m_originalView.getViewport().top)
        {
            size.y -= m_originalView.getViewport().top - topLeft.y;
            topLeft.y = m_originalView.getViewport().top;
        }
        if (size.x > m_originalView.getViewport().left + m_originalView.getViewport().width - topLeft.x)
            size.x = m_originalView.getViewport().left + m_originalView.getViewport().width - topLeft.x;
        if (size.y > m_originalView.getViewport().top + m_originalView.getViewport().height - topLeft.y)
            size.y = m_originalView.getViewport().top + m_originalView.getViewport().height - topLeft.y;

        if ((size.x >= 0) && (size.y >= 0))
        {
            sf::View view{{viewTopLeft.x, viewTopLeft.y, size.x * m_originalView.getSize().x / m_originalView.getViewport().width, size.y * m_originalView.getSize().y / m_originalView.getViewport().height}};
            view.setViewport({topLeft.x, topLeft.y, size.x, size.y});
            target.setView(view);
        }
        else // The clipping area lies outside the viewport
        {
            sf::View view{{0, 0, 0, 0}};
            view.setViewport({0, 0, 0, 0});
            target.setView(view);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Clipping::~Clipping()
    {
        m_target.setView(m_oldView);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Clipping::setGuiView(const sf::View& view)
    {
        m_originalView = view;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
