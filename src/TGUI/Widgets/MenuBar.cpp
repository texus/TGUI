/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Container.hpp>
#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Widgets/MenuBar.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::MenuBar()
    {
        m_callback.widgetType = "MenuBar";

        addSignal<std::vector<sf::String>, sf::String>("MenuItemClicked");

        m_renderer = std::make_shared<MenuBarRenderer>(this);
        reload();

        setSize({0, 20});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::Ptr MenuBar::create()
    {
        return std::make_shared<MenuBar>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::Ptr MenuBar::copy(MenuBar::ConstPtr menuBar)
    {
        if (menuBar)
            return std::static_pointer_cast<MenuBar>(menuBar->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setPosition(const Layout2d& position)
    {
        Widget::setPosition(position);

        if (!m_menus.empty())
        {
            // Position the menus
            sf::Vector2f pos = getPosition();
            for (unsigned int i = 0; i < m_menus.size(); ++i)
            {
                m_menus[i].text.setPosition({pos.x + getRenderer()->m_distanceToSide, pos.y + ((getSize().y - m_menus[i].text.getSize().y) / 2.f)});

                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    m_menus[i].menuItems[j].setPosition(pos.x + 2 * getRenderer()->m_distanceToSide, pos.y + (j+1)*getSize().y + ((getSize().y - m_menus[i].menuItems[j].getSize().y) / 2.f));

                pos.x += m_menus[i].text.getSize().x + 2 * getRenderer()->m_distanceToSide;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        setTextSize(findBestTextSize(getFont(), getSize().y * 0.85f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setFont(const Font& font)
    {
        Widget::setFont(font);

        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                m_menus[i].menuItems[j].setFont(font);

            m_menus[i].text.setFont(font);
        }

        setTextSize(findBestTextSize(getFont(), getSize().y * 0.85f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::addMenu(const sf::String& text)
    {
        Menu newMenu;

        newMenu.text.setFont(m_font);
        newMenu.text.setText(text);
        newMenu.text.setTextColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
        newMenu.text.setTextSize(m_textSize);

        m_menus.push_back(std::move(newMenu));

        // Update the position of the menus
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::addMenuItem(const sf::String& menu, const sf::String& text)
    {
        sf::Vector2f pos = getPosition();

        // Search for the menu
        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            // If this is the menu then add the menu item to it
            if (m_menus[i].text.getText() == menu)
            {
                Label menuItem;
                menuItem.setFont(m_font);
                menuItem.setText(text);
                menuItem.setTextColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
                menuItem.setTextSize(m_textSize);

                m_menus[i].menuItems.push_back(std::move(menuItem));

                // Position the new menu item
                m_menus[i].menuItems.back().setPosition({pos.x + 2 * getRenderer()->m_distanceToSide, pos.y + m_menus[i].menuItems.size() * getSize().y + ((getSize().y - m_menus[i].menuItems.back().getSize().y) / 2.f)});

                return true;
            }

            pos.x += m_menus[i].text.getSize().x + 2 * getRenderer()->m_distanceToSide;
        }

        // could not find the menu
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::addMenuItem(const sf::String& text)
    {
        if (!m_menus.empty())
            return addMenuItem(m_menus.back().text.getText(), text);
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::removeMenu(const sf::String& menu)
    {
        // Search for the menu
        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            // If this is the menu then remove it
            if (m_menus[i].text.getText() == menu)
            {
                m_menus.erase(m_menus.begin() + i);

                // The menu was removed, so it can't remain open
                if (m_visibleMenu == static_cast<int>(i))
                    m_visibleMenu = -1;

                // Update the position of the menus
                updatePosition();
                return true;
            }
        }

        // could not find the menu
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::removeMenuItem(const sf::String& menu, const sf::String& menuItem)
    {
        // Search for the menu
        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            // If this is the menu then search for the menu item
            if (m_menus[i].text.getText() == menu)
            {
                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                {
                    // If this is the menu item then remove it
                    if (m_menus[i].menuItems[j].getText() == menuItem)
                    {
                        m_menus[i].menuItems.erase(m_menus[i].menuItems.begin() + j);

                        // The item can't still be selected
                        if (m_menus[i].selectedMenuItem == static_cast<int>(j))
                            m_menus[i].selectedMenuItem = -1;

                        // Update the position of the menus
                        updatePosition();
                        return true;
                    }
                }
            }
        }

        // could not find menu item
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::removeAllMenus()
    {
        m_menus.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setTextSize(unsigned int size)
    {
        m_textSize = size;

        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                m_menus[i].menuItems[j].setTextSize(m_textSize);

            m_menus[i].text.setTextSize(m_textSize);
        }

        // Update the position of the items
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setMinimumSubMenuWidth(float minimumWidth)
    {
        m_minimumSubMenuWidth = minimumWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setOpacity(float opacity)
    {
        Widget::setOpacity(opacity);

        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                m_menus[i].menuItems[j].setTextColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));

            m_menus[i].text.setTextColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
        }

        if (m_visibleMenu != -1)
        {
            if (m_menus[m_visibleMenu].selectedMenuItem != -1)
            {
                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
                m_menus[m_visibleMenu].selectedMenuItem = -1;
            }

            m_menus[m_visibleMenu].text.setTextColor(calcColorOpacity(getRenderer()->m_selectedTextColor, getOpacity()));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setParent(Container* parent)
    {
        Widget::setParent(parent);

        if (getSize().x == 0)
            setSize(bindWidth(m_parent->shared_from_this()), m_size.y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::mouseOnWidget(float x, float y) const
    {
        // Check if the mouse is on top of the menu bar
        if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(x, y))
            return true;
        else
        {
            // Check if there is a menu open
            if (m_visibleMenu != -1)
            {
                // Search the left position of the open menu
                float left = 0;
                for (int i = 0; i < m_visibleMenu; ++i)
                    left += m_menus[i].text.getSize().x + (2 * getRenderer()->m_distanceToSide);

                // Find out what the width of the menu should be
                float width = m_minimumSubMenuWidth;
                for (unsigned int j = 0; j < m_menus[m_visibleMenu].menuItems.size(); ++j)
                {
                    if (width < m_menus[m_visibleMenu].menuItems[j].getSize().x + (3 * getRenderer()->m_distanceToSide))
                        width = m_menus[m_visibleMenu].menuItems[j].getSize().x + (3 * getRenderer()->m_distanceToSide);
                }

                // Check if the mouse is on top of the open menu
                if (sf::FloatRect{getPosition().x + left, getPosition().y + getSize().y, width, getSize().y * m_menus[m_visibleMenu].menuItems.size()}.contains(x, y))
                    return true;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::leftMousePressed(float x, float y)
    {
        // Check if a menu should be opened or closed
        if (y <= getSize().y + getPosition().y)
        {
            // Loop through the menus to check if the mouse is on top of them
            float menuWidth = 0;
            for (unsigned int i = 0; i < m_menus.size(); ++i)
            {
                menuWidth += m_menus[i].text.getSize().x + (2 * getRenderer()->m_distanceToSide);
                if (x < menuWidth)
                {
                    // Close the menu when it was already open
                    if (m_visibleMenu == static_cast<int>(i))
                    {
                        if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                        {
                            m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
                            m_menus[m_visibleMenu].selectedMenuItem = -1;
                        }

                        m_menus[m_visibleMenu].text.setTextColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
                        m_visibleMenu = -1;
                    }

                    // If this menu can be opened then do so
                    else if (!m_menus[i].menuItems.empty())
                    {
                        m_menus[i].text.setTextColor(calcColorOpacity(getRenderer()->m_selectedTextColor, getOpacity()));
                        m_visibleMenu = static_cast<int>(i);
                    }

                    break;
                }
            }
        }

        m_mouseDown = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::leftMouseReleased(float, float y)
    {
        if (m_mouseDown)
        {
            // Check if the mouse is on top of one of the menus
            if (y > getSize().y + getPosition().y)
            {
                unsigned int selectedMenuItem = static_cast<unsigned int>((y - getSize().y - getPosition().y) / getSize().y);

                if (selectedMenuItem < m_menus[m_visibleMenu].menuItems.size())
                {
                    m_callback.index = m_visibleMenu;
                    m_callback.text = m_menus[m_visibleMenu].menuItems[selectedMenuItem].getText();

                    sendSignal("MenuItemClicked",
                               std::vector<sf::String>{m_menus[m_visibleMenu].text.getText(), m_menus[m_visibleMenu].menuItems[selectedMenuItem].getText()},
                               m_menus[m_visibleMenu].menuItems[selectedMenuItem].getText());

                    closeVisibleMenu();
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseMoved(float x, float y)
    {
        if (!m_mouseHover)
            mouseEnteredWidget();

        // Check if the mouse is on top of the menu bar (not on an open menus)
        if (y <= getSize().y + getPosition().y)
        {
            // Don't open a menu without having clicked first
            if (m_visibleMenu != -1)
            {
                // Loop through the menus to check if the mouse is on top of them
                float menuWidth = 0;
                for (unsigned int i = 0; i < m_menus.size(); ++i)
                {
                    menuWidth += m_menus[i].text.getSize().x + (2 * getRenderer()->m_distanceToSide);
                    if (x < menuWidth)
                    {
                        // Check if the menu is already open
                        if (m_visibleMenu == static_cast<int>(i))
                        {
                            // If one of the menu items is selected then unselect it
                            if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                            {
                                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
                                m_menus[m_visibleMenu].selectedMenuItem = -1;
                            }
                        }
                        else // The menu isn't open yet
                        {
                            // If there is another menu open then close it first
                            closeVisibleMenu();

                            // If this menu can be opened then do so
                            if (!m_menus[i].menuItems.empty())
                            {
                                m_menus[i].text.setTextColor(calcColorOpacity(getRenderer()->m_selectedTextColor, getOpacity()));
                                m_visibleMenu = static_cast<int>(i);
                            }
                        }
                        break;
                    }
                }
            }
        }
        else // The mouse is on top of one of the menus
        {
            // Calculate on what menu item the mouse is located
            int selectedMenuItem = static_cast<int>((y - getSize().y - getPosition().y) / getSize().y);

            // Check if the mouse is on a different item than before
            if (selectedMenuItem != m_menus[m_visibleMenu].selectedMenuItem)
            {
                // If another of the menu items is selected then unselect it
                if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                    m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));

                // Mark the item below the mouse as selected
                m_menus[m_visibleMenu].selectedMenuItem = selectedMenuItem;
                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(calcColorOpacity(getRenderer()->m_selectedTextColor, getOpacity()));
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseNoLongerDown()
    {
        if (!m_mouseDown)
            closeVisibleMenu();

        Widget::mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseLeftWidget()
    {
        // Menu items which are selected on mouse hover should not remain selected now that the mouse has left
        if ((m_visibleMenu != -1) && (m_menus[m_visibleMenu].selectedMenuItem != -1))
        {
            m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
            m_menus[m_visibleMenu].selectedMenuItem = -1;
        }

        Widget::mouseLeftWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::closeVisibleMenu()
    {
        // Check if there is still a menu open
        if (m_visibleMenu != -1)
        {
            // If an item in that menu was selected then unselect it first
            if (m_menus[m_visibleMenu].selectedMenuItem != -1)
            {
                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
                m_menus[m_visibleMenu].selectedMenuItem = -1;
            }

            m_menus[m_visibleMenu].text.setTextColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
            m_visibleMenu = -1;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::reload(const std::string& primary, const std::string& secondary, bool force)
    {
        getRenderer()->setBackgroundColor({255, 255, 255});
        getRenderer()->setTextColor({0, 0, 0});
        getRenderer()->setSelectedBackgroundColor({0, 110, 255});
        getRenderer()->setSelectedTextColor({255, 255, 255});
        getRenderer()->setBackgroundTexture({});
        getRenderer()->setItemBackgroundTexture({});
        getRenderer()->setSelectedItemBackgroundTexture({});
        getRenderer()->setDistanceToSide(4);

        if (m_theme && primary != "")
        {
            Widget::reload(primary, secondary, force);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Draw the background
        getRenderer()->draw(target, states);

        // Draw the texts of the menus
        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            target.draw(m_menus[i].text, states);

            // Draw the menu items when the menu is open
            if (m_visibleMenu == static_cast<int>(i))
            {
                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    target.draw(m_menus[i].menuItems[j], states);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "backgroundcolor")
            setBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "selectedbackgroundcolor")
            setSelectedBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "textcolor")
            setTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "selectedtextcolor")
            setSelectedTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "backgroundimage")
            setBackgroundTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "itembackgroundimage")
            setItemBackgroundTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "selecteditembackgroundimage")
            setSelectedItemBackgroundTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "distancetoside")
            setDistanceToSide(Deserializer::deserialize(ObjectConverter::Type::Number, value).getNumber());
        else
            WidgetRenderer::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::setProperty(std::string property, ObjectConverter&& value)
    {
        property = toLower(property);

        if (value.getType() == ObjectConverter::Type::Color)
        {
            if (property == "backgroundcolor")
                setBackgroundColor(value.getColor());
            else if (property == "selectedbackgroundcolor")
                setSelectedBackgroundColor(value.getColor());
            else if (property == "textcolor")
                setTextColor(value.getColor());
            else if (property == "selectedtextcolor")
                setSelectedTextColor(value.getColor());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Texture)
        {
            if (property == "backgroundimage")
                setBackgroundTexture(value.getTexture());
            else if (property == "itembackgroundimage")
                setItemBackgroundTexture(value.getTexture());
            else if (property == "selecteditembackgroundimage")
                setSelectedItemBackgroundTexture(value.getTexture());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Number)
        {
            if (property == "distancetoside")
                setDistanceToSide(value.getNumber());
        }
        else
            WidgetRenderer::setProperty(property, std::move(value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter MenuBarRenderer::getProperty(std::string property) const
    {
        property = toLower(property);

        if (property == "backgroundcolor")
            return m_backgroundColor;
        else if (property == "selectedbackgroundcolor")
            return m_selectedBackgroundColor;
        else if (property == "textcolor")
            return m_textColor;
        else if (property == "selectedtextcolor")
            return m_selectedTextColor;
        else if (property == "backgroundimage")
            return m_backgroundTexture;
        else if (property == "itembackgroundimage")
            return m_itemBackgroundTexture;
        else if (property == "selecteditembackgroundimage")
            return m_selectedItemBackgroundTexture;
        else if (property == "distancetoside")
            return m_distanceToSide;
        else
            return WidgetRenderer::getProperty(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, ObjectConverter> MenuBarRenderer::getPropertyValuePairs() const
    {
        auto pairs = WidgetRenderer::getPropertyValuePairs();

        if (m_backgroundTexture.isLoaded())
            pairs["BackgroundImage"] = m_backgroundTexture;
        if (m_itemBackgroundTexture.isLoaded())
            pairs["ItemBackgroundImage"] = m_itemBackgroundTexture;
        if (m_selectedItemBackgroundTexture.isLoaded())
            pairs["SelectedItemBackgroundImage"] = m_selectedItemBackgroundTexture;

        pairs["BackgroundColor"] = m_backgroundColor;
        pairs["SelectedBackgroundColor"] = m_selectedBackgroundColor;
        pairs["TextColor"] = m_textColor;
        pairs["SelectedTextColor"] = m_selectedTextColor;
        pairs["DistanceToSide"] = m_distanceToSide;
        return pairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::setBackgroundColor(const Color& backgroundColor)
    {
        m_backgroundColor = backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::setTextColor(const Color& textColor)
    {
        m_textColor = textColor;

        for (unsigned int i = 0; i < m_menuBar->m_menus.size(); ++i)
        {
            for (unsigned int j = 0; j < m_menuBar->m_menus[i].menuItems.size(); ++j)
            {
                if (m_menuBar->m_menus[i].selectedMenuItem != static_cast<int>(j))
                    m_menuBar->m_menus[i].menuItems[j].setTextColor(calcColorOpacity(m_textColor, m_menuBar->getOpacity()));
            }

            m_menuBar->m_menus[i].text.setTextColor(calcColorOpacity(m_textColor, m_menuBar->getOpacity()));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::setSelectedBackgroundColor(const Color& selectedBackgroundColor)
    {
        m_selectedBackgroundColor = selectedBackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::setSelectedTextColor(const Color& selectedTextColor)
    {
        m_selectedTextColor = selectedTextColor;

        if (m_menuBar->m_visibleMenu != -1)
        {
            if (m_menuBar->m_menus[m_menuBar->m_visibleMenu].selectedMenuItem != -1)
                m_menuBar->m_menus[m_menuBar->m_visibleMenu].menuItems[m_menuBar->m_menus[m_menuBar->m_visibleMenu].selectedMenuItem].setTextColor(calcColorOpacity(m_selectedTextColor, m_menuBar->getOpacity()));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::setDistanceToSide(float distanceToSide)
    {
        m_distanceToSide = distanceToSide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::setBackgroundTexture(const Texture& texture)
    {
        m_backgroundTexture = texture;
        if (m_backgroundTexture.isLoaded())
        {
            m_backgroundTexture.setPosition(m_menuBar->getPosition());
            m_backgroundTexture.setSize(m_menuBar->getSize());
            m_backgroundTexture.setColor({m_backgroundTexture.getColor().r, m_backgroundTexture.getColor().g, m_backgroundTexture.getColor().b, static_cast<sf::Uint8>(m_menuBar->getOpacity() * 255)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::setItemBackgroundTexture(const Texture& texture)
    {
        m_itemBackgroundTexture = texture;
        if (m_itemBackgroundTexture.isLoaded())
        {
            m_itemBackgroundTexture.setPosition(m_menuBar->getPosition());
            m_itemBackgroundTexture.setSize(m_menuBar->getSize());
            m_itemBackgroundTexture.setColor({m_itemBackgroundTexture.getColor().r, m_itemBackgroundTexture.getColor().g, m_itemBackgroundTexture.getColor().b, static_cast<sf::Uint8>(m_menuBar->getOpacity() * 255)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::setSelectedItemBackgroundTexture(const Texture& texture)
    {
        m_selectedItemBackgroundTexture = texture;
        if (m_selectedItemBackgroundTexture.isLoaded())
        {
            m_selectedItemBackgroundTexture.setPosition(m_menuBar->getPosition());
            m_selectedItemBackgroundTexture.setSize(m_menuBar->getSize());
            m_selectedItemBackgroundTexture.setColor({m_selectedItemBackgroundTexture.getColor().r, m_selectedItemBackgroundTexture.getColor().g, m_selectedItemBackgroundTexture.getColor().b, static_cast<sf::Uint8>(m_menuBar->getOpacity() * 255)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Draw the background
        if (m_backgroundTexture.isLoaded())
        {
            Texture background = m_backgroundTexture;
            background.setPosition(m_menuBar->getPosition());
            background.setSize(m_menuBar->getSize());
            target.draw(background, states);
        }
        else
        {
            sf::RectangleShape background{m_menuBar->getSize()};
            background.setPosition({m_menuBar->getPosition()});
            background.setFillColor(calcColorOpacity(m_backgroundColor, m_menuBar->getOpacity()));
            target.draw(background, states);
        }

        // Draw the menu backgrounds
        float positionX = m_menuBar->getPosition().x;
        Texture backgroundTexture = m_itemBackgroundTexture;
        for (unsigned int i = 0; i < m_menuBar->m_menus.size(); ++i)
        {
            // Is the menu open?
            if (m_menuBar->m_visibleMenu == static_cast<int>(i))
            {
                // Find out what the width of the menu should be
                float menuWidth = m_menuBar->m_minimumSubMenuWidth;
                for (unsigned int j = 0; j < m_menuBar->m_menus[i].menuItems.size(); ++j)
                    menuWidth = std::max(menuWidth, m_menuBar->m_menus[i].menuItems[j].getSize().x + (3 * m_distanceToSide));

                if (m_selectedItemBackgroundTexture.isLoaded() && backgroundTexture.isLoaded())
                {
                    Texture selectedBackgroundTexture = m_selectedItemBackgroundTexture;
                    selectedBackgroundTexture.setPosition({positionX, m_menuBar->getPosition().y});
                    selectedBackgroundTexture.setSize({m_menuBar->m_menus[i].text.getSize().x + 2*m_distanceToSide, m_menuBar->getSize().y});
                    target.draw(selectedBackgroundTexture, states);

                    backgroundTexture.setSize({menuWidth, m_menuBar->getSize().y});
                    selectedBackgroundTexture.setSize({menuWidth, m_menuBar->getSize().y});
                    for (unsigned int j = 0; j < m_menuBar->m_menus[i].menuItems.size(); ++j)
                    {
                        if (m_menuBar->m_menus[i].selectedMenuItem == static_cast<int>(j))
                        {
                            selectedBackgroundTexture.setPosition({positionX, m_menuBar->getPosition().y + (j+1)*m_menuBar->getSize().y});
                            target.draw(selectedBackgroundTexture, states);
                        }
                        else
                        {
                            backgroundTexture.setPosition({positionX, m_menuBar->getPosition().y + (j+1)*m_menuBar->getSize().y});
                            target.draw(backgroundTexture, states);
                        }
                    }
                }
                else if (backgroundTexture.isLoaded())
                {
                    backgroundTexture.setPosition({positionX, m_menuBar->getPosition().y});
                    backgroundTexture.setSize({m_menuBar->m_menus[i].text.getSize().x + 2*m_distanceToSide, m_menuBar->getSize().y});
                    target.draw(backgroundTexture, states);

                    backgroundTexture.setSize({menuWidth, m_menuBar->getSize().y});
                    for (unsigned int j = 0; j < m_menuBar->m_menus[i].menuItems.size(); ++j)
                    {
                        backgroundTexture.setPosition({positionX, m_menuBar->getPosition().y + (j+1)*m_menuBar->getSize().y});
                        target.draw(backgroundTexture, states);
                    }
                }
                else
                {
                    sf::RectangleShape background{{m_menuBar->m_menus[i].text.getSize().x + 2*m_distanceToSide, m_menuBar->getSize().y}};
                    background.setPosition({positionX, m_menuBar->getPosition().y});
                    background.setFillColor(calcColorOpacity(m_selectedBackgroundColor, m_menuBar->getOpacity()));
                    target.draw(background, states);

                    background.setSize({menuWidth, m_menuBar->getSize().y});
                    for (unsigned int j = 0; j < m_menuBar->m_menus[i].menuItems.size(); ++j)
                    {
                        background.setPosition({positionX, m_menuBar->getPosition().y + (j+1)*m_menuBar->getSize().y});

                        if (m_menuBar->m_menus[i].selectedMenuItem == static_cast<int>(j))
                            background.setFillColor(calcColorOpacity(m_selectedBackgroundColor, m_menuBar->getOpacity()));
                        else
                            background.setFillColor(calcColorOpacity(m_backgroundColor, m_menuBar->getOpacity()));

                        target.draw(background, states);
                    }
                }
            }
            else // This menu is not open
            {
                if (backgroundTexture.isLoaded())
                {
                    backgroundTexture.setPosition({positionX, m_menuBar->getPosition().y});
                    backgroundTexture.setSize({m_menuBar->m_menus[i].text.getSize().x + 2*m_distanceToSide, m_menuBar->getSize().y});
                    target.draw(backgroundTexture, states);
                }
            }

            positionX += m_menuBar->m_menus[i].text.getSize().x + 2*m_distanceToSide;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> MenuBarRenderer::clone(Widget* widget)
    {
        auto renderer = std::make_shared<MenuBarRenderer>(*this);
        renderer->m_menuBar = static_cast<MenuBar*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
