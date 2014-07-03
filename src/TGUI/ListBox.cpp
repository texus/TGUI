/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2014 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <algorithm>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::ListBox()
    {
        m_callback.widgetType = Type_ListBox;
        m_draggableWidget = true;

        setSize({50, 100});
        changeColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::ListBox(const ListBox& listBoxToCopy) :
        Widget                   {listBoxToCopy},
        WidgetBorders            {listBoxToCopy},
        m_loadedConfigFile       {listBoxToCopy.m_loadedConfigFile},
        m_items                  {listBoxToCopy.m_items},
        m_selectedItem           {listBoxToCopy.m_selectedItem},
        m_itemHeight             {listBoxToCopy.m_itemHeight},
        m_textSize               {listBoxToCopy.m_textSize},
        m_maxItems               {listBoxToCopy.m_maxItems},
        m_backgroundColor        {listBoxToCopy.m_backgroundColor},
        m_textColor              {listBoxToCopy.m_textColor},
        m_selectedBackgroundColor{listBoxToCopy.m_selectedBackgroundColor},
        m_selectedTextColor      {listBoxToCopy.m_selectedTextColor},
        m_borderColor            {listBoxToCopy.m_borderColor},
        m_textFont               {listBoxToCopy.m_textFont}
    {
        if (listBoxToCopy.m_scroll != nullptr)
            m_scroll = Scrollbar::copy(listBoxToCopy.m_scroll);
        else
            m_scroll = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox& ListBox::operator= (const ListBox& right)
    {
        if (this != &right)
        {
            ListBox temp(right);
            Widget::operator=(right);
            WidgetBorders::operator=(right);

            std::swap(m_loadedConfigFile,        temp.m_loadedConfigFile);
            std::swap(m_items,                   temp.m_items);
            std::swap(m_selectedItem,            temp.m_selectedItem);
            std::swap(m_itemHeight,              temp.m_itemHeight);
            std::swap(m_textSize,                temp.m_textSize);
            std::swap(m_maxItems,                temp.m_maxItems);
            std::swap(m_scroll,                  temp.m_scroll);
            std::swap(m_backgroundColor,         temp.m_backgroundColor);
            std::swap(m_textColor,               temp.m_textColor);
            std::swap(m_selectedBackgroundColor, temp.m_selectedBackgroundColor);
            std::swap(m_selectedTextColor,       temp.m_selectedTextColor);
            std::swap(m_borderColor,             temp.m_borderColor);
            std::swap(m_textFont,                temp.m_textFont);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::Ptr ListBox::create(const std::string& configFileFilename)
    {
        auto listBox = std::make_shared<ListBox>();

        listBox->m_loadedConfigFile = getResourcePath() + configFileFilename;

        // Open the config file
        ConfigFile configFile{listBox->m_loadedConfigFile, "ListBox"};

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = configFileFilename.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = configFileFilename.substr(0, slashPos+1);

        // Handle the read properties
        for (auto it = configFile.getProperties().cbegin(); it != configFile.getProperties().cend(); ++it)
        {
            if (it->first == "backgroundcolor")
            {
                listBox->setBackgroundColor(extractColor(it->second));
            }
            else if (it->first == "textcolor")
            {
                listBox->setTextColor(extractColor(it->second));
            }
            else if (it->first == "selectedbackgroundcolor")
            {
                listBox->setSelectedBackgroundColor(extractColor(it->second));
            }
            else if (it->first == "selectedtextcolor")
            {
                listBox->setSelectedTextColor(extractColor(it->second));
            }
            else if (it->first == "bordercolor")
            {
                listBox->setBorderColor(extractColor(it->second));
            }
            else if (it->first == "borders")
            {
                Borders borders;
                if (extractBorders(it->second, borders))
                    listBox->setBorders(borders);
                else
                    throw Exception{"Failed to parse the 'Borders' property in section ListBox in " + listBox->m_loadedConfigFile};
            }
            else if (it->first == "scrollbar")
            {
                if ((it->second.length() < 3) || (it->second[0] != '"') || (it->second[it->second.length()-1] != '"'))
                    throw Exception{"Failed to parse value for Scrollbar in section ListBox in " + listBox->m_loadedConfigFile + "."};

                try
                {
                    // load the scrollbar
                    listBox->m_scroll = Scrollbar::create(configFileFolder + it->second.substr(1, it->second.length()-2));
                }
                catch (const Exception& e)
                {
                    listBox->m_scroll = nullptr;
                    throw Exception{"Failed to create the internal scrollbar in ListBox. " + std::string{e.what()}};
                }

                // Initialize the scrollbar
                listBox->m_scroll->setVerticalScroll(true);
                listBox->m_scroll->setSize({listBox->m_scroll->getSize().x, listBox->getSize().y});
                listBox->m_scroll->setLowValue(listBox->getSize().y);
                listBox->m_scroll->setMaximum(listBox->m_items.size() * listBox->m_itemHeight);
            }
            else
                throw Exception{"Unrecognized property '" + it->first + "' in section ListBox in " + listBox->m_loadedConfigFile + "."};
        }

        return listBox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setSize(const Layout& size)
    {
        Widget::setSize(size);

        // If there is a scrollbar then reinitialize it
        if (m_scroll != nullptr)
        {
            m_scroll->setSize({m_scroll->getSize().x, getSize().y});
            m_scroll->setLowValue(static_cast<unsigned int>(getSize().y));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::changeColors(const sf::Color& backgroundColor,         const sf::Color& textColor,
                               const sf::Color& selectedBackgroundColor, const sf::Color& selectedTextColor,
                               const sf::Color& borderColor)
    {
        m_backgroundColor         = backgroundColor;
        m_textColor               = textColor;
        m_selectedBackgroundColor = selectedBackgroundColor;
        m_selectedTextColor       = selectedTextColor;
        m_borderColor             = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ListBox::addItem(const sf::String& itemName, int id)
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
                    return -1;
            }

            // Add the item to the list
            m_items.push_back(itemName);
            m_itemIds.push_back(id);

            // If there is a scrollbar then tell it that another item was added
            if (m_scroll != nullptr)
                m_scroll->setMaximum(m_items.size() * m_itemHeight);

            // Return the item index
            return m_items.size() - 1;
        }
        else // The item limit was reached
            return -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::setSelectedItem(const sf::String& itemName)
    {
        // Loop through all items
        for (unsigned int i = 0; i < m_items.size(); ++i)
        {
            // Check if a match was found
            if (m_items[i] == itemName)
            {
                // Select the item
                m_selectedItem = static_cast<int>(i);

                // Move the scrollbar if needed
                if (m_scroll)
                {
                    if (m_selectedItem * getItemHeight() < m_scroll->getValue())
                        m_scroll->setValue(m_selectedItem * getItemHeight());
                    else if ((m_selectedItem + 1) * getItemHeight() > m_scroll->getValue() + m_scroll->getLowValue())
                        m_scroll->setValue((m_selectedItem + 1) * getItemHeight() - m_scroll->getLowValue());
                }

                return true;
            }
        }

        // No match was found
        m_selectedItem = -1;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::setSelectedItem(int index)
    {
        if (index < 0)
        {
            deselectItem();
            return true;
        }

        // If the index is too high then deselect the items
        if (index > static_cast<int>(m_items.size())-1)
        {
            m_selectedItem = -1;
            return false;
        }

        // Select the item
        m_selectedItem = index;

        // Move the scrollbar if needed
        if (m_scroll)
        {
            if (m_selectedItem * getItemHeight() < m_scroll->getValue())
                m_scroll->setValue(m_selectedItem * getItemHeight());
            else if ((m_selectedItem + 1) * getItemHeight() > m_scroll->getValue() + m_scroll->getLowValue())
                m_scroll->setValue((m_selectedItem + 1) * getItemHeight() - m_scroll->getLowValue());
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::deselectItem()
    {
        m_selectedItem = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItem(unsigned int index)
    {
        // The index can't be too high
        if (index > m_items.size()-1)
            return false;

        // Remove the item
        m_items.erase(m_items.begin() + index);
        m_itemIds.erase(m_itemIds.begin() + index);

        // If there is a scrollbar then tell it that an item was removed
        if (m_scroll != nullptr)
            m_scroll->setMaximum(m_items.size() * m_itemHeight);

        // Check if the selected item should change
        if (m_selectedItem == static_cast<int>(index))
            m_selectedItem = -1;
        else if (m_selectedItem > static_cast<int>(index))
            --m_selectedItem;

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItem(const sf::String& itemName)
    {
        // Loop through all items
        for (unsigned int i = 0; i < m_items.size(); ++i)
        {
            // When the name matches then delete the item
            if (m_items[i] == itemName)
            {
                m_items.erase(m_items.begin() + i);
                m_itemIds.erase(m_itemIds.begin() + i);

                // Check if the selected item should change
                if (m_selectedItem == static_cast<int>(i))
                    m_selectedItem = -1;
                else if (m_selectedItem > static_cast<int>(i))
                    --m_selectedItem;

                // If there is a scrollbar then tell it that an item was removed
                if (m_scroll != nullptr)
                    m_scroll->setMaximum(m_items.size() * m_itemHeight);

                return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListBox::removeItemsById(int id)
    {
        unsigned int removedItems = 0;

        for (unsigned int i = 0; i < m_items.size();)
        {
            if (m_itemIds[i] == id)
            {
                m_items.erase(m_items.begin() + i);
                m_itemIds.erase(m_itemIds.begin() + i);

                removedItems++;
            }
            else
                ++i;
        }

        return removedItems;
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

    sf::String ListBox::getItem(unsigned int index) const
    {
        // The index can't be too high
        if (index > m_items.size()-1)
            return "";

        // Return the item
        return m_items[index];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ListBox::getItemIndex(const sf::String& itemName) const
    {
        // Loop through all items
        for (unsigned int i = 0; i < m_items.size(); ++i)
        {
            // When the name matches then return the index
            if (m_items[i] == itemName)
                return i;
        }

        // No match was found
        return -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::changeItem(unsigned int index, const sf::String& newValue)
    {
        if (index >= m_items.size()) {
            return false;
        }

        m_items[index] = newValue;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListBox::changeItems(const sf::String& originalValue, const sf::String& newValue)
    {
        unsigned int amountChanged = 0;
        for (auto it = m_items.begin(); it != m_items.end(); ++it)
        {
            if (*it == originalValue)
            {
                *it = newValue;
                amountChanged++;
            }
        }

        return amountChanged;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListBox::changeItemsById(int id, const sf::String& newValue)
    {
        unsigned int amountChanged = 0;
        auto idIt = m_itemIds.begin();
        for (auto it = m_items.begin(); it != m_items.end(); ++it, ++idIt)
        {
            if (*idIt == id)
            {
                *it = newValue;
                amountChanged++;
            }
        }

        return amountChanged;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::setScrollbar(const std::string& scrollbarConfigFileFilename)
    {
        // Calling setScrollbar with an empty string does the same as removeScrollbar
        if (scrollbarConfigFileFilename.empty() == true)
        {
            removeScrollbar();
            return true;
        }

        try
        {
            m_scroll = Scrollbar::create(scrollbarConfigFileFilename);
        }
        catch (const Exception& e)
        {
            m_scroll = nullptr;
            return false;
        }

        // Initialize the scrollbar
        m_scroll->setVerticalScroll(true);
        m_scroll->setSize({m_scroll->getSize().x, getSize().y});
        m_scroll->setLowValue(static_cast<unsigned int>(getSize().y));
        m_scroll->setMaximum(m_items.size() * m_itemHeight);

        return true;
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

            // Remove the items that didn't fit inside the list box
            m_items.erase(m_items.begin() + m_maxItems, m_items.end());
            m_itemIds.erase(m_itemIds.begin() + m_maxItems, m_itemIds.end());
        }
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

        // Some items might be removed when there is no scrollbar
        if (m_scroll == nullptr)
        {
            // When the items no longer fit inside the list box then we need to remove some
            if ((m_items.size() * m_itemHeight) > static_cast<unsigned int>(getSize().y))
            {
                // Calculate ho many items fit inside the list box
                m_maxItems = static_cast<unsigned int>(getSize().y) / m_itemHeight;

                // Remove the items that didn't fit inside the list box
                m_items.erase(m_items.begin() + m_maxItems, m_items.end());
                m_itemIds.erase(m_itemIds.begin() + m_maxItems, m_itemIds.end());
            }
        }
        else // There is a scrollbar
        {
            // Set the maximum of the scrollbar
            m_scroll->setMaximum(m_items.size() * m_itemHeight);
        }
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
                m_scroll->setMaximum(m_items.size() * m_itemHeight);
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
        {
            // Temporarily set the position of the scroll
            m_scroll->setPosition({getPosition().x + getSize().x - m_scroll->getSize().x, getPosition().y});

            // Pass the event
            m_scroll->mouseOnWidget(x, y);

            // Reset the position
            m_scroll->setPosition({0, 0});
        }

        // Check if the mouse is on top of the list box
        if (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x, getSize().y)).contains(x, y))
            return true;
        else // The mouse is not on top of the list box
        {
            if (m_mouseHover)
                mouseLeftWidget();

            m_mouseHover = false;
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::leftMousePressed(float x, float y)
    {
        // Set the mouse down flag to true
        m_mouseDown = true;

        // This will be true when the click didn't occur on the scrollbar
        bool clickedOnListBox = true;

        // If there is a scrollbar then pass the event
        if (m_scroll != nullptr)
        {
            // Temporarily set the position of the scroll
            m_scroll->setPosition({getPosition().x + getSize().x - m_scroll->getSize().x, getPosition().y});

            // Pass the event
            if (m_scroll->mouseOnWidget(x, y))
            {
                m_scroll->leftMousePressed(x, y);
                clickedOnListBox = false;
            }

            // Reset the position
            m_scroll->setPosition({0, 0});
        }

        // If the click occured on the list box
        if (clickedOnListBox)
        {
            // Remember the old selected item
            int oldSelectedItem = m_selectedItem;

            // Check if there is a scrollbar or whether it is hidden
            if ((m_scroll != nullptr) && (m_scroll->getLowValue() < m_scroll->getMaximum()))
            {
                // Check if we clicked on the first (perhaps partially) visible item
                if (y - getPosition().y <= (m_itemHeight - (m_scroll->getValue() % m_itemHeight)))
                {
                    // We clicked on the first visible item
                    m_selectedItem = static_cast<int>(m_scroll->getValue() / m_itemHeight);
                }
                else // We didn't click on the first visible item
                {
                    // Calculate on what item we clicked
                    if ((m_scroll->getValue() % m_itemHeight) == 0)
                        m_selectedItem = static_cast<int>((y - getPosition().y) / m_itemHeight + (m_scroll->getValue() / m_itemHeight));
                    else
                        m_selectedItem = static_cast<int>((((y - getPosition().y) - (m_itemHeight - (m_scroll->getValue() % m_itemHeight))) / m_itemHeight) + (m_scroll->getValue() / m_itemHeight) + 1);
                }
            }
            else // There is no scrollbar or it is not displayed
            {
                // Calculate on which item we clicked
                m_selectedItem = static_cast<int>((y - getPosition().y) / m_itemHeight);

                // When you clicked behind the last item then unselect the selected item
                if (m_selectedItem > static_cast<int>(m_items.size())-1)
                    m_selectedItem = -1;
            }

            // Add the callback (if the user requested it)
            if ((oldSelectedItem != m_selectedItem) && (m_callbackFunctions[ItemSelected].empty() == false))
            {
                // When no item is selected then send an empty string, otherwise send the item
                if (m_selectedItem < 0)
                    m_callback.text  = "";
                else
                    m_callback.text = m_items[m_selectedItem];

                m_callback.value   = m_selectedItem;
                m_callback.trigger = ItemSelected;
                addCallback();
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

            // Temporarily set the position of the scroll
            m_scroll->setPosition({getPosition().x + (getSize().x - m_scroll->getSize().x), getPosition().y});

            // Pass the event
            m_scroll->leftMouseReleased(x, y);

            // Reset the position
            m_scroll->setPosition({0, 0});

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
        }

        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseMoved(float x, float y)
    {
        if (m_mouseHover == false)
            mouseEnteredWidget();

        m_mouseHover = true;

        // If there is a scrollbar then pass the event
        if (m_scroll != nullptr)
        {
            // Temporarily set the position of the scroll
            m_scroll->setPosition({getPosition().x + (getSize().x - m_scroll->getSize().x), getPosition().y});

            // Check if you are dragging the thumb of the scrollbar
            if ((m_scroll->m_mouseDown) && (m_scroll->m_mouseDownOnThumb))
            {
                // Pass the event, even when the mouse is not on top of the scrollbar
                m_scroll->mouseMoved(x, y);
            }
            else // You are just moving the mouse
            {
                // When the mouse is on top of the scrollbar then pass the mouse move event
                if (m_scroll->mouseOnWidget(x, y))
                    m_scroll->mouseMoved(x, y);
            }

            // Reset the position
            m_scroll->setPosition({0, 0});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseWheelMoved(int delta, int, int)
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
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseNotOnWidget()
    {
        if (m_mouseHover)
            mouseLeftWidget();

        m_mouseHover = false;

        if (m_scroll != nullptr)
            m_scroll->m_mouseHover = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseNoLongerDown()
    {
        m_mouseDown = false;

        if (m_scroll != nullptr)
            m_scroll->m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
    void ListBox::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "configfile")
        {
            load(value);
        }
        else if (property == "backgroundcolor")
        {
            setBackgroundColor(extractColor(value));
        }
        else if (property == "textcolor")
        {
            setTextColor(extractColor(value));
        }
        else if (property == "selectedbackgroundcolor")
        {
            setSelectedBackgroundColor(extractColor(value));
        }
        else if (property == "selectedtextcolor")
        {
            setSelectedTextColor(extractColor(value));
        }
        else if (property == "bordercolor")
        {
            setBorderColor(extractColor(value));
        }
        else if (property == "itemheight")
        {
            setItemHeight(tgui::stoi(value));
        }
        else if (property == "maximumitems")
        {
            setMaximumItems(tgui::stoi(value));
        }
        else if (property == "borders")
        {
            Borders borders;
            if (extractBorders(value, borders))
                setBorders(borders);
            else
                throw Exception{"Failed to parse 'Borders' property."};
        }
        else if (property == "items")
        {
            removeAllItems();

            std::vector<sf::String> items;
            decodeList(value, items);

            for (auto it = items.cbegin(); it != items.cend(); ++it)
                addItem(*it);
        }
        else if (property == "selecteditem")
        {
            setSelectedItem(tgui::stoi(value));
        }
        else if (property == "callback")
        {
            Widget::setProperty(property, value);

            std::vector<sf::String> callbacks;
            decodeList(value, callbacks);

            for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
            {
                if ((*it == "ItemSelected") || (*it == "itemselected"))
                    bindCallback(ItemSelected);
            }
        }
        else // The property didn't match
            Widget::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "backgroundcolor")
            value = "(" + tgui::to_string(int(getBackgroundColor().r)) + "," + tgui::to_string(int(getBackgroundColor().g)) + "," + tgui::to_string(int(getBackgroundColor().b)) + "," + tgui::to_string(int(getBackgroundColor().a)) + ")";
        else if (property == "textcolor")
            value = "(" + tgui::to_string(int(getTextColor().r)) + "," + tgui::to_string(int(getTextColor().g)) + "," + tgui::to_string(int(getTextColor().b)) + "," + tgui::to_string(int(getTextColor().a)) + ")";
        else if (property == "selectedbackgroundcolor")
            value = "(" + tgui::to_string(int(getSelectedBackgroundColor().r)) + "," + tgui::to_string(int(getSelectedBackgroundColor().g))
                    + "," + tgui::to_string(int(getSelectedBackgroundColor().b)) + "," + tgui::to_string(int(getSelectedBackgroundColor().a)) + ")";
        else if (property == "selectedtextcolor")
            value = "(" + tgui::to_string(int(getSelectedTextColor().r)) + "," + tgui::to_string(int(getSelectedTextColor().g))
                    + "," + tgui::to_string(int(getSelectedTextColor().b)) + "," + tgui::to_string(int(getSelectedTextColor().a)) + ")";
        else if (property == "bordercolor")
            value = "(" + tgui::to_string(int(getBorderColor().r)) + "," + tgui::to_string(int(getBorderColor().g)) + "," + tgui::to_string(int(getBorderColor().b)) + "," + tgui::to_string(int(getBorderColor().a)) + ")";
        else if (property == "itemheight")
            value = tgui::to_string(getItemHeight());
        else if (property == "maximumitems")
            value = tgui::to_string(getMaximumItems());
        else if (property == "borders")
            value = "(" + tgui::to_string(getBorders().left) + "," + tgui::to_string(getBorders().top) + "," + tgui::to_string(getBorders().right) + "," + tgui::to_string(getBorders().bottom) + ")";
        else if (property == "items")
            encodeList(m_items, value);
        else if (property == "selecteditem")
            value = tgui::to_string(getSelectedItemIndex());
        else if (property == "callback")
        {
            std::string tempValue;
            Widget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_callbackFunctions.find(ItemSelected) != m_callbackFunctions.end()) && (m_callbackFunctions.at(ItemSelected).size() == 1) && (m_callbackFunctions.at(ItemSelected).front() == nullptr))
                callbacks.push_back("ItemSelected");

            encodeList(callbacks, value);

            if (value.empty() || tempValue.empty())
                value += tempValue;
            else
                value += "," + tempValue;
        }
        else // The property didn't match
            Widget::getProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::list< std::pair<std::string, std::string> > ListBox::getPropertyList() const
    {
        auto list = Widget::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("ConfigFile", "string"));
        list.push_back(std::pair<std::string, std::string>("BackgroundColor", "color"));
        list.push_back(std::pair<std::string, std::string>("TextColor", "color"));
        list.push_back(std::pair<std::string, std::string>("SelectedBackgroundColor", "color"));
        list.push_back(std::pair<std::string, std::string>("SelectedTextColor", "color"));
        list.push_back(std::pair<std::string, std::string>("BorderColor", "color"));
        list.push_back(std::pair<std::string, std::string>("ItemHeight", "uint"));
        list.push_back(std::pair<std::string, std::string>("MaximumItems", "uint"));
        list.push_back(std::pair<std::string, std::string>("Borders", "borders"));
        list.push_back(std::pair<std::string, std::string>("Items", "string"));
        list.push_back(std::pair<std::string, std::string>("SelectedItem", "int"));
        return list;
    }
*/
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!getTextFont() && m_parent->getGlobalFont())
            setTextFont(*m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        const sf::View& view = target.getView();

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / view.getSize().x;
        float scaleViewY = target.getSize().y / view.getSize().y;

        // Get the global position
        sf::Vector2f topLeftPosition;
        sf::Vector2f bottomRightPosition;

        if ((m_scroll != nullptr) && (m_scroll->getLowValue() < m_scroll->getMaximum()))
        {
            topLeftPosition = {((getAbsolutePosition().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                               ((getAbsolutePosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};
            bottomRightPosition = {(getAbsolutePosition().x + getSize().x - m_scroll->getSize().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                   (getAbsolutePosition().y + getSize().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top)};
        }
        else
        {
            topLeftPosition = {((getAbsolutePosition().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                               ((getAbsolutePosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};
            bottomRightPosition = {(getAbsolutePosition().x + getSize().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                   (getAbsolutePosition().y + getSize().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top)};
        }

        // Adjust the transformation
        states.transform *= getTransform();

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Draw the borders
        {
            // Draw left border
            sf::RectangleShape border({m_borders.left, getSize().y + m_borders.top});
            border.setPosition(-m_borders.left, -m_borders.top);
            border.setFillColor(m_borderColor);
            target.draw(border, states);

            // Draw top border
            border.setSize({getSize().x + m_borders.right, m_borders.top});
            border.setPosition(0, -m_borders.top);
            target.draw(border, states);

            // Draw right border
            border.setSize({m_borders.right, getSize().y + m_borders.bottom});
            border.setPosition(getSize().x, 0);
            target.draw(border, states);

            // Draw bottom border
            border.setSize({getSize().x + m_borders.left, m_borders.bottom});
            border.setPosition(-m_borders.left, getSize().y);
            target.draw(border, states);
        }

        // Draw the background
        sf::RectangleShape front(getSize());
        front.setFillColor(m_backgroundColor);
        target.draw(front, states);

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Calculate the clipping area
        GLint scissorLeft = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
        GLint scissorTop = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
        GLint scissorRight = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.x * scaleViewX), scissor[0] + scissor[2]);
        GLint scissorBottom = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

        // If the widget outside the window then don't draw anything
        if (scissorRight < scissorLeft)
            scissorRight = scissorLeft;
        else if (scissorBottom < scissorTop)
            scissorTop = scissorBottom;

        // Create a text widget to draw the items
        Label text;
        text.setTextFont(*m_textFont);
        text.setTextSize(m_textSize);

        // Check if there is a scrollbar and whether it isn't hidden
        if ((m_scroll != nullptr) && (m_scroll->getLowValue() < m_scroll->getMaximum()))
        {
            // Store the transformation
            sf::Transform storedTransform = states.transform;

            // Find out which items should be drawn
            unsigned int firstItem = m_scroll->getValue() / m_itemHeight;
            unsigned int lastItem = (m_scroll->getValue() + m_scroll->getLowValue()) / m_itemHeight;

            // Show another item when the scrollbar is standing between two items
            if ((m_scroll->getValue() + m_scroll->getLowValue()) % m_itemHeight != 0)
                ++lastItem;

            // Set the clipping area
            glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

            for (unsigned int i = firstItem; i < lastItem; ++i)
            {
                // Restore the transformations
                states.transform = storedTransform;

                // Set the next item
                text.setText(m_items[i]);

                // Check if we are drawing the selected item
                if (m_selectedItem == static_cast<int>(i))
                {
                    // Draw a background for the selected item
                    {
                        // Set a new transformation
                        states.transform.translate(0, (static_cast<float>(i * m_itemHeight) - m_scroll->getValue()));

                        // Create and draw the background
                        sf::RectangleShape back({getSize().x, static_cast<float>(m_itemHeight)});
                        back.setFillColor(m_selectedBackgroundColor);
                        target.draw(back, states);

                        // Restore the transformation
                        states.transform = storedTransform;
                    }

                    // Change the text color
                    text.setTextColor(m_selectedTextColor);
                }
                else // Set the normal text color
                    text.setTextColor(m_textColor);

                // Set the translation for the text
                states.transform.translate(2, static_cast<float>(i * m_itemHeight) - m_scroll->getValue() + ((m_itemHeight - text.getSize().y) / 2.0f));

                // Draw the text
                target.draw(text, states);
            }
        }
        else // There is no scrollbar or it is invisible
        {
            // Set the clipping area
            glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

            // Store the current transformations
            sf::Transform storedTransform = states.transform;

            for (unsigned int i = 0; i < m_items.size(); ++i)
            {
                // Restore the transformations
                states.transform = storedTransform;

                // Set the next item
                text.setText(m_items[i]);

                // Check if we are drawing the selected item
                if (m_selectedItem == static_cast<int>(i))
                {
                    // Draw a background for the selected item
                    {
                        // Set a new transformation
                        states.transform.translate(0, static_cast<float>(i * m_itemHeight));

                        // Create and draw the background
                        sf::RectangleShape back({getSize().x, static_cast<float>(m_itemHeight)});
                        back.setFillColor(m_selectedBackgroundColor);
                        target.draw(back, states);

                        // Restore the transformation
                        states.transform = storedTransform;
                    }

                    // Change the text color
                    text.setTextColor(m_selectedTextColor);
                }
                else // Set the normal text color
                    text.setTextColor(m_textColor);

                // Set the translation for the text
                states.transform.translate(2, (i * m_itemHeight) + ((m_itemHeight - text.getSize().y) / 2.0f));

                // Draw the text
                target.draw(text, states);
            }
        }

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);

        // Check if there is a scrollbar
        if (m_scroll != nullptr)
        {
            // Reset the transformation
            states.transform = oldTransform;
            states.transform.translate(getSize().x - m_scroll->getSize().x, 0);

            // Draw the scrollbar
            target.draw(*m_scroll, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
