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


#include <TGUI/Container.hpp>
#include <TGUI/MenuBar.hpp>

#include <SFML/OpenGL.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::MenuBar()
    {
        m_callback.widgetType = Type_MenuBar;

        m_renderer = std::make_shared<MenuBarRenderer>(this);

        setSize({0, 20});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::Ptr MenuBar::create(const std::string& themeFileFilename, const std::string& section)
    {
        auto menuBar = std::make_shared<MenuBar>();

        if (themeFileFilename != "")
        {
            std::string loadedThemeFile = getResourcePath() + themeFileFilename;

            // Open the theme file
            ConfigFile themeFile{loadedThemeFile, section};

            // Find the folder that contains the theme file
            std::string themeFileFolder = "";
            std::string::size_type slashPos = loadedThemeFile.find_last_of("/\\");
            if (slashPos != std::string::npos)
                themeFileFolder = loadedThemeFile.substr(0, slashPos+1);

            // Handle the read properties
            for (auto it = themeFile.getProperties().cbegin(); it != themeFile.getProperties().cend(); ++it)
            {
                try
                {
                    menuBar->getRenderer()->setProperty(it->first, it->second, themeFileFolder);
                }
                catch (const Exception& e)
                {
                    throw Exception{std::string(e.what()) + " In section '" + section + "' in " + loadedThemeFile + "."};
                }
            }
        }

        return menuBar;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setPosition(const Layout& position)
    {
        Widget::setPosition(position);

        if (!m_menus.empty())
        {
            Label tempText(m_menus[0].text);
            tempText.setText("kg");

            // Position the menus
            sf::Vector2f pos = {getPosition().x, getPosition().y + (getSize().y - tempText.getSize().y) / 2.0f};
            for (unsigned int i = 0; i < m_menus.size(); ++i)
            {
                m_menus[i].text.setPosition({pos.x + getRenderer()->m_distanceToSide, pos.y});

                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    m_menus[i].menuItems[j].setPosition(pos.x + 2 * getRenderer()->m_distanceToSide, pos.y + (j+1)*getSize().y);

                pos.x += m_menus[i].text.getSize().x + 2 * getRenderer()->m_distanceToSide;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setSize(const Layout& size)
    {
        Widget::setSize(size);

        setTextSize(static_cast<unsigned int>(getSize().y * 0.75f));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::addMenu(const sf::String& text)
    {
        Menu newMenu;

        newMenu.text.setTextFont(*m_textFont);
        newMenu.text.setText(text);
        newMenu.text.setTextColor(getRenderer()->m_textColor);
        newMenu.text.setTextSize(m_textSize);

        m_menus.push_back(std::move(newMenu));

        // Update the position of the menus
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::addMenuItem(const sf::String& menu, const sf::String& text)
    {
        Label tempText(m_menus[0].text);
        tempText.setText("kg");

        sf::Vector2f pos = {getPosition().x, getPosition().y + (getSize().y - tempText.getSize().y) / 2.0f};

        // Search for the menu
        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            // If this is the menu then add the menu item to it
            if (m_menus[i].text.getText() == menu)
            {
                Label menuItem;
                menuItem.setTextFont(*m_textFont);
                menuItem.setText(text);
                menuItem.setTextColor(getRenderer()->m_textColor);
                menuItem.setTextSize(m_textSize);

                m_menus[i].menuItems.push_back(std::move(menuItem));

                // Position the new menu item
                m_menus[i].menuItems.back().setPosition({pos.x + 2 * getRenderer()->m_distanceToSide, pos.y + m_menus[i].menuItems.size() * getSize().y});

                return true;
            }

            pos.x += m_menus[i].text.getSize().x + 2 * getRenderer()->m_distanceToSide;
        }

        // could not find the menu
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

    bool MenuBar::mouseOnWidget(float x, float y)
    {
        // Check if the mouse is on top of the menu bar
        if (getTransform().transformRect(sf::FloatRect(0, 0, getSize().x, getSize().y)).contains(x, y))
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
                if (getTransform().transformRect(sf::FloatRect(left, getSize().y, width, getSize().y * m_menus[m_visibleMenu].menuItems.size())).contains(x, y))
                    return true;
            }
        }

        if (m_mouseHover)
            mouseLeftWidget();

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
                            m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(getRenderer()->m_textColor);
                            m_menus[m_visibleMenu].selectedMenuItem = -1;
                        }

                        m_menus[m_visibleMenu].text.setTextColor(getRenderer()->m_textColor);
                        m_visibleMenu = -1;
                    }

                    // If this menu can be opened then do so
                    else if (!m_menus[i].menuItems.empty())
                    {
                        m_menus[i].text.setTextColor(getRenderer()->m_selectedTextColor);
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
                    if (m_callbackFunctions[MenuItemClicked].empty() == false)
                    {
                        m_callback.trigger = MenuItemClicked;
                        m_callback.text = m_menus[m_visibleMenu].menuItems[selectedMenuItem].getText();
                        m_callback.index = m_visibleMenu;
                        addCallback();
                    }

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
                                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(getRenderer()->m_textColor);
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
                                if (m_visibleMenu != -1)
                                    m_menus[m_visibleMenu].text.setTextColor(getRenderer()->m_textColor);

                                m_menus[i].text.setTextColor(getRenderer()->m_selectedTextColor);
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
                    m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(getRenderer()->m_textColor);

                // Mark the item below the mouse as selected
                m_menus[m_visibleMenu].selectedMenuItem = selectedMenuItem;
                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(getRenderer()->m_selectedTextColor);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseNoLongerDown()
    {
        closeVisibleMenu();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!m_textFont && m_parent->getGlobalFont())
            getRenderer()->setTextFont(*m_parent->getGlobalFont());

        setSize(m_parent->getSize().x, m_size.y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseLeftWidget()
    {
        // Menu items which are selected on mouse hover should not remain selected now that the mouse has left
        if ((m_visibleMenu != -1) && (m_menus[m_visibleMenu].selectedMenuItem != -1))
        {
            m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(getRenderer()->m_textColor);
            m_menus[m_visibleMenu].selectedMenuItem = -1;
        }
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
                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setTextColor(getRenderer()->m_textColor);
                m_menus[m_visibleMenu].selectedMenuItem = -1;
            }

            m_menus[m_visibleMenu].text.setTextColor(getRenderer()->m_textColor);
            m_visibleMenu = -1;
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

    void MenuBarRenderer::setProperty(std::string property, const std::string& value, const std::string& rootPath)
    {
        if (property == "backgroundcolor")
            setBackgroundColor(extractColorFromString(property, value));
        else if (property == "textcolor")
            setTextColor(extractColorFromString(property, value));
        else if (property == "selectedbackgroundcolor")
            setSelectedBackgroundColor(extractColorFromString(property, value));
        else if (property == "selectedtextcolor")
            setSelectedTextColor(extractColorFromString(property, value));
        else if (property == "distancetoside")
            setDistanceToSide(tgui::stoul(value));
        else if (property == "backgroundimage")
            extractTextureFromString(property, value, rootPath, m_backgroundTexture);
        else if (property == "itembackgroundimage")
            extractTextureFromString(property, value, rootPath, m_itemBackgroundTexture);
        else if (property == "selecteditembackgroundimage")
            extractTextureFromString(property, value, rootPath, m_selectedItemBackgroundTexture);
        else
            throw Exception{"Unrecognized property '" + property + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_backgroundColor = backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::setTextColor(const sf::Color& textColor)
    {
        m_textColor = textColor;

        for (unsigned int i = 0; i < m_menuBar->m_menus.size(); ++i)
        {
            for (unsigned int j = 0; j < m_menuBar->m_menus[i].menuItems.size(); ++j)
            {
                if (m_menuBar->m_menus[i].selectedMenuItem != static_cast<int>(j))
                    m_menuBar->m_menus[i].menuItems[j].setTextColor(textColor);
            }

            m_menuBar->m_menus[i].text.setTextColor(textColor);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::setSelectedBackgroundColor(const sf::Color& selectedBackgroundColor)
    {
        m_selectedBackgroundColor = selectedBackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::setSelectedTextColor(const sf::Color& selectedTextColor)
    {
        m_selectedTextColor = selectedTextColor;

        if (m_menuBar->m_visibleMenu != -1)
        {
            if (m_menuBar->m_menus[m_menuBar->m_visibleMenu].selectedMenuItem != -1)
                m_menuBar->m_menus[m_menuBar->m_visibleMenu].menuItems[m_menuBar->m_menus[m_menuBar->m_visibleMenu].selectedMenuItem].setTextColor(selectedTextColor);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::setTextFont(const sf::Font& font)
    {
        m_menuBar->m_textFont = &font;

        for (unsigned int i = 0; i < m_menuBar->m_menus.size(); ++i)
        {
            for (unsigned int j = 0; j < m_menuBar->m_menus[i].menuItems.size(); ++j)
                m_menuBar->m_menus[i].menuItems[j].setTextFont(font);

            m_menuBar->m_menus[i].text.setTextFont(font);
        }

        m_menuBar->setTextSize(m_menuBar->m_textSize);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::setDistanceToSide(unsigned int distanceToSide)
    {
        m_distanceToSide = distanceToSide;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::setBackgroundImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_backgroundTexture.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_backgroundTexture = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::setItemBackgroundImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_itemBackgroundTexture.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_itemBackgroundTexture = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::setSelectedItemBackgroundImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_selectedItemBackgroundTexture.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_selectedItemBackgroundTexture = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBarRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Draw the background
        if (m_backgroundTexture.getData())
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
            background.setFillColor(m_backgroundColor);
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

                if (m_selectedItemBackgroundTexture.getData() && backgroundTexture.getData())
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
                else if (backgroundTexture.getData())
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
                    background.setFillColor(m_selectedBackgroundColor);
                    target.draw(background, states);

                    background.setSize({menuWidth, m_menuBar->getSize().y});
                    for (unsigned int j = 0; j < m_menuBar->m_menus[i].menuItems.size(); ++j)
                    {
                        background.setPosition({positionX, m_menuBar->getPosition().y + (j+1)*m_menuBar->getSize().y});

                        if (m_menuBar->m_menus[i].selectedMenuItem == static_cast<int>(j))
                            background.setFillColor(m_selectedBackgroundColor);
                        else
                            background.setFillColor(m_backgroundColor);

                        target.draw(background, states);
                    }
                }
            }
            else // This menu is not open
            {
                if (backgroundTexture.getData())
                {
                    backgroundTexture.setPosition({positionX, m_menuBar->getPosition().y});
                    backgroundTexture.setSize({m_menuBar->m_menus[i].text.getSize().x + 2*m_distanceToSide, m_menuBar->getSize().y});
                    target.draw(backgroundTexture, states);
                }
            }

            positionX += m_menuBar->m_menus[i].text.getSize().x + 2*m_distanceToSide;

            /*
            states.transform.translate(static_cast<float>(m_DistanceToSide), 0);

            // Is the menu open?
            if (m_visibleMenu == static_cast<int>(i))
            {
                states.transform.translate(0, getSize().y);

                // Find out what the width of the menu should be
                float menuWidth = m_minimumSubMenuWidth;
                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    menuWidth = std::max(menuWidth, m_menus[i].menuItems[j].getSize().x + (3 * getRenderer()->m_distanceToSide));

                // Draw the background of the menu
                sf::RectangleShape background({menuWidth, getSize().y * m_menus[i].menuItems.size()});
                background.setFillColor(getRenderer()->m_backgroundColor);
                target.draw(background, states);

                // If there is a selected menu item then draw its background
                if (m_menus[i].selectedMenuItem != -1)
                {
                    states.transform.translate(0, m_menus[i].selectedMenuItem * getSize().y);
                    background = sf::RectangleShape({menuWidth, getSize().y});
                    background.setFillColor(getRenderer()->m_selectedBackgroundColor);
                    target.draw(background, states);
                    states.transform.translate(0, m_menus[i].selectedMenuItem * -getSize().y);
                }

                states.transform.translate(m_menus[i].text.getSize().x + 2 * getRenderer()->m_distanceToSide, -getSize().y);
            }

            states.transform.translate(m_menus[i].text.getSize().x + 2*getRenderer()->m_distanceToSide, 0);
            */
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> MenuBarRenderer::clone(Widget* widget)
    {
        auto renderer = std::shared_ptr<MenuBarRenderer>(new MenuBarRenderer{*this});
        renderer->m_menuBar = static_cast<MenuBar*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
