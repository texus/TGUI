/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Widgets/MenuBar.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBar::MenuBar()
    {
        m_type = "MenuBar";
        m_distanceToSideCached = Text::getLineHeight(m_fontCached, getGlobalTextSize()) * 0.4f;

        m_renderer = aurora::makeCopied<MenuBarRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTextSize(getGlobalTextSize());
        setMinimumSubMenuWidth((Text::getLineHeight(m_fontCached, m_textSize) * 4) + (2 * m_distanceToSideCached));
        setSize({"100%", Text::getLineHeight(m_fontCached, m_textSize) * 1.25f});
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

    MenuBarRenderer* MenuBar::getSharedRenderer()
    {
        return aurora::downcast<MenuBarRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MenuBarRenderer* MenuBar::getSharedRenderer() const
    {
        return aurora::downcast<const MenuBarRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MenuBarRenderer* MenuBar::getRenderer()
    {
        return aurora::downcast<MenuBarRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MenuBarRenderer* MenuBar::getRenderer() const
    {
        return aurora::downcast<const MenuBarRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_spriteBackground.setSize(getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::addMenu(const sf::String& text)
    {
        Menu newMenu;
        newMenu.text.setFont(m_fontCached);
        newMenu.text.setColor(m_textColorCached);
        newMenu.text.setOpacity(m_opacityCached);
        newMenu.text.setCharacterSize(m_textSize);
        newMenu.text.setString(text);
        m_menus.push_back(std::move(newMenu));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::addMenuItem(const sf::String& menu, const sf::String& text)
    {
        // Search for the menu
        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            // If this is the menu then add the menu item to it
            if (m_menus[i].text.getString() == menu)
            {
                Text menuItem;
                menuItem.setFont(m_fontCached);
                menuItem.setColor(m_textColorCached);
                menuItem.setOpacity(m_opacityCached);
                menuItem.setCharacterSize(m_textSize);
                menuItem.setString(text);

                m_menus[i].menuItems.push_back(std::move(menuItem));
                return true;
            }
        }

        // Could not find the menu
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::addMenuItem(const sf::String& text)
    {
        if (!m_menus.empty())
            return addMenuItem(m_menus.back().text.getString(), text);
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
            if (m_menus[i].text.getString() == menu)
            {
                m_menus.erase(m_menus.begin() + i);

                // The menu was removed, so it can't remain open
                if (m_visibleMenu == static_cast<int>(i))
                    m_visibleMenu = -1;

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
            if (m_menus[i].text.getString() == menu)
            {
                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                {
                    // If this is the menu item then remove it
                    if (m_menus[i].menuItems[j].getString() == menuItem)
                    {
                        m_menus[i].menuItems.erase(m_menus[i].menuItems.begin() + j);

                        // The item can't still be selected
                        if (m_menus[i].selectedMenuItem == static_cast<int>(j))
                            m_menus[i].selectedMenuItem = -1;

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
                m_menus[i].menuItems[j].setCharacterSize(m_textSize);

            m_menus[i].text.setCharacterSize(m_textSize);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int MenuBar::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setMinimumSubMenuWidth(float minimumWidth)
    {
        m_minimumSubMenuWidth = minimumWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float MenuBar::getMinimumSubMenuWidth() const
    {
        return m_minimumSubMenuWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::setInvertedMenuDirection(bool invertDirection)
    {
        m_invertedMenuDirection = invertDirection;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::getInvertedMenuDirection() const
    {
        return m_invertedMenuDirection;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::pair<sf::String, std::vector<sf::String>>> MenuBar::getMenus() const
    {
        std::vector<std::pair<sf::String, std::vector<sf::String>>> menus;

        for (const auto& menu : m_menus)
        {
            std::vector<sf::String> items;
            for (const auto& item : menu.menuItems)
                items.push_back(item.getString());

            menus.emplace_back(menu.text.getString(), std::move(items));
        }

        return menus;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::closeMenu()
    {
        // Check if there is still a menu open
        if (m_visibleMenu != -1)
        {
            // If an item in that menu was selected then unselect it first
            if (m_menus[m_visibleMenu].selectedMenuItem != -1)
            {
                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(m_textColorCached);
                m_menus[m_visibleMenu].selectedMenuItem = -1;
            }

            m_menus[m_visibleMenu].text.setColor(m_textColorCached);
            m_visibleMenu = -1;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool MenuBar::mouseOnWidget(Vector2f pos) const
    {
        // Check if the mouse is on top of the menu bar
        if (FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos))
            return true;
        else
        {
            // Check if there is a menu open
            if (m_visibleMenu != -1)
            {
                // Search the left position of the open menu
                float left = 0;
                for (int i = 0; i < m_visibleMenu; ++i)
                    left += m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);

                // Find out what the width of the menu should be
                float width = m_minimumSubMenuWidth;
                for (unsigned int j = 0; j < m_menus[m_visibleMenu].menuItems.size(); ++j)
                {
                    if (width < m_menus[m_visibleMenu].menuItems[j].getSize().x + (3 * m_distanceToSideCached))
                        width = m_menus[m_visibleMenu].menuItems[j].getSize().x + (3 * m_distanceToSideCached);
                }

                // Check if the mouse is on top of the open menu
                if (m_invertedMenuDirection)
                {
                    if (FloatRect{left, -(getSize().y * m_menus[m_visibleMenu].menuItems.size()), width, getSize().y * m_menus[m_visibleMenu].menuItems.size()}.contains(pos))
                        return true;
                }
                else
                {
                    if (FloatRect{getPosition().x + left, getPosition().y + getSize().y, width, getSize().y * m_menus[m_visibleMenu].menuItems.size()}.contains(pos))
                        return true;
                }
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::leftMousePressed(Vector2f pos)
    {
        // Check if a menu should be opened or closed
        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
        {
            // Loop through the menus to check if the mouse is on top of them
            float menuWidth = 0;
            for (unsigned int i = 0; i < m_menus.size(); ++i)
            {
                menuWidth += m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);
                if (pos.x < menuWidth)
                {
                    // Close the menu when it was already open
                    if (m_visibleMenu == static_cast<int>(i))
                    {
                        if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                        {
                            m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(m_textColorCached);
                            m_menus[m_visibleMenu].selectedMenuItem = -1;
                        }

                        m_menus[m_visibleMenu].text.setColor(m_textColorCached);
                        m_visibleMenu = -1;
                    }

                    // If this menu can be opened then do so
                    else if (!m_menus[i].menuItems.empty())
                    {
                        if (m_selectedTextColorCached.isSet())
                            m_menus[i].text.setColor(m_selectedTextColorCached);
                        else
                            m_menus[i].text.setColor(m_textColorCached);

                        m_visibleMenu = static_cast<int>(i);
                    }

                    break;
                }
            }
        }

        m_mouseDown = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::leftMouseReleased(Vector2f pos)
    {
        if (m_mouseDown)
        {
            pos -= getPosition();

            // Check if the mouse is on top of one of the menus
            if (!FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
            {
                std::size_t selectedMenuItem;
                if (m_invertedMenuDirection)
                    selectedMenuItem = static_cast<std::size_t>((-pos.y-1) / getSize().y);
                else
                    selectedMenuItem = static_cast<std::size_t>((pos.y - getSize().y) / getSize().y);

                if (selectedMenuItem < m_menus[m_visibleMenu].menuItems.size())
                {
                    onMenuItemClick.emit(this,
                                         m_menus[m_visibleMenu].menuItems[selectedMenuItem].getString(),
                                         {m_menus[m_visibleMenu].text.getString(), m_menus[m_visibleMenu].menuItems[selectedMenuItem].getString()});

                    closeMenu();
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseMoved(Vector2f pos)
    {
        pos -= getPosition();

        if (!m_mouseHover)
            mouseEnteredWidget();

        // Check if the mouse is on top of the menu bar (not on an open menus)
        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
        {
            // Don't open a menu without having clicked first
            if (m_visibleMenu != -1)
            {
                // Loop through the menus to check if the mouse is on top of them
                float menuWidth = 0;
                for (unsigned int i = 0; i < m_menus.size(); ++i)
                {
                    menuWidth += m_menus[i].text.getSize().x + (2 * m_distanceToSideCached);
                    if (pos.x < menuWidth)
                    {
                        // Check if the menu is already open
                        if (m_visibleMenu == static_cast<int>(i))
                        {
                            // If one of the menu items is selected then unselect it
                            if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                            {
                                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(m_textColorCached);
                                m_menus[m_visibleMenu].selectedMenuItem = -1;
                            }
                        }
                        else // The menu isn't open yet
                        {
                            // If there is another menu open then close it first
                            closeMenu();

                            // If this menu can be opened then do so
                            if (!m_menus[i].menuItems.empty())
                            {
                                if (m_selectedTextColorCached.isSet())
                                    m_menus[i].text.setColor(m_selectedTextColorCached);
                                else
                                    m_menus[i].text.setColor(m_textColorCached);

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
            int selectedMenuItem;
            if (m_invertedMenuDirection)
                selectedMenuItem = static_cast<int>((-pos.y-1) / getSize().y);
            else
                selectedMenuItem = static_cast<int>((pos.y - getSize().y) / getSize().y);

            // Check if the mouse is on a different item than before
            if (selectedMenuItem != m_menus[m_visibleMenu].selectedMenuItem)
            {
                // If another of the menu items is selected then unselect it
                if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                    m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(m_textColorCached);

                // Mark the item below the mouse as selected
                m_menus[m_visibleMenu].selectedMenuItem = selectedMenuItem;
                if (m_selectedTextColorCached.isSet())
                    m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(m_selectedTextColorCached);
                else
                    m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(m_textColorCached);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseNoLongerDown()
    {
        if (!m_mouseDown)
            closeMenu();

        Widget::mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::mouseLeftWidget()
    {
        // Menu items which are selected on mouse hover should not remain selected now that the mouse has left
        if ((m_visibleMenu != -1) && (m_menus[m_visibleMenu].selectedMenuItem != -1))
        {
            m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(m_textColorCached);
            m_menus[m_visibleMenu].selectedMenuItem = -1;
        }

        Widget::mouseLeftWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& MenuBar::getSignal(std::string signalName)
    {
        if (signalName == toLower(onMenuItemClick.getName()))
            return onMenuItemClick;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::rendererChanged(const std::string& property)
    {
        if (property == "textcolor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();

            for (unsigned int i = 0; i < m_menus.size(); ++i)
            {
                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                {
                    if (m_menus[i].selectedMenuItem != static_cast<int>(j))
                        m_menus[i].menuItems[j].setColor(m_textColorCached);
                }

                m_menus[i].text.setColor(m_textColorCached);
            }

            if ((m_visibleMenu != -1) && (m_menus[m_visibleMenu].selectedMenuItem != -1) && m_selectedTextColorCached.isSet())
                m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(m_selectedTextColorCached);
        }
        else if (property == "selectedtextcolor")
        {
            m_selectedTextColorCached = getSharedRenderer()->getSelectedTextColor();

            if (m_visibleMenu != -1)
            {
                if (m_menus[m_visibleMenu].selectedMenuItem != -1)
                {
                    if (m_selectedTextColorCached.isSet())
                        m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(m_selectedTextColorCached);
                    else
                        m_menus[m_visibleMenu].menuItems[m_menus[m_visibleMenu].selectedMenuItem].setColor(m_textColorCached);
                }
            }
        }
        else if (property == "texturebackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if (property == "textureitembackground")
        {
            m_spriteItemBackground.setTexture(getSharedRenderer()->getTextureItemBackground());
        }
        else if (property == "textureselecteditembackground")
        {
            m_spriteSelectedItemBackground.setTexture(getSharedRenderer()->getTextureSelectedItemBackground());
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "selectedbackgroundcolor")
        {
            m_selectedBackgroundColorCached = getSharedRenderer()->getSelectedBackgroundColor();
        }
        else if (property == "distancetoside")
        {
            m_distanceToSideCached = getSharedRenderer()->getDistanceToSide();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            for (unsigned int i = 0; i < m_menus.size(); ++i)
            {
                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    m_menus[i].menuItems[j].setOpacity(m_opacityCached);

                m_menus[i].text.setOpacity(m_opacityCached);
            }

            m_spriteBackground.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            for (unsigned int i = 0; i < m_menus.size(); ++i)
            {
                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    m_menus[i].menuItems[j].setFont(m_fontCached);

                m_menus[i].text.setFont(m_fontCached);
            }

            setTextSize(Text::findBestTextSize(m_fontCached, getSize().y * 0.8f));
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> MenuBar::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        const auto& menus = getMenus();
        for (const auto& menu : menus)
        {
            auto menuNode = std::make_unique<DataIO::Node>();
            menuNode->name = "Menu";

            menuNode->propertyValuePairs["Name"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(menu.first));

            const auto& items = menu.second;
            if (!items.empty())
            {
                std::string itemList = "[" + Serializer::serialize(items[0]);
                for (std::size_t i = 1; i < items.size(); ++i)
                    itemList += ", " + Serializer::serialize(items[i]);
                itemList += "]";

                menuNode->propertyValuePairs["Items"] = std::make_unique<DataIO::ValueNode>(itemList);
            }

            node->children.push_back(std::move(menuNode));
        }

        node->propertyValuePairs["TextSize"] = std::make_unique<DataIO::ValueNode>(to_string(m_textSize));
        node->propertyValuePairs["MinimumSubMenuWidth"] = std::make_unique<DataIO::ValueNode>(to_string(m_minimumSubMenuWidth));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs["textsize"])
            setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["minimumsubmenuwidth"])
            setMinimumSubMenuWidth(tgui::stof(node->propertyValuePairs["minimumsubmenuwidth"]->value));

        for (const auto& childNode : node->children)
        {
            if (toLower(childNode->name) == "menu")
            {
                if (childNode->propertyValuePairs["name"])
                {
                    addMenu(Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs["name"]->value).getString());

                    if (childNode->propertyValuePairs["items"])
                    {
                        if (!childNode->propertyValuePairs["items"]->listNode)
                            throw Exception{"Failed to parse 'Items' property inside 'Menu' property, expected a list as value"};

                        for (std::size_t i = 0; i < childNode->propertyValuePairs["items"]->valueList.size(); ++i)
                            addMenuItem(Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs["items"]->valueList[i]).getString());
                    }
                }
            }
        }
        node->children.erase(std::remove_if(node->children.begin(), node->children.end(), \
                                        [](const std::unique_ptr<DataIO::Node>& child){ return toLower(child->name) == "menu"; }), node->children.end());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MenuBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());

        sf::RenderStates textStates = states;

        float nextItemDistance;
        if (m_invertedMenuDirection)
            nextItemDistance = -getSize().y;
        else
            nextItemDistance = getSize().y;

        // Draw the background
        if (m_spriteBackground.isSet())
            m_spriteBackground.draw(target, states);
        else
            drawRectangleShape(target, states, getSize(), m_backgroundColorCached);

        // Draw the menu backgrounds
        Sprite backgroundSprite = m_spriteItemBackground;
        for (unsigned int i = 0; i < m_menus.size(); ++i)
        {
            // Is the menu open?
            if (m_visibleMenu == static_cast<int>(i))
            {
                const sf::RenderStates oldStates = states;

                // Find out what the width of the menu should be
                float menuWidth = m_minimumSubMenuWidth;
                for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    menuWidth = std::max(menuWidth, m_menus[i].menuItems[j].getSize().x + (2 * m_distanceToSideCached));

                Sprite selectedBackgroundSprite = m_spriteSelectedItemBackground;
                if (selectedBackgroundSprite.isSet() && backgroundSprite.isSet())
                {
                    selectedBackgroundSprite.setSize({m_menus[i].text.getSize().x + (2 * m_distanceToSideCached), getSize().y});
                    selectedBackgroundSprite.draw(target, states);

                    backgroundSprite.setSize({menuWidth, getSize().y});
                    selectedBackgroundSprite.setSize({menuWidth, getSize().y});
                    for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    {
                        states.transform.translate({0, nextItemDistance});
                        if (m_menus[i].selectedMenuItem == static_cast<int>(j))
                            selectedBackgroundSprite.draw(target, states);
                        else
                            backgroundSprite.draw(target, states);
                    }
                }
                else if (backgroundSprite.isSet())
                {
                    backgroundSprite.setSize({m_menus[i].text.getSize().x + (2 * m_distanceToSideCached), getSize().y});
                    backgroundSprite.draw(target, states);

                    backgroundSprite.setSize({menuWidth, getSize().y});
                    for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    {
                        states.transform.translate({0, nextItemDistance});
                        backgroundSprite.draw(target, states);
                    }
                }
                else // No textures where loaded
                {
                    if (m_selectedBackgroundColorCached.isSet())
                        drawRectangleShape(target, states, {m_menus[i].text.getSize().x + (2 * m_distanceToSideCached), getSize().y}, m_selectedBackgroundColorCached);
                    else
                        drawRectangleShape(target, states, {m_menus[i].text.getSize().x + (2 * m_distanceToSideCached), getSize().y}, m_backgroundColorCached);

                    for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    {
                        states.transform.translate({0, nextItemDistance});
                        if ((m_menus[i].selectedMenuItem == static_cast<int>(j)) && m_selectedBackgroundColorCached.isSet())
                            drawRectangleShape(target, states, {menuWidth, getSize().y}, m_selectedBackgroundColorCached);
                        else
                            drawRectangleShape(target, states, {menuWidth, getSize().y}, m_backgroundColorCached);
                    }
                }

                states = oldStates;
            }
            else // This menu is not open
            {
                if (backgroundSprite.isSet())
                {
                    backgroundSprite.setSize({m_menus[i].text.getSize().x + (2 * m_distanceToSideCached), getSize().y});
                    backgroundSprite.draw(target, states);
                }
            }

            states.transform.translate({m_menus[i].text.getSize().x + (2 * m_distanceToSideCached), 0});
        }

        // Draw the texts of the menus
        if (!m_menus.empty())
        {
            textStates.transform.translate({m_distanceToSideCached, (getSize().y - m_menus[0].text.getSize().y) / 2.f});
            for (unsigned int i = 0; i < m_menus.size(); ++i)
            {
                m_menus[i].text.draw(target, textStates);

                // Draw the menu items when the menu is open
                if (m_visibleMenu == static_cast<int>(i))
                {
                    const sf::RenderStates oldStates = textStates;

                    for (unsigned int j = 0; j < m_menus[i].menuItems.size(); ++j)
                    {
                        textStates.transform.translate({0, nextItemDistance});
                        m_menus[i].menuItems[j].draw(target, textStates);
                    }

                    textStates = oldStates;
                }

                textStates.transform.translate({m_menus[i].text.getSize().x + (2 * m_distanceToSideCached), 0});
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
