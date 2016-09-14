/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <SFML/OpenGL.hpp>

#include <TGUI/Container.hpp>
#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Widgets/ListBox.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::ListBox()
    {
        m_callback.widgetType = "ListBox";
        m_type = "ListBox";

        m_draggableWidget = true;

        addSignal<sf::String, TypeSet<sf::String, sf::String>>("ItemSelected");
        addSignal<sf::String, TypeSet<sf::String, sf::String>>("MousePressed");
        addSignal<sf::String, TypeSet<sf::String, sf::String>>("MouseReleased");
        addSignal<sf::String, TypeSet<sf::String, sf::String>>("DoubleClicked");

        m_renderer = aurora::makeCopied<ListBoxRenderer>();
        setRenderer(m_renderer->getData());

        getRenderer()->setBorders({2});
        getRenderer()->setBackgroundColor({245, 245, 245});
        getRenderer()->setTextColor({60, 60, 60});
        getRenderer()->setTextColorHover(sf::Color::Black);
        getRenderer()->setBackgroundColorHover(sf::Color::White);
        getRenderer()->setSelectedBackgroundColor({0, 110, 255});
        getRenderer()->setSelectedBackgroundColorHover({30, 150, 255});
        getRenderer()->setSelectedTextColor(sf::Color::White);

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

        Borders borders = getRenderer()->getBorders();
        Padding padding = getRenderer()->getPadding();
        m_scroll.setPosition(getSize().x - borders.right - padding.right - m_scroll.getSize().x, borders.top + padding.top);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        getRenderer()->getTextureBackground().setSize(getInnerSize());

        Padding padding = getRenderer()->getPadding();
        m_scroll.setSize({m_scroll.getSize().x, std::max(0.f, getInnerSize().y - padding.top - padding.bottom)});
        m_scroll.setLowValue(static_cast<unsigned int>(m_scroll.getSize().y));

        updatePosition();
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
            newItem.setFont(getRenderer()->getFont());
            newItem.setColor(getRenderer()->getTextColor());
            newItem.setOpacity(getRenderer()->getOpacity());
            newItem.setStyle(getRenderer()->getTextStyle());
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
        updatePosition();

        // Keep it simple and forget hover when an item is removed
        updateHoveringItem(-1);

        // Check if the selected item should change
        if (m_selectedItem == static_cast<int>(index))
            m_selectedItem = -1;
        else if (m_selectedItem > static_cast<int>(index))
        {
            updateSelectedItem(m_selectedItem - 1);
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::removeAllItems()
    {
        // Clear the list, remove all items
        m_items.clear();
        m_itemIds.clear();

        // Unselect any selected item
        m_selectedItem = -1;
        m_hoveringItem = -1;

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
            m_textSize = findBestTextSize(getRenderer()->getFont(), itemHeight * 0.8f);
            for (auto& item : m_items)
                item.setCharacterSize(m_textSize);
        }

        m_scroll.setScrollAmount(m_itemHeight);
        m_scroll.setMaximum(static_cast<unsigned int>(m_items.size() * m_itemHeight));
        updatePosition();
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
            m_textSize = findBestTextSize(getRenderer()->getFont(), m_itemHeight * 0.8f);

        for (auto& item : m_items)
            item.setCharacterSize(m_textSize);

        updatePosition();
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
            updatePosition();
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
        return sf::FloatRect{0, 0, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::leftMousePressed(sf::Vector2f pos)
    {
        m_mouseDown = true;

        if (m_scroll.mouseOnWidget(pos - m_scroll.getPosition()))
        {
            m_scroll.leftMousePressed(pos - m_scroll.getPosition());
        }
        else
        {
            Borders borders = getRenderer()->getBorders();
            Padding padding = getRenderer()->getPadding();
            if (sf::FloatRect{borders.left + padding.left, borders.top + padding.top, getInnerSize().x - padding.left - padding.right, getInnerSize().y - padding.top - padding.bottom}.contains(pos))
            {
                pos.y -= borders.top + padding.top;

                int hoveringItem = static_cast<int>(((pos.y - (m_itemHeight - (m_scroll.getValue() % m_itemHeight))) / m_itemHeight) + (m_scroll.getValue() / m_itemHeight) + 1);
                if (hoveringItem < static_cast<int>(m_items.size()))
                    updateHoveringItem(hoveringItem);
                else
                    updateHoveringItem(-1);

                if (m_hoveringItem >= 0)
                {
                    m_callback.text = m_items[m_hoveringItem].getString();
                    m_callback.itemId = m_itemIds[m_hoveringItem];
                    sendSignal("MousePressed", m_items[m_hoveringItem].getString(), m_items[m_hoveringItem].getString(), m_itemIds[m_hoveringItem]);
                }

                if (m_selectedItem != m_hoveringItem)
                {
                    m_possibleDoubleClick = false;

                    updateSelectedItem(m_hoveringItem);

                    if (m_selectedItem >= 0)
                    {
                        m_callback.text = m_items[m_selectedItem].getString();
                        m_callback.itemId = m_itemIds[m_selectedItem];
                        sendSignal("ItemSelected", m_items[m_selectedItem].getString(), m_items[m_selectedItem].getString(), m_itemIds[m_selectedItem]);
                    }
                    else
                    {
                        m_callback.text = "";
                        m_callback.itemId = "";
                        sendSignal("ItemSelected", "", "", "");
                    }
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
            {
                m_callback.text  = m_items[m_selectedItem].getString();
                m_callback.itemId = m_itemIds[m_selectedItem];
                sendSignal("MouseReleased", m_items[m_selectedItem].getString(), m_items[m_selectedItem].getString(), m_itemIds[m_selectedItem]);
            }

            // Check if you double-clicked
            if (m_possibleDoubleClick)
            {
                m_possibleDoubleClick = false;

                if (m_selectedItem >= 0)
                    sendSignal("DoubleClicked", m_items[m_selectedItem].getString(), m_items[m_selectedItem].getString(), m_itemIds[m_selectedItem]);
            }
            else // This is the first click
            {
                m_animationTimeElapsed = {};
                m_possibleDoubleClick = true;
            }
        }

        m_scroll.leftMouseReleased(pos - m_scroll.getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseMoved(sf::Vector2f pos)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

        updateHoveringItem(-1);

        // Check if the mouse event should go to the scrollbar
        if ((m_scroll.isMouseDown() && m_scroll.isMouseDownOnThumb()) || m_scroll.mouseOnWidget(pos - m_scroll.getPosition()))
        {
            m_scroll.mouseMoved(pos - m_scroll.getPosition());
        }
        else // Mouse not on scrollbar or dragging the scrollbar thumb
        {
            m_scroll.mouseNoLongerOnWidget();

            // Find out on which item the mouse is hovering
            Borders borders = getRenderer()->getBorders();
            Padding padding = getRenderer()->getPadding();
            if (sf::FloatRect{borders.left + padding.left, borders.top + padding.top, getInnerSize().x - padding.left - padding.right, getInnerSize().y - padding.top - padding.bottom}.contains(pos))
            {
                pos.y -= borders.top + padding.top;

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

                        if (m_selectedItem >= 0)
                        {
                            m_callback.text = m_items[m_selectedItem].getString();
                            m_callback.itemId = m_itemIds[m_selectedItem];
                            sendSignal("ItemSelected", m_items[m_selectedItem].getString(), m_items[m_selectedItem].getString(), m_itemIds[m_selectedItem]);
                        }
                        else
                        {
                            m_callback.text = "";
                            m_callback.itemId = "";
                            sendSignal("ItemSelected", "", "", "");
                        }
                    }
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseWheelMoved(int delta, int x, int y)
    {
        if (m_scroll.isShown())
        {
            m_scroll.mouseWheelMoved(delta, 0, 0);

            // Update on which item the mouse is hovering
            mouseMoved({static_cast<float>(x), static_cast<float>(y)});
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

    void ListBox::rendererChanged(const std::string& property, ObjectConverter& value)
    {
        if ((property == "borders") || (property == "padding"))
        {
            updateSize();
        }
        else if ((property == "textcolor") || (property == "textcolorhover") || (property == "selectedtextcolor") || (property == "selectedtextcolorhover"))
        {
            updateItemColorsAndStyle();
        }
        else if (property == "texturebackground")
        {
            value.getTexture().setSize(getInnerSize());
            value.getTexture().setOpacity(getRenderer()->getOpacity());
        }
        else if (property == "textstyle")
        {
            for (auto& item : m_items)
                item.setStyle(value.getTextStyle());

            if ((m_selectedItem >= 0) && (getRenderer()->getSelectedTextStyle().isSet()))
                m_items[m_selectedItem].setStyle(getRenderer()->getSelectedTextStyle());
        }
        else if (property == "selectedtextstyle")
        {
            if (m_selectedItem >= 0)
            {
                if (value.getTextStyle().isSet())
                    m_items[m_selectedItem].setStyle(value.getTextStyle());
                else
                    m_items[m_selectedItem].setStyle(getRenderer()->getTextStyle());
            }
        }
        else if (property == "scrollbar")
        {
            m_scroll.setRenderer(value.getRenderer());
        }
        else if (property == "opacity")
        {
            float opacity = value.getNumber();

            getRenderer()->getTextureBackground().setOpacity(opacity);
            m_scroll.getRenderer()->setOpacity(opacity);
            for (auto& item : m_items)
                item.setOpacity(opacity);
        }
        else if (property == "font")
        {
            std::shared_ptr<sf::Font> font = value.getFont();
            for (auto& item : m_items)
                item.setFont(font);

            // Recalculate the text size with the new font
            if (m_requestedTextSize == 0)
            {
                m_textSize = findBestTextSize(font, m_itemHeight * 0.8f);
                for (auto& item : m_items)
                    item.setCharacterSize(m_textSize);
            }

            updatePosition();
        }
        else if ((property != "bordercolor")
              && (property != "backgroundcolor")
              && (property != "backgroundcolorhover")
              && (property != "selectedbackgroundcolor")
              && (property != "selectedbackgroundcolorhover"))
        {
            Widget::rendererChanged(property, value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ListBox::getInnerSize() const
    {
        Borders borders = getRenderer()->getBorders();
        return {getSize().x - borders.left - borders.right, getSize().y - borders.top - borders.bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateSelectedAndHoveringItemColorsAndStyle()
    {
        if (m_selectedItem >= 0)
        {
            if ((m_selectedItem == m_hoveringItem) && getRenderer()->getSelectedTextColorHover().isSet())
                m_items[m_selectedItem].setColor(getRenderer()->getSelectedTextColorHover());
            else if (getRenderer()->getSelectedTextColor().isSet())
                m_items[m_selectedItem].setColor(getRenderer()->getSelectedTextColor());

            if (getRenderer()->getSelectedTextStyle().isSet())
                m_items[m_selectedItem].setStyle(getRenderer()->getSelectedTextStyle());
        }

        if ((m_hoveringItem >= 0) && (m_selectedItem != m_hoveringItem))
        {
            if (getRenderer()->getTextColorHover().isSet())
                m_items[m_hoveringItem].setColor(getRenderer()->getTextColorHover());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateItemColorsAndStyle()
    {
        for (auto& item : m_items)
        {
            item.setColor(getRenderer()->getTextColor());
            item.setStyle(getRenderer()->getTextStyle());
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
                if ((m_selectedItem == m_hoveringItem) && getRenderer()->getSelectedTextColor().isSet())
                    m_items[m_hoveringItem].setColor(getRenderer()->getSelectedTextColor());
                else
                    m_items[m_hoveringItem].setColor(getRenderer()->getTextColor());
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
                if ((m_selectedItem == m_hoveringItem) && getRenderer()->getTextColorHover().isSet())
                    m_items[m_selectedItem].setColor(getRenderer()->getTextColorHover());
                else
                    m_items[m_selectedItem].setColor(getRenderer()->getTextColor());

                m_items[m_selectedItem].setStyle(getRenderer()->getTextStyle());
            }

            m_selectedItem = item;

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
        sf::RenderStates statesForScrollbar = states;

        // Draw the borders
        Borders borders = getRenderer()->getBorders();
        if (borders != Borders{0})
        {
            drawBorders(target, states, borders, getSize(), getRenderer()->getBorderColor());
            states.transform.translate({borders.left, borders.top});
        }

        // Draw the background
        if (getRenderer()->getTextureBackground().isLoaded())
            getRenderer()->getTextureBackground().draw(target, states);
        else
            drawRectangleShape(target, states, getInnerSize(), getRenderer()->getBackgroundColor());

        // Draw the items and their selected/hover backgrounds
        {
            // Set the clipping for all draw calls that happen until this clipping object goes out of scope
            Padding padding = getRenderer()->getPadding();
            float maxItemWidth = getInnerSize().x - padding.left - padding.right;
            if (m_scroll.isShown())
                maxItemWidth -= m_scroll.getSize().x;
            Clipping clipping{target, states, {padding.left, padding.top}, {maxItemWidth, getInnerSize().y - padding.top - padding.bottom}};

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

            states.transform.translate({padding.left, padding.top - m_scroll.getValue()});

            // Draw the background of the selected item
            if (m_selectedItem >= 0)
            {
                states.transform.translate({0, static_cast<float>(m_selectedItem * m_itemHeight)});

                sf::Vector2f size = {getInnerSize().x - padding.left - padding.right, static_cast<float>(m_itemHeight)};
                if ((m_selectedItem == m_hoveringItem) && getRenderer()->getSelectedBackgroundColorHover().isSet())
                    drawRectangleShape(target, states, size, getRenderer()->getSelectedBackgroundColorHover());
                else
                    drawRectangleShape(target, states, size, getRenderer()->getSelectedBackgroundColor());

                states.transform.translate({0, -static_cast<float>(m_selectedItem * m_itemHeight)});
            }

            // Draw the background of the item on which the mouse is standing
            if ((m_hoveringItem >= 0) && (m_hoveringItem != m_selectedItem) && getRenderer()->getBackgroundColorHover().isSet())
            {
                states.transform.translate({0, static_cast<float>(m_hoveringItem * m_itemHeight)});
                drawRectangleShape(target, states, {getInnerSize().x - padding.left - padding.right, static_cast<float>(m_itemHeight)}, getRenderer()->getBackgroundColorHover());
                states.transform.translate({0, -static_cast<float>(m_hoveringItem * m_itemHeight)});
            }

            // Draw the items
            for (std::size_t i = firstItem; i < lastItem; ++i)
                m_items[i].draw(target, states);
        }

        // Draw the scrollbar
        target.draw(m_scroll, statesForScrollbar);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
