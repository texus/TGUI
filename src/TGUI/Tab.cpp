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


#include <TGUI/Container.hpp>
#include <TGUI/Tab.hpp>

#include <SFML/OpenGL.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab::Tab()
    {
        m_callback.widgetType = "Tab";

        addSignal<sf::String>("TabSelected");

        m_renderer = std::make_shared<TabRenderer>(this);

        getRenderer()->setBorders({2, 2, 2, 2});

        setTabHeight(30);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab::Ptr Tab::create(const std::string& themeFileFilename, const std::string& section)
    {
        auto tab = std::make_shared<Tab>();

        if (themeFileFilename != "")
        {
            tab->getRenderer()->setBorders({0, 0, 0, 0});

            std::string loadedThemeFile = getResourcePath() + themeFileFilename;

            // Open the theme file
            ThemeFileParser themeFile{loadedThemeFile, section};

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
                    tab->getRenderer()->setProperty(it->first, it->second, themeFileFolder);
                }
                catch (const Exception& e)
                {
                    throw Exception{std::string(e.what()) + " In section '" + section + "' in " + loadedThemeFile + "."};
                }
            }

            // Clear the vectors
            tab->m_tabTexts.clear();

            // Make sure the required texture was loaded
            if (tab->getRenderer()->m_textureNormal.getData() && tab->getRenderer()->m_textureSelected.getData())
                tab->setTabHeight(tab->getRenderer()->m_textureNormal.getSize().y);

            // Recalculate the text size when auto sizing
            if (tab->m_textSize == 0)
                tab->setTextSize(0);
        }

        return tab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab::Ptr Tab::copy(Tab::ConstPtr tab)
    {
        if (tab)
            return std::make_shared<Tab>(*tab);
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setPosition(const Layout& position)
    {
        Widget::setPosition(position);

        float positionX = getPosition().x;

        float textHeight;
        if (m_font)
            textHeight = sf::Text{"kg", *m_font, getTextSize()}.getLocalBounds().height;
        else
            textHeight = 0;

        auto textureNormalIt = getRenderer()->m_texturesNormal.begin();
        auto textureSelectedIt = getRenderer()->m_texturesSelected.begin();
        auto tabTextIt = m_tabTexts.begin();
        for (auto tabWidthIt = m_tabWidth.cbegin(); tabWidthIt != m_tabWidth.cend(); ++tabWidthIt, ++textureNormalIt, ++textureSelectedIt, ++tabTextIt)
        {
            if (getRenderer()->m_textureNormal.getData() && getRenderer()->m_textureSelected.getData())
            {
                textureNormalIt->setPosition({positionX, getPosition().y});
                textureSelectedIt->setPosition({positionX, getPosition().y});
            }

            tabTextIt->setPosition({positionX + m_distanceToSide + ((*tabWidthIt - (2 * m_distanceToSide) - tabTextIt->getSize().x) / 2.0f),
                                    getPosition().y + ((m_tabHeight - textHeight) / 2.0f)});

            positionX += *tabWidthIt + ((getRenderer()->getBorders().left + getRenderer()->getBorders().right) / 2.0f);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setSize(const Layout&)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tab::add(const sf::String& text, bool selectTab)
    {
        // Use the insert function to put the tab in the right place
        insert(m_tabTexts.size(), text, selectTab);

        // Return the index of the new tab
        return m_tabTexts.size()-1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::insert(unsigned int index, const sf::String& text, bool selectTab)
    {
        // If the index is too high then just insert at the end
        if (index > m_tabWidth.size())
            index = m_tabWidth.size();

        // Create the new tab
        Label newTab;
        newTab.setTextFont(m_font);
        newTab.setTextColor(getRenderer()->m_textColor);
        newTab.setTextSize(getTextSize());
        newTab.setText(text);

        // Calculate the width of the tab
        auto tabWidthIt = m_tabWidth.insert(m_tabWidth.begin() + index,
                                            m_maximumTabWidth ? (std::min(newTab.getSize().x + (2 * m_distanceToSide), m_maximumTabWidth))
                                                              : (newTab.getSize().x + (2 * m_distanceToSide))
                                           );

        // Add the new tab sprite
        if (getRenderer()->m_textureNormal.getData() && getRenderer()->m_textureSelected.getData())
        {
            auto normalIt = getRenderer()->m_texturesNormal.insert(getRenderer()->m_texturesNormal.begin() + index, getRenderer()->m_textureNormal);
            auto selectedIt = getRenderer()->m_texturesSelected.insert(getRenderer()->m_texturesSelected.begin() + index, getRenderer()->m_textureSelected);

            normalIt->setSize({*tabWidthIt, normalIt->getSize().y});
            selectedIt->setSize({*tabWidthIt, selectedIt->getSize().y});

            normalIt->setPosition({getPosition().x + getSize().x, getPosition().y});
            selectedIt->setPosition({getPosition().x + getSize().x, getPosition().y});
        }

        float textHeight;
        if (m_font)
            textHeight = sf::Text{"kg", *m_font, getTextSize()}.getLocalBounds().height;
        else
            textHeight = 0;

        // Set the correct size of the tab text
        newTab.setPosition({getPosition().x + getSize().x + m_distanceToSide + ((*tabWidthIt - (2 * m_distanceToSide) - newTab.getSize().x) / 2.0f),
                            getPosition().y + ((m_tabHeight - textHeight) / 2.0f)});

        // Add the tab
        m_tabTexts.insert(m_tabTexts.begin() + index, std::move(newTab));

        // Update the cached width of the widget
        if (m_tabTexts.size() > 1)
            m_width += *tabWidthIt + ((getRenderer()->getBorders().left + getRenderer()->getBorders().right) / 2.0f);
        else
            m_width += *tabWidthIt;

        if (m_selectedTab >= static_cast<int>(index))
            m_selectedTab++;

        // If the tab has to be selected then do so
        if (selectTab)
            select(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String Tab::getText(unsigned int index) const
    {
        if (index >= m_tabTexts.size())
            return "";
        else
            return m_tabTexts[index].getText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tab::changeText(unsigned int index, const sf::String& text)
    {
        if (index >= m_tabTexts.size())
            return false;

        // Update the width
        {
            m_width -= m_tabWidth[index];

            m_tabTexts[index].setText(text);
            if (m_maximumTabWidth)
                m_tabWidth[index] = std::min(m_tabTexts[index].getSize().x + (2 * m_distanceToSide), m_maximumTabWidth);
            else
                m_tabWidth[index] = m_tabTexts[index].getSize().x + (2 * m_distanceToSide);

            m_width += m_tabWidth[index];
        }

        // Update the image sizes
        if (getRenderer()->m_textureNormal.getData() && getRenderer()->m_textureSelected.getData())
        {
            getRenderer()->m_texturesNormal[index].setSize({m_tabWidth[index], getRenderer()->m_texturesNormal[index].getSize().y});
            getRenderer()->m_texturesSelected[index].setSize({m_tabWidth[index], getRenderer()->m_texturesSelected[index].getSize().y});
        }

        // Recalculate the positions of the tabs
        updatePosition();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::select(const sf::String& text)
    {
        for (unsigned int i = 0; i < m_tabTexts.size(); ++i)
        {
            if (m_tabTexts[i].getText() == text)
            {
                select(i);
                return;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::select(unsigned int index)
    {
        // If the index is too big then do nothing
        if (index > m_tabTexts.size() - 1)
            return;

        // Don't select a tab that is already selected
        if (m_selectedTab == static_cast<int>(index))
            return;

        if (m_selectedTab >= 0)
            m_tabTexts[m_selectedTab].setTextColor(getRenderer()->m_textColor);

        // Select the tab
        m_selectedTab = index;
        m_tabTexts[m_selectedTab].setTextColor(getRenderer()->m_selectedTextColor);

        // Send the callback
        m_callback.text = m_tabTexts[index].getText();
        sendSignal("TabSelected", m_tabTexts[index].getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::deselect()
    {
        if (m_selectedTab >= 0)
            m_tabTexts[m_selectedTab].setTextColor(getRenderer()->m_textColor);

        m_selectedTab = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::remove(const sf::String& text)
    {
        for (unsigned int i = 0; i < m_tabTexts.size(); ++i)
        {
            if (m_tabTexts[i].getText() == text)
            {
                remove(i);
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::remove(unsigned int index)
    {
        // The index can't be too high
        if (index > m_tabTexts.size() - 1)
            return;

        if (getRenderer()->m_textureNormal.getData() && getRenderer()->m_textureSelected.getData())
        {
            auto texturesNormalIt = getRenderer()->m_texturesNormal.begin();
            std::advance(texturesNormalIt, index);

            auto texturesSelectedIt = getRenderer()->m_texturesSelected.begin();
            std::advance(texturesSelectedIt, index);

            getRenderer()->m_texturesNormal.erase(texturesNormalIt);
            getRenderer()->m_texturesSelected.erase(texturesSelectedIt);
        }

        m_width -= m_tabWidth[index] + ((getRenderer()->getBorders().left + getRenderer()->getBorders().right) / 2.0f);

        // Remove the tab
        m_tabTexts.erase(m_tabTexts.begin() + index);
        m_tabWidth.erase(m_tabWidth.begin() + index);

        // Check if the selected tab should change
        if (m_selectedTab == static_cast<int>(index))
            m_selectedTab = -1;
        else if (m_selectedTab > static_cast<int>(index))
            --m_selectedTab;

        // Recalculate the positions of the tabs
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::removeAll()
    {
        m_tabTexts.clear();
        m_tabWidth.clear();
        m_selectedTab = -1;

        getRenderer()->m_texturesNormal.clear();
        getRenderer()->m_texturesSelected.clear();

        m_width = 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTextSize(unsigned int size)
    {
        if (m_textSize != size)
        {
            m_textSize = size;
            recalculateTabsWidth();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tab::getTextSize() const
    {
        if (m_textSize == 0)
            return static_cast<unsigned int>(m_tabHeight * 0.75f);
        else
            return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTabHeight(float height)
    {
        m_tabHeight = height;

        if (getRenderer()->m_textureNormal.getData() && getRenderer()->m_textureSelected.getData())
        {
            getRenderer()->m_textureNormal.setSize({getRenderer()->m_textureNormal.getSize().x, height});
            getRenderer()->m_textureSelected.setSize({getRenderer()->m_textureSelected.getSize().x, height});

            for (auto it = getRenderer()->m_texturesNormal.begin(); it != getRenderer()->m_texturesNormal.end(); ++it)
                it->setSize({it->getSize().x, height});

            for (auto it = getRenderer()->m_texturesSelected.begin(); it != getRenderer()->m_texturesSelected.end(); ++it)
                it->setSize({it->getSize().x, height});
        }

        // Recalculate the size when auto sizing
        if (m_textSize == 0)
            setTextSize(0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setMaximumTabWidth(float maximumWidth)
    {
        m_maximumTabWidth = std::abs(maximumWidth);

        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        if (getRenderer()->m_textureNormal.getData() && getRenderer()->m_textureSelected.getData())
        {
            getRenderer()->m_textureNormal.setColor(sf::Color(255, 255, 255, m_opacity));
            getRenderer()->m_textureSelected.setColor(sf::Color(255, 255, 255, m_opacity));

            for (auto it = getRenderer()->m_texturesNormal.begin(); it != getRenderer()->m_texturesNormal.end(); ++it)
                it->setColor(sf::Color(255, 255, 255, m_opacity));

            for (auto it = getRenderer()->m_texturesSelected.begin(); it != getRenderer()->m_texturesSelected.end(); ++it)
                it->setColor(sf::Color(255, 255, 255, m_opacity));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tab::mouseOnWidget(float x, float y)
    {
        if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(x, y))
            return true;

        if (m_mouseHover)
            mouseLeftWidget();

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::leftMousePressed(float x, float)
    {
        float width = getPosition().x;

        // Loop through all tabs
        for (unsigned int i = 0; i < m_tabWidth.size(); ++i)
        {
            // Append the width of the tab
            width += m_tabWidth[i] + (getRenderer()->getBorders().left / 2.0f);

            // If the mouse went down on a tab then select it
            if (x < width)
            {
                select(i);
                break;
            }

            width += (getRenderer()->getBorders().right / 2.0f);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::recalculateTabsWidth()
    {
        m_width = 0;

        auto textureNormalIt = getRenderer()->m_texturesNormal.begin();
        auto textureSelectedIt = getRenderer()->m_texturesSelected.begin();
        for (unsigned int i = 0; i < m_tabWidth.size(); ++i, ++textureNormalIt, ++textureSelectedIt)
        {
            if (m_maximumTabWidth)
                m_tabWidth[i] = std::min(m_tabTexts[i].getSize().x + (2 * m_distanceToSide), m_maximumTabWidth);
            else
                m_tabWidth[i] = m_tabTexts[i].getSize().x + (2 * m_distanceToSide);

            if (getRenderer()->m_textureNormal.getData() && getRenderer()->m_textureSelected.getData())
            {
                textureNormalIt->setSize({m_tabWidth[i], m_tabHeight});
                textureSelectedIt->setSize({m_tabWidth[i], m_tabHeight});
            }

            m_width += m_tabWidth[i];
        }

        if (!m_tabWidth.empty())
            m_width += ((m_tabWidth.size() - 1) * ((getRenderer()->getBorders().left + getRenderer()->getBorders().right) / 2.0f));

        // Recalculate the positions of the tabs
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!m_font && m_parent->getGlobalFont())
            getRenderer()->setTextFont(m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Draw the background
        getRenderer()->draw(target, states);

        GLint scissor[4];
        float accumulatedTabWidth = 0;

        // Draw the text
        for (unsigned int i = 0; i < m_tabTexts.size(); ++i)
        {
            // Check if clipping is required for this tab
            bool clippingRequired = false;
            if (m_tabTexts[i].getSize().x > m_tabWidth[i] - 2 * m_distanceToSide)
                clippingRequired = true;

            // Check if clipping is required for this text
            if (clippingRequired)
            {
                // Get the old clipping area
                glGetIntegerv(GL_SCISSOR_BOX, scissor);

                const sf::View& view = target.getView();

                // Calculate the scale factor of the view
                float scaleViewX = target.getSize().x / view.getSize().x;
                float scaleViewY = target.getSize().y / view.getSize().y;

                // Get the global position
                sf::Vector2f topLeftPosition = {((getAbsolutePosition().x + accumulatedTabWidth + m_distanceToSide + (view.getSize().x / 2.f) - view.getCenter().x) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                ((getAbsolutePosition().y + (view.getSize().y / 2.f) - view.getCenter().y) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};
                sf::Vector2f bottomRightPosition = {((getAbsolutePosition().x + accumulatedTabWidth + m_tabWidth[i] - m_distanceToSide - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                                    ((getAbsolutePosition().y + ((m_tabHeight + m_tabTexts[i].getSize().y) / 2.f) - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};

                // Calculate the clipping area
                GLint scissorLeft = std::max(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
                GLint scissorTop = std::max(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
                GLint scissorRight = std::min(static_cast<GLint>(bottomRightPosition.x * scaleViewX), scissor[0] + scissor[2]);
                GLint scissorBottom = std::min(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

                if (scissorRight < scissorLeft)
                    scissorRight = scissorLeft;
                else if (scissorBottom < scissorTop)
                    scissorTop = scissorBottom;

                // Set the clipping area
                glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);
            }

            // Draw the text
            target.draw(m_tabTexts[i], states);

            // Reset the old clipping area when needed
            if (clippingRequired)
            {
                clippingRequired = false;
                glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
            }

            accumulatedTabWidth += m_tabWidth[i] + ((getRenderer()->getBorders().left + getRenderer()->getBorders().right) / 2.0f);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabRenderer::setProperty(std::string property, const std::string& value, const std::string& rootPath)
    {
        if (property == "textcolor")
            setTextColor(extractColorFromString(property, value));
        else if (property == "selectedtextcolor")
            setSelectedTextColor(extractColorFromString(property, value));
        else if (property == "distancetoside")
            setDistanceToSide(tgui::stoul(value));
        else if (property == "normalimage")
            extractTextureFromString(property, value, rootPath, m_textureNormal);
        else if (property == "selectedimage")
            extractTextureFromString(property, value, rootPath, m_textureSelected);
        else if (property == "backgroundcolor")
            setBackgroundColor(extractColorFromString(property, value));
        else if (property == "selectedbackgroundcolor")
            setSelectedBackgroundColor(extractColorFromString(property, value));
        else if (property == "bordercolor")
            setBorderColor(extractColorFromString(property, value));
        else if (property == "borders")
            setBorders(extractBordersFromString(property, value));
        else
            throw Exception{"Unrecognized property '" + property + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabRenderer::setTextFont(std::shared_ptr<sf::Font> font)
    {
        m_tab->m_font = font;

        for (auto& tab : m_tab->m_tabTexts)
            tab.setTextFont(font);

        m_tab->recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabRenderer::setDistanceToSide(unsigned int distanceToSide)
    {
        m_tab->m_distanceToSide = distanceToSide;

        m_tab->recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabRenderer::setNormalImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_textureNormal.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_textureNormal = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabRenderer::setSelectedImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_textureSelected.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_textureSelected = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        float positionX = m_tab->getPosition().x;
        auto textureNormalIt = m_texturesNormal.cbegin();
        auto textureSelectedIt = m_texturesSelected.cbegin();
        for (unsigned int i = 0; i < m_tab->m_tabTexts.size(); ++i, ++textureNormalIt, ++textureSelectedIt)
        {
            if (m_textureNormal.getData() && m_textureSelected.getData())
            {
                if (m_tab->m_selectedTab == static_cast<int>(i))
                    target.draw(*textureSelectedIt, states);
                else
                    target.draw(*textureNormalIt, states);
            }
            else // There are no textures
            {
                sf::RectangleShape background{{m_tab->m_tabWidth[i], m_tab->m_tabHeight}};
                background.setPosition({positionX, m_tab->getPosition().y});

                if (m_tab->m_selectedTab == static_cast<int>(i))
                    background.setFillColor(m_selectedBackgroundColor);
                else
                    background.setFillColor(m_backgroundColor);

                target.draw(background, states);
            }

            // If there are borders then also draw them between the tabs
            if (((m_borders.left != 0) || (m_borders.right != 0)) && (i+1 != m_tab->m_tabTexts.size()))
            {
                sf::RectangleShape border({(m_borders.left + m_borders.right / 2.0f), m_tab->m_tabHeight});
                border.setPosition(positionX + m_tab->m_tabWidth[i], m_tab->getPosition().y);
                border.setFillColor(m_borderColor);
                target.draw(border, states);
            }

            // Position the next tab
            positionX += m_tab->m_tabWidth[i] + ((m_borders.left + m_borders.right) / 2.0f);
        }

        // Draw the borders around the tab
        if (m_borders != Borders{0, 0, 0, 0})
        {
            sf::Vector2f position = m_tab->getPosition();
            sf::Vector2f size = m_tab->getSize();

            // Draw left border
            sf::RectangleShape border({m_borders.left, size.y + m_borders.top});
            border.setPosition(position.x - m_borders.left, position.y - m_borders.top);
            border.setFillColor(m_borderColor);
            target.draw(border, states);

            // Draw top border
            border.setSize({size.x + m_borders.right, m_borders.top});
            border.setPosition(position.x, position.y - m_borders.top);
            target.draw(border, states);

            // Draw right border
            border.setSize({m_borders.right, size.y + m_borders.bottom});
            border.setPosition(position.x + size.x, position.y);
            target.draw(border, states);

            // Draw bottom border
            border.setSize({size.x + m_borders.left, m_borders.bottom});
            border.setPosition(position.x - m_borders.left, position.y + size.y);
            target.draw(border, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> TabRenderer::clone(Widget* widget)
    {
        auto renderer = std::shared_ptr<TabRenderer>(new TabRenderer{*this});
        renderer->m_tab = static_cast<Tab*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
