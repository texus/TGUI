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
#include <TGUI/ListBox.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/ChildWindow.hpp>
#include <TGUI/SharedWidgetPtr.inl>
#include <TGUI/ComboBox.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox()
    {
        m_callback.widgetType = Type_ComboBox;
        m_draggableWidget = true;

        m_listBox->hide();
        m_listBox->setSize(50, 24);
        m_listBox->setItemHeight(24);
        m_listBox->changeColors();
        m_listBox->bindCallback(&ComboBox::newItemSelectedCallbackFunction, this, ListBox::ItemSelected);
        m_listBox->bindCallback(&ComboBox::listBoxUnfocusedCallbackFunction, this, ListBox::Unfocused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox(const ComboBox& copy) :
        Widget                  (copy),
        WidgetBorders           (copy),
        m_loadedConfigFile      (copy.m_loadedConfigFile),
        m_separateHoverImage    (copy.m_separateHoverImage),
        m_nrOfItemsToDisplay    (copy.m_nrOfItemsToDisplay),
        m_listBox               (copy.m_listBox.clone()),
        m_textureArrowUpNormal  (copy.m_textureArrowUpNormal),
        m_textureArrowUpHover   (copy.m_textureArrowUpHover),
        m_textureArrowDownNormal(copy.m_textureArrowDownNormal),
        m_textureArrowDownHover (copy.m_textureArrowDownHover)
    {
        m_listBox->hide();
        m_listBox->unbindAllCallback();
        m_listBox->bindCallback(&ComboBox::newItemSelectedCallbackFunction, this, ListBox::ItemSelected);
        m_listBox->bindCallback(&ComboBox::listBoxUnfocusedCallbackFunction, this, ListBox::Unfocused);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox& ComboBox::operator= (const ComboBox& right)
    {
        if (this != &right)
        {
            ComboBox temp(right);
            this->Widget::operator=(right);
            this->WidgetBorders::operator=(right);

            std::swap(m_loadedConfigFile,       temp.m_loadedConfigFile);
            std::swap(m_separateHoverImage,     temp.m_separateHoverImage);
            std::swap(m_nrOfItemsToDisplay,     temp.m_nrOfItemsToDisplay);
            std::swap(m_listBox,                temp.m_listBox);
            std::swap(m_textureArrowUpNormal,   temp.m_textureArrowUpNormal);
            std::swap(m_textureArrowUpHover,    temp.m_textureArrowUpHover);
            std::swap(m_textureArrowDownNormal, temp.m_textureArrowDownNormal);
            std::swap(m_textureArrowDownHover,  temp.m_textureArrowDownHover);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox* ComboBox::clone()
    {
        return new ComboBox(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::load(const std::string& configFileFilename)
    {
        m_loadedConfigFile = getResourcePath() + configFileFilename;

        // Remove all textures if they were loaded before
        if (m_textureArrowUpNormal.getData() != nullptr)    TGUI_TextureManager.removeTexture(m_textureArrowUpNormal);
        if (m_textureArrowUpHover.getData() != nullptr)     TGUI_TextureManager.removeTexture(m_textureArrowUpHover);
        if (m_textureArrowDownNormal.getData() != nullptr)  TGUI_TextureManager.removeTexture(m_textureArrowDownNormal);
        if (m_textureArrowDownHover.getData() != nullptr)   TGUI_TextureManager.removeTexture(m_textureArrowDownHover);

        // Open the config file
        ConfigFile configFile(m_loadedConfigFile, "ComboBox");

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = configFileFilename.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = configFileFilename.substr(0, slashPos+1);

        // Handle the read properties
        for (auto it = configFile.getProperties().cbegin(); it != configFile.getProperties().cend(); ++it)
        {
            if (it->first == "separatehoverimage")
            {
                m_separateHoverImage = configFile.readBool(it);
            }
            else if (it->first == "backgroundcolor")
            {
                setBackgroundColor(configFile.readColor(it));
            }
            else if (it->first == "textcolor")
            {
                setTextColor(configFile.readColor(it));
            }
            else if (it->first == "selectedbackgroundcolor")
            {
                setSelectedBackgroundColor(configFile.readColor(it));
            }
            else if (it->first == "selectedtextcolor")
            {
                setSelectedTextColor(configFile.readColor(it));
            }
            else if (it->first == "bordercolor")
            {
                setBorderColor(configFile.readColor(it));
            }
            else if (it->first == "borders")
            {
                Borders borders;
                if (extractBorders(it->second, borders))
                    setBorders(borders.left, borders.top, borders.right, borders.bottom);
                else
                    throw Exception("Failed to parse the 'Borders' property in section ComboBox in " + m_loadedConfigFile);
            }
            else if (it->first == "arrowupnormalimage")
            {
                configFile.readTexture(it, getResourcePath() + configFileFolder, m_textureArrowUpNormal);
            }
            else if (it->first == "arrowuphoverimage")
            {
                configFile.readTexture(it, getResourcePath() + configFileFolder, m_textureArrowUpHover);
            }
            else if (it->first == "arrowdownnormalimage")
            {
                configFile.readTexture(it, getResourcePath() + configFileFolder, m_textureArrowDownNormal);
            }
            else if (it->first == "arrowdownhoverimage")
            {
                configFile.readTexture(it, getResourcePath() + configFileFolder, m_textureArrowDownHover);
            }
            else if (it->first == "scrollbar")
            {
                if ((it->second.length() < 3) || (it->second[0] != '"') || (it->second[it->second.length()-1] != '"'))
                    throw Exception("Failed to parse value for Scrollbar in section ComboBox in " + m_loadedConfigFile + ".");

                m_listBox->setScrollbar(configFileFolder + it->second.substr(1, it->second.length()-2));
            }
            else
                throw Exception("Unrecognized property '" + it->first + "' in section ComboBox in " + m_loadedConfigFile + ".");
        }

        // Make sure the required textures were loaded
        if ((m_textureArrowUpNormal.getData() == nullptr) || (m_textureArrowDownNormal.getData() == nullptr))
            throw Exception("Not all needed images were loaded for the combo box. Is the ComboBox section in " + m_loadedConfigFile + " complete?");

        // Remove all items (in case this is the second time that the load function was called)
        m_listBox->removeAllItems();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& ComboBox::getLoadedConfigFile() const
    {
        return m_loadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setSize(float width, float height)
    {
        // A negative size is not allowed for this widget
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // Set the height of the combo box
        m_listBox->setItemHeight(TGUI_MAXIMUM(10, static_cast<unsigned int>(height)));

        // Set the size of the list box
        if (m_nrOfItemsToDisplay > 0)
            m_listBox->setSize(width, static_cast<float>(m_listBox->getItemHeight() * (TGUI_MINIMUM(m_nrOfItemsToDisplay, m_listBox->getItems().size()))));
        else
            m_listBox->setSize(width, static_cast<float>(m_listBox->getItemHeight() * m_listBox->getItems().size()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ComboBox::getSize() const
    {
        return sf::Vector2f(m_listBox->getSize().x, static_cast<float>(m_listBox->getItemHeight()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ComboBox::getFullSize() const
    {
        return sf::Vector2f(getSize().x + m_borders.left + m_borders.right,
                            getSize().y + m_borders.top + m_borders.bottom);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setItemsToDisplay(unsigned int nrOfItemsInList)
    {
        m_nrOfItemsToDisplay = nrOfItemsInList;

        if (m_nrOfItemsToDisplay < m_listBox->m_items.size())
            m_listBox->setSize(m_listBox->getSize().x, static_cast<float>(m_nrOfItemsToDisplay * m_listBox->getItemHeight()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ComboBox::getItemsToDisplay() const
    {
        return m_nrOfItemsToDisplay;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::changeColors(const sf::Color& backgroundColor,         const sf::Color& textColor,
                                const sf::Color& selectedBackgroundColor, const sf::Color& selectedTextColor,
                                const sf::Color& borderColor)
    {
        m_listBox->changeColors(backgroundColor, textColor, selectedBackgroundColor, selectedTextColor, borderColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_listBox->setBackgroundColor(backgroundColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setTextColor(const sf::Color& textColor)
    {
        m_listBox->setTextColor(textColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setSelectedBackgroundColor(const sf::Color& selectedBackgroundColor)
    {
        m_listBox->setSelectedBackgroundColor(selectedBackgroundColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setSelectedTextColor(const sf::Color& selectedTextColor)
    {
        m_listBox->setSelectedTextColor(selectedTextColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setBorderColor(const sf::Color& borderColor)
    {
        m_listBox->setBorderColor(borderColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ComboBox::getBackgroundColor() const
    {
        return m_listBox->getBackgroundColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ComboBox::getTextColor() const
    {
        return m_listBox->getTextColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ComboBox::getSelectedBackgroundColor() const
    {
        return m_listBox->getSelectedBackgroundColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ComboBox::getSelectedTextColor() const
    {
        return m_listBox->getSelectedTextColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ComboBox::getBorderColor() const
    {
        return m_listBox->getBorderColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setTextFont(const sf::Font& font)
    {
        m_listBox->setTextFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* ComboBox::getTextFont() const
    {
        return m_listBox->getTextFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setBorders(float leftBorder, float topBorder, float rightBorder, float bottomBorder)
    {
        // Set the new border size
        m_borders.left   = leftBorder;
        m_borders.top    = topBorder;
        m_borders.right  = rightBorder;
        m_borders.bottom = bottomBorder;
        m_listBox->setBorders(m_borders.left, m_borders.bottom, m_borders.right, m_borders.bottom);

        // There is a minimum width
        if (m_listBox->getSize().x < 50 + m_textureArrowDownNormal.getSize().x)
            m_listBox->setSize(50.0f + m_textureArrowDownNormal.getSize().x, m_listBox->getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ComboBox::addItem(const sf::String& item, int id)
    {
        // Make room to add another item, until there are enough items
        if ((m_nrOfItemsToDisplay == 0) || (m_nrOfItemsToDisplay > m_listBox->getItems().size()))
            m_listBox->setSize(m_listBox->getSize().x, static_cast<float>(m_listBox->getItemHeight() * (m_listBox->getItems().size() + 1)));

        // Add the item
        return m_listBox->addItem(item, id);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItem(const sf::String& itemName)
    {
        return m_listBox->setSelectedItem(itemName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItem(int index)
    {
        return m_listBox->setSelectedItem(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::deselectItem()
    {
        m_listBox->deselectItem();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItem(unsigned int index)
    {
        return m_listBox->removeItem(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItem(const sf::String& itemName)
    {
        return m_listBox->removeItem(itemName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ComboBox::removeItemsById(int id)
    {
        return m_listBox->removeItemsById(id);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::removeAllItems()
    {
        m_listBox->removeAllItems();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ComboBox::getItem(unsigned int index) const
    {
        return m_listBox->getItem(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ComboBox::getItemIndex(const sf::String& itemName) const
    {
        return m_listBox->getItemIndex(itemName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String>& ComboBox::getItems() const
    {
        return m_listBox->getItems();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String ComboBox::getSelectedItem() const
    {
        return m_listBox->getSelectedItem();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ComboBox::getSelectedItemIndex() const
    {
        return m_listBox->getSelectedItemIndex();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int ComboBox::getSelectedItemId() const
    {
        return m_listBox->getSelectedItemId();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setScrollbar(const std::string& scrollbarConfigFileFilename)
    {
        return m_listBox->setScrollbar(scrollbarConfigFileFilename);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::removeScrollbar()
    {
        m_listBox->removeScrollbar();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setMaximumItems(unsigned int maximumItems)
    {
        m_listBox->setMaximumItems(maximumItems);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ComboBox::getMaximumItems() const
    {
        return m_listBox->getMaximumItems();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        m_listBox->setTransparency(m_opacity);

        m_textureArrowUpNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureArrowDownNormal.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureArrowUpHover.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureArrowDownHover.setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::mouseOnWidget(float x, float y)
    {
        // Get the current position
        sf::Vector2f position = getPosition();

        // Check if the mouse is on top of the combo box
        if ((x > position.x - m_borders.left) && (x < position.x + m_listBox->getSize().x + m_borders.right)
         && (y > position.y - m_borders.top) && (y < position.y + m_listBox->getItemHeight() + m_borders.bottom))
        {
            return true;
        }

        if (m_mouseHover)
            mouseLeftWidget();

        // The mouse is not on top of the combo box
        m_mouseHover = false;
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::leftMousePressed(float, float)
    {
        m_mouseDown = true;

        // If the list wasn't visible then open it
        if (!m_listBox->isVisible())
        {
            // Show the list
            showListBox();

            // Check if there is a scrollbar
            if (m_listBox->m_scroll != nullptr)
            {
                // If the selected item is not visible then change the value of the scrollbar
                if (m_nrOfItemsToDisplay > 0)
                {
                    if (static_cast<unsigned int>(m_listBox->getSelectedItemIndex() + 1) > m_nrOfItemsToDisplay)
                        m_listBox->m_scroll->setValue((static_cast<unsigned int>(m_listBox->getSelectedItemIndex()) - m_nrOfItemsToDisplay + 1) * m_listBox->getItemHeight());
                    else
                        m_listBox->m_scroll->setValue(0);
                }
            }
        }
        else // This list was already open, so close it now
            hideListBox();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::leftMouseReleased(float, float)
    {
        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::mouseWheelMoved(int delta, int, int)
    {
        // The list isn't visible
        if (!m_listBox->isVisible())
        {
            // Check if you are scrolling down
            if (delta < 0)
            {
                // select the next item
                if (static_cast<unsigned int>(m_listBox->getSelectedItemIndex() + 1) < m_listBox->m_items.size())
                    m_listBox->setSelectedItem(static_cast<unsigned int>(m_listBox->getSelectedItemIndex()+1));
            }
            else // You are scrolling up
            {
                // select the previous item
                if (m_listBox->getSelectedItemIndex() > 0)
                    m_listBox->setSelectedItem(static_cast<unsigned int>(m_listBox->getSelectedItemIndex()-1));
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::mouseNoLongerDown()
    {
        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "configfile")
        {
            load(value);
        }
        else if (property == "itemstodisplay")
        {
            setItemsToDisplay(tgui::stoi(value));
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
                throw Exception("Failed to parse 'Borders' property.");
        }
        else if (property == "maximumitems")
        {
            setMaximumItems(tgui::stoi(value));
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

    void ComboBox::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "configfile")
            value = getLoadedConfigFile();
        else if (property == "itemstodisplay")
            value = tgui::to_string(getItemsToDisplay());
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
        else if (property == "borders")
            value = "(" + tgui::to_string(getBorders().left) + "," + tgui::to_string(getBorders().top) + "," + tgui::to_string(getBorders().right) + "," + tgui::to_string(getBorders().bottom) + ")";
        else if (property == "maximumitems")
            value = tgui::to_string(getMaximumItems());
        else if (property == "items")
            encodeList(m_listBox->getItems(), value);
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
        Widget::initialize(parent);

        if (!getTextFont() && m_parent->getGlobalFont())
            setTextFont(*m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::showListBox()
    {
        if (!m_listBox->isVisible())
        {
            m_listBox->show();

            sf::Vector2f position(getPosition().x, getPosition().y + m_listBox->getItemHeight() + m_borders.bottom);

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

            m_listBox->setPosition(position);
            static_cast<Container*>(container)->add(m_listBox);
            m_listBox->focus();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::hideListBox()
    {
        // If the list was open then close it now
        if (m_listBox->isVisible())
        {
            m_listBox->hide();

            // Find the gui in order to remove the ListBox from it
            Widget* container = this;
            while (container->getParent() != nullptr)
                container = container->getParent();

            static_cast<Container*>(container)->remove(m_listBox);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::newItemSelectedCallbackFunction()
    {
        if (m_callbackFunctions[ItemSelected].empty() == false)
        {
            // When no item is selected then send an empty string, otherwise send the item
            m_callback.text    = m_listBox->getSelectedItem();
            m_callback.value   = m_listBox->getSelectedItemIndex();
            m_callback.trigger = ItemSelected;
            addCallback();
        }

        hideListBox();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::listBoxUnfocusedCallbackFunction()
    {
        if (m_mouseHover == false)
            hideListBox();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        const sf::View& view = target.getView();

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / view.getSize().x;
        float scaleViewY = target.getSize().y / view.getSize().y;

        // Get the global position
        sf::Vector2f topLeftPosition = sf::Vector2f(((getAbsolutePosition().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                    ((getAbsolutePosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top));
        sf::Vector2f bottomRightPosition = sf::Vector2f((getAbsolutePosition().x + (m_listBox->getSize().x - (m_textureArrowDownNormal.getSize().x * (static_cast<float>(m_listBox->getItemHeight()) / m_textureArrowDownNormal.getSize().y)))
                                                         - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                                        (getAbsolutePosition().y + m_listBox->getSize().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top));

        // Adjust the transformation
        states.transform *= getTransform();

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Draw left border
        sf::RectangleShape border(sf::Vector2f(m_borders.left, static_cast<float>(m_listBox->getItemHeight() + m_borders.top)));
        border.setPosition(-m_borders.left, -m_borders.top);
        border.setFillColor(m_listBox->m_borderColor);
        target.draw(border, states);

        // Draw top border
        border.setSize(sf::Vector2f(static_cast<float>(m_listBox->getSize().x + m_borders.right), m_borders.top));
        border.setPosition(0, -m_borders.top);
        target.draw(border, states);

        // Draw right border
        border.setSize(sf::Vector2f(m_borders.right, static_cast<float>(m_listBox->getItemHeight() + m_borders.bottom)));
        border.setPosition(static_cast<float>(m_listBox->getSize().x), 0);
        target.draw(border, states);

        // Draw bottom border
        border.setSize(sf::Vector2f(m_listBox->getSize().x + m_borders.left, m_borders.bottom));
        border.setPosition(-m_borders.left, static_cast<float>(m_listBox->getItemHeight()));
        target.draw(border, states);

        // Draw the combo box
        sf::RectangleShape front(sf::Vector2f(static_cast<float>(m_listBox->getSize().x),
                                              static_cast<float>(m_listBox->getItemHeight())));
        front.setFillColor(m_listBox->getBackgroundColor());
        target.draw(front, states);

        // Create a text widget to draw it
        sf::Text tempText("kg", *m_listBox->getTextFont());
        tempText.setCharacterSize(static_cast<unsigned int>(m_listBox->getItemHeight() * 0.8f));
        tempText.setColor(m_listBox->getTextColor());

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
        states.transform.translate(2, std::floor((static_cast<int>(m_listBox->getItemHeight()) - tempText.getLocalBounds().height) / 2.0f -  tempText.getLocalBounds().top));
        tempText.setString(m_listBox->getSelectedItem());
        target.draw(tempText, states);

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);

        // Reset the transformations
        states.transform = oldTransform;

        // Set the arrow like it should (down when list box is invisible, up when it is visible)
        if (m_listBox->isVisible())
        {
            float scaleFactor =  static_cast<float>(m_listBox->getItemHeight()) / m_textureArrowUpNormal.getSize().y;
            states.transform.translate(m_listBox->getSize().x - (m_textureArrowUpNormal.getSize().x * scaleFactor), 0);
            states.transform.scale(scaleFactor, scaleFactor);

            // Draw the arrow
            if (m_separateHoverImage)
            {
                if (m_mouseHover && m_textureArrowUpHover.getData())
                    target.draw(m_textureArrowUpHover, states);
                else
                    target.draw(m_textureArrowUpNormal, states);
            }
            else // There is no separate hover image
            {
                target.draw(m_textureArrowUpNormal, states);

                if (m_mouseHover && m_textureArrowUpHover.getData())
                    target.draw(m_textureArrowUpHover, states);
            }
        }
        else
        {
            float scaleFactor =  static_cast<float>(m_listBox->getItemHeight()) / m_textureArrowDownNormal.getSize().y;
            states.transform.translate(m_listBox->getSize().x - (m_textureArrowDownNormal.getSize().x * scaleFactor), 0);
            states.transform.scale(scaleFactor, scaleFactor);

            // Draw the arrow
            if (m_separateHoverImage)
            {
                if (m_mouseHover && m_textureArrowDownHover.getData())
                    target.draw(m_textureArrowDownHover, states);
                else
                    target.draw(m_textureArrowDownNormal, states);
            }
            else // There is no separate hover image
            {
                target.draw(m_textureArrowDownNormal, states);

                if (m_mouseHover && m_textureArrowDownHover.getData())
                    target.draw(m_textureArrowDownHover, states);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
