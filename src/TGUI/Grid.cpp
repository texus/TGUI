/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2013 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Objects.hpp>
#include <TGUI/GroupObject.hpp>
#include <TGUI/Grid.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Grid()
    {
        m_Callback.objectType = Type_Grid;

        // There is nothing to load
        m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Grid(const Grid& copy) :
    GroupObject(copy),
    m_Size     (copy.m_Size)
    {
        const std::vector<Object::Ptr>& objects = copy.m_EventManager.m_Objects;

        for (unsigned int row = 0; row < copy.m_GridObjects.size(); ++row)
        {
            // Create a new row
            addRow();

            for (unsigned int col = 0; col < copy.m_GridObjects[row].size(); ++col)
            {
                // Find the object that belongs in this square
                for (unsigned int i = 0; i < objects.size(); ++i)
                {
                    // If an object matches then add it to the grid
                    if (objects[i] == copy.m_GridObjects[row][col])
                        addToRow(objects[i]);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::~Grid()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid& Grid::operator= (const Grid& right)
    {
        // Make sure it is not the same object
        if (this != &right)
        {
            Grid temp(right);
            this->GroupObject::operator=(right);

            std::swap(m_GridObjects, temp.m_GridObjects);
            std::swap(m_ObjBorders,  temp.m_ObjBorders);
            std::swap(m_ObjLayout,   temp.m_ObjLayout);
            std::swap(m_RowHeight,   temp.m_RowHeight);
            std::swap(m_ColumnWidth, temp.m_ColumnWidth);
            std::swap(m_Size,        temp.m_Size);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid* Grid::clone()
    {
        return new Grid(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Grid::getSize() const
    {
        return m_Size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::remove(const Object::Ptr& object)
    {
        remove(object.get());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::remove(Object* object)
    {
        // Find the object in the grid
        for (unsigned int row = 0; row < m_GridObjects.size(); ++row)
        {
            for (unsigned int col = 0; col < m_GridObjects[row].size(); ++col)
            {
                if (m_GridObjects[row][col] == object)
                {
                    // Remove the object from the grid
                    m_GridObjects[row].erase(m_GridObjects[row].begin() + col);
                    m_ObjBorders[row].erase(m_ObjBorders[row].begin() + col);
                    m_ObjLayout[row].erase(m_ObjLayout[row].begin() + col);

                    // Check if this is the last column
                    if (m_ColumnWidth.size() == m_GridObjects[row].size() + 1)
                    {
                        // Check if there is another row with this many columns
                        bool rowFound = false;
                        for (unsigned int i = 0; i < m_GridObjects.size(); ++i)
                        {
                            if (m_GridObjects[i].size() >= m_ColumnWidth.size())
                            {
                                rowFound = true;
                                break;
                            }
                        }

                        // Erase the last column if no other row is using it
                        if (rowFound == false)
                            m_ColumnWidth.erase(m_ColumnWidth.end()-1);
                    }

                    // If the row is empty then remove it as well
                    if (m_GridObjects[row].empty())
                    {
                        m_GridObjects.erase(m_GridObjects.begin() + row);
                        m_ObjBorders.erase(m_ObjBorders.begin() + row);
                        m_ObjLayout.erase(m_ObjLayout.begin() + row);
                        m_RowHeight.erase(m_RowHeight.begin() + row);
                    }

                    // Update the positions of all remaining objects
                    updatePositionsOfAllObjects();
                }
            }
        }

        Group::remove(object);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::removeAllObjects()
    {
        m_GridObjects.clear();
        m_ObjBorders.clear();
        m_ObjLayout.clear();

        m_RowHeight.clear();
        m_ColumnWidth.clear();

        Group::removeAllObjects();

        m_Size.x = 0;
        m_Size.y = 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::addToRow(const Object::Ptr& object, const Vector4u& borders, Layout::Layouts layout)
    {
        // If there is no row yet then create one
        if (m_GridObjects.empty())
            addRow(0);

        // Add the object to the grid
        m_GridObjects[m_GridObjects.size()-1].push_back(object);
        m_ObjBorders[m_ObjBorders.size()-1].push_back(borders);
        m_ObjLayout[m_ObjLayout.size()-1].push_back(layout);

        // If this is the first row to have so many columns then reserve some space for it
        if (m_ColumnWidth.size() < m_GridObjects[m_ObjLayout.size()-1].size())
            m_ColumnWidth.push_back(0);

        // Update the objects
        updateObjects();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::addRow(unsigned int rowHeight)
    {
        m_GridObjects.push_back( std::vector<Object::Ptr>() );
        m_ObjBorders.push_back( std::vector<Vector4u>() );
        m_ObjLayout.push_back( std::vector<Layout::Layouts>() );
        m_RowHeight.push_back(rowHeight);

        m_Size.y += static_cast<float>(rowHeight);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::updateObjects()
    {
        // Reset the column widths
        for (std::vector<unsigned int>::iterator it=m_ColumnWidth.begin(); it!=m_ColumnWidth.end(); ++it)
            *it = 0;

        // Loop through all objects
        for (unsigned int row=0; row < m_GridObjects.size(); ++row)
        {
            // Reset the row height
            m_RowHeight[row] = 0;

            for (unsigned int col=0; col < m_GridObjects[row].size(); ++col)
            {
                // Remember the biggest column width
                if (m_ColumnWidth[col] < m_GridObjects[row][col]->getSize().x + m_ObjBorders[row][col].x1 + m_ObjBorders[row][col].x3)
                    m_ColumnWidth[col] = static_cast<unsigned int>(m_GridObjects[row][col]->getSize().x + m_ObjBorders[row][col].x1 + m_ObjBorders[row][col].x3);

                // Remember the biggest row height
                if (m_RowHeight[row] < m_GridObjects[row][col]->getSize().y + m_ObjBorders[row][col].x2 + m_ObjBorders[row][col].x4)
                    m_RowHeight[row] = static_cast<unsigned int>(m_GridObjects[row][col]->getSize().y + m_ObjBorders[row][col].x2 + m_ObjBorders[row][col].x4);
            }
        }

        // Reposition all objects
        updatePositionsOfAllObjects();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::changeObjectLayout(const Object::Ptr& object, Layout::Layouts layout)
    {
        // Find the object in the grid
        for (unsigned int row=0; row < m_GridObjects.size(); ++row)
        {
            for (unsigned int col=0; col < m_GridObjects[row].size(); ++col)
            {
                if (m_GridObjects[row][col] == object)
                {
                    // Change the layout of the object
                    m_ObjLayout[row][col] = layout;

                    // Recalculate the position of the object
                    {
                        float left = 0;
                        float top = 0;

                        for (unsigned int i=0; i < row; ++i)
                            top += static_cast<float>(m_RowHeight[i]);

                        for (unsigned int i=0; i < col; ++i)
                            left += static_cast<float>(m_ColumnWidth[i]);

                        switch (m_ObjLayout[row][col])
                        {
                        case Layout::UpperLeft:
                            left += static_cast<float>(m_ObjBorders[row][col].x1);
                            top += static_cast<float>(m_ObjBorders[row][col].x2);
                            break;

                        case Layout::Up:
                            left += m_ObjBorders[row][col].x1 + (((m_ColumnWidth[col] - m_ObjBorders[row][col].x1 - m_ObjBorders[row][col].x3) - m_GridObjects[row][col]->getSize().x) / 2.f);
                            top += static_cast<float>(m_ObjBorders[row][col].x2);
                            break;

                        case Layout::UpperRight:
                            left += m_ColumnWidth[col] - m_ObjBorders[row][col].x3 - m_GridObjects[row][col]->getSize().x;
                            top += static_cast<float>(m_ObjBorders[row][col].x2);
                            break;

                        case Layout::Right:
                            left += m_ColumnWidth[col] - m_ObjBorders[row][col].x3 - m_GridObjects[row][col]->getSize().x;
                            top += m_ObjBorders[row][col].x2 + (((m_RowHeight[row] - m_ObjBorders[row][col].x2 - m_ObjBorders[row][col].x4) - m_GridObjects[row][col]->getSize().y) / 2.f);
                            break;

                        case Layout::BottomRight:
                            left += m_ColumnWidth[col] - m_ObjBorders[row][col].x3 - m_GridObjects[row][col]->getSize().x;
                            top += m_RowHeight[row] - m_ObjBorders[row][col].x4 - m_GridObjects[row][col]->getSize().y;
                            break;

                        case Layout::Bottom:
                            left += m_ObjBorders[row][col].x1 + (((m_ColumnWidth[col] - m_ObjBorders[row][col].x1 - m_ObjBorders[row][col].x3) - m_GridObjects[row][col]->getSize().x) / 2.f);
                            top += m_RowHeight[row] - m_ObjBorders[row][col].x4 - m_GridObjects[row][col]->getSize().y;
                            break;

                        case Layout::BottomLeft:
                            left += static_cast<float>(m_ObjBorders[row][col].x1);
                            top += m_RowHeight[row] - m_ObjBorders[row][col].x4 - m_GridObjects[row][col]->getSize().y;
                            break;

                        case Layout::Left:
                            left += static_cast<float>(m_ObjBorders[row][col].x1);
                            top += m_ObjBorders[row][col].x2 + (((m_RowHeight[row] - m_ObjBorders[row][col].x2 - m_ObjBorders[row][col].x4) - m_GridObjects[row][col]->getSize().y) / 2.f);
                            break;

                        case Layout::Center:
                            left += m_ObjBorders[row][col].x1 + (((m_ColumnWidth[col] - m_ObjBorders[row][col].x1 - m_ObjBorders[row][col].x3) - m_GridObjects[row][col]->getSize().x) / 2.f);
                            top += m_ObjBorders[row][col].x2 + (((m_RowHeight[row] - m_ObjBorders[row][col].x2 - m_ObjBorders[row][col].x4) - m_GridObjects[row][col]->getSize().y) / 2.f);
                            break;
                        }

                        m_GridObjects[row][col]->setPosition(left, top);
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::updatePositionsOfAllObjects()
    {
        Vector2f position;
        Vector2f previousPosition;

        // Reset the grid size (will be recalculated in this function)
        m_Size.x = 0;
        m_Size.y = 0;

        // Loop through all rows
        for (unsigned int row=0; row < m_GridObjects.size(); ++row)
        {
            // Remember the current position
            previousPosition = position;

            // Loop through all objects in the row
            for (unsigned int col=0; col < m_GridObjects[row].size(); ++col)
            {
                // Place the next object on the correct position
                switch (m_ObjLayout[row][col])
                {
                case Layout::UpperLeft:
                    position.x += static_cast<float>(m_ObjBorders[row][col].x1);
                    position.y += static_cast<float>(m_ObjBorders[row][col].x2);

                    m_GridObjects[row][col]->setPosition(position);

                    position.x -= static_cast<float>(m_ObjBorders[row][col].x1) - m_ColumnWidth[col];
                    position.y -= static_cast<float>(m_ObjBorders[row][col].x2);
                    break;

                case Layout::Up:
                    position.x += m_ObjBorders[row][col].x1 + (((m_ColumnWidth[col] - m_ObjBorders[row][col].x1 - m_ObjBorders[row][col].x3) - m_GridObjects[row][col]->getSize().x) / 2.f);
                    position.y += static_cast<float>(m_ObjBorders[row][col].x2);

                    m_GridObjects[row][col]->setPosition(position);

                    position.x -= m_ObjBorders[row][col].x1 + (((m_ColumnWidth[col] - m_ObjBorders[row][col].x1 - m_ObjBorders[row][col].x3) - m_GridObjects[row][col]->getSize().x) / 2.f) - m_ColumnWidth[col];
                    position.y -= static_cast<float>(m_ObjBorders[row][col].x2);
                    break;

                case Layout::UpperRight:
                    position.x += m_ColumnWidth[col] - m_ObjBorders[row][col].x3 - m_GridObjects[row][col]->getSize().x;
                    position.y += static_cast<float>(m_ObjBorders[row][col].x2);

                    m_GridObjects[row][col]->setPosition(position);

                    position.x -= m_ColumnWidth[col] - m_ObjBorders[row][col].x3 - m_GridObjects[row][col]->getSize().x - m_ColumnWidth[col];
                    position.y -= static_cast<float>(m_ObjBorders[row][col].x2);
                    break;

                case Layout::Right:
                    position.x += m_ColumnWidth[col] - m_ObjBorders[row][col].x3 - m_GridObjects[row][col]->getSize().x;
                    position.y += m_ObjBorders[row][col].x2 + (((m_RowHeight[row] - m_ObjBorders[row][col].x2 - m_ObjBorders[row][col].x4) - m_GridObjects[row][col]->getSize().y) / 2.f);

                    m_GridObjects[row][col]->setPosition(position);

                    position.x -= m_ColumnWidth[col] - m_ObjBorders[row][col].x3 - m_GridObjects[row][col]->getSize().x - m_ColumnWidth[col];
                    position.y -= m_ObjBorders[row][col].x2 + (((m_RowHeight[row] - m_ObjBorders[row][col].x2 - m_ObjBorders[row][col].x4) - m_GridObjects[row][col]->getSize().y) / 2.f);
                    break;

                case Layout::BottomRight:
                    position.x += m_ColumnWidth[col] - m_ObjBorders[row][col].x3 - m_GridObjects[row][col]->getSize().x;
                    position.y += m_RowHeight[row] - m_ObjBorders[row][col].x4 - m_GridObjects[row][col]->getSize().y;

                    m_GridObjects[row][col]->setPosition(position);

                    position.x -= m_ColumnWidth[col] - m_ObjBorders[row][col].x3 - m_GridObjects[row][col]->getSize().x - m_ColumnWidth[col];
                    position.y -= m_RowHeight[row] - m_ObjBorders[row][col].x4 - m_GridObjects[row][col]->getSize().y;
                    break;

                case Layout::Bottom:
                    position.x += m_ObjBorders[row][col].x1 + (((m_ColumnWidth[col] - m_ObjBorders[row][col].x1 - m_ObjBorders[row][col].x3) - m_GridObjects[row][col]->getSize().x) / 2.f);
                    position.y += m_RowHeight[row] - m_ObjBorders[row][col].x4 - m_GridObjects[row][col]->getSize().y;

                    m_GridObjects[row][col]->setPosition(position);

                    position.x -= m_ObjBorders[row][col].x1 + (((m_ColumnWidth[col] - m_ObjBorders[row][col].x1 - m_ObjBorders[row][col].x3) - m_GridObjects[row][col]->getSize().x) / 2.f) - m_ColumnWidth[col];
                    position.y -= m_RowHeight[row] - m_ObjBorders[row][col].x4 - m_GridObjects[row][col]->getSize().y;
                    break;

                case Layout::BottomLeft:
                    position.x += static_cast<float>(m_ObjBorders[row][col].x1);
                    position.y += m_RowHeight[row] - m_ObjBorders[row][col].x4 - m_GridObjects[row][col]->getSize().y;

                    m_GridObjects[row][col]->setPosition(position);

                    position.x -= static_cast<float>(m_ObjBorders[row][col].x1) - m_ColumnWidth[col];
                    position.y -= m_RowHeight[row] - m_ObjBorders[row][col].x4 - m_GridObjects[row][col]->getSize().y;
                    break;

                case Layout::Left:
                    position.x += static_cast<float>(m_ObjBorders[row][col].x1);
                    position.y += m_ObjBorders[row][col].x2 + (((m_RowHeight[row] - m_ObjBorders[row][col].x2 - m_ObjBorders[row][col].x4) - m_GridObjects[row][col]->getSize().y) / 2.f);

                    m_GridObjects[row][col]->setPosition(position);

                    position.x -= static_cast<float>(m_ObjBorders[row][col].x1) - m_ColumnWidth[col];
                    position.y -= m_ObjBorders[row][col].x2 + (((m_RowHeight[row] - m_ObjBorders[row][col].x2 - m_ObjBorders[row][col].x4) - m_GridObjects[row][col]->getSize().y) / 2.f);
                    break;

                case Layout::Center:
                    position.x += m_ObjBorders[row][col].x1 + (((m_ColumnWidth[col] - m_ObjBorders[row][col].x1 - m_ObjBorders[row][col].x3) - m_GridObjects[row][col]->getSize().x) / 2.f);
                    position.y += m_ObjBorders[row][col].x2 + (((m_RowHeight[row] - m_ObjBorders[row][col].x2 - m_ObjBorders[row][col].x4) - m_GridObjects[row][col]->getSize().y) / 2.f);

                    m_GridObjects[row][col]->setPosition(position);

                    position.x -= m_ObjBorders[row][col].x1 + (((m_ColumnWidth[col] - m_ObjBorders[row][col].x1 - m_ObjBorders[row][col].x3) - m_GridObjects[row][col]->getSize().x) / 2.f) - m_ColumnWidth[col];
                    position.y -= m_ObjBorders[row][col].x2 + (((m_RowHeight[row] - m_ObjBorders[row][col].x2 - m_ObjBorders[row][col].x4) - m_GridObjects[row][col]->getSize().y) / 2.f);
                    break;
                }
            }

            // Add the height of this row to the size of the grid
            m_Size.y += static_cast<float>(m_RowHeight[row]);

            // Go to the next row
            position = previousPosition;
            position.y += m_RowHeight[row];
        }

        // Loop through all columns of the largest row to find the width of the grid
        for (std::vector<unsigned int>::const_iterator it = m_ColumnWidth.begin(); it != m_ColumnWidth.end(); ++it)
            m_Size.x += static_cast<float>(*it);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Grid::mouseOnObject(float x, float y)
    {
        // Don't continue when the child window has not been loaded yet
        if (m_Loaded == false)
            return false;

        // Get the current position
        Vector2f position = getPosition();

        // Check if the mouse might be on top of the grid
        if ((x > position.x) && (y > position.y))
        {
            // Check if the mouse is on the grid
            if ((x < position.x + m_Size.x) && (y < position.y + m_Size.y))
                return true;
        }

        if (m_MouseHover)
            mouseLeftObject();

        // Tell the objects inside the grid that the mouse is no longer on top of them
        m_EventManager.mouseNotOnObject();
        m_MouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::objectFocused()
    {
        m_EventManager.tabKeyPressed();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::objectUnfocused()
    {
        m_EventManager.unfocusAllObjects();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Set the transformation
        states.transform *= getTransform();

        // Draw all objects
        for (unsigned int row=0; row < m_GridObjects.size(); ++row)
        {
            for (unsigned int col=0; col < m_GridObjects[row].size(); ++col)
                target.draw(*m_GridObjects[row][col], states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
