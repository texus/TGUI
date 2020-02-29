/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/SignalImpl.hpp>

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
                        addWidget(m_widgets[i], row, col, gridToCopy.m_objPadding[row][col], gridToCopy.m_objAlignment[row][col]);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid::Grid(Grid&& gridToMove) :
        Container           {std::move(gridToMove)},
        m_autoSize          {std::move(gridToMove.m_autoSize)},
        m_gridWidgets       {std::move(gridToMove.m_gridWidgets)},
        m_objPadding        {std::move(gridToMove.m_objPadding)},
        m_objAlignment      {std::move(gridToMove.m_objAlignment)},
        m_rowHeight         {std::move(gridToMove.m_rowHeight)},
        m_columnWidth       {std::move(gridToMove.m_columnWidth)},
        m_connectedCallbacks{}
    {
        for (auto& widget : m_widgets)
        {
            widget->disconnect(gridToMove.m_connectedCallbacks[widget]);
            m_connectedCallbacks[widget] = widget->connect("SizeChanged", [this](){ updateWidgets(); });
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Grid& Grid::operator= (const Grid& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Container::operator=(right);
            m_autoSize = right.m_autoSize;
            m_connectedCallbacks.clear();

            for (std::size_t row = 0; row < right.m_gridWidgets.size(); ++row)
            {
                for (std::size_t col = 0; col < right.m_gridWidgets[row].size(); ++col)
                {
                    // Find the widget that belongs in this square
                    for (std::size_t i = 0; i < right.m_widgets.size(); ++i)
                    {
                        // If a widget matches then add it to the grid
                        if (right.m_widgets[i] == right.m_gridWidgets[row][col])
                            addWidget(m_widgets[i], row, col, right.m_objPadding[row][col], right.m_objAlignment[row][col]);
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
            m_objPadding         = std::move(right.m_objPadding);
            m_objAlignment       = std::move(right.m_objAlignment);
            m_rowHeight          = std::move(right.m_rowHeight);
            m_columnWidth        = std::move(right.m_columnWidth);
            m_connectedCallbacks = std::move(right.m_connectedCallbacks);

            for (auto& widget : m_widgets)
            {
                widget->disconnect(right.m_connectedCallbacks[widget]);
                m_connectedCallbacks[widget] = widget->connect("SizeChanged", [this](){ updateWidgets(); });
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

        m_autoSize = false;

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
        {
            widget->disconnect(callbackIt->second);
            m_connectedCallbacks.erase(callbackIt);
        }

        // Find the widget in the grid
        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col] == widget)
                {
                    // Remove the widget from the grid
                    m_gridWidgets[row].erase(m_gridWidgets[row].begin() + col);
                    m_objPadding[row].erase(m_objPadding[row].begin() + col);
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
                        m_objPadding.erase(m_objPadding.begin() + row);
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
        m_objPadding.clear();
        m_objAlignment.clear();

        m_rowHeight.clear();
        m_columnWidth.clear();

        m_connectedCallbacks.clear();

        updateWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::addWidget(const Widget::Ptr& widget, std::size_t row, std::size_t col, const Padding& padding, Alignment alignment)
    {
        // If the widget hasn't already been added then add it now
        if (std::find(getWidgets().begin(), getWidgets().end(), widget) == getWidgets().end())
            add(widget);

        // Create the row if it did not exist yet
        if (m_gridWidgets.size() < row + 1)
        {
            m_gridWidgets.resize(row + 1);
            m_objPadding.resize(row + 1);
            m_objAlignment.resize(row + 1);
        }

        // Create the column if it did not exist yet
        if (m_gridWidgets[row].size() < col + 1)
        {
            m_gridWidgets[row].resize(col + 1, nullptr);
            m_objPadding[row].resize(col + 1);
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
        m_objPadding[row][col] = padding;
        m_objAlignment[row][col] = alignment;

        // Update the widgets
        updateWidgets();

        // Automatically update the widgets when their size changes
        m_connectedCallbacks[widget] = widget->connect("SizeChanged", [this](){ updateWidgets(); });
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

    void Grid::setWidgetPadding(const Widget::Ptr& widget, const Padding& padding)
    {
        // Find the widget in the grid
        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col] == widget)
                    setWidgetPadding(row, col, padding);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::setWidgetPadding(std::size_t row, std::size_t col, const Padding& padding)
    {
        if (((row < m_gridWidgets.size()) && (col < m_gridWidgets[row].size())) && (m_gridWidgets[row][col] != nullptr))
        {
            // Change padding of the widget
            m_objPadding[row][col] = padding;

            // Update all widgets
            updateWidgets();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Padding Grid::getWidgetPadding(const Widget::Ptr& widget) const
    {
        // Find the widget in the grid
        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col] == widget)
                    return getWidgetPadding(row, col);
            }
        }

        return {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Padding Grid::getWidgetPadding(std::size_t row, std::size_t col) const
    {
        if (((row < m_gridWidgets.size()) && (col < m_gridWidgets[row].size())) && (m_gridWidgets[row][col] != nullptr))
            return m_objPadding[row][col];
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

    bool Grid::mouseOnWidget(Vector2f pos) const
    {
        return FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Grid::save(SavingRenderersMap& renderers) const
    {
        auto node = Container::save(renderers);

        const auto& children = getWidgets();
        auto widgetsMap = getWidgetLocations();
        if (!widgetsMap.empty())
        {
            auto alignmentToString = [](Grid::Alignment alignment) -> std::string {
                switch (alignment)
                {
                case Grid::Alignment::Center:
                    return "Center";
                case Grid::Alignment::UpperLeft:
                    return "UpperLeft";
                case Grid::Alignment::Up:
                    return "Up";
                case Grid::Alignment::UpperRight:
                    return "UpperRight";
                case Grid::Alignment::Right:
                    return "Right";
                case Grid::Alignment::BottomRight:
                    return "BottomRight";
                case Grid::Alignment::Bottom:
                    return "Bottom";
                case Grid::Alignment::BottomLeft:
                    return "BottomLeft";
                case Grid::Alignment::Left:
                    return "Left";
                default:
                    throw Exception{"Invalid grid alignment encountered."};
                }
            };

            auto getWidgetsInGridString = [&](const Widget::Ptr& w) -> std::string {
                auto it = widgetsMap.find(w);
                if (it != widgetsMap.end())
                {
                    const auto row = it->second.first;
                    const auto col = it->second.second;
                    return "\"(" + to_string(row)
                         + ", " + to_string(col)
                         + ", " + getWidgetPadding(row, col).toString()
                         + ", " + alignmentToString(getWidgetAlignment(row, col))
                         + ")\"";
                }
                else
                    return "\"()\"";
            };

            std::string str = "[" + getWidgetsInGridString(children[0]);

            for (std::size_t i = 1; i < children.size(); ++i)
                str += ", " + getWidgetsInGridString(children[i]);

            str += "]";
            node->propertyValuePairs["GridWidgets"] = std::make_unique<DataIO::ValueNode>(str);
        }

        if (m_autoSize)
            node->propertyValuePairs.erase("Size");

        node->propertyValuePairs["AutoSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_autoSize));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Grid::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Container::load(node, renderers);

        if (node->propertyValuePairs["autosize"])
            setAutoSize(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["autosize"]->value).getBool());

        if (node->propertyValuePairs["gridwidgets"])
        {
            if (!node->propertyValuePairs["gridwidgets"]->listNode)
                throw Exception{"Failed to parse 'GridWidgets' property, expected a list as value"};

            const auto& elements = node->propertyValuePairs["gridwidgets"]->valueList;
            if (elements.size() != getWidgets().size())
                throw Exception{"Failed to parse 'GridWidgets' property, the amount of items has to match with the amount of child widgets"};

            for (unsigned int i = 0; i < elements.size(); ++i)
            {
                std::string str = elements[i];

                // Remove quotes
                if ((str.size() >= 2) && (str[0] == '"') && (str[str.size()-1] == '"'))
                    str = str.substr(1, str.size()-2);

                // Remove brackets
                if ((str.size() >= 2) && (str[0] == '(') && (str[str.size()-1] == ')'))
                    str = str.substr(1, str.size()-2);

                // Ignore empty values (which are widgets that have not been given a location in the grid)
                if (str.empty())
                    continue;

                int row;
                int col;
                Padding padding;
                auto alignment = Grid::Alignment::Center;

                std::size_t index = 0;
                std::size_t pos = str.find(',');
                if (pos == std::string::npos)
                    throw Exception{"Failed to parse 'GridWidgets' property. Expected list values to be in the form of '\"(row, column, (padding), alignment)\"'. Missing comma after row."};

                row = strToInt(str.substr(index, pos - index));
                index = pos + 1;

                pos = str.find(',', index);
                if (pos == std::string::npos)
                    throw Exception{"Failed to parse 'GridWidgets' property. Expected list values to be in the form of '\"(row, column, (padding), alignment)\"'. Missing comma after column."};

                col = strToInt(str.substr(index, pos - index));
                index = pos + 1;

                if (row < 0 || col < 0)
                    throw Exception{"Failed to parse 'GridWidgets' property, row and column have to be positive integers"};

                pos = str.find('(', index);
                if (pos == std::string::npos)
                    throw Exception{"Failed to parse 'GridWidgets' property. Expected list values to be in the form of '\"(row, column, (padding), alignment)\"'. Missing opening bracket for padding."};

                index = pos;
                pos = str.find(')', index);
                if (pos == std::string::npos)
                    throw Exception{"Failed to parse 'GridWidgets' property. Expected list values to be in the form of '\"(row, column, (padding), alignment)\"'. Missing closing bracket for padding."};

                padding = Deserializer::deserialize(ObjectConverter::Type::Outline, str.substr(index, pos+1 - index)).getOutline();
                index = pos + 1;

                pos = str.find(',', index);
                if (pos == std::string::npos)
                    throw Exception{"Failed to parse 'GridWidgets' property. Expected list values to be in the form of '\"(row, column, (padding), alignment)\"'. Missing comma after padding."};

                std::string alignmentStr = toLower(trim(str.substr(pos + 1)));
                if (alignmentStr == "center")
                    alignment = Grid::Alignment::Center;
                else if (alignmentStr == "upperleft")
                    alignment = Grid::Alignment::UpperLeft;
                else if (alignmentStr == "up")
                    alignment = Grid::Alignment::Up;
                else if (alignmentStr == "upperright")
                    alignment = Grid::Alignment::UpperRight;
                else if (alignmentStr == "right")
                    alignment = Grid::Alignment::Right;
                else if (alignmentStr == "bottomright")
                    alignment = Grid::Alignment::BottomRight;
                else if (alignmentStr == "bottom")
                    alignment = Grid::Alignment::Bottom;
                else if (alignmentStr == "bottomleft")
                    alignment = Grid::Alignment::BottomLeft;
                else if (alignmentStr == "left")
                    alignment = Grid::Alignment::Left;
                else
                    throw Exception{"Failed to parse 'GridWidgets' property. Invalid alignment '" + alignmentStr + "'."};

                addWidget(getWidgets()[i], static_cast<std::size_t>(row), static_cast<std::size_t>(col), padding, alignment);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Grid::getMinimumSize() const
    {
        // Calculate the required space to have all widgets in the grid.
        Vector2f minSize;

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
        Vector2f position;

        // Fill the entire space when a size was given
        Vector2f extraOffset;
        if (!m_autoSize)
        {
            const Vector2f minimumSize = getMinimumSize();
            if (getSize().x > minimumSize.x)
            {
                if (m_columnWidth.size() > 1)
                    extraOffset.x = (getSize().x - minimumSize.x) / (m_columnWidth.size() - 1);
                else
                    position.x += (getSize().x - minimumSize.x) / 2.f;
            }

            if (getSize().y > minimumSize.y)
            {
                if (m_rowHeight.size() > 1)
                    extraOffset.y = (getSize().y - minimumSize.y) / (m_rowHeight.size() - 1);
                else
                    position.y += (getSize().y - minimumSize.y) / 2.f;
            }
        }

        for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
        {
            const Vector2f previousPosition = position;

            for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
            {
                if (m_gridWidgets[row][col].get() == nullptr)
                {
                    position.x += m_columnWidth[col] + extraOffset.x;
                    continue;
                }

                Vector2f cellPosition = position;
                switch (m_objAlignment[row][col])
                {
                case Alignment::UpperLeft:
                    cellPosition.x += m_objPadding[row][col].getLeft();
                    cellPosition.y += m_objPadding[row][col].getTop();
                    break;

                case Alignment::Up:
                    cellPosition.x += m_objPadding[row][col].getLeft() + (((m_columnWidth[col] - m_objPadding[row][col].getLeft() - m_objPadding[row][col].getRight()) - m_gridWidgets[row][col]->getFullSize().x) / 2.f);
                    cellPosition.y += m_objPadding[row][col].getTop();
                    break;

                case Alignment::UpperRight:
                    cellPosition.x += m_columnWidth[col] - m_objPadding[row][col].getRight() - m_gridWidgets[row][col]->getFullSize().x;
                    cellPosition.y += m_objPadding[row][col].getTop();
                    break;

                case Alignment::Right:
                    cellPosition.x += m_columnWidth[col] - m_objPadding[row][col].getRight() - m_gridWidgets[row][col]->getFullSize().x;
                    cellPosition.y += m_objPadding[row][col].getTop() + (((m_rowHeight[row] - m_objPadding[row][col].getTop() - m_objPadding[row][col].getBottom()) - m_gridWidgets[row][col]->getFullSize().y) / 2.f);
                    break;

                case Alignment::BottomRight:
                    cellPosition.x += m_columnWidth[col] - m_objPadding[row][col].getRight() - m_gridWidgets[row][col]->getFullSize().x;
                    cellPosition.y += m_rowHeight[row] - m_objPadding[row][col].getBottom() - m_gridWidgets[row][col]->getFullSize().y;
                    break;

                case Alignment::Bottom:
                    cellPosition.x += m_objPadding[row][col].getLeft() + (((m_columnWidth[col] - m_objPadding[row][col].getLeft() - m_objPadding[row][col].getRight()) - m_gridWidgets[row][col]->getFullSize().x) / 2.f);
                    cellPosition.y += m_rowHeight[row] - m_objPadding[row][col].getBottom() - m_gridWidgets[row][col]->getFullSize().y;
                    break;

                case Alignment::BottomLeft:
                    cellPosition.x += m_objPadding[row][col].getLeft();
                    cellPosition.y += m_rowHeight[row] - m_objPadding[row][col].getBottom() - m_gridWidgets[row][col]->getFullSize().y;
                    break;

                case Alignment::Left:
                    cellPosition.x += m_objPadding[row][col].getLeft();
                    cellPosition.y += m_objPadding[row][col].getTop() + (((m_rowHeight[row] - m_objPadding[row][col].getTop() - m_objPadding[row][col].getBottom()) - m_gridWidgets[row][col]->getFullSize().y) / 2.f);
                    break;

                case Alignment::Center:
                    cellPosition.x += m_objPadding[row][col].getLeft() + (((m_columnWidth[col] - m_objPadding[row][col].getLeft() - m_objPadding[row][col].getRight()) - m_gridWidgets[row][col]->getFullSize().x) / 2.f);
                    cellPosition.y += m_objPadding[row][col].getTop() + (((m_rowHeight[row] - m_objPadding[row][col].getTop() - m_objPadding[row][col].getBottom()) - m_gridWidgets[row][col]->getFullSize().y) / 2.f);
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
                if (m_columnWidth[col] < m_gridWidgets[row][col]->getFullSize().x + m_objPadding[row][col].getLeft() + m_objPadding[row][col].getRight())
                    m_columnWidth[col] = m_gridWidgets[row][col]->getFullSize().x + m_objPadding[row][col].getLeft() + m_objPadding[row][col].getRight();

                // Remember the biggest row height
                if (m_rowHeight[row] < m_gridWidgets[row][col]->getFullSize().y + m_objPadding[row][col].getTop() + m_objPadding[row][col].getBottom())
                    m_rowHeight[row] = m_gridWidgets[row][col]->getFullSize().y + m_objPadding[row][col].getTop() + m_objPadding[row][col].getBottom();
            }
        }

        if (m_autoSize)
        {
            Vector2f size;
            for (std::size_t row = 0; row < m_gridWidgets.size(); ++row)
            {
                float rowWidth = 0;
                for (std::size_t col = 0; col < m_gridWidgets[row].size(); ++col)
                    rowWidth += m_columnWidth[col];

                size.x = std::max(size.x, rowWidth);
                size.y += m_rowHeight[row];
            }

            Container::setSize(size);
        }

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
