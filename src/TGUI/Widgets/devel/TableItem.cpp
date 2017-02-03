/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
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


#include <TGUI/Widgets/devel/TableItem.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TableItem::TableItem()
    {
        m_callback.widgetType = "TableItem";
        setBackgroundColor(sf::Color::Transparent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TableItem::setItem(const Widget::Ptr& widgetPtr, HorizontalAlign align)
    {
        if (m_widget != nullptr)
            remove(m_widget);

        m_widget = widgetPtr;
        m_align = align;
        updateItem();

        if (m_widget != nullptr)
            add(m_widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TableItem::setHorizontalAlign(HorizontalAlign align)
    {
        m_align = align;
        updateItem();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TableItem::setFont(const Font& font)
    {
        m_widget->setFont(font);
        updateItem();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TableItem::setSize(const Layout2d& size)
    {
        Panel::setSize(size);
        updateItem();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TableItem::updateItem()
    {
        if (m_widget != nullptr)
        {
            if (m_align == Left)
                m_widget->setPosition(m_widget->getWidgetOffset().x, m_widget->getWidgetOffset().y + (getSize().y - m_widget->getFullSize().y) / 2.f);
            else if (m_align == Center)
                m_widget->setPosition(m_widget->getWidgetOffset().x + (getSize().x - m_widget->getSize().x) / 2.f, m_widget->getWidgetOffset().y + (getSize().y - m_widget->getSize().y) / 2.f);
            else
                m_widget->setPosition(m_widget->getWidgetOffset().x + getSize().x - m_widget->getSize().x, m_widget->getWidgetOffset().y + (getSize().y - m_widget->getSize().y) / 2.f);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
