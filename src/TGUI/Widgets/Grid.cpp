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


#include <TGUI/Widgets/Grid.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Grid()
    {
        m_callback.widgetType = "Grid";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Grid(const Grid& gridToCopy) :
        Container {gridToCopy},
        m_realSize{gridToCopy.m_realSize}
    {
        const std::vector<Widget::Ptr>& widgets = gridToCopy.m_widgets;

        for (unsigned int row = 0; row < gridToCopy.m_gridWidgets.size(); ++row)
        {
            for (unsigned int col = 0; col < gridToCopy.m_gridWidgets[row].size(); ++col)
            {
                // Find the widget that belongs in this square
                for (unsigned int i = 0; i < widgets.size(); ++i)
                {
                    // If a widget matches then add it to the grid
                    if (widgets[i] == gridToCopy.m_gridWidgets[row][col])
                        addWidget(m_widgets[i], row, col, gridToCopy.m_objBorders[row][col], gridToCopy.m_objAlignment[row][col]);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid& Grid::operator= (const Grid& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Grid temp{right};
            Container::operator=(right);

            std::swap(m_gridWidgets,  temp.m_gridWidgets);
            std::swap(m_objBorders,   temp.m_objBorders);
            std::swap(m_objAlignment, temp.m_objAlignment);
            std::swap(m_rowHeight,    temp.m_rowHeight);
            std::swap(m_columnWidth,  temp.m_columnWidth);
            std::swap(m_realSize,     temp.m_realSize);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Ptr Grid::create()
    {
        return std::make_shared<Grid>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Ptr Grid::copy(Grid::ConstPtr grid)
    {
        if (grid)
            return std::static_pointer_cast<Grid>(grid->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        updatePositionsOfAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Grid::remove(const Widget::Ptr& widget)
    {
        auto callbackIt = m_connectedCallbacks.find(widget);
        if (callbackIt != m_connectedCallbacks.end())
            m_connectedCallbacks.erase(callbackIt);

        // Find the widget in the grid
        for (unsigned int row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (unsigned int col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col] == widget)
                {
                    // Remove the widget from the grid
                    m_gridWidgets[row].erase(m_gridWidgets[row].begin() + col);
                    m_objBorders[row].erase(m_objBorders[row].begin() + col);
                    m_objAlignment[row].erase(m_objAlignment[row].begin() + col);

                    // Check if this is the last column
                    if (m_columnWidth.size() == m_gridWidgets[row].size() + 1)
                    {
                        // Check if there is another row with this many columns
                        bool rowFound = false;
                        for (unsigned int i = 0; i < m_gridWidgets.size(); ++i)
                        {
                            if (m_gridWidgets[i].size() >= m_columnWidth.size())
                            {
                                rowFound = true;
                                break;
                            }
                        }

                        // Erase the last column if no other row is using it
                        if (!rowFound)
                            m_columnWidth.erase(m_columnWidth.end()-1);
                    }

                    // If the row is empty then remove it as well
                    if (m_gridWidgets[row].empty())
                    {
                        m_gridWidgets.erase(m_gridWidgets.begin() + row);
                        m_objBorders.erase(m_objBorders.begin() + row);
                        m_objAlignment.erase(m_objAlignment.begin() + row);
                        m_rowHeight.erase(m_rowHeight.begin() + row);
                    }

                    // Update the positions of all remaining widgets
                    updatePositionsOfAllWidgets();
                }
            }
        }

        return Container::remove(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::removeAllWidgets()
    {
        m_gridWidgets.clear();
        m_objBorders.clear();
        m_objAlignment.clear();

        m_rowHeight.clear();
        m_columnWidth.clear();

        m_connectedCallbacks.clear();

        Container::removeAllWidgets();

        setSize(0,0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::addWidget(const Widget::Ptr& widget, unsigned int row, unsigned int col,
                         const Borders& borders, Alignment alignment)
    {
        // If the widget hasn't already been added then add it now
        if (std::find(getWidgets().begin(), getWidgets().end(), widget) == getWidgets().end())
            add(widget);

        // Create the row if it did not exist yet
        if (m_gridWidgets.size() < row + 1)
        {
            m_gridWidgets.resize(row + 1);
            m_objBorders.resize(row + 1);
            m_objAlignment.resize(row + 1);
        }

        // Create the column if it did not exist yet
        if (m_gridWidgets[row].size() < col + 1)
        {
            m_gridWidgets[row].resize(col + 1, nullptr);
            m_objBorders[row].resize(col + 1);
            m_objAlignment[row].resize(col + 1);
        }

        // If this is a new row then reserve some space for it
        if (m_rowHeight.size() < row + 1)
            m_rowHeight.resize(row + 1, 0);

        // If this is the first row to have so many columns then reserve some space for it
        if (m_columnWidth.size() < col + 1)
            m_columnWidth.resize(col + 1, 0);

        // Add the widget to the grid
        m_gridWidgets[row][col] = widget;
        m_objBorders[row][col] = borders;
        m_objAlignment[row][col] = alignment;

        // Update the widgets
        updateWidgets();

        // Automatically update the widgets when their size changes
        m_connectedCallbacks[widget] = widget->connect("SizeChanged", &Grid::updateWidgets, this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Grid::getWidget(unsigned int row, unsigned int col)
    {
        if ((m_gridWidgets.size() > row) && (m_gridWidgets[row].size() > col))
            return m_gridWidgets[row][col];
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::updateWidgets()
    {
        // Reset the column widths
        for (auto it = m_columnWidth.begin(); it != m_columnWidth.end(); ++it)
            *it = 0;

        // Loop through all widgets
        for (unsigned int row = 0; row < m_gridWidgets.size(); ++row)
        {
            // Reset the row height
            m_rowHeight[row] = 0;

            for (unsigned int col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col].get() == nullptr)
                    continue;

                // Remember the biggest column width
                if (m_columnWidth[col] < m_gridWidgets[row][col]->getFullSize().x + m_objBorders[row][col].left + m_objBorders[row][col].right)
                    m_columnWidth[col] = m_gridWidgets[row][col]->getFullSize().x + m_objBorders[row][col].left + m_objBorders[row][col].right;

                // Remember the biggest row height
                if (m_rowHeight[row] < m_gridWidgets[row][col]->getFullSize().y + m_objBorders[row][col].top + m_objBorders[row][col].bottom)
                    m_rowHeight[row] = m_gridWidgets[row][col]->getFullSize().y + m_objBorders[row][col].top + m_objBorders[row][col].bottom;
            }
        }

        // Reposition all widgets
        updatePositionsOfAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::changeWidgetBorders(const Widget::Ptr& widget, const Borders& borders)
    {
        // Find the widget in the grid
        for (unsigned int row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (unsigned int col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col] == widget)
                {
                    // Change borders of the widget
                    m_objBorders[row][col] = borders;

                    // Update all widgets
                    updateWidgets();
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::changeWidgetAlignment(const Widget::Ptr& widget, Alignment alignment)
    {
        // Find the widget in the grid
        for (unsigned int row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (unsigned int col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col] == widget)
                {
                    // Change the alignment of the widget
                    m_objAlignment[row][col] = alignment;

                    // Recalculate the position of the widget
                    {
                        // Calculate the available space which is distributed when widgets are positionned.
                        sf::Vector2f availableSpace;
                        sf::Vector2f minSize = getMinSize();

                        if (m_realSize.x > minSize.x)
                            availableSpace.x = m_realSize.x - minSize.x;
                        if (m_realSize.y > minSize.y)
                            availableSpace.y = m_realSize.y - minSize.y;

                        sf::Vector2f availSpaceOffset{0.5f * availableSpace.x / m_columnWidth.size(),
                                                      0.5f * availableSpace.y / m_rowHeight.size()};

                        float left = 0;
                        float top = 0;

                        for (unsigned int i = 0; i < row; ++i)
                            top += m_rowHeight[i] + 2 * availSpaceOffset.y;

                        for (unsigned int i = 0; i < col; ++i)
                            left += m_columnWidth[i] + 2 * availSpaceOffset.x;

                        switch (m_objAlignment[row][col])
                        {
                        case Alignment::UpperLeft:
                            left += m_objBorders[row][col].left + availSpaceOffset.x;
                            top += m_objBorders[row][col].top + availSpaceOffset.y;
                            break;

                        case Alignment::Up:
                            left += m_objBorders[row][col].left + (((m_columnWidth[col] - m_objBorders[row][col].left - m_objBorders[row][col].right) - m_gridWidgets[row][col]->getFullSize().x) / 2.f) + availSpaceOffset.x;
                            top += m_objBorders[row][col].top + availSpaceOffset.y;
                            break;

                        case Alignment::UpperRight:
                            left += m_columnWidth[col] - m_objBorders[row][col].right - m_gridWidgets[row][col]->getFullSize().x + availSpaceOffset.x;
                            top += m_objBorders[row][col].top + availSpaceOffset.y;
                            break;

                        case Alignment::Right:
                            left += m_columnWidth[col] - m_objBorders[row][col].right - m_gridWidgets[row][col]->getFullSize().x + availSpaceOffset.x;
                            top += m_objBorders[row][col].top + (((m_rowHeight[row] - m_objBorders[row][col].top - m_objBorders[row][col].bottom) - m_gridWidgets[row][col]->getFullSize().y) / 2.f) + availSpaceOffset.y;
                            break;

                        case Alignment::BottomRight:
                            left += m_columnWidth[col] - m_objBorders[row][col].right - m_gridWidgets[row][col]->getFullSize().x + availSpaceOffset.x;
                            top += m_rowHeight[row] - m_objBorders[row][col].bottom - m_gridWidgets[row][col]->getFullSize().y + availSpaceOffset.y;
                            break;

                        case Alignment::Bottom:
                            left += m_objBorders[row][col].left + (((m_columnWidth[col] - m_objBorders[row][col].left - m_objBorders[row][col].right) - m_gridWidgets[row][col]->getFullSize().x) / 2.f) + availSpaceOffset.x;
                            top += m_rowHeight[row] - m_objBorders[row][col].bottom - m_gridWidgets[row][col]->getFullSize().y + availSpaceOffset.y;
                            break;

                        case Alignment::BottomLeft:
                            left += m_objBorders[row][col].left + availSpaceOffset.x;
                            top += m_rowHeight[row] - m_objBorders[row][col].bottom - m_gridWidgets[row][col]->getFullSize().y + availSpaceOffset.y;
                            break;

                        case Alignment::Left:
                            left += m_objBorders[row][col].left + availSpaceOffset.x;
                            top += m_objBorders[row][col].top + (((m_rowHeight[row] - m_objBorders[row][col].top - m_objBorders[row][col].bottom) - m_gridWidgets[row][col]->getFullSize().y) / 2.f) + availSpaceOffset.y;
                            break;

                        case Alignment::Center:
                            left += m_objBorders[row][col].left + (((m_columnWidth[col] - m_objBorders[row][col].left - m_objBorders[row][col].right) - m_gridWidgets[row][col]->getFullSize().x) / 2.f) + availSpaceOffset.x;
                            top += m_objBorders[row][col].top + (((m_rowHeight[row] - m_objBorders[row][col].top - m_objBorders[row][col].bottom) - m_gridWidgets[row][col]->getFullSize().y) / 2.f) + availSpaceOffset.y;
                            break;
                        }

                        m_gridWidgets[row][col]->setPosition({left, top});
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
        for (auto it = m_rowHeight.cbegin(); it != m_rowHeight.cend(); ++it)
            minSize.y += *it;

        // Loop through all columns to find the minimum width required by the grid
        for (auto it = m_columnWidth.cbegin(); it != m_columnWidth.cend(); ++it)
            minSize.x += *it;

        return minSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::updatePositionsOfAllWidgets()
    {
        sf::Vector2f position;
        sf::Vector2f previousPosition;

        // Calculate the size and the available space which will be distributed when widgets will be positionned.
        sf::Vector2f availableSpace;
        m_realSize = m_size.getValue();
        sf::Vector2f minSize = getMinSize();

        if (getSize().x > minSize.x)
            availableSpace.x = getSize().x - minSize.x;
        else
            m_realSize.x = minSize.x;

        if (getSize().y > minSize.y)
            availableSpace.y = getSize().y - minSize.y;
        else
            m_realSize.y = minSize.y;

        sf::Vector2f availSpaceOffset{0.5f * availableSpace.x / m_columnWidth.size(),
                                      0.5f * availableSpace.y / m_rowHeight.size()};

        // Loop through all rows
        for (unsigned int row = 0; row < m_gridWidgets.size(); ++row)
        {
            // Remember the current position
            previousPosition = position;

            // Loop through all widgets in the row
            for (unsigned int col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col].get() == nullptr)
                {
                    position.x += m_columnWidth[col] + 2 * availSpaceOffset.x;
                    continue;
                }

                sf::Vector2f cellPosition = position;

                // Place the next widget on the correct position
                switch (m_objAlignment[row][col])
                {
                case Alignment::UpperLeft:
                    cellPosition.x += m_objBorders[row][col].left + availSpaceOffset.x;
                    cellPosition.y += m_objBorders[row][col].top + availSpaceOffset.y;
                    break;

                case Alignment::Up:
                    cellPosition.x += m_objBorders[row][col].left + (((m_columnWidth[col] - m_objBorders[row][col].left - m_objBorders[row][col].right) - m_gridWidgets[row][col]->getFullSize().x) / 2.f) + availSpaceOffset.x;
                    cellPosition.y += m_objBorders[row][col].top + availSpaceOffset.y;
                    break;

                case Alignment::UpperRight:
                    cellPosition.x += m_columnWidth[col] - m_objBorders[row][col].right - m_gridWidgets[row][col]->getFullSize().x + availSpaceOffset.x;
                    cellPosition.y += m_objBorders[row][col].top + availSpaceOffset.y;
                    break;

                case Alignment::Right:
                    cellPosition.x += m_columnWidth[col] - m_objBorders[row][col].right - m_gridWidgets[row][col]->getFullSize().x + availSpaceOffset.x;
                    cellPosition.y += m_objBorders[row][col].top + (((m_rowHeight[row] - m_objBorders[row][col].top - m_objBorders[row][col].bottom) - m_gridWidgets[row][col]->getFullSize().y) / 2.f) + availSpaceOffset.y;
                    break;

                case Alignment::BottomRight:
                    cellPosition.x += m_columnWidth[col] - m_objBorders[row][col].right - m_gridWidgets[row][col]->getFullSize().x + availSpaceOffset.x;
                    cellPosition.y += m_rowHeight[row] - m_objBorders[row][col].bottom - m_gridWidgets[row][col]->getFullSize().y + availSpaceOffset.y;
                    break;

                case Alignment::Bottom:
                    cellPosition.x += m_objBorders[row][col].left + (((m_columnWidth[col] - m_objBorders[row][col].left - m_objBorders[row][col].right) - m_gridWidgets[row][col]->getFullSize().x) / 2.f) + availSpaceOffset.x;
                    cellPosition.y += m_rowHeight[row] - m_objBorders[row][col].bottom - m_gridWidgets[row][col]->getFullSize().y + availSpaceOffset.y;
                    break;

                case Alignment::BottomLeft:
                    cellPosition.x += m_objBorders[row][col].left + availSpaceOffset.x;
                    cellPosition.y += m_rowHeight[row] - m_objBorders[row][col].bottom - m_gridWidgets[row][col]->getFullSize().y + availSpaceOffset.y;
                    break;

                case Alignment::Left:
                    cellPosition.x += m_objBorders[row][col].left + availSpaceOffset.x;
                    cellPosition.y += m_objBorders[row][col].top + (((m_rowHeight[row] - m_objBorders[row][col].top - m_objBorders[row][col].bottom) - m_gridWidgets[row][col]->getFullSize().y) / 2.f) + availSpaceOffset.y;
                    break;

                case Alignment::Center:
                    cellPosition.x += m_objBorders[row][col].left + (((m_columnWidth[col] - m_objBorders[row][col].left - m_objBorders[row][col].right) - m_gridWidgets[row][col]->getFullSize().x) / 2.f) + availSpaceOffset.x;
                    cellPosition.y += m_objBorders[row][col].top + (((m_rowHeight[row] - m_objBorders[row][col].top - m_objBorders[row][col].bottom) - m_gridWidgets[row][col]->getFullSize().y) / 2.f) + availSpaceOffset.y;
                    break;
                }

                m_gridWidgets[row][col]->setPosition(cellPosition);
                position.x += m_columnWidth[col] + 2 * availSpaceOffset.x;
            }

            // Go to the next row
            position = previousPosition;
            position.y += m_rowHeight[row] + 2 * availSpaceOffset.y;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Grid::mouseOnWidget(float x, float y) const
    {
        return sf::FloatRect{getPosition().x, getPosition().y, m_realSize.x, m_realSize.y}.contains(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw all widgets
        for (unsigned int row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (unsigned int col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col].get() != nullptr)
                {
                    if (m_gridWidgets[row][col]->isVisible())
                        target.draw(*m_gridWidgets[row][col], states);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
