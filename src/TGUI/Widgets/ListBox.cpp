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


#include <TGUI/Widgets/ListBox.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    static std::map<std::string, ObjectConverter> defaultRendererValues =
            {
                {"borders", Borders{2}},
                {"padding", Padding{0}},
                {"bordercolor", sf::Color::Black},
                {"textcolor", Color{60, 60, 60}},
                {"textcolorhover", sf::Color::Black},
                {"selectedtextcolor", sf::Color::White},
                {"backgroundcolor", Color{245, 245, 245}},
                {"backgroundcolorhover", sf::Color::White},
                {"selectedbackgroundcolor", Color{0, 110, 255}},
                {"selectedbackgroundcolorhover", Color{30, 150, 255}}
            };

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::ListBox()
    {
        m_type = "ListBox";

        m_draggableWidget = true;

        m_renderer = aurora::makeCopied<ListBoxRenderer>();
        setRenderer(RendererData::create(defaultRendererValues));

        setSize({150, 154});
        setItemHeight(m_itemHeight);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::Ptr ListBox::create()
    {
        return std::make_shared<ListBox>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::Ptr ListBox::copy(ListBox::ConstPtr listBox)
    {
        if (listBox)
            return std::static_pointer_cast<ListBox>(listBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setPosition(const Layout2d& position)
    {
        Widget::setPosition(position);

        for (std::size_t i = 0; i < m_items.size(); ++i)
            m_items[i].setPosition({0, (i * m_itemHeight) + ((m_itemHeight - m_items[i].getSize().y) / 2.0f)});

        m_scroll.setPosition(getSize().x - m_bordersCached.getRight() - m_paddingCached.getRight() - m_scroll.getSize().x, m_bordersCached.getTop() + m_paddingCached.getTop());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_bordersCached.updateParentSize(getSize());
        m_paddingCached.updateParentSize(getSize());

        m_spriteBackground.setSize(getInnerSize());

        m_scroll.setSize({m_scroll.getSize().x, std::max(0.f, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom())});
        m_scroll.setLowValue(static_cast<unsigned int>(m_scroll.getSize().y));

        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::addItem(const sf::String& itemName, const sf::String& id)
    {
        // Check if the item limit is reached (if there is one)
        if ((m_maxItems == 0) || (m_items.size() < m_maxItems))
        {
            m_scroll.setMaximum(static_cast<unsigned int>((m_items.size() + 1) * m_itemHeight));

            // Scroll down when auto-scrolling is enabled
            if (m_autoScroll && (m_scroll.getLowValue() < m_scroll.getMaximum()))
                m_scroll.setValue(m_scroll.getMaximum() - m_scroll.getLowValue());

            // Create the new item
            Text newItem;
            newItem.setFont(m_fontCached);
            newItem.setColor(m_textColorCached);
            newItem.setOpacity(m_opacityCached);
            newItem.setStyle(m_textStyleCached);
            newItem.setCharacterSize(m_textSize);
            newItem.setString(itemName);
            newItem.setPosition({0, (m_items.size() * m_itemHeight) + ((m_itemHeight - newItem.getSize().y) / 2.0f)});

            // Add the new item to the list
            m_items.push_back(std::move(newItem));
            m_itemIds.push_back(id);
            return true;
        }
        else // The item limit was reached
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::setSelectedItem(const sf::String& itemName)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].getString() == itemName)
                return setSelectedItemByIndex(i);
        }

        // No match was found
        deselectItem();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::setSelectedItemById(const sf::String& id)
    {
        for (std::size_t i = 0; i < m_itemIds.size(); ++i)
        {
            if (m_itemIds[i] == id)
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
        if (m_selectedItem * getItemHeight() < m_scroll.getValue())
            m_scroll.setValue(m_selectedItem * getItemHeight());
        else if ((m_selectedItem + 1) * getItemHeight() > m_scroll.getValue() + m_scroll.getLowValue())
            m_scroll.setValue((m_selectedItem + 1) * getItemHeight() - m_scroll.getLowValue());

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::deselectItem()
    {
        updateSelectedItem(-1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItem(const sf::String& itemName)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].getString() == itemName)
                return removeItemByIndex(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItemById(const sf::String& id)
    {
        for (std::size_t i = 0; i < m_itemIds.size(); ++i)
        {
            if (m_itemIds[i] == id)
                return removeItemByIndex(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItemByIndex(std::size_t index)
    {
        if (index >= m_items.size())
            return false;

        // Remove the item
        m_items.erase(m_items.begin() + index);
        m_itemIds.erase(m_itemIds.begin() + index);

        m_scroll.setMaximum(static_cast<unsigned int>(m_items.size() * m_itemHeight));
        setPosition(m_position);

        // Keep it simple and forget hover when an item is removed
        updateHoveringItem(-1);

        // Check if the selected item should change
        if (m_selectedItem == static_cast<int>(index))
            updateSelectedItem(-1);
        else if (m_selectedItem > static_cast<int>(index))
            updateSelectedItem(m_selectedItem - 1);

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::removeAllItems()
    {
        // Clear the list, remove all items
        m_items.clear();
        m_itemIds.clear();

        // Unselect any selected item
        updateSelectedItem(-1);
        updateHoveringItem(-1);

        m_scroll.setMaximum(0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ListBox::getItemById(const sf::String& id) const
    {
        for (std::size_t i = 0; i < m_itemIds.size(); ++i)
        {
            if (m_itemIds[i] == id)
                return m_items[i].getString();
        }

        return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ListBox::getSelectedItem() const
    {
        return (m_selectedItem >= 0) ? m_items[m_selectedItem].getString() : "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ListBox::getSelectedItemId() const
    {
        return (m_selectedItem >= 0) ? m_itemIds[m_selectedItem] : "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ListBox::getSelectedItemIndex() const
    {
        return m_selectedItem;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::changeItem(const sf::String& originalValue, const sf::String& newValue)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].getString() == originalValue)
                return changeItemByIndex(i, newValue);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::changeItemById(const sf::String& id, const sf::String& newValue)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_itemIds[i] == id)
                return changeItemByIndex(i, newValue);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::changeItemByIndex(std::size_t index, const sf::String& newValue)
    {
        if (index >= m_items.size())
            return false;

        m_items[index].setString(newValue);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t ListBox::getItemCount() const
    {
        return m_items.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String> ListBox::getItems() const
    {
        std::vector<sf::String> items;
        for (const auto& item : m_items)
            items.push_back(item.getString());

        return items;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<sf::String>& ListBox::getItemIds() const
    {
        return m_itemIds;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setItemHeight(unsigned int itemHeight)
    {
        // Set the new heights
        m_itemHeight = itemHeight;
        if (m_requestedTextSize == 0)
        {
            m_textSize = Text::findBestTextSize(m_fontCached, itemHeight * 0.8f);
            for (auto& item : m_items)
                item.setCharacterSize(m_textSize);
        }

        m_scroll.setScrollAmount(m_itemHeight);
        m_scroll.setMaximum(static_cast<unsigned int>(m_items.size() * m_itemHeight));
        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListBox::getItemHeight() const
    {
        return m_itemHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setTextSize(unsigned int textSize)
    {
        m_requestedTextSize = textSize;

        if (textSize)
            m_textSize = textSize;
        else
            m_textSize = Text::findBestTextSize(m_fontCached, m_itemHeight * 0.8f);

        for (auto& item : m_items)
            item.setCharacterSize(m_textSize);

        setPosition(m_position);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListBox::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setMaximumItems(std::size_t maximumItems)
    {
        // Set the new limit
        m_maxItems = maximumItems;

        // Check if we already passed the limit
        if ((m_maxItems > 0) && (m_maxItems < m_items.size()))
        {
            // Remove the items that passed the limitation
            m_items.erase(m_items.begin() + m_maxItems, m_items.end());
            m_itemIds.erase(m_itemIds.begin() + m_maxItems, m_itemIds.end());

            m_scroll.setMaximum(static_cast<unsigned int>(m_items.size() * m_itemHeight));
            setPosition(m_position);
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

    bool ListBox::mouseOnWidget(sf::Vector2f pos) const
    {
        return sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::leftMousePressed(sf::Vector2f pos)
    {
        pos -= getPosition();

        m_mouseDown = true;

        if (m_scroll.mouseOnWidget(pos))
        {
            m_scroll.leftMousePressed(pos);
        }
        else
        {
            if (sf::FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(), m_bordersCached.getTop() + m_paddingCached.getTop(),
                              getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
            {
                pos.y -= m_bordersCached.getTop() + m_paddingCached.getTop();

                int hoveringItem = static_cast<int>(((pos.y - (m_itemHeight - (m_scroll.getValue() % m_itemHeight))) / m_itemHeight) + (m_scroll.getValue() / m_itemHeight) + 1);
                if (hoveringItem < static_cast<int>(m_items.size()))
                    updateHoveringItem(hoveringItem);
                else
                    updateHoveringItem(-1);

                if (m_hoveringItem >= 0)
                    onMousePress.emit(this, m_items[m_hoveringItem].getString(), m_itemIds[m_hoveringItem]);

                if (m_selectedItem != m_hoveringItem)
                {
                    m_possibleDoubleClick = false;

                    updateSelectedItem(m_hoveringItem);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::leftMouseReleased(sf::Vector2f pos)
    {
        if (m_mouseDown && !m_scroll.isMouseDown())
        {
            if (m_selectedItem >= 0)
                onMouseRelease.emit(this, m_items[m_selectedItem].getString(), m_itemIds[m_selectedItem]);

            // Check if you double-clicked
            if (m_possibleDoubleClick)
            {
                m_possibleDoubleClick = false;

                if (m_selectedItem >= 0)
                    onDoubleClick.emit(this, m_items[m_selectedItem].getString(), m_itemIds[m_selectedItem]);
            }
            else // This is the first click
            {
                m_animationTimeElapsed = {};
                m_possibleDoubleClick = true;
            }
        }

        m_scroll.leftMouseReleased(pos - getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseMoved(sf::Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        updateHoveringItem(-1);

        // Check if the mouse event should go to the scrollbar
        if ((m_scroll.isMouseDown() && m_scroll.isMouseDownOnThumb()) || m_scroll.mouseOnWidget(pos))
        {
            m_scroll.mouseMoved(pos);
        }
        else // Mouse not on scrollbar or dragging the scrollbar thumb
        {
            m_scroll.mouseNoLongerOnWidget();

            // Find out on which item the mouse is hovering
            if (sf::FloatRect{m_bordersCached.getLeft() + m_paddingCached.getLeft(),
                              m_bordersCached.getTop() + m_paddingCached.getTop(), getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}.contains(pos))
            {
                pos.y -= m_bordersCached.getTop() + m_paddingCached.getTop();

                int hoveringItem = static_cast<int>(((pos.y - (m_itemHeight - (m_scroll.getValue() % m_itemHeight))) / m_itemHeight) + (m_scroll.getValue() / m_itemHeight) + 1);
                if (hoveringItem < static_cast<int>(m_items.size()))
                    updateHoveringItem(hoveringItem);
                else
                    updateHoveringItem(-1);

                // If the mouse is held down then select the item below the mouse
                if (m_mouseDown && !m_scroll.isMouseDown())
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

    void ListBox::mouseWheelScrolled(float delta, sf::Vector2f pos)
    {
        if (m_scroll.isShown())
        {
            m_scroll.mouseWheelScrolled(delta, pos - getPosition());

            // Update on which item the mouse is hovering
            mouseMoved(pos);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseNoLongerOnWidget()
    {
        Widget::mouseNoLongerOnWidget();
        m_scroll.mouseNoLongerOnWidget();

        updateHoveringItem(-1);

        m_possibleDoubleClick = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseNoLongerDown()
    {
        Widget::mouseNoLongerDown();
        m_scroll.mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ListBox::getSignal(std::string signalName)
    {
        if (signalName == toLower(onItemSelect.getName()))
            return onItemSelect;
        else if (signalName == toLower(onMousePress.getName()))
            return onMousePress;
        else if (signalName == toLower(onMouseRelease.getName()))
            return onMouseRelease;
        else if (signalName == toLower(onDoubleClick.getName()))
            return onDoubleClick;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getRenderer()->getBorders();
            setSize(m_size);
        }
        else if (property == "padding")
        {
            m_paddingCached = getRenderer()->getPadding();
            setSize(m_size);
        }
        else if (property == "textcolor")
        {
            m_textColorCached = getRenderer()->getTextColor();
            updateItemColorsAndStyle();
        }
        else if (property == "textcolorhover")
        {
            m_textColorHoverCached = getRenderer()->getTextColorHover();
            updateItemColorsAndStyle();
        }
        else if (property == "selectedtextcolor")
        {
            m_selectedTextColorCached = getRenderer()->getSelectedTextColor();
            updateItemColorsAndStyle();
        }
        else if (property == "selectedtextcolorhover")
        {
            m_selectedTextColorHoverCached = getRenderer()->getSelectedTextColorHover();
            updateItemColorsAndStyle();
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getRenderer()->getTextureBackground());
        }
        else if (property == "textstyle")
        {
            m_textStyleCached = getRenderer()->getTextStyle();

            for (auto& item : m_items)
                item.setStyle(m_textStyleCached);

            if ((m_selectedItem >= 0) && m_selectedTextStyleCached.isSet())
                m_items[m_selectedItem].setStyle(m_selectedTextStyleCached);
        }
        else if (property == "selectedtextstyle")
        {
            m_selectedTextStyleCached = getRenderer()->getSelectedTextStyle();

            if (m_selectedItem >= 0)
            {
                if (m_selectedTextStyleCached.isSet())
                    m_items[m_selectedItem].setStyle(m_selectedTextStyleCached);
                else
                    m_items[m_selectedItem].setStyle(m_textStyleCached);
            }
        }
        else if (property == "scrollbar")
        {
            m_scroll.setRenderer(getRenderer()->getScrollbar());
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getRenderer()->getBorderColor();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getRenderer()->getBackgroundColor();
        }
        else if (property == "backgroundcolorhover")
        {
            m_backgroundColorHoverCached = getRenderer()->getBackgroundColorHover();
        }
        else if (property == "selectedbackgroundcolor")
        {
            m_selectedBackgroundColorCached = getRenderer()->getSelectedBackgroundColor();
        }
        else if (property == "selectedbackgroundcolorhover")
        {
            m_selectedBackgroundColorHoverCached = getRenderer()->getSelectedBackgroundColorHover();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_spriteBackground.setOpacity(m_opacityCached);
            m_scroll.getRenderer()->setOpacity(m_opacityCached);
            for (auto& item : m_items)
                item.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            for (auto& item : m_items)
                item.setFont(m_fontCached);

            // Recalculate the text size with the new font
            if (m_requestedTextSize == 0)
            {
                m_textSize = Text::findBestTextSize(m_fontCached, m_itemHeight * 0.8f);
                for (auto& item : m_items)
                    item.setCharacterSize(m_textSize);
            }

            setPosition(m_position);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ListBox::getInnerSize() const
    {
        return {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(), getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateSelectedAndHoveringItemColorsAndStyle()
    {
        if (m_selectedItem >= 0)
        {
            if ((m_selectedItem == m_hoveringItem) && m_selectedTextColorHoverCached.isSet())
                m_items[m_selectedItem].setColor(m_selectedTextColorHoverCached);
            else if (m_selectedTextColorCached.isSet())
                m_items[m_selectedItem].setColor(m_selectedTextColorCached);

            if (m_selectedTextStyleCached.isSet())
                m_items[m_selectedItem].setStyle(m_selectedTextStyleCached);
        }

        if ((m_hoveringItem >= 0) && (m_selectedItem != m_hoveringItem))
        {
            if (m_textColorHoverCached.isSet())
                m_items[m_hoveringItem].setColor(m_textColorHoverCached);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateItemColorsAndStyle()
    {
        for (auto& item : m_items)
        {
            item.setColor(m_textColorCached);
            item.setStyle(m_textStyleCached);
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
                    m_items[m_hoveringItem].setColor(m_selectedTextColorCached);
                else
                    m_items[m_hoveringItem].setColor(m_textColorCached);
            }

            m_hoveringItem = item;

            updateSelectedAndHoveringItemColorsAndStyle();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateSelectedItem(int item)
    {
        if (m_selectedItem != item)
        {
            if (m_selectedItem >= 0)
            {
                if ((m_selectedItem == m_hoveringItem) && m_textColorHoverCached.isSet())
                    m_items[m_selectedItem].setColor(m_textColorHoverCached);
                else
                    m_items[m_selectedItem].setColor(m_textColorCached);

                m_items[m_selectedItem].setStyle(m_textStyleCached);
            }

            m_selectedItem = item;
            if (m_selectedItem >= 0)
                onItemSelect.emit(this, m_items[m_selectedItem].getString(), m_itemIds[m_selectedItem]);
            else
                onItemSelect.emit(this, "", "");

            updateSelectedAndHoveringItemColorsAndStyle();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::update(sf::Time elapsedTime)
    {
        Widget::update(elapsedTime);

        // When double-clicking, the second click has to come within 500 milliseconds
        if (m_animationTimeElapsed >= sf::milliseconds(500))
        {
            m_animationTimeElapsed = {};
            m_possibleDoubleClick = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());
        const sf::RenderStates statesForScrollbar = states;

        // Draw the borders
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        // Draw the background
        if (m_spriteBackground.isSet())
            m_spriteBackground.draw(target, states);
        else
            drawRectangleShape(target, states, getInnerSize(), m_backgroundColorCached);

        // Draw the items and their selected/hover backgrounds
        {
            // Set the clipping for all draw calls that happen until this clipping object goes out of scope
            float maxItemWidth = getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight();
            if (m_scroll.isShown())
                maxItemWidth -= m_scroll.getSize().x;
            const Clipping clipping{target, states, {m_paddingCached.getLeft(), m_paddingCached.getTop()}, {maxItemWidth, getInnerSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()}};

            // Find out which items are visible
            std::size_t firstItem = 0;
            std::size_t lastItem = m_items.size();
            if (m_scroll.getLowValue() < m_scroll.getMaximum())
            {
                firstItem = m_scroll.getValue() / m_itemHeight;
                lastItem = (m_scroll.getValue() + m_scroll.getLowValue()) / m_itemHeight;

                // Show another item when the scrollbar is standing between two items
                if ((m_scroll.getValue() + m_scroll.getLowValue()) % m_itemHeight != 0)
                    ++lastItem;
            }

            states.transform.translate({m_paddingCached.getLeft(), m_paddingCached.getTop() - m_scroll.getValue()});

            // Draw the background of the selected item
            if (m_selectedItem >= 0)
            {
                states.transform.translate({0, static_cast<float>(m_selectedItem * m_itemHeight)});

                const sf::Vector2f size = {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), static_cast<float>(m_itemHeight)};
                if ((m_selectedItem == m_hoveringItem) && m_selectedBackgroundColorHoverCached.isSet())
                    drawRectangleShape(target, states, size, m_selectedBackgroundColorHoverCached);
                else
                    drawRectangleShape(target, states, size, m_selectedBackgroundColorCached);

                states.transform.translate({0, -static_cast<float>(m_selectedItem * m_itemHeight)});
            }

            // Draw the background of the item on which the mouse is standing
            if ((m_hoveringItem >= 0) && (m_hoveringItem != m_selectedItem) && m_backgroundColorHoverCached.isSet())
            {
                states.transform.translate({0, static_cast<float>(m_hoveringItem * m_itemHeight)});
                drawRectangleShape(target, states, {getInnerSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(), static_cast<float>(m_itemHeight)}, m_backgroundColorHoverCached);
                states.transform.translate({0, -static_cast<float>(m_hoveringItem * m_itemHeight)});
            }

            // Draw the items
            for (std::size_t i = firstItem; i < lastItem; ++i)
                m_items[i].draw(target, states);
        }

        // Draw the scrollbar
        m_scroll.draw(target, statesForScrollbar);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
