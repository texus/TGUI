/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (VDV_B@hotmail.com)
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


#include <TGUI/TGUI.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Listbox::Listbox() :
    m_SelectedItem           (0),
    m_Size                   (50, 100),
    m_ItemHeight             (24),
    m_TextSize               (20),
    m_MaxItems               (0),
    m_Scroll                 (NULL),
    m_LoadedScrollbarPathname("")
    {
        m_ObjectType = listbox;
        m_Loaded = true;

        changeColors();

        m_RenderTexture = new sf::RenderTexture();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Listbox::Listbox(const Listbox& copy) :
    OBJECT                   (copy),
    m_Items                  (copy.m_Items),
    m_SelectedItem           (copy.m_SelectedItem),
    m_Size                   (copy.m_Size),
    m_ItemHeight             (copy.m_ItemHeight),
    m_TextSize               (copy.m_TextSize),
    m_MaxItems               (copy.m_MaxItems),
    m_LoadedScrollbarPathname(copy.m_LoadedScrollbarPathname),
    m_BackgroundColor        (copy.m_BackgroundColor),
    m_TextColor              (copy.m_TextColor),
    m_SelectedBackgroundColor(copy.m_SelectedBackgroundColor),
    m_SelectedTextColor      (copy.m_SelectedTextColor),
    m_BorderColor            (copy.m_BorderColor),
    m_TextFont               (copy.m_TextFont)
    {
        // Copy the render texture
        if (m_RenderTexture->create(copy.m_RenderTexture->getSize().x, copy.m_RenderTexture->getSize().y) == false)
            m_Loaded = false;

        // If there is a scrollbar then copy it
        if (copy.m_Scroll != NULL)
            m_Scroll = new Scrollbar(*copy.m_Scroll);
        else
            m_Scroll = NULL;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Listbox::~Listbox()
    {
        if (m_Scroll != NULL)
            delete m_Scroll;

        delete m_RenderTexture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Listbox& Listbox::operator= (const Listbox& right)
    {
        if (this != &right)
        {
            Listbox temp(right);
            this->OBJECT::operator=(right);

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
            std::swap(m_LoadedScrollbarPathname, temp.m_LoadedScrollbarPathname);
            std::swap(m_BackgroundColor,         temp.m_BackgroundColor);
            std::swap(m_TextColor,               temp.m_TextColor);
            std::swap(m_SelectedBackgroundColor, temp.m_SelectedBackgroundColor);
            std::swap(m_SelectedTextColor,       temp.m_SelectedTextColor);
            std::swap(m_BorderColor,             temp.m_BorderColor);
            std::swap(m_TextFont,                temp.m_TextFont);
            std::swap(m_RenderTexture,           temp.m_RenderTexture);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Listbox::initialize(const sf::Font& globalFont)
    {
        setTextFont(globalFont);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Listbox::load(unsigned int width, unsigned int height, const std::string scrollbarPathname, unsigned int itemHeight)
    {
        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // If there already was a scrollbar then delete it now
        if (m_Scroll != NULL)
        {
            delete m_Scroll;
            m_Scroll = NULL;
        }

        // There is a minimum width
        if (width < (50 + m_LeftBorder + m_RightBorder))
            width = 50 + m_LeftBorder + m_RightBorder;

        unsigned int textSize;

        // Check if an item height was given
        if (itemHeight > 0)
        {
            // There is a minimum item height
            if (itemHeight < 10)
            {
                itemHeight = 10;
                textSize = 8;
            }
            else // the item height is ok, now calculate the text size
                textSize = static_cast<unsigned int>(itemHeight * 0.8f);

            // There is also a minimum listbox height
            if (height < (itemHeight + m_TopBorder + m_BottomBorder))
            {
                height = itemHeight + m_TopBorder + m_BottomBorder;
            }
            else // The height isn't too low
            {
                // Calculate two perfect heights
                unsigned int height1 = ((height - m_TopBorder - m_BottomBorder) / itemHeight) * itemHeight;
                unsigned int height2 = (((height - m_TopBorder - m_BottomBorder) / itemHeight) + 1) * itemHeight;

                // Calculate the difference with the original one
                unsigned int difference1, difference2;

                if ((height - m_TopBorder - m_BottomBorder) > height1)
                    difference1 = (height - m_TopBorder - m_BottomBorder) - height1;
                else
                    difference1 = height1 - (height - m_TopBorder - m_BottomBorder);

                if ((height - m_TopBorder - m_BottomBorder) > height2)
                    difference2 = (height - m_TopBorder - m_BottomBorder) - height2;
                else
                    difference2 = height2 - (height - m_TopBorder - m_BottomBorder);

                // Find out which one is closest to the original height and adjust the height
                if (difference1 < difference2)
                    height = height1 + m_TopBorder + m_BottomBorder;
                else
                    height = height2 + m_TopBorder + m_BottomBorder;
            }
        }
        else // no item height was given
        {
            // There is a minimum listbox height
            if (height < (100 + m_TopBorder + m_BottomBorder))
            {
                // Calculate the height
                height = 100 + m_TopBorder + m_BottomBorder;

                // Calculate the item height and text size
                itemHeight = height / 10;
                textSize = static_cast<unsigned int>(itemHeight * 0.8f);

                // Calculate the adjusted height
                height = m_ItemHeight * 10;
            }
            else // the height is ok
            {
                // Calculate the item height and text size
                itemHeight = height / 10;
                textSize = static_cast<unsigned int>(itemHeight * 0.8f);

                // Calculate the adjusted height
                height = m_ItemHeight * 10;
            }
        }

        // Store the values
        m_Size.x = width;
        m_Size.y = height;
        m_TextSize = textSize;
        m_ItemHeight = itemHeight;
        m_LoadedScrollbarPathname = scrollbarPathname;

        // Create the render texture
        if (m_RenderTexture->create(m_Size.x - m_LeftBorder - m_RightBorder, m_Size.y - m_TopBorder - m_BottomBorder) == false)
            return false;

        // If there is a scrollbar then load it
        if (scrollbarPathname.empty() == false)
        {
            // load the scrollbar and check if it failed
            m_Scroll = new Scrollbar();
            if(m_Scroll->load(scrollbarPathname) == false)
            {
                // The scrollbar couldn't be loaded so it must be deleted
                delete m_Scroll;
                m_Scroll = NULL;

                return false;
            }
            else // The scrollbar was loaded successfully
            {
                // The scrollbar has to be vertical
                m_Scroll->verticalScroll = true;

                // Set the low value
                m_Scroll->setLowValue(m_Size.y - m_TopBorder - m_BottomBorder);

                // Set the maximum
                m_Scroll->setMaximum(m_Items.size() * m_ItemHeight);

                m_Loaded = true;
                return true;
            }
        }
        else
        {
            m_Loaded = true;
            return true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Listbox::setSize(float width, float height)
    {
        // A negative size is not allowed for this object
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // The calculations require an unsigned integer
        unsigned int uiHeight = static_cast<unsigned int>(height);

        // There is a minimum width
        if (m_Scroll == NULL)
            width = TGUI_MAXIMUM(50 + m_LeftBorder + m_RightBorder, width);
        else
            width = TGUI_MAXIMUM(50 + m_LeftBorder + m_RightBorder + m_Scroll->m_TextureArrowNormal->getSize().x, width);

        // There is also a minimum listbox height
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

        // Create the render texture
        if (m_RenderTexture->create(m_Size.x - m_LeftBorder - m_RightBorder, m_Size.y - m_TopBorder - m_BottomBorder) == false)
            m_Loaded = false;

        // If there is a scrollbar then change it
        if (m_Scroll != NULL)
        {
            // Set the low value
            m_Scroll->setLowValue(m_Size.y - m_TopBorder - m_BottomBorder);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u Listbox::getSize() const
    {
        return m_Size;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Listbox::getScaledSize() const
    {
        return Vector2f(m_Size.x * getScale().x, m_Size.y * getScale().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Listbox::getLoadedScrollbarPathname()
    {
        return m_LoadedScrollbarPathname;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Listbox::changeColors(const sf::Color& backgroundColor,         const sf::Color& textColor,
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

    void Listbox::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_BackgroundColor = backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Listbox::setTextColor(const sf::Color& textColor)
    {
        m_TextColor = textColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Listbox::setSelectedBackgroundColor(const sf::Color& selectedBackgroundColor)
    {
        m_SelectedBackgroundColor = selectedBackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Listbox::setSelectedTextColor(const sf::Color& selectedTextColor)
    {
        m_SelectedTextColor = selectedTextColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Listbox::setBorderColor(const sf::Color& borderColor)
    {
        m_BorderColor = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Listbox::getBackgroundColor()
    {
        return m_BackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Listbox::getTextColor()
    {
        return m_TextColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Listbox::getSelectedBackgroundColor()
    {
        return m_SelectedBackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Listbox::getSelectedTextColor()
    {
        return m_SelectedTextColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Listbox::getBorderColor()
    {
        return m_BorderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Listbox::setTextFont(const sf::Font& font)
    {
        m_TextFont = font;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font& Listbox::getTextFont()
    {
        return m_TextFont;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Listbox::addItem(const std::string itemName, bool cropText)
    {
        // Check if the item limit is reached (if there is one)
        if ((m_MaxItems == 0) || (m_Items.size() < m_MaxItems))
        {
            // If there is a scrollbar then there is no limit
            if (m_Scroll != NULL)
            {
                goto addTheItem;
            }
            else // There is no scrollbar
            {
                // Calculate the amount of items that fit in the listbox
                unsigned int maximumItems = m_Size.y / m_ItemHeight;

                // Check if the item still fits in the listbox
                if (m_Items.size() < maximumItems)
                {
                    goto addTheItem;
                }
                else // The item no longer fits inside the listbox
                    return 0;
            }
        }
        else // The item limit was reached
            return 0;

      addTheItem:

        // Check if the string should be cropped when it is too long
        if (cropText)
        {
            // Create a copy of the string (because the original can't be changed)
            std::string itemNameCopy = itemName;

            // Create a text object
            sf::Text tempText(itemNameCopy);
            tempText.setCharacterSize(static_cast<unsigned int>(m_TextSize * getScale().y));

            // Calculate the maximum text width (the text must fit inside the listbox)
            float maximumTextWidth;

            if (m_Scroll == NULL)
                maximumTextWidth = (m_Size.x - (m_LeftBorder - m_RightBorder)) * getScale().x;
            else
                maximumTextWidth = ((m_Size.x - (m_LeftBorder - m_RightBorder)) * getScale().x) - m_Scroll->getSize().x;

            // Check if the text is too long to fit inside the listbox
            while (tempText.getGlobalBounds().width > maximumTextWidth)
            {
                // Make sure that the string is not empty already
                if (itemNameCopy.empty() == false)
                    break;

                // Remove the last character
                itemNameCopy.erase(itemNameCopy.length() -1);
                tempText.setString(itemNameCopy);
            }

            // Add the item to the list
            m_Items.push_back(itemNameCopy);
        }
        else // Just add the item to the list
            m_Items.push_back(itemName);

        // If there is a scrollbar then tell it that another item was added
        if (m_Scroll != NULL)
            m_Scroll->setMaximum(m_Items.size() * m_ItemHeight);

        // Return the item id
        return m_Items.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Listbox::setSelectedItem(const std::string itemName)
    {
        // Loop through all items
        for (unsigned int i=0; i<m_Items.size(); ++i)
        {
            // Check if a match was found
            if (m_Items[i].compare(itemName) == 0)
            {
                // Select the item
                m_SelectedItem = i + 1;
                return true;
            }
        }

        // No match was found
        m_SelectedItem = 0;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Listbox::setSelectedItem(unsigned int id)
    {
        // Make sure that the id is not too high
        if (id <= m_Items.size())
        {
            // Select the item
            m_SelectedItem = id;
            return true;
        }
        else // The id was too high
        {
            m_SelectedItem = 0;
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Listbox::removeItem(unsigned int id)
    {
        // If the id is valid then remove the item
        if (id <= m_Items.size())
        {
            m_Items.erase(m_Items.begin() + id - 1);

            // If there is a scrollbar then tell it that an item was removed
            if (m_Scroll != NULL)
                m_Scroll->setMaximum(m_Items.size() * m_ItemHeight);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Listbox::removeItem(const std::string itemName)
    {
        // Loop through all items
        for (unsigned int i=0; i<m_Items.size(); ++i)
        {
            // When the name matches then delete the item
            if (m_Items[i].compare(itemName))
            {
                m_Items.erase(m_Items.begin() + i);

                // If the removed item was selected then unselect it
                if (m_SelectedItem == i + 1)
                    m_SelectedItem = 0;

                // If the selected item was after the removed item then move it
                if (m_SelectedItem > i + 1)
                    --m_SelectedItem;

                // If there is a scrollbar then tell it that an item was removed
                if (m_Scroll != NULL)
                    m_Scroll->setMaximum(m_Items.size() * m_ItemHeight);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Listbox::removeAllItems()
    {
        // Clear the list, remove all items
        m_Items.clear();

        // Unselect any selected item
        m_SelectedItem = 0;

        // If there is a scrollbar then tell it that all item were removed
        if (m_Scroll != NULL)
            m_Scroll->setMaximum(0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Listbox::getItem(unsigned int id)
    {
        // Check if the id is valid
        if ((id > 0) && (id <= m_Items.size()))
        {
            // Return the item
            return m_Items[id - 1];
        }
        else // The id is outside the range
            return std::string("");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Listbox::getItemID(const std::string itemName)
    {
        // Loop through all items
        for (unsigned int i=0; i<m_Items.size(); ++i)
        {
            // When the name matches then return the IDd you have requested that you be notified on this event. You can view your new message by clicking on the following link:

            if (m_Items[i].compare(itemName))
                return i + 1;
        }

        // No match was found
        return 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::string>& Listbox::getItems()
    {
        return m_Items;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Listbox::getSelectedItem() const
    {
        // Make sure that an item is selected
        if (m_SelectedItem > 0)
        {
            // Make sure that the item exists
            if (m_Items.size() < m_SelectedItem)
                return "";

            // Return the item
            return m_Items[m_SelectedItem - 1];
        }
        else // If nothing is selected then return an empty string
            return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Listbox::getSelectedItemID()
    {
        return m_SelectedItem;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Listbox::setScrollbar(const std::string scrollbarPathname)
    {
        // Calling setScrollbar with an empty string does the same as removeScrollbar
        if (scrollbarPathname.empty() == true)
        {
            removeScrollbar();
            return true;
        }

        // If the scrollbar was already created then delete it first
        if (m_Scroll != NULL)
            delete m_Scroll;

        // Remember the scrollbar pathname
        m_LoadedScrollbarPathname = "";

        // load the scrollbar and check if it failed
        m_Scroll = new Scrollbar();
        if(m_Scroll->load(scrollbarPathname) == false)
        {
            // The scrollbar couldn't be loaded so it must be deleted
            delete m_Scroll;
            m_Scroll = NULL;

            return false;
        }
        else // The scrollbar was loaded successfully
        {
            // The scrollbar has to be vertical
            m_Scroll->verticalScroll = true;

            // Set the low value
            m_Scroll->setLowValue(m_Size.y - m_TopBorder - m_BottomBorder);

            return true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Listbox::removeScrollbar()
    {
        // There is no more scrollbar, so the string should be empty
        m_LoadedScrollbarPathname = "";

        // Delete the scrollbar
        delete m_Scroll;
        m_Scroll = NULL;

        // When the items no longer fit inside the listbox then we need to remove some
        if ((m_Items.size() * m_ItemHeight) > m_Size.y)
        {
            // Calculate ho many items fit inside the listbox
            m_MaxItems = m_Size.y / m_ItemHeight;

            // Remove the items that didn't fit inside the listbox
            m_Items.erase(m_Items.begin() + m_MaxItems, m_Items.end());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Listbox::setItemHeight(unsigned int itemHeight)
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
            // When the items no longer fit inside the listbox then we need to remove some
            if ((m_Items.size() * m_ItemHeight) > m_Size.y)
            {
                // Calculate ho many items fit inside the listbox
                m_MaxItems = m_Size.y / m_ItemHeight;

                // Remove the items that didn't fit inside the listbox
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

    unsigned int Listbox::getItemHeight()
    {
        return m_ItemHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Listbox::setMaximumItems(unsigned int maximumItems)
    {
        // Set the new limit
        m_MaxItems = maximumItems;

        // Check if we already passed the limit
        if ((m_MaxItems > 0) && (m_MaxItems < m_Items.size()))
        {
            // Copy the list of items
            std::vector<std::string> items = getItems();

            // Remove the items that passed the limitation
            m_Items.erase(m_Items.begin() + m_MaxItems, m_Items.end());

            // If there is a scrollbar then tell it that the number of items was changed
            if (m_Scroll != NULL)
                m_Scroll->setMaximum(m_Items.size() * m_ItemHeight);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Listbox::getMaximumItems()
    {
        return m_MaxItems;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Listbox::setBorders(unsigned int leftBorder, unsigned int topBorder, unsigned int rightBorder, unsigned int bottomBorder)
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
            // If there are items then they should still fit inside the listbox
            if (m_Items.size() > 0)
            {
                if (m_Size.y < ((m_Items.size() * m_ItemHeight) - m_TopBorder - m_BottomBorder))
                    m_Size.y = (m_Items.size() * m_ItemHeight) - m_TopBorder - m_BottomBorder;
            }
            else // There are no items
            {
                // At least one item should fit inside the listbox
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

        // Recreate the render texture
        if (m_RenderTexture->create(m_Size.x - m_LeftBorder - m_RightBorder, m_Size.y - m_TopBorder - m_BottomBorder) == false)
            m_Loaded = false;

        // Check if there is a scrollbar
        if (m_Scroll != NULL)
        {
            // Set the low value
            m_Scroll->setLowValue(m_Size.y - m_TopBorder - m_BottomBorder);

            // Tell the scrollbar how many items there are
            m_Scroll->setMaximum(m_Items.size() * m_ItemHeight);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Listbox::mouseOnObject(float x, float y)
    {
        // Get the current position and scale
        Vector2f position = getPosition();
        Vector2f curScale = getScale();

        // Pass the event to the scrollbar (if there is one)
        if (m_Scroll != NULL)
        {
            // Temporarily set the position and scale of the scroll
            m_Scroll->setPosition(position.x + ((m_Size.x - m_RightBorder) * curScale.x) - m_Scroll->getSize().x, position.y + (m_TopBorder * curScale.y));
            m_Scroll->setScale(1, (curScale.y * (m_Size.y- m_TopBorder - m_BottomBorder)) / m_Scroll->getSize().y);

            // Pass the event
            m_Scroll->mouseOnObject(x, y);

            // Reset the position and scale
            m_Scroll->setPosition(0, 0);
            m_Scroll->setScale(1, 1);
        }

        // Check if the mouse is on top of the listbox
        if (getTransform().transformRect(sf::FloatRect(static_cast<float>(m_LeftBorder), static_cast<float>(m_TopBorder), static_cast<float>(getSize().x - m_LeftBorder - m_RightBorder), static_cast<float>(getSize().y - m_TopBorder - m_BottomBorder))).contains(x, y))
            return true;
        else // The mouse is not on top of the listbox
        {
            m_MouseHover = false;
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Listbox::leftMousePressed(float x, float y)
    {
        // Set the mouse down flag to true
        m_MouseDown = true;

        // This will be true when the click didn't occur on the scrollbar
        bool clickedOnListbox = true;

        // If there is a scrollbar then pass the event
        if (m_Scroll != NULL)
        {
            // Get the current scale
            Vector2f curScale = getScale();

            // Temporarily set the position and scale of the scroll
            m_Scroll->setPosition(getPosition().x + ((m_Size.x - m_RightBorder) * curScale.x) - m_Scroll->getSize().x, getPosition().y + (m_TopBorder * curScale.y));
            m_Scroll->setScale(1, (curScale.y * (m_Size.y- m_TopBorder - m_BottomBorder)) / m_Scroll->getSize().y);

            // Pass the event
            if (m_Scroll->mouseOnObject(x, y))
            {
                m_Scroll->leftMousePressed(x, y);
                clickedOnListbox = false;
            }

            // Reset the position and scale
            m_Scroll->setPosition(0, 0);
            m_Scroll->setScale(1, 1);
        }

        // If the click occured on the listbox
        if (clickedOnListbox)
        {
            // Remember the old selected item
            unsigned int oldSelectedItem = m_SelectedItem;

            // Check if there is a scrollbar or whether it is hidden
            if ((m_Scroll != NULL) && (m_Scroll->m_LowValue < m_Scroll->m_Maximum))
            {
                // Check if we clicked on the first (perhaps partially) visible item
                if ((y - getPosition().y - m_TopBorder) <= (m_ItemHeight - (m_Scroll->m_Value % m_ItemHeight)))
                {
                    // We clicked on the first visible item
                    m_SelectedItem = m_Scroll->m_Value / m_ItemHeight + 1;
                }
                else // We didn't click on the first visible item
                {
                    // Calculate on what item we clicked
                    if ((m_Scroll->m_Value % m_ItemHeight) == 0)
                        m_SelectedItem = static_cast<unsigned int>(((y - getPosition().y - m_TopBorder) / m_ItemHeight) + (m_Scroll->m_Value / m_ItemHeight) + 1);
                    else
                        m_SelectedItem = static_cast<unsigned int>(((y - getPosition().y - m_TopBorder - (m_ItemHeight - (m_Scroll->m_Value % m_ItemHeight))) / m_ItemHeight) + (m_Scroll->m_Value / m_ItemHeight) + 2);
                }
            }
            else // There is no scrollbar or it is not displayed
            {
                // Calculate on which item we clicked
                m_SelectedItem = static_cast<unsigned int> ((y - getPosition().y - m_TopBorder) / (m_ItemHeight * getScale().y)) + 1;

                // When you clicked behind the last item then unselect the selected item
                if (m_SelectedItem > m_Items.size())
                    m_SelectedItem = 0;
            }


            // Add the callback (if the user requested it)
            if ((callbackID > 0) && (oldSelectedItem != m_SelectedItem))
            {
                Callback callback;
                callback.callbackID = callbackID;
                callback.trigger    = Callback::itemSelected;

                // When no item is selected then send an empty string
                if (m_SelectedItem == 0)
                {
                    callback.value = 0;
                    callback.text  = "";
                }
                else // an item was selected
                {
                    callback.value = m_SelectedItem;
                    callback.text  = m_Items[m_SelectedItem-1];
                }

                m_Parent->addCallback(callback);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Listbox::leftMouseReleased(float x, float y)
    {
        // If there is a scrollbar then pass it the event
        if (m_Scroll != NULL)
        {
            // Remember the old scrollbar value
            unsigned int oldValue = m_Scroll->m_Value;

            // Get the current scale
            Vector2f curScale = getScale();

            // Temporarily set the position and scale of the scroll
            m_Scroll->setPosition(getPosition().x + ((m_Size.x - m_RightBorder) * curScale.x) - m_Scroll->getSize().x, getPosition().y + (m_TopBorder * curScale.y));
            m_Scroll->setScale(1, (curScale.y * (m_Size.y- m_TopBorder - m_BottomBorder)) / m_Scroll->getSize().y);

            // Pass the event
            m_Scroll->leftMouseReleased(x, y);

            // Reset the position and scale
            m_Scroll->setPosition(0, 0);
            m_Scroll->setScale(1, 1);

            // Check if the scrollbar value was incremented (you have pressed on the down arrow)
            if (m_Scroll->m_Value == oldValue + 1)
            {
                // Decrement the value
                --m_Scroll->m_Value;

                // Scroll down with the whole item height instead of with a single pixel
                m_Scroll->setValue(m_Scroll->m_Value + m_ItemHeight - (m_Scroll->m_Value % m_ItemHeight));
            }
            else if (m_Scroll->m_Value == oldValue - 1) // Check if the scrollbar value was decremented (you have pressed on the up arrow)
            {
                // increment the value
                ++m_Scroll->m_Value;

                // Scroll up with the whole item height instead of with a single pixel
                if (m_Scroll->m_Value % m_ItemHeight > 0)
                    m_Scroll->setValue(m_Scroll->m_Value - (m_Scroll->m_Value % m_ItemHeight));
                else
                    m_Scroll->setValue(m_Scroll->m_Value - m_ItemHeight);
            }
        }

        m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Listbox::mouseMoved(float x, float y)
    {
        // Get the current scale
        Vector2f curScale = getScale();

        // If there is a scrollbar then pass the event
        if (m_Scroll != NULL)
        {
            // Temporarily set the position and scale of the scroll
            m_Scroll->setPosition(getPosition().x + ((m_Size.x - m_RightBorder) * curScale.x) - m_Scroll->getSize().x, getPosition().y + (m_TopBorder * curScale.y));
            m_Scroll->setScale(1, (curScale.y * (m_Size.y- m_TopBorder - m_BottomBorder)) / m_Scroll->getSize().y);

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

    void Listbox::mouseNotOnObject()
    {
        m_MouseHover = false;

        if (m_Scroll != NULL)
            m_Scroll->m_MouseHover = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Listbox::mouseNoLongerDown()
    {
        m_MouseDown = false;

        if (m_Scroll != NULL)
            m_Scroll->m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Listbox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Get the current position and scale
        Vector2f position = getPosition();
        Vector2f curScale = getScale();

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Adjust the transformation
        states.transform *= getTransform();

        // Draw the borders
        {
            sf::RectangleShape Back(Vector2f(static_cast<float>(m_Size.x), static_cast<float>(m_Size.y)));
            Back.setFillColor(m_BorderColor);
            target.draw(Back, states);
        }

        // Move the front rect a little bit
        states.transform.translate(static_cast<float>(m_LeftBorder), static_cast<float>(m_TopBorder));

        // Draw the listbox
        {
            sf::RectangleShape Front(Vector2f(static_cast<float>(m_Size.x - m_LeftBorder - m_RightBorder),
                                              static_cast<float>(m_Size.y - m_TopBorder - m_BottomBorder)));
            Front.setFillColor(m_BackgroundColor);
            target.draw(Front, states);
        }

        // Change the scale factors
        states.transform.scale(curScale.y / curScale.x, 1);

        // Create a text object to draw the items
        sf::Text text("", m_TextFont, m_TextSize);

        // Check if there is a scrollbar and whether it isn't hidden
        if ((m_Scroll != NULL) && (m_Scroll->m_LowValue < m_Scroll->m_Maximum))
        {
            // Store the transformation (without translation)
            states.transform = oldTransform;
            oldTransform.scale(curScale.x, curScale.y);
            sf::Transform storedTransform = states.transform;

            // Find out which items should be drawn
            unsigned int firstItem = m_Scroll->m_Value / m_ItemHeight;
            unsigned int lastItem = (m_Scroll->m_Value + m_Scroll->m_LowValue) / m_ItemHeight;

            // Show another item when the scrollbar is standing between two items
            if ((m_Scroll->m_Value + m_Scroll->m_LowValue) % m_ItemHeight != 0)
                ++lastItem;

            // Clear the render texture that we will be drawing on
            m_RenderTexture->clear(sf::Color::Transparent);

            for (unsigned int i=firstItem; i<lastItem; ++i)
            {
                // Restore the transformations
                states.transform = storedTransform;

                // Set the next item
                text.setString(m_Items[i]);

                // Get the global bounds
                sf::FloatRect bounds = text.getGlobalBounds();

                // Calculate the maximum text width (the text must fit inside the listbox)
                float maximumTextWidth = ((m_Size.x - m_LeftBorder - m_RightBorder - 4) * (curScale.x / curScale.y)) - (m_Scroll->getSize().x / curScale.y);

                // Check if the text is too long to fit inside the listbox
                while (bounds.width > maximumTextWidth)
                {
                    // Remove the last character
                    std::string tempString = text.getString().toAnsiString();
                    tempString.erase(tempString.length() -1);
                    text.setString(tempString);

                    // Recalculate the size
                    bounds = text.getGlobalBounds();
                }

                // Check if we are drawing the selected item
                if ((m_SelectedItem - 1) == i)
                {
                    // Draw a background for the selected item
                    {
                        // Set a new transformation
                        states.transform.translate(0, (static_cast<float>(static_cast<int>((i * m_ItemHeight) - m_Scroll->m_Value) + ((m_ItemHeight / 2.0f) - (bounds.height / 2.0f) - bounds.top)))).scale(curScale.x / curScale.y, 1);

                        // Create and draw the background
                        sf::RectangleShape Back(Vector2f(static_cast<float>(m_Size.x - m_LeftBorder - m_RightBorder), static_cast<float>(m_ItemHeight)));
                        Back.setFillColor(m_SelectedBackgroundColor);
                        m_RenderTexture->draw(Back, states);

                        // Restore the transformation
                        states.transform = storedTransform;
                    }

                    // Change the text color
                    text.setColor(m_SelectedTextColor);
                }
                else // Set the normal text color
                    text.setColor(m_TextColor);

                // Set the translation for the text
                states.transform.translate(2, (static_cast<float>(static_cast<int>((i * m_ItemHeight) - m_Scroll->m_Value) + ((m_ItemHeight / 2.0f) - (bounds.height / 2.0f) - bounds.top))));

                // Draw the text on the render texture
                m_RenderTexture->draw(text, states);
            }

            // Display the render texture
            m_RenderTexture->display();

            // Restore the old transformation
            states.transform = oldTransform;
            states.transform *= getTransform();
            states.transform.translate(static_cast<float>(m_LeftBorder), static_cast<float>(m_TopBorder));

            // Draw the text on the window
            sf::Sprite sprite(m_RenderTexture->getTexture());
            target.draw(sprite, states);
        }
        else // There is no scrollbar or it is invisible
        {
            // Store the current transformations
            sf::Transform storedTransform = states.transform;

            for (unsigned int i=0; i<m_Items.size(); ++i)
            {
                // Restore the transformations
                states.transform = storedTransform;

                // Set the next item
                text.setString(m_Items[i]);

                // Check if we are drawing the selected item
                if ((m_SelectedItem - 1) == i)
                {
                    // Draw a background for the selected item
                    {
                        // Set a new transformation
                        states.transform.translate(0, static_cast<float>(i * m_ItemHeight)).scale(curScale.x / curScale.y, 1);

                        // Create and draw the background
                        sf::RectangleShape Back(Vector2f(static_cast<float>(m_Size.x - m_LeftBorder - m_RightBorder), static_cast<float>(m_ItemHeight)));
                        Back.setFillColor(m_SelectedBackgroundColor);
                        target.draw(Back, states);

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

                // Calculate the maximum text width (the text must fit inside the listbox)
                float maximumTextWidth = (m_Size.x - m_LeftBorder - m_RightBorder - 4) * curScale.x / curScale.y;

                // Check if the text is too long to fit inside the listbox
                while (bounds.width > maximumTextWidth)
                {
                    // Remove the last character
                    std::string tempString = text.getString().toAnsiString();
                    tempString.erase(tempString.length() -1);
                    text.setString(tempString);

                    // Recalculate the size
                    bounds = text.getGlobalBounds();
                }

                // Set the translation for the text
                states.transform.translate(2, (i * m_ItemHeight) + ((m_ItemHeight / 2.0f) - (bounds.height / 2.0f) - bounds.top));

                // Draw the text
                target.draw(text, states);
            }
        }

        // Check if there is a scrollbar
        if (m_Scroll != NULL)
        {
            // Reset the transformation
            states.transform = oldTransform;
            states.transform.translate(position.x + ((m_Size.x - m_RightBorder) * curScale.x) - m_Scroll->getSize().x, position.y + (m_TopBorder * curScale.y));
            m_Scroll->setScale(1, (curScale.y * (m_Size.y - m_TopBorder - m_BottomBorder)) / m_Scroll->getSize().y);

            // Draw the scrollbar
            target.draw(*m_Scroll, states);

            // Reset the scale of the scrollbar
            m_Scroll->setScale(1, 1);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
