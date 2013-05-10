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
#include <TGUI/ClickableObject.hpp>
#include <TGUI/Slider.hpp>
#include <TGUI/Scrollbar.hpp>
#include <TGUI/ListBox.hpp>
#include <TGUI/ComboBox.hpp>

#include <SFML/OpenGL.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox() :
    m_SeparateHoverImage(false),
    m_ShowList          (false),
    m_MouseOnListBox    (false),
    m_NrOfItemsToDisplay(0)
    {
        m_Callback.objectType = Type_ComboBox;
        m_DraggableObject = true;

        m_ListBox = new ListBox();
        m_ListBox->setSize(50, 24);
        m_ListBox->setItemHeight(24);
        m_ListBox->changeColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox(const ComboBox& copy) :
    Object              (copy),
    ObjectBorders       (copy),
    m_SeparateHoverImage(copy.m_SeparateHoverImage),
    m_ShowList          (copy.m_ShowList),
    m_MouseOnListBox    (copy.m_MouseOnListBox),
    m_NrOfItemsToDisplay(copy.m_NrOfItemsToDisplay)
    {
        TGUI_TextureManager.copyTexture(copy.m_TextureArrowUpNormal, m_TextureArrowUpNormal);
        TGUI_TextureManager.copyTexture(copy.m_TextureArrowUpHover, m_TextureArrowUpHover);
        TGUI_TextureManager.copyTexture(copy.m_TextureArrowDownNormal, m_TextureArrowDownNormal);
        TGUI_TextureManager.copyTexture(copy.m_TextureArrowDownHover, m_TextureArrowDownHover);

        // Copy the list box
        m_ListBox = new tgui::ListBox(*copy.m_ListBox);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::~ComboBox()
    {
        if (m_TextureArrowUpNormal.data != NULL)    TGUI_TextureManager.removeTexture(m_TextureArrowUpNormal);
        if (m_TextureArrowUpHover.data != NULL)     TGUI_TextureManager.removeTexture(m_TextureArrowUpHover);
        if (m_TextureArrowDownNormal.data != NULL)  TGUI_TextureManager.removeTexture(m_TextureArrowDownNormal);
        if (m_TextureArrowDownHover.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureArrowDownHover);

        delete m_ListBox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox& ComboBox::operator= (const ComboBox& right)
    {
        if (this != &right)
        {
            ComboBox temp(right);
            this->Object::operator=(right);
            this->ObjectBorders::operator=(right);

            // Delete the old list box
            delete m_ListBox;

            std::swap(m_SeparateHoverImage,     temp.m_SeparateHoverImage);
            std::swap(m_ShowList,               temp.m_ShowList);
            std::swap(m_MouseOnListBox,         temp.m_MouseOnListBox);
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
        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // Remove all textures if they were loaded before
        if (m_TextureArrowUpNormal.data != NULL)    TGUI_TextureManager.removeTexture(m_TextureArrowUpNormal);
        if (m_TextureArrowUpHover.data != NULL)     TGUI_TextureManager.removeTexture(m_TextureArrowUpHover);
        if (m_TextureArrowDownNormal.data != NULL)  TGUI_TextureManager.removeTexture(m_TextureArrowDownNormal);
        if (m_TextureArrowDownHover.data != NULL)   TGUI_TextureManager.removeTexture(m_TextureArrowDownHover);

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
        if (!configFile.read("ComboBox", properties, values))
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
                Vector4u borders;
                if (extractVector4u(value, borders))
                    setBorders(borders.x1, borders.x2, borders.x3, borders.x4);
            }
            else if (property == "arrowupnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureArrowUpNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowUpNormalImage in section ComboBox in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "arrowuphoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureArrowUpHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowUpHoverImage in section ComboBox in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "arrowdownnormalimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureArrowDownNormal))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowDownNormalImage in section ComboBox in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "arrowdownhoverimage")
            {
                if (!configFile.readTexture(value, configFileFolder, m_TextureArrowDownHover))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for ArrowDownHoverImage in section ComboBox in " + configFileFilename + ".");
                    return false;
                }
            }
            else if (property == "scrollbar")
            {
                if ((value.length() < 3) || (value[0] != '"') || (value[value.length()-1] != '"'))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for Scrollbar in section ComboBox in " + configFileFilename + ".");
                    return false;
                }

                if (!m_ListBox->setScrollbar(configFileFolder + value.substr(1, value.length()-2)))
                    return false;
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section ComboBox in " + configFileFilename + ".");
        }

        // Make sure the required textures were loaded
        if ((m_TextureArrowUpNormal.data == NULL) || (m_TextureArrowDownNormal.data == NULL))
        {
            TGUI_OUTPUT("TGUI error: Not all needed images were loaded for the combo box. Is the ComboBox section in " + configFileFilename + " complete?");
            return false;
        }

        // Check if optional textures were loaded
        if ((m_TextureArrowUpHover.data != NULL) && (m_TextureArrowDownHover.data != NULL))
        {
            m_ObjectPhase |= ObjectPhase_Hover;
        }

        // Remove all items (in case this is the second time that the load function was called)
        m_ListBox->removeAllItems();

        return m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setSize(float width, float height)
    {
        // Don't set the scale when loading failed
        if (m_Loaded == false)
            return;

        // A negative size is not allowed for this object
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // Set the height of the combo box
        if (height > m_TopBorder + m_BottomBorder)
            m_ListBox->setItemHeight(static_cast<unsigned int>(height - m_TopBorder - m_BottomBorder));
        else
            m_ListBox->setItemHeight(10);

        // Set the size of the list box
        if (m_NrOfItemsToDisplay > 0)
            m_ListBox->setSize(width, height * (TGUI_MINIMUM(m_NrOfItemsToDisplay, m_ListBox->getItems().size())) - m_TopBorder);
        else
            m_ListBox->setSize(width, height * m_ListBox->getItems().size() - m_TopBorder);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f ComboBox::getSize() const
    {
        if (m_Loaded)
            return Vector2f(m_ListBox->getSize().x, m_ListBox->getItemHeight() + m_TopBorder + m_BottomBorder);
        else
            return Vector2f(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setItemsToDisplay(unsigned int nrOfItemsInList)
    {
        // At least one item must be shown
        if (nrOfItemsInList < 1)
            nrOfItemsInList = 1;

        // Make the change
        m_NrOfItemsToDisplay = nrOfItemsInList;
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
        // Calculate the new item height
        unsigned int itemHeight = m_ListBox->getItemHeight() + m_TopBorder + m_BottomBorder - topBorder - bottomBorder;

        // Set the new border size
        m_LeftBorder   = leftBorder;
        m_TopBorder    = topBorder;
        m_RightBorder  = rightBorder;
        m_BottomBorder = bottomBorder;
        m_ListBox->setBorders(m_LeftBorder, 0, m_RightBorder, m_BottomBorder);

        // Don't set the width and height when loading failed
        if (m_Loaded == false)
            return;

        // There is a minimum width
        if (m_ListBox->getSize().x < (50 + (m_LeftBorder + m_RightBorder + m_TextureArrowDownNormal.getSize().x) * getScale().x))
            m_ListBox->setSize(50 + (m_LeftBorder + m_RightBorder + m_TextureArrowDownNormal.getSize().x) * getScale().x, m_ListBox->getSize().y);

        // The item height needs to change
        m_ListBox->setItemHeight(itemHeight);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ComboBox::addItem(const sf::String& item)
    {
        // An item can only be added when the combo box was loaded correctly
        if (m_Loaded == false)
            return false;

        // Make room to add another item, until there are enough items
        if ((m_NrOfItemsToDisplay == 0) || (m_NrOfItemsToDisplay > m_ListBox->getItems().size()))
            m_ListBox->setSize(m_ListBox->getSize().x, (m_ListBox->getItemHeight() * getScale().y * (m_ListBox->getItems().size() + 1)) + m_BottomBorder);

        // Add the item
        return m_ListBox->addItem(item);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItem(const sf::String& itemName)
    {
        return m_ListBox->setSelectedItem(itemName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItem(unsigned int index)
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
        return m_ListBox->removeItem(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItem(const sf::String& itemName)
    {
        return m_ListBox->removeItem(itemName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::removeAllItems()
    {
        m_ListBox->removeAllItems();
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

    bool ComboBox::mouseOnObject(float x, float y)
    {
        // Don't do anything when the combo box wasn't loaded correctly
        if (m_Loaded == false)
            return false;

        // Get the current position and scale
        Vector2f position = getPosition();
        Vector2f curScale = getScale();

        if ((x > position.x) && (x < position.x + (m_ListBox->getSize().x * curScale.x)) && (y > position.y))
        {
            // Check if the mouse is on top of the combo box
            if (y < position.y + ((m_ListBox->getItemHeight() + m_TopBorder + m_BottomBorder) * curScale.y))
            {
                m_MouseOnListBox = false;
                m_ListBox->mouseNotOnObject();
                return true;
            }

            // Check if the list box is visible
            if (m_ShowList)
            {
                // Temporarily set the position and scale for the list box
                m_ListBox->setScale(curScale);
                m_ListBox->setPosition(position.x, position.y + ((m_ListBox->getItemHeight() + m_TopBorder + m_BottomBorder) * curScale.y));

                // Pass the event to the list box
                if (m_ListBox->mouseOnObject(x, y))
                {
                    // Reset the position and scale of the list box
                    m_ListBox->setPosition(0, 0);
                    m_ListBox->setScale(1, 1);

                    m_MouseOnListBox = true;
                    return true;
                }

                // Reset the position and scale of the list box
                m_ListBox->setPosition(0, 0);
                m_ListBox->setScale(1, 1);
            }
        }

        if (m_MouseHover)
            mouseLeftObject();

        // The mouse is not on top of the combo box
        m_MouseHover = false;
        m_ListBox->mouseNotOnObject();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::leftMousePressed(float x, float y)
    {
        if (m_Loaded == false)
            return;

        m_MouseDown = true;

        // Check if the list is visible and the mouse went down on top of the list box
        if ((m_ShowList == true) && (m_MouseOnListBox == true))
        {
            // Store the current selected item
            int oldItem = m_ListBox->getSelectedItemIndex();

            // Temporarily set the position and scale for the list box
            m_ListBox->setScale(getScale());
            m_ListBox->setPosition(getPosition().x, getPosition().y + ((m_ListBox->getItemHeight() + m_TopBorder + m_BottomBorder) * getScale().y));

            // Pass the event to the list box
            m_ListBox->leftMousePressed(x, y);

            // Reset the position and scale of the list box
            m_ListBox->setPosition(0, 0);
            m_ListBox->setScale(1, 1);

            // Add the callback (if the user requested it)
            if ((oldItem != m_ListBox->getSelectedItemIndex()) && (m_CallbackFunctions[ItemSelected].empty() == false))
            {
                m_Callback.trigger = ItemSelected;
                m_Callback.value   = m_ListBox->getSelectedItemIndex();
                m_Callback.text    = m_ListBox->getSelectedItem();
                addCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::leftMouseReleased(float x, float y)
    {
        if (m_Loaded == false)
            return;

        // Make sure that the mouse went down on top of the combo box
        if (m_MouseDown == true)
        {
            // Check if the list is visible
            if (m_ShowList == true)
            {
                // Check if the mouse is on top of the list box
                if (m_MouseOnListBox == true)
                {
                    // Check if the mouse went down on top of the list box
                    if (m_ListBox->m_MouseDown == true)
                    {
                        // Make sure the mouse didn't went down on the scrollbar
                        if ((m_ListBox->m_Scroll == NULL) || ((m_ListBox->m_Scroll != NULL) && (m_ListBox->m_Scroll->m_MouseDown == false)))
                        {
                            // Stop showing the list
                            m_ShowList = false;
                        }

                        // Temporarily set the position and scale for the list box
                        m_ListBox->setScale(getScale());
                        m_ListBox->setPosition(getPosition().x, getPosition().y + ((m_ListBox->getItemHeight() + m_TopBorder + m_BottomBorder) * getScale().y));

                        // Pass the event to the list box
                        m_ListBox->leftMouseReleased(x, y);

                        // Reset the position and scale of the list box
                        m_ListBox->setPosition(0, 0);
                        m_ListBox->setScale(1, 1);
                    }
                }
                else // The mouse is not on top of the list box
                    m_ShowList = false;
            }
            else // The list wasn't visible
            {
                // Show the list
                m_ShowList = true;

                // Check if there is a scrollbar
                if (m_ListBox->m_Scroll != NULL)
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

            // The mouse is no longer down
            m_MouseDown = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::mouseMoved(float x, float y)
    {
        // Don't do anything when the combo box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        if (m_MouseHover == false)
            mouseEnteredObject();

        m_MouseHover = true;

        // Check if the list is visible and the mouse is on top of the list box
        if ((m_ShowList == true) && (m_MouseOnListBox == true))
        {
            // Temporarily set the position and scale for the list box
            m_ListBox->setScale(getScale());
            m_ListBox->setPosition(getPosition().x, getPosition().y + ((m_ListBox->getItemHeight() + m_TopBorder + m_BottomBorder) * getScale().y));

            // Pass the event to the list box
            m_ListBox->mouseMoved(x, y);

            // Reset the position and scale of the list box
            m_ListBox->setPosition(0, 0);
            m_ListBox->setScale(1, 1);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::mouseWheelMoved(int delta)
    {
        // Check if the list is displayed
        if (m_ShowList)
        {
            // Only do something when there is a scrollbar
            if (m_ListBox->m_Scroll != NULL)
            {
                if (m_ListBox->m_Scroll->getLowValue() < m_ListBox->m_Scroll->getMaximum())
                {
                    // Check if you are scrolling down
                    if (delta < 0)
                    {
                        // Scroll down
                        m_ListBox->m_Scroll->setValue( m_ListBox->m_Scroll->getValue() + (static_cast<unsigned int>(-delta) * (m_ListBox->m_ItemHeight / 2)) );
                    }
                    else // You are scrolling up
                    {
                        unsigned int change = static_cast<unsigned int>(delta) * (m_ListBox->m_ItemHeight / 2);

                        // Scroll up
                        if (change < m_ListBox->m_Scroll->getValue())
                            m_ListBox->m_Scroll->setValue( m_ListBox->m_Scroll->getValue() - change );
                        else
                            m_ListBox->m_Scroll->setValue(0);
                    }
                }
            }
        }
        else // The list isn't visible
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
        // Make sure that the mouse went down on top of the combo box
        if (m_MouseDown == true)
        {
            // Check if the list is visible
            if (m_ShowList == true)
            {
                // Check if the mouse is on top of the list box
                if (m_MouseOnListBox == true)
                {
                    // Check if the mouse went down on top of the list box
                    if (m_ListBox->m_MouseDown == true)
                    {
                        // Check if the mouse went down on the scrollbar
                        if ((m_ListBox->m_Scroll != NULL) && (m_ListBox->m_Scroll->m_MouseDown == true))
                        {
                            m_MouseDown = false;
                            m_ListBox->mouseNotOnObject();
                            m_ListBox->mouseNoLongerDown();

                            // Don't hide the list
                            return;
                        }
                    }
                }
            }
        }

        m_MouseDown = false;
        m_ListBox->mouseNotOnObject();
        m_ListBox->mouseNoLongerDown();

        m_ShowList = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::initialize(tgui::Group *const parent)
    {
        m_Parent = parent;
        m_ListBox->setTextFont(m_Parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw anything when the combo box was not loaded correctly
        if (m_Loaded == false)
            return;

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / target.getView().getSize().x;
        float scaleViewY = target.getSize().y / target.getView().getSize().y;

        // Get the current scale
        Vector2f curScale = getScale();

        Vector2f viewPosition = (target.getView().getSize() / 2.f) - target.getView().getCenter();

        // Get the global position
        Vector2f topLeftPosition = states.transform.transformPoint(getPosition() + Vector2f(m_LeftBorder * curScale.x, m_TopBorder * curScale.y) + viewPosition);
        Vector2f bottomRightPosition = states.transform.transformPoint(getPosition().x + ((m_ListBox->getSize().x - m_RightBorder - (m_TextureArrowDownNormal.getSize().x * (static_cast<float>(m_ListBox->getItemHeight()) / m_TextureArrowDownNormal.getSize().y))) * curScale.x) + viewPosition.x,
                                                                       getPosition().y + ((m_ListBox->getSize().y - m_BottomBorder) * curScale.y) + viewPosition.y);

        // Adjust the transformation
        states.transform *= getTransform();

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Draw the borders
        sf::RectangleShape Back(Vector2f(static_cast<float>(m_ListBox->getSize().x), static_cast<float>(m_ListBox->getItemHeight() + m_TopBorder + m_BottomBorder)));
        Back.setFillColor(m_ListBox->getBorderColor());
        target.draw(Back, states);

        // Move the front rect a little bit
        states.transform.translate(static_cast<float>(m_LeftBorder), static_cast<float>(m_TopBorder));

        // Draw the combo box
        sf::RectangleShape Front(Vector2f(static_cast<float>(m_ListBox->getSize().x - m_LeftBorder - m_RightBorder),
                                          static_cast<float>(m_ListBox->getItemHeight() - ((m_TopBorder - m_BottomBorder) / curScale.y))));
        Front.setFillColor(m_ListBox->getBackgroundColor());
        target.draw(Front, states);

        // Create a text object to draw it
        sf::Text tempText("kg", *m_ListBox->getTextFont());
        tempText.setCharacterSize(m_ListBox->getItemHeight());
        tempText.setCharacterSize(static_cast<unsigned int>(tempText.getCharacterSize() - tempText.getLocalBounds().top));
        tempText.setColor(m_ListBox->getTextColor());

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Calculate the clipping area
        GLint scissorLeft = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
        GLint scissorTop = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
        GLint scissorRight = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.x  * scaleViewX), scissor[0] + scissor[2]);
        GLint scissorBottom = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

        // If the object outside the window then don't draw anything
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
        if (m_ShowList)
        {
            float scaleFactor =  static_cast<float>(m_ListBox->getItemHeight()) / m_TextureArrowUpNormal.getSize().y;
            states.transform.translate(m_ListBox->getSize().x - m_RightBorder - (m_TextureArrowUpNormal.getSize().x * scaleFactor), static_cast<float>(m_TopBorder));
            states.transform.scale(scaleFactor, scaleFactor);

            // Draw the arrow
            if (m_SeparateHoverImage)
            {
                if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover) && (m_MouseOnListBox == false))
                    target.draw(m_TextureArrowUpHover, states);
                else
                    target.draw(m_TextureArrowUpNormal, states);
            }
            else // There is no separate hover image
            {
                target.draw(m_TextureArrowUpNormal, states);

                if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover) && (m_MouseOnListBox == false))
                    target.draw(m_TextureArrowUpHover, states);
            }

            // Set the list box to the correct position and draw it
            states.transform = oldTransform.translate(0, static_cast<float>(m_ListBox->getItemHeight() + m_TopBorder + m_BottomBorder));
            target.draw(*m_ListBox, states);
        }
        else
        {
            float scaleFactor =  static_cast<float>(m_ListBox->getItemHeight()) / m_TextureArrowDownNormal.getSize().y;
            states.transform.translate(m_ListBox->getSize().x - m_RightBorder - (m_TextureArrowDownNormal.getSize().x * scaleFactor), static_cast<float>(m_TopBorder));
            states.transform.scale(scaleFactor, scaleFactor);

            // Draw the arrow
            if (m_SeparateHoverImage)
            {
                if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover) && (m_MouseOnListBox == false))
                    target.draw(m_TextureArrowDownHover, states);
                else
                    target.draw(m_TextureArrowDownNormal, states);
            }
            else // There is no separate hover image
            {
                target.draw(m_TextureArrowDownNormal, states);

                if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover) && (m_MouseOnListBox == false))
                    target.draw(m_TextureArrowDownHover, states);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
