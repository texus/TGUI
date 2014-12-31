/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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
    m_Rows       (1),
    m_Columns    (1),
    m_VisibleCell(1, 1)
    {
        m_Callback.widgetType = Type_SpriteSheet;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpriteSheet::SpriteSheet(const SpriteSheet& copy) :
    Picture      (copy),
    m_Rows       (copy.m_Rows),
    m_Columns    (copy.m_Columns),
    m_VisibleCell(copy.m_VisibleCell)
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

            std::swap(m_Rows,        temp.m_Rows);
            std::swap(m_Columns,     temp.m_Columns);
            std::swap(m_VisibleCell, temp.m_VisibleCell);
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
        if (m_Loaded == false)
            return;

        // Store the new size
        m_Size.x = width;
        m_Size.y = height;

        // Make sure the sprite has the correct size
        m_Texture.sprite.setScale((m_Size.x * m_Columns) / m_Texture.data->texture.getSize().x, (m_Size.y * m_Rows) / m_Texture.data->texture.getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f SpriteSheet::getSize() const
    {
        if (m_Loaded)
            return m_Size;
        else
            return sf::Vector2f(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpriteSheet::setCells(unsigned int rows, unsigned int columns)
    {
        // Make sure that the picture was already loaded
        if (m_Loaded == false)
            return;

        // You can't have 0 rows
        if (rows == 0)
            rows = 1;

        // You can't have 0 columns
        if (columns == 0)
            columns = 1;

        // Store the number of rows and columns
        m_Rows = rows;
        m_Columns = columns;

        // Make the correct part of the image visible
        m_Texture.sprite.setTextureRect(sf::IntRect((m_VisibleCell.x-1) * m_Texture.data->texture.getSize().x / m_Columns,
                                                    (m_VisibleCell.y-1) * m_Texture.data->texture.getSize().y / m_Rows,
                                                    static_cast<int>(m_Texture.data->texture.getSize().x / m_Columns),
                                                    static_cast<int>(m_Texture.data->texture.getSize().y / m_Rows)));

        // Make sure the sprite has the correct size
        m_Texture.sprite.setScale((m_Size.x * m_Columns) / m_Texture.data->texture.getSize().x, (m_Size.y * m_Rows) / m_Texture.data->texture.getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpriteSheet::setRows(unsigned int rows)
    {
        setCells(rows, m_Columns);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SpriteSheet::getRows() const
    {
        return m_Rows;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpriteSheet::setColumns(unsigned int columns)
    {
        setCells(m_Rows, columns);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int SpriteSheet::getColumns() const
    {
        return m_Columns;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpriteSheet::setVisibleCell(unsigned int row, unsigned int column)
    {
        // Make sure that the picture was already loaded
        if (m_Loaded == false)
            return;

        // You can't make a row visible that doesn't exist
        if (row > m_Rows)
            row = m_Rows;
        else if (row == 0)
            row = 1;

        // You can't make a column visible that doesn't exist
        if (column > m_Columns)
            column = m_Columns;
        else if (column == 0)
            column = 1;

        // store the visible cell
        m_VisibleCell.x = column;
        m_VisibleCell.y = row;

        // Make the correct part of the image visible
        m_Texture.sprite.setTextureRect(sf::IntRect((m_VisibleCell.x-1) * m_Texture.data->texture.getSize().x / m_Columns,
                                                    (m_VisibleCell.y-1) * m_Texture.data->texture.getSize().y / m_Rows,
                                                    static_cast<int>(m_Texture.data->texture.getSize().x / m_Columns),
                                                    static_cast<int>(m_Texture.data->texture.getSize().y / m_Rows)));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2u SpriteSheet::getVisibleCell() const
    {
        return m_VisibleCell;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SpriteSheet::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "rows")
        {
            setRows(atoi(value.c_str()));
        }
        else if (property == "columns")
        {
            setColumns(atoi(value.c_str()));
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
            value = to_string(getRows());
        else if (property == "columns")
            value = to_string(getColumns());
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
