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


#include <TGUI/Widgets/devel/TableRow.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TableRow::TableRow()
    {
        m_callback.widgetType = "TableRow";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TableRow::setItem(unsigned int column, const std::string& name, TableItem::HorizontalAlign align)
    {
        BoxLayout::insert(column, createItem(name, align, m_normalTextColor));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TableRow::setItem(unsigned int column, const std::string& name, const sf::Color& color, TableItem::HorizontalAlign align)
    {
        BoxLayout::insert(column, createItem(name, align, color));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TableRow::addItem(const std::string& name)
    {
        BoxLayout::add(createItem(name, m_align, m_normalTextColor));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TableRow::addItem(const std::string& name, const sf::Color& color)
    {
        BoxLayout::add(createItem(name, m_align, color));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TableRow::addItems(const std::vector<std::string>& columns, const sf::Color& color, TableItem::HorizontalAlign align)
    {
        for (std::string column : columns)
            BoxLayout::add(createItem(column, align, color));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TableRow::insert(std::size_t index, const tgui::Widget::Ptr& widget, TableItem::HorizontalAlign align, const sf::String& widgetName)
    {
        return HorizontalLayout::insert(index, createItem(widget, align), widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TableRow::add(const tgui::Widget::Ptr& widget, const sf::String& widgetName)
    {
        HorizontalLayout::insert(m_widgets.size(), createItem(widget, m_align), widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TableRow::add(const tgui::Widget::Ptr& widget, TableItem::HorizontalAlign align, const sf::String& widgetName)
    {
        HorizontalLayout::insert(m_widgets.size(), createItem(widget, align), widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TableRow::add(const tgui::Widget::Ptr& widget, bool fixedHeight, TableItem::HorizontalAlign align, const sf::String& widgetName)
    {
        HorizontalLayout::insert(m_widgets.size(),
                                 createItem(widget, (align == TableItem::None) ? m_align : align, fixedHeight),
                                 widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TableRow::setHoverBackgroundColor(const sf::Color& color)
    {
        m_hoverBackgroundColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TableRow::setNormalBackgroundColor(const sf::Color& color)
    {
        m_normalBackgroundColor = color;
        setBackgroundColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TableRow::setNormalTextColor(const sf::Color& color)
    {
        m_normalTextColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TableRow::setItemsHorizontalAlign(TableItem::HorizontalAlign align)
    {
        m_align = align;

        for (std::size_t i = 0; i < m_widgets.size(); ++i)
            std::dynamic_pointer_cast<TableItem>(m_widgets[i])->setHorizontalAlign(align);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TableRow::mouseEnteredWidget()
    {
        BoxLayout::mouseEnteredWidget();
        if (m_hoverBackgroundColor != sf::Color::Transparent)
            setBackgroundColor(m_hoverBackgroundColor);
        else
            setBackgroundColor(m_normalBackgroundColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TableRow::mouseLeftWidget()
    {
        BoxLayout::mouseLeftWidget();
        setBackgroundColor(m_normalBackgroundColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TableItem::Ptr TableRow::createItem(const std::string& name, TableItem::HorizontalAlign align, const sf::Color& color)
    {
        auto label = std::make_shared<tgui::Label>();
        label->setText(name);
        label->setTextColor(color);

        auto layout = std::make_shared<tgui::TableItem>();
        layout->setItem(label, (align == TableItem::None) ? m_align : align);

        return layout;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TableItem::Ptr TableRow::createItem(const tgui::Widget::Ptr& widget, TableItem::HorizontalAlign align, bool fixedHeight)
    {
        if (fixedHeight && widget->getFullSize().y > m_customHeight)
            m_customHeight = widget->getFullSize().y;

        auto layout = std::make_shared<tgui::TableItem>();
        layout->setItem(widget, align);

        return layout;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
