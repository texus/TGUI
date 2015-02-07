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

#include <SFML/OpenGL.hpp>

#include <TGUI/Scrollbar.hpp>
#include <TGUI/ListBox.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/ChildWindow.hpp>
#include <TGUI/SharedWidgetPtr.inl>
#include <TGUI/ComboBox.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox() :
    m_SeparateHoverImage(false),
    m_NrOfItemsToDisplay(0)
    {
        m_Callback.widgetType = Type_ComboBox;
        m_DraggableWidget = true;

        m_ListBox->hide();
        m_ListBox->setSize(50, 24);
        m_ListBox->setItemHeight(24);
        m_ListBox->changeColors();
        m_ListBox->bindCallback(&ComboBox::newItemSelectedCallbackFunction, this, ListBox::ItemSelected);
        m_ListBox->bindCallback(&ComboBox::listBoxUnfocusedCallbackFunction, this, ListBox::Unfocused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox(const ComboBox& copy) :
    Widget              (copy),
    WidgetBorders       (copy),
    m_LoadedConfigFile  (copy.m_LoadedConfigFile),
    m_SeparateHoverImage(copy.m_SeparateHoverImage),
    m_NrOfItemsToDisplay(copy.m_NrOfItemsToDisplay),
    m_ListBox           (copy.m_ListBox.clone())
    {
        m_ListBox->hide();
        m_ListBox->unbindAllCallback();
        m_ListBox->bindCallback(&ComboBox::newItemSelectedCallbackFunction, this, ListBox::ItemSelected);
        m_ListBox->bindCallback(&ComboBox::listBoxUnfocusedCallbackFunction, this, ListBox::Unfocused);

        TGUI_TextureManager.copyTexture(copy.m_TextureArrowUpNormal, m_TextureArrowUpNormal);
        TGUI_TextureManager.copyTexture(copy.m_TextureArrowUpHover, m_TextureArrowUpHover);
        TGUI_TextureManager.copyTexture(copy.m_TextureArrowDownNormal, m_TextureArrowDownNormal);
        TGUI_TextureManager.copyTexture(copy.m_TextureArrowDownHover, m_TextureArrowDownHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::~ComboBox()
    {
        if (m_TextureArrowUpNormal.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureArrowUpNormal);
        if (m_TextureArrowUpHover.data != nullptr)     TGUI_TextureManager.removeTexture(m_TextureArrowUpHover);
        if (m_TextureArrowDownNormal.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureArrowDownNormal);
        if (m_TextureArrowDownHover.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureArrowDownHover);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox& ComboBox::operator= (const ComboBox& right)
    {
        if (this != &right)
        {
            ComboBox temp(right);
            this->Widget::operator=(right);
            this->WidgetBorders::operator=(right);

            std::swap(m_LoadedConfigFile,       temp.m_LoadedConfigFile);
            std::swap(m_SeparateHoverImage,     temp.m_SeparateHoverImage);
            std::swap(m_NrOfItemsToDisplay,     temp.m_NrOfItemsToDisplay);
            std::swap(m_ListBox,                temp.m_ListBox);
            std::swap(m_TextureArrowUpNormal,   temp.m_TextureArrowUpNormal);
            std::swap(m_TextureArrowUpHover,    temp.m_TextureArrowUpHover);
            std::swap(m_TextureArrowDownNormal, temp.m_TextureArrowDownNormal);
            std::swap(m_TextureArrowDownHover,  temp.m_TextureArrowDownHover);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox* ComboBox::clone()
    {
        return new ComboBox(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::load(const std::string& configFileFilename)
    {
        m_LoadedConfigFile = getResourcePath() + configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // Remove all textures if they were loaded before
        if (m_TextureArrowUpNormal.data != nullptr)    TGUI_TextureManager.removeTexture(m_TextureArrowUpNormal);
        if (m_TextureArrowUpHover.data != nullptr)     TGUI_TextureManager.removeTexture(m_TextureArrowUpHover);
        if (m_TextureArrowDownNormal.data != nullptr)  TGUI_TextureManager.removeTexture(m_TextureArrowDownNormal);
        if (m_TextureArrowDownHover.data != nullptr)   TGUI_TextureManager.removeTexture(m_TextureArrowDownHover);

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
        if (!configFile.read("ComboBox", properties, values))
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

            if (property == "separatehoverimage")
            {
                m_SeparateHoverImage = configFile.readBool(value, false);
            }
            else if (property == "backgroundcolor")
            {
                setBackgroundColor(configFile.readColor(value));
            }
            else if (property == "textcolor")
            {
                setTextColor(configFile.readColor(value));
            }
            else if (property == "selectedbackgroundcolor")
            {
                setSelectedBackgroundColor(configFile.readColor(value));
            }
            else if (property == "selectedtextcolor")
            {
                setSelectedTextColor(configFile.readColor(value));
            }
            else if (property == "bordercolor")
            {
                setBorderColor(configFile.readColor(value));
            }
            else if (property == "borders")
            {
                Borders borders;
                if (extractBorders(value, borders))
                    setBorders(borders.left, borders.top, borders.right, borders.bottom);
            }
            else if (property == "arrowupnormalimage")
            {
                if (!configFile.readTexture(value, getResourcePath() + configFileFolder, m_TextureArrowUpNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowUpNormalImage in section ComboBox in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "arrowuphoverimage")
            {
                if (!configFile.readTexture(value, getResourcePath() + configFileFolder, m_TextureArrowUpHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowUpHoverImage in section ComboBox in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "arrowdownnormalimage")
            {
                if (!configFile.readTexture(value, getResourcePath() + configFileFolder, m_TextureArrowDownNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowDownNormalImage in section ComboBox in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "arrowdownhoverimage")
            {
                if (!configFile.readTexture(value, getResourcePath() + configFileFolder, m_TextureArrowDownHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowDownHoverImage in section ComboBox in " + m_LoadedConfigFile + ".");
                    return false;
                }
            }
            else if (property == "scrollbar")
            {
                if ((value.length() < 3) || (value[0] != '"') || (value[value.length()-1] != '"'))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for Scrollbar in section ComboBox in " + m_LoadedConfigFile + ".");
                    return false;
                }

                if (!m_ListBox->setScrollbar(configFileFolder + value.substr(1, value.length()-2)))
                    return false;
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section ComboBox in " + m_LoadedConfigFile + ".");
        }

        // Make sure the required textures were loaded
        if ((m_TextureArrowUpNormal.data == nullptr) || (m_TextureArrowDownNormal.data == nullptr))
        {
            TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the combo box. Is the ComboBox section in " + m_LoadedConfigFile + " complete?");
            return false;
        }

        // Check if optional textures were loaded
        if ((m_TextureArrowUpHover.data != nullptr) && (m_TextureArrowDownHover.data != nullptr))
        {
            m_WidgetPhase |= WidgetPhase_Hover;
        }

        // Remove all items (in case this is the second time that the load function was called)
        m_ListBox->removeAllItems();

        return m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& ComboBox::getLoadedConfigFile() const
    {
        return m_LoadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setSize(float width, float height)
    {
        // Don't set the scale when loading failed
        if (m_Loaded == false)
            return;

        // A negative size is not allowed for this widget
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // Set the height of the combo box
        m_ListBox->setItemHeight(TGUI_MAXIMUM(10, static_cast<unsigned int>(height)));

        // Set the size of the list box
        if (m_NrOfItemsToDisplay > 0)
            m_ListBox->setSize(width, static_cast<float>(m_ListBox->getItemHeight() * (TGUI_MINIMUM(m_NrOfItemsToDisplay, TGUI_MAXIMUM(m_ListBox->getItems().size(), 1)))));
        else
            m_ListBox->setSize(width, static_cast<float>(m_ListBox->getItemHeight() * TGUI_MAXIMUM(m_ListBox->getItems().size(), 1)));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ComboBox::getSize() const
    {
        if (m_Loaded)
            return sf::Vector2f(m_ListBox->getSize().x, static_cast<float>(m_ListBox->getItemHeight()));
        else
            return sf::Vector2f(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ComboBox::getFullSize() const
    {
        return sf::Vector2f(getSize().x + m_LeftBorder + m_RightBorder,
                            getSize().y + m_TopBorder + m_BottomBorder);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setItemsToDisplay(unsigned int nrOfItemsInList)
    {
        m_NrOfItemsToDisplay = nrOfItemsInList;

        if (m_NrOfItemsToDisplay < m_ListBox->m_Items.size())
            m_ListBox->setSize(m_ListBox->getSize().x, static_cast<float>(m_NrOfItemsToDisplay * m_ListBox->getItemHeight()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ComboBox::getItemsToDisplay() const
    {
        return m_NrOfItemsToDisplay;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::changeColors(const sf::Color& backgroundColor,         const sf::Color& textColor,
                                const sf::Color& selectedBackgroundColor, const sf::Color& selectedTextColor,
                                const sf::Color& borderColor)
    {
        m_ListBox->changeColors(backgroundColor, textColor, selectedBackgroundColor, selectedTextColor, borderColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_ListBox->setBackgroundColor(backgroundColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setTextColor(const sf::Color& textColor)
    {
        m_ListBox->setTextColor(textColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setSelectedBackgroundColor(const sf::Color& selectedBackgroundColor)
    {
        m_ListBox->setSelectedBackgroundColor(selectedBackgroundColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setSelectedTextColor(const sf::Color& selectedTextColor)
    {
        m_ListBox->setSelectedTextColor(selectedTextColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setBorderColor(const sf::Color& borderColor)
    {
        m_ListBox->setBorderColor(borderColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ComboBox::getBackgroundColor() const
    {
        return m_ListBox->getBackgroundColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ComboBox::getTextColor() const
    {
        return m_ListBox->getTextColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ComboBox::getSelectedBackgroundColor() const
    {
        return m_ListBox->getSelectedBackgroundColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ComboBox::getSelectedTextColor() const
    {
        return m_ListBox->getSelectedTextColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ComboBox::getBorderColor() const
    {
        return m_ListBox->getBorderColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setTextFont(const sf::Font& font)
    {
        m_ListBox->setTextFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* ComboBox::getTextFont() const
    {
        return m_ListBox->getTextFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setBorders(unsigned int leftBorder, unsigned int topBorder, unsigned int rightBorder, unsigned int bottomBorder)
    {
        // Set the new border size
        m_LeftBorder   = leftBorder;
        m_TopBorder    = topBorder;
        m_RightBorder  = rightBorder;
        m_BottomBorder = bottomBorder;
        m_ListBox->setBorders(m_LeftBorder, m_BottomBorder, m_RightBorder, m_BottomBorder);

        // Don't set the width and height when loading failed
        if (m_Loaded == false)
            return;

        // There is a minimum width
        if (m_ListBox->getSize().x < 50 + m_TextureArrowDownNormal.getSize().x)
            m_ListBox->setSize(50.0f + m_TextureArrowDownNormal.getSize().x, m_ListBox->getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ComboBox::addItem(const sf::String& item, int id)
    {
        // An item can only be added when the combo box was loaded correctly
        if (m_Loaded == false)
            return false;

        // Make room to add another item, until there are enough items
        if ((m_NrOfItemsToDisplay == 0) || (m_NrOfItemsToDisplay > m_ListBox->getItems().size()))
            m_ListBox->setSize(m_ListBox->getSize().x, static_cast<float>(m_ListBox->getItemHeight() * (m_ListBox->getItems().size() + 1)));

        // Add the item
        return m_ListBox->addItem(item, id);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItem(const sf::String& itemName)
    {
        return m_ListBox->setSelectedItem(itemName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItem(int index)
    {
        return m_ListBox->setSelectedItem(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::deselectItem()
    {
        m_ListBox->deselectItem();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItem(unsigned int index)
    {
        bool ret = m_ListBox->removeItem(index);

        // Shrink the list size
        if ((m_NrOfItemsToDisplay == 0) || (m_ListBox->getItems().size() < m_NrOfItemsToDisplay))
            m_ListBox->setSize(m_ListBox->getSize().x, static_cast<float>(m_ListBox->getItemHeight() * TGUI_MAXIMUM(m_ListBox->getItems().size(), 1)));

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItem(const sf::String& itemName)
    {
        bool ret = m_ListBox->removeItem(itemName);

        // Shrink the list size
        if ((m_NrOfItemsToDisplay == 0) || (m_ListBox->getItems().size() < m_NrOfItemsToDisplay))
            m_ListBox->setSize(m_ListBox->getSize().x, static_cast<float>(m_ListBox->getItemHeight() * TGUI_MAXIMUM(m_ListBox->getItems().size(), 1)));

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ComboBox::removeItemsById(int id)
    {
        unsigned int ret = m_ListBox->removeItemsById(id);

        // Shrink the list size
        if ((m_NrOfItemsToDisplay == 0) || (m_ListBox->getItems().size() < m_NrOfItemsToDisplay))
            m_ListBox->setSize(m_ListBox->getSize().x, static_cast<float>(m_ListBox->getItemHeight() * TGUI_MAXIMUM(m_ListBox->getItems().size(), 1)));

        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::removeAllItems()
    {
        m_ListBox->removeAllItems();
        m_ListBox->setSize(m_ListBox->getSize().x, static_cast<float>(m_ListBox->getItemHeight()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ComboBox::getItem(unsigned int index) const
    {
        return m_ListBox->getItem(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ComboBox::getItemIndex(const sf::String& itemName) const
    {
        return m_ListBox->getItemIndex(itemName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String>& ComboBox::getItems() const
    {
        return m_ListBox->getItems();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ComboBox::getSelectedItem() const
    {
        return m_ListBox->getSelectedItem();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ComboBox::getSelectedItemIndex() const
    {
        return m_ListBox->getSelectedItemIndex();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ComboBox::getSelectedItemId() const
    {
        return m_ListBox->getSelectedItemId();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::changeItem(unsigned int index, const sf::String& newValue)
    {
        return m_ListBox->changeItem(index, newValue);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ComboBox::changeItems(const sf::String& originalValue, const sf::String& newValue)
    {
        return m_ListBox->changeItems(originalValue, newValue);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ComboBox::changeItemsById(int id, const sf::String& newValue)
    {
        return m_ListBox->changeItemsById(id, newValue);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setScrollbar(const std::string& scrollbarConfigFileFilename)
    {
        return m_ListBox->setScrollbar(scrollbarConfigFileFilename);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::removeScrollbar()
    {
        m_ListBox->removeScrollbar();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setMaximumItems(unsigned int maximumItems)
    {
        m_ListBox->setMaximumItems(maximumItems);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ComboBox::getMaximumItems() const
    {
        return m_ListBox->getMaximumItems();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        m_ListBox->setTransparency(m_Opacity);

        m_TextureArrowUpNormal.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureArrowDownNormal.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureArrowUpHover.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        m_TextureArrowDownHover.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::mouseOnWidget(float x, float y)
    {
        // Don't do anything when the combo box wasn't loaded correctly
        if (m_Loaded == false)
            return false;

        // Get the current position
        sf::Vector2f position = getPosition();

        // Check if the mouse is on top of the combo box
        if ((x > position.x - m_LeftBorder) && (x < position.x + m_ListBox->getSize().x + m_RightBorder)
         && (y > position.y - m_TopBorder) && (y < position.y + m_ListBox->getItemHeight() + m_BottomBorder))
        {
            return true;
        }

        if (m_MouseHover)
            mouseLeftWidget();

        // The mouse is not on top of the combo box
        m_MouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::leftMousePressed(float, float)
    {
        m_MouseDown = true;

        // If the list wasn't visible then open it
        if (!m_ListBox->isVisible())
        {
            // Show the list
            showListBox();

            // Check if there is a scrollbar
            if (m_ListBox->m_Scroll != nullptr)
            {
                // If the selected item is not visible then change the value of the scrollbar
                if (m_NrOfItemsToDisplay > 0)
                {
                    if (static_cast<unsigned int>(m_ListBox->getSelectedItemIndex() + 1) > m_NrOfItemsToDisplay)
                        m_ListBox->m_Scroll->setValue((static_cast<unsigned int>(m_ListBox->getSelectedItemIndex()) - m_NrOfItemsToDisplay + 1) * m_ListBox->getItemHeight());
                    else
                        m_ListBox->m_Scroll->setValue(0);
                }
            }
        }
        else // This list was already open, so close it now
            hideListBox();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::leftMouseReleased(float, float)
    {
        m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::mouseWheelMoved(int delta, int, int)
    {
        // The list isn't visible
        if (!m_ListBox->isVisible())
        {
            // Check if you are scrolling down
            if (delta < 0)
            {
                // select the next item
                if (static_cast<unsigned int>(m_ListBox->getSelectedItemIndex() + 1) < m_ListBox->m_Items.size())
                    m_ListBox->setSelectedItem(static_cast<unsigned int>(m_ListBox->getSelectedItemIndex()+1));
            }
            else // You are scrolling up
            {
                // select the previous item
                if (m_ListBox->getSelectedItemIndex() > 0)
                    m_ListBox->setSelectedItem(static_cast<unsigned int>(m_ListBox->getSelectedItemIndex()-1));
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::mouseNoLongerDown()
    {
        m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "configfile")
        {
            load(value);
        }
        else if (property == "itemstodisplay")
        {
            setItemsToDisplay(atoi(value.c_str()));
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
        else if (property == "borders")
        {
            Borders borders;
            if (extractBorders(value, borders))
                setBorders(borders.left, borders.top, borders.right, borders.bottom);
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'Borders' property.");
        }
        else if (property == "maximumitems")
        {
            setMaximumItems(atoi(value.c_str()));
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

    bool ComboBox::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "itemstodisplay")
            value = to_string(getItemsToDisplay());
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
        else if (property == "borders")
            value = "(" + to_string(getBorders().left) + "," + to_string(getBorders().top) + "," + to_string(getBorders().right) + "," + to_string(getBorders().bottom) + ")";
        else if (property == "maximumitems")
            value = to_string(getMaximumItems());
        else if (property == "items")
            encodeList(m_ListBox->getItems(), value);
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

    std::list< std::pair<std::string, std::string> > ComboBox::getPropertyList() const
    {
        auto list = Widget::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("ConfigFile", "string"));
        list.push_back(std::pair<std::string, std::string>("ItemsToDisplay", "uint"));
        list.push_back(std::pair<std::string, std::string>("BackgroundColor", "color"));
        list.push_back(std::pair<std::string, std::string>("TextColor", "color"));
        list.push_back(std::pair<std::string, std::string>("SelectedBackgroundColor", "color"));
        list.push_back(std::pair<std::string, std::string>("SelectedTextColor", "color"));
        list.push_back(std::pair<std::string, std::string>("BorderColor", "color"));
        list.push_back(std::pair<std::string, std::string>("Borders", "borders"));
        list.push_back(std::pair<std::string, std::string>("MaximumItems", "uint"));
        list.push_back(std::pair<std::string, std::string>("Items", "string"));
        list.push_back(std::pair<std::string, std::string>("SelectedItem", "int"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::initialize(Container *const parent)
    {
        m_Parent = parent;
        setTextFont(m_Parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::showListBox()
    {
        if (!m_ListBox->isVisible())
        {
            m_ListBox->show();

            sf::Vector2f position(getPosition().x, getPosition().y + m_ListBox->getItemHeight() + m_BottomBorder);

            Widget* container = this;
            while (container->getParent() != nullptr)
            {
                container = container->getParent();
                position += container->getPosition();

                // Child window needs an exception
                if (container->getWidgetType() == Type_ChildWindow)
                {
                    ChildWindow* child = static_cast<ChildWindow*>(container);
                    position.x += child->getBorders().left;
                    position.y += child->getBorders().top + child->getTitleBarHeight();
                }
            }

            m_ListBox->setPosition(position);
            static_cast<Container*>(container)->add(m_ListBox);
            m_ListBox->focus();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::hideListBox()
    {
        // If the list was open then close it now
        if (m_ListBox->isVisible())
        {
            m_ListBox->hide();

            // Find the gui in order to remove the ListBox from it
            Widget* container = this;
            while (container->getParent() != nullptr)
                container = container->getParent();

            static_cast<Container*>(container)->remove(m_ListBox);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::newItemSelectedCallbackFunction()
    {
        if (m_CallbackFunctions[ItemSelected].empty() == false)
        {
            // When no item is selected then send an empty string, otherwise send the item
            m_Callback.text    = m_ListBox->getSelectedItem();
            m_Callback.value   = m_ListBox->getSelectedItemIndex();
            m_Callback.trigger = ItemSelected;
            addCallback();
        }

        hideListBox();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::listBoxUnfocusedCallbackFunction()
    {
        if (m_MouseHover == false)
            hideListBox();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw anything when the combo box was not loaded correctly
        if (m_Loaded == false)
            return;

        const sf::View& view = target.getView();

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / view.getSize().x;
        float scaleViewY = target.getSize().y / view.getSize().y;

        // Get the global position
        sf::Vector2f topLeftPosition = sf::Vector2f(((getAbsolutePosition().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                    ((getAbsolutePosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top));
        sf::Vector2f bottomRightPosition = sf::Vector2f((getAbsolutePosition().x + (m_ListBox->getSize().x - (m_TextureArrowDownNormal.getSize().x * (static_cast<float>(m_ListBox->getItemHeight()) / m_TextureArrowDownNormal.getSize().y)))
                                                         - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                                        (getAbsolutePosition().y + m_ListBox->getSize().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top));

        // Adjust the transformation
        states.transform *= getTransform();

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Draw left border
        sf::RectangleShape border(sf::Vector2f(static_cast<float>(m_LeftBorder), static_cast<float>(m_ListBox->getItemHeight() + m_TopBorder)));
        border.setPosition(-static_cast<float>(m_LeftBorder), -static_cast<float>(m_TopBorder));
        border.setFillColor(m_ListBox->m_BorderColor);
        target.draw(border, states);

        // Draw top border
        border.setSize(sf::Vector2f(static_cast<float>(m_ListBox->getSize().x + m_RightBorder), static_cast<float>(m_TopBorder)));
        border.setPosition(0, -static_cast<float>(m_TopBorder));
        target.draw(border, states);

        // Draw right border
        border.setSize(sf::Vector2f(static_cast<float>(m_RightBorder), static_cast<float>(m_ListBox->getItemHeight() + m_BottomBorder)));
        border.setPosition(static_cast<float>(m_ListBox->getSize().x), 0);
        target.draw(border, states);

        // Draw bottom border
        border.setSize(sf::Vector2f(m_ListBox->getSize().x + m_LeftBorder, static_cast<float>(m_BottomBorder)));
        border.setPosition(-static_cast<float>(m_LeftBorder), static_cast<float>(m_ListBox->getItemHeight()));
        target.draw(border, states);

        // Draw the combo box
        sf::RectangleShape front(sf::Vector2f(static_cast<float>(m_ListBox->getSize().x),
                                              static_cast<float>(m_ListBox->getItemHeight())));
        front.setFillColor(m_ListBox->getBackgroundColor());
        target.draw(front, states);

        // Create a text widget to draw it
        sf::Text tempText("kg", *m_ListBox->getTextFont());
        tempText.setCharacterSize(static_cast<unsigned int>(m_ListBox->getItemHeight() * 0.8f));
        tempText.setColor(m_ListBox->getTextColor());

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Calculate the clipping area
        GLint scissorLeft = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
        GLint scissorTop = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
        GLint scissorRight = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.x  * scaleViewX), scissor[0] + scissor[2]);
        GLint scissorBottom = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

        // If the widget outside the window then don't draw anything
        if (scissorRight < scissorLeft)
            scissorRight = scissorLeft;
        else if (scissorBottom < scissorTop)
            scissorTop = scissorBottom;

        // Set the clipping area
        glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

        // Draw the selected item
        states.transform.translate(2, std::floor((static_cast<int>(m_ListBox->getItemHeight()) - tempText.getLocalBounds().height) / 2.0f -  tempText.getLocalBounds().top));
        tempText.setString(m_ListBox->getSelectedItem());
        target.draw(tempText, states);

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);

        // Reset the transformations
        states.transform = oldTransform;

        // Set the arrow like it should (down when list box is invisible, up when it is visible)
        if (m_ListBox->isVisible())
        {
            float scaleFactor =  static_cast<float>(m_ListBox->getItemHeight()) / m_TextureArrowUpNormal.getSize().y;
            states.transform.translate(m_ListBox->getSize().x - (m_TextureArrowUpNormal.getSize().x * scaleFactor), 0);
            states.transform.scale(scaleFactor, scaleFactor);

            // Draw the arrow
            if (m_SeparateHoverImage)
            {
                if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                    target.draw(m_TextureArrowUpHover, states);
                else
                    target.draw(m_TextureArrowUpNormal, states);
            }
            else // There is no separate hover image
            {
                target.draw(m_TextureArrowUpNormal, states);

                if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                    target.draw(m_TextureArrowUpHover, states);
            }
        }
        else
        {
            float scaleFactor =  static_cast<float>(m_ListBox->getItemHeight()) / m_TextureArrowDownNormal.getSize().y;
            states.transform.translate(m_ListBox->getSize().x - (m_TextureArrowDownNormal.getSize().x * scaleFactor), 0);
            states.transform.scale(scaleFactor, scaleFactor);

            // Draw the arrow
            if (m_SeparateHoverImage)
            {
                if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                    target.draw(m_TextureArrowDownHover, states);
                else
                    target.draw(m_TextureArrowDownNormal, states);
            }
            else // There is no separate hover image
            {
                target.draw(m_TextureArrowDownNormal, states);

                if ((m_MouseHover) && (m_WidgetPhase & WidgetPhase_Hover))
                    target.draw(m_TextureArrowDownHover, states);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
