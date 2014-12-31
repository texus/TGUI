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


#include <TGUI/SharedWidgetPtr.inl>
#include <TGUI/Grid.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Grid()
    {
        m_Callback.widgetType = Type_Grid;

        // There is nothing to load
        m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Grid(const Grid& gridToCopy) :
    Container     (gridToCopy),
    m_Size        (gridToCopy.m_Size),
    m_IntendedSize(gridToCopy.m_IntendedSize)
    {
        const std::vector<Widget::Ptr>& widgets = gridToCopy.m_Widgets;

        for (unsigned int row = 0; row < gridToCopy.m_GridWidgets.size(); ++row)
        {
            for (unsigned int col = 0; col < gridToCopy.m_GridWidgets[row].size(); ++col)
            {
                // Find the widget that belongs in this square
                for (unsigned int i = 0; i < widgets.size(); ++i)
                {
                    // If a widget matches then add it to the grid
                    if (widgets[i] == gridToCopy.m_GridWidgets[row][col])
                        addWidget(widgets[i], row, col, gridToCopy.m_ObjBorders[row][col], gridToCopy.m_ObjLayout[row][col]);
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
        // Make sure it is not the same widget
        if (this != &right)
        {
            Grid temp(right);
            this->Container::operator=(right);

            std::swap(m_GridWidgets,  temp.m_GridWidgets);
            std::swap(m_ObjBorders,   temp.m_ObjBorders);
            std::swap(m_ObjLayout,    temp.m_ObjLayout);
            std::swap(m_RowHeight,    temp.m_RowHeight);
            std::swap(m_ColumnWidth,  temp.m_ColumnWidth);
            std::swap(m_Size,         temp.m_Size);
            std::swap(m_IntendedSize, temp.m_IntendedSize);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid* Grid::clone()
    {
        return new Grid(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::setSize(float width, float height)
    {
        // A negative size is not allowed for this object
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // Change the intended size of the grid
        m_IntendedSize.x = width;
        m_IntendedSize.y = height;

        updatePositionsOfAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Grid::getSize() const
    {
        return m_Size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::remove(const Widget::Ptr& widget)
    {
        remove(widget.get());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::remove(Widget* widget)
    {
        // Find the widget in the grid
        for (unsigned int row = 0; row < m_GridWidgets.size(); ++row)
        {
            for (unsigned int col = 0; col < m_GridWidgets[row].size(); ++col)
            {
                if (m_GridWidgets[row][col].get() == widget)
                {
                    // Remove the widget from the grid
                    m_GridWidgets[row].erase(m_GridWidgets[row].begin() + col);
                    m_ObjBorders[row].erase(m_ObjBorders[row].begin() + col);
                    m_ObjLayout[row].erase(m_ObjLayout[row].begin() + col);

                    // Check if this is the last column
                    if (m_ColumnWidth.size() == m_GridWidgets[row].size() + 1)
                    {
                        // Check if there is another row with this many columns
                        bool rowFound = false;
                        for (unsigned int i = 0; i < m_GridWidgets.size(); ++i)
                        {
                            if (m_GridWidgets[i].size() >= m_ColumnWidth.size())
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
                    if (m_GridWidgets[row].empty())
                    {
                        m_GridWidgets.erase(m_GridWidgets.begin() + row);
                        m_ObjBorders.erase(m_ObjBorders.begin() + row);
                        m_ObjLayout.erase(m_ObjLayout.begin() + row);
                        m_RowHeight.erase(m_RowHeight.begin() + row);
                    }

                    // Update the positions of all remaining widgets
                    updatePositionsOfAllWidgets();
                }
            }
        }

        Container::remove(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::removeAllWidgets()
    {
        m_GridWidgets.clear();
        m_ObjBorders.clear();
        m_ObjLayout.clear();

        m_RowHeight.clear();
        m_ColumnWidth.clear();

        Container::removeAllWidgets();

        m_Size.x = 0;
        m_Size.y = 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::addWidget(const Widget::Ptr& widget, unsigned int row, unsigned int col,
                         const Borders& borders, Layout::Layouts layout)
    {
        // Create the row if it didn't exist yet
        if (m_GridWidgets.size() < row + 1)
        {
            m_GridWidgets.resize(row + 1);
            m_ObjBorders.resize(row + 1);
            m_ObjLayout.resize(row + 1);
        }

        // Create the column if it didn't exist yet
        if (m_GridWidgets[row].size() < col + 1)
        {
            m_GridWidgets[row].resize(col + 1, nullptr);
            m_ObjBorders[row].resize(col + 1);
            m_ObjLayout[row].resize(col + 1);
        }

        // If this is a new row then reserve some space for it
        if (m_RowHeight.size() < row + 1)
            m_RowHeight.resize(row + 1, 0);

        // If this is the first row to have so many columns then reserve some space for it
        if (m_ColumnWidth.size() < col + 1)
            m_ColumnWidth.resize(col + 1, 0);

        // Add the widget to the grid
        m_GridWidgets[row][col] = widget;
        m_ObjBorders[row][col] = borders;
        m_ObjLayout[row][col] = layout;

        // Update the widgets
        updateWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Grid::getWidget(unsigned int row, unsigned int col)
    {
        if ((m_GridWidgets.size() > row) && (m_GridWidgets[row].size() > col))
            return m_GridWidgets[row][col];
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::updateWidgets()
    {
        // Reset the column widths
        for (std::vector<unsigned int>::iterator it = m_ColumnWidth.begin(); it != m_ColumnWidth.end(); ++it)
            *it = 0;

        // Loop through all widgets
        for (unsigned int row = 0; row < m_GridWidgets.size(); ++row)
        {
            // Reset the row height
            m_RowHeight[row] = 0;

            for (unsigned int col = 0; col < m_GridWidgets[row].size(); ++col)
            {
                if (m_GridWidgets[row][col].get() == nullptr)
                    continue;

                // Remember the biggest column width
                if (m_ColumnWidth[col] < m_GridWidgets[row][col]->getFullSize().x + m_ObjBorders[row][col].left + m_ObjBorders[row][col].right)
                    m_ColumnWidth[col] = static_cast<unsigned int>(m_GridWidgets[row][col]->getFullSize().x + m_ObjBorders[row][col].left + m_ObjBorders[row][col].right);

                // Remember the biggest row height
                if (m_RowHeight[row] < m_GridWidgets[row][col]->getFullSize().y + m_ObjBorders[row][col].top + m_ObjBorders[row][col].bottom)
                    m_RowHeight[row] = static_cast<unsigned int>(m_GridWidgets[row][col]->getFullSize().y + m_ObjBorders[row][col].top + m_ObjBorders[row][col].bottom);
            }
        }

        // Reposition all widgets
        updatePositionsOfAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::changeWidgetBorders(const Widget::Ptr& widget, const Borders& borders)
    {
        // Find the widget in the grid
        for (unsigned int row = 0; row < m_GridWidgets.size(); ++row)
        {
            for (unsigned int col = 0; col < m_GridWidgets[row].size(); ++col)
            {
                if (m_GridWidgets[row][col] == widget)
                {
                    // Change borders of the widget
                    m_ObjBorders[row][col] = borders;

                    // Update all widgets
                    updateWidgets();
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::changeWidgetLayout(const Widget::Ptr& widget, Layout::Layouts layout)
    {
        // Find the widget in the grid
        for (unsigned int row = 0; row < m_GridWidgets.size(); ++row)
        {
            for (unsigned int col = 0; col < m_GridWidgets[row].size(); ++col)
            {
                if (m_GridWidgets[row][col] == widget)
                {
                    // Change the layout of the widget
                    m_ObjLayout[row][col] = layout;

                    // Recalculate the position of the widget
                    {
                        // Calculate the available space which is distributed when widgets are positionned.
                        sf::Vector2f availableSpace;
                        sf::Vector2f minSize = getMinSize();

                        if (m_Size.x > minSize.x)
                            availableSpace.x = m_Size.x - minSize.x;
                        if (m_Size.y > minSize.y)
                            availableSpace.y = m_Size.y - minSize.y;

                        sf::Vector2f availSpaceOffset(0.5f * availableSpace.x / m_ColumnWidth.size(),
                                                      0.5f * availableSpace.y / m_RowHeight.size());

                        float left = 0;
                        float top = 0;

                        for (unsigned int i=0; i < row; ++i)
                            top += static_cast<float>(m_RowHeight[i]) + 2 * availSpaceOffset.y;

                        for (unsigned int i=0; i < col; ++i)
                            left += static_cast<float>(m_ColumnWidth[i]) + 2 * availSpaceOffset.x;

                        switch (m_ObjLayout[row][col])
                        {
                        case Layout::UpperLeft:
                            left += static_cast<float>(m_ObjBorders[row][col].left) + availSpaceOffset.x;
                            top += static_cast<float>(m_ObjBorders[row][col].top) + availSpaceOffset.y;
                            break;

                        case Layout::Up:
                            left += m_ObjBorders[row][col].left + (((m_ColumnWidth[col] - m_ObjBorders[row][col].left - m_ObjBorders[row][col].right) - m_GridWidgets[row][col]->getFullSize().x) / 2.f) + availSpaceOffset.x;
                            top += static_cast<float>(m_ObjBorders[row][col].top) + availSpaceOffset.y;
                            break;

                        case Layout::UpperRight:
                            left += m_ColumnWidth[col] - m_ObjBorders[row][col].right - m_GridWidgets[row][col]->getFullSize().x + availSpaceOffset.x;
                            top += static_cast<float>(m_ObjBorders[row][col].top) + availSpaceOffset.y;
                            break;

                        case Layout::Right:
                            left += m_ColumnWidth[col] - m_ObjBorders[row][col].right - m_GridWidgets[row][col]->getFullSize().x + availSpaceOffset.x;
                            top += m_ObjBorders[row][col].top + (((m_RowHeight[row] - m_ObjBorders[row][col].top - m_ObjBorders[row][col].bottom) - m_GridWidgets[row][col]->getFullSize().y) / 2.f) + availSpaceOffset.y;
                            break;

                        case Layout::BottomRight:
                            left += m_ColumnWidth[col] - m_ObjBorders[row][col].right - m_GridWidgets[row][col]->getFullSize().x + availSpaceOffset.x;
                            top += m_RowHeight[row] - m_ObjBorders[row][col].bottom - m_GridWidgets[row][col]->getFullSize().y + availSpaceOffset.y;
                            break;

                        case Layout::Bottom:
                            left += m_ObjBorders[row][col].left + (((m_ColumnWidth[col] - m_ObjBorders[row][col].left - m_ObjBorders[row][col].right) - m_GridWidgets[row][col]->getFullSize().x) / 2.f) + availSpaceOffset.x;
                            top += m_RowHeight[row] - m_ObjBorders[row][col].bottom - m_GridWidgets[row][col]->getFullSize().y + availSpaceOffset.y;
                            break;

                        case Layout::BottomLeft:
                            left += static_cast<float>(m_ObjBorders[row][col].left) + availSpaceOffset.x;
                            top += m_RowHeight[row] - m_ObjBorders[row][col].bottom - m_GridWidgets[row][col]->getFullSize().y + availSpaceOffset.y;
                            break;

                        case Layout::Left:
                            left += static_cast<float>(m_ObjBorders[row][col].left) + availSpaceOffset.x;
                            top += m_ObjBorders[row][col].top + (((m_RowHeight[row] - m_ObjBorders[row][col].top - m_ObjBorders[row][col].bottom) - m_GridWidgets[row][col]->getFullSize().y) / 2.f) + availSpaceOffset.y;
                            break;

                        case Layout::Center:
                            left += m_ObjBorders[row][col].left + (((m_ColumnWidth[col] - m_ObjBorders[row][col].left - m_ObjBorders[row][col].right) - m_GridWidgets[row][col]->getFullSize().x) / 2.f) + availSpaceOffset.x;
                            top += m_ObjBorders[row][col].top + (((m_RowHeight[row] - m_ObjBorders[row][col].top - m_ObjBorders[row][col].bottom) - m_GridWidgets[row][col]->getFullSize().y) / 2.f) + availSpaceOffset.y;
                            break;
                        }

                        m_GridWidgets[row][col]->setPosition(left, top);
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Grid::getMinSize()
    {
        // Calculate the required place to have all widgets in the grid.
        sf::Vector2f minSize;

        // Loop through all rows to find the minimum height required by the grid
        for (auto it = m_RowHeight.cbegin(); it != m_RowHeight.cend(); ++it)
            minSize.y += static_cast<float>(*it);

        // Loop through all columns to find the minimum width required by the grid
        for (auto it = m_ColumnWidth.cbegin(); it != m_ColumnWidth.cend(); ++it)
            minSize.x += static_cast<float>(*it);

        return minSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::updatePositionsOfAllWidgets()
    {
        sf::Vector2f position;
        sf::Vector2f previousPosition;

        // Calculate m_Size and the available space which will be distributed when widgets will be positionned.
        sf::Vector2f availableSpace;
        m_Size = m_IntendedSize;
        sf::Vector2f minSize = getMinSize();

        if (m_IntendedSize.x > minSize.x)
            availableSpace.x = m_IntendedSize.x - minSize.x;
        else
            m_Size.x = minSize.x;

        if (m_IntendedSize.y > minSize.y)
            availableSpace.y = m_IntendedSize.y - minSize.y;
        else
            m_Size.y = minSize.y;

        sf::Vector2f availSpaceOffset(0.5f * availableSpace.x / m_ColumnWidth.size(),
                                      0.5f * availableSpace.y / m_RowHeight.size());

        // Loop through all rows
        for (unsigned int row = 0; row < m_GridWidgets.size(); ++row)
        {
            // Remember the current position
            previousPosition = position;

            // Loop through all widgets in the row
            for (unsigned int col = 0; col < m_GridWidgets[row].size(); ++col)
            {
                if (m_GridWidgets[row][col].get() == nullptr)
                {
                    position.x += m_ColumnWidth[col] + 2 * availSpaceOffset.x;
                    continue;
                }

                sf::Vector2f cellPosition(position);

                // Place the next widget on the correct position
                switch (m_ObjLayout[row][col])
                {
                case Layout::UpperLeft:
                    cellPosition.x += static_cast<float>(m_ObjBorders[row][col].left) + availSpaceOffset.x;
                    cellPosition.y += static_cast<float>(m_ObjBorders[row][col].top) + availSpaceOffset.y;
                    break;

                case Layout::Up:
                    cellPosition.x += m_ObjBorders[row][col].left + (((m_ColumnWidth[col] - m_ObjBorders[row][col].left - m_ObjBorders[row][col].right) - m_GridWidgets[row][col]->getFullSize().x) / 2.f) + availSpaceOffset.x;
                    cellPosition.y += static_cast<float>(m_ObjBorders[row][col].top) + availSpaceOffset.y;
                    break;

                case Layout::UpperRight:
                    cellPosition.x += m_ColumnWidth[col] - m_ObjBorders[row][col].right - m_GridWidgets[row][col]->getFullSize().x + availSpaceOffset.x;
                    cellPosition.y += static_cast<float>(m_ObjBorders[row][col].top) + availSpaceOffset.y;
                    break;

                case Layout::Right:
                    cellPosition.x += m_ColumnWidth[col] - m_ObjBorders[row][col].right - m_GridWidgets[row][col]->getFullSize().x + availSpaceOffset.x;
                    cellPosition.y += m_ObjBorders[row][col].top + (((m_RowHeight[row] - m_ObjBorders[row][col].top - m_ObjBorders[row][col].bottom) - m_GridWidgets[row][col]->getFullSize().y) / 2.f) + availSpaceOffset.y;
                    break;

                case Layout::BottomRight:
                    cellPosition.x += m_ColumnWidth[col] - m_ObjBorders[row][col].right - m_GridWidgets[row][col]->getFullSize().x + availSpaceOffset.x;
                    cellPosition.y += m_RowHeight[row] - m_ObjBorders[row][col].bottom - m_GridWidgets[row][col]->getFullSize().y + availSpaceOffset.y;
                    break;

                case Layout::Bottom:
                    cellPosition.x += m_ObjBorders[row][col].left + (((m_ColumnWidth[col] - m_ObjBorders[row][col].left - m_ObjBorders[row][col].right) - m_GridWidgets[row][col]->getFullSize().x) / 2.f) + availSpaceOffset.x;
                    cellPosition.y += m_RowHeight[row] - m_ObjBorders[row][col].bottom - m_GridWidgets[row][col]->getFullSize().y + availSpaceOffset.y;
                    break;

                case Layout::BottomLeft:
                    cellPosition.x += static_cast<float>(m_ObjBorders[row][col].left) + availSpaceOffset.x;
                    cellPosition.y += m_RowHeight[row] - m_ObjBorders[row][col].bottom - m_GridWidgets[row][col]->getFullSize().y + availSpaceOffset.y;
                    break;

                case Layout::Left:
                    cellPosition.x += static_cast<float>(m_ObjBorders[row][col].left) + availSpaceOffset.x;
                    cellPosition.y += m_ObjBorders[row][col].top + (((m_RowHeight[row] - m_ObjBorders[row][col].top - m_ObjBorders[row][col].bottom) - m_GridWidgets[row][col]->getFullSize().y) / 2.f) + availSpaceOffset.y;
                    break;

                case Layout::Center:
                    cellPosition.x += m_ObjBorders[row][col].left + (((m_ColumnWidth[col] - m_ObjBorders[row][col].left - m_ObjBorders[row][col].right) - m_GridWidgets[row][col]->getFullSize().x) / 2.f) + availSpaceOffset.x;
                    cellPosition.y += m_ObjBorders[row][col].top + (((m_RowHeight[row] - m_ObjBorders[row][col].top - m_ObjBorders[row][col].bottom) - m_GridWidgets[row][col]->getFullSize().y) / 2.f) + availSpaceOffset.y;
                    break;
                }

                m_GridWidgets[row][col]->setPosition(cellPosition);
                position.x += m_ColumnWidth[col] + 2 * availSpaceOffset.x;
            }

            // Go to the next row
            position = previousPosition;
            position.y += m_RowHeight[row] + 2 * availSpaceOffset.y;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Grid::mouseOnWidget(float x, float y)
    {
        // Don't continue when the child window has not been loaded yet
        if (m_Loaded == false)
            return false;

        // Get the current position
        sf::Vector2f position = getPosition();

        // Check if the mouse might be on top of the grid
        if ((x > position.x) && (y > position.y))
        {
            // Check if the mouse is on the grid
            if ((x < position.x + m_Size.x) && (y < position.y + m_Size.y))
                return true;
        }

        if (m_MouseHover)
        {
            mouseLeftWidget();

            // Tell the widgets inside the grid that the mouse is no longer on top of them
            for (unsigned int i = 0; i < m_Widgets.size(); ++i)
                m_Widgets[i]->mouseNotOnWidget();

            m_MouseHover = false;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Set the transformation
        states.transform *= getTransform();

        // Draw all widgets
        for (unsigned int row = 0; row < m_GridWidgets.size(); ++row)
        {
            for (unsigned int col = 0; col < m_GridWidgets[row].size(); ++col)
            {
                if (m_GridWidgets[row][col].get() != nullptr)
                    target.draw(*m_GridWidgets[row][col], states);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
