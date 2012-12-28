/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (VDV_B@hotmail.com)
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


#include <TGUI/TGUI.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Grid()
    {
        m_ObjectType = grid;

        // There is nothing to load
        m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Grid(const Grid& gridToCopy) :
    GroupObject(gridToCopy),
    m_Size     (gridToCopy.m_Size)
    {
        for (unsigned int row=0; row<gridToCopy.m_GridObjects.size(); ++row)
        {
            // Create a new row
            addRow();

            for (unsigned int col=0; col<gridToCopy.m_GridObjects[row].size(); ++col)
            {
                // Find the object that belongs in this sqare
                for (unsigned int i=0; i<gridToCopy.m_EventManager.m_Objects.size(); ++i)
                {
                    // If an object matches then add it to the grid
                    if (gridToCopy.m_EventManager.m_Objects[i] == gridToCopy.m_GridObjects[row][col])
                        addToRow(m_EventManager.m_Objects[i]);
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

    void Grid::initialize()
    {
        globalFont = m_Parent->globalFont;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid* Grid::clone()
    {
        return new Grid(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::setSize(float width, float height)
    {
        // If the size was reset to 0 then there should be no more scaling
        if ((width == 0) && (height == 0))
        {
            // Reset the scale
            setScale(1, 1);
        }
        else // A size was given
        {
            // Fake the size to make the correct calculation
            m_Size.x = 0;
            m_Size.y = 0;

            // Set the new scaling factors
            Vector2u size = getSize();
            setScale(width / size.x, height / size.y);
        }

        // Store the new size
        m_Size.x = static_cast<unsigned int>(width);
        m_Size.y = static_cast<unsigned int>(height);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u Grid::getSize() const
    {
        // Check if there is no fixed size
        if ((m_Size.x == 0) && (m_Size.y == 0))
        {
            // If the grid is empty the nothing can be calculated
            if (m_GridObjects.empty())
                return Vector2u(0, 0);

            Vector2u size;
            unsigned int largestRow = 0;
            unsigned int mostColumns = 0;

            // Loop through all rows
            for (unsigned int row=0; row<m_GridObjects.size(); ++row)
            {
                // Add the height of this row
                size.y += m_RowHeight[row];

                // Find out if this row contains the most columns
                if (m_GridObjects[row].size() > mostColumns)
                {
                    largestRow = row;
                    mostColumns = m_GridObjects[row].size();
                }
            }

            // Loop through all columns of the largest row
            for (unsigned int col=0; col<m_GridObjects[largestRow].size(); ++col)
                size.x += m_ColumnWidth[col];

            // Return the size of the grid
            return size;
        }
        else // There is a fixed size
            return m_Size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Grid::getScaledSize() const
    {
        // Check if there is no fixed size
        if ((m_Size.x == 0) && (m_Size.y == 0))
        {
            Vector2u size = getSize();
            return Vector2f(size.x * getScale().x, size.y * getScale().y);
        }
        else // There is a fixed size
        {
            // If the grid is empty the nothing can be calculated
            if (m_GridObjects.empty())
                return Vector2f(0, 0);

            Vector2u size;
            unsigned int largestRow = 0;
            unsigned int mostColumns = 0;

            // Loop through all rows
            for (unsigned int row=0; row<m_GridObjects.size(); ++row)
            {
                // Add the height of this row
                size.y += m_RowHeight[row];

                // Find out if this row contains the most columns
                if (m_GridObjects[row].size() > mostColumns)
                {
                    largestRow = row;
                    mostColumns = m_GridObjects[row].size();
                }
            }

            // Loop through all columns of the largest row
            for (unsigned int col=0; col<m_GridObjects[largestRow].size(); ++col)
                size.x += m_ColumnWidth[col];

            // Return the size of the grid
            return Vector2f(size.x * getScale().x, size.y * getScale().y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::remove(OBJECT* object)
    {
        // Find the object in the grid
        for (unsigned int row=0; row < m_GridObjects.size(); ++row)
        {
            for (unsigned int col=0; col < m_GridObjects[row].size(); ++col)
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
                        for (unsigned int i=0; i<m_GridObjects.size(); ++i)
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
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::addToRow(OBJECT* const object, const Vector4u& borders, Layout::layouts layout)
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

        // Recalculate the scale factors if needed
        if ((m_Size.x > 0) || (m_Size.y > 0))
            setSize(static_cast<float>(m_Size.x), static_cast<float>(m_Size.y));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::addRow(unsigned int rowHeight)
    {
        m_GridObjects.push_back( std::vector<OBJECT*>() );
        m_ObjBorders.push_back( std::vector<Vector4u>() );
        m_ObjLayout.push_back( std::vector<Layout::layouts>() );
        m_RowHeight.push_back(rowHeight);

        // Recalculate the scale factors if needed
        if ((m_Size.x > 0) || (m_Size.y > 0))
            setSize(static_cast<float>(m_Size.x), static_cast<float>(m_Size.y));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::updateObjects()
    {
        // Recalculate the scale factors if needed
        if ((m_Size.x > 0) || (m_Size.y > 0))
            setSize(static_cast<float>(m_Size.x), static_cast<float>(m_Size.y));

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
                if (m_ColumnWidth[col] < m_GridObjects[row][col]->getScaledSize().x + m_ObjBorders[row][col].x1 + m_ObjBorders[row][col].x3)
                    m_ColumnWidth[col] = static_cast<unsigned int>(m_GridObjects[row][col]->getScaledSize().x + m_ObjBorders[row][col].x1 + m_ObjBorders[row][col].x3);

                // Remember the biggest row height
                if (m_RowHeight[row] < m_GridObjects[row][col]->getScaledSize().y + m_ObjBorders[row][col].x2 + m_ObjBorders[row][col].x4)
                    m_RowHeight[row] = static_cast<unsigned int>(m_GridObjects[row][col]->getScaledSize().y + m_ObjBorders[row][col].x2 + m_ObjBorders[row][col].x4);
            }
        }

        // Reposition all objects
        updatePositionsOfAllObjects();

        // Recalculate the scale factors if needed
        if ((m_Size.x > 0) || (m_Size.y > 0))
            setSize(static_cast<float>(m_Size.x), static_cast<float>(m_Size.y));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::changeObjectLayout(const OBJECT* const object, Layout::layouts layout)
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

                    // Update the draw borders of the object
                    updatePosition(row, col);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::updatePosition(unsigned int row, unsigned int col)
    {
        float left = 0;
        float top = 0;

        for (unsigned int i=0; i < row; ++i)
            top += static_cast<float>(m_RowHeight[i]);

        for (unsigned int i=0; i < col; ++i)
            left += static_cast<float>(m_ColumnWidth[i]);

        // TODO: Support more layouts
        if (m_ObjLayout[row][col] == Layout::TopLeft)
        {
            left += static_cast<float>(m_ObjBorders[row][col].x1);
            top += static_cast<float>(m_ObjBorders[row][col].x2);
        }
        else if (m_ObjLayout[row][col] == Layout::Center)
        {
            left += m_ObjBorders[row][col].x1 + ((m_ColumnWidth[col] - m_ObjBorders[row][col].x1 - m_ObjBorders[row][col].x3) - m_GridObjects[row][col]->getScaledSize().x) / 2.f;
            top += m_ObjBorders[row][col].x2 + ((m_RowHeight[row] - m_ObjBorders[row][col].x2 - m_ObjBorders[row][col].x4) - m_GridObjects[row][col]->getScaledSize().y) / 2.f;
        }

        m_GridObjects[row][col]->setPosition(left, top);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::updatePositionsOfAllObjects()
    {
        Vector2f position;
        Vector2f previousPosition;

        // Loop through all rows
        for (unsigned int row=0; row < m_GridObjects.size(); ++row)
        {
            // Remember the current position
            previousPosition = position;

            // Loop through all objects in the row
            for (unsigned int col=0; col < m_GridObjects[row].size(); ++col)
            {
                // Place the next object on the correct position
                // TODO: Support more layouts
                if (m_ObjLayout[row][col] == Layout::TopLeft)
                {
                    position.x += static_cast<float>(m_ObjBorders[row][col].x1);
                    position.y += static_cast<float>(m_ObjBorders[row][col].x2);

                    m_GridObjects[row][col]->setPosition(position);

                    position.x -= static_cast<float>(m_ObjBorders[row][col].x1 - m_ColumnWidth[col]);
                    position.y -= static_cast<float>(m_ObjBorders[row][col].x2);
                }
                else if (m_ObjLayout[row][col] == Layout::Center)
                {
                    position.x += m_ObjBorders[row][col].x1 + ((m_ColumnWidth[col] - m_ObjBorders[row][col].x1 - m_ObjBorders[row][col].x3) - m_GridObjects[row][col]->getScaledSize().x) / 2.f;
                    position.y += m_ObjBorders[row][col].x2 + ((m_RowHeight[row] - m_ObjBorders[row][col].x2 - m_ObjBorders[row][col].x4) - m_GridObjects[row][col]->getScaledSize().y) / 2.f;

                    m_GridObjects[row][col]->setPosition(position);

                    position.x -= m_ObjBorders[row][col].x1 + ((m_ColumnWidth[col] - m_ObjBorders[row][col].x1 - m_ObjBorders[row][col].x3) - m_GridObjects[row][col]->getScaledSize().x) / 2.f - m_ColumnWidth[col];
                    position.y -= m_ObjBorders[row][col].x2 + ((m_RowHeight[row] - m_ObjBorders[row][col].x2 - m_ObjBorders[row][col].x4) - m_GridObjects[row][col]->getScaledSize().y) / 2.f;
                }
            }

            // Go to the next row
            position = previousPosition;
            position.y += m_RowHeight[row];
        }
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
            // Get the size of the grid
            Vector2f size = getScaledSize();

            // Check if the mouse is on the grid
            if ((x < position.x + size.x) && (y < position.y + size.y))
                return true;
        }

        // Tell the objects inside the grid that the mouse is no longer on top of them
        m_EventManager.mouseNotOnObject();
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

    void Grid::addCallback(const Callback& callback)
    {
        // Pass the callback to the parent. It has to get to the main window eventually.
        m_Parent->addCallback(callback);
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
