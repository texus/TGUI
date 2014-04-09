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


#include <TGUI/SpriteSheet.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpriteSheet::SpriteSheet() :
    m_rows       (1),
    m_columns    (1),
    m_visibleCell(1, 1)
    {
        m_callback.widgetType = Type_SpriteSheet;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpriteSheet::SpriteSheet(const SpriteSheet& copy) :
    Picture      (copy),
    m_rows       (copy.m_rows),
    m_columns    (copy.m_columns),
    m_visibleCell(copy.m_visibleCell)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpriteSheet& SpriteSheet::operator= (const SpriteSheet& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            SpriteSheet temp(right);
            this->Picture::operator=(right);

            std::swap(m_rows,        temp.m_rows);
            std::swap(m_columns,     temp.m_columns);
            std::swap(m_visibleCell, temp.m_visibleCell);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpriteSheet* SpriteSheet::clone()
    {
        return new SpriteSheet(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpriteSheet::setPosition(float x, float y)
    {
        Transformable::setPosition(x, y);

        m_texture.setPosition(getPosition().x - ((m_visibleCell.x-1) * m_texture.getSize().x / m_columns),
                              getPosition().y - ((m_visibleCell.y-1) * m_texture.getSize().y / m_rows));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpriteSheet::setSize(float width, float height)
    {
        m_texture.setSize(width * m_columns, height * m_rows);

        // Make the correct part of the image visible
        m_texture.setTextureRect(sf::FloatRect((m_visibleCell.x-1) * m_texture.getSize().x / m_columns,
                                               (m_visibleCell.y-1) * m_texture.getSize().y / m_rows,
                                               static_cast<int>(m_texture.getSize().x / m_columns),
                                               static_cast<int>(m_texture.getSize().y / m_rows)));

        // Make sure the image is displayed at the correct position
        m_texture.setPosition(getPosition().x - ((m_visibleCell.x-1) * m_texture.getSize().x / m_columns),
                              getPosition().y - ((m_visibleCell.y-1) * m_texture.getSize().y / m_rows));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f SpriteSheet::getSize() const
    {
        return m_texture.getSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpriteSheet::setCells(unsigned int rows, unsigned int columns)
    {
        // You can't have 0 rows
        if (rows == 0)
            rows = 1;

        // You can't have 0 columns
        if (columns == 0)
            columns = 1;

        // Make sure the sprite has the correct size
        m_texture.setSize(m_texture.getSize().x / m_columns * columns, m_texture.getSize().y / m_rows * rows);

        // Store the number of rows and columns
        m_rows = rows;
        m_columns = columns;

        // Make the correct part of the image visible
        m_texture.setTextureRect(sf::FloatRect((m_visibleCell.x-1) * m_texture.getSize().x / m_columns,
                                               (m_visibleCell.y-1) * m_texture.getSize().y / m_rows,
                                               static_cast<int>(m_texture.getSize().x / m_columns),
                                               static_cast<int>(m_texture.getSize().y / m_rows)));

        // Make sure the image is displayed at the correct position
        m_texture.setPosition(getPosition().x - ((m_visibleCell.x-1) * m_texture.getSize().x / m_columns),
                              getPosition().y - ((m_visibleCell.y-1) * m_texture.getSize().y / m_rows));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpriteSheet::setRows(unsigned int rows)
    {
        setCells(rows, m_columns);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SpriteSheet::getRows() const
    {
        return m_rows;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpriteSheet::setColumns(unsigned int columns)
    {
        setCells(m_rows, columns);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SpriteSheet::getColumns() const
    {
        return m_columns;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpriteSheet::setVisibleCell(unsigned int row, unsigned int column)
    {
        // You can't make a row visible that doesn't exist
        if (row > m_rows)
            row = m_rows;
        else if (row == 0)
            row = 1;

        // You can't make a column visible that doesn't exist
        if (column > m_columns)
            column = m_columns;
        else if (column == 0)
            column = 1;

        // store the visible cell
        m_visibleCell.x = column;
        m_visibleCell.y = row;

        // Make the correct part of the image visible
        m_texture.setTextureRect(sf::FloatRect((m_visibleCell.x-1) * m_texture.getSize().x / m_columns,
                                               (m_visibleCell.y-1) * m_texture.getSize().y / m_rows,
                                               static_cast<int>(m_texture.getSize().x / m_columns),
                                               static_cast<int>(m_texture.getSize().y / m_rows)));

        // Make sure the image is displayed at the correct position
        m_texture.setPosition(getPosition().x - ((m_visibleCell.x-1) * m_texture.getSize().x / m_columns),
                              getPosition().y - ((m_visibleCell.y-1) * m_texture.getSize().y / m_rows));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2u SpriteSheet::getVisibleCell() const
    {
        return m_visibleCell;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpriteSheet::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "rows")
        {
            setRows(tgui::stoi(value));
        }
        else if (property == "columns")
        {
            setColumns(tgui::stoi(value));
        }
        else // The property didn't match
            Picture::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpriteSheet::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "rows")
            value = tgui::to_string(getRows());
        else if (property == "columns")
            value = tgui::to_string(getColumns());
        else // The property didn't match
            Picture::getProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::list< std::pair<std::string, std::string> > SpriteSheet::getPropertyList() const
    {
        auto list = Picture::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("Rows", "uint"));
        list.push_back(std::pair<std::string, std::string>("Columns", "uint"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
