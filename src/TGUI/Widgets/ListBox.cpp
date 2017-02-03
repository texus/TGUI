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
        m_draggableWidget = true;

        addSignal<sf::String, TypeSet<sf::String, sf::String>>("ItemSelected");
        addSignal<sf::String, TypeSet<sf::String, sf::String>>("MousePressed");
        addSignal<sf::String, TypeSet<sf::String, sf::String>>("MouseReleased");
        addSignal<sf::String, TypeSet<sf::String, sf::String>>("DoubleClicked");

        m_renderer = std::make_shared<ListBoxRenderer>(this);
        reload();

        setSize({150, 154});
        setItemHeight(22);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::ListBox(const ListBox& listBoxToCopy) :
        Widget               {listBoxToCopy},
        m_items              (listBoxToCopy.m_items), // Did not compile in VS2013 when using braces
        m_itemIds            (listBoxToCopy.m_itemIds), // Did not compile in VS2013 when using braces
        m_selectedItem       {listBoxToCopy.m_selectedItem},
        m_hoveringItem       {listBoxToCopy.m_hoveringItem},
        m_itemHeight         {listBoxToCopy.m_itemHeight},
        m_requestedTextSize  {listBoxToCopy.m_requestedTextSize},
        m_textSize           {listBoxToCopy.m_textSize},
        m_maxItems           {listBoxToCopy.m_maxItems},
        m_scroll             {Scrollbar::copy(listBoxToCopy.m_scroll)},
        m_possibleDoubleClick{listBoxToCopy.m_possibleDoubleClick}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox& ListBox::operator= (const ListBox& right)
    {
        if (this != &right)
        {
            ListBox temp(right);
            Widget::operator=(right);

            std::swap(m_items,               temp.m_items);
            std::swap(m_itemIds,             temp.m_itemIds);
            std::swap(m_selectedItem,        temp.m_selectedItem);
            std::swap(m_hoveringItem,        temp.m_hoveringItem);
            std::swap(m_itemHeight,          temp.m_itemHeight);
            std::swap(m_requestedTextSize,   temp.m_requestedTextSize);
            std::swap(m_textSize,            temp.m_textSize);
            std::swap(m_maxItems,            temp.m_maxItems);
            std::swap(m_scroll,              temp.m_scroll);
            std::swap(m_possibleDoubleClick, temp.m_possibleDoubleClick);
        }

        return *this;
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

        getRenderer()->m_backgroundTexture.setPosition(getPosition());

        Padding padding = getRenderer()->getScaledPadding();

        if (m_font != nullptr)
        {
            for (std::size_t i = 0; i < m_items.size(); ++i)
            {
                m_items[i].setPosition({getPosition().x + padding.left,
                                        getPosition().y + padding.top + (i * m_itemHeight) + ((m_itemHeight - m_items[i].getSize().y) / 2.0f)});

                if ((m_scroll != nullptr) && (m_scroll->getLowValue() < m_scroll->getMaximum()))
                    m_items[i].setPosition({m_items[i].getPosition().x, m_items[i].getPosition().y - m_scroll->getValue()});
            }
        }

        if (m_scroll != nullptr)
            m_scroll->setPosition(getPosition().x + getSize().x - m_scroll->getSize().x - padding.right, getPosition().y + padding.top);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        getRenderer()->m_backgroundTexture.setSize(getSize());

        // If there is a scrollbar then reinitialize it
        if (m_scroll != nullptr)
        {
            Padding padding = getRenderer()->getScaledPadding();
            m_scroll->setSize({m_scroll->getSize().x, std::max(0.f, getSize().y - padding.top - padding.bottom)});
            m_scroll->setLowValue(static_cast<unsigned int>(std::max(0.f, getSize().y - padding.top - padding.bottom)));
        }

        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ListBox::getFullSize() const
    {
        return {getSize().x + getRenderer()->getBorders().left + getRenderer()->getBorders().right,
                getSize().y + getRenderer()->getBorders().top + getRenderer()->getBorders().bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setFont(const Font& font)
    {
        Widget::setFont(font);

        for (auto& item : m_items)
            item.setFont(font);

        // Recalculate the text size with the new font
        if (m_requestedTextSize == 0)
        {
            m_textSize = findBestTextSize(getFont(), m_itemHeight * 0.85f);
            for (auto& item : m_items)
                item.setTextSize(m_textSize);
        }

        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::addItem(const sf::String& itemName, const sf::String& id)
    {
        // Check if the item limit is reached (if there is one)
        if ((m_maxItems == 0) || (m_items.size() < m_maxItems))
        {
            // If there is no scrollbar then there is another limit
            if (m_scroll == nullptr)
            {
                // Calculate the amount of items that fit in the list box
                std::size_t maximumItems = static_cast<std::size_t>(getSize().y / m_itemHeight);

                // Check if the item still fits in the list box
                if (m_items.size() == maximumItems)
                    return false;
            }
            else // There is a scrollbar so tell it that another item was added
            {
                m_scroll->setMaximum(static_cast<unsigned int>((m_items.size() + 1) * m_itemHeight));

                // Scroll down when auto-scrolling is enabled
                if (m_autoScroll && (m_scroll->getLowValue() < m_scroll->getMaximum()))
                    m_scroll->setValue(m_scroll->getMaximum() - m_scroll->getLowValue());
            }

            // Create the new item
            Label newItem;
            newItem.setFont(getFont());
            newItem.setTextColor(getRenderer()->m_textColor);
            newItem.setTextSize(m_textSize);
            newItem.setText(itemName);

            // Add the new item to the list
            m_items.push_back(std::move(newItem));
            m_itemIds.push_back(id);

            updatePosition();
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
            if (m_items[i].getText() == itemName)
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

        if (m_selectedItem >= 0)
            m_items[m_selectedItem].setTextColor(getRenderer()->m_textColor);

        // Select the item
        m_selectedItem = static_cast<int>(index);
        m_items[m_selectedItem].setTextColor(getRenderer()->m_selectedTextColor);

        // Move the scrollbar if needed
        if (m_scroll)
        {
            if (m_selectedItem * getItemHeight() < m_scroll->getValue())
                m_scroll->setValue(m_selectedItem * getItemHeight());
            else if ((m_selectedItem + 1) * getItemHeight() > m_scroll->getValue() + m_scroll->getLowValue())
                m_scroll->setValue((m_selectedItem + 1) * getItemHeight() - m_scroll->getLowValue());

            updatePosition();
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::deselectItem()
    {
        if (m_selectedItem >= 0)
        {
            m_items[m_selectedItem].setTextColor(getRenderer()->m_textColor);
            m_selectedItem = -1;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItem(const sf::String& itemName)
    {
        for (std::size_t i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].getText() == itemName)
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

        // If there is a scrollbar then tell it that an item was removed
        if (m_scroll != nullptr)
        {
            m_scroll->setMaximum(static_cast<unsigned int>(m_items.size() * m_itemHeight));
            updatePosition();
        }

        // Check if the selected item should change
        if (m_selectedItem == static_cast<int>(index))
            m_selectedItem = -1;
        else if (m_selectedItem > static_cast<int>(index))
        {
            --m_selectedItem;
            m_items[m_selectedItem].setTextColor(getRenderer()->m_selectedTextColor);
        }

        // Check if the hovering item should change
        if (m_hoveringItem >= static_cast<int>(m_items.size()))
            m_hoveringItem = -1;

        if (m_hoveringItem >= 0)
            m_items[m_hoveringItem].setTextColor(getRenderer()->m_hoverTextColor);

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

        // If there is a scrollbar then tell it that all item were removed
        if (m_scroll != nullptr)
            m_scroll->setMaximum(0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ListBox::getItemById(const sf::String& id) const
    {
        for (std::size_t i = 0; i < m_itemIds.size(); ++i)
        {
            if (m_itemIds[i] == id)
                return m_items[i].getText();
        }

        return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ListBox::getSelectedItem() const
    {
        return (m_selectedItem >= 0) ? m_items[m_selectedItem].getText() : "";
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
            if (m_items[i].getText() == originalValue)
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

        m_items[index].setText(newValue);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String> ListBox::getItems()
    {
        std::vector<sf::String> items;
        for (auto& label : m_items)
            items.push_back(label.getText());

        return items;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<sf::String>& ListBox::getItemIds()
    {
        return m_itemIds;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setScrollbar(Scrollbar::Ptr scrollbar)
    {
        m_scroll = scrollbar;

        if (m_scroll)
        {
            Padding padding = getRenderer()->getScaledPadding();
            m_scroll->setSize({m_scroll->getSize().x, std::max(0.f, getSize().y - padding.top - padding.bottom)});
            m_scroll->setLowValue(static_cast<unsigned int>(std::max(0.f, getSize().y - padding.top - padding.bottom)));
            m_scroll->setMaximum(static_cast<unsigned int>(m_items.size() * m_itemHeight));
            m_scroll->setPosition(getPosition().x + getSize().x - m_scroll->getSize().x, getPosition().y);
        }
        else // The scrollbar was removed
        {
            // When the items no longer fit inside the list box then we need to remove some
            if ((m_items.size() * m_itemHeight) > static_cast<std::size_t>(getSize().y))
            {
                // Calculate ho many items fit inside the list box
                m_maxItems = static_cast<std::size_t>(getSize().y / m_itemHeight);

                // Remove the items that did not fit inside the list box
                m_items.erase(m_items.begin() + m_maxItems, m_items.end());
                m_itemIds.erase(m_itemIds.begin() + m_maxItems, m_itemIds.end());
            }
        }

        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Scrollbar::Ptr ListBox::getScrollbar() const
    {
        return m_scroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setItemHeight(unsigned int itemHeight)
    {
        // Set the new heights
        m_itemHeight = itemHeight;
        if (m_requestedTextSize == 0)
        {
            m_textSize = findBestTextSize(getFont(), itemHeight * 0.85f);
            for (auto& item : m_items)
                item.setTextSize(m_textSize);
        }

        // Some items might be removed when there is no scrollbar
        if (m_scroll == nullptr)
        {
            // When the items no longer fit inside the list box then we need to remove some
            if ((m_items.size() * m_itemHeight) > static_cast<std::size_t>(getSize().y))
            {
                // Calculate ho many items fit inside the list box
                m_maxItems = static_cast<std::size_t>(getSize().y / m_itemHeight);

                // Remove the items that did not fit inside the list box
                m_items.erase(m_items.begin() + m_maxItems, m_items.end());
                m_itemIds.erase(m_itemIds.begin() + m_maxItems, m_itemIds.end());
            }
        }
        else // There is a scrollbar
        {
            // Set the maximum of the scrollbar
            m_scroll->setMaximum(static_cast<unsigned int>(m_items.size() * m_itemHeight));
        }

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
            m_textSize = findBestTextSize(getFont(), m_itemHeight * 0.85f);

        for (auto& item : m_items)
            item.setTextSize(m_textSize);

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

            // If there is a scrollbar then tell it that the number of items was changed
            if (m_scroll != nullptr)
            {
                m_scroll->setMaximum(static_cast<unsigned int>(m_items.size() * m_itemHeight));
                updatePosition();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setAutoScroll(bool autoScroll)
    {
        m_autoScroll = autoScroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setOpacity(float opacity)
    {
        Widget::setOpacity(opacity);

        getRenderer()->m_backgroundTexture.setColor({getRenderer()->m_backgroundTexture.getColor().r, getRenderer()->m_backgroundTexture.getColor().g, getRenderer()->m_backgroundTexture.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});

        if (m_scroll != nullptr)
            m_scroll->setOpacity(m_opacity);

        updateItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ListBox::getWidgetOffset() const
    {
        return {getRenderer()->getBorders().left, getRenderer()->getBorders().top};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::mouseOnWidget(float x, float y) const
    {
        return sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::leftMousePressed(float x, float y)
    {
        // If there is a scrollbar then pass the event
        bool mouseOnScrollbar = false;
        if (m_scroll != nullptr)
        {
            if (m_scroll->mouseOnWidget(x, y))
            {
                m_scroll->leftMousePressed(x, y);
                mouseOnScrollbar = true;
            }
        }

        // If the click occurred on the list box
        Padding padding = getRenderer()->getScaledPadding();
        if (!mouseOnScrollbar && (sf::FloatRect{getPosition().x + padding.left, getPosition().y + padding.top, getSize().x - padding.left - padding.right, getSize().y - padding.top - padding.bottom}.contains(x, y)))
        {
            m_mouseDown = true;

            if (m_hoveringItem >= 0)
            {
                m_callback.text = m_items[m_hoveringItem].getText();
                m_callback.itemId = m_itemIds[m_hoveringItem];
                sendSignal("MousePressed", m_items[m_hoveringItem].getText(), m_items[m_hoveringItem].getText(), m_itemIds[m_hoveringItem]);
            }

            if (m_selectedItem != m_hoveringItem)
            {
                m_possibleDoubleClick = false;

                if (m_selectedItem >= 0)
                    m_items[m_selectedItem].setTextColor(getRenderer()->m_textColor);

                m_selectedItem = m_hoveringItem;

                if (m_selectedItem >= 0)
                {
                    m_items[m_selectedItem].setTextColor(getRenderer()->m_selectedTextColor);

                    m_callback.text  = m_items[m_selectedItem].getText();
                    m_callback.itemId = m_itemIds[m_selectedItem];
                    sendSignal("ItemSelected", m_items[m_selectedItem].getText(), m_items[m_selectedItem].getText(), m_itemIds[m_selectedItem]);
                }
                else
                {
                    m_callback.text  = "";
                    m_callback.itemId = "";
                    sendSignal("ItemSelected", "", "", "");
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::leftMouseReleased(float x, float y)
    {
        // If there is a scrollbar then pass it the event
        if (m_scroll != nullptr)
        {
            // Remember the old scrollbar value
            unsigned int oldValue = m_scroll->getValue();

            // Pass the event
            m_scroll->leftMouseReleased(x, y);

            // Check if the scrollbar value was incremented (you have pressed on the down arrow)
            if (m_scroll->getValue() == oldValue + 1)
            {
                // Decrement the value
                m_scroll->setValue(m_scroll->getValue()-1);

                // Scroll down with the whole item height instead of with a single pixel
                m_scroll->setValue(m_scroll->getValue() + m_itemHeight - (m_scroll->getValue() % m_itemHeight));
            }
            else if (m_scroll->getValue() == oldValue - 1) // Check if the scrollbar value was decremented (you have pressed on the up arrow)
            {
                // increment the value
                m_scroll->setValue(m_scroll->getValue()+1);

                // Scroll up with the whole item height instead of with a single pixel
                if (m_scroll->getValue() % m_itemHeight > 0)
                    m_scroll->setValue(m_scroll->getValue() - (m_scroll->getValue() % m_itemHeight));
                else
                    m_scroll->setValue(m_scroll->getValue() - m_itemHeight);
            }

            updatePosition();
        }

        if (m_mouseDown)
        {
            if (m_selectedItem >= 0)
            {
                m_callback.text  = m_items[m_selectedItem].getText();
                m_callback.itemId = m_itemIds[m_selectedItem];
                sendSignal("MouseReleased", m_items[m_selectedItem].getText(), m_items[m_selectedItem].getText(), m_itemIds[m_selectedItem]);
            }

            // Check if you double-clicked
            if (m_possibleDoubleClick)
            {
                m_possibleDoubleClick = false;

                if (m_selectedItem >= 0)
                    sendSignal("DoubleClicked", m_items[m_selectedItem].getText(), m_items[m_selectedItem].getText(), m_itemIds[m_selectedItem]);
            }
            else // This is the first click
            {
                m_animationTimeElapsed = {};
                m_possibleDoubleClick = true;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseMoved(float x, float y)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

        // If there is a scrollbar then pass the event
        bool mouseOnScrollbar = false;
        if (m_scroll != nullptr)
        {
            // Check if you are dragging the thumb of the scrollbar
            if ((m_scroll->m_mouseDown) && (m_scroll->m_mouseDownOnThumb))
            {
                // Pass the event, even when the mouse is not on top of the scrollbar
                m_scroll->mouseMoved(x, y);

                updatePosition();
                mouseOnScrollbar = true;
            }
            else // You are just moving the mouse
            {
                // When the mouse is on top of the scrollbar then pass the mouse move event
                if (m_scroll->mouseOnWidget(x, y))
                {
                    m_scroll->mouseMoved(x, y);

                    // The mouse is no longer on top of an item
                    if ((m_hoveringItem >= 0) && (m_selectedItem != m_hoveringItem))
                    {
                        m_items[m_hoveringItem].setTextColor(getRenderer()->m_textColor);
                        m_hoveringItem = -1;
                    }

                    updatePosition();
                    mouseOnScrollbar = true;
                }
            }
        }

        // Find out on which item the mouse is hovering
        Padding padding = getRenderer()->getScaledPadding();
        if (!mouseOnScrollbar && (sf::FloatRect{getPosition().x + padding.left, getPosition().y + padding.top, getSize().x - padding.left - padding.right, getSize().y - padding.top - padding.bottom}.contains(x, y)))
        {
            y -= padding.top;

            if ((m_hoveringItem >= 0) && (m_selectedItem != m_hoveringItem))
                m_items[m_hoveringItem].setTextColor(getRenderer()->m_textColor);

            // Check if there is a scrollbar or whether it is hidden
            if ((m_scroll != nullptr) && (m_scroll->getLowValue() < m_scroll->getMaximum()))
            {
                // Check if the mouse is on the first (perhaps partially) visible item
                if (y - getPosition().y <= (m_itemHeight - (m_scroll->getValue() % m_itemHeight)))
                {
                    m_hoveringItem = static_cast<int>(m_scroll->getValue() / m_itemHeight);
                }
                else // The mouse is not on the first visible item
                {
                    // Calculate on what item the mouse is standing
                    if ((m_scroll->getValue() % m_itemHeight) == 0)
                        m_hoveringItem = static_cast<int>((y - getPosition().y) / m_itemHeight + (m_scroll->getValue() / m_itemHeight));
                    else
                        m_hoveringItem = static_cast<int>((((y - getPosition().y) - (m_itemHeight - (m_scroll->getValue() % m_itemHeight))) / m_itemHeight) + (m_scroll->getValue() / m_itemHeight) + 1);
                }
            }
            else // There is no scrollbar or it is not displayed
            {
                // Calculate on which item the mouse is standing
                m_hoveringItem = static_cast<int>((y - getPosition().y) / m_itemHeight);

                // Check if the mouse is behind the last item
                if (m_hoveringItem > static_cast<int>(m_items.size())-1)
                    m_hoveringItem = -1;
            }

            // If the mouse is held down then select the item below the mouse
            if (m_mouseDown)
            {
                if (m_selectedItem != m_hoveringItem)
                {
                    m_possibleDoubleClick = false;

                    if (m_selectedItem >= 0)
                        m_items[m_selectedItem].setTextColor(getRenderer()->m_textColor);

                    m_selectedItem = m_hoveringItem;

                    if (m_selectedItem >= 0)
                    {
                        m_items[m_selectedItem].setTextColor(getRenderer()->m_selectedTextColor);

                        m_callback.text = m_items[m_selectedItem].getText();
                        m_callback.itemId = m_itemIds[m_selectedItem];
                        sendSignal("ItemSelected", m_items[m_selectedItem].getText(), m_items[m_selectedItem].getText(), m_itemIds[m_selectedItem]);
                    }
                    else
                    {
                        m_callback.text = "";
                        m_callback.itemId = "";
                        sendSignal("ItemSelected", "", "", "");
                    }
                }
            }
            else // The mouse isn't held down, just change the text color to hover
            {
                if ((m_hoveringItem >= 0) && (m_selectedItem != m_hoveringItem))
                    m_items[m_hoveringItem].setTextColor(getRenderer()->m_hoverTextColor);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseWheelMoved(int delta, int x, int y)
    {
        // Only do something when there is a scrollbar
        if (m_scroll != nullptr)
        {
            if (m_scroll->getLowValue() < m_scroll->getMaximum())
            {
                // Check if you are scrolling down
                if (delta < 0)
                {
                    // Scroll down
                    m_scroll->setValue(m_scroll->getValue() + (static_cast<unsigned int>(-delta) * m_itemHeight));
                }
                else // You are scrolling up
                {
                    unsigned int change = static_cast<unsigned int>(delta) * m_itemHeight;

                    // Scroll up
                    if (change < m_scroll->getValue())
                        m_scroll->setValue(m_scroll->getValue() - change);
                    else
                        m_scroll->setValue(0);
                }

                updatePosition();
                mouseMoved(static_cast<float>(x), static_cast<float>(y));
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseNoLongerOnWidget()
    {
        if (m_mouseHover)
            mouseLeftWidget();

        if (m_scroll != nullptr)
            m_scroll->m_mouseHover = false;

        if ((m_hoveringItem >= 0) && (m_selectedItem != m_hoveringItem))
        {
            m_items[m_hoveringItem].setTextColor(getRenderer()->m_textColor);
            m_hoveringItem = -1;
        }

        m_possibleDoubleClick = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseNoLongerDown()
    {
        Widget::mouseNoLongerDown();
        if (m_scroll != nullptr)
            m_scroll->mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateItemColors()
    {
        for (auto& item : m_items)
            item.setTextColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));

        if ((m_hoveringItem >= 0) && (m_selectedItem != m_hoveringItem))
            m_items[m_hoveringItem].setTextColor(calcColorOpacity(getRenderer()->m_hoverTextColor, getOpacity()));

        if (m_selectedItem >= 0)
            m_items[m_selectedItem].setTextColor(calcColorOpacity(getRenderer()->m_selectedTextColor, getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::reload(const std::string& primary, const std::string& secondary, bool force)
    {
        getRenderer()->setBorders({2, 2, 2, 2});
        getRenderer()->setBackgroundColor({245, 245, 245});
        getRenderer()->setTextColorNormal({60, 60, 60});
        getRenderer()->setTextColorHover({0, 0, 0});
        getRenderer()->setHoverBackgroundColor({255, 255, 255});
        getRenderer()->setSelectedBackgroundColor({0, 110, 255});
        getRenderer()->setSelectedTextColor({255, 255, 255});
        getRenderer()->setBorderColor({0, 0, 0});
        getRenderer()->setBackgroundTexture({});

        if (m_theme && primary != "")
        {
            getRenderer()->setBorders({0, 0, 0, 0});
            getRenderer()->setHoverBackgroundColor(sf::Color::Transparent);

            Widget::reload(primary, secondary, force);

            if (force)
            {
                if (getRenderer()->m_backgroundTexture.isLoaded())
                    setSize(getRenderer()->m_backgroundTexture.getImageSize());
            }
            else
                updateSize();
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
        // Draw the background
        getRenderer()->draw(target, states);

        {
            // Set the clipping for all draw calls that happen until this clipping object goes out of scope
            Padding padding = getRenderer()->getScaledPadding();
            Clipping clipping{target, states, {getPosition().x + padding.left, getPosition().y + padding.top}, {getSize().x - padding.left - padding.right, getSize().y - padding.top - padding.bottom}};

            // Find out which items are visible
            std::size_t firstItem = 0;
            std::size_t lastItem = m_items.size();
            if ((m_scroll != nullptr) && (m_scroll->getLowValue() < m_scroll->getMaximum()))
            {
                firstItem = m_scroll->getValue() / m_itemHeight;
                lastItem = (m_scroll->getValue() + m_scroll->getLowValue()) / m_itemHeight;

                // Show another item when the scrollbar is standing between two items
                if ((m_scroll->getValue() + m_scroll->getLowValue()) % m_itemHeight != 0)
                    ++lastItem;
            }

            // Draw the background of the selected item
            if (m_selectedItem >= 0)
            {
                sf::RectangleShape back({getSize().x - padding.left - padding.right, static_cast<float>(m_itemHeight)});
                back.setFillColor(calcColorOpacity(getRenderer()->m_selectedBackgroundColor, getOpacity()));
                back.setPosition({getPosition().x + padding.left, getPosition().y + padding.top + (m_selectedItem * m_itemHeight)});

                if ((m_scroll != nullptr) && (m_scroll->getLowValue() < m_scroll->getMaximum()))
                    back.setPosition({back.getPosition().x, back.getPosition().y - m_scroll->getValue()});

                target.draw(back, states);
            }

            // Draw the background of the item on which the mouse is standing
            if ((m_hoveringItem >= 0) && (m_hoveringItem != m_selectedItem) && (getRenderer()->m_hoverBackgroundColor != sf::Color::Transparent))
            {
                sf::RectangleShape back({getSize().x - padding.left - padding.right, static_cast<float>(m_itemHeight)});
                back.setFillColor(calcColorOpacity(getRenderer()->m_hoverBackgroundColor, getOpacity()));
                back.setPosition({getPosition().x + padding.left, getPosition().y + padding.top + (m_hoveringItem * m_itemHeight)});

                if ((m_scroll != nullptr) && (m_scroll->getLowValue() < m_scroll->getMaximum()))
                    back.setPosition({back.getPosition().x, back.getPosition().y - m_scroll->getValue()});

                target.draw(back, states);
            }

            // Draw the items
            for (std::size_t i = firstItem; i < lastItem; ++i)
                target.draw(m_items[i], states);
        }

        // Draw the scrollbar
        if (m_scroll != nullptr)
            target.draw(*m_scroll, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "borders")
            setBorders(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == "padding")
            setPadding(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == "backgroundcolor")
            setBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "textcolor")
            setTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "textcolornormal")
            setTextColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "textcolorhover")
            setTextColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "hoverbackgroundcolor")
            setHoverBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "selectedbackgroundcolor")
            setSelectedBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "selectedtextcolor")
            setSelectedTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "bordercolor")
            setBorderColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "backgroundimage")
            setBackgroundTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "scrollbar")
        {
            if (toLower(value) == "none")
                m_listBox->setScrollbar(nullptr);
            else
            {
                if (m_listBox->getTheme() == nullptr)
                    throw Exception{"Failed to load scrollbar, ListBox has no connected theme to load the scrollbar with"};

                m_listBox->setScrollbar(m_listBox->getTheme()->internalLoad(m_listBox->m_primaryLoadingParameter,
                                                                            Deserializer::deserialize(ObjectConverter::Type::String, value).getString()));
            }
        }
        else
            WidgetRenderer::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setProperty(std::string property, ObjectConverter&& value)
    {
        property = toLower(property);

        if (value.getType() == ObjectConverter::Type::Borders)
        {
            if (property == "borders")
                setBorders(value.getBorders());
            else if (property == "padding")
                setPadding(value.getBorders());
            else
                return WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Color)
        {
            if (property == "backgroundcolor")
                setBackgroundColor(value.getColor());
            else if (property == "textcolor")
                setTextColor(value.getColor());
            else if (property == "textcolornormal")
                setTextColorNormal(value.getColor());
            else if (property == "textcolorhover")
                setTextColorHover(value.getColor());
            else if (property == "hoverbackgroundcolor")
                setHoverBackgroundColor(value.getColor());
            else if (property == "selectedbackgroundcolor")
                setSelectedBackgroundColor(value.getColor());
            else if (property == "selectedtextcolor")
                setSelectedTextColor(value.getColor());
            else if (property == "bordercolor")
                setBorderColor(value.getColor());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Texture)
        {
            if (property == "backgroundimage")
                setBackgroundTexture(value.getTexture());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::String)
        {
            if (property == "scrollbar")
            {
                if (toLower(value.getString()) == "none")
                    m_listBox->setScrollbar(nullptr);
                else
                {
                    if (m_listBox->getTheme() == nullptr)
                        throw Exception{"Failed to load scrollbar, ListBox has no connected theme to load the scrollbar with"};

                    m_listBox->setScrollbar(m_listBox->getTheme()->internalLoad(m_listBox->getPrimaryLoadingParameter(), value.getString()));
                }
            }
        }
        else
            WidgetRenderer::setProperty(property, std::move(value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter ListBoxRenderer::getProperty(std::string property) const
    {
        property = toLower(property);

        if (property == "borders")
            return m_borders;
        else if (property == "padding")
            return m_padding;
        else if (property == "backgroundcolor")
            return m_backgroundColor;
        else if (property == "textcolor")
            return m_textColor;
        else if (property == "textcolornormal")
            return m_textColor;
        else if (property == "textcolorhover")
            return m_hoverTextColor;
        else if (property == "hoverbackgroundcolor")
            return m_hoverBackgroundColor;
        else if (property == "selectedbackgroundcolor")
            return m_selectedBackgroundColor;
        else if (property == "selectedtextcolor")
            return m_selectedTextColor;
        else if (property == "bordercolor")
            return m_borderColor;
        else if (property == "backgroundimage")
            return m_backgroundTexture;
        else
            return WidgetRenderer::getProperty(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, ObjectConverter> ListBoxRenderer::getPropertyValuePairs() const
    {
        auto pairs = WidgetRenderer::getPropertyValuePairs();

        if (m_backgroundTexture.isLoaded())
            pairs["BackgroundImage"] = m_backgroundTexture;
        else
            pairs["BackgroundColor"] = m_backgroundColor;

        pairs["TextColorNormal"] = m_textColor;
        pairs["TextColorHover"] = m_hoverTextColor;
        pairs["HoverBackgroundColor"] = m_hoverBackgroundColor;
        pairs["SelectedBackgroundColor"] = m_selectedBackgroundColor;
        pairs["SelectedTextColor"] = m_selectedTextColor;
        pairs["BorderColor"] = m_borderColor;
        pairs["Borders"] = m_borders;
        pairs["Padding"] = m_padding;
        return pairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setBackgroundColor(const Color& backgroundColor)
    {
        m_backgroundColor = backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setTextColor(const Color& color)
    {
        setTextColorNormal(color);
        setTextColorHover(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setTextColorNormal(const Color& color)
    {
        m_textColor = color;
        m_listBox->updateItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setTextColorHover(const Color& color)
    {
        m_hoverTextColor = color;
        m_listBox->updateItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setHoverBackgroundColor(const Color& hoverBackgroundColor)
    {
        m_hoverBackgroundColor = hoverBackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setSelectedBackgroundColor(const Color& selectedBackgroundColor)
    {
        m_selectedBackgroundColor = selectedBackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setSelectedTextColor(const Color& selectedTextColor)
    {
        m_selectedTextColor = selectedTextColor;
        m_listBox->updateItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setBorderColor(const Color& borderColor)
    {
        m_borderColor = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setBackgroundTexture(const Texture& texture)
    {
        m_backgroundTexture = texture;
        if (m_backgroundTexture.isLoaded())
        {
            m_backgroundTexture.setPosition(m_listBox->getPosition());
            m_backgroundTexture.setSize(m_listBox->getSize());
            m_backgroundTexture.setColor({m_backgroundTexture.getColor().r, m_backgroundTexture.getColor().g, m_backgroundTexture.getColor().b, static_cast<sf::Uint8>(m_listBox->getOpacity() * 255)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setPadding(const Padding& padding)
    {
        WidgetPadding::setPadding(padding);

        m_listBox->updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Draw the background
        if (m_backgroundTexture.isLoaded())
            target.draw(m_backgroundTexture, states);
        else
        {
            sf::RectangleShape background(m_listBox->getSize());
            background.setPosition(m_listBox->getPosition());
            background.setFillColor(calcColorOpacity(m_backgroundColor, m_listBox->getOpacity()));
            target.draw(background, states);
        }

        // Draw the borders
        if (m_borders != Borders{0, 0, 0, 0})
        {
            sf::Vector2f size = m_listBox->getSize();
            sf::Vector2f position = m_listBox->getPosition();

            // Draw left border
            sf::RectangleShape border({m_borders.left, size.y + m_borders.top});
            border.setPosition(position.x - m_borders.left, position.y - m_borders.top);
            border.setFillColor(calcColorOpacity(m_borderColor, m_listBox->getOpacity()));
            target.draw(border, states);

            // Draw top border
            border.setSize({size.x + m_borders.right, m_borders.top});
            border.setPosition(position.x, position.y - m_borders.top);
            target.draw(border, states);

            // Draw right border
            border.setSize({m_borders.right, size.y + m_borders.bottom});
            border.setPosition(position.x + size.x, position.y);
            target.draw(border, states);

            // Draw bottom border
            border.setSize({size.x + m_borders.left, m_borders.bottom});
            border.setPosition(position.x - m_borders.left, position.y + size.y);
            target.draw(border, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Padding ListBoxRenderer::getScaledPadding() const
    {
        Padding padding = getPadding();
        Padding scaledPadding = padding;

        auto& texture = m_backgroundTexture;
        if (texture.isLoaded())
        {
            switch (texture.getScalingType())
            {
            case Texture::ScalingType::Normal:
                if ((texture.getImageSize().x != 0) && (texture.getImageSize().y != 0))
                {
                    scaledPadding.left = padding.left * (texture.getSize().x / texture.getImageSize().x);
                    scaledPadding.right = padding.right * (texture.getSize().x / texture.getImageSize().x);
                    scaledPadding.top = padding.top * (texture.getSize().y / texture.getImageSize().y);
                    scaledPadding.bottom = padding.bottom * (texture.getSize().y / texture.getImageSize().y);
                }
                break;

            case Texture::ScalingType::Horizontal:
                if ((texture.getImageSize().x != 0) && (texture.getImageSize().y != 0))
                {
                    scaledPadding.left = padding.left * (texture.getSize().y / texture.getImageSize().y);
                    scaledPadding.right = padding.right * (texture.getSize().y / texture.getImageSize().y);
                    scaledPadding.top = padding.top * (texture.getSize().y / texture.getImageSize().y);
                    scaledPadding.bottom = padding.bottom * (texture.getSize().y / texture.getImageSize().y);
                }
                break;

            case Texture::ScalingType::Vertical:
                if ((texture.getImageSize().x != 0) && (texture.getImageSize().y != 0))
                {
                    scaledPadding.left = padding.left * (texture.getSize().x / texture.getImageSize().x);
                    scaledPadding.right = padding.right * (texture.getSize().x / texture.getImageSize().x);
                    scaledPadding.top = padding.top * (texture.getSize().x / texture.getImageSize().x);
                    scaledPadding.bottom = padding.bottom * (texture.getSize().x / texture.getImageSize().x);
                }
                break;

            case Texture::ScalingType::NineSlice:
                break;
            }
        }

        return scaledPadding;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> ListBoxRenderer::clone(Widget* widget)
    {
        auto renderer = std::make_shared<ListBoxRenderer>(*this);
        renderer->m_listBox = static_cast<ListBox*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
