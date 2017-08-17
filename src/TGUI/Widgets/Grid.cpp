/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
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
        m_type = "Grid";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Grid(const Grid& gridToCopy) :
        Container {gridToCopy},
        m_autoSize{gridToCopy.m_autoSize}
    {
        for (std::size_t row = 0; row < gridToCopy.m_gridWidgets.size(); ++row)
        {
            for (std::size_t col = 0; col < gridToCopy.m_gridWidgets[row].size(); ++col)
            {
                // Find the widget that belongs in this square
                for (std::size_t i = 0; i < gridToCopy.m_widgets.size(); ++i)
                {
                    // If a widget matches then add it to the grid
                    if (gridToCopy.m_widgets[i] == gridToCopy.m_gridWidgets[row][col])
                        addWidget(m_widgets[i], row, col, gridToCopy.m_objBorders[row][col], gridToCopy.m_objAlignment[row][col]);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Grid(Grid&& gridToMove) :
        Container           {std::move(gridToMove)},
        m_autoSize          {std::move(gridToMove.m_autoSize)},
        m_gridWidgets       {std::move(gridToMove.m_gridWidgets)},
        m_objBorders        {std::move(gridToMove.m_objBorders)},
        m_objAlignment      {std::move(gridToMove.m_objAlignment)},
        m_rowHeight         {std::move(gridToMove.m_rowHeight)},
        m_columnWidth       {std::move(gridToMove.m_columnWidth)},
        m_connectedCallbacks{}
    {
        for (auto& widget : m_widgets)
        {
            widget->onSizeChange.disconnect(gridToMove.m_connectedCallbacks[widget]);
            m_connectedCallbacks[widget] = widget->onSizeChange.connect([this](){ updateWidgets(); });
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid& Grid::operator= (const Grid& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Container::operator=(right);

            for (std::size_t row = 0; row < right.m_gridWidgets.size(); ++row)
            {
                for (std::size_t col = 0; col < right.m_gridWidgets[row].size(); ++col)
                {
                    // Find the widget that belongs in this square
                    for (std::size_t i = 0; i < right.m_widgets.size(); ++i)
                    {
                        // If a widget matches then add it to the grid
                        if (right.m_widgets[i] == right.m_gridWidgets[row][col])
                            addWidget(m_widgets[i], row, col, right.m_objBorders[row][col], right.m_objAlignment[row][col]);
                    }
                }
            }
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid& Grid::operator= (Grid&& right)
    {
        if (this != &right)
        {
            Container::operator=(std::move(right));
            m_autoSize           = std::move(right.m_autoSize);
            m_gridWidgets        = std::move(right.m_gridWidgets);
            m_objBorders         = std::move(right.m_objBorders);
            m_objAlignment       = std::move(right.m_objAlignment);
            m_rowHeight          = std::move(right.m_rowHeight);
            m_columnWidth        = std::move(right.m_columnWidth);
            m_connectedCallbacks = std::move(right.m_connectedCallbacks);

            for (auto& widget : m_widgets)
            {
                widget->onSizeChange.disconnect(right.m_connectedCallbacks[widget]);
                m_connectedCallbacks[widget] = widget->onSizeChange.connect([this](){ updateWidgets(); });
            }
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
        Container::setSize(size);

        updatePositionsOfAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::setAutoSize(bool autoSize)
    {
        if (m_autoSize != autoSize)
        {
            m_autoSize = autoSize;
            updatePositionsOfAllWidgets();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Grid::getAutoSize() const
    {
        return m_autoSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Grid::remove(const Widget::Ptr& widget)
    {
        const auto callbackIt = m_connectedCallbacks.find(widget);
        if (callbackIt != m_connectedCallbacks.end())
            m_connectedCallbacks.erase(callbackIt);

        // Find the widget in the grid
        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
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
                        for (std::size_t i = 0; i < m_gridWidgets.size(); ++i)
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
        Container::removeAllWidgets();

        m_gridWidgets.clear();
        m_objBorders.clear();
        m_objAlignment.clear();

        m_rowHeight.clear();
        m_columnWidth.clear();

        m_connectedCallbacks.clear();

        updateWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::addWidget(const Widget::Ptr& widget, std::size_t row, std::size_t col, const Borders& borders, Alignment alignment)
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
        m_connectedCallbacks[widget] = widget->onSizeChange.connect([this](){ updateWidgets(); });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Grid::getWidget(std::size_t row, std::size_t col) const
    {
        if ((row < m_gridWidgets.size()) && (col < m_gridWidgets[row].size()))
            return m_gridWidgets[row][col];
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<Widget::Ptr, std::pair<std::size_t, std::size_t>> Grid::getWidgetLocations() const
    {
        std::map<Widget::Ptr, std::pair<std::size_t, std::size_t>> widgetsMap;

        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col])
                    widgetsMap[m_gridWidgets[row][col]] = {row, col};
            }
        }

        return widgetsMap;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::setWidgetBorders(const Widget::Ptr& widget, const Borders& borders)
    {
        // Find the widget in the grid
        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col] == widget)
                    setWidgetBorders(row, col, borders);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::setWidgetBorders(std::size_t row, std::size_t col, const Borders& borders)
    {
        if (((row < m_gridWidgets.size()) && (col < m_gridWidgets[row].size())) && (m_gridWidgets[row][col] != nullptr))
        {
            // Change borders of the widget
            m_objBorders[row][col] = borders;

            // Update all widgets
            updateWidgets();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Borders Grid::getWidgetBorders(const Widget::Ptr& widget) const
    {
        // Find the widget in the grid
        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col] == widget)
                    return getWidgetBorders(row, col);
            }
        }

        return {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Borders Grid::getWidgetBorders(std::size_t row, std::size_t col) const
    {
        if (((row < m_gridWidgets.size()) && (col < m_gridWidgets[row].size())) && (m_gridWidgets[row][col] != nullptr))
            return m_objBorders[row][col];
        else
            return {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::setWidgetAlignment(const Widget::Ptr& widget, Alignment alignment)
    {
        // Find the widget in the grid
        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col] == widget)
                    setWidgetAlignment(row, col, alignment);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::setWidgetAlignment(std::size_t row, std::size_t col, Alignment alignment)
    {
        if (((row < m_gridWidgets.size()) && (col < m_gridWidgets[row].size())) && (m_gridWidgets[row][col] != nullptr))
        {
            m_objAlignment[row][col] = alignment;
            updatePositionsOfAllWidgets();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Alignment Grid::getWidgetAlignment(const Widget::Ptr& widget) const
    {
        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col] == widget)
                    return getWidgetAlignment(row, col);
            }
        }

        return Alignment::Center;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Alignment Grid::getWidgetAlignment(std::size_t row, std::size_t col) const
    {
        if (((row < m_gridWidgets.size()) && (col < m_gridWidgets[row].size())) && (m_gridWidgets[row][col] != nullptr))
            return m_objAlignment[row][col];
        else
            return Alignment::Center;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<std::vector<Widget::Ptr>>& Grid::getGridWidgets() const
    {
        return m_gridWidgets;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Grid::mouseOnWidget(sf::Vector2f pos) const
    {
        return sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Grid::getMinimumSize() const
    {
        // Calculate the required space to have all widgets in the grid.
        sf::Vector2f minSize;

        // Loop through all rows to find the minimum height required by the grid
        for (float rowHeight : m_rowHeight)
            minSize.y += rowHeight;

        // Loop through all columns to find the minimum width required by the grid
        for (float columnWidth : m_columnWidth)
            minSize.x += columnWidth;

        return minSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::updatePositionsOfAllWidgets()
    {
        sf::Vector2f extraOffset;
        if (!m_autoSize && m_gridWidgets.size() > 1)
        {
            const sf::Vector2f minimumSize = getMinimumSize();
            if (getSize().x > minimumSize.x)
                extraOffset.x = (getSize().x - minimumSize.x) / (m_gridWidgets.size() - 1);
            if (getSize().y > minimumSize.y)
                extraOffset.y = (getSize().y - minimumSize.y) / (m_gridWidgets.size() - 1);
        }

        sf::Vector2f position;
        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            const sf::Vector2f previousPosition = position;

            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col].get() == nullptr)
                {
                    position.x += m_columnWidth[col] + extraOffset.x;
                    continue;
                }

                sf::Vector2f cellPosition = position + (extraOffset / 2.f);
                switch (m_objAlignment[row][col])
                {
                case Alignment::UpperLeft:
                    cellPosition.x += m_objBorders[row][col].getLeft();
                    cellPosition.y += m_objBorders[row][col].getTop();
                    break;

                case Alignment::Up:
                    cellPosition.x += m_objBorders[row][col].getLeft() + (((m_columnWidth[col] - m_objBorders[row][col].getLeft() - m_objBorders[row][col].getRight()) - m_gridWidgets[row][col]->getFullSize().x) / 2.f);
                    cellPosition.y += m_objBorders[row][col].getTop();
                    break;

                case Alignment::UpperRight:
                    cellPosition.x += m_columnWidth[col] - m_objBorders[row][col].getRight() - m_gridWidgets[row][col]->getFullSize().x;
                    cellPosition.y += m_objBorders[row][col].getTop();
                    break;

                case Alignment::Right:
                    cellPosition.x += m_columnWidth[col] - m_objBorders[row][col].getRight() - m_gridWidgets[row][col]->getFullSize().x;
                    cellPosition.y += m_objBorders[row][col].getTop() + (((m_rowHeight[row] - m_objBorders[row][col].getTop() - m_objBorders[row][col].getBottom()) - m_gridWidgets[row][col]->getFullSize().y) / 2.f);
                    break;

                case Alignment::BottomRight:
                    cellPosition.x += m_columnWidth[col] - m_objBorders[row][col].getRight() - m_gridWidgets[row][col]->getFullSize().x;
                    cellPosition.y += m_rowHeight[row] - m_objBorders[row][col].getBottom() - m_gridWidgets[row][col]->getFullSize().y;
                    break;

                case Alignment::Bottom:
                    cellPosition.x += m_objBorders[row][col].getLeft() + (((m_columnWidth[col] - m_objBorders[row][col].getLeft() - m_objBorders[row][col].getRight()) - m_gridWidgets[row][col]->getFullSize().x) / 2.f);
                    cellPosition.y += m_rowHeight[row] - m_objBorders[row][col].getBottom() - m_gridWidgets[row][col]->getFullSize().y;
                    break;

                case Alignment::BottomLeft:
                    cellPosition.x += m_objBorders[row][col].getLeft();
                    cellPosition.y += m_rowHeight[row] - m_objBorders[row][col].getBottom() - m_gridWidgets[row][col]->getFullSize().y;
                    break;

                case Alignment::Left:
                    cellPosition.x += m_objBorders[row][col].getLeft();
                    cellPosition.y += m_objBorders[row][col].getTop() + (((m_rowHeight[row] - m_objBorders[row][col].getTop() - m_objBorders[row][col].getBottom()) - m_gridWidgets[row][col]->getFullSize().y) / 2.f);
                    break;

                case Alignment::Center:
                    cellPosition.x += m_objBorders[row][col].getLeft() + (((m_columnWidth[col] - m_objBorders[row][col].getLeft() - m_objBorders[row][col].getRight()) - m_gridWidgets[row][col]->getFullSize().x) / 2.f);
                    cellPosition.y += m_objBorders[row][col].getTop() + (((m_rowHeight[row] - m_objBorders[row][col].getTop() - m_objBorders[row][col].getBottom()) - m_gridWidgets[row][col]->getFullSize().y) / 2.f);
                    break;
                }

                m_gridWidgets[row][col]->setPosition(cellPosition);
                position.x += m_columnWidth[col] + extraOffset.x;
            }

            // Move to the next row
            position = previousPosition;
            position.y += m_rowHeight[row] + extraOffset.y;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::updateWidgets()
    {
        // Reset the column widths
        for (float& width : m_columnWidth)
            width = 0;

        // Loop through all widgets
        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            // Reset the row height
            m_rowHeight[row] = 0;

            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col].get() == nullptr)
                    continue;

                // Remember the biggest column width
                if (m_columnWidth[col] < m_gridWidgets[row][col]->getFullSize().x + m_objBorders[row][col].getLeft() + m_objBorders[row][col].getRight())
                    m_columnWidth[col] = m_gridWidgets[row][col]->getFullSize().x + m_objBorders[row][col].getLeft() + m_objBorders[row][col].getRight();

                // Remember the biggest row height
                if (m_rowHeight[row] < m_gridWidgets[row][col]->getFullSize().y + m_objBorders[row][col].getTop() + m_objBorders[row][col].getBottom())
                    m_rowHeight[row] = m_gridWidgets[row][col]->getFullSize().y + m_objBorders[row][col].getTop() + m_objBorders[row][col].getBottom();
            }
        }

        // Reposition all widgets
        updatePositionsOfAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        // Draw all widgets
        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col].get() != nullptr)
                {
                    if (m_gridWidgets[row][col]->isVisible())
                        m_gridWidgets[row][col]->draw(target, states);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
