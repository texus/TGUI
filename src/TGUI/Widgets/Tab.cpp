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
#include <TGUI/Widgets/Tab.hpp>
#include <TGUI/Clipping.hpp>

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
        reload();

        setTabHeight(30);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab::Ptr Tab::create()
    {
        return std::make_shared<Tab>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tab::Ptr Tab::copy(Tab::ConstPtr tab)
    {
        if (tab)
            return std::static_pointer_cast<Tab>(tab->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setPosition(const Layout2d& position)
    {
        Widget::setPosition(position);

        float positionX = getPosition().x;

        auto textureNormalIt = getRenderer()->m_texturesNormal.begin();
        auto textureSelectedIt = getRenderer()->m_texturesSelected.begin();
        auto tabTextIt = m_tabTexts.begin();
        for (auto tabWidthIt = m_tabWidth.cbegin(); tabWidthIt != m_tabWidth.cend(); ++tabWidthIt, ++tabTextIt)
        {
            if (getRenderer()->m_textureNormal.isLoaded() && getRenderer()->m_textureSelected.isLoaded())
            {
                textureNormalIt->setPosition({positionX, getPosition().y});
                textureSelectedIt->setPosition({positionX, getPosition().y});

                textureNormalIt++;
                textureSelectedIt++;
            }

            tabTextIt->setPosition({positionX + getRenderer()->m_distanceToSide + ((*tabWidthIt - (2 * getRenderer()->m_distanceToSide) - tabTextIt->getSize().x) / 2.0f),
                                    getPosition().y + ((m_tabHeight - tabTextIt->getSize().y) / 2.0f)});

            positionX += *tabWidthIt + ((getRenderer()->getBorders().left + getRenderer()->getBorders().right) / 2.0f);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setSize(const Layout2d&)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setFont(const Font& font)
    {
        Widget::setFont(font);

        for (auto& tab : m_tabTexts)
            tab.setFont(font);

        // Recalculate the size when auto sizing
        if (m_requestedTextSize == 0)
            setTextSize(0);
        else
            recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t Tab::add(const sf::String& text, bool selectTab)
    {
        // Use the insert function to put the tab in the right place
        insert(m_tabTexts.size(), text, selectTab);

        // Return the index of the new tab
        return m_tabTexts.size()-1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::insert(std::size_t index, const sf::String& text, bool selectTab)
    {
        // If the index is too high then just insert at the end
        if (index > m_tabWidth.size())
            index = m_tabWidth.size();

        // Create the new tab
        Label newTab;
        newTab.setFont(m_font);
        newTab.setTextColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));
        newTab.setTextSize(getTextSize());
        newTab.setText(text);

        // Calculate the width of the tab
        auto tabWidthIt = m_tabWidth.insert(m_tabWidth.begin() + index,
                                            m_maximumTabWidth ? (std::min(newTab.getSize().x + (2 * getRenderer()->m_distanceToSide), m_maximumTabWidth))
                                                              : (newTab.getSize().x + (2 * getRenderer()->m_distanceToSide))
                                           );

        // Add the new tab sprite
        if (getRenderer()->m_textureNormal.isLoaded() && getRenderer()->m_textureSelected.isLoaded())
        {
            auto normalIt = getRenderer()->m_texturesNormal.insert(getRenderer()->m_texturesNormal.begin() + index, getRenderer()->m_textureNormal);
            auto selectedIt = getRenderer()->m_texturesSelected.insert(getRenderer()->m_texturesSelected.begin() + index, getRenderer()->m_textureSelected);

            normalIt->setSize({*tabWidthIt, normalIt->getSize().y});
            selectedIt->setSize({*tabWidthIt, selectedIt->getSize().y});

            normalIt->setPosition({getPosition().x + getSize().x, getPosition().y});
            selectedIt->setPosition({getPosition().x + getSize().x, getPosition().y});
        }

        // Set the correct size of the tab text
        newTab.setPosition({getPosition().x + getSize().x + getRenderer()->m_distanceToSide + ((*tabWidthIt - (2 * getRenderer()->m_distanceToSide) - newTab.getSize().x) / 2.0f),
                            getPosition().y + ((m_tabHeight - newTab.getSize().y) / 2.0f)});

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

    sf::String Tab::getText(std::size_t index) const
    {
        if (index >= m_tabTexts.size())
            return "";
        else
            return m_tabTexts[index].getText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tab::changeText(std::size_t index, const sf::String& text)
    {
        if (index >= m_tabTexts.size())
            return false;

        // Update the width
        {
            m_width -= m_tabWidth[index];

            m_tabTexts[index].setText(text);
            if (m_maximumTabWidth)
                m_tabWidth[index] = std::min(m_tabTexts[index].getSize().x + (2 * getRenderer()->m_distanceToSide), m_maximumTabWidth);
            else
                m_tabWidth[index] = m_tabTexts[index].getSize().x + (2 * getRenderer()->m_distanceToSide);

            m_width += m_tabWidth[index];
        }

        // Update the image sizes
        if (getRenderer()->m_textureNormal.isLoaded() && getRenderer()->m_textureSelected.isLoaded())
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

    void Tab::select(std::size_t index)
    {
        // If the index is too big then do nothing
        if (index > m_tabTexts.size() - 1)
            return;

        // Don't select a tab that is already selected
        if (m_selectedTab == static_cast<int>(index))
            return;

        if (m_selectedTab >= 0)
            m_tabTexts[m_selectedTab].setTextColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));

        // Select the tab
        m_selectedTab = static_cast<int>(index);
        m_tabTexts[m_selectedTab].setTextColor(calcColorOpacity(getRenderer()->m_selectedTextColor, getOpacity()));

        // Send the callback
        m_callback.text = m_tabTexts[index].getText();
        sendSignal("TabSelected", m_tabTexts[index].getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::deselect()
    {
        if (m_selectedTab >= 0)
            m_tabTexts[m_selectedTab].setTextColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));

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

    void Tab::remove(std::size_t index)
    {
        // The index can't be too high
        if (index > m_tabTexts.size() - 1)
            return;

        if (getRenderer()->m_textureNormal.isLoaded() && getRenderer()->m_textureSelected.isLoaded())
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
        if ((size == 0) || (m_requestedTextSize != size))
        {
            m_requestedTextSize = size;

            if (size == 0)
                m_textSize = findBestTextSize(getFont(), m_tabHeight * 0.85f);
            else // An exact size was given
                m_textSize = size;

            for (auto& label : m_tabTexts)
                label.setTextSize(getTextSize());

            recalculateTabsWidth();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tab::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setTabHeight(float height)
    {
        m_tabHeight = height;

        if (getRenderer()->m_textureNormal.isLoaded() && getRenderer()->m_textureSelected.isLoaded())
        {
            getRenderer()->m_textureNormal.setSize({getRenderer()->m_textureNormal.getSize().x, height});
            getRenderer()->m_textureSelected.setSize({getRenderer()->m_textureSelected.getSize().x, height});

            auto& texturesNormal = getRenderer()->m_texturesNormal;
            for (auto it = texturesNormal.begin(); it != texturesNormal.end(); ++it)
                it->setSize({it->getSize().x, height});

            auto& texturesSelected = getRenderer()->m_texturesSelected;
            for (auto it = texturesSelected.begin(); it != texturesSelected.end(); ++it)
                it->setSize({it->getSize().x, height});
        }

        // Recalculate the size when auto sizing
        if (m_requestedTextSize == 0)
            setTextSize(0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setMaximumTabWidth(float maximumWidth)
    {
        m_maximumTabWidth = std::abs(maximumWidth);

        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::setOpacity(float opacity)
    {
        Widget::setOpacity(opacity);

        if (getRenderer()->m_textureNormal.isLoaded() && getRenderer()->m_textureSelected.isLoaded())
        {
            getRenderer()->m_textureNormal.setColor({getRenderer()->m_textureNormal.getColor().r, getRenderer()->m_textureNormal.getColor().g, getRenderer()->m_textureNormal.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
            getRenderer()->m_textureSelected.setColor({getRenderer()->m_textureSelected.getColor().r, getRenderer()->m_textureSelected.getColor().g, getRenderer()->m_textureSelected.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});

            auto& texturesNormal = getRenderer()->m_texturesNormal;
            for (auto it = texturesNormal.begin(); it != texturesNormal.end(); ++it)
                it->setColor({it->getColor().r, it->getColor().g, it->getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});

            auto& texturesSelected = getRenderer()->m_texturesSelected;
            for (auto it = texturesSelected.begin(); it != texturesSelected.end(); ++it)
                it->setColor({it->getColor().r, it->getColor().g, it->getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        }

        for (auto& tabText : m_tabTexts)
            tabText.setTextColor(calcColorOpacity(getRenderer()->m_textColor, getOpacity()));

        if (m_selectedTab >= 0)
            m_tabTexts[m_selectedTab].setTextColor(calcColorOpacity(getRenderer()->m_selectedTextColor, getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Tab::getWidgetOffset() const
    {
        return {getRenderer()->getBorders().left, getRenderer()->getBorders().top};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tab::mouseOnWidget(float x, float y) const
    {
        return sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(x, y);
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
        for (unsigned int i = 0; i < m_tabWidth.size(); ++i)
        {
            if (m_maximumTabWidth)
                m_tabWidth[i] = std::min(m_tabTexts[i].getSize().x + (2 * getRenderer()->m_distanceToSide), m_maximumTabWidth);
            else
                m_tabWidth[i] = m_tabTexts[i].getSize().x + (2 * getRenderer()->m_distanceToSide);

            if (getRenderer()->m_textureNormal.isLoaded() && getRenderer()->m_textureSelected.isLoaded())
            {
                textureNormalIt->setSize({m_tabWidth[i], m_tabHeight});
                textureSelectedIt->setSize({m_tabWidth[i], m_tabHeight});

                textureNormalIt++;
                textureSelectedIt++;
            }

            m_width += m_tabWidth[i];
        }

        if (!m_tabWidth.empty())
            m_width += ((m_tabWidth.size() - 1) * ((getRenderer()->getBorders().left + getRenderer()->getBorders().right) / 2.0f));

        // Recalculate the positions of the tabs
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::reload(const std::string& primary, const std::string& secondary, bool force)
    {
        getRenderer()->setBorders({2, 2, 2, 2});
        getRenderer()->setBackgroundColor({255, 255, 255});
        getRenderer()->setSelectedBackgroundColor({0, 110, 255});
        getRenderer()->setTextColor({0, 0, 0});
        getRenderer()->setSelectedTextColor({255, 255, 255});
        getRenderer()->setBorderColor({0, 0, 0});
        getRenderer()->setNormalTexture({});
        getRenderer()->setSelectedTexture({});

        if (m_theme && primary != "")
        {
            getRenderer()->setBorders({0, 0, 0, 0});
            Widget::reload(primary, secondary, force);

            if (force)
            {
                if (getRenderer()->m_textureNormal.isLoaded() && getRenderer()->m_textureSelected.isLoaded())
                    setTabHeight(getRenderer()->m_textureNormal.getSize().y);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tab::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Draw the background
        getRenderer()->draw(target, states);

        // Draw the text
        float accumulatedTabWidth = 0;
        for (unsigned int i = 0; i < m_tabTexts.size(); ++i)
        {
            // Only apply clipping if required for this tab
            if (m_tabTexts[i].getSize().x > m_tabWidth[i] - 2 * getRenderer()->m_distanceToSide)
            {
                Clipping clipping{target, states, {getPosition().x + accumulatedTabWidth + getRenderer()->m_distanceToSide, getPosition().y}, {m_tabWidth[i] - (2 * getRenderer()->m_distanceToSide), m_tabHeight}};
                target.draw(m_tabTexts[i], states);
            }
            else // Draw text without clipping
                target.draw(m_tabTexts[i], states);

            accumulatedTabWidth += m_tabWidth[i] + ((getRenderer()->getBorders().left + getRenderer()->getBorders().right) / 2.0f);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabRenderer::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "borders")
            setBorders(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == "backgroundcolor")
            setBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "selectedbackgroundcolor")
            setSelectedBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "textcolor")
            setTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "selectedtextcolor")
            setSelectedTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "bordercolor")
            setBorderColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "normalimage")
            setNormalTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "selectedimage")
            setSelectedTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "distancetoside")
            setDistanceToSide(Deserializer::deserialize(ObjectConverter::Type::Number, value).getNumber());
        else
            WidgetRenderer::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabRenderer::setProperty(std::string property, ObjectConverter&& value)
    {
        property = toLower(property);

        if (value.getType() == ObjectConverter::Type::Borders)
        {
            if (property == "borders")
                setBorders(value.getBorders());
            else
                return WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Color)
        {
            if (property == "backgroundcolor")
                setBackgroundColor(value.getColor());
            else if (property == "selectedbackgroundcolor")
                setSelectedBackgroundColor(value.getColor());
            else if (property == "textcolor")
                setTextColor(value.getColor());
            else if (property == "selectedtextcolor")
                setSelectedTextColor(value.getColor());
            else if (property == "bordercolor")
                setBorderColor(value.getColor());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Texture)
        {
            if (property == "normalimage")
                setNormalTexture(value.getTexture());
            else if (property == "selectedimage")
                setSelectedTexture(value.getTexture());
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

    ObjectConverter TabRenderer::getProperty(std::string property) const
    {
        property = toLower(property);

        if (property == "borders")
            return m_borders;
        else if (property == "backgroundcolor")
            return m_backgroundColor;
        else if (property == "selectedbackgroundcolor")
            return m_selectedBackgroundColor;
        else if (property == "textcolor")
            return m_textColor;
        else if (property == "selectedtextcolor")
            return m_selectedTextColor;
        else if (property == "bordercolor")
            return m_borderColor;
        else if (property == "normalimage")
            return m_textureNormal;
        else if (property == "selectedimage")
            return m_textureSelected;
        else if (property == "distancetoside")
            return m_distanceToSide;
        else
            return WidgetRenderer::getProperty(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, ObjectConverter> TabRenderer::getPropertyValuePairs() const
    {
        auto pairs = WidgetRenderer::getPropertyValuePairs();

        if (m_textureNormal.isLoaded() && m_textureSelected.isLoaded())
        {
            pairs["NormalImage"] = m_textureNormal;
            pairs["SelectedImage"] = m_textureSelected;
        }
        else
        {
            pairs["BackgroundColor"] = m_backgroundColor;
            pairs["SelectedBackgroundColor"] = m_selectedBackgroundColor;
        }

        pairs["TextColor"] = m_textColor;
        pairs["SelectedTextColor"] = m_selectedTextColor;
        pairs["BorderColor"] = m_borderColor;
        pairs["Borders"] = m_borders;
        pairs["DistanceToSide"] = m_distanceToSide;
        return pairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabRenderer::setTextColor(const Color& color)
    {
        m_textColor = color;

        for (auto& tabText : m_tab->m_tabTexts)
            tabText.setTextColor(calcColorOpacity(m_textColor, m_tab->getOpacity()));

        if (m_tab->m_selectedTab >= 0)
            m_tab->m_tabTexts[m_tab->m_selectedTab].setTextColor(calcColorOpacity(m_selectedTextColor, m_tab->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabRenderer::setSelectedTextColor(const Color& color)
    {
        m_selectedTextColor = color;

        if (m_tab->m_selectedTab >= 0)
            m_tab->m_tabTexts[m_tab->m_selectedTab].setTextColor(calcColorOpacity(m_selectedTextColor, m_tab->getOpacity()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabRenderer::setDistanceToSide(float distanceToSide)
    {
        m_distanceToSide = distanceToSide;

        m_tab->recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabRenderer::setBackgroundColor(const Color& color)
    {
        m_backgroundColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabRenderer::setSelectedBackgroundColor(const Color& color)
    {
        m_selectedBackgroundColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabRenderer::setBorderColor(const Color& color)
    {
        m_borderColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabRenderer::setNormalTexture(const Texture& texture)
    {
        m_textureNormal = texture;
        if (m_textureNormal.isLoaded())
        {
            m_textureNormal.setColor({m_textureNormal.getColor().r, m_textureNormal.getColor().g, m_textureNormal.getColor().b, static_cast<sf::Uint8>(m_tab->getOpacity() * 255)});

            if (m_textureSelected.isLoaded())
            {
                m_texturesNormal.clear();
                m_texturesSelected.clear();

                for (std::size_t i = 0; i < m_tab->getTabsCount(); ++i)
                {
                    m_texturesNormal.push_back(m_textureNormal);
                    m_texturesSelected.push_back(m_textureSelected);
                }

                m_tab->recalculateTabsWidth();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabRenderer::setSelectedTexture(const Texture& texture)
    {
        m_textureSelected = texture;
        if (m_textureSelected.isLoaded())
        {
            m_textureSelected.setColor({m_textureSelected.getColor().r, m_textureSelected.getColor().g, m_textureSelected.getColor().b, static_cast<sf::Uint8>(m_tab->getOpacity() * 255)});

            if (m_textureNormal.isLoaded())
            {
                m_texturesNormal.clear();
                m_texturesSelected.clear();

                for (std::size_t i = 0; i < m_tab->getTabsCount(); ++i)
                {
                    m_texturesNormal.push_back(m_textureNormal);
                    m_texturesSelected.push_back(m_textureSelected);
                }

                m_tab->recalculateTabsWidth();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        float positionX = m_tab->getPosition().x;
        auto textureNormalIt = m_texturesNormal.cbegin();
        auto textureSelectedIt = m_texturesSelected.cbegin();
        for (unsigned int i = 0; i < m_tab->m_tabTexts.size(); ++i)
        {
            if (m_textureNormal.isLoaded() && m_textureSelected.isLoaded())
            {
                if (m_tab->m_selectedTab == static_cast<int>(i))
                    target.draw(*textureSelectedIt, states);
                else
                    target.draw(*textureNormalIt, states);

                textureNormalIt++;
                textureSelectedIt++;
            }
            else // There are no textures
            {
                sf::RectangleShape background{{m_tab->m_tabWidth[i], m_tab->m_tabHeight}};
                background.setPosition({positionX, m_tab->getPosition().y});

                if (m_tab->m_selectedTab == static_cast<int>(i))
                    background.setFillColor(calcColorOpacity(m_selectedBackgroundColor, m_tab->getOpacity()));
                else
                    background.setFillColor(calcColorOpacity(m_backgroundColor, m_tab->getOpacity()));

                target.draw(background, states);
            }

            // If there are borders then also draw them between the tabs
            if (((m_borders.left != 0) || (m_borders.right != 0)) && (i+1 != m_tab->m_tabTexts.size()))
            {
                sf::RectangleShape border({(m_borders.left + m_borders.right / 2.0f), m_tab->m_tabHeight});
                border.setPosition(positionX + m_tab->m_tabWidth[i], m_tab->getPosition().y);
                border.setFillColor(calcColorOpacity(m_borderColor, m_tab->getOpacity()));
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
            border.setFillColor(calcColorOpacity(m_borderColor, m_tab->getOpacity()));
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
        auto renderer = std::make_shared<TabRenderer>(*this);
        renderer->m_tab = static_cast<Tab*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
