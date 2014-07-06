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
#include <TGUI/ComboBox.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox()
    {
        m_callback.widgetType = Type_ComboBox;
        m_draggableWidget = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::ComboBox(const ComboBox& listBoxToCopy) :
        Widget                  {listBoxToCopy},
        WidgetBorders           {listBoxToCopy},
        m_loadedConfigFile      {listBoxToCopy.m_loadedConfigFile},
        m_separateHoverImage    {listBoxToCopy.m_separateHoverImage},
        m_nrOfItemsToDisplay    {listBoxToCopy.m_nrOfItemsToDisplay},
        m_textureArrowUpNormal  {listBoxToCopy.m_textureArrowUpNormal},
        m_textureArrowUpHover   {listBoxToCopy.m_textureArrowUpHover},
        m_textureArrowDownNormal{listBoxToCopy.m_textureArrowDownNormal},
        m_textureArrowDownHover {listBoxToCopy.m_textureArrowDownHover},
        m_text                  {listBoxToCopy.m_text}
    {
        if (listBoxToCopy.m_listBox != nullptr)
        {
            m_listBox = ListBox::copy(listBoxToCopy.m_listBox);
            m_listBox->hide();
            m_listBox->unbindAllCallback();
            m_listBox->bindCallback(ListBox::ItemSelected, std::bind(&ComboBox::newItemSelectedCallbackFunction, this));
            m_listBox->bindCallback(ListBox::Unfocused, std::bind(&ComboBox::listBoxUnfocusedCallbackFunction, this));
        }
        else
            m_listBox = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox& ComboBox::operator= (const ComboBox& right)
    {
        if (this != &right)
        {
            ComboBox temp{right};
            Widget::operator=(right);
            WidgetBorders::operator=(right);

            std::swap(m_loadedConfigFile,       temp.m_loadedConfigFile);
            std::swap(m_separateHoverImage,     temp.m_separateHoverImage);
            std::swap(m_nrOfItemsToDisplay,     temp.m_nrOfItemsToDisplay);
            std::swap(m_listBox,                temp.m_listBox);
            std::swap(m_textureArrowUpNormal,   temp.m_textureArrowUpNormal);
            std::swap(m_textureArrowUpHover,    temp.m_textureArrowUpHover);
            std::swap(m_textureArrowDownNormal, temp.m_textureArrowDownNormal);
            std::swap(m_textureArrowDownHover,  temp.m_textureArrowDownHover);
            std::swap(m_text,                   temp.m_text);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ComboBox::Ptr ComboBox::create(const std::string& configFileFilename)
    {
        auto comboBox = std::make_shared<ComboBox>();

        comboBox->m_loadedConfigFile = getResourcePath() + configFileFilename;

        comboBox->m_listBox = ListBox::create(configFileFilename);
        comboBox->m_listBox->hide();
        comboBox->m_listBox->changeColors();
        comboBox->m_listBox->bindCallback(ListBox::ItemSelected, std::bind(&ComboBox::newItemSelectedCallbackFunction, comboBox));
        comboBox->m_listBox->bindCallback(ListBox::Unfocused, std::bind(&ComboBox::listBoxUnfocusedCallbackFunction, comboBox));

        comboBox->setSize({50, 24});

        // Open the config file
        ConfigFile configFile{comboBox->m_loadedConfigFile, "ComboBox"};

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
                comboBox->m_separateHoverImage = configFile.readBool(it);
            }
            else if (it->first == "backgroundcolor")
            {
                comboBox->setBackgroundColor(configFile.readColor(it));
            }
            else if (it->first == "textcolor")
            {
                comboBox->setTextColor(configFile.readColor(it));
            }
            else if (it->first == "selectedbackgroundcolor")
            {
                comboBox->setSelectedBackgroundColor(configFile.readColor(it));
            }
            else if (it->first == "selectedtextcolor")
            {
                comboBox->setSelectedTextColor(configFile.readColor(it));
            }
            else if (it->first == "bordercolor")
            {
                comboBox->setBorderColor(configFile.readColor(it));
            }
            else if (it->first == "borders")
            {
                Borders borders;
                if (extractBorders(it->second, borders))
                    comboBox->setBorders(borders);
                else
                    throw Exception{"Failed to parse the 'Borders' property in section ComboBox in " + comboBox->m_loadedConfigFile};
            }
            else if (it->first == "arrowupnormalimage")
            {
                configFile.readTexture(it, getResourcePath() + configFileFolder, comboBox->m_textureArrowUpNormal);
            }
            else if (it->first == "arrowuphoverimage")
            {
                configFile.readTexture(it, getResourcePath() + configFileFolder, comboBox->m_textureArrowUpHover);
            }
            else if (it->first == "arrowdownnormalimage")
            {
                configFile.readTexture(it, getResourcePath() + configFileFolder, comboBox->m_textureArrowDownNormal);
            }
            else if (it->first == "arrowdownhoverimage")
            {
                configFile.readTexture(it, getResourcePath() + configFileFolder, comboBox->m_textureArrowDownHover);
            }
            else if (it->first == "scrollbar")
            {
                if ((it->second.length() < 3) || (it->second[0] != '"') || (it->second[it->second.length()-1] != '"'))
                    throw Exception{"Failed to parse value for Scrollbar in section ComboBox in " + comboBox->m_loadedConfigFile + "."};

                comboBox->m_listBox->setScrollbar(configFileFolder + it->second.substr(1, it->second.length()-2));
            }
            else
                throw Exception{"Unrecognized property '" + it->first + "' in section ComboBox in " + comboBox->m_loadedConfigFile + "."};
        }

        // Make sure the required textures were loaded
        if ((comboBox->m_textureArrowUpNormal.getData() == nullptr) || (comboBox->m_textureArrowDownNormal.getData() == nullptr))
            throw Exception{"Not all needed images were loaded for the combo box. Is the ComboBox section in " + comboBox->m_loadedConfigFile + " complete?"};

        // Remove all items (in case this is the second time that the load function was called)
        comboBox->m_listBox->removeAllItems();

        return comboBox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setPosition(const Layout& position)
    {
        Widget::setPosition(position);

        m_textureArrowUpNormal.setPosition({getPosition().x + getSize().x - m_textureArrowUpNormal.getSize().x, getPosition().y});
        m_textureArrowDownNormal.setPosition({getPosition().x + getSize().x - m_textureArrowDownNormal.getSize().x, getPosition().y});

        m_textureArrowUpHover.setPosition(m_textureArrowUpNormal.getPosition());
        m_textureArrowDownHover.setPosition(m_textureArrowUpNormal.getPosition());

        float textHeight = sf::Text{"kg", *m_text.getTextFont(), m_text.getTextSize()}.getLocalBounds().height;
        m_text.setPosition(getPosition().x + (m_text.getTextSize() / 10.0f),
                           getPosition().y + ((getSize().y - textHeight) / 2.0f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setSize(const Layout& size)
    {
        Widget::setSize(size);

        m_listBox->setItemHeight(static_cast<unsigned int>(getSize().y));

        if (m_nrOfItemsToDisplay > 0)
            m_listBox->setSize({getSize().x, static_cast<float>(m_listBox->getItemHeight() * (TGUI_MINIMUM(m_nrOfItemsToDisplay, m_listBox->getItemCount())))});
        else
            m_listBox->setSize({getSize().x, static_cast<float>(m_listBox->getItemHeight() * m_listBox->getItemCount())});

        m_textureArrowUpNormal.setSize({m_textureArrowUpNormal.getImageSize().x * (getSize().y / m_textureArrowUpNormal.getImageSize().y), getSize().y});
        m_textureArrowDownNormal.setSize({m_textureArrowDownNormal.getImageSize().x * (getSize().y / m_textureArrowDownNormal.getImageSize().y), getSize().y});

        m_textureArrowUpHover.setSize(m_textureArrowUpNormal.getSize());
        m_textureArrowDownHover.setSize(m_textureArrowUpNormal.getSize());

        m_text.setTextSize(static_cast<unsigned int>(getSize().y * 0.8f));

        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setItemsToDisplay(unsigned int nrOfItemsInList)
    {
        m_nrOfItemsToDisplay = nrOfItemsInList;

        if (m_nrOfItemsToDisplay < m_listBox->m_items.size())
            m_listBox->setSize({m_listBox->getSize().x, static_cast<float>(m_nrOfItemsToDisplay * m_listBox->getItemHeight())});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::changeColors(const sf::Color& backgroundColor,         const sf::Color& textColor,
                                const sf::Color& selectedBackgroundColor, const sf::Color& selectedTextColor,
                                const sf::Color& borderColor)
    {
        m_listBox->changeColors(backgroundColor, textColor, selectedBackgroundColor, selectedTextColor, borderColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setTextColor(const sf::Color& textColor)
    {
        m_text.setTextColor(textColor);
        m_listBox->setTextColor(textColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setTextFont(const sf::Font& font)
    {
        m_text.setTextFont(font);
        m_listBox->setTextFont(font);

        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::setBorders(const Borders& borders)
    {
        m_borders = borders;
        m_listBox->setBorders({m_borders.left, m_borders.bottom, m_borders.right, m_borders.bottom});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::addItem(const sf::String& item, const sf::String& id)
    {
        // Make room to add another item, until there are enough items
        if ((m_nrOfItemsToDisplay == 0) || (m_nrOfItemsToDisplay > m_listBox->getItemCount()))
            m_listBox->setSize({m_listBox->getSize().x, static_cast<float>(m_listBox->getItemHeight() * (m_listBox->getItemCount() + 1))});

        // Add the item
        return m_listBox->addItem(item, id);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItem(const sf::String& itemName)
    {
        bool ret = m_listBox->setSelectedItem(itemName);
        m_text.setText(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::setSelectedItemById(const sf::String& id)
    {
        bool ret = m_listBox->setSelectedItemById(id);
        m_text.setText(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::deselectItem()
    {
        m_text.setText("");
        m_listBox->deselectItem();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItem(const sf::String& itemName)
    {
        bool ret = m_listBox->removeItem(itemName);
        m_text.setText(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::removeItemById(const sf::String& id)
    {
        bool ret = m_listBox->removeItemById(id);
        m_text.setText(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::removeAllItems()
    {
        m_text.setText("");
        m_listBox->removeAllItems();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::changeItem(const sf::String& originalValue, const sf::String& newValue)
    {
        bool ret = m_listBox->changeItem(originalValue, newValue);
        m_text.setText(m_listBox->getSelectedItem());
        return ret;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ComboBox::changeItemById(const sf::String& id, const sf::String& newValue)
    {
        bool ret = m_listBox->changeItemById(id, newValue);
        m_text.setText(m_listBox->getSelectedItem());
        return ret;
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
        // Check if the mouse is on top of the combo box
        if ((x > getPosition().x - m_borders.left) && (x < getPosition().x + getSize().x + m_borders.right)
         && (y > getPosition().y - m_borders.top) && (y < getPosition().y + getSize().y + m_borders.bottom))
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
                    if (static_cast<unsigned int>(m_listBox->m_selectedItem + 1) > m_nrOfItemsToDisplay)
                        m_listBox->m_scroll->setValue(static_cast<unsigned int>((m_listBox->m_selectedItem + 1 - m_nrOfItemsToDisplay) * m_listBox->getItemHeight()));
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
                if (static_cast<unsigned int>(m_listBox->m_selectedItem + 1) < m_listBox->m_items.size())
                    m_listBox->setSelectedItem(static_cast<unsigned int>(m_listBox->m_selectedItem+1));
            }
            else // You are scrolling up
            {
                // select the previous item
                if (m_listBox->m_selectedItem > 0)
                    m_listBox->setSelectedItem(static_cast<unsigned int>(m_listBox->m_selectedItem-1));
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::mouseNoLongerDown()
    {
        m_mouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!getTextFont() && m_parent->getGlobalFont())
            setTextFont(*m_parent->getGlobalFont());

        m_text.initialize(parent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ComboBox::showListBox()
    {
        if (!m_listBox->isVisible())
        {
            m_listBox->show();

            sf::Vector2f position = {getPosition().x, getPosition().y + getSize().y + m_borders.top};

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
        m_text.setText(m_listBox->getSelectedItem());

        if (m_callbackFunctions[ItemSelected].empty() == false)
        {
            // When no item is selected then send an empty string, otherwise send the item
            m_callback.text    = m_listBox->getSelectedItem();
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
        sf::Vector2f topLeftPosition = {((getAbsolutePosition().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                        ((getAbsolutePosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};
        sf::Vector2f bottomRightPosition = {(getAbsolutePosition().x + (getSize().x - (m_textureArrowDownNormal.getSize().x * (getSize().y / m_textureArrowDownNormal.getSize().y)))
                                             - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                            (getAbsolutePosition().y + m_listBox->getSize().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top)};

        // Draw the combo box
        sf::RectangleShape front(getSize());
        front.setPosition(getPosition());
        front.setFillColor(m_listBox->getBackgroundColor());
        target.draw(front, states);

        // Draw the borders
        {
            // Draw left border
            sf::RectangleShape border({m_borders.left, getSize().y + m_borders.top});
            border.setPosition(getPosition().x - m_borders.left, getPosition().y - m_borders.top);
            border.setFillColor(m_listBox->m_borderColor);
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
        target.draw(m_text, states);

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);

        // Set the arrow like it should (down when list box is invisible, up when it is visible)
        if (m_listBox->isVisible())
        {
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
