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

    void SpriteSheet::setSize(float width, float height)
    {
        // Make sure that the picture was already loaded
        if (m_loaded == false)
            return;

        // Store the new size
        m_size.x = width;
        m_size.y = height;

        // Make sure the sprite has the correct size
        m_texture.sprite.setScale((m_size.x * m_columns) / m_texture.data->texture.getSize().x, (m_size.y * m_rows) / m_texture.data->texture.getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f SpriteSheet::getSize() const
    {
        if (m_loaded)
            return m_size;
        else
            return sf::Vector2f(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpriteSheet::setCells(unsigned int rows, unsigned int columns)
    {
        // Make sure that the picture was already loaded
        if (m_loaded == false)
            return;

        // You can't have 0 rows
        if (rows == 0)
            rows = 1;

        // You can't have 0 columns
        if (columns == 0)
            columns = 1;

        // Store the number of rows and columns
        m_rows = rows;
        m_columns = columns;

        // Make the correct part of the image visible
        m_texture.sprite.setTextureRect(sf::IntRect((m_visibleCell.x-1) * m_texture.data->texture.getSize().x / m_columns,
                                                    (m_visibleCell.y-1) * m_texture.data->texture.getSize().y / m_rows,
                                                    static_cast<int>(m_texture.data->texture.getSize().x / m_columns),
                                                    static_cast<int>(m_texture.data->texture.getSize().y / m_rows)));

        // Make sure the sprite has the correct size
        m_texture.sprite.setScale((m_size.x * m_columns) / m_texture.data->texture.getSize().x, (m_size.y * m_rows) / m_texture.data->texture.getSize().y);
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
        // Make sure that the picture was already loaded
        if (m_loaded == false)
            return;

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
        m_texture.sprite.setTextureRect(sf::IntRect((m_visibleCell.x-1) * m_texture.data->texture.getSize().x / m_columns,
                                                    (m_visibleCell.y-1) * m_texture.data->texture.getSize().y / m_rows,
                                                    static_cast<int>(m_texture.data->texture.getSize().x / m_columns),
                                                    static_cast<int>(m_texture.data->texture.getSize().y / m_rows)));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2u SpriteSheet::getVisibleCell() const
    {
        return m_visibleCell;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SpriteSheet::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "rows")
        {
            setRows(std::stoi(value));
        }
        else if (property == "columns")
        {
            setColumns(std::stoi(value));
        }
        else // The property didn't match
            return Picture::setProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SpriteSheet::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "rows")
            value = std::to_string(getRows());
        else if (property == "columns")
            value = std::to_string(getColumns());
        else // The property didn't match
            return Picture::getProperty(property, value);

        // You pass here when one of the properties matched
        return true;
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
