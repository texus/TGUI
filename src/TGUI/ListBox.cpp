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

#include <cassert>

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

    void ListBox::setPosition(const Layout& position)
    {
        Widget::setPosition(position);

        float textHeight = sf::Text{"kg", *m_textFont, m_textSize}.getLocalBounds().height;
        for (unsigned int i = 0; i < m_items.size(); ++i)
        {
            m_items[i].setPosition({getPosition().x + (m_textSize / 10.0f),
                                    getPosition().y + static_cast<float>(i * m_itemHeight) + ((m_itemHeight - textHeight) / 2.0f)});

            if ((m_scroll != nullptr) && (m_scroll->getLowValue() < m_scroll->getMaximum()))
                m_items[i].setPosition({m_items[i].getPosition().x, m_items[i].getPosition().y - m_scroll->getValue()});
        }

        if (m_scroll != nullptr)
            m_scroll->setPosition(getPosition().x + getSize().x - m_scroll->getSize().x, getPosition().y);
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

        updatePosition();
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

        updateItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setTextColor(const sf::Color& textColor)
    {
        m_textColor = textColor;
        updateItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setSelectedTextColor(const sf::Color& selectedTextColor)
    {
        m_selectedTextColor = selectedTextColor;
        updateItemColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setTextFont(const sf::Font& font)
    {
        m_textFont = &font;

        for (auto& item : m_items)
            item.setTextFont(font);

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
                unsigned int maximumItems = static_cast<unsigned int>(getSize().y) / m_itemHeight;

                // Check if the item still fits in the list box
                if (m_items.size() == maximumItems)
                    return false;
            }
            else // There is a scrollbar so tell it that another item was added
            {
                m_scroll->setMaximum(m_items.size() * m_itemHeight);
            }

            // Create the new item
            Label newItem;
            newItem.setTextFont(*m_textFont);
            newItem.setTextColor(m_textColor);
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
            {
                setSelectedItemByIndex(i);
                return true;
            }
        }

        // No match was found
        if (m_selectedItem >= 0)
            m_items[m_selectedItem].setTextColor(m_textColor);

        m_selectedItem = -1;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::setSelectedItemById(const sf::String& id)
    {
        for (unsigned int i = 0; i < m_itemIds.size(); ++i)
        {
            if (m_itemIds[i] == id)
            {
                setSelectedItemByIndex(i);
                return true;
            }
        }

        // No match was found
        if (m_selectedItem >= 0)
            m_items[m_selectedItem].setTextColor(m_textColor);

        m_selectedItem = -1;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::deselectItem()
    {
        if (m_selectedItem >= 0)
        {
            m_items[m_selectedItem].setTextColor(m_textColor);
            m_selectedItem = -1;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItem(const sf::String& itemName)
    {
        for (unsigned int i = 0; i < m_items.size(); ++i)
        {
            if (m_items[i].getText() == itemName)
            {
                removeItemByIndex(i);
                return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItemById(const sf::String& id)
    {
        for (unsigned int i = 0; i < m_itemIds.size(); ++i)
        {
            if (m_itemIds[i] == id)
            {
                removeItemByIndex(i);
                return true;
            }
        }

        return false;
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
        m_scroll->setPosition(getPosition().x + getSize().x - m_scroll->getSize().x, getPosition().y);

        updatePosition();
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
            if (m_scroll->mouseOnWidget(x, y))
            {
                m_scroll->leftMousePressed(x, y);
                clickedOnListBox = false;
            }
        }

        // If the click occured on the list box
        if (clickedOnListBox)
        {
            if (m_selectedItem >= 0)
                m_items[m_selectedItem].setTextColor(m_textColor);

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

            if (m_selectedItem >= 0)
                m_items[m_selectedItem].setTextColor(m_selectedTextColor);

            // Add the callback (if the user requested it)
            if ((oldSelectedItem != m_selectedItem) && (m_callbackFunctions[ItemSelected].empty() == false))
            {
                // When no item is selected then send an empty string, otherwise send the item
                if (m_selectedItem < 0)
                    m_callback.text  = "";
                else
                    m_callback.text = m_items[m_selectedItem].getText();

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

            updatePosition();
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

                updatePosition();
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

    void ListBox::updateItemColors()
    {
        for (auto& item : m_items)
            item.setTextColor(m_textColor);

        if (m_selectedItem >= 0)
            m_items[m_selectedItem].setTextColor(m_selectedTextColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setSelectedItemByIndex(unsigned int index)
    {
        assert(index < m_items.size());

        if (m_selectedItem >= 0)
            m_items[m_selectedItem].setTextColor(m_textColor);

        // Select the item
        m_selectedItem = static_cast<int>(index);
        m_items[m_selectedItem].setTextColor(m_selectedTextColor);

        // Move the scrollbar if needed
        if (m_scroll)
        {
            if (m_selectedItem * getItemHeight() < m_scroll->getValue())
                m_scroll->setValue(m_selectedItem * getItemHeight());
            else if ((m_selectedItem + 1) * getItemHeight() > m_scroll->getValue() + m_scroll->getLowValue())
                m_scroll->setValue((m_selectedItem + 1) * getItemHeight() - m_scroll->getLowValue());

            updatePosition();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::removeItemByIndex(unsigned int index)
    {
        assert(index < m_items.size());

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
            --m_selectedItem;
    }

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

        // Draw the background
        sf::RectangleShape front(getSize());
        front.setPosition(getPosition());
        front.setFillColor(m_backgroundColor);
        target.draw(front, states);

        // Draw the borders
        {
            // Draw left border
            sf::RectangleShape border({m_borders.left, getSize().y + m_borders.top});
            border.setPosition(getPosition().x - m_borders.left, getPosition().y - m_borders.top);
            border.setFillColor(m_borderColor);
            target.draw(border, states);

            // Draw top border
            border.setSize({getSize().x + m_borders.right, m_borders.top});
            border.setPosition(getPosition().x, getPosition().y - m_borders.top);
            target.draw(border, states);

            // Draw right border
            border.setSize({m_borders.right, getSize().y + m_borders.bottom});
            border.setPosition(getPosition().x + getSize().x, getPosition().y);
            target.draw(border, states);

            // Draw bottom border
            border.setSize({getSize().x + m_borders.left, m_borders.bottom});
            border.setPosition(getPosition().x - m_borders.left, getPosition().y + getSize().y);
            target.draw(border, states);
        }

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
            sf::RectangleShape back({getSize().x, static_cast<float>(m_itemHeight)});
            back.setFillColor(m_selectedBackgroundColor);
            back.setPosition({getPosition().x, getPosition().y + (m_selectedItem * m_itemHeight)});

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
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
