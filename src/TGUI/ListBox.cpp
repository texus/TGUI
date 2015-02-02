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


#include <SFML/OpenGL.hpp>

#include <TGUI/Scrollbar.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/ListBox.hpp>
#include <TGUI/Label.hpp>

#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::ListBox()
    {
        m_callback.widgetType = WidgetType::ListBox;
        m_draggableWidget = true;
        m_animatedWidget = true;

        addSignal<sf::String, TypeSet<sf::String, sf::String>>("ItemSelected");
        addSignal<sf::String, TypeSet<sf::String, sf::String>>("MousePressed");
        addSignal<sf::String, TypeSet<sf::String, sf::String>>("MouseReleased");
        addSignal<sf::String, TypeSet<sf::String, sf::String>>("DoubleClicked");

        m_renderer = std::make_shared<ListBoxRenderer>(this);

        getRenderer()->setBorders({2, 2, 2, 2});

        setSize({150, 150});
        setItemHeight(20);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::ListBox(const ListBox& listBoxToCopy) :
        Widget               {listBoxToCopy},
        m_items              (listBoxToCopy.m_items), // Did not compile in VS2013 when using braces
        m_itemIds            (listBoxToCopy.m_itemIds), // Did not compile in VS2013 when using braces
        m_selectedItem       {listBoxToCopy.m_selectedItem},
        m_hoveringItem       {listBoxToCopy.m_hoveringItem},
        m_itemHeight         {listBoxToCopy.m_itemHeight},
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
            std::swap(m_textSize,            temp.m_textSize);
            std::swap(m_maxItems,            temp.m_maxItems);
            std::swap(m_scroll,              temp.m_scroll);
            std::swap(m_possibleDoubleClick, temp.m_possibleDoubleClick);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::Ptr ListBox::create(const std::string& themeFileFilename, const std::string& section)
    {
        auto listBox = std::make_shared<ListBox>();

        if (themeFileFilename != "")
        {
            listBox->getRenderer()->setBorders({0, 0, 0, 0});
            listBox->getRenderer()->setHoverBackgroundColor(sf::Color::Transparent);

            std::string loadedThemeFile = getResourcePath() + themeFileFilename;

            // Open the theme file
            ThemeFileParser themeFile{loadedThemeFile, section};

            // Find the folder that contains the theme file
            std::string themeFileFolder = "";
            std::string::size_type slashPos = loadedThemeFile.find_last_of("/\\");
            if (slashPos != std::string::npos)
                themeFileFolder = loadedThemeFile.substr(0, slashPos+1);

            // Handle the read properties
            for (auto it = themeFile.getProperties().cbegin(); it != themeFile.getProperties().cend(); ++it)
            {
                try
                {
                    if (it->first == "scrollbar")
                    {
                        if (toLower(it->second) != "none")
                        {
                            if ((it->second.length() < 3) || (it->second[0] != '"') || (it->second[it->second.length()-1] != '"'))
                                throw Exception{"Failed to parse value for 'Scrollbar' property."};

                            listBox->getRenderer()->setScrollbar(themeFileFilename, it->second.substr(1, it->second.length()-2));
                        }
                        else // There should be no scrollbar
                            listBox->removeScrollbar();
                    }
                    else
                        listBox->getRenderer()->setProperty(it->first, it->second, themeFileFolder);
                }
                catch (const Exception& e)
                {
                    throw Exception{std::string(e.what()) + " In section '" + section + "' in " + loadedThemeFile + "."};
                }
            }

            listBox->updateSize();
        }

        return listBox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::Ptr ListBox::copy(ListBox::ConstPtr listBox)
    {
        if (listBox)
            return std::make_shared<ListBox>(*listBox);
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setPosition(const Layout& position)
    {
        Widget::setPosition(position);

        getRenderer()->m_backgroundTexture.setPosition(getPosition());

        Padding padding = getRenderer()->getScaledPadding();

        if (m_font != nullptr)
        {
            float textHeight = sf::Text{"kg", *m_font, m_textSize}.getLocalBounds().height;
            for (unsigned int i = 0; i < m_items.size(); ++i)
            {
                m_items[i].setPosition({getPosition().x + (textHeight / 10.0f) + padding.left,
                                        getPosition().y + static_cast<float>(i * m_itemHeight) + ((m_itemHeight - textHeight) / 2.0f) + padding.top});

                if ((m_scroll != nullptr) && (m_scroll->getLowValue() < m_scroll->getMaximum()))
                    m_items[i].setPosition({m_items[i].getPosition().x, m_items[i].getPosition().y - m_scroll->getValue()});
            }
        }

        if (m_scroll != nullptr)
            m_scroll->setPosition(getPosition().x + getSize().x - m_scroll->getSize().x - padding.right, getPosition().y + padding.top);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setSize(const Layout& size)
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
       return sf::Vector2f(getSize().x + getRenderer()->getBorders().left + getRenderer()->getBorders().right,
                getSize().y + getRenderer()->getBorders().top + getRenderer()->getBorders().bottom);
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
                unsigned int maximumItems = static_cast<unsigned int>(getSize().y) / m_itemHeight;

                // Check if the item still fits in the list box
                if (m_items.size() == maximumItems)
                    return false;
            }
            else // There is a scrollbar so tell it that another item was added
            {
                m_scroll->setMaximum((m_items.size() + 1) * m_itemHeight);
            }

            // Create the new item
            Label newItem;
            newItem.setTextFont(m_font);
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
        for (unsigned int i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].getText() == itemName)
                return setSelectedItemByIndex(i);
        }

        // No match was found
        if (m_selectedItem >= 0)
            m_items[m_selectedItem].setTextColor(getRenderer()->m_textColor);

        m_selectedItem = -1;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::setSelectedItemById(const sf::String& id)
    {
        for (unsigned int i = 0; i < m_itemIds.size(); ++i)
        {
            if (m_itemIds[i] == id)
                return setSelectedItemByIndex(i);
        }

        // No match was found
        if (m_selectedItem >= 0)
            m_items[m_selectedItem].setTextColor(getRenderer()->m_textColor);

        m_selectedItem = -1;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::setSelectedItemByIndex(unsigned int index)
    {
        if (index >= m_items.size())
            return false;

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
        for (unsigned int i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].getText() == itemName)
                return removeItemByIndex(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItemById(const sf::String& id)
    {
        for (unsigned int i = 0; i < m_itemIds.size(); ++i)
        {
            if (m_itemIds[i] == id)
                return removeItemByIndex(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItemByIndex(unsigned int index)
    {
        if (index >= m_items.size())
            return false;

        // Remove the item
        m_items.erase(m_items.begin() + index);
        m_itemIds.erase(m_itemIds.begin() + index);

        // If there is a scrollbar then tell it that an item was removed
        if (m_scroll != nullptr)
        {
            m_scroll->setMaximum(m_items.size() * m_itemHeight);
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

        // If there is a scrollbar then tell it that all item were removed
        if (m_scroll != nullptr)
            m_scroll->setMaximum(0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ListBox::getItemById(const sf::String& id) const
    {
        for (unsigned int i = 0; i < m_itemIds.size(); ++i)
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

    bool ListBox::changeItem(const sf::String& originalValue, const sf::String& newValue)
    {
        for (auto& item : m_items)
        {
            if (item.getText() == originalValue)
            {
                item.setText(newValue);
                return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::changeItemById(const sf::String& id, const sf::String& newValue)
    {
        for (unsigned int i = 0; i < m_items.size(); ++i)
        {
            if (m_itemIds[i] == id)
            {
                m_items[i].setText(newValue);
                return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::removeScrollbar()
    {
        m_scroll = nullptr;

        // When the items no longer fit inside the list box then we need to remove some
        if ((m_items.size() * m_itemHeight) > static_cast<unsigned int>(getSize().y))
        {
            // Calculate ho many items fit inside the list box
            m_maxItems = static_cast<unsigned int>(getSize().y) / m_itemHeight;

            // Remove the items that did not fit inside the list box
            m_items.erase(m_items.begin() + m_maxItems, m_items.end());
            m_itemIds.erase(m_itemIds.begin() + m_maxItems, m_itemIds.end());
        }

        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setItemHeight(unsigned int itemHeight)
    {
        // There is a minimum height
        if (itemHeight < 10)
            itemHeight = 10;

        // Set the new heights
        m_itemHeight = itemHeight;
        m_textSize   = static_cast<unsigned int>(itemHeight * 0.8f);

        for (auto& item : m_items)
            item.setTextSize(m_textSize);

        // Some items might be removed when there is no scrollbar
        if (m_scroll == nullptr)
        {
            // When the items no longer fit inside the list box then we need to remove some
            if ((m_items.size() * m_itemHeight) > static_cast<unsigned int>(getSize().y))
            {
                // Calculate ho many items fit inside the list box
                m_maxItems = static_cast<unsigned int>(getSize().y) / m_itemHeight;

                // Remove the items that did not fit inside the list box
                m_items.erase(m_items.begin() + m_maxItems, m_items.end());
                m_itemIds.erase(m_itemIds.begin() + m_maxItems, m_itemIds.end());
            }
        }
        else // There is a scrollbar
        {
            // Set the maximum of the scrollbar
            m_scroll->setMaximum(m_items.size() * m_itemHeight);
        }

        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setMaximumItems(unsigned int maximumItems)
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
                m_scroll->setMaximum(m_items.size() * m_itemHeight);
                updatePosition();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        if (m_scroll != nullptr)
            m_scroll->setTransparency(m_opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::mouseOnWidget(float x, float y)
    {
        // Pass the event to the scrollbar (if there is one)
        if (m_scroll != nullptr)
            m_scroll->mouseOnWidget(x, y);

        // Check if the mouse is on top of the list box
        if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(x, y))
            return true;
        else // The mouse is not on top of the list box
        {
            mouseNotOnWidget();
            return false;
        }
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
                sendSignal("MousePressed", m_items[m_hoveringItem].getText(), m_itemIds[m_hoveringItem]);
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
                    sendSignal("ItemSelected", m_items[m_selectedItem].getText(), m_itemIds[m_selectedItem]);
                }
                else
                {
                    m_callback.text  = "";
                    m_callback.itemId = "";
                    sendSignal("ItemSelected", "", "");
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
            m_mouseDown = false;

            if (m_selectedItem >= 0)
            {
                m_callback.text  = m_items[m_selectedItem].getText();
                m_callback.itemId = m_itemIds[m_selectedItem];
                sendSignal("MouseReleased", m_items[m_selectedItem].getText(), m_itemIds[m_selectedItem]);
            }

            // Check if you double-clicked
            if (m_possibleDoubleClick)
            {
                m_possibleDoubleClick = false;

                if (m_selectedItem >= 0)
                    sendSignal("DoubleClicked", m_items[m_selectedItem].getText(), m_itemIds[m_selectedItem]);
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
                        sendSignal("ItemSelected", m_items[m_selectedItem].getText(), m_itemIds[m_selectedItem]);
                    }
                    else
                    {
                        m_callback.text = "";
                        m_callback.itemId = "";
                        sendSignal("ItemSelected", "", "");
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
                    m_scroll->setValue(m_scroll->getValue() + (static_cast<unsigned int>(-delta) * (m_itemHeight / 2)));
                }
                else // You are scrolling up
                {
                    unsigned int change = static_cast<unsigned int>(delta) * (m_itemHeight / 2);

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

    void ListBox::mouseNotOnWidget()
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
        if (m_scroll != nullptr)
            m_scroll->m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::updateItemColors()
    {
        for (auto& item : m_items)
            item.setTextColor(getRenderer()->m_textColor);

        if (m_selectedItem >= 0)
            m_items[m_selectedItem].setTextColor(getRenderer()->m_selectedTextColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!m_font && m_parent->getGlobalFont())
            getRenderer()->setTextFont(m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::update()
    {
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

        const sf::View& view = target.getView();

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / view.getSize().x;
        float scaleViewY = target.getSize().y / view.getSize().y;

        Padding padding = getRenderer()->getScaledPadding();

        // Get the global position
        sf::Vector2f topLeftPosition = {((getAbsolutePosition().x + padding.left - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                        ((getAbsolutePosition().y + padding.top - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};

        sf::Vector2f bottomRightPosition = {((getAbsolutePosition().x + getSize().x - padding.right - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                            ((getAbsolutePosition().y + getSize().y - padding.bottom - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};

        if ((m_scroll != nullptr) && (m_scroll->getLowValue() < m_scroll->getMaximum()))
            bottomRightPosition.x -= m_scroll->getSize().x;

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Calculate the clipping area
        GLint scissorLeft = std::max(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
        GLint scissorTop = std::max(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
        GLint scissorRight = std::min(static_cast<GLint>(bottomRightPosition.x * scaleViewX), scissor[0] + scissor[2]);
        GLint scissorBottom = std::min(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

        if (scissorRight < scissorLeft)
            scissorRight = scissorLeft;
        else if (scissorBottom < scissorTop)
            scissorTop = scissorBottom;

        // Set the clipping area
        glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

        // Find out which items are visible
        unsigned int firstItem = 0;
        unsigned int lastItem = m_items.size();
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
            back.setFillColor(getRenderer()->m_selectedBackgroundColor);
            back.setPosition({getPosition().x + padding.left, getPosition().y + padding.top + (m_selectedItem * m_itemHeight)});

            if ((m_scroll != nullptr) && (m_scroll->getLowValue() < m_scroll->getMaximum()))
                back.setPosition({back.getPosition().x, back.getPosition().y - m_scroll->getValue()});

            target.draw(back, states);
        }

        // Draw the background of the item on which the mouse is standing
        if ((m_hoveringItem >= 0) && (m_hoveringItem != m_selectedItem) && (getRenderer()->m_hoverBackgroundColor != sf::Color::Transparent))
        {
            sf::RectangleShape back({getSize().x - padding.left - padding.right, static_cast<float>(m_itemHeight)});
            back.setFillColor(getRenderer()->m_hoverBackgroundColor);
            back.setPosition({getPosition().x + padding.left, getPosition().y + padding.top + (m_hoveringItem * m_itemHeight)});

            if ((m_scroll != nullptr) && (m_scroll->getLowValue() < m_scroll->getMaximum()))
                back.setPosition({back.getPosition().x, back.getPosition().y - m_scroll->getValue()});

            target.draw(back, states);
        }

        // Draw the items
        for (unsigned int i = firstItem; i < lastItem; ++i)
            target.draw(m_items[i], states);

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);

        // Draw the scrollbar
        if (m_scroll != nullptr)
            target.draw(*m_scroll, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setProperty(std::string property, const std::string& value, const std::string& rootPath)
    {
        if (property == "backgroundimage")
            extractTextureFromString(property, value, rootPath, m_backgroundTexture);
        else if (property == "backgroundcolor")
            setBackgroundColor(extractColorFromString(property, value));
        else if (property == "textcolor")
            setTextColor(extractColorFromString(property, value));
        else if (property == "textcolornormal")
            setTextColorNormal(extractColorFromString(property, value));
        else if (property == "textcolorhover")
            setTextColorHover(extractColorFromString(property, value));
        else if (property == "hoverbackgroundcolor")
            setHoverBackgroundColor(extractColorFromString(property, value));
        else if (property == "selectedbackgroundcolor")
            setSelectedBackgroundColor(extractColorFromString(property, value));
        else if (property == "selectedtextcolor")
            setSelectedTextColor(extractColorFromString(property, value));
        else if (property == "bordercolor")
            setBorderColor(extractColorFromString(property, value));
        else if (property == "borders")
            setBorders(extractBordersFromString(property, value));
        else if (property == "padding")
            setPadding(extractBordersFromString(property, value));
        else
            throw Exception{"Unrecognized property '" + property + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setBackgroundImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_backgroundTexture.load(filename, partRect, middlePart, repeated);
        else
            m_backgroundTexture = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_backgroundColor = backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setTextColor(const sf::Color& textColor)
    {
        m_textColor = textColor;
        m_hoverTextColor = textColor;

        m_listBox->updateItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setTextColorNormal(const sf::Color& textColor)
    {
        m_textColor = textColor;
        m_listBox->updateItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setTextColorHover(const sf::Color& textColor)
    {
        m_hoverTextColor = textColor;
        m_listBox->updateItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setHoverBackgroundColor(const sf::Color& hoverBackgroundColor)
    {
        m_hoverBackgroundColor = hoverBackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setSelectedBackgroundColor(const sf::Color& selectedBackgroundColor)
    {
        m_selectedBackgroundColor = selectedBackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setSelectedTextColor(const sf::Color& selectedTextColor)
    {
        m_selectedTextColor = selectedTextColor;
        m_listBox->updateItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setBorderColor(const sf::Color& borderColor)
    {
        m_borderColor = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setTextFont(std::shared_ptr<sf::Font> font)
    {
        m_listBox->m_font = font;

        for (auto& item : m_listBox->m_items)
            item.setTextFont(font);

        m_listBox->updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setPadding(const Padding& padding)
    {
        WidgetPadding::setPadding(padding);

        m_listBox->updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::setScrollbar(const std::string& scrollbarThemeFileFilename, const std::string& section)
    {
        m_listBox->m_scroll = Scrollbar::create(scrollbarThemeFileFilename, section);

        m_listBox->m_scroll->setSize({m_listBox->m_scroll->getSize().x, m_listBox->getSize().y});
        m_listBox->m_scroll->setLowValue(static_cast<unsigned int>(m_listBox->getSize().y));
        m_listBox->m_scroll->setMaximum(m_listBox->m_items.size() * m_listBox->m_itemHeight);
        m_listBox->m_scroll->setPosition(m_listBox->getPosition().x + m_listBox->getSize().x - m_listBox->m_scroll->getSize().x, m_listBox->getPosition().y);

        m_listBox->updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBoxRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Draw the background
        if (m_backgroundTexture.getData() == nullptr)
        {
            sf::RectangleShape background(m_listBox->getSize());
            background.setPosition(m_listBox->getPosition());
            background.setFillColor(m_backgroundColor);
            target.draw(background, states);
        }
        else
            target.draw(m_backgroundTexture, states);

        // Draw the borders
        if (m_borders != Borders{0, 0, 0, 0})
        {
            sf::Vector2f size = m_listBox->getSize();
            sf::Vector2f position = m_listBox->getPosition();

            // Draw left border
            sf::RectangleShape border({m_borders.left, size.y + m_borders.top});
            border.setPosition(position.x - m_borders.left, position.y - m_borders.top);
            border.setFillColor(m_borderColor);
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
        if (texture.getData() != nullptr)
        {
            switch (texture.getScalingType())
            {
            case Texture::ScalingType::Normal:
                scaledPadding.left = padding.left * (texture.getSize().x / texture.getImageSize().x);
                scaledPadding.right = padding.right * (texture.getSize().x / texture.getImageSize().x);
                scaledPadding.top = padding.top * (texture.getSize().y / texture.getImageSize().y);
                scaledPadding.bottom = padding.bottom * (texture.getSize().y / texture.getImageSize().y);
                break;

            case Texture::ScalingType::Horizontal:
                scaledPadding.left = padding.left * (texture.getSize().y / texture.getImageSize().y);
                scaledPadding.right = padding.right * (texture.getSize().y / texture.getImageSize().y);
                scaledPadding.top = padding.top * (texture.getSize().y / texture.getImageSize().y);
                scaledPadding.bottom = padding.bottom * (texture.getSize().y / texture.getImageSize().y);
                break;

            case Texture::ScalingType::Vertical:
                scaledPadding.left = padding.left * (texture.getSize().x / texture.getImageSize().x);
                scaledPadding.right = padding.right * (texture.getSize().x / texture.getImageSize().x);
                scaledPadding.top = padding.top * (texture.getSize().x / texture.getImageSize().x);
                scaledPadding.bottom = padding.bottom * (texture.getSize().x / texture.getImageSize().x);
                break;

            case Texture::ScalingType::NineSliceScaling:
                break;
            }
        }

        return scaledPadding;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> ListBoxRenderer::clone(Widget* widget)
    {
        auto renderer = std::shared_ptr<ListBoxRenderer>(new ListBoxRenderer{*this});
        renderer->m_listBox = static_cast<ListBox*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
