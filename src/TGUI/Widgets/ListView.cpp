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


#include <TGUI/Widgets/ListView.hpp>
#include <TGUI/Keyboard.hpp>
#include <TGUI/Clipping.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListView::ListView()
    {
        m_type = "ListView";
        m_draggableWidget = true;

        m_horizontalScrollbar->setSize(m_horizontalScrollbar->getSize().y, m_horizontalScrollbar->getSize().x);

        m_renderer = aurora::makeCopied<ListViewRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setItemHeight(static_cast<unsigned int>(Text::getLineHeight(m_fontCached, m_textSize) * 1.25f));
        setSize({m_itemHeight * 12,
                 getHeaderHeight() + getHeaderSeparatorHeight() + (m_itemHeight * 6)
                 + m_paddingCached.getTop() + m_paddingCached.getBottom() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListView::Ptr ListView::create()
    {
        return std::make_shared<ListView>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListView::Ptr ListView::copy(ListView::ConstPtr listView)
    {
        if (listView)
            return std::static_pointer_cast<ListView>(listView->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListViewRenderer* ListView::getSharedRenderer()
    {
        return aurora::downcast<ListViewRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ListViewRenderer* ListView::getSharedRenderer() const
    {
        return aurora::downcast<const ListViewRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListViewRenderer* ListView::getRenderer()
    {
        return aurora::downcast<ListViewRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ListViewRenderer* ListView::getRenderer() const
    {
        return aurora::downcast<const ListViewRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        m_verticalScrollbar->setPosition(getSize().x - m_bordersCached.getRight() - m_verticalScrollbar->getSize().x, m_bordersCached.getTop());
        m_horizontalScrollbar->setPosition(m_bordersCached.getLeft(), getSize().y - m_bordersCached.getBottom() - m_horizontalScrollbar->getSize().y);
        updateScrollbars();

        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ListView::addColumn(const sf::String& text, float width, ColumnAlignment alignment)
    {
        Column column;
        column.text = createHeaderText(text);
        column.alignment = alignment;
        column.designWidth = width;
        if (width)
            column.width = width;
        else
            column.width = calculateAutoColumnWidth(column.text);

        m_columns.push_back(std::move(column));
        updateHorizontalScrollbarMaximum();

        return m_columns.size()-1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setColumnText(std::size_t index, const sf::String& text)
    {
        if (index >= m_columns.size())
        {
            TGUI_PRINT_WARNING("setColumnText called with invalid index.");
            return;
        }

        m_columns[index].text = createHeaderText(text);
        if (m_columns[index].designWidth == 0)
            m_columns[index].width = calculateAutoColumnWidth(m_columns[index].text);

        updateHorizontalScrollbarMaximum();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ListView::getColumnText(std::size_t index) const
    {
        if (index < m_columns.size())
            return m_columns[index].text.getString();
        else
        {
            TGUI_PRINT_WARNING("getColumnText called with invalid index.");
            return "";
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setColumnWidth(std::size_t index, float width)
    {
        if (index >= m_columns.size())
        {
            TGUI_PRINT_WARNING("setColumnWidth called with invalid index.");
            return;
        }

        m_columns[index].designWidth = width;
        if (width)
            m_columns[index].width = width;
        else
            m_columns[index].width = calculateAutoColumnWidth(m_columns[index].text);

        updateHorizontalScrollbarMaximum();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float ListView::getColumnWidth(std::size_t index) const
    {
        if (index < m_columns.size())
            return m_columns[index].width;
        else
        {
            TGUI_PRINT_WARNING("getColumnWidth called with invalid index.");
            return 0;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::removeAllColumns()
    {
        m_columns.clear();
        updateHorizontalScrollbarMaximum();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ListView::getColumnCount() const
    {
        return m_columns.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setHeaderHeight(float height)
    {
        m_requestedHeaderHeight = height;
        updateVerticalScrollbarMaximum();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float ListView::getHeaderHeight() const
    {
        if (m_requestedHeaderHeight > 0)
            return m_requestedHeaderHeight;
        else
            return m_itemHeight * 1.25f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float ListView::getCurrentHeaderHeight() const
    {
        if (m_headerVisible && !m_columns.empty())
            return getHeaderHeight() + getHeaderSeparatorHeight();
        else
            return 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setColumnAlignment(std::size_t columnIndex, ColumnAlignment alignment)
    {
        if (columnIndex < m_columns.size())
            m_columns[columnIndex].alignment = alignment;
        else
        {
            TGUI_PRINT_WARNING("setColumnAlignment called with invalid columnIndex.");
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListView::ColumnAlignment ListView::getColumnAlignment(std::size_t columnIndex) const
    {
        if (columnIndex < m_columns.size())
            return m_columns[columnIndex].alignment;
        else
        {
            TGUI_PRINT_WARNING("getColumnAlignment called with invalid columnIndex.");
            return ColumnAlignment::Left;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setHeaderVisible(bool showHeader)
    {
        m_headerVisible = showHeader;
        updateVerticalScrollbarMaximum();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListView::getHeaderVisible() const
    {
        return m_headerVisible;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ListView::addItem(const sf::String& text)
    {
        TGUI_EMPLACE_BACK(item, m_items)
        item.texts.push_back(createText(text));
        item.icon.setOpacity(m_opacityCached);

        updateVerticalScrollbarMaximum();

        // Scroll down when auto-scrolling is enabled
        if (m_autoScroll && (m_verticalScrollbar->getViewportSize() < m_verticalScrollbar->getMaximum()))
            m_verticalScrollbar->setValue(m_verticalScrollbar->getMaximum() - m_verticalScrollbar->getViewportSize());

        return m_items.size()-1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ListView::addItem(const std::vector<sf::String>& itemTexts)
    {
        TGUI_EMPLACE_BACK(item, m_items)
        item.texts.reserve(itemTexts.size());
        for (const auto& text : itemTexts)
            item.texts.push_back(createText(text));

        item.icon.setOpacity(m_opacityCached);

        updateVerticalScrollbarMaximum();

        // Scroll down when auto-scrolling is enabled
        if (m_autoScroll && (m_verticalScrollbar->getViewportSize() < m_verticalScrollbar->getMaximum()))
            m_verticalScrollbar->setValue(m_verticalScrollbar->getMaximum() - m_verticalScrollbar->getViewportSize());

        return m_items.size()-1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::addMultipleItems(const std::vector<std::vector<sf::String>>& items)
    {
        for (unsigned int i = 0; i < items.size(); ++i)
        {
            TGUI_EMPLACE_BACK(item, m_items)
            item.texts.reserve(items[i].size());
            for (const auto& text : items[i])
                item.texts.push_back(createText(text));

            item.icon.setOpacity(m_opacityCached);
        }

        updateVerticalScrollbarMaximum();

        // Scroll down when auto-scrolling is enabled
        if (m_autoScroll && (m_verticalScrollbar->getViewportSize() < m_verticalScrollbar->getMaximum()))
            m_verticalScrollbar->setValue(m_verticalScrollbar->getMaximum() - m_verticalScrollbar->getViewportSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListView::changeItem(std::size_t index, const std::vector<sf::String>& itemTexts)
    {
        if (index >= m_items.size())
            return false;

        Item& item = m_items[index];
        item.texts.clear();
        item.texts.reserve(itemTexts.size());
        for (const auto& text : itemTexts)
            item.texts.push_back(createText(text));

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListView::changeSubItem(std::size_t index, std::size_t column, const sf::String& itemText)
    {
        if (index >= m_items.size())
            return false;

        Item& item = m_items[index];
        if (column >= item.texts.size())
            item.texts.resize(column + 1);

        item.texts[column] = createText(itemText);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListView::removeItem(std::size_t index)
    {
        // Update the hovered item
        if (m_hoveredItem >= 0)
        {
            if (m_hoveredItem == static_cast<int>(index))
                updateHoveredItem(-1);
            else if (m_hoveredItem > static_cast<int>(index))
                m_hoveredItem = m_hoveredItem - 1;
        }

        // Update the selected items
        if (!m_selectedItems.empty())
        {
            if (m_selectedItems.count(index))
            {
                m_selectedItems.erase(index);
                setItemColor(index, m_textColorCached);
                if (!m_multiSelect)
                    onItemSelect.emit(this, -1);
            }

            // Don't call updateSelectedItem here, there should not be no callback and the item hasn't been erased yet so it would point to the wrong place
            decltype(m_selectedItems) newSelectedItems;
            for (const auto selectedItem : m_selectedItems)
            {
                if (selectedItem < index)
                    newSelectedItems.insert(selectedItem);
                else if (selectedItem > index)
                {
                    newSelectedItems.insert(selectedItem - 1);
                    setItemColor(selectedItem, m_textColorCached);
                }
            }

            m_selectedItems = newSelectedItems;
            updateSelectedAndhoveredItemColors();
        }

        if (index >= m_items.size())
            return false;

        const bool wasIconSet = m_items[index].icon.isSet();
        m_items.erase(m_items.begin() + index);

        if (wasIconSet)
        {
            --m_iconCount;

            const float oldMaxIconWidth = m_maxIconWidth;
            m_maxIconWidth = 0;
            if (m_iconCount > 0)
            {
                // Rescan all items to find the largest icon
                for (const auto& item : m_items)
                {
                    if (!item.icon.isSet())
                        continue;

                    m_maxIconWidth = std::max(m_maxIconWidth, item.icon.getSize().x);
                    if (m_maxIconWidth == oldMaxIconWidth)
                        break;
                }
            }
        }

        updateVerticalScrollbarMaximum();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::removeAllItems()
    {
        updateSelectedItem(-1);
        updateHoveredItem(-1);

        m_items.clear();

        m_iconCount = 0;
        m_maxIconWidth = 0;

        updateVerticalScrollbarMaximum();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setSelectedItem(std::size_t index)
    {
        if (index >= m_items.size())
        {
            updateSelectedItem(-1);
            return;
        }

        updateSelectedItem(static_cast<int>(index));

        // Move the scrollbar
        if (index * getItemHeight() < m_verticalScrollbar->getValue())
            m_verticalScrollbar->setValue(static_cast<unsigned int>(index * getItemHeight()));
        else if (static_cast<unsigned int>(index + 1) * getItemHeight() > m_verticalScrollbar->getValue() + m_verticalScrollbar->getViewportSize())
            m_verticalScrollbar->setValue(static_cast<unsigned int>(index + 1) * getItemHeight() - m_verticalScrollbar->getViewportSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setSelectedItems(const std::set<std::size_t>& indices)
    {
        if (!m_multiSelect)
        {
            updateSelectedItem(indices.empty() ? -1 : static_cast<int>(*indices.begin()));
            return;
        }

        if (m_selectedItems == indices)
            return;

        for (const auto index : m_selectedItems)
        {
            if (indices.find(index) == indices.end())
                setItemColor(index, m_textColorCached);
        }

        m_selectedItems = indices;
        updateSelectedAndhoveredItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TGUI_REMOVE_DEPRECATED_CODE
    void ListView::deselectItem()
    {
        updateSelectedItem(-1);
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::deselectItems()
    {
        updateSelectedItem(-1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ListView::getSelectedItemIndex() const
    {
        if (!m_selectedItems.empty())
            return static_cast<int>(*m_selectedItems.begin());
        else
            return -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setMultiSelect(bool multiSelect)
    {
        m_multiSelect = multiSelect;
        if (!m_multiSelect && m_selectedItems.size() > 1)
            updateSelectedItem(static_cast<int>(*m_selectedItems.begin()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListView::getMultiSelect() const
    {
        return m_multiSelect;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::set<std::size_t> ListView::getSelectedItemIndices() const
    {
        return m_selectedItems;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setItemIcon(std::size_t index, const Texture& texture)
    {
        if (index >= m_items.size())
        {
            TGUI_PRINT_WARNING("setItemIcon called with invalid index.");
            return;
        }

        const bool wasIconSet = m_items[index].icon.isSet();
        m_items[index].icon.setTexture(texture);

        if (m_items[index].icon.isSet())
        {
            m_maxIconWidth = std::max(m_maxIconWidth, m_items[index].icon.getSize().x);
            if (!wasIconSet)
                ++m_iconCount;
        }
        else if (wasIconSet)
        {
            --m_iconCount;

            const float oldMaxIconWidth = m_maxIconWidth;
            m_maxIconWidth = 0;
            if (m_iconCount > 0)
            {
                // Rescan all items to find the largest icon
                for (const auto& item : m_items)
                {
                    if (!item.icon.isSet())
                        continue;

                    m_maxIconWidth = std::max(m_maxIconWidth, item.icon.getSize().x);
                    if (m_maxIconWidth == oldMaxIconWidth)
                        break;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Texture ListView::getItemIcon(std::size_t index) const
    {
        if (index < m_items.size())
            return m_items[index].icon.getTexture();
        else
        {
            TGUI_PRINT_WARNING("getItemIcon called with invalid index.");
            return {};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ListView::getItemCount() const
    {
        return m_items.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ListView::getItemCell(std::size_t rowIndex, std::size_t columnIndex) const
    {
        if (rowIndex >= m_items.size())
            return "";

        if (columnIndex != 0 && columnIndex >= m_columns.size())
            return "";

        if (columnIndex < m_items[rowIndex].texts.size())
            return m_items[rowIndex].texts[columnIndex].getString();

        return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ListView::getItem(std::size_t index) const
    {
        if (index >= m_items.size())
            return "";

        if (m_items[index].texts.empty())
            return "";

        return m_items[index].texts[0].getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String> ListView::getItemRow(std::size_t index) const
    {
        std::vector<sf::String> row;
        if (index < m_items.size())
        {
            for (const auto& text : m_items[index].texts)
                row.push_back(text.getString());
        }

        row.resize(std::max<std::size_t>(1, m_columns.size()));
        return row;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::sort(std::size_t index, const std::function<bool(const sf::String&, const sf::String&)>& cmp)
    {
        if (index >= m_items.size())
            return;

        std::sort(m_items.begin(), m_items.end(),
            [index, &cmp](const ListView::Item &a, const ListView::Item& b)
            {
                sf::String s1;
                if (index < a.texts.size())
                    s1 = a.texts[index].getString();

                sf::String s2;
                if (index < b.texts.size())
                    s2 = b.texts[index].getString();

                return cmp(s1, s2);
            });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String> ListView::getItems() const
    {
        std::vector<sf::String> items(m_items.size());

        for (std::size_t i = 0; i < m_items.size(); i++)
            items[i] = getItemCell(i, 0);

        return items;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::vector<sf::String>> ListView::getItemRows() const
    {
        std::vector<std::vector<sf::String>> rows;

        for (const auto& item : m_items)
        {
            std::vector<sf::String> row;
            for (const auto& text : item.texts)
                row.push_back(text.getString());

            row.resize(std::max<std::size_t>(1, m_columns.size()));
            rows.push_back(std::move(row));
        }

        return rows;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setItemHeight(unsigned int itemHeight)
    {
        // Set the new heights
        m_itemHeight = itemHeight;
        if (m_requestedTextSize == 0)
        {
            m_textSize = Text::findBestTextSize(m_fontCached, itemHeight * 0.8f);
            for (auto& item : m_items)
            {
                for (auto& text : item.texts)
                    text.setCharacterSize(m_textSize);
            }
        }

        updateVerticalScrollbarMaximum();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListView::getItemHeight() const
    {
        return m_itemHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setTextSize(unsigned int textSize)
    {
        m_requestedTextSize = textSize;

        if (textSize)
            m_textSize = textSize;
        else
            m_textSize = Text::findBestTextSize(m_fontCached, m_itemHeight * 0.8f);

        for (auto& item : m_items)
        {
            for (auto& text : item.texts)
                text.setCharacterSize(m_textSize);
        }

        const unsigned int headerTextSize = getHeaderTextSize();
        for (Column& column : m_columns)
            column.text.setCharacterSize(headerTextSize);

        m_horizontalScrollbar->setScrollAmount(m_textSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setHeaderTextSize(unsigned int textSize)
    {
        m_headerTextSize = textSize;

        const unsigned int headerTextSize = getHeaderTextSize();
        for (Column& column : m_columns)
        {
            column.text.setCharacterSize(headerTextSize);

            if (column.designWidth == 0)
                column.width = calculateAutoColumnWidth(column.text);
        }

        updateHorizontalScrollbarMaximum();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListView::getHeaderTextSize() const
    {
        if (m_headerTextSize)
            return m_headerTextSize;
        else
            return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setSeparatorWidth(unsigned int width)
    {
        m_separatorWidth = width;
        updateHorizontalScrollbarMaximum();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListView::getSeparatorWidth() const
    {
        return m_separatorWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setHeaderSeparatorHeight(unsigned int height)
    {
        m_headerSeparatorHeight = height;
        updateVerticalScrollbarMaximum();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListView::getHeaderSeparatorHeight() const
    {
        return m_headerSeparatorHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setGridLinesWidth(unsigned int width)
    {
        m_gridLinesWidth = width;
        updateHorizontalScrollbarMaximum();
        updateVerticalScrollbarMaximum();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListView::getGridLinesWidth() const
    {
        return m_gridLinesWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setAutoScroll(bool autoScroll)
    {
        m_autoScroll = autoScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListView::getAutoScroll() const
    {
        return m_autoScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setShowVerticalGridLines(bool showGridLines)
    {
        m_showVerticalGridLines = showGridLines;
        updateHorizontalScrollbarMaximum();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListView::getShowVerticalGridLines() const
    {
        return m_showVerticalGridLines;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setShowHorizontalGridLines(bool showGridLines)
    {
        m_showHorizontalGridLines = showGridLines;
        updateVerticalScrollbarMaximum();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListView::getShowHorizontalGridLines() const
    {
        return m_showHorizontalGridLines;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setExpandLastColumn(bool expand)
    {
        m_expandLastColumn = expand;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListView::getExpandLastColumn() const
    {
        return m_expandLastColumn;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setVerticalScrollbarPolicy(Scrollbar::Policy policy)
    {
        m_verticalScrollbarPolicy = policy;

        if (policy == Scrollbar::Policy::Always)
        {
            m_verticalScrollbar->setVisible(true);
            m_verticalScrollbar->setAutoHide(false);
        }
        else if (policy == Scrollbar::Policy::Never)
        {
            m_verticalScrollbar->setVisible(false);
        }
        else // Scrollbar::Policy::Automatic
        {
            m_verticalScrollbar->setVisible(true);
            m_verticalScrollbar->setAutoHide(true);
        }

        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Policy ListView::getVerticalScrollbarPolicy() const
    {
        return m_verticalScrollbarPolicy;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setHorizontalScrollbarPolicy(Scrollbar::Policy policy)
    {
        m_horizontalScrollbarPolicy = policy;

        if (policy == Scrollbar::Policy::Always)
        {
            m_horizontalScrollbar->setVisible(true);
            m_horizontalScrollbar->setAutoHide(false);
        }
        else if (policy == Scrollbar::Policy::Never)
        {
            m_horizontalScrollbar->setVisible(false);
        }
        else // Scrollbar::Policy::Automatic
        {
            m_horizontalScrollbar->setVisible(true);
            m_horizontalScrollbar->setAutoHide(true);
        }

        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Policy ListView::getHorizontalScrollbarPolicy() const
    {
        return m_horizontalScrollbarPolicy;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setVerticalScrollbarValue(unsigned int value)
    {
        m_verticalScrollbar->setValue(value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListView::getVerticalScrollbarValue() const
    {
        return m_verticalScrollbar->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setHorizontalScrollbarValue(unsigned int value)
    {
        m_horizontalScrollbar->setValue(value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListView::getHorizontalScrollbarValue() const
    {
        return m_horizontalScrollbar->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListView::mouseOnWidget(Vector2f pos) const
    {
        return FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();
        m_mouseDown = true;

        if (m_verticalScrollbar->mouseOnWidget(pos))
        {
            m_verticalScrollbar->leftMousePressed(pos);
        }
        else if (m_horizontalScrollbar->isShown() && m_horizontalScrollbar->mouseOnWidget(pos))
        {
            m_horizontalScrollbar->leftMousePressed(pos);
        }

        // Check if an item was clicked
        else if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(), m_bordersCached.getTop() + m_paddingCached.getTop() + getCurrentHeaderHeight(),
                           getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
        {
            updateHoveredItemByMousePos(pos);

            const bool mouseOnSelectedItem = (m_selectedItems.find(m_hoveredItem) != m_selectedItems.end());
            if (!mouseOnSelectedItem)
                m_possibleDoubleClick = -1;

            if (m_multiSelect && keyboard::isMultiselectModifierPressed())
            {
                if (mouseOnSelectedItem)
                    removeSelectedItem(m_hoveredItem);
                else
                    addSelectedItem(m_hoveredItem);

                m_lastMouseDownItem = m_hoveredItem;
            }
            else if (m_multiSelect && (m_hoveredItem >= 0) && keyboard::isShiftPressed())
            {
                if (m_lastMouseDownItem < 0)
                    m_lastMouseDownItem = m_hoveredItem;

                std::set<std::size_t> selectedItems;
                const std::size_t rangeStart = static_cast<std::size_t>(std::min(m_lastMouseDownItem, m_hoveredItem));
                const std::size_t rangeEnd = static_cast<std::size_t>(std::max(m_lastMouseDownItem, m_hoveredItem));
                for (std::size_t i = rangeStart; i <= rangeEnd; ++i)
                    selectedItems.insert(i);

                setSelectedItems(selectedItems);
            }
            else
            {
                updateSelectedItem(m_hoveredItem);
                m_lastMouseDownItem = m_hoveredItem;
            }

            // Check if you double-clicked
            if (m_possibleDoubleClick >= 0)
            {
                if (!m_selectedItems.empty())
                    onDoubleClick.emit(this, m_possibleDoubleClick);
                m_possibleDoubleClick = -1;
            }
            else // This is the first click
            {
                m_animationTimeElapsed = {};
                m_possibleDoubleClick = m_hoveredItem;
            }
        }

        // Check if the header was clicked
        else if ((getCurrentHeaderHeight() > 0)
              && FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(), m_bordersCached.getTop() + m_paddingCached.getTop(),
                           getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), getCurrentHeaderHeight()}.contains(pos))
        {
            m_mouseOnHeaderIndex = getColumnIndexBelowMouse(pos.x);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::leftMouseReleased(Vector2f pos)
    {
        pos -= getPosition();

        if (m_verticalScrollbar->isShown() && m_verticalScrollbar->isMouseDown())
            m_verticalScrollbar->leftMouseReleased(pos);

        if (m_horizontalScrollbar->isShown() && m_horizontalScrollbar->isMouseDown())
            m_horizontalScrollbar->leftMouseReleased(pos);

        if (m_mouseOnHeaderIndex >= 0)
        {
            if ((getCurrentHeaderHeight() > 0)
              && FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(), m_bordersCached.getTop() + m_paddingCached.getTop(),
                           getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), getCurrentHeaderHeight()}.contains(pos))
            {
                if (m_mouseOnHeaderIndex == getColumnIndexBelowMouse(pos.x))
                    onHeaderClick.emit(this, m_mouseOnHeaderIndex);
            }

            m_mouseOnHeaderIndex = -1;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::rightMousePressed(Vector2f pos)
    {
        pos -= getPosition();
        if ((m_verticalScrollbar->isShown() && m_verticalScrollbar->mouseOnWidget(pos)) || (m_horizontalScrollbar->isShown() && m_horizontalScrollbar->mouseOnWidget(pos)))
            return;

        int itemIndex = -1;
        if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(), m_bordersCached.getTop() + m_paddingCached.getTop() + getCurrentHeaderHeight(),
                      getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
        {
            updateHoveredItemByMousePos(pos);
            itemIndex = m_hoveredItem;
        }

        onRightClick.emit(this, itemIndex);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        const int oldHoveredItem = m_hoveredItem;
        updateHoveredItem(-1);

        // Check if the mouse event should go to the scrollbar
        if ((m_verticalScrollbar->isMouseDown() && m_verticalScrollbar->isMouseDownOnThumb()) || m_verticalScrollbar->mouseOnWidget(pos))
        {
            m_verticalScrollbar->mouseMoved(pos);
        }
        else if ((m_horizontalScrollbar->isMouseDown() && m_horizontalScrollbar->isMouseDownOnThumb()) || m_horizontalScrollbar->mouseOnWidget(pos))
        {
            m_horizontalScrollbar->mouseMoved(pos);
        }
        else // Mouse not on scrollbar or dragging the scrollbar thumb
        {
            m_verticalScrollbar->mouseNoLongerOnWidget();
            m_horizontalScrollbar->mouseNoLongerOnWidget();

            // Find out on which item the mouse is hovered
            if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(),
                          m_bordersCached.getTop() + m_paddingCached.getTop() + getCurrentHeaderHeight(),
                          getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                          getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
            {
                updateHoveredItemByMousePos(pos);

                // If the mouse is held down then select the item below the mouse
                if ((m_hoveredItem != oldHoveredItem) && m_mouseDown && !m_verticalScrollbar->isMouseDown())
                {
                    const bool mouseOnSelectedItem = (m_selectedItems.find(m_hoveredItem) != m_selectedItems.end());
                    if (!mouseOnSelectedItem)
                        m_possibleDoubleClick = -1;

                    if (m_multiSelect)
                    {
                        if (keyboard::isMultiselectModifierPressed())
                        {
                            // If the control/command key is pressed then toggle the selection of the item below the mouse
                            if (mouseOnSelectedItem)
                                removeSelectedItem(m_hoveredItem);
                            else
                                addSelectedItem(m_hoveredItem);
                        }
                        else // Control/command isn't pressed. Select items between current position and item where mouse went down
                        {
                            if (m_hoveredItem >= 0)
                            {
                                if (m_lastMouseDownItem < 0)
                                    m_lastMouseDownItem = m_hoveredItem;

                                std::set<std::size_t> selectedItems;
                                const std::size_t rangeStart = static_cast<std::size_t>(std::min(m_lastMouseDownItem, m_hoveredItem));
                                const std::size_t rangeEnd = static_cast<std::size_t>(std::max(m_lastMouseDownItem, m_hoveredItem));
                                for (std::size_t i = rangeStart; i <= rangeEnd; ++i)
                                    selectedItems.insert(i);

                                setSelectedItems(selectedItems);
                            }
                        }
                    }
                    else // Only one item can be selected at once. Select the one below the mouse.
                        updateSelectedItem(m_hoveredItem);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListView::mouseWheelScrolled(float delta, Vector2f pos)
    {
        if (m_horizontalScrollbar->isShown()
            && (!m_verticalScrollbar->isShown()
                || m_horizontalScrollbar->mouseOnWidget(pos - getPosition())
                || keyboard::isShiftPressed()))
        {
            m_horizontalScrollbar->mouseWheelScrolled(delta, pos - getPosition());
            mouseMoved(pos); // Update on which item the mouse is hovered
            return true;
        }
        else if (m_verticalScrollbar->isShown())
        {
            m_verticalScrollbar->mouseWheelScrolled(delta, pos - getPosition());
            mouseMoved(pos); // Update on which item the mouse is hovered
            return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::mouseNoLongerOnWidget()
    {
        Widget::mouseNoLongerOnWidget();
        m_verticalScrollbar->mouseNoLongerOnWidget();
        m_horizontalScrollbar->mouseNoLongerOnWidget();

        updateHoveredItem(-1);

        m_possibleDoubleClick = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::leftMouseButtonNoLongerDown()
    {
        Widget::leftMouseButtonNoLongerDown();
        m_verticalScrollbar->leftMouseButtonNoLongerDown();
        m_horizontalScrollbar->leftMouseButtonNoLongerDown();
        m_mouseOnHeaderIndex = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ListView::getSignal(std::string signalName)
    {
        if (signalName == toLower(onItemSelect.getName()))
            return onItemSelect;
        else if (signalName == toLower(onDoubleClick.getName()))
            return onDoubleClick;
        else if (signalName == toLower(onRightClick.getName()))
            return onRightClick;
        else if (signalName == toLower(onHeaderClick.getName()))
            return onHeaderClick;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            setSize(m_size);
        }
        else if (property == "textcolor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            updateItemColors();

            if (!m_headerTextColorCached.isSet())
                updateHeaderTextsColor();
        }
        else if (property == "textcolorhover")
        {
            m_textColorHoverCached = getSharedRenderer()->getTextColorHover();
            updateItemColors();
        }
        else if (property == "selectedtextcolor")
        {
            m_selectedTextColorCached = getSharedRenderer()->getSelectedTextColor();
            updateItemColors();
        }
        else if (property == "selectedtextcolorhover")
        {
            m_selectedTextColorHoverCached = getSharedRenderer()->getSelectedTextColorHover();
            updateItemColors();
        }
        else if (property == "scrollbar")
        {
            m_verticalScrollbar->setRenderer(getSharedRenderer()->getScrollbar());
            m_horizontalScrollbar->setRenderer(getSharedRenderer()->getScrollbar());

            // If no scrollbar width was set then we may need to use the one from the texture
            if (!getSharedRenderer()->getScrollbarWidth())
            {
                m_verticalScrollbar->setSize({m_verticalScrollbar->getDefaultWidth(), m_verticalScrollbar->getSize().y});
                m_horizontalScrollbar->setSize({m_horizontalScrollbar->getSize().x, m_horizontalScrollbar->getDefaultWidth()});
                setSize(m_size);
            }
        }
        else if (property == "scrollbarwidth")
        {
            const float width = getSharedRenderer()->getScrollbarWidth() ? getSharedRenderer()->getScrollbarWidth() : m_verticalScrollbar->getDefaultWidth();
            m_verticalScrollbar->setSize({width, m_verticalScrollbar->getSize().y});
            m_horizontalScrollbar->setSize({m_verticalScrollbar->getSize().x, width});
            setSize(m_size);
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "separatorcolor")
        {
            m_separatorColorCached = getSharedRenderer()->getSeparatorColor();
        }
        else if (property == "gridlinescolor")
        {
            m_gridLinesColorCached = getSharedRenderer()->getGridLinesColor();
        }
        else if (property == "headertextcolor")
        {
            m_headerTextColorCached = getSharedRenderer()->getHeaderTextColor();
            updateHeaderTextsColor();
        }
        else if (property == "headerbackgroundcolor")
        {
            m_headerBackgroundColorCached = getSharedRenderer()->getHeaderBackgroundColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "backgroundcolorhover")
        {
            m_backgroundColorHoverCached = getSharedRenderer()->getBackgroundColorHover();
        }
        else if (property == "selectedbackgroundcolor")
        {
            m_selectedBackgroundColorCached = getSharedRenderer()->getSelectedBackgroundColor();
        }
        else if (property == "selectedbackgroundcolorhover")
        {
            m_selectedBackgroundColorHoverCached = getSharedRenderer()->getSelectedBackgroundColorHover();
        }
        else if ((property == "opacity") || (property == "opacitydisabled"))
        {
            Widget::rendererChanged(property);

            m_verticalScrollbar->setInheritedOpacity(m_opacityCached);
            m_horizontalScrollbar->setInheritedOpacity(m_opacityCached);

            for (auto& column : m_columns)
                column.text.setOpacity(m_opacityCached);

            for (auto& item : m_items)
            {
                for (auto& text : item.texts)
                    text.setOpacity(m_opacityCached);

                item.icon.setOpacity(m_opacityCached);
            }
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            for (auto& column : m_columns)
                column.text.setFont(m_fontCached);

            for (auto& item : m_items)
            {
                for (auto& text : item.texts)
                    text.setFont(m_fontCached);
            }

            // Recalculate the text size with the new font
            if (m_requestedTextSize == 0)
            {
                m_textSize = Text::findBestTextSize(m_fontCached, m_itemHeight * 0.8f);
                for (auto& item : m_items)
                {
                    for (auto& text : item.texts)
                        text.setCharacterSize(m_textSize);
                }

                if (!m_headerTextSize)
                {
                    for (auto& column : m_columns)
                        column.text.setCharacterSize(m_textSize);
                }
            }

            // Recalculate the width of the columns if they depended on the header text
            for (auto& column : m_columns)
            {
                if (column.designWidth == 0)
                    column.width = calculateAutoColumnWidth(column.text);
            }
            updateHorizontalScrollbarMaximum();
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ListView::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        for (const auto& column : m_columns)
        {
            auto columnNode = std::make_unique<DataIO::Node>();
            columnNode->name = "Column";

            columnNode->propertyValuePairs["Text"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(column.text.getString()));
            if (column.designWidth)
                columnNode->propertyValuePairs["Width"] = std::make_unique<DataIO::ValueNode>(to_string(column.designWidth));

            if (column.alignment == ColumnAlignment::Center)
                columnNode->propertyValuePairs["Alignment"] = std::make_unique<DataIO::ValueNode>("Center");
            else if (column.alignment == ColumnAlignment::Right)
                columnNode->propertyValuePairs["Alignment"] = std::make_unique<DataIO::ValueNode>("Right");

            node->children.push_back(std::move(columnNode));
        }

        for (const auto& item : m_items)
        {
            auto itemNode = std::make_unique<DataIO::Node>();
            itemNode->name = "Item";

            if (!item.texts.empty())
            {
                std::string textsList = "[" + Serializer::serialize(item.texts[0].getString());
                for (std::size_t i = 1; i < item.texts.size(); ++i)
                    textsList += ", " + Serializer::serialize(item.texts[i].getString());
                textsList += "]";

                itemNode->propertyValuePairs["Texts"] = std::make_unique<DataIO::ValueNode>(textsList);
            }

            node->children.push_back(std::move(itemNode));
        }

        if (!m_autoScroll)
            node->propertyValuePairs["AutoScroll"] = std::make_unique<DataIO::ValueNode>("false");

        if (!m_headerVisible)
            node->propertyValuePairs["HeaderVisible"] = std::make_unique<DataIO::ValueNode>("false");

        if (m_headerTextSize)
            node->propertyValuePairs["HeaderTextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_headerTextSize));

        if (m_multiSelect)
            node->propertyValuePairs["MultiSelect"] = std::make_unique<DataIO::ValueNode>("true");

        if (!m_selectedItems.empty())
        {
            auto it = m_selectedItems.cbegin();
            std::string iList = "[" + Serializer::serialize(*it);
            for (++it; it != m_selectedItems.cend(); ++it)
                iList += ", " + Serializer::serialize(*it);

            iList += "]";

            node->propertyValuePairs["SelectedItemIndices"] = std::make_unique<DataIO::ValueNode>(iList);
        }

        if (m_gridLinesWidth != 1)
            node->propertyValuePairs["GridLinesWidth"] = std::make_unique<DataIO::ValueNode>(to_string(m_gridLinesWidth));

        if (m_showHorizontalGridLines)
            node->propertyValuePairs["ShowHorizontalGridLines"] = std::make_unique<DataIO::ValueNode>(to_string(m_showHorizontalGridLines));

        if (m_verticalScrollbarPolicy != Scrollbar::Policy::Automatic)
        {
            if (m_verticalScrollbarPolicy == Scrollbar::Policy::Always)
                node->propertyValuePairs["VerticalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Always");
            else if (m_verticalScrollbarPolicy == Scrollbar::Policy::Never)
                node->propertyValuePairs["VerticalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Never");
        }
        if (m_horizontalScrollbarPolicy != Scrollbar::Policy::Automatic)
        {
            if (m_horizontalScrollbarPolicy == Scrollbar::Policy::Always)
                node->propertyValuePairs["HorizontalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Always");
            else if (m_horizontalScrollbarPolicy == Scrollbar::Policy::Never)
                node->propertyValuePairs["HorizontalScrollbarPolicy"] = std::make_unique<DataIO::ValueNode>("Never");
        }

        node->propertyValuePairs["HeaderVisible"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_headerVisible));
        node->propertyValuePairs["HeaderHeight"] = std::make_unique<DataIO::ValueNode>(to_string(m_requestedHeaderHeight));
        node->propertyValuePairs["SeparatorWidth"] = std::make_unique<DataIO::ValueNode>(to_string(m_separatorWidth));
        node->propertyValuePairs["HeaderSeparatorHeight"] = std::make_unique<DataIO::ValueNode>(to_string(m_headerSeparatorHeight));
        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        node->propertyValuePairs["ItemHeight"] = std::make_unique<DataIO::ValueNode>(to_string(m_itemHeight));
        node->propertyValuePairs["ShowVerticalGridLines"] = std::make_unique<DataIO::ValueNode>(to_string(m_showVerticalGridLines));
        node->propertyValuePairs["ExpandLastColumn"] = std::make_unique<DataIO::ValueNode>(to_string(m_expandLastColumn));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        for (const auto& childNode : node->children)
        {
            if (toLower(childNode->name) != "column")
                continue;

            sf::String text;
            float width = 0;
            ColumnAlignment alignment = ColumnAlignment::Left;

            if (childNode->propertyValuePairs["text"])
                text = Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs["text"]->value).getString();
            if (childNode->propertyValuePairs["width"])
                width = Deserializer::deserialize(ObjectConverter::Type::Number, childNode->propertyValuePairs["width"]->value).getNumber();

            if (childNode->propertyValuePairs["alignment"])
            {
                std::string alignmentString = toLower(Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs["alignment"]->value).getString());
                if (alignmentString == "right")
                    alignment = ColumnAlignment::Right;
                else if (alignmentString == "center")
                    alignment = ColumnAlignment::Center;
                else if (alignmentString != "left")
                    throw Exception{"Failed to parse Alignment property, found unknown value."};
            }

            addColumn(text, width, alignment);
        }

        for (const auto& childNode : node->children)
        {
            if (toLower(childNode->name) != "item")
                continue;

            if (!childNode->propertyValuePairs["texts"])
                throw Exception{"Failed to parse 'Item' property, no Texts property found"};
            if (!childNode->propertyValuePairs["texts"]->listNode)
                throw Exception{"Failed to parse 'Texts' property inside the 'Item' property, expected a list as value"};

            std::vector<sf::String> itemRow;
            itemRow.reserve(childNode->propertyValuePairs["texts"]->valueList.size());
            for (const auto& item : childNode->propertyValuePairs["texts"]->valueList)
                itemRow.push_back(Deserializer::deserialize(ObjectConverter::Type::String, item).getString());

            addItem(itemRow);
        }

        if (node->propertyValuePairs["autoscroll"])
            setAutoScroll(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["autoscroll"]->value).getBool());
        if (node->propertyValuePairs["headervisible"])
            setHeaderVisible(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["headervisible"]->value).getBool());
        if (node->propertyValuePairs["headerheight"])
            setHeaderHeight(strToFloat(node->propertyValuePairs["headerheight"]->value));
        if (node->propertyValuePairs["headertextsize"])
            setHeaderTextSize(strToInt(node->propertyValuePairs["headertextsize"]->value));
        if (node->propertyValuePairs["separatorwidth"])
            setSeparatorWidth(strToInt(node->propertyValuePairs["separatorwidth"]->value));
        if (node->propertyValuePairs["headerseparatorheight"])
            setHeaderSeparatorHeight(strToInt(node->propertyValuePairs["headerseparatorheight"]->value));
        if (node->propertyValuePairs["textsize"])
            setTextSize(strToInt(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["itemheight"])
            setItemHeight(strToInt(node->propertyValuePairs["itemheight"]->value));
        if (node->propertyValuePairs["multiselect"])
            setMultiSelect(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["multiselect"]->value).getBool());
#ifndef TGUI_NEXT
        if (node->propertyValuePairs["selecteditemindex"])
            setSelectedItem(strToInt(node->propertyValuePairs["selecteditemindex"]->value));
#endif
        if (node->propertyValuePairs["selecteditemindices"])
        {
            if (!node->propertyValuePairs["selecteditemindices"]->listNode)
                throw Exception{"Failed to parse 'SelectedItemIndices' property, expected a list as value"};

            for (const auto& item : node->propertyValuePairs["selecteditemindices"]->valueList)
                addSelectedItem(strToInt(item));
        }
        if (node->propertyValuePairs["gridlineswidth"])
            setGridLinesWidth(strToInt(node->propertyValuePairs["gridlineswidth"]->value));
        if (node->propertyValuePairs["showhorizontalgridlines"])
            setShowHorizontalGridLines(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["showhorizontalgridlines"]->value).getBool());
        if (node->propertyValuePairs["showverticalgridlines"])
            setShowVerticalGridLines(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["showverticalgridlines"]->value).getBool());
        if (node->propertyValuePairs["expandlastcolumn"])
            setExpandLastColumn(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["expandlastcolumn"]->value).getBool());

        if (node->propertyValuePairs["verticalscrollbarpolicy"])
        {
            std::string policy = toLower(trim(node->propertyValuePairs["verticalscrollbarpolicy"]->value));
            if (policy == "automatic")
                setVerticalScrollbarPolicy(Scrollbar::Policy::Automatic);
            else if (policy == "always")
                setVerticalScrollbarPolicy(Scrollbar::Policy::Always);
            else if (policy == "never")
                setVerticalScrollbarPolicy(Scrollbar::Policy::Never);
            else
                throw Exception{"Failed to parse VerticalScrollbarPolicy property, found unknown value."};
        }

        if (node->propertyValuePairs["horizontalscrollbarpolicy"])
        {
            std::string policy = toLower(trim(node->propertyValuePairs["horizontalscrollbarpolicy"]->value));
            if (policy == "automatic")
                setHorizontalScrollbarPolicy(Scrollbar::Policy::Automatic);
            else if (policy == "always")
                setHorizontalScrollbarPolicy(Scrollbar::Policy::Always);
            else if (policy == "never")
                setHorizontalScrollbarPolicy(Scrollbar::Policy::Never);
            else
                throw Exception{"Failed to parse HorizontalScrollbarPolicy property, found unknown value."};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ListView::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Text ListView::createText(const sf::String& caption)
    {
        Text text;
        text.setFont(m_fontCached);
        text.setColor(m_textColorCached);
        text.setOpacity(m_opacityCached);
        text.setCharacterSize(m_textSize);
        text.setString(caption);
        return text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Text ListView::createHeaderText(const sf::String& caption)
    {
        Text text;
        text.setFont(m_fontCached);
        text.setOpacity(m_opacityCached);
        text.setCharacterSize(getHeaderTextSize());
        text.setString(caption);

        if (m_headerTextColorCached.isSet())
            text.setColor(m_headerTextColorCached);
        else
            text.setColor(m_textColorCached);

        return text;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setItemColor(std::size_t index, const Color& color)
    {
        for (auto& text : m_items[index].texts)
            text.setColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float ListView::calculateAutoColumnWidth(const Text& text)
    {
        return text.getSize().x + (2.f * text.getExtraHorizontalOffset());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::updateSelectedAndhoveredItemColors()
    {
        for (const std::size_t selectedItem : m_selectedItems)
        {
            if ((static_cast<int>(selectedItem) == m_hoveredItem) && m_selectedTextColorHoverCached.isSet())
                setItemColor(selectedItem, m_selectedTextColorHoverCached);
            else if (m_selectedTextColorCached.isSet())
                setItemColor(selectedItem, m_selectedTextColorCached);
        }

        if ((m_hoveredItem >= 0) && (m_selectedItems.find(m_hoveredItem) == m_selectedItems.end()))
        {
            if (m_textColorHoverCached.isSet())
                setItemColor(m_hoveredItem, m_textColorHoverCached);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::updateItemColors()
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
            setItemColor(i, m_textColorCached);

        updateSelectedAndhoveredItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::updateHeaderTextsColor()
    {
        for (auto& column : m_columns)
        {
            if (m_headerTextColorCached.isSet())
                column.text.setColor(m_headerTextColorCached);
            else
                column.text.setColor(m_textColorCached);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::updateHoveredItem(int item)
    {
        if (m_hoveredItem == item)
            return;

        if (m_hoveredItem >= 0)
        {
            if ((m_selectedItems.find(m_hoveredItem) != m_selectedItems.end()) && m_selectedTextColorCached.isSet())
                setItemColor(m_hoveredItem, m_selectedTextColorCached);
            else
                setItemColor(m_hoveredItem, m_textColorCached);
        }

        m_hoveredItem = item;

        updateSelectedAndhoveredItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::updateSelectedItem(int item)
    {
        if (m_selectedItems.empty() && (item == -1))
            return;
        if ((m_selectedItems.size() == 1) && (static_cast<int>(*m_selectedItems.begin()) == item))
            return;

        for (const auto selectedItem : m_selectedItems)
        {
            if ((static_cast<int>(selectedItem) == m_hoveredItem) && m_textColorHoverCached.isSet())
                setItemColor(selectedItem, m_textColorHoverCached);
            else
                setItemColor(selectedItem, m_textColorCached);
        }

        if (item >= 0)
        {
            m_selectedItems = {static_cast<std::size_t>(item)};
            onItemSelect.emit(this, item);
        }
        else
        {
            m_selectedItems.clear();
            onItemSelect.emit(this, -1);
        }

        updateSelectedAndhoveredItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::addSelectedItem(int item)
    {
        if (!m_multiSelect)
        {
            updateSelectedItem(item);
            return;
        }

        if (m_selectedItems.find(item) != m_selectedItems.end())
            return;

        if (item >= 0)
        {
            if ((item == m_hoveredItem) && m_textColorHoverCached.isSet())
                setItemColor(static_cast<std::size_t>(item), m_textColorHoverCached);
            else
                setItemColor(static_cast<std::size_t>(item), m_textColorCached);

            m_selectedItems.insert(item);
        }

        updateSelectedAndhoveredItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::removeSelectedItem(std::size_t item)
    {
        m_selectedItems.erase(item);
        if ((static_cast<int>(item) == m_hoveredItem) && m_textColorHoverCached.isSet())
            setItemColor(item, m_textColorHoverCached);
        else
            setItemColor(item, m_textColorCached);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::updateHoveredItemByMousePos(Vector2f mousePos)
    {
        mousePos.y -= (m_bordersCached.getTop() + m_paddingCached.getTop() + getCurrentHeaderHeight());

        int hoveredItem;
        if (m_showHorizontalGridLines && (m_gridLinesWidth > 0))
            hoveredItem = static_cast<int>(std::ceil((mousePos.y + m_verticalScrollbar->getValue() - m_itemHeight - (m_gridLinesWidth / 2.f)) / (m_itemHeight + m_gridLinesWidth)));
        else
            hoveredItem = static_cast<int>(std::ceil((mousePos.y + m_verticalScrollbar->getValue() - m_itemHeight + 1) / m_itemHeight));

        if ((hoveredItem >= 0) && (hoveredItem < static_cast<int>(m_items.size())))
            updateHoveredItem(hoveredItem);
        else
            updateHoveredItem(-1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListView::getTotalSeparatorWidth() const
    {
        if (m_showVerticalGridLines && (m_gridLinesWidth > m_separatorWidth))
            return m_gridLinesWidth;
        else
            return m_separatorWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ListView::getColumnIndexBelowMouse(float mouseLeft)
    {
        float leftPos = mouseLeft - m_bordersCached.getLeft() - m_paddingCached.getLeft();
        if (m_horizontalScrollbar->isShown())
            leftPos += static_cast<float>(m_horizontalScrollbar->getValue());

        const unsigned int separatorWidth = getTotalSeparatorWidth();

        float columnRight = 0;
        for (unsigned int i = 0; i < m_columns.size(); ++i)
        {
            columnRight += m_columns[i].width + separatorWidth;
            if (leftPos < columnRight)
            {
                if (leftPos < columnRight - separatorWidth)
                    return static_cast<int>(i);
                else // Clicked on separator
                    return -1;
            }
        }

        // If the last column is exanded and no matching column was found then we must have clicked on the last one
        if (m_expandLastColumn)
            return static_cast<int>(m_columns.size()) - 1;

        return -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::updateScrollbars()
    {
        const bool verticalScrollbarAtBottom = (m_verticalScrollbar->getValue() + m_verticalScrollbar->getViewportSize() >= m_verticalScrollbar->getMaximum());
        const Vector2f innerSize = {std::max(0.f, getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight()),
                                    std::max(0.f, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom() - getCurrentHeaderHeight())};

        if (m_horizontalScrollbar->isShown())
        {
            m_verticalScrollbar->setSize({m_verticalScrollbar->getSize().x, std::max(0.f, getInnerSize().y) - m_horizontalScrollbar->getSize().y});
            m_verticalScrollbar->setViewportSize(static_cast<unsigned int>(innerSize.y - m_horizontalScrollbar->getSize().y));
        }
        else
        {
            m_verticalScrollbar->setSize({m_verticalScrollbar->getSize().x, std::max(0.f, getInnerSize().y)});
            m_verticalScrollbar->setViewportSize(static_cast<unsigned int>(innerSize.y));
        }

        if (m_verticalScrollbar->isShown())
        {
            m_horizontalScrollbar->setSize({getInnerSize().x - m_verticalScrollbar->getSize().x, m_horizontalScrollbar->getSize().y});
            m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(innerSize.x - m_verticalScrollbar->getSize().x));
        }
        else
        {
            m_horizontalScrollbar->setSize({getInnerSize().x, m_horizontalScrollbar->getSize().y});
            m_horizontalScrollbar->setViewportSize(static_cast<unsigned int>(innerSize.x));
        }

        if (m_showHorizontalGridLines && (m_gridLinesWidth > 0))
            m_verticalScrollbar->setScrollAmount(m_itemHeight + m_gridLinesWidth);
        else
            m_verticalScrollbar->setScrollAmount(m_itemHeight);

        // If the scrollbar was at the bottom then keep it at the bottom if it changes due to a different viewport size
        if (verticalScrollbarAtBottom && (m_verticalScrollbar->getValue() + m_verticalScrollbar->getViewportSize() < m_verticalScrollbar->getMaximum()))
            m_verticalScrollbar->setValue(m_verticalScrollbar->getMaximum() - m_verticalScrollbar->getViewportSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::updateVerticalScrollbarMaximum()
    {
        const bool verticalScrollbarAtBottom = (m_verticalScrollbar->getValue() + m_verticalScrollbar->getViewportSize() >= m_verticalScrollbar->getMaximum());

        unsigned int maximum = static_cast<unsigned int>(m_items.size() * m_itemHeight);
        if (m_showHorizontalGridLines && (m_gridLinesWidth > 0) && !m_items.empty())
            maximum += static_cast<unsigned int>((m_items.size() - 1) * m_gridLinesWidth);

        m_verticalScrollbar->setMaximum(maximum);
        updateScrollbars();

        // If the scrollbar was at the bottom then keep it at the bottom
        if (verticalScrollbarAtBottom && (m_verticalScrollbar->getValue() + m_verticalScrollbar->getViewportSize() < m_verticalScrollbar->getMaximum()))
            m_verticalScrollbar->setValue(m_verticalScrollbar->getMaximum() - m_verticalScrollbar->getViewportSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::updateHorizontalScrollbarMaximum()
    {
        if (!m_headerVisible || m_columns.empty())
            m_horizontalScrollbar->setMaximum(0u);
        else if (m_columns.size() == 1)
            m_horizontalScrollbar->setMaximum(static_cast<unsigned int>(m_columns[0].width));
        else
        {
            float width = 0;
            for (const auto& column : m_columns)
                width += column.width;

            if (m_expandLastColumn)
                width += (m_columns.size() - 1) * static_cast<float>(getTotalSeparatorWidth());
            else
                width += m_columns.size() * static_cast<float>(getTotalSeparatorWidth());

            m_horizontalScrollbar->setMaximum(static_cast<unsigned int>(width));
        }

        updateScrollbars();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::drawHeaderText(sf::RenderTarget& target, sf::RenderStates states, float columnWidth, float headerHeight, std::size_t column) const
    {
        if (column >= m_columns.size())
            return;

        const unsigned int headerTextSize = getHeaderTextSize();
        const float textPadding = Text::getExtraHorizontalOffset(m_fontCached, headerTextSize);

        const Clipping clipping{target, states, {textPadding, 0}, {columnWidth - (2 * textPadding), headerHeight}};

        float translateX;
        if ((m_columns[column].alignment == ColumnAlignment::Left) || (column >= m_columns.size()))
            translateX = textPadding;
        else if (m_columns[column].alignment == ColumnAlignment::Center)
            translateX = (columnWidth - m_columns[column].text.getSize().x) / 2.f;
        else // if (m_columns[column].alignment == ColumnAlignment::Right)
            translateX = columnWidth - textPadding - m_columns[column].text.getSize().x;

        states.transform.translate({translateX, (headerHeight - Text::getLineHeight(m_fontCached, headerTextSize)) / 2.0f});
        m_columns[column].text.draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::drawColumn(sf::RenderTarget& target, sf::RenderStates states, std::size_t firstItem, std::size_t lastItem, std::size_t column, float columnWidth) const
    {
        if (firstItem == lastItem)
            return;

        const unsigned int requiredItemHeight = m_itemHeight + (m_showHorizontalGridLines ? m_gridLinesWidth : 0);
        const float verticalTextOffset = (m_itemHeight - Text::getLineHeight(m_fontCached, m_textSize)) / 2.0f;
        const float textPadding = Text::getExtraHorizontalOffset(m_fontCached, m_textSize);
        const float columnHeight = getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()
                                   - getCurrentHeaderHeight() - (m_horizontalScrollbar->isShown() ? m_horizontalScrollbar->getSize().y : 0);

        // Draw the icons.
        // If at least one icon is set then all items in the first column have to be shifted to make room for the icon.
        if ((column == 0) && (m_iconCount > 0))
        {
            const sf::Transform transformBeforeIcons = states.transform;
            const Clipping clipping{target, states, {textPadding, 0}, {columnWidth - (2 * textPadding), columnHeight}};

            states.transform.translate({0, (requiredItemHeight * firstItem) - static_cast<float>(m_verticalScrollbar->getValue())});

            for (std::size_t i = firstItem; i < lastItem; ++i)
            {
                if (!m_items[i].icon.isSet())
                {
                    states.transform.translate({0, static_cast<float>(requiredItemHeight)});
                    continue;
                }

                const float verticalIconOffset = (m_itemHeight - m_items[i].icon.getSize().y) / 2.f;

                states.transform.translate({textPadding, verticalIconOffset});
                m_items[i].icon.draw(target, states);
                states.transform.translate({-textPadding, static_cast<float>(requiredItemHeight) - verticalIconOffset});
            }

            states.transform = transformBeforeIcons;

            const float extraIconSpace = m_maxIconWidth + textPadding;
            columnWidth -= extraIconSpace;
            states.transform.translate({extraIconSpace, 0});
        }

        const Clipping clipping{target, states, {textPadding, 0}, {columnWidth - (2 * textPadding), columnHeight}};

        states.transform.translate({0, (requiredItemHeight * firstItem) - static_cast<float>(m_verticalScrollbar->getValue())});
        for (std::size_t i = firstItem; i < lastItem; ++i)
        {
            if (column >= m_items[i].texts.size())
            {
                states.transform.translate({0, static_cast<float>(requiredItemHeight)});
                continue;
            }

            float translateX;
            if ((column >= m_columns.size()) || (m_columns[column].alignment == ColumnAlignment::Left))
                translateX = textPadding;
            else if (m_columns[column].alignment == ColumnAlignment::Center)
                translateX = (columnWidth - m_items[i].texts[column].getSize().x) / 2.f;
            else // if (m_columns[column].alignment == ColumnAlignment::Right)
                translateX = columnWidth - textPadding - m_items[i].texts[column].getSize().x;

            states.transform.translate({translateX, verticalTextOffset});
            m_items[i].texts[column].draw(target, states);
            states.transform.translate({-translateX, static_cast<float>(requiredItemHeight) - verticalTextOffset});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListView::update(sf::Time elapsedTime)
    {
        const bool screenRefreshRequired = Widget::update(elapsedTime);

        if (m_animationTimeElapsed >= sf::milliseconds(getDoubleClickTime()))
        {
            m_animationTimeElapsed = {};
            m_possibleDoubleClick = -1;
        }

        return screenRefreshRequired;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());
        const sf::RenderStates statesForScrollbar = states;

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate(m_bordersCached.getOffset());
        }

        // Draw the background
        drawRectangleShape(target, states, getInnerSize(), m_backgroundColorCached);

        const unsigned int totalItemHeight = m_itemHeight + (m_showHorizontalGridLines ? m_gridLinesWidth : 0);

        // Find out which items are visible
        std::size_t firstItem = 0;
        std::size_t lastItem = m_items.size();
        if (m_verticalScrollbar->getViewportSize() < m_verticalScrollbar->getMaximum())
        {
            firstItem = m_verticalScrollbar->getValue() / totalItemHeight;
            lastItem = ((static_cast<std::size_t>(m_verticalScrollbar->getValue()) + m_verticalScrollbar->getViewportSize()) / totalItemHeight) + 1;
            if (lastItem > m_items.size())
                lastItem = m_items.size();
        }

        states.transform.translate({m_paddingCached.getLeft(), m_paddingCached.getTop()});

        // Draw the scrollbars
        if (m_verticalScrollbar->isVisible())
            m_verticalScrollbar->draw(target, statesForScrollbar);
        if (m_horizontalScrollbar->isVisible())
            m_horizontalScrollbar->draw(target, statesForScrollbar);

        const float headerHeight = getHeaderHeight();
        const float totalHeaderHeight = getCurrentHeaderHeight();
        const float innerHeight = getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()
                                  - (m_horizontalScrollbar->isShown() ? m_horizontalScrollbar->getSize().y : 0);

        float availableWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
        if (m_verticalScrollbar->isShown())
            availableWidth -= m_verticalScrollbar->getSize().x;

        // Draw the header background
        if (totalHeaderHeight > 0)
        {
            // We deliberately draw behind the header separator to make sure it has the same color as
            // the column separator when the color is semi-transparent.
            if (m_headerBackgroundColorCached.isSet())
                drawRectangleShape(target, states, {availableWidth, totalHeaderHeight}, m_headerBackgroundColorCached);

            // Draw the separator line between the header and the contents
            if (m_headerSeparatorHeight > 0)
            {
                sf::RenderStates headerStates = states;
                headerStates.transform.translate({0, static_cast<float>(headerHeight)});

                const Color& separatorColor = m_separatorColorCached.isSet() ? m_separatorColorCached : m_borderColorCached;
                drawRectangleShape(target, headerStates, {availableWidth, static_cast<float>(m_headerSeparatorHeight)}, separatorColor);
            }
        }

        if (m_showHorizontalGridLines || !m_selectedItems.empty() || (m_hoveredItem >= 0))
        {
            states.transform.translate({0, totalHeaderHeight});
            const Clipping clipping{target, states, {}, {availableWidth, innerHeight - totalHeaderHeight}};

            // Draw the horizontal grid lines
            if (m_showHorizontalGridLines && (m_gridLinesWidth > 0) && !m_items.empty())
            {
                sf::Transform transformBeforeGridLines = states.transform;

                states.transform.translate({0, (totalItemHeight * firstItem) + m_itemHeight - static_cast<float>(m_verticalScrollbar->getValue())});

                const Color& gridLineColor = m_gridLinesColorCached.isSet() ? m_gridLinesColorCached : (m_separatorColorCached.isSet() ? m_separatorColorCached : m_borderColorCached);
                for (std::size_t i = firstItem; i <= lastItem; ++i)
                {
                    drawRectangleShape(target, states, {availableWidth, static_cast<float>(m_gridLinesWidth)}, gridLineColor);
                    states.transform.translate({0, static_cast<float>(totalItemHeight)});
                }

                states.transform = transformBeforeGridLines;
            }

            // Draw the background of the selected item
            if (!m_selectedItems.empty())
            {
                for(const std::size_t selectedItem : m_selectedItems)
                {
                    states.transform.translate({0, selectedItem * static_cast<float>(totalItemHeight) - m_verticalScrollbar->getValue()});

                    if ((static_cast<int>(selectedItem) == m_hoveredItem) && m_selectedBackgroundColorHoverCached.isSet())
                        drawRectangleShape(target, states, {availableWidth, static_cast<float>(m_itemHeight)}, m_selectedBackgroundColorHoverCached);
                    else
                        drawRectangleShape(target, states, {availableWidth, static_cast<float>(m_itemHeight)}, m_selectedBackgroundColorCached);

                    states.transform.translate({0, -static_cast<int>(selectedItem) * static_cast<float>(totalItemHeight) + m_verticalScrollbar->getValue()});
                }
            }

            // Draw the background of the item on which the mouse is standing
            if ((m_hoveredItem >= 0) && (m_selectedItems.find(m_hoveredItem) == m_selectedItems.end()) && m_backgroundColorHoverCached.isSet())
            {
                states.transform.translate({0, m_hoveredItem * static_cast<float>(totalItemHeight) - m_verticalScrollbar->getValue()});
                drawRectangleShape(target, states, {availableWidth, static_cast<float>(m_itemHeight)}, m_backgroundColorHoverCached);
                states.transform.translate({0, -m_hoveredItem * static_cast<float>(totalItemHeight) + m_verticalScrollbar->getValue()});
            }

            // We haven't drawn the header yet, so move back up
            states.transform.translate({0, -totalHeaderHeight});
        }

        const Clipping clipping{target, states, {}, {availableWidth, innerHeight}};
        if (m_horizontalScrollbar->isShown())
            states.transform.translate({-static_cast<float>(m_horizontalScrollbar->getValue()), 0});

        const unsigned int separatorWidth = getTotalSeparatorWidth();

        // Draw the header texts
        if (totalHeaderHeight > 0)
        {
            const Color& separatorColor = m_separatorColorCached.isSet() ? m_separatorColorCached : m_borderColorCached;

            sf::RenderStates headerStates = states;
            float columnLeftPos = 0;
            for (std::size_t col = 0; col < m_columns.size(); ++col)
            {
                if (m_expandLastColumn && (col + 1 == m_columns.size()))
                    drawHeaderText(target, headerStates, availableWidth - columnLeftPos, headerHeight, col);
                else
                {
                    drawHeaderText(target, headerStates, m_columns[col].width, headerHeight, col);
                    headerStates.transform.translate({m_columns[col].width, 0});

                    if (m_separatorWidth)
                    {
                        if (m_separatorWidth == separatorWidth)
                            drawRectangleShape(target, headerStates, {static_cast<float>(m_separatorWidth), headerHeight}, separatorColor);
                        else
                        {
                            const float separatorOffset = (separatorWidth - m_separatorWidth) / 2.f;
                            headerStates.transform.translate({separatorOffset, 0});
                            drawRectangleShape(target, headerStates, {static_cast<float>(m_separatorWidth), headerHeight}, separatorColor);
                            headerStates.transform.translate({-separatorOffset, 0});
                        }
                    }

                    headerStates.transform.translate({static_cast<float>(separatorWidth), 0});
                    columnLeftPos += m_columns[col].width + separatorWidth;
                }
            }

            states.transform.translate({0, totalHeaderHeight});
        }

        // Draw the items and the separation lines
        if (m_columns.empty())
            drawColumn(target, states, firstItem, lastItem, 0, getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight());
        else
        {
            float columnLeftPos = 0;
            for (std::size_t col = 0; col < m_columns.size(); ++col)
            {
                if (m_expandLastColumn && (col + 1 == m_columns.size()))
                    drawColumn(target, states, firstItem, lastItem, col, availableWidth - columnLeftPos);
                else
                {
                    drawColumn(target, states, firstItem, lastItem, col, m_columns[col].width);
                    states.transform.translate({m_columns[col].width, 0});

                    if (separatorWidth)
                    {
                        if (m_showVerticalGridLines && (m_gridLinesWidth > 0))
                        {
                            const Color& gridLineColor = m_gridLinesColorCached.isSet() ? m_gridLinesColorCached : (m_separatorColorCached.isSet() ? m_separatorColorCached : m_borderColorCached);
                            if (m_gridLinesWidth == separatorWidth)
                                drawRectangleShape(target, states, {static_cast<float>(m_gridLinesWidth), innerHeight - totalHeaderHeight}, gridLineColor);
                            else
                            {
                                const float gridLineOffset = (separatorWidth - m_gridLinesWidth) / 2.f;
                                states.transform.translate({gridLineOffset, 0});
                                drawRectangleShape(target, states, {static_cast<float>(m_gridLinesWidth), innerHeight - totalHeaderHeight}, gridLineColor);
                                states.transform.translate({-gridLineOffset, 0});
                            }
                        }

                        states.transform.translate({static_cast<float>(separatorWidth), 0});
                    }

                    columnLeftPos += m_columns[col].width + separatorWidth;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
