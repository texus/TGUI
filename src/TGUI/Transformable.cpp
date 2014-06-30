/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2014 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <cmath>

#include <TGUI/Transformable.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Transformable::Transformable(const Transformable& copy) :
        m_transformNeedUpdate(copy.m_transformNeedUpdate),
        m_transform          (copy.m_transform)
    {
        setPosition(copy.m_position);
        setSize(copy.m_size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Transformable& Transformable::operator=(const Transformable& right)
    {
        if (this != &right)
        {
            m_transformNeedUpdate = right.m_transformNeedUpdate;
            m_transform           = right.m_transform;

            setPosition(right.m_position);
            setSize(right.m_size);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Transformable::setPosition(const Layout& position)
    {
        m_position = position;
        m_position.setCallbackFunction(std::bind(&Transformable::updatePosition, this));

        m_transformNeedUpdate = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Transformable::move(const Layout& offset)
    {
        setPosition(m_position + offset);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Transformable::move(const Layout1d& x, const Layout1d& y)
    {
        setPosition({m_position.x + x, m_position.y + y});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Transformable::setSize(const Layout& size)
    {
        m_size = size;

        if (m_size.getValue().x < 0)
        {
            m_size.x = m_size.x * -1;
            m_size.recalculateValue();
        }

        if (m_size.getValue().y < 0)
        {
            m_size.y = m_size.y * -1;
            m_size.recalculateValue();
        }

        m_size.setCallbackFunction(std::bind(&Transformable::updateSize, this));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Transformable::scale(const Layout& factors)
    {
        setSize({m_size.x * factors.x, m_size.y * factors.y});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Transformable::scale(const Layout1d& x, const Layout1d& y)
    {
        setSize({m_size.x * x, m_size.y * y});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Transform& Transformable::getTransform() const
    {
        if (m_transformNeedUpdate)
        {
            m_transform = sf::Transform( 1, 0, m_position.getValue().x,
                                         0, 1, m_position.getValue().y,
                                         0.f, 0.f, 1.f);

            m_transformNeedUpdate = false;
        }

        return m_transform;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Transformable::updatePosition()
    {
        m_position.recalculateValue();
        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Transformable::updateSize()
    {
        m_size.recalculateValue();
        setSize(m_size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
