/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
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


#include <TGUI/Widgets/Tabs.hpp>
#include <TGUI/Clipping.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tabs::Tabs()
    {
        m_type = "Tabs";

        m_renderer = aurora::makeCopied<TabsRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

        setTabHeight(30);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tabs::Ptr Tabs::create()
    {
        return std::make_shared<Tabs>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tabs::Ptr Tabs::copy(Tabs::ConstPtr tabs)
    {
        if (tabs)
            return std::static_pointer_cast<Tabs>(tabs->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TabsRenderer* Tabs::getSharedRenderer()
    {
        return aurora::downcast<TabsRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TabsRenderer* Tabs::getSharedRenderer() const
    {
        return aurora::downcast<const TabsRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TabsRenderer* Tabs::getRenderer()
    {
        return aurora::downcast<TabsRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TabsRenderer* Tabs::getRenderer() const
    {
        return aurora::downcast<const TabsRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        // Tabs is no longer auto-sizing
        m_autoSize = false;
        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setAutoSize(bool autoSize)
    {
        if (m_autoSize != autoSize)
        {
            m_autoSize = autoSize;
            recalculateTabsWidth();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::getAutoSize() const
    {
        return m_autoSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t Tabs::add(const sf::String& text, bool selectTab)
    {
        // Use the insert function to put the tab in the right place
        insert(m_tabTexts.size(), text, selectTab);

        // Return the index of the new tab
        return m_tabTexts.size()-1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::insert(std::size_t index, const sf::String& text, bool selectTab)
    {
        // If the index is too high then just insert at the end
        if (index > m_tabWidth.size())
            index = m_tabWidth.size();

        // Create the new tab
        Text newTab;
        newTab.setFont(m_fontCached);
        newTab.setColor(m_textColorCached);
        newTab.setOpacity(m_opacityCached);
        newTab.setCharacterSize(getTextSize());
        newTab.setString(text);

        m_tabWidth.insert(m_tabWidth.begin() + index, 0);
        m_tabTexts.insert(m_tabTexts.begin() + index, std::move(newTab));
        recalculateTabsWidth();

        if (m_hoveringTab >= static_cast<int>(index))
            m_hoveringTab++;
        if (m_selectedTab >= static_cast<int>(index))
            m_selectedTab++;

        // If the tab has to be selected then do so
        if (selectTab)
            select(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String Tabs::getText(std::size_t index) const
    {
        if (index >= m_tabTexts.size())
            return "";
        else
            return m_tabTexts[index].getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::changeText(std::size_t index, const sf::String& text)
    {
        if (index >= m_tabTexts.size())
            return false;

        m_tabTexts[index].setString(text);
        recalculateTabsWidth();

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::select(const sf::String& text)
    {
        for (unsigned int i = 0; i < m_tabTexts.size(); ++i)
        {
            if (m_tabTexts[i].getString() == text)
            {
                select(i);
                return;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::select(std::size_t index)
    {
        // If the index is too big then do nothing
        if (index > m_tabTexts.size() - 1)
            return;

        // Don't select a tab that is already selected
        if (m_selectedTab == static_cast<int>(index))
            return;

        if (m_selectedTab >= 0)
            m_tabTexts[m_selectedTab].setColor(m_textColorCached);

        // Select the tab
        m_selectedTab = static_cast<int>(index);
        m_tabTexts[m_selectedTab].setColor(m_selectedTextColorCached);

        // Send the callback
        onTabSelect.emit(this, m_tabTexts[index].getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::deselect()
    {
        if (m_selectedTab >= 0)
            m_tabTexts[m_selectedTab].setColor(m_textColorCached);

        m_selectedTab = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::remove(const sf::String& text)
    {
        for (unsigned int i = 0; i < m_tabTexts.size(); ++i)
        {
            if (m_tabTexts[i].getString() == text)
            {
                remove(i);
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::remove(std::size_t index)
    {
        // The index can't be too high
        if (index > m_tabTexts.size() - 1)
            return;

        // Remove the tab
        m_tabTexts.erase(m_tabTexts.begin() + index);
        m_tabWidth.erase(m_tabWidth.begin() + index);

        // Check if the selected tab should be updated
        if (m_selectedTab == static_cast<int>(index))
            m_selectedTab = -1;
        else if (m_selectedTab > static_cast<int>(index))
            --m_selectedTab;

        // New hovered tab depends on several factors, we keep it simple and just remove the hover state
        m_hoveringTab = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::removeAll()
    {
        m_tabTexts.clear();
        m_tabWidth.clear();
        m_selectedTab = -1;
        m_hoveringTab = -1;

        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String Tabs::getSelected() const
    {
        if (m_selectedTab >= 0)
            return m_tabTexts[m_selectedTab].getString();
        else
            return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Tabs::getSelectedIndex() const
    {
        return m_selectedTab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setTextSize(unsigned int size)
    {
        if ((size == 0) || (m_requestedTextSize != size))
        {
            m_requestedTextSize = size;

            if (size == 0)
                m_textSize = Text::findBestTextSize(m_fontCached, (getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()) * 0.8f);
            else
                m_textSize = size;

            for (auto& text : m_tabTexts)
                text.setCharacterSize(m_textSize);

            recalculateTabsWidth();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Tabs::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setTabHeight(float height)
    {
        Widget::setSize({getSizeLayout().x, height});
        m_bordersCached.updateParentSize(getSize());

        // Recalculate the size when the text is auto sizing
        if (m_requestedTextSize == 0)
            setTextSize(0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setMaximumTabWidth(float maximumWidth)
    {
        m_maximumTabWidth = maximumWidth;

        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Tabs::getMaximumTabWidth() const
    {
        return m_maximumTabWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setMinimumTabWidth(float minimumWidth)
    {
        m_minimumTabWidth = minimumWidth;

        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Tabs::getMinimumTabWidth() const
    {
        return m_minimumTabWidth;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t Tabs::getTabsCount() const
    {
        return m_tabTexts.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::mouseOnWidget(sf::Vector2f pos) const
    {
        return sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::leftMousePressed(sf::Vector2f pos)
    {
        pos -= getPosition();

        float width = m_bordersCached.getLeft() / 2.f;

        // Loop through all tabs
        for (unsigned int i = 0; i < m_tabWidth.size(); ++i)
        {
            // Append the width of the tab
            width += (m_bordersCached.getLeft() / 2.f) + m_tabWidth[i] + (m_bordersCached.getRight() / 2.0f);

            // If the mouse went down on this tab then select it
            if (pos.x < width)
            {
                select(i);
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::mouseMoved(sf::Vector2f pos)
    {
        Widget::mouseMoved(pos);

        pos -= getPosition();
        m_hoveringTab = -1;
        float width = m_bordersCached.getLeft() / 2.f;

        // Loop through all tabs
        for (unsigned int i = 0; i < m_tabWidth.size(); ++i)
        {
            // Append the width of the tab
            width += (m_bordersCached.getLeft() / 2.f) + m_tabWidth[i] + (m_bordersCached.getRight() / 2.0f);

            // If the mouse is on top of this tab then remember it
            if (pos.x < width)
            {
                m_hoveringTab = i;
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::mouseNoLongerOnWidget()
    {
        Widget::mouseNoLongerOnWidget();

        m_hoveringTab = -1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::recalculateTabsWidth()
    {
        if (m_autoSize)
        {
            if (m_tabWidth.empty())
                Widget::setSize({m_bordersCached.getLeft() + m_bordersCached.getRight(), getSizeLayout().y});
            else
            {
                // First calculate the width of the tabs as if there aren't any borders
                float totalWidth = 0;
                for (unsigned int i = 0; i < m_tabWidth.size(); ++i)
                {
                    m_tabWidth[i] = m_tabTexts[i].getSize().x + std::max(m_minimumTabWidth, 2 * m_distanceToSideCached);
                    if ((m_maximumTabWidth > 0) && (m_maximumTabWidth < m_tabWidth[i]))
                        m_tabWidth[i] = m_maximumTabWidth;

                    totalWidth += m_tabWidth[i];
                }

                // Now add the borders to the tabs
                totalWidth += (m_tabWidth.size() + 1) * ((m_bordersCached.getLeft() + m_bordersCached.getRight()) / 2.f);

                Widget::setSize({totalWidth, getSizeLayout().y});
            }
        }
        else // A size was provided
        {
            const float tabWidth = (getSize().x - ((m_tabWidth.size() + 1) * ((m_bordersCached.getLeft() + m_bordersCached.getRight()) / 2.f))) / m_tabWidth.size();
            for (unsigned int i = 0; i < m_tabWidth.size(); ++i)
                m_tabWidth[i] = tabWidth;
        }

        m_bordersCached.updateParentSize(getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Tabs::getSignal(std::string signalName)
    {
        if (signalName == toLower(onTabSelect.getName()))
            return onTabSelect;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::rendererChanged(const std::string& property)
    {
        if (property == "borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            recalculateTabsWidth();
        }
        else if (property == "textcolor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            updateTextColors();
        }
        else if (property == "textcolorhover")
        {
            m_textColorHoverCached = getSharedRenderer()->getTextColorHover();
            updateTextColors();
        }
        else if (property == "selectedtextcolor")
        {
            m_selectedTextColorCached = getSharedRenderer()->getSelectedTextColor();
            updateTextColors();
        }
        else if (property == "selectedtextcolorhover")
        {
            m_selectedTextColorHoverCached = getSharedRenderer()->getSelectedTextColorHover();
            updateTextColors();
        }
        else if (property == "texturetab")
        {
            m_spriteTab.setTexture(getSharedRenderer()->getTextureTab());
        }
        else if (property == "textureselectedtab")
        {
            m_spriteSelectedTab.setTexture(getSharedRenderer()->getTextureSelectedTab());
        }
        else if (property == "distancetoside")
        {
            m_distanceToSideCached = getSharedRenderer()->getDistanceToSide();
            recalculateTabsWidth();
        }
        else if (property == "backgroundcolor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == "backgroundcolorhover")
        {
            m_backgroundColorHoverCached = getSharedRenderer()->getBackgroundColorHover();
        }
        else if (property == "selectedbackgroundcolor")
        {
            m_selectedBackgroundColorCached = getSharedRenderer()->getSelectedBackgroundColor();
        }
        else if (property == "selectedbackgroundcolorhover")
        {
            m_selectedBackgroundColorHoverCached = getSharedRenderer()->getSelectedBackgroundColorHover();
        }
        else if (property == "bordercolor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);

            m_spriteTab.setOpacity(m_opacityCached);
            m_spriteSelectedTab.setOpacity(m_opacityCached);

            for (auto& tabText : m_tabTexts)
                tabText.setOpacity(m_opacityCached);
        }
        else if (property == "font")
        {
            Widget::rendererChanged(property);

            for (auto& tab : m_tabTexts)
                tab.setFont(m_fontCached);

            // Recalculate the size when the text is auto sizing
            if (m_requestedTextSize == 0)
                setTextSize(0);
            else
                recalculateTabsWidth();
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Tabs::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (getTabsCount() > 0)
        {
            std::string tabList = "[" + Serializer::serialize(getText(0));
            for (std::size_t i = 1; i < getTabsCount(); ++i)
                tabList += ", " + Serializer::serialize(getText(i));

            tabList += "]";
            node->propertyValuePairs["Tabs"] = make_unique<DataIO::ValueNode>(tabList);
        }

        if (getSelectedIndex() >= 0)
            node->propertyValuePairs["Selected"] = make_unique<DataIO::ValueNode>(to_string(getSelectedIndex()));

        if (m_maximumTabWidth > 0)
            node->propertyValuePairs["MaximumTabWidth"] = make_unique<DataIO::ValueNode>(to_string(m_maximumTabWidth));

        if (m_autoSize)
        {
            node->propertyValuePairs.erase("Size");
            node->propertyValuePairs["TabHeight"] = make_unique<DataIO::ValueNode>(to_string(getSize().y));
        }

        node->propertyValuePairs["TextSize"] = make_unique<DataIO::ValueNode>(to_string(m_textSize));
        node->propertyValuePairs["AutoSize"] = make_unique<DataIO::ValueNode>(to_string(m_autoSize));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::updateTextColors()
    {
        for (auto& tabText : m_tabTexts)
            tabText.setColor(m_textColorCached);

        if (m_selectedTab >= 0)
        {
            if ((m_selectedTab == m_hoveringTab) && m_selectedTextColorHoverCached.isSet())
                m_tabTexts[m_selectedTab].setColor(m_selectedTextColorHoverCached);
            else if (m_selectedTextColorCached.isSet())
                m_tabTexts[m_selectedTab].setColor(m_selectedTextColorCached);
        }

        if ((m_hoveringTab >= 0) && (m_selectedTab != m_hoveringTab))
        {
            if (m_textColorHoverCached.isSet())
                m_tabTexts[m_hoveringTab].setColor(m_textColorHoverCached);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());
        states.transform.translate({});

        // Draw the borders around the tabs
        if (m_bordersCached != Borders{0})
        {
            drawBorders(target, states, m_bordersCached, getSize(), m_borderColorCached);
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        const float usableHeight = getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom();
        for (unsigned int i = 0; i < m_tabTexts.size(); ++i)
        {
            sf::RenderStates textStates = states;

            // Draw the background of the tab
            if (m_selectedTab == static_cast<int>(i))
            {
                if (m_spriteSelectedTab.isSet())
                {
                    Sprite spriteSelectedTab = m_spriteSelectedTab;
                    spriteSelectedTab.setSize({m_tabWidth[i], usableHeight});
                    spriteSelectedTab.draw(target, states);
                }
                else // No texture was loaded
                {
                    if ((m_hoveringTab == static_cast<int>(i)) && m_selectedBackgroundColorHoverCached.isSet())
                        drawRectangleShape(target, states, {m_tabWidth[i], usableHeight}, m_selectedBackgroundColorHoverCached);
                    else
                        drawRectangleShape(target, states, {m_tabWidth[i], usableHeight}, m_selectedBackgroundColorCached);
                }
            }
            else // This tab is not selected
            {
                if (m_spriteTab.isSet())
                {
                    Sprite spriteTab = m_spriteTab;
                    spriteTab.setSize({m_tabWidth[i], usableHeight});
                    spriteTab.draw(target, states);
                }
                else // No texture was loaded
                {
                    if ((m_hoveringTab == static_cast<int>(i)) && m_backgroundColorHoverCached.isSet())
                        drawRectangleShape(target, states, {m_tabWidth[i], usableHeight}, m_backgroundColorHoverCached);
                    else
                        drawRectangleShape(target, states, {m_tabWidth[i], usableHeight}, m_backgroundColorCached);
                }
            }

            // Draw the borders between the tabs
            states.transform.translate({m_tabWidth[i], 0});
            if ((m_bordersCached != Borders{0}) && (i < m_tabWidth.size() - 1))
            {
                drawRectangleShape(target, states, {(m_bordersCached.getLeft() + m_bordersCached.getRight()) / 2.f, usableHeight}, m_borderColorCached);
                states.transform.translate({(m_bordersCached.getLeft() + m_bordersCached.getRight()) / 2.f, 0});
            }

            // Apply clipping if required for the text in this tab
            std::unique_ptr<Clipping> clipping;
            const float usableWidth = m_tabWidth[i] - (2 * m_distanceToSideCached);
            if (m_tabTexts[i].getSize().x > usableWidth)
                clipping = make_unique<Clipping>(target, textStates, sf::Vector2f{m_distanceToSideCached, 0}, sf::Vector2f{usableWidth, usableHeight});

            // Draw the text
            textStates.transform.translate({m_distanceToSideCached + ((usableWidth - m_tabTexts[i].getSize().x) / 2.f), ((usableHeight - m_tabTexts[i].getSize().y) / 2.f)});
            m_tabTexts[i].draw(target, textStates);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
