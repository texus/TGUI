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


#include <TGUI/Widgets/devel/Table.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Table::Table()
    {
        m_callback.widgetType = "Table";

/// TODO
/*
        m_tableBorder.setFillColor(sf::Color::Transparent);
        m_tableBorder.setOutlineColor(sf::Color::Green);
        m_tableBorder.setOutlineThickness(1);

        m_headerSeparator.setFillColor(sf::Color::Green);
        //m_headerSeparator.setOutlineThickness(1);
*/
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Table::setFont(const Font& font)
    {
        BoxLayout::setFont(font);
        if (getFont())
            calculateLabelHeight();

        if (m_header)
            m_header->setFont(font);

        updateWidgetPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Table::setTextSize(unsigned int size)
    {
        m_characterSize = size;
        if (getFont())
            calculateLabelHeight();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Table::setSize(const Layout2d& size)
    {
        BoxLayout::setSize(size);
        m_headerSeparator.setSize({getSize().x, 1.f});
        m_tableBorder.setSize(getSize());

        updateColumnsDelimitatorsSize();

        for (auto& widget : m_widgets)
            widget->setSize(getSize().x, widget->getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Table::insert(std::size_t index, const tgui::Widget::Ptr& widget, const sf::String& widgetName)
    {
        auto row = std::dynamic_pointer_cast<TableRow>(widget);
        if (row == nullptr)
        {
            row = std::make_shared<tgui::TableRow>();
            row->add(widget);
        }

        for (std::size_t i = 0; i < m_header->getWidgets().size(); ++i)
        {
            row->setFixedSize(i, m_header->getFixedSize(i));
            row->setRatio(i, m_header->getRatio(i));
        }

        if (m_rowsEvenColor == sf::Color::Transparent)
            row->setNormalBackgroundColor(m_rowsOddColor);
        else if (index % 2 == 0)
            row->setNormalBackgroundColor(m_rowsEvenColor);
        else
            row->setNormalBackgroundColor(m_rowsOddColor);

        float customHeight = row->getCustomHeight();
        row->setSize(getSize().x, (customHeight > m_rowHeight) ? customHeight : m_rowHeight);

        return BoxLayout::insert(index, row, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Table::add(const tgui::Widget::Ptr& widget, const sf::String& widgetName)
    {
        insert(m_widgets.size(), widget, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Table::addRow(const std::vector<std::string>& columns)
    {
        auto row = std::make_shared<TableRow>();
        for (const std::string& column : columns)
            row->addItem(column, m_normalTextColor);

        insert(m_widgets.size(), row, "");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Table::setHeader(const tgui::TableRow::Ptr& row)
    {
        m_header = row;
        m_header->setParent(this);

        m_columnsDelimitators.resize(row->getWidgets().size() - 1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Table::setHeaderColumns(const std::vector<std::string>& columns)
    {
        m_header->removeAllWidgets();

        for (const std::string& column : columns)
            m_header->addItem(column, sf::Color::White);

        m_header->setNormalBackgroundColor({234,97,83});
        setHeader(m_header);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Table::setFixedColumnWidth(std::size_t column, float size)
    {
        m_header->setFixedSize(column, size);
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
            std::dynamic_pointer_cast<HorizontalLayout>(m_widgets[i])->setFixedSize(column, size);

        updateColumnsDelimitatorsPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Table::setColumnRatio(std::size_t column, float ratio)
    {
        m_header->setRatio(column, ratio);
        for (std::size_t i = 0; i < m_widgets.size(); ++i)
            std::dynamic_pointer_cast<HorizontalLayout>(m_widgets[i])->setRatio(column, ratio);

        updateColumnsDelimitatorsPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Table::setRowsColor(const sf::Color& color)
    {
        m_rowsOddColor = color;
        m_rowsEvenColor = sf::Color::Transparent;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Table::setStripesRowsColor(const sf::Color& evenColor, const sf::Color& oddColor)
    {
        m_rowsOddColor = oddColor;
        m_rowsEvenColor = evenColor;

        for (std::size_t i = 0; i < m_widgets.size(); ++i)
        {
            if (i % 2 == 0)
                std::dynamic_pointer_cast<TableRow>(m_widgets[i])->setNormalBackgroundColor(evenColor);
            else
                std::dynamic_pointer_cast<TableRow>(m_widgets[i])->setNormalBackgroundColor(oddColor);

            std::dynamic_pointer_cast<TableRow>(m_widgets[i])->setHoverBackgroundColor({255,255,0});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Table::setTextColor(const sf::Color& color)
    {
        m_normalTextColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Table::updateWidgetPositions()
    {
        if (m_header != nullptr)
        {
            m_header->setPosition(0, 0);
            m_header->setSize(getSize().x, m_rowHeight);
            updateColumnsDelimitatorsPosition();
            m_headerSeparator.setPosition(0, m_rowHeight);
        }

        if (m_widgets.empty())
            return;

        m_widgets[0]->setPosition(0, m_header->getSize().y + m_header->getPosition().y + 1.f);

        auto row = std::dynamic_pointer_cast<TableRow>(m_widgets[0]);
        float customHeight = row->getCustomHeight();
        row->setSize(getSize().x, (customHeight > m_rowHeight) ? customHeight : m_rowHeight);

        for (std::size_t i = 1; i < m_widgets.size(); ++i)
        {
            m_widgets[i]->setPosition(0, m_widgets[i-1]->getSize().y + m_widgets[i-1]->getPosition().y + 1.f);

            row = std::dynamic_pointer_cast<TableRow>(m_widgets[i]);
            customHeight = row->getCustomHeight();
            row->setSize(getSize().x, (customHeight > m_rowHeight) ? customHeight : m_rowHeight);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Table::calculateLabelHeight()
    {
        auto label = std::make_shared<Label>();
        label->setFont(getFont());
        label->setTextSize(m_characterSize);

        m_rowHeight = label->getSize().y;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Table::updateColumnsDelimitatorsPosition()
    {
        auto headerWidgets = m_header->getWidgets();
        for (std::size_t i = 1; i < headerWidgets.size(); ++i)
            m_columnsDelimitators[i-1].setPosition(headerWidgets[i]->getPosition().x, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Table::updateColumnsDelimitatorsSize()
    {
        for (std::size_t i = 0; i < m_columnsDelimitators.size(); ++i)
        {
            m_columnsDelimitators[i].setFillColor(sf::Color::Black);
            m_columnsDelimitators[i].setSize({1.f, getSize().y});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Table::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Set the position
        states.transform.translate(getPosition());

        // Draw the background
        if (m_backgroundColor != sf::Color::Transparent)
        {
            sf::RectangleShape background(getSize());
            background.setFillColor(m_backgroundColor);
            target.draw(background, states);
        }

        // Draw the widgets
        target.draw(*m_header, states);
        drawWidgetContainer(&target, states);

/// TODO
/*
        target.draw(m_tableBorder, states);
        //target.draw(m_headerSeparator, states);
*/
        for (std::size_t i = 0; i < m_columnsDelimitators.size(); ++i)
            target.draw(m_columnsDelimitators[i], states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
