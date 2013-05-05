/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Objects.hpp>
#include <TGUI/Slider.hpp>
#include <TGUI/Scrollbar.hpp>
#include <TGUI/ListBox.hpp>

#include <SFML/OpenGL.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::ListBox() :
    m_SelectedItem           (0),
    m_Size                   (50, 100),
    m_ItemHeight             (24),
    m_TextSize               (20),
    m_MaxItems               (0),
    m_Scroll                 (NULL)
    {
        m_Callback.objectType = Type_ListBox;
        m_DraggableObject = true;
        m_Loaded = true;

        changeColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::ListBox(const ListBox& copy) :
    Object                   (copy),
    ObjectBorders            (copy),
    m_Items                  (copy.m_Items),
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
        if (copy.m_Scroll != NULL)
            m_Scroll = new Scrollbar(*copy.m_Scroll);
        else
            m_Scroll = NULL;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox::~ListBox()
    {
        if (m_Scroll != NULL)
            delete m_Scroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ListBox& ListBox::operator= (const ListBox& right)
    {
        if (this != &right)
        {
            ListBox temp(right);
            this->Object::operator=(right);
            this->ObjectBorders::operator=(right);

            // If there already was a scrollbar then delete it now
            if (m_Scroll != NULL)
            {
                delete m_Scroll;
                m_Scroll = NULL;
            }

            std::swap(m_Items,                   temp.m_Items);
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

    bool ListBox::load(const std::string& configFileFilename)
    {
        // If there already was a scrollbar then delete it now
        if (m_Scroll != NULL)
        {
            delete m_Scroll;
            m_Scroll = NULL;
        }

        // Open the config file
        ConfigFile configFile;
        if (!configFile.open(configFileFilename))
        {
            TGUI_OUTPUT("TGUI error: Failed to open " + configFileFilename + ".");
            return false;
        }

        // Read the properties and their values (as strings)
        std::vector<std::string> properties;
        std::vector<std::string> values;
        if (!configFile.read("ListBox", properties, values))
        {
            TGUI_OUTPUT("TGUI error: Failed to parse " + configFileFilename + ".");
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
                Vector4u borders;
                if (extractVector4u(value, borders))
                    setBorders(borders.x1, borders.x2, borders.x3, borders.x4);
            }
            else if (property == "scrollbar")
            {
                if ((value.length() < 3) || (value[0] != '"') || (value[value.length()-1] != '"'))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for Scrollbar in section ChatBox in " + configFileFilename + ".");
                    return false;
                }

                // load the scrollbar and check if it failed
                m_Scroll = new Scrollbar();
                if (m_Scroll->load(configFileFolder + value.substr(1, value.length()-2)) == false)
                {
                    // The scrollbar couldn't be loaded so it must be deleted
                    delete m_Scroll;
                    m_Scroll = NULL;

                    return false;
                }
                else // The scrollbar was loaded successfully
                {
                    // Initialize the scrollbar
                    m_Scroll->setVerticalScroll(true);
                    m_Scroll->setSize(m_Scroll->getSize().x, static_cast<float>(m_Size.y) - m_TopBorder - m_BottomBorder);
                    m_Scroll->setLowValue(m_Size.y - m_TopBorder - m_BottomBorder);
                    m_Scroll->setMaximum(m_Items.size() * m_ItemHeight);
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section ListBox in " + configFileFilename + ".");
        }

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::setSize(float width, float height)
    {
        // A negative size is not allowed for this object
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // The calculations require an unsigned integer
        unsigned int uiHeight = static_cast<unsigned int>(height);

        // There is a minimum width
        if (m_Scroll == NULL)
            width = TGUI_MAXIMUM(50.f + m_LeftBorder + m_RightBorder, width);
        else
            width = TGUI_MAXIMUM(50.f + m_LeftBorder + m_RightBorder + m_Scroll->getSize().x, width);

        // There is also a minimum list box height
        if (uiHeight < (m_ItemHeight + m_TopBorder + m_BottomBorder))
        {
            uiHeight = m_ItemHeight + m_TopBorder + m_BottomBorder;
        }
        else // The height isn't too low
        {
            // Calculate two perfect heights
            unsigned int height1 = ((uiHeight - m_TopBorder - m_BottomBorder) / m_ItemHeight) * m_ItemHeight;
            unsigned int height2 = (((uiHeight - m_TopBorder - m_BottomBorder) / m_ItemHeight) + 1) * m_ItemHeight;

            // Calculate the difference with the original one
            unsigned int difference1, difference2;

            if ((uiHeight - m_TopBorder - m_BottomBorder) > height1)
                difference1 = (uiHeight - m_TopBorder - m_BottomBorder) - height1;
            else
                difference1 = height1 - (uiHeight - m_TopBorder - m_BottomBorder);

            if ((uiHeight - m_TopBorder - m_BottomBorder) > height2)
                difference2 = (uiHeight - m_TopBorder - m_BottomBorder) - height2;
            else
                difference2 = height2 - (uiHeight - m_TopBorder - m_BottomBorder);

            // Find out which one is closest to the original height and adjust the height
            if (difference1 < difference2)
                uiHeight = height1 + m_TopBorder + m_BottomBorder;
            else
                uiHeight = height2 + m_TopBorder + m_BottomBorder;
        }

        // Store the values
        m_Size.x = static_cast<unsigned int>(width);
        m_Size.y = uiHeight;

        // If there is a scrollbar then reinitialize it
        if (m_Scroll != NULL)
        {
            m_Scroll->setSize(m_Scroll->getSize().x, static_cast<float>(m_Size.y) - m_TopBorder - m_BottomBorder);
            m_Scroll->setLowValue(m_Size.y - m_TopBorder - m_BottomBorder);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ListBox::getSize() const
    {
        return Vector2f(m_Size.x, m_Size.y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::changeColors(const sf::Color& backgroundColor,         const sf::Color& textColor,
                               const sf::Color& selectedBackgroundColor, const sf::Color& selectedTextColor,
                               const sf::Color& borderColor)
    {
        // Store the new colors
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
        m_TextFont = font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* ListBox::getTextFont() const
    {
        return &m_TextFont;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ListBox::addItem(const sf::String& itemName)
    {
        // Check if the item limit is reached (if there is one)
        if ((m_MaxItems == 0) || (m_Items.size() < m_MaxItems))
        {
            // If there is no scrollbar then there is another limit
            if (m_Scroll == NULL)
            {
                // Calculate the amount of items that fit in the list box
                unsigned int maximumItems = m_Size.y / m_ItemHeight;

                // Check if the item still fits in the list box
                if (m_Items.size() == maximumItems)
                    return -1;
            }

            // Add the item to the list
            m_Items.push_back(itemName);

            // If there is a scrollbar then tell it that another item was added
            if (m_Scroll != NULL)
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
        for (unsigned int i=0; i<m_Items.size(); ++i)
        {
            // Check if a match was found
            if (m_Items[i] == itemName)
            {
                // Select the item
                m_SelectedItem = static_cast<int>(i);
                return true;
            }
        }

        TGUI_OUTPUT("TGUI warning: Failed to select the item in the list box. The name didn't match any item.");

        // No match was found
        m_SelectedItem = -1;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::setSelectedItem(unsigned int index)
    {
        // If the index is too high then deselect the items
        if (index > m_Items.size()-1)
        {
            TGUI_OUTPUT("TGUI warning: Failed to select the item in the list box. The index was too high.");
            m_SelectedItem = -1;
            return false;
        }

        // Select the item
        m_SelectedItem = index;
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
        {
            TGUI_OUTPUT("TGUI warning: Failed to remove the item from the list box. The index was too high.");
            return false;
        }

        // Remove the item
        m_Items.erase(m_Items.begin() + index);

        // If there is a scrollbar then tell it that an item was removed
        if (m_Scroll != NULL)
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
        for (unsigned int i=0; i<m_Items.size(); ++i)
        {
            // When the name matches then delete the item
            if (m_Items[i] == itemName)
            {
                m_Items.erase(m_Items.begin() + i);

                // Check if the selected item should change
                if (m_SelectedItem == static_cast<int>(i))
                    m_SelectedItem = -1;
                else if (m_SelectedItem > static_cast<int>(i))
                    --m_SelectedItem;

                // If there is a scrollbar then tell it that an item was removed
                if (m_Scroll != NULL)
                    m_Scroll->setMaximum(m_Items.size() * m_ItemHeight);

                return true;
            }
        }

        TGUI_OUTPUT("TGUI warning: Failed to remove the item from the list box. The name didn't match any item.");
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::removeAllItems()
    {
        // Clear the list, remove all items
        m_Items.clear();

        // Unselect any selected item
        m_SelectedItem = -1;

        // If there is a scrollbar then tell it that all item were removed
        if (m_Scroll != NULL)
            m_Scroll->setMaximum(0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ListBox::getItem(unsigned int index) const
    {
        // The index can't be too high
        if (index > m_Items.size()-1)
        {
            TGUI_OUTPUT("TGUI warning: The index of the item was too high. Returning an empty string.");
            return "";
        }

        // Return the item
        return m_Items[index];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ListBox::getItemIndex(const sf::String& itemName) const
    {
        // Loop through all items
        for (unsigned int i=0; i<m_Items.size(); ++i)
        {
            // When the name matches then return the index
            if (m_Items[i] == itemName)
                return i;
        }

        // No match was found
        TGUI_OUTPUT("TGUI warning: The name didn't match any item. Returning -1 as item index.");
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

    bool ListBox::setScrollbar(const std::string& scrollbarConfigFileFilename)
    {
        // Calling setScrollbar with an empty string does the same as removeScrollbar
        if (scrollbarConfigFileFilename.empty() == true)
        {
            removeScrollbar();
            return true;
        }

        // If the scrollbar was already created then delete it first
        if (m_Scroll != NULL)
            delete m_Scroll;

        // load the scrollbar and check if it failed
        m_Scroll = new Scrollbar();
        if(m_Scroll->load(scrollbarConfigFileFilename) == false)
        {
            // The scrollbar couldn't be loaded so it must be deleted
            delete m_Scroll;
            m_Scroll = NULL;

            return false;
        }
        else // The scrollbar was loaded successfully
        {
            // Initialize the scrollbar
            m_Scroll->setVerticalScroll(true);
            m_Scroll->setSize(m_Scroll->getSize().x, static_cast<float>(m_Size.y) - m_TopBorder - m_BottomBorder);
            m_Scroll->setLowValue(m_Size.y - m_TopBorder - m_BottomBorder);
            m_Scroll->setMaximum(m_Items.size() * m_ItemHeight);

            return true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::removeScrollbar()
    {
        // Delete the scrollbar
        delete m_Scroll;
        m_Scroll = NULL;

        // When the items no longer fit inside the list box then we need to remove some
        if ((m_Items.size() * m_ItemHeight) > m_Size.y)
        {
            // Calculate ho many items fit inside the list box
            m_MaxItems = m_Size.y / m_ItemHeight;

            // Remove the items that didn't fit inside the list box
            m_Items.erase(m_Items.begin() + m_MaxItems, m_Items.end());
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

        // Adjust the height a little bit
        setSize(static_cast<float>(m_Size.x), static_cast<float>(m_Size.y));

        // Some items might be removed when there is no scrollbar
        if (m_Scroll == NULL)
        {
            // When the items no longer fit inside the list box then we need to remove some
            if ((m_Items.size() * m_ItemHeight) > m_Size.y)
            {
                // Calculate ho many items fit inside the list box
                m_MaxItems = m_Size.y / m_ItemHeight;

                // Remove the items that didn't fit inside the list box
                m_Items.erase(m_Items.begin() + m_MaxItems, m_Items.end());
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

            // If there is a scrollbar then tell it that the number of items was changed
            if (m_Scroll != NULL)
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
        // Set the new border size
        m_LeftBorder   = leftBorder;
        m_TopBorder    = topBorder;
        m_RightBorder  = rightBorder;
        m_BottomBorder = bottomBorder;

        // There is a minimum width
        if (m_Size.x < (50 + m_LeftBorder + m_RightBorder))
            m_Size.x = 50 + m_LeftBorder + m_RightBorder;

        // There is also a minimum height (when there is no scrollbar)
        if (m_Scroll == NULL)
        {
            // If there are items then they should still fit inside the list box
            if (m_Items.size() > 0)
            {
                if (m_Size.y < ((m_Items.size() * m_ItemHeight) - m_TopBorder - m_BottomBorder))
                    m_Size.y = (m_Items.size() * m_ItemHeight) - m_TopBorder - m_BottomBorder;
            }
            else // There are no items
            {
                // At least one item should fit inside the list box
                if (m_Size.y < (m_ItemHeight - m_TopBorder - m_BottomBorder))
                    m_Size.y = m_ItemHeight - m_TopBorder - m_BottomBorder;
            }
        }

        // Calculate two perfect heights
        unsigned int height1 = ((m_Size.y - m_TopBorder - m_BottomBorder) / m_ItemHeight) * m_ItemHeight;
        unsigned int height2 = (((m_Size.y - m_TopBorder - m_BottomBorder) / m_ItemHeight) + 1) * m_ItemHeight;

        // Calculate the difference with the original one
        unsigned int difference1, difference2;

        if ((m_Size.y - m_TopBorder - m_BottomBorder) > height1)
            difference1 = (m_Size.y - m_TopBorder - m_BottomBorder) - height1;
        else
            difference1 = height1 - (m_Size.y - m_TopBorder - m_BottomBorder);

        if ((m_Size.y - m_TopBorder - m_BottomBorder) > height2)
            difference2 = (m_Size.y - m_TopBorder - m_BottomBorder) - height2;
        else
            difference2 = height2 - (m_Size.y - m_TopBorder - m_BottomBorder);

        // Find out which one is closest to the original height and adjust the height
        if (difference1 < difference2)
            m_Size.y = height1 + m_TopBorder + m_BottomBorder;
        else
            m_Size.y = height2 + m_TopBorder + m_BottomBorder;

        // If there is a scrollbar then reinitialize it
        if (m_Scroll != NULL)
        {
            m_Scroll->setSize(m_Scroll->getSize().x, static_cast<float>(m_Size.y) - m_TopBorder - m_BottomBorder);
            m_Scroll->setLowValue(m_Size.y - m_TopBorder - m_BottomBorder);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ListBox::mouseOnObject(float x, float y)
    {
        // Get the current position and scale
        Vector2f position = getPosition();
        Vector2f curScale = getScale();

        // Pass the event to the scrollbar (if there is one)
        if (m_Scroll != NULL)
        {
            // Temporarily set the position and scale of the scroll
            m_Scroll->setPosition(position.x + ((m_Size.x - m_RightBorder - m_Scroll->getSize().x) * curScale.x), position.y + (m_TopBorder * curScale.y));
            m_Scroll->setScale(curScale);

            // Pass the event
            m_Scroll->mouseOnObject(x, y);

            // Reset the position and scale
            m_Scroll->setPosition(0, 0);
            m_Scroll->setScale(1, 1);
        }

        // Check if the mouse is on top of the list box
        if (getTransform().transformRect(sf::FloatRect(static_cast<float>(m_LeftBorder), static_cast<float>(m_TopBorder), static_cast<float>(m_Size.x - m_LeftBorder - m_RightBorder), static_cast<float>(m_Size.y - m_TopBorder - m_BottomBorder))).contains(x, y))
            return true;
        else // The mouse is not on top of the list box
        {
            if (m_MouseHover)
                mouseLeftObject();

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
        if (m_Scroll != NULL)
        {
            // Get the current scale
            Vector2f curScale = getScale();

            // Temporarily set the position and scale of the scroll
            m_Scroll->setPosition(getPosition().x + ((m_Size.x - m_RightBorder - m_Scroll->getSize().x) * curScale.x), getPosition().y + (m_TopBorder * curScale.y));
            m_Scroll->setScale(curScale);

            // Pass the event
            if (m_Scroll->mouseOnObject(x, y))
            {
                m_Scroll->leftMousePressed(x, y);
                clickedOnListBox = false;
            }

            // Reset the position and scale
            m_Scroll->setPosition(0, 0);
            m_Scroll->setScale(1, 1);
        }

        // If the click occured on the list box
        if (clickedOnListBox)
        {
            // Remember the old selected item
            int oldSelectedItem = m_SelectedItem;

            // Check if there is a scrollbar or whether it is hidden
            if ((m_Scroll != NULL) && (m_Scroll->getLowValue() < m_Scroll->getMaximum()))
            {
                // Check if we clicked on the first (perhaps partially) visible item
                if ((y - getPosition().y - m_TopBorder) / getScale().y <= (m_ItemHeight - (m_Scroll->getValue() % m_ItemHeight)))
                {
                    // We clicked on the first visible item
                    m_SelectedItem = static_cast<int>(m_Scroll->getValue() / m_ItemHeight);
                }
                else // We didn't click on the first visible item
                {
                    // Calculate on what item we clicked
                    if ((m_Scroll->getValue() % m_ItemHeight) == 0)
                        m_SelectedItem = static_cast<int>((y - getPosition().y - m_TopBorder) / (getScale().y * m_ItemHeight)) + (m_Scroll->getValue() / m_ItemHeight);
                    else
                        m_SelectedItem = static_cast<int>(((((y - getPosition().y - m_TopBorder) / getScale().y) - (m_ItemHeight - (m_Scroll->getValue() % m_ItemHeight))) / m_ItemHeight) + (m_Scroll->getValue() / m_ItemHeight) + 1);
                }
            }
            else // There is no scrollbar or it is not displayed
            {
                // Calculate on which item we clicked
                m_SelectedItem = static_cast<int>((y - getPosition().y - m_TopBorder) / (getScale().y * m_ItemHeight));

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
                    m_Callback.text  = m_Items[m_SelectedItem];

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
        if (m_Scroll != NULL)
        {
            // Remember the old scrollbar value
            unsigned int oldValue = m_Scroll->getValue();

            // Get the current scale
            Vector2f curScale = getScale();

            // Temporarily set the position and scale of the scroll
            m_Scroll->setPosition(getPosition().x + ((m_Size.x - m_RightBorder - m_Scroll->getSize().x) * curScale.x), getPosition().y + (m_TopBorder * curScale.y));
            m_Scroll->setScale(curScale);

            // Pass the event
            m_Scroll->leftMouseReleased(x, y);

            // Reset the position and scale
            m_Scroll->setPosition(0, 0);
            m_Scroll->setScale(1, 1);

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
            mouseEnteredObject();

        m_MouseHover = true;

        // Get the current scale
        Vector2f curScale = getScale();

        // If there is a scrollbar then pass the event
        if (m_Scroll != NULL)
        {
            // Temporarily set the position and scale of the scroll
            m_Scroll->setPosition(getPosition().x + ((m_Size.x - m_RightBorder - m_Scroll->getSize().x) * curScale.x), getPosition().y + (m_TopBorder * curScale.y));
            m_Scroll->setScale(curScale);

            // Check if you are dragging the thumb of the scrollbar
            if ((m_Scroll->m_MouseDown) && (m_Scroll->m_MouseDownOnThumb))
            {
                // Pass the event, even when the mouse is not on top of the scrollbar
                m_Scroll->mouseMoved(x, y);
            }
            else // You are just moving the mouse
            {
                // When the mouse is on top of the scrollbar then pass the mouse move event
                if (m_Scroll->mouseOnObject(x, y))
                    m_Scroll->mouseMoved(x, y);
            }

            // Reset the position and scale
            m_Scroll->setPosition(0, 0);
            m_Scroll->setScale(1, 1);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseWheelMoved(int delta)
    {
        // Only do something when there is a scrollbar
        if (m_Scroll != NULL)
        {
            if (m_Scroll->getLowValue() < m_Scroll->getMaximum())
            {
                // Check if you are scrolling down
                if (delta < 0)
                {
                    // Scroll down
                    m_Scroll->setValue( m_Scroll->getValue() + (static_cast<unsigned int>(-delta) * (m_ItemHeight / 2)) );
                }
                else // You are scrolling up
                {
                    unsigned int change = static_cast<unsigned int>(delta) * (m_ItemHeight / 2);

                    // Scroll up
                    if (change < m_Scroll->getValue())
                        m_Scroll->setValue( m_Scroll->getValue() - change );
                    else
                        m_Scroll->setValue(0);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseNotOnObject()
    {
        if (m_MouseHover)
            mouseLeftObject();

        m_MouseHover = false;

        if (m_Scroll != NULL)
            m_Scroll->m_MouseHover = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::mouseNoLongerDown()
    {
        m_MouseDown = false;

        if (m_Scroll != NULL)
            m_Scroll->m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::initialize(tgui::Group *const parent)
    {
        m_Parent = parent;
        setTextFont(m_Parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ListBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Get the current scale
        Vector2f curScale = getScale();

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / target.getView().getSize().x;
        float scaleViewY = target.getSize().y / target.getView().getSize().y;

        // Get the global position
        Vector2f topLeftPosition;
        Vector2f bottomRightPosition;

        Vector2f viewPosition = (target.getView().getSize() / 2.f) - target.getView().getCenter();

        if ((m_Scroll != NULL) && (m_Scroll->getLowValue() < m_Scroll->getMaximum()))
        {
            topLeftPosition = states.transform.transformPoint(getPosition() + Vector2f(m_LeftBorder * curScale.x, m_TopBorder * curScale.y) + viewPosition);
            bottomRightPosition = states.transform.transformPoint(getPosition().x + ((m_Size.x - m_RightBorder - m_Scroll->getSize().x) * curScale.x) + viewPosition.x, getPosition().y + ((m_Size.y - m_BottomBorder) * curScale.y) + viewPosition.y);
        }
        else
        {
            topLeftPosition = states.transform.transformPoint(getPosition() + Vector2f(m_LeftBorder * curScale.x, m_TopBorder * curScale.y) + viewPosition);
            bottomRightPosition = states.transform.transformPoint(getPosition() + Vector2f(m_Size.x * curScale.x, m_Size.y * curScale.y) - Vector2f(m_RightBorder * curScale.x, m_BottomBorder * curScale.y) + viewPosition);
        }

        // Adjust the transformation
        states.transform *= getTransform();

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Draw the borders
        {
            sf::RectangleShape back(Vector2f(static_cast<float>(m_Size.x), static_cast<float>(m_Size.y)));
            back.setFillColor(m_BorderColor);
            target.draw(back, states);
        }

        // Move the front rect a little bit
        states.transform.translate(static_cast<float>(m_LeftBorder), static_cast<float>(m_TopBorder));

        // Draw the background
        {
            sf::RectangleShape front(Vector2f(static_cast<float>(m_Size.x - m_LeftBorder - m_RightBorder),
                                              static_cast<float>(m_Size.y - m_TopBorder - m_BottomBorder)));
            front.setFillColor(m_BackgroundColor);
            target.draw(front, states);
        }

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Calculate the clipping area
        GLint scissorLeft = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
        GLint scissorTop = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
        GLint scissorRight = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.x * scaleViewX), scissor[0] + scissor[2]);
        GLint scissorBottom = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

        // If the object outside the window then don't draw anything
        if (scissorRight < scissorLeft)
            scissorRight = scissorLeft;
        else if (scissorBottom < scissorTop)
            scissorTop = scissorBottom;

        // Create a text object to draw the items
        sf::Text text("", m_TextFont, m_TextSize);

        // Check if there is a scrollbar and whether it isn't hidden
        if ((m_Scroll != NULL) && (m_Scroll->getLowValue() < m_Scroll->getMaximum()))
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

            for (unsigned int i=firstItem; i<lastItem; ++i)
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
                        states.transform.translate(0, (static_cast<float>(i * m_ItemHeight) - m_Scroll->getValue())).scale(curScale.x / curScale.y, 1);

                        // Create and draw the background
                        sf::RectangleShape back(Vector2f(static_cast<float>(m_Size.x - m_LeftBorder - m_RightBorder), static_cast<float>(m_ItemHeight)));
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

            for (unsigned int i=0; i<m_Items.size(); ++i)
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
                        states.transform.translate(0, static_cast<float>(i * m_ItemHeight)).scale(curScale.x / curScale.y, 1);

                        // Create and draw the background
                        sf::RectangleShape back(Vector2f(static_cast<float>(m_Size.x - m_LeftBorder - m_RightBorder), static_cast<float>(m_ItemHeight)));
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
        if (m_Scroll != NULL)
        {
            // Reset the transformation
            states.transform = oldTransform;
            states.transform.translate(static_cast<float>(m_Size.x) - m_RightBorder - m_Scroll->getSize().x, static_cast<float>(m_TopBorder));

            // Draw the scrollbar
            target.draw(*m_Scroll, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
