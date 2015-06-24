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


#include <cmath>
#include <algorithm>

#include <SFML/OpenGL.hpp>

#include <TGUI/Scrollbar.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/ListBox.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::ListBox() :
    m_SelectedItem(-1),
    m_Size        (50, 100),
    m_ItemHeight  (24),
    m_TextSize    (19),
    m_MaxItems    (0),
    m_Scroll      (nullptr),
    m_TextFont    (nullptr)
    {
        m_Callback.widgetType = Type_ListBox;
        m_DraggableWidget = true;
        m_Loaded = true;

        changeColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::ListBox(const ListBox& copy) :
    Widget                   (copy),
    WidgetBorders            (copy),
    m_LoadedConfigFile       (copy.m_LoadedConfigFile),
    m_Items                  (copy.m_Items),
    m_ItemIds                (copy.m_ItemIds),
    m_SelectedItem           (copy.m_SelectedItem),
    m_Size                   (copy.m_Size),
    m_ItemHeight             (copy.m_ItemHeight),
    m_TextSize               (copy.m_TextSize),
    m_MaxItems               (copy.m_MaxItems),
    m_BackgroundColor        (copy.m_BackgroundColor),
    m_TextColor              (copy.m_TextColor),
    m_SelectedBackgroundColor(copy.m_SelectedBackgroundColor),
    m_SelectedTextColor      (copy.m_SelectedTextColor),
    m_BorderColor            (copy.m_BorderColor),
    m_TextFont               (copy.m_TextFont)
    {
        // If there is a scrollbar then copy it
        if (copy.m_Scroll != nullptr)
            m_Scroll = new Scrollbar(*copy.m_Scroll);
        else
            m_Scroll = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::~ListBox()
    {
        if (m_Scroll != nullptr)
            delete m_Scroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox& ListBox::operator= (const ListBox& right)
    {
        if (this != &right)
        {
            ListBox temp(right);
            this->Widget::operator=(right);
            this->WidgetBorders::operator=(right);

            // If there already was a scrollbar then delete it now
            if (m_Scroll != nullptr)
            {
                delete m_Scroll;
                m_Scroll = nullptr;
            }

            std::swap(m_LoadedConfigFile,        temp.m_LoadedConfigFile);
            std::swap(m_Items,                   temp.m_Items);
            std::swap(m_ItemIds,                 temp.m_ItemIds);
            std::swap(m_SelectedItem,            temp.m_SelectedItem);
            std::swap(m_Size,                    temp.m_Size);
            std::swap(m_ItemHeight,              temp.m_ItemHeight);
            std::swap(m_TextSize,                temp.m_TextSize);
            std::swap(m_MaxItems,                temp.m_MaxItems);
            std::swap(m_Scroll,                  temp.m_Scroll);
            std::swap(m_BackgroundColor,         temp.m_BackgroundColor);
            std::swap(m_TextColor,               temp.m_TextColor);
            std::swap(m_SelectedBackgroundColor, temp.m_SelectedBackgroundColor);
            std::swap(m_SelectedTextColor,       temp.m_SelectedTextColor);
            std::swap(m_BorderColor,             temp.m_BorderColor);
            std::swap(m_TextFont,                temp.m_TextFont);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox* ListBox::clone()
    {
        return new ListBox(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::load(const std::string& configFileFilename, const std::string& sectionName)
    {
        m_LoadedConfigFile = getResourcePath() + configFileFilename;

        // If there already was a scrollbar then delete it now
        if (m_Scroll != nullptr)
        {
            delete m_Scroll;
            m_Scroll = nullptr;
        }

        // Open the config file
        ConfigFile configFile;
        if (!configFile.open(m_LoadedConfigFile))
        {
            TGUI_OUTPUT("TGUI error: Failed to open " + m_LoadedConfigFile + ".");
            return false;
        }

        // Read the properties and their values (as strings)
        std::vector<std::string> properties;
        std::vector<std::string> values;
        if (!configFile.read(sectionName, properties, values))
        {
            TGUI_OUTPUT("TGUI error: Failed to parse " + m_LoadedConfigFile + ".");
            return false;
        }

        // Close the config file
        configFile.close();

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = configFileFilename.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = configFileFilename.substr(0, slashPos+1);

        // Handle the read properties
        for (unsigned int i = 0; i < properties.size(); ++i)
        {
            std::string property = properties[i];
            std::string value = values[i];

            if (property == "backgroundcolor")
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
            else if (property == "borders")
            {
                Borders borders;
                if (extractBorders(value, borders))
                    setBorders(borders.left, borders.top, borders.right, borders.bottom);
            }
            else if (property == "scrollbar")
            {
                if ((value.length() < 3) || (value[0] != '"') || (value[value.length()-1] != '"'))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for Scrollbar in section ChatBox in " + m_LoadedConfigFile + ".");
                    return false;
                }

                // load the scrollbar and check if it failed
                m_Scroll = new Scrollbar();
                if (m_Scroll->load(configFileFolder + value.substr(1, value.length()-2)) == false)
                {
                    // The scrollbar couldn't be loaded so it must be deleted
                    delete m_Scroll;
                    m_Scroll = nullptr;

                    return false;
                }
                else // The scrollbar was loaded successfully
                {
                    // Initialize the scrollbar
                    m_Scroll->setVerticalScroll(true);
                    m_Scroll->setSize(m_Scroll->getSize().x, static_cast<float>(m_Size.y));
                    m_Scroll->setLowValue(m_Size.y);
                    m_Scroll->setMaximum(m_Items.size() * m_ItemHeight);
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section ListBox in " + m_LoadedConfigFile + ".");
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& ListBox::getLoadedConfigFile() const
    {
        return m_LoadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setSize(float width, float height)
    {
        // A negative size is not allowed for this widget
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // There is a minimum width
        if (m_Scroll == nullptr)
            width = TGUI_MAXIMUM(50.f, width);
        else
            width = TGUI_MAXIMUM(50.f + m_Scroll->getSize().x, width);

        // There is also a minimum list box height
        if (height < m_ItemHeight)
            height = static_cast<float>(m_ItemHeight);

        // Store the values
        m_Size.x = static_cast<unsigned int>(width);
        m_Size.y = static_cast<unsigned int>(height);

        // If there is a scrollbar then reinitialize it
        if (m_Scroll != nullptr)
        {
            m_Scroll->setSize(m_Scroll->getSize().x, static_cast<float>(m_Size.y));
            m_Scroll->setLowValue(m_Size.y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ListBox::getSize() const
    {
        return sf::Vector2f(static_cast<float>(m_Size.x), static_cast<float>(m_Size.y));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ListBox::getFullSize() const
    {
        return sf::Vector2f(getSize().x + m_LeftBorder + m_RightBorder,
                            getSize().y + m_TopBorder + m_BottomBorder);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::changeColors(const sf::Color& backgroundColor,         const sf::Color& textColor,
                               const sf::Color& selectedBackgroundColor, const sf::Color& selectedTextColor,
                               const sf::Color& borderColor)
    {
        m_BackgroundColor         = backgroundColor;
        m_TextColor               = textColor;
        m_SelectedBackgroundColor = selectedBackgroundColor;
        m_SelectedTextColor       = selectedTextColor;
        m_BorderColor             = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_BackgroundColor = backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setTextColor(const sf::Color& textColor)
    {
        m_TextColor = textColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setSelectedBackgroundColor(const sf::Color& selectedBackgroundColor)
    {
        m_SelectedBackgroundColor = selectedBackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setSelectedTextColor(const sf::Color& selectedTextColor)
    {
        m_SelectedTextColor = selectedTextColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setBorderColor(const sf::Color& borderColor)
    {
        m_BorderColor = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ListBox::getBackgroundColor() const
    {
        return m_BackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ListBox::getTextColor() const
    {
        return m_TextColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ListBox::getSelectedBackgroundColor() const
    {
        return m_SelectedBackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ListBox::getSelectedTextColor() const
    {
        return m_SelectedTextColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ListBox::getBorderColor() const
    {
        return m_BorderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setTextFont(const sf::Font& font)
    {
        m_TextFont = &font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* ListBox::getTextFont() const
    {
        return m_TextFont;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ListBox::addItem(const sf::String& itemName, int id)
    {
        // Check if the item limit is reached (if there is one)
        if ((m_MaxItems == 0) || (m_Items.size() < m_MaxItems))
        {
            // If there is no scrollbar then there is another limit
            if (m_Scroll == nullptr)
            {
                // Calculate the amount of items that fit in the list box
                unsigned int maximumItems = m_Size.y / m_ItemHeight;

                // Check if the item still fits in the list box
                if (m_Items.size() == maximumItems)
                    return -1;
            }

            // Add the item to the list
            m_Items.push_back(itemName);
            m_ItemIds.push_back(id);

            // If there is a scrollbar then tell it that another item was added
            if (m_Scroll != nullptr)
                m_Scroll->setMaximum(m_Items.size() * m_ItemHeight);

            // Return the item index
            return m_Items.size() - 1;
        }
        else // The item limit was reached
            return -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::setSelectedItem(const sf::String& itemName)
    {
        // Loop through all items
        for (unsigned int i = 0; i < m_Items.size(); ++i)
        {
            // Check if a match was found
            if (m_Items[i] == itemName)
            {
                // Select the item
                m_SelectedItem = static_cast<int>(i);

                // Move the scrollbar if needed
                if (m_Scroll)
                {
                    if (m_SelectedItem * getItemHeight() < m_Scroll->getValue())
                        m_Scroll->setValue(m_SelectedItem * getItemHeight());
                    else if ((m_SelectedItem + 1) * getItemHeight() > m_Scroll->getValue() + m_Scroll->getLowValue())
                        m_Scroll->setValue((m_SelectedItem + 1) * getItemHeight() - m_Scroll->getLowValue());
                }

                return true;
            }
        }

        // No match was found
        m_SelectedItem = -1;
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
        if (index > int(m_Items.size()-1))
        {
            m_SelectedItem = -1;
            return false;
        }

        // Select the item
        m_SelectedItem = index;

        // Move the scrollbar if needed
        if (m_Scroll)
        {
            if (m_SelectedItem * getItemHeight() < m_Scroll->getValue())
                m_Scroll->setValue(m_SelectedItem * getItemHeight());
            else if ((m_SelectedItem + 1) * getItemHeight() > m_Scroll->getValue() + m_Scroll->getLowValue())
                m_Scroll->setValue((m_SelectedItem + 1) * getItemHeight() - m_Scroll->getLowValue());
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::deselectItem()
    {
        m_SelectedItem = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItem(unsigned int index)
    {
        // The index can't be too high
        if (index > m_Items.size()-1)
            return false;

        // Remove the item
        m_Items.erase(m_Items.begin() + index);
        m_ItemIds.erase(m_ItemIds.begin() + index);

        // If there is a scrollbar then tell it that an item was removed
        if (m_Scroll != nullptr)
            m_Scroll->setMaximum(m_Items.size() * m_ItemHeight);

        // Check if the selected item should change
        if (m_SelectedItem == static_cast<int>(index))
            m_SelectedItem = -1;
        else if (m_SelectedItem > static_cast<int>(index))
            --m_SelectedItem;

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::removeItem(const sf::String& itemName)
    {
        // Loop through all items
        for (unsigned int i = 0; i < m_Items.size(); ++i)
        {
            // When the name matches then delete the item
            if (m_Items[i] == itemName)
            {
                m_Items.erase(m_Items.begin() + i);
                m_ItemIds.erase(m_ItemIds.begin() + i);

                // Check if the selected item should change
                if (m_SelectedItem == static_cast<int>(i))
                    m_SelectedItem = -1;
                else if (m_SelectedItem > static_cast<int>(i))
                    --m_SelectedItem;

                // If there is a scrollbar then tell it that an item was removed
                if (m_Scroll != nullptr)
                    m_Scroll->setMaximum(m_Items.size() * m_ItemHeight);

                return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListBox::removeItemsById(int id)
    {
        unsigned int removedItems = 0;

        for (unsigned int i = 0; i < m_Items.size();)
        {
            if (m_ItemIds[i] == id)
            {
                m_Items.erase(m_Items.begin() + i);
                m_ItemIds.erase(m_ItemIds.begin() + i);

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
        m_Items.clear();
        m_ItemIds.clear();

        // Unselect any selected item
        m_SelectedItem = -1;

        // If there is a scrollbar then tell it that all item were removed
        if (m_Scroll != nullptr)
            m_Scroll->setMaximum(0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ListBox::getItem(unsigned int index) const
    {
        // The index can't be too high
        if (index > m_Items.size()-1)
            return "";

        // Return the item
        return m_Items[index];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ListBox::getItemIndex(const sf::String& itemName) const
    {
        // Loop through all items
        for (unsigned int i = 0; i < m_Items.size(); ++i)
        {
            // When the name matches then return the index
            if (m_Items[i] == itemName)
                return i;
        }

        // No match was found
        return -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String>& ListBox::getItems()
    {
        return m_Items;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ListBox::getSelectedItem() const
    {
        if (m_SelectedItem == -1)
            return "";
        else
            return m_Items[m_SelectedItem];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ListBox::getSelectedItemIndex() const
    {
        return m_SelectedItem;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ListBox::getSelectedItemId() const
    {
        if (m_SelectedItem == -1)
            return 0;
        else
            return m_ItemIds[m_SelectedItem];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::changeItem(unsigned int index, const sf::String& newValue)
    {
        if (index >= m_Items.size()) {
            return false;
        }

        m_Items[index] = newValue;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListBox::changeItems(const sf::String& originalValue, const sf::String& newValue)
    {
        unsigned int amountChanged = 0;
        for (auto it = m_Items.begin(); it != m_Items.end(); ++it)
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
        auto idIt = m_ItemIds.begin();
        for (auto it = m_Items.begin(); it != m_Items.end(); ++it, ++idIt)
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

        // If the scrollbar was already created then delete it first
        if (m_Scroll != nullptr)
            delete m_Scroll;

        // load the scrollbar and check if it failed
        m_Scroll = new Scrollbar();
        if(m_Scroll->load(scrollbarConfigFileFilename) == false)
        {
            // The scrollbar couldn't be loaded so it must be deleted
            delete m_Scroll;
            m_Scroll = nullptr;

            return false;
        }
        else // The scrollbar was loaded successfully
        {
            // Initialize the scrollbar
            m_Scroll->setVerticalScroll(true);
            m_Scroll->setSize(m_Scroll->getSize().x, static_cast<float>(m_Size.y));
            m_Scroll->setLowValue(m_Size.y);
            m_Scroll->setMaximum(m_Items.size() * m_ItemHeight);

            return true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::removeScrollbar()
    {
        // Delete the scrollbar
        delete m_Scroll;
        m_Scroll = nullptr;

        // When the items no longer fit inside the list box then we need to remove some
        if ((m_Items.size() * m_ItemHeight) > m_Size.y)
        {
            // Calculate ho many items fit inside the list box
            m_MaxItems = m_Size.y / m_ItemHeight;

            // Remove the items that didn't fit inside the list box
            m_Items.erase(m_Items.begin() + m_MaxItems, m_Items.end());
            m_ItemIds.erase(m_ItemIds.begin() + m_MaxItems, m_ItemIds.end());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setItemHeight(unsigned int itemHeight)
    {
        // There is a minimum height
        if (itemHeight < 10)
            itemHeight = 10;

        // Set the new heights
        m_ItemHeight = itemHeight;
        m_TextSize   = static_cast<unsigned int>(itemHeight * 0.8f);

        // Some items might be removed when there is no scrollbar
        if (m_Scroll == nullptr)
        {
            // When the items no longer fit inside the list box then we need to remove some
            if ((m_Items.size() * m_ItemHeight) > m_Size.y)
            {
                // Calculate ho many items fit inside the list box
                m_MaxItems = m_Size.y / m_ItemHeight;

                // Remove the items that didn't fit inside the list box
                m_Items.erase(m_Items.begin() + m_MaxItems, m_Items.end());
                m_ItemIds.erase(m_ItemIds.begin() + m_MaxItems, m_ItemIds.end());
            }
        }
        else // There is a scrollbar
        {
            // Set the maximum of the scrollbar
            m_Scroll->setMaximum(m_Items.size() * m_ItemHeight);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListBox::getItemHeight() const
    {
        return m_ItemHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setMaximumItems(unsigned int maximumItems)
    {
        // Set the new limit
        m_MaxItems = maximumItems;

        // Check if we already passed the limit
        if ((m_MaxItems > 0) && (m_MaxItems < m_Items.size()))
        {
            // Remove the items that passed the limitation
            m_Items.erase(m_Items.begin() + m_MaxItems, m_Items.end());
            m_ItemIds.erase(m_ItemIds.begin() + m_MaxItems, m_ItemIds.end());

            // If there is a scrollbar then tell it that the number of items was changed
            if (m_Scroll != nullptr)
                m_Scroll->setMaximum(m_Items.size() * m_ItemHeight);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ListBox::getMaximumItems() const
    {
        return m_MaxItems;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setBorders(unsigned int leftBorder, unsigned int topBorder, unsigned int rightBorder, unsigned int bottomBorder)
    {
        m_LeftBorder   = leftBorder;
        m_TopBorder    = topBorder;
        m_RightBorder  = rightBorder;
        m_BottomBorder = bottomBorder;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        if (m_Scroll != nullptr)
            m_Scroll->setTransparency(m_Opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::mouseOnWidget(float x, float y)
    {
        // Get the current position
        sf::Vector2f position = getPosition();

        // Pass the event to the scrollbar (if there is one)
        if (m_Scroll != nullptr)
        {
            // Temporarily set the position of the scroll
            m_Scroll->setPosition(position.x + m_Size.x - m_Scroll->getSize().x, position.y);

            // Pass the event
            m_Scroll->mouseOnWidget(x, y);

            // Reset the position
            m_Scroll->setPosition(0, 0);
        }

        // Check if the mouse is on top of the list box
        if (getTransform().transformRect(sf::FloatRect(0, 0, static_cast<float>(m_Size.x), static_cast<float>(m_Size.y))).contains(x, y))
            return true;
        else // The mouse is not on top of the list box
        {
            if (m_MouseHover)
                mouseLeftWidget();

            m_MouseHover = false;
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::leftMousePressed(float x, float y)
    {
        // Set the mouse down flag to true
        m_MouseDown = true;

        // This will be true when the click didn't occur on the scrollbar
        bool clickedOnListBox = true;

        // If there is a scrollbar then pass the event
        if (m_Scroll != nullptr)
        {
            // Temporarily set the position of the scroll
            m_Scroll->setPosition(getPosition().x + m_Size.x - m_Scroll->getSize().x, getPosition().y);

            // Pass the event
            if (m_Scroll->mouseOnWidget(x, y))
            {
                m_Scroll->leftMousePressed(x, y);
                clickedOnListBox = false;
            }

            // Reset the position
            m_Scroll->setPosition(0, 0);
        }

        // If the click occured on the list box
        if (clickedOnListBox)
        {
            // Remember the old selected item
            int oldSelectedItem = m_SelectedItem;

            // Check if there is a scrollbar or whether it is hidden
            if ((m_Scroll != nullptr) && (m_Scroll->getLowValue() < m_Scroll->getMaximum()))
            {
                // Check if we clicked on the first (perhaps partially) visible item
                if (y - getPosition().y <= (m_ItemHeight - (m_Scroll->getValue() % m_ItemHeight)))
                {
                    // We clicked on the first visible item
                    m_SelectedItem = static_cast<int>(m_Scroll->getValue() / m_ItemHeight);
                }
                else // We didn't click on the first visible item
                {
                    // Calculate on what item we clicked
                    if ((m_Scroll->getValue() % m_ItemHeight) == 0)
                        m_SelectedItem = static_cast<int>((y - getPosition().y) / m_ItemHeight + (m_Scroll->getValue() / m_ItemHeight));
                    else
                        m_SelectedItem = static_cast<int>((((y - getPosition().y) - (m_ItemHeight - (m_Scroll->getValue() % m_ItemHeight))) / m_ItemHeight) + (m_Scroll->getValue() / m_ItemHeight) + 1);
                }
            }
            else // There is no scrollbar or it is not displayed
            {
                // Calculate on which item we clicked
                m_SelectedItem = static_cast<int>((y - getPosition().y) / m_ItemHeight);

                // When you clicked behind the last item then unselect the selected item
                if (m_SelectedItem > static_cast<int>(m_Items.size())-1)
                    m_SelectedItem = -1;
            }

            // Add the callback (if the user requested it)
            if ((oldSelectedItem != m_SelectedItem) && (m_CallbackFunctions[ItemSelected].empty() == false))
            {
                // When no item is selected then send an empty string, otherwise send the item
                if (m_SelectedItem < 0)
                    m_Callback.text  = "";
                else
                    m_Callback.text = m_Items[m_SelectedItem];

                m_Callback.value   = m_SelectedItem;
                m_Callback.trigger = ItemSelected;
                addCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::leftMouseReleased(float x, float y)
    {
        // If there is a scrollbar then pass it the event
        if (m_Scroll != nullptr)
        {
            // Remember the old scrollbar value
            unsigned int oldValue = m_Scroll->getValue();

            // Temporarily set the position of the scroll
            m_Scroll->setPosition(getPosition().x + (m_Size.x - m_Scroll->getSize().x), getPosition().y);

            // Pass the event
            m_Scroll->leftMouseReleased(x, y);

            // Reset the position
            m_Scroll->setPosition(0, 0);

            // Check if the scrollbar value was incremented (you have pressed on the down arrow)
            if (m_Scroll->getValue() == oldValue + 1)
            {
                // Decrement the value
                m_Scroll->setValue(m_Scroll->getValue()-1);

                // Scroll down with the whole item height instead of with a single pixel
                m_Scroll->setValue(m_Scroll->getValue() + m_ItemHeight - (m_Scroll->getValue() % m_ItemHeight));
            }
            else if (m_Scroll->getValue() == oldValue - 1) // Check if the scrollbar value was decremented (you have pressed on the up arrow)
            {
                // increment the value
                m_Scroll->setValue(m_Scroll->getValue()+1);

                // Scroll up with the whole item height instead of with a single pixel
                if (m_Scroll->getValue() % m_ItemHeight > 0)
                    m_Scroll->setValue(m_Scroll->getValue() - (m_Scroll->getValue() % m_ItemHeight));
                else
                    m_Scroll->setValue(m_Scroll->getValue() - m_ItemHeight);
            }
        }

        m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseMoved(float x, float y)
    {
        if (m_MouseHover == false)
            mouseEnteredWidget();

        m_MouseHover = true;

        // If there is a scrollbar then pass the event
        if (m_Scroll != nullptr)
        {
            // Temporarily set the position of the scroll
            m_Scroll->setPosition(getPosition().x + (m_Size.x - m_Scroll->getSize().x), getPosition().y);

            // Check if you are dragging the thumb of the scrollbar
            if ((m_Scroll->m_MouseDown) && (m_Scroll->m_MouseDownOnThumb))
            {
                // Pass the event, even when the mouse is not on top of the scrollbar
                m_Scroll->mouseMoved(x, y);
            }
            else // You are just moving the mouse
            {
                // When the mouse is on top of the scrollbar then pass the mouse move event
                if (m_Scroll->mouseOnWidget(x, y))
                    m_Scroll->mouseMoved(x, y);
            }

            // Reset the position
            m_Scroll->setPosition(0, 0);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseWheelMoved(int delta, int, int)
    {
        // Only do something when there is a scrollbar
        if (m_Scroll != nullptr)
        {
            if (m_Scroll->getLowValue() < m_Scroll->getMaximum())
            {
                // Check if you are scrolling down
                if (delta < 0)
                {
                    // Scroll down
                    m_Scroll->setValue(m_Scroll->getValue() + (static_cast<unsigned int>(-delta) * (m_ItemHeight / 2)));
                }
                else // You are scrolling up
                {
                    unsigned int change = static_cast<unsigned int>(delta) * (m_ItemHeight / 2);

                    // Scroll up
                    if (change < m_Scroll->getValue())
                        m_Scroll->setValue(m_Scroll->getValue() - change);
                    else
                        m_Scroll->setValue(0);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseNotOnWidget()
    {
        if (m_MouseHover)
            mouseLeftWidget();

        m_MouseHover = false;

        if (m_Scroll != nullptr)
            m_Scroll->m_MouseHover = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseNoLongerDown()
    {
        m_MouseDown = false;

        if (m_Scroll != nullptr)
            m_Scroll->m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::setProperty(std::string property, const std::string& value)
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
            setItemHeight(atoi(value.c_str()));
        }
        else if (property == "maximumitems")
        {
            setMaximumItems(atoi(value.c_str()));
        }
        else if (property == "borders")
        {
            Borders borders;
            if (extractBorders(value, borders))
                setBorders(borders.left, borders.top, borders.right, borders.bottom);
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'Borders' property.");
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
            setSelectedItem(atoi(value.c_str()));
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
            return Widget::setProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "backgroundcolor")
            value = "(" + to_string(int(getBackgroundColor().r)) + "," + to_string(int(getBackgroundColor().g)) + "," + to_string(int(getBackgroundColor().b)) + "," + to_string(int(getBackgroundColor().a)) + ")";
        else if (property == "textcolor")
            value = "(" + to_string(int(getTextColor().r)) + "," + to_string(int(getTextColor().g)) + "," + to_string(int(getTextColor().b)) + "," + to_string(int(getTextColor().a)) + ")";
        else if (property == "selectedbackgroundcolor")
            value = "(" + to_string(int(getSelectedBackgroundColor().r)) + "," + to_string(int(getSelectedBackgroundColor().g))
                    + "," + to_string(int(getSelectedBackgroundColor().b)) + "," + to_string(int(getSelectedBackgroundColor().a)) + ")";
        else if (property == "selectedtextcolor")
            value = "(" + to_string(int(getSelectedTextColor().r)) + "," + to_string(int(getSelectedTextColor().g))
                    + "," + to_string(int(getSelectedTextColor().b)) + "," + to_string(int(getSelectedTextColor().a)) + ")";
        else if (property == "bordercolor")
            value = "(" + to_string(int(getBorderColor().r)) + "," + to_string(int(getBorderColor().g)) + "," + to_string(int(getBorderColor().b)) + "," + to_string(int(getBorderColor().a)) + ")";
        else if (property == "itemheight")
            value = to_string(getItemHeight());
        else if (property == "maximumitems")
            value = to_string(getMaximumItems());
        else if (property == "borders")
            value = "(" + to_string(getBorders().left) + "," + to_string(getBorders().top) + "," + to_string(getBorders().right) + "," + to_string(getBorders().bottom) + ")";
        else if (property == "items")
            encodeList(m_Items, value);
        else if (property == "selecteditem")
            value = to_string(getSelectedItemIndex());
        else if (property == "callback")
        {
            std::string tempValue;
            Widget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_CallbackFunctions.find(ItemSelected) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(ItemSelected).size() == 1) && (m_CallbackFunctions.at(ItemSelected).front() == nullptr))
                callbacks.push_back("ItemSelected");

            encodeList(callbacks, value);

            if (value.empty() || tempValue.empty())
                value += tempValue;
            else
                value += "," + tempValue;
        }
        else // The property didn't match
            return Widget::getProperty(property, value);

        // You pass here when one of the properties matched
        return true;
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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::initialize(Container *const parent)
    {
        m_Parent = parent;
        setTextFont(m_Parent->getGlobalFont());
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

        if ((m_Scroll != nullptr) && (m_Scroll->getLowValue() < m_Scroll->getMaximum()))
        {
            topLeftPosition = sf::Vector2f(((getAbsolutePosition().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                           ((getAbsolutePosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top));
            bottomRightPosition = sf::Vector2f((getAbsolutePosition().x + m_Size.x - m_Scroll->getSize().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                               (getAbsolutePosition().y + m_Size.y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top));
        }
        else
        {
            topLeftPosition = sf::Vector2f(((getAbsolutePosition().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                           ((getAbsolutePosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top));
            bottomRightPosition = sf::Vector2f((getAbsolutePosition().x + m_Size.x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                               (getAbsolutePosition().y + m_Size.y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top));
        }

        // Adjust the transformation
        states.transform *= getTransform();

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Draw the borders
        {
            // Draw left border
            sf::RectangleShape border(sf::Vector2f(static_cast<float>(m_LeftBorder), static_cast<float>(m_Size.y + m_TopBorder)));
            border.setPosition(-static_cast<float>(m_LeftBorder), -static_cast<float>(m_TopBorder));
            border.setFillColor(m_BorderColor);
            target.draw(border, states);

            // Draw top border
            border.setSize(sf::Vector2f(static_cast<float>(m_Size.x + m_RightBorder), static_cast<float>(m_TopBorder)));
            border.setPosition(0, -static_cast<float>(m_TopBorder));
            target.draw(border, states);

            // Draw right border
            border.setSize(sf::Vector2f(static_cast<float>(m_RightBorder), static_cast<float>(m_Size.y + m_BottomBorder)));
            border.setPosition(static_cast<float>(m_Size.x), 0);
            target.draw(border, states);

            // Draw bottom border
            border.setSize(sf::Vector2f(static_cast<float>(m_Size.x + m_LeftBorder), static_cast<float>(m_BottomBorder)));
            border.setPosition(-static_cast<float>(m_LeftBorder), static_cast<float>(m_Size.y));
            target.draw(border, states);
        }

        // Draw the background
        sf::RectangleShape front(sf::Vector2f(static_cast<float>(m_Size.x), static_cast<float>(m_Size.y)));
        front.setFillColor(m_BackgroundColor);
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
        sf::Text text("", *m_TextFont, m_TextSize);

        // Check if there is a scrollbar and whether it isn't hidden
        if ((m_Scroll != nullptr) && (m_Scroll->getLowValue() < m_Scroll->getMaximum()))
        {
            // Store the transformation
            sf::Transform storedTransform = states.transform;

            // Find out which items should be drawn
            unsigned int firstItem = m_Scroll->getValue() / m_ItemHeight;
            unsigned int lastItem = (m_Scroll->getValue() + m_Scroll->getLowValue()) / m_ItemHeight;

            // Show another item when the scrollbar is standing between two items
            if ((m_Scroll->getValue() + m_Scroll->getLowValue()) % m_ItemHeight != 0)
                ++lastItem;

            // Set the clipping area
            glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

            for (unsigned int i = firstItem; i < lastItem; ++i)
            {
                // Restore the transformations
                states.transform = storedTransform;

                // Set the next item
                text.setString(m_Items[i]);

                // Get the global bounds
                sf::FloatRect bounds = text.getGlobalBounds();

                // Check if we are drawing the selected item
                if (m_SelectedItem == static_cast<int>(i))
                {
                    // Draw a background for the selected item
                    {
                        // Set a new transformation
                        states.transform.translate(0, (static_cast<float>(i * m_ItemHeight) - m_Scroll->getValue()));

                        // Create and draw the background
                        sf::RectangleShape back(sf::Vector2f(static_cast<float>(m_Size.x), static_cast<float>(m_ItemHeight)));
                        back.setFillColor(m_SelectedBackgroundColor);
                        target.draw(back, states);

                        // Restore the transformation
                        states.transform = storedTransform;
                    }

                    // Change the text color
                    text.setColor(m_SelectedTextColor);
                }
                else // Set the normal text color
                    text.setColor(m_TextColor);

                // Set the translation for the text
                states.transform.translate(2, std::floor(static_cast<float>(i * m_ItemHeight) - m_Scroll->getValue() + ((m_ItemHeight - bounds.height) / 2.0f) - bounds.top));

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

            for (unsigned int i = 0; i < m_Items.size(); ++i)
            {
                // Restore the transformations
                states.transform = storedTransform;

                // Set the next item
                text.setString(m_Items[i]);

                // Check if we are drawing the selected item
                if (m_SelectedItem == static_cast<int>(i))
                {
                    // Draw a background for the selected item
                    {
                        // Set a new transformation
                        states.transform.translate(0, static_cast<float>(i * m_ItemHeight));

                        // Create and draw the background
                        sf::RectangleShape back(sf::Vector2f(static_cast<float>(m_Size.x), static_cast<float>(m_ItemHeight)));
                        back.setFillColor(m_SelectedBackgroundColor);
                        target.draw(back, states);

                        // Restore the transformation
                        states.transform = storedTransform;
                    }

                    // Change the text color
                    text.setColor(m_SelectedTextColor);
                }
                else // Set the normal text color
                    text.setColor(m_TextColor);

                // Get the global bounds
                sf::FloatRect bounds = text.getGlobalBounds();

                // Set the translation for the text
                states.transform.translate(2, std::floor((i * m_ItemHeight) + ((m_ItemHeight - bounds.height) / 2.0f) - bounds.top));

                // Draw the text
                target.draw(text, states);
            }
        }

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);

        // Check if there is a scrollbar
        if (m_Scroll != nullptr)
        {
            // Reset the transformation
            states.transform = oldTransform;
            states.transform.translate(static_cast<float>(m_Size.x) - m_Scroll->getSize().x, 0);

            // Draw the scrollbar
            target.draw(*m_Scroll, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
