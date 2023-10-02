/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Widgets/ListBox.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char ListBox::StaticWidgetType[];
#endif

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::ListBox(const char* typeName, bool initRenderer) :
        Widget{typeName, false}
    {
        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<ListBoxRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }

        setTextSize(getGlobalTextSize());
        setItemHeight(static_cast<unsigned int>(std::round(Text::getLineHeight(m_fontCached, m_textSizeCached) * 1.25f)));
        setSize({Text::getLineHeight(m_fontCached, m_textSizeCached) * 10,
                 (m_itemHeight * 7) + m_paddingCached.getTop() + m_paddingCached.getBottom() + m_bordersCached.getTop() + m_bordersCached.getBottom()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::Ptr ListBox::create()
    {
        return std::make_shared<ListBox>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::Ptr ListBox::copy(const ListBox::ConstPtr& listBox)
    {
        if (listBox)
            return std::static_pointer_cast<ListBox>(listBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBoxRenderer* ListBox::getSharedRenderer()
    {
        return aurora::downcast<ListBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const ListBoxRenderer* ListBox::getSharedRenderer() const
    {
        return aurora::downcast<const ListBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBoxRenderer* ListBox::getRenderer()
    {
        return aurora::downcast<ListBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        m_spriteBackground.setSize(getInnerSize());

        m_scroll->setSize({m_scroll->getSize().x, std::max(0.f, getInnerSize().y)});
        m_scroll->setViewportSize(static_cast<unsigned int>(getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()));

        updateItemPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ListBox::addItem(const String& itemName, const String& id)
    {
        // Check if the item limit is reached (if there is one)
        if ((m_maxItems > 0) && (m_items.size() >= m_maxItems))
            return m_maxItems;

        m_scroll->setMaximum(static_cast<unsigned int>((m_items.size() + 1) * m_itemHeight));

        // Scroll down when auto-scrolling is enabled
        if (m_autoScroll && (m_scroll->getViewportSize() < m_scroll->getMaximum()))
        {
            m_scroll->setValue(m_scroll->getMaximum() - m_scroll->getViewportSize());
            triggerOnScroll();
        }

        // Create the new item
        Text newItem;
        newItem.setFont(m_fontCached);
        newItem.setColor(m_textColorCached);
        newItem.setOpacity(m_opacityCached);
        newItem.setStyle(m_textStyleCached);
        newItem.setCharacterSize(m_textSizeCached);
        newItem.setString(itemName);
        newItem.setPosition({0, (m_items.size() * m_itemHeight) + ((m_itemHeight - newItem.getSize().y) / 2.0f)});

        // Add the new item to the list
        m_items.emplace_back();
        m_items.back().text = std::move(newItem);
        m_items.back().id = id;
        return m_items.size() - 1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::setSelectedItem(const String& itemName)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].text.getString() == itemName)
                return setSelectedItemByIndex(i);
        }

        // No match was found
        deselectItem();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::setSelectedItemById(const String& id)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].id == id)
                return setSelectedItemByIndex(i);
        }

        // No match was found
        deselectItem();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::setSelectedItemByIndex(std::size_t index)
    {
        if (index >= m_items.size())
        {
            deselectItem();
            return false;
        }

        updateSelectedItem(static_cast<int>(index));

        // Move the scrollbar
        if (index * getItemHeight() < m_scroll->getValue())
        {
            m_scroll->setValue(static_cast<unsigned int>(index) * getItemHeight());
            triggerOnScroll();
        }
        else if ((index + 1) * getItemHeight() > m_scroll->getValue() + m_scroll->getViewportSize())
        {
            m_scroll->setValue((static_cast<unsigned int>(index) + 1) * getItemHeight() - m_scroll->getViewportSize());
            triggerOnScroll();
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::deselectItem()
    {
        updateSelectedItem(-1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItem(const String& itemName)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].text.getString() == itemName)
                return removeItemByIndex(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItemById(const String& id)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].id == id)
                return removeItemByIndex(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItemByIndex(std::size_t index)
    {
        if (index >= m_items.size())
            return false;

        // Keep it simple and forget hover when an item is removed
        updateHoveringItem(-1);

        // Check if the selected item should change
        if (m_selectedItem == static_cast<int>(index))
            updateSelectedItem(-1);
        else if (m_selectedItem > static_cast<int>(index))
        {
            // Don't call updateSelectedItem here, there should not be no callback and the item hasn't been erased yet so it would point to the wrong place
            m_selectedItem = m_selectedItem - 1;
        }

        // Remove the item
        m_items.erase(m_items.begin() + static_cast<std::ptrdiff_t>(index));

        m_scroll->setMaximum(static_cast<unsigned int>(m_items.size() * m_itemHeight));
        updateItemPositions();
        triggerOnScroll();

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::removeAllItems()
    {
        // Unselect any selected item
        updateSelectedItem(-1);
        updateHoveringItem(-1);

        // Clear the list, remove all items
        m_items.clear();

        m_scroll->setMaximum(0);
        triggerOnScroll();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String ListBox::getItemById(const String& id) const
    {
        for (const auto& item : m_items)
        {
            if (item.id == id)
                return item.text.getString();
        }

        return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String ListBox::getItemByIndex(std::size_t index) const
    {
        if (index >= m_items.size())
            return "";

        return m_items[index].text.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ListBox::getIndexById(const String& id) const
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].id == id)
                return static_cast<int>(i);
        }

        return -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String ListBox::getIdByIndex(std::size_t index) const
    {
        if (index >= m_items.size())
            return "";

        return m_items[index].id;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String ListBox::getSelectedItem() const
    {
        return (m_selectedItem >= 0) ? m_items[static_cast<std::size_t>(m_selectedItem)].text.getString() : "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String ListBox::getSelectedItemId() const
    {
        return (m_selectedItem >= 0) ? m_items[static_cast<std::size_t>(m_selectedItem)].id : "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ListBox::getSelectedItemIndex() const
    {
        return m_selectedItem;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::changeItem(const String& originalValue, const String& newValue)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].text.getString() == originalValue)
                return changeItemByIndex(i, newValue);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::changeItemById(const String& id, const String& newValue)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].id == id)
                return changeItemByIndex(i, newValue);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::changeItemByIndex(std::size_t index, const String& newValue)
    {
        if (index >= m_items.size())
            return false;

        m_items[index].text.setString(newValue);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ListBox::getItemCount() const
    {
        return m_items.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<String> ListBox::getItems() const
    {
        std::vector<String> items;
        for (const auto& item : m_items)
            items.push_back(item.text.getString());

        return items;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<String> ListBox::getItemIds() const
    {
        std::vector<String> ids;
        for (const auto& item : m_items)
            ids.push_back(item.id);

        return ids;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setItemData(std::size_t index, Any data)
    {
        if (index >= m_items.size())
        {
            TGUI_PRINT_WARNING("ListBox::setItemData called with invalid index.");
            return;
        }

        m_items[index].data = std::move(data);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setItemHeight(unsigned int itemHeight)
    {
        // Set the new heights
        m_itemHeight = itemHeight;

        if ((m_textSize == 0) && !getSharedRenderer()->getTextSize())
        {
            m_textSizeCached = Text::findBestTextSize(m_fontCached, m_itemHeight * 0.8f);
            for (auto& item : m_items)
                item.text.setCharacterSize(m_textSizeCached);
        }

        m_scroll->setScrollAmount(m_itemHeight);
        m_scroll->setMaximum(static_cast<unsigned int>(m_items.size() * m_itemHeight));
        updateItemPositions();
        triggerOnScroll();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListBox::getItemHeight() const
    {
        return m_itemHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateTextSize()
    {
        if ((m_textSize == 0) && !getSharedRenderer()->getTextSize())
            m_textSizeCached = Text::findBestTextSize(m_fontCached, m_itemHeight * 0.8f);

        for (auto& item : m_items)
            item.text.setCharacterSize(m_textSizeCached);

        updateItemPositions();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setMaximumItems(std::size_t maximumItems)
    {
        // Set the new limit
        m_maxItems = maximumItems;

        // Check if we already passed the limit
        if ((m_maxItems > 0) && (m_maxItems < m_items.size()))
        {
            if (m_hoveringItem >= static_cast<int>(maximumItems))
                updateHoveringItem(-1);
            if (m_selectedItem >= static_cast<int>(maximumItems))
                updateSelectedItem(-1);

            // Remove the items that passed the limitation
            m_items.erase(m_items.begin() + static_cast<std::ptrdiff_t>(m_maxItems), m_items.end());

            m_scroll->setMaximum(static_cast<unsigned int>(m_items.size() * m_itemHeight));
            updateItemPositions();
            triggerOnScroll();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ListBox::getMaximumItems() const
    {
        return m_maxItems;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setAutoScroll(bool autoScroll)
    {
        m_autoScroll = autoScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::getAutoScroll() const
    {
        return m_autoScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setTextAlignment(TextAlignment alignment)
    {
        m_textAlignment = alignment;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::TextAlignment ListBox::getTextAlignment() const
    {
        return m_textAlignment;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::contains(const String& itemStr) const
    {
        return std::find_if(m_items.begin(), m_items.end(), [itemStr](const Item& item){ return item.text.getString() == itemStr; }) != m_items.end();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::containsId(const String& id) const
    {
        return std::find_if(m_items.begin(), m_items.end(), [id](const Item& item){ return item.id == id; }) != m_items.end();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setScrollbarValue(unsigned int value)
    {
        m_scroll->setValue(value);
        triggerOnScroll();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListBox::getScrollbarValue() const
    {
        return m_scroll->getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::isMouseOnWidget(Vector2f pos) const
    {
        if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos))
        {
            if (!m_transparentTextureCached || !m_spriteBackground.isSet() || !m_spriteBackground.isTransparentPixel(pos - getPosition() - m_bordersCached.getOffset()))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        Widget::leftMousePressed(pos);

        bool isDragging = false;
        if (m_scroll->isMouseOnWidget(pos))
        {
            isDragging = m_scroll->leftMousePressed(pos);
            triggerOnScroll();
        }
        else
        {
            if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(), m_bordersCached.getTop() + m_paddingCached.getTop(),
                          getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
            {
                pos.y -= m_bordersCached.getTop() + m_paddingCached.getTop();

                // NOLINTNEXTLINE(bugprone-integer-division)
                const int hoveringItem = static_cast<int>(((pos.y - (m_itemHeight - (m_scroll->getValue() % m_itemHeight))) / m_itemHeight) + (m_scroll->getValue() / m_itemHeight) + 1);
                if (hoveringItem < static_cast<int>(m_items.size()))
                    updateHoveringItem(hoveringItem);
                else
                    updateHoveringItem(-1);

                if (m_selectedItem != m_hoveringItem)
                {
                    m_possibleDoubleClick = false;

                    updateSelectedItem(m_hoveringItem);
                }

                // Call the MousePress event after the item has already been changed, so that selected item represents the clicked item
                if (m_selectedItem >= 0)
                {
                    const Item& selectedItem = m_items[static_cast<std::size_t>(m_selectedItem)];
                    onMousePress.emit(this, m_selectedItem, selectedItem.text.getString(), selectedItem.id);
                }
            }
        }

        return isDragging;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::leftMouseReleased(Vector2f pos)
    {
        if (m_mouseDown && !m_scroll->isMouseDown())
        {
            if (m_selectedItem >= 0)
            {
                const Item& selectedItem = m_items[static_cast<std::size_t>(m_selectedItem)];
                onMouseRelease.emit(this, m_selectedItem, selectedItem.text.getString(), selectedItem.id);
            }

            // Check if you double-clicked
            if (m_possibleDoubleClick)
            {
                m_possibleDoubleClick = false;

                if (m_selectedItem >= 0)
                {
                    const Item& selectedItem = m_items[static_cast<std::size_t>(m_selectedItem)];
                    onDoubleClick.emit(this, m_selectedItem, selectedItem.text.getString(), selectedItem.id);
                }
            }
            else // This is the first click
            {
                m_animationTimeElapsed = {};
                m_possibleDoubleClick = true;
            }
        }

        m_scroll->leftMouseReleased(pos - getPosition());
        triggerOnScroll();

        Widget::leftMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        updateHoveringItem(-1);

        // Check if the mouse event should go to the scrollbar
        if ((m_scroll->isMouseDown() && m_scroll->isMouseDownOnThumb()) || m_scroll->isMouseOnWidget(pos))
        {
            m_scroll->mouseMoved(pos);
            triggerOnScroll();
        }
        else // Mouse not on scrollbar or dragging the scrollbar thumb
        {
            m_scroll->mouseNoLongerOnWidget();

            // Find out on which item the mouse is hovering
            if (FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(),
                          m_bordersCached.getTop() + m_paddingCached.getTop(), getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
            {
                pos.y -= m_bordersCached.getTop() + m_paddingCached.getTop();

                // NOLINTNEXTLINE(bugprone-integer-division)
                int hoveringItem = static_cast<int>(((pos.y - (m_itemHeight - (m_scroll->getValue() % m_itemHeight))) / m_itemHeight) + (m_scroll->getValue() / m_itemHeight) + 1);
                if (hoveringItem < static_cast<int>(m_items.size()))
                    updateHoveringItem(hoveringItem);
                else
                    updateHoveringItem(-1);

                // If the mouse is held down then select the item below the mouse
                if (m_mouseDown && !m_scroll->isMouseDown())
                {
                    if (m_selectedItem != m_hoveringItem)
                    {
                        m_possibleDoubleClick = false;

                        updateSelectedItem(m_hoveringItem);
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::scrolled(float delta, Vector2f pos, bool touch)
    {
        if (!m_scroll->isShown())
            return false;

        if (!m_scroll->scrolled(delta, pos - getPosition(), touch))
            return false;

        triggerOnScroll();

        // Update on which item the mouse is hovering
        mouseMoved(pos);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseNoLongerOnWidget()
    {
        Widget::mouseNoLongerOnWidget();
        m_scroll->mouseNoLongerOnWidget();

        updateHoveringItem(-1);

        m_possibleDoubleClick = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::leftMouseButtonNoLongerDown()
    {
        Widget::leftMouseButtonNoLongerDown();
        m_scroll->leftMouseButtonNoLongerDown();
        triggerOnScroll();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::keyPressed(const Event::KeyEvent& event)
    {
        Widget::keyPressed(event);
        if (event.code == Event::KeyboardKey::Up && (m_selectedItem > 0))
        {
            setSelectedItemByIndex(static_cast<std::size_t>(m_selectedItem - 1));
        }
        else if ((event.code == Event::KeyboardKey::Down)
              && (m_selectedItem >= 0) && (static_cast<std::size_t>(m_selectedItem) + 1 < m_items.size()))
        {
            setSelectedItemByIndex(static_cast<std::size_t>(m_selectedItem) + 1);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::canHandleKeyPress(const Event::KeyEvent& event)
    {
        if ((event.code == Event::KeyboardKey::Up) || (event.code == Event::KeyboardKey::Down))
            return true;
        else
            return Widget::canHandleKeyPress(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ListBox::getSignal(String signalName)
    {
        if (signalName == onItemSelect.getName())
            return onItemSelect;
        else if (signalName == onMousePress.getName())
            return onMousePress;
        else if (signalName == onMouseRelease.getName())
            return onMouseRelease;
        else if (signalName == onDoubleClick.getName())
            return onDoubleClick;
        else if (signalName == onScroll.getName())
            return onScroll;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::rendererChanged(const String& property)
    {
        if (property == U"Borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == U"Padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            setSize(m_size);
        }
        else if (property == U"TextColor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            updateItemColorsAndStyle();
        }
        else if (property == U"TextColorHover")
        {
            m_textColorHoverCached = getSharedRenderer()->getTextColorHover();
            updateItemColorsAndStyle();
        }
        else if (property == U"SelectedTextColor")
        {
            m_selectedTextColorCached = getSharedRenderer()->getSelectedTextColor();
            updateItemColorsAndStyle();
        }
        else if (property == U"SelectedTextColorHover")
        {
            m_selectedTextColorHoverCached = getSharedRenderer()->getSelectedTextColorHover();
            updateItemColorsAndStyle();
        }
        else if (property == U"TextureBackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == U"TextStyle")
        {
            m_textStyleCached = getSharedRenderer()->getTextStyle();

            for (auto& item : m_items)
                item.text.setStyle(m_textStyleCached);

            if ((m_selectedItem >= 0) && m_selectedTextStyleCached.isSet())
                m_items[static_cast<std::size_t>(m_selectedItem)].text.setStyle(m_selectedTextStyleCached);
        }
        else if (property == U"SelectedTextStyle")
        {
            m_selectedTextStyleCached = getSharedRenderer()->getSelectedTextStyle();

            if (m_selectedItem >= 0)
            {
                if (m_selectedTextStyleCached.isSet())
                    m_items[static_cast<std::size_t>(m_selectedItem)].text.setStyle(m_selectedTextStyleCached);
                else
                    m_items[static_cast<std::size_t>(m_selectedItem)].text.setStyle(m_textStyleCached);
            }
        }
        else if (property == U"Scrollbar")
        {
            m_scroll->setRenderer(getSharedRenderer()->getScrollbar());

            // If no scrollbar width was set then we may need to use the one from the texture
            if (getSharedRenderer()->getScrollbarWidth() == 0)
            {
                m_scroll->setSize({m_scroll->getDefaultWidth(), m_scroll->getSize().y});
                setSize(m_size);
            }
        }
        else if (property == U"ScrollbarWidth")
        {
            const float width = (getSharedRenderer()->getScrollbarWidth() != 0) ? getSharedRenderer()->getScrollbarWidth() : m_scroll->getDefaultWidth();
            m_scroll->setSize({width, m_scroll->getSize().y});
            setSize(m_size);
        }
        else if (property == U"BorderColor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == U"BackgroundColor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == U"BackgroundColorHover")
        {
            m_backgroundColorHoverCached = getSharedRenderer()->getBackgroundColorHover();
        }
        else if (property == U"SelectedBackgroundColor")
        {
            m_selectedBackgroundColorCached = getSharedRenderer()->getSelectedBackgroundColor();
        }
        else if (property == U"SelectedBackgroundColorHover")
        {
            m_selectedBackgroundColorHoverCached = getSharedRenderer()->getSelectedBackgroundColorHover();
        }
        else if ((property == U"Opacity") || (property == U"OpacityDisabled"))
        {
            Widget::rendererChanged(property);

            m_scroll->setInheritedOpacity(m_opacityCached);
            m_spriteBackground.setOpacity(m_opacityCached);
            for (auto& item : m_items)
                item.text.setOpacity(m_opacityCached);
        }
        else if (property == U"Font")
        {
            Widget::rendererChanged(property);

            for (auto& item : m_items)
                item.text.setFont(m_fontCached);

            // Recalculate the text size with the new font
            if ((m_textSize == 0) && !getSharedRenderer()->getTextSize())
            {
                m_textSizeCached = Text::findBestTextSize(m_fontCached, m_itemHeight * 0.8f);
                for (auto& item : m_items)
                    item.text.setCharacterSize(m_textSizeCached);
            }

            updateItemPositions();
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> ListBox::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (getItemCount() > 0)
        {
            const auto& items = getItems();
            const auto& ids = getItemIds();

            bool itemIdsUsed = false;
            String itemList = "[" + Serializer::serialize(items[0]);
            String itemIdList = "[" + Serializer::serialize(ids[0]);
            for (std::size_t i = 1; i < items.size(); ++i)
            {
                itemList += ", " + Serializer::serialize(items[i]);
                itemIdList += ", " + Serializer::serialize(ids[i]);

                if (!ids[i].empty())
                    itemIdsUsed = true;
            }
            itemList += "]";
            itemIdList += "]";

            node->propertyValuePairs[U"Items"] = std::make_unique<DataIO::ValueNode>(itemList);
            if (itemIdsUsed)
                node->propertyValuePairs[U"ItemIds"] = std::make_unique<DataIO::ValueNode>(itemIdList);
        }

        if (!m_autoScroll)
            node->propertyValuePairs[U"AutoScroll"] = std::make_unique<DataIO::ValueNode>("false");

        if (m_selectedItem >= 0)
            node->propertyValuePairs[U"SelectedItemIndex"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_selectedItem));

        if (m_textAlignment == TextAlignment::Center)
            node->propertyValuePairs[U"TextAlignment"] = std::make_unique<DataIO::ValueNode>("Center");
        else if (m_textAlignment == TextAlignment::Right)
            node->propertyValuePairs[U"TextAlignment"] = std::make_unique<DataIO::ValueNode>("Right");

        node->propertyValuePairs[U"ItemHeight"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_itemHeight));
        node->propertyValuePairs[U"MaximumItems"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_maxItems));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs[U"Items"])
        {
            if (!node->propertyValuePairs[U"Items"]->listNode)
                throw Exception{U"Failed to parse 'Items' property, expected a list as value"};

            if (node->propertyValuePairs[U"ItemIds"])
            {
                if (!node->propertyValuePairs[U"ItemIds"]->listNode)
                    throw Exception{U"Failed to parse 'ItemIds' property, expected a list as value"};

                if (node->propertyValuePairs[U"Items"]->valueList.size() != node->propertyValuePairs[U"ItemIds"]->valueList.size())
                    throw Exception{U"Amounts of values for 'Items' differs from the amount in 'ItemIds'"};

                for (std::size_t i = 0; i < node->propertyValuePairs[U"Items"]->valueList.size(); ++i)
                {
                    addItem(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"Items"]->valueList[i]).getString(),
                            Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"ItemIds"]->valueList[i]).getString());
                }
            }
            else // There are no item ids
            {
                for (const auto& item : node->propertyValuePairs[U"Items"]->valueList)
                    addItem(Deserializer::deserialize(ObjectConverter::Type::String, item).getString());
            }
        }
        else // If there are no items, there should be no item ids
        {
            if (node->propertyValuePairs[U"ItemIds"])
                throw Exception{U"Found 'ItemIds' property while there is no 'Items' property"};
        }

        if (node->propertyValuePairs[U"TextAlignment"])
        {
            String alignment = Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[U"TextAlignment"]->value).getString();
            if (alignment == U"Right")
                setTextAlignment(TextAlignment::Right);
            else if (alignment == U"Center")
                setTextAlignment(TextAlignment::Center);
            else if (alignment != U"Left")
                throw Exception{U"Failed to parse TextAlignment property, found unknown value."};
        }

        if (node->propertyValuePairs[U"AutoScroll"])
            setAutoScroll(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"AutoScroll"]->value).getBool());
        if (node->propertyValuePairs[U"ItemHeight"])
            setItemHeight(node->propertyValuePairs[U"ItemHeight"]->value.toUInt());
        if (node->propertyValuePairs[U"MaximumItems"])
            setMaximumItems(node->propertyValuePairs[U"MaximumItems"]->value.toUInt());
        if (node->propertyValuePairs[U"SelectedItemIndex"])
            setSelectedItemByIndex(node->propertyValuePairs[U"SelectedItemIndex"]->value.toUInt());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ListBox::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateItemPositions()
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
            m_items[i].text.setPosition({0, (i * m_itemHeight) + ((m_itemHeight - m_items[i].text.getSize().y) / 2.0f)});

        m_scroll->setPosition(getSize().x - m_bordersCached.getRight() - m_scroll->getSize().x, m_bordersCached.getTop());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateSelectedAndHoveringItemColorsAndStyle()
    {
        if (m_selectedItem >= 0)
        {
            Item& selectedItem = m_items[static_cast<std::size_t>(m_selectedItem)];
            if ((m_selectedItem == m_hoveringItem) && m_selectedTextColorHoverCached.isSet())
                selectedItem.text.setColor(m_selectedTextColorHoverCached);
            else if (m_selectedTextColorCached.isSet())
                selectedItem.text.setColor(m_selectedTextColorCached);

            if (m_selectedTextStyleCached.isSet())
                selectedItem.text.setStyle(m_selectedTextStyleCached);
        }

        if ((m_hoveringItem >= 0) && (m_selectedItem != m_hoveringItem))
        {
            if (m_textColorHoverCached.isSet())
                m_items[static_cast<std::size_t>(m_hoveringItem)].text.setColor(m_textColorHoverCached);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateItemColorsAndStyle()
    {
        for (auto& item : m_items)
        {
            item.text.setColor(m_textColorCached);
            item.text.setStyle(m_textStyleCached);
        }

        updateSelectedAndHoveringItemColorsAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateHoveringItem(int item)
    {
        if (m_hoveringItem != item)
        {
            if (m_hoveringItem >= 0)
            {
                if ((m_selectedItem == m_hoveringItem) && m_selectedTextColorCached.isSet())
                    m_items[static_cast<std::size_t>(m_hoveringItem)].text.setColor(m_selectedTextColorCached);
                else
                    m_items[static_cast<std::size_t>(m_hoveringItem)].text.setColor(m_textColorCached);
            }

            m_hoveringItem = item;

            updateSelectedAndHoveringItemColorsAndStyle();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateSelectedItem(int item)
    {
        if (m_selectedItem == item)
            return;

        if (m_selectedItem >= 0)
        {
            Item& selectedItem = m_items[static_cast<std::size_t>(m_selectedItem)];
            if ((m_selectedItem == m_hoveringItem) && m_textColorHoverCached.isSet())
                selectedItem.text.setColor(m_textColorHoverCached);
            else
                selectedItem.text.setColor(m_textColorCached);

            selectedItem.text.setStyle(m_textStyleCached);
        }

        m_selectedItem = item;
        if (m_selectedItem >= 0)
        {
            const Item& selectedItem = m_items[static_cast<std::size_t>(m_selectedItem)];
            onItemSelect.emit(this, m_selectedItem, selectedItem.text.getString(), selectedItem.id);
        }
        else
            onItemSelect.emit(this, m_selectedItem, "", "");

        updateSelectedAndHoveringItemColorsAndStyle();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::triggerOnScroll()
    {
        const unsigned int currentScrollbarValue = m_scroll->getValue();
        if (currentScrollbarValue == m_lastScrollbarValue)
            return;

        m_lastScrollbarValue = currentScrollbarValue;
        onScroll.emit(this, currentScrollbarValue);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::updateTime(Duration elapsedTime)
    {
        const bool screenRefreshRequired = Widget::updateTime(elapsedTime);

        if (m_animationTimeElapsed >= getDoubleClickTime())
        {
            m_animationTimeElapsed = {};
            m_possibleDoubleClick = false;
        }

        return screenRefreshRequired;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::draw(BackendRenderTarget& target, RenderStates states) const
    {
        const RenderStates statesForScrollbar = states;

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorCached, m_opacityCached));
            states.transform.translate(m_bordersCached.getOffset());
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            target.drawSprite(states, m_spriteBackground);
        else
            target.drawFilledRect(states, getInnerSize(), Color::applyOpacity(m_backgroundColorCached, m_opacityCached));

        // Draw the items and their selected/hover backgrounds
        {
            float maxItemWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
            if (m_scroll->isShown())
                maxItemWidth -= m_scroll->getSize().x;

            target.addClippingLayer(states, {{m_paddingCached.getLeft(), m_paddingCached.getTop()}, {maxItemWidth, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}});

            // Find out which items are visible
            std::size_t firstItem = 0;
            std::size_t lastItem = m_items.size();
            if (m_scroll->getViewportSize() < m_scroll->getMaximum())
            {
                firstItem = m_scroll->getValue() / m_itemHeight;
                lastItem = (m_scroll->getValue() + m_scroll->getViewportSize()) / m_itemHeight;

                // Show another item when the scrollbar is standing between two items
                if ((m_scroll->getValue() + m_scroll->getViewportSize()) % m_itemHeight != 0)
                    ++lastItem;
            }

            states.transform.translate({m_paddingCached.getLeft(), m_paddingCached.getTop() - m_scroll->getValue()});

            // Draw the background of the selected item
            if (m_selectedItem >= 0)
            {
                states.transform.translate({0, m_selectedItem * static_cast<float>(m_itemHeight)});

                const Vector2f size = {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), static_cast<float>(m_itemHeight)};
                if ((m_selectedItem == m_hoveringItem) && m_selectedBackgroundColorHoverCached.isSet())
                    target.drawFilledRect(states, size, Color::applyOpacity(m_selectedBackgroundColorHoverCached, m_opacityCached));
                else
                    target.drawFilledRect(states, size, Color::applyOpacity(m_selectedBackgroundColorCached, m_opacityCached));

                states.transform.translate({0, -m_selectedItem * static_cast<float>(m_itemHeight)});
            }

            // Draw the background of the item on which the mouse is standing
            if ((m_hoveringItem >= 0) && (m_hoveringItem != m_selectedItem) && m_backgroundColorHoverCached.isSet())
            {
                states.transform.translate({0, m_hoveringItem * static_cast<float>(m_itemHeight)});
                target.drawFilledRect(states, {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), static_cast<float>(m_itemHeight)}, Color::applyOpacity(m_backgroundColorHoverCached, m_opacityCached));
                states.transform.translate({0, -m_hoveringItem * static_cast<float>(m_itemHeight)});
            }

            // Draw the items
            if (m_textAlignment == ListBox::TextAlignment::Right)
            {
                const float textPadding = Text::getExtraHorizontalPadding(m_fontCached, m_textSizeCached);
                for (std::size_t i = firstItem; i < lastItem; ++i)
                {
                    const float textWidth = m_items[i].text.getSize().x;
                    states.transform.translate({maxItemWidth - textPadding - textWidth, 0});
                    target.drawText(states, m_items[i].text);
                    states.transform.translate({-maxItemWidth + textPadding + textWidth, 0});
                }
            }
            else if (m_textAlignment == ListBox::TextAlignment::Center)
            {
                for (std::size_t i = firstItem; i < lastItem; ++i)
                {
                    const float textWidth = m_items[i].text.getSize().x;
                    states.transform.translate({(maxItemWidth - textWidth) / 2.f, 0});
                    target.drawText(states, m_items[i].text);
                    states.transform.translate({-(maxItemWidth - textWidth) / 2.f, 0});
                }
            }
            else // m_textAlignment == ListBox::TextAlignment::Left
            {
                states.transform.translate({Text::getExtraHorizontalPadding(m_fontCached, m_textSizeCached), 0});
                for (std::size_t i = firstItem; i < lastItem; ++i)
                    target.drawText(states, m_items[i].text);
            }

            target.removeClippingLayer();
        }

        // Draw the scrollbar
        m_scroll->draw(target, statesForScrollbar);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr ListBox::clone() const
    {
        return std::make_shared<ListBox>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
