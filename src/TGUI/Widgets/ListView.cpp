/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2019 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Clipping.hpp>

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
                 getHeaderHeight() + (m_itemHeight * 6) + m_paddingCached.getTop() + m_paddingCached.getBottom() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
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
        column.text = createText(text);
        column.text.setCharacterSize(getHeaderTextSize());
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

    void ListView::setColumnWidth(std::size_t index, float width)
    {
        if (index >= m_columns.size())
            return;

        m_columns[index].designWidth = width;
        if (width)
            m_columns[index].width = width;
        else
            m_columns[index].width = calculateAutoColumnWidth(m_columns[index].text);

        updateHorizontalScrollbarMaximum();
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
        setSize(getSize());
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

    void ListView::setHeaderVisible(bool showHeader)
    {
        m_headerVisible = showHeader;
        updateHorizontalScrollbarMaximum();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListView::getHeaderVisible() const
    {
        return m_headerVisible;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::setColumnAlignment(unsigned int columnIndex, ColumnAlignment alignment)
    {
        if (columnIndex < m_columns.size())
            m_columns[columnIndex].alignment = alignment;
        else
        {
            TGUI_PRINT_WARNING("setColumnAlignment called with invalid columnIndex.");
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListView::ColumnAlignment ListView::getColumnAlignment(unsigned int columnIndex) const
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

    std::size_t ListView::addItem(const sf::String& text)
    {
        Item item;
        item.texts.push_back(createText(text));
        m_items.push_back(std::move(item));

        updateVerticalScrollbarMaximum();

        // Scroll down when auto-scrolling is enabled
        if (m_autoScroll && (m_verticalScrollbar->getViewportSize() < m_verticalScrollbar->getMaximum()))
            m_verticalScrollbar->setValue(m_verticalScrollbar->getMaximum() - m_verticalScrollbar->getViewportSize());

        return m_items.size()-1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ListView::addItem(const std::vector<sf::String>& itemTexts)
    {
        Item item;
        item.texts.reserve(itemTexts.size());
        for (const auto& text : itemTexts)
            item.texts.push_back(createText(text));

        m_items.push_back(std::move(item));

        updateVerticalScrollbarMaximum();

        // Scroll down when auto-scrolling is enabled
        if (m_autoScroll && (m_verticalScrollbar->getViewportSize() < m_verticalScrollbar->getMaximum()))
            m_verticalScrollbar->setValue(m_verticalScrollbar->getMaximum() - m_verticalScrollbar->getViewportSize());

        return m_items.size()-1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListView::removeItem(std::size_t index)
    {
        // Update the hovered item
        if (m_hoveredItem >= 0)
        {
            if (m_hoveredItem == static_cast<int>(index))
                updateHoveredItem(-1);
            else if (m_selectedItem > static_cast<int>(index))
            {
                // Don't call updateSelectedItem here, there should not be no callback and the item hasn't been erased yet so it would point to the wrong place
                m_selectedItem = m_selectedItem - 1;
            }
        }

        // Update the selected item
        if (m_selectedItem >= 0)
        {
            if (m_selectedItem == static_cast<int>(index))
                updateSelectedItem(-1);
            else if (m_selectedItem > static_cast<int>(index))
            {
                // Don't call updateSelectedItem here, there should not be no callback and the item hasn't been erased yet so it would point to the wrong place
                m_selectedItem = m_selectedItem - 1;
            }
        }

        if (index >= m_items.size())
            return false;

        m_items.erase(m_items.begin() + index);

        updateVerticalScrollbarMaximum();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::removeAllItems()
    {
        updateSelectedItem(-1);
        updateHoveredItem(-1);

        m_items.clear();

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

        updateSelectedItem(index);

        // Move the scrollbar
        if (m_selectedItem * getItemHeight() < m_verticalScrollbar->getValue())
            m_verticalScrollbar->setValue(m_selectedItem * getItemHeight());
        else if ((m_selectedItem + 1) * getItemHeight() > m_verticalScrollbar->getValue() + m_verticalScrollbar->getViewportSize())
            m_verticalScrollbar->setValue((m_selectedItem + 1) * getItemHeight() - m_verticalScrollbar->getViewportSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::deselectItem()
    {
        updateSelectedItem(-1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ListView::getSelectedItemIndex() const
    {
        return m_selectedItem;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ListView::getItemCount() const
    {
        return m_items.size();
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

        row.resize(m_columns.size());
        return row;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String> ListView::getItems() const
    {
        std::vector<sf::String> items;

        for (const auto& item : m_items)
        {
            if (item.texts.empty())
                items.push_back("");
            else
                items.push_back(item.texts[0].getString());
        }

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

            row.resize(m_columns.size());
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

        m_verticalScrollbar->setScrollAmount(m_itemHeight);
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

    unsigned int ListView::getTextSize() const
    {
        return m_textSize;
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

    bool ListView::mouseOnWidget(Vector2f pos) const
    {
        return FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        m_mouseDown = true;

        const float headerHeight = (m_headerVisible && !m_columns.empty()) ? getHeaderHeight() : 0.f;

        if (m_verticalScrollbar->mouseOnWidget(pos))
        {
            m_verticalScrollbar->leftMousePressed(pos);
        }
        else if (m_horizontalScrollbar->isShown() && m_horizontalScrollbar->mouseOnWidget(pos))
        {
            m_horizontalScrollbar->leftMousePressed(pos);
        }
        else if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(), m_bordersCached.getTop() + m_paddingCached.getTop() + headerHeight,
                           getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
        {
            pos.y -= (m_bordersCached.getTop() + m_paddingCached.getTop() + headerHeight);

            int hoveredItem = static_cast<int>(((pos.y - (m_itemHeight - (m_verticalScrollbar->getValue() % m_itemHeight))) / m_itemHeight) + (m_verticalScrollbar->getValue() / m_itemHeight) + 1);
            if (hoveredItem < static_cast<int>(m_items.size()))
                updateHoveredItem(hoveredItem);
            else
                updateHoveredItem(-1);

            if (m_selectedItem != m_hoveredItem)
            {
                m_possibleDoubleClick = false;
                updateSelectedItem(m_hoveredItem);
            }

            // Check if you double-clicked
            if (m_possibleDoubleClick)
            {
                m_possibleDoubleClick = false;
                if (m_selectedItem >= 0)
                    onDoubleClick.emit(this, m_selectedItem);
            }
            else // This is the first click
            {
                m_animationTimeElapsed = {};
                m_possibleDoubleClick = true;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::leftMouseReleased(Vector2f pos)
    {
        if (m_verticalScrollbar->isShown() && m_verticalScrollbar->isMouseDown())
            m_verticalScrollbar->leftMouseReleased(pos - getPosition());

        if (m_horizontalScrollbar->isShown() && m_horizontalScrollbar->isMouseDown())
            m_horizontalScrollbar->leftMouseReleased(pos - getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

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
            const float headerHeight = (m_headerVisible && !m_columns.empty()) ? getHeaderHeight() : 0.f;
            if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(),
                          m_bordersCached.getTop() + m_paddingCached.getTop() + headerHeight,
                          getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                          getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
            {
                pos.y -= (m_bordersCached.getTop() + m_paddingCached.getTop() + headerHeight);

                int hoveredItem = static_cast<int>(((pos.y - (m_itemHeight - (m_verticalScrollbar->getValue() % m_itemHeight))) / m_itemHeight) + (m_verticalScrollbar->getValue() / m_itemHeight) + 1);
                if (hoveredItem < static_cast<int>(m_items.size()))
                    updateHoveredItem(hoveredItem);
                else
                    updateHoveredItem(-1);

                // If the mouse is held down then select the item below the mouse
                if (m_mouseDown && !m_verticalScrollbar->isMouseDown())
                {
                    if (m_selectedItem != m_hoveredItem)
                    {
                        m_possibleDoubleClick = false;

                        updateSelectedItem(m_hoveredItem);
                    }
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
                || sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
                || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)))
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

        m_possibleDoubleClick = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::mouseNoLongerDown()
    {
        Widget::mouseNoLongerDown();
        m_verticalScrollbar->mouseNoLongerDown();
        m_horizontalScrollbar->mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ListView::getSignal(std::string signalName)
    {
        if (signalName == toLower(onItemSelect.getName()))
            return onItemSelect;
        else if (signalName == toLower(onDoubleClick.getName()))
            return onDoubleClick;
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

            // If no scrollbar width was set then we may need to use the one from the texture
            if (!getSharedRenderer()->getScrollbarWidth())
            {
                m_verticalScrollbar->setSize({m_verticalScrollbar->getDefaultWidth(), m_verticalScrollbar->getSize().y});
                setSize(m_size);
            }
        }
        else if (property == "scrollbarwidth")
        {
            const float width = getSharedRenderer()->getScrollbarWidth() ? getSharedRenderer()->getScrollbarWidth() : m_verticalScrollbar->getDefaultWidth();
            m_verticalScrollbar->setSize({width, m_verticalScrollbar->getSize().y});
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
        else if (property == "headertextcolor")
        {
            m_headerTextColorCached = getSharedRenderer()->getHeaderTextColor();
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

                node->propertyValuePairs["Texts"] = std::make_unique<DataIO::ValueNode>(textsList);
            }
        }

        if (!m_autoScroll)
            node->propertyValuePairs["AutoScroll"] = std::make_unique<DataIO::ValueNode>("false");

        if (m_headerTextSize)
            node->propertyValuePairs["HeaderTextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_headerTextSize));

        if (m_selectedItem >= 0)
            node->propertyValuePairs["SelectedItemIndex"] = std::make_unique<DataIO::ValueNode>(to_string(m_selectedItem));

        node->propertyValuePairs["HeaderVisible"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_headerVisible));
        node->propertyValuePairs["HeaderHeight"] = std::make_unique<DataIO::ValueNode>(to_string(m_requestedHeaderHeight));
        node->propertyValuePairs["SeparatorWidth"] = std::make_unique<DataIO::ValueNode>(to_string(m_separatorWidth));
        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        node->propertyValuePairs["ItemHeight"] = std::make_unique<DataIO::ValueNode>(to_string(m_itemHeight));
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
                width = static_cast<unsigned int>(Deserializer::deserialize(ObjectConverter::Type::Number, childNode->propertyValuePairs["width"]->value).getNumber());

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

            if (!node->propertyValuePairs["items"])
                throw Exception{"Failed to parse 'Item' property, no Texts property found"};
            if (!node->propertyValuePairs["texts"]->listNode)
                throw Exception{"Failed to parse 'Texts' property inside the 'Item' property, expected a list as value"};

            for (const auto& item : childNode->propertyValuePairs["texts"]->valueList)
                addItem(Deserializer::deserialize(ObjectConverter::Type::String, item).getString());
        }

        if (node->propertyValuePairs["autoscroll"])
            setAutoScroll(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["autoscroll"]->value).getBool());
        if (node->propertyValuePairs["headervisible"])
            setHeaderVisible(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["headervisible"]->value).getBool());
        if (node->propertyValuePairs["headerheight"])
            setHeaderHeight(tgui::stoi(node->propertyValuePairs["headerheight"]->value));
        if (node->propertyValuePairs["separatorwidth"])
            setSeparatorWidth(tgui::stoi(node->propertyValuePairs["separatorwidth"]->value));
        if (node->propertyValuePairs["textsize"])
            setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["itemheight"])
            setItemHeight(tgui::stoi(node->propertyValuePairs["itemheight"]->value));
        if (node->propertyValuePairs["selecteditemindex"])
            setSelectedItem(tgui::stoi(node->propertyValuePairs["selecteditemindex"]->value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ListView::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(), getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
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
        if (m_selectedItem >= 0)
        {
            if ((m_selectedItem == m_hoveredItem) && m_selectedTextColorHoverCached.isSet())
                setItemColor(m_selectedItem, m_selectedTextColorHoverCached);
            else if (m_selectedTextColorCached.isSet())
                setItemColor(m_selectedItem, m_selectedTextColorCached);
        }

        if ((m_hoveredItem >= 0) && (m_selectedItem != m_hoveredItem))
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

    void ListView::updateHoveredItem(int item)
    {
        if (m_hoveredItem == item)
            return;

        if (m_hoveredItem >= 0)
        {
            if ((m_selectedItem == m_hoveredItem) && m_selectedTextColorCached.isSet())
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
        if (m_selectedItem == item)
            return;

        if (m_selectedItem >= 0)
        {
            if ((m_selectedItem == m_hoveredItem) && m_textColorHoverCached.isSet())
                setItemColor(m_selectedItem, m_textColorHoverCached);
            else
                setItemColor(m_selectedItem, m_textColorCached);
        }

        m_selectedItem = item;
        onItemSelect.emit(this, m_selectedItem);

        updateSelectedAndhoveredItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::updateScrollbars()
    {
        const float headerHeight = (m_headerVisible && !m_columns.empty()) ? getHeaderHeight() : 0.f;
        const Vector2f innerSize = {std::max(0.f, getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight()),
                                    std::max(0.f, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom() - headerHeight)};

        if (m_verticalScrollbarPolicy != Scrollbar::Policy::Never)
        {
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
        }

        if (m_horizontalScrollbarPolicy != Scrollbar::Policy::Never)
        {
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
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::updateVerticalScrollbarMaximum()
    {
        m_verticalScrollbar->setMaximum(static_cast<unsigned int>(m_items.size() * m_itemHeight));
        updateScrollbars();
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
                width += column.width + m_separatorWidth;

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

        const float headerHeight = (m_headerVisible && !m_columns.empty()) ? getHeaderHeight() : 0.f;
        const float textPadding = Text::getExtraHorizontalOffset(m_fontCached, m_textSize);
        const float columnHeight = getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()
                                   - headerHeight - (m_horizontalScrollbar->isShown() ? m_horizontalScrollbar->getSize().y : 0);
        const Clipping clipping{target, states, {textPadding, 0}, {columnWidth - (2 * textPadding), columnHeight}};

        states.transform.translate({0, -static_cast<float>(m_verticalScrollbar->getValue())});
        states.transform.translate({0, (m_itemHeight * firstItem) + (m_itemHeight - Text::getLineHeight(m_fontCached, m_textSize)) / 2.0f});

        for (std::size_t i = firstItem; i < lastItem; ++i)
        {
            if (column >= m_items[i].texts.size())
            {
                states.transform.translate({0, static_cast<float>(m_itemHeight)});
                continue;
            }

            float translateX;
            if ((m_columns[column].alignment == ColumnAlignment::Left) || (column >= m_columns.size()))
                translateX = textPadding;
            else if (m_columns[column].alignment == ColumnAlignment::Center)
                translateX = (columnWidth - m_items[i].texts[column].getSize().x) / 2.f;
            else // if (m_columns[column].alignment == ColumnAlignment::Right)
                translateX = columnWidth - textPadding - m_items[i].texts[column].getSize().x;

            states.transform.translate({translateX, 0});
            m_items[i].texts[column].draw(target, states);
            states.transform.translate({-translateX, static_cast<float>(m_itemHeight)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListView::update(sf::Time elapsedTime)
    {
        Widget::update(elapsedTime);

        if (m_animationTimeElapsed >= sf::milliseconds(getDoubleClickTime()))
        {
            m_animationTimeElapsed = {};
            m_possibleDoubleClick = false;
        }
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

        float availableWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
        if (m_verticalScrollbar->isShown())
            availableWidth -= m_verticalScrollbar->getSize().x;

        // Find out which items are visible
        std::size_t firstItem = 0;
        std::size_t lastItem = m_items.size();
        if (m_verticalScrollbar->getViewportSize() < m_verticalScrollbar->getMaximum())
        {
            firstItem = m_verticalScrollbar->getValue() / m_itemHeight;
            lastItem = (m_verticalScrollbar->getValue() + m_verticalScrollbar->getViewportSize()) / m_itemHeight;

            // Show another item when the scrollbar is standing between two items
            if ((m_verticalScrollbar->getValue() + m_verticalScrollbar->getViewportSize()) % m_itemHeight != 0)
                ++lastItem;
        }

        states.transform.translate({m_paddingCached.getLeft(), m_paddingCached.getTop()});

        // Draw the scrollbars
        if (m_verticalScrollbar->isVisible())
            m_verticalScrollbar->draw(target, statesForScrollbar);
        if (m_horizontalScrollbar->isVisible())
            m_horizontalScrollbar->draw(target, statesForScrollbar);

        const float headerHeight = (m_headerVisible && !m_columns.empty()) ? getHeaderHeight() : 0.f;
        const float innerHeight = getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()
                                  - (m_horizontalScrollbar->isShown() ? m_horizontalScrollbar->getSize().y : 0);

        // Draw the background of the selected and hovered items
        if ((m_selectedItem >= 0) || (m_hoveredItem >= 0))
        {
            states.transform.translate({0, headerHeight});
            const Clipping clipping{target, states, {}, {availableWidth, innerHeight - headerHeight}};

            // Draw the background of the selected item
            if (m_selectedItem >= 0)
            {
                states.transform.translate({0, m_selectedItem * static_cast<float>(m_itemHeight) - m_verticalScrollbar->getValue()});

                if ((m_selectedItem == m_hoveredItem) && m_selectedBackgroundColorHoverCached.isSet())
                    drawRectangleShape(target, states, {availableWidth, static_cast<float>(m_itemHeight)}, m_selectedBackgroundColorHoverCached);
                else
                    drawRectangleShape(target, states, {availableWidth, static_cast<float>(m_itemHeight)}, m_selectedBackgroundColorCached);

                states.transform.translate({0, -m_selectedItem * static_cast<float>(m_itemHeight) + m_verticalScrollbar->getValue()});
            }

            // Draw the background of the item on which the mouse is standing
            if ((m_hoveredItem >= 0) && (m_hoveredItem != m_selectedItem) && m_backgroundColorHoverCached.isSet())
            {
                states.transform.translate({0, m_hoveredItem * static_cast<float>(m_itemHeight) - m_verticalScrollbar->getValue()});
                drawRectangleShape(target, states, {availableWidth, static_cast<float>(m_itemHeight)}, m_backgroundColorHoverCached);
                states.transform.translate({0, -m_hoveredItem * static_cast<float>(m_itemHeight) + m_verticalScrollbar->getValue()});
            }

            states.transform.translate({0, -headerHeight});
        }

        // Draw the header background
        if (m_headerVisible && !m_columns.empty())
        {
            if (m_headerBackgroundColorCached.isSet())
                drawRectangleShape(target, states, {availableWidth, headerHeight}, m_headerBackgroundColorCached);
        }

        const Clipping clipping{target, states, {}, {availableWidth, innerHeight}};
        if (m_horizontalScrollbar->isShown())
            states.transform.translate({-static_cast<float>(m_horizontalScrollbar->getValue()), 0});

        // Draw the header texts
        if (m_headerVisible && !m_columns.empty())
        {
            if (m_columns.empty())
                drawHeaderText(target, states, getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), headerHeight, 0);
            else
            {
                sf::RenderStates headerStates = states;

                for (std::size_t col = 0; col < m_columns.size(); ++col)
                {
                    drawHeaderText(target, headerStates, m_columns[col].width, headerHeight, col);
                    headerStates.transform.translate({m_columns[col].width + m_separatorWidth, 0});
                }
            }

            states.transform.translate({0, headerHeight});
        }

        // Draw the items and the separation lines
        if (m_columns.empty())
            drawColumn(target, states, firstItem, lastItem, 0, getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight());
        else
        {
            for (std::size_t col = 0; col < m_columns.size(); ++col)
            {
                drawColumn(target, states, firstItem, lastItem, col, m_columns[col].width);
                states.transform.translate({m_columns[col].width, 0});

                if (m_separatorWidth)
                {
                    if (headerHeight)
                        states.transform.translate({0, -headerHeight});

                    const Color& separatorColor = m_separatorColorCached.isSet() ? m_separatorColorCached : m_borderColorCached;
                    drawRectangleShape(target, states, {static_cast<float>(m_separatorWidth), innerHeight}, separatorColor);
                    states.transform.translate({static_cast<float>(m_separatorWidth), headerHeight});
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
