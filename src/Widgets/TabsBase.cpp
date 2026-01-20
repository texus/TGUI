/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2026 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Optional.hpp>

#include <algorithm>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TabsBase::TabsBase(const char* typeName, bool initRenderer) :
        Widget{typeName, initRenderer},
        m_distanceToSideCached(std::round(Text::getLineHeight(m_fontCached, getGlobalTextSize()) * 0.4f))
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TabsRenderer* TabsBase::getSharedRenderer()
    {
        return downcast<TabsRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const TabsRenderer* TabsBase::getSharedRenderer() const
    {
        return downcast<const TabsRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TabsRenderer* TabsBase::getRenderer()
    {
        return downcast<TabsRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabsBase::setEnabled(bool enabled)
    {
        Widget::setEnabled(enabled);
        updateTextColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TabsBase::add(const String& text, bool selectTab)
    {
        // Use the insert function to put the tab in the right place
        insert(m_tabs.size(), text, selectTab);

        // Return the index of the new tab
        return m_tabs.size()-1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabsBase::insert(std::size_t index, const String& text, bool selectTab)
    {
        // If the index is too high then just insert at the end
        index = std::min(index, m_tabs.size());

        // Create the new tab
        Tab newTab;
        newTab.visible = true;
        newTab.enabled = true;
        newTab.width = 0;
        newTab.text.setFont(m_fontCached);
        newTab.text.setColor(m_textColorCached);
        newTab.text.setOpacity(m_opacityCached);
        newTab.text.setCharacterSize(getTextSize());
        newTab.text.setString(text);

        m_tabs.insert(m_tabs.begin() + static_cast<std::ptrdiff_t>(index), std::move(newTab));
        recalculateTabsSize();

        // New hovered tab depends on several factors, we keep it simple and just remove the hover state
        m_hoveringTab = -1;

        if (m_selectedTab >= static_cast<int>(index))
            m_selectedTab++;

        // If the tab has to be selected then do so
        if (selectTab)
            select(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String TabsBase::getText(std::size_t index) const
    {
        if (index >= m_tabs.size())
            return "";
        return m_tabs[index].text.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TabsBase::changeText(std::size_t index, const String& text)
    {
        if (index >= m_tabs.size())
            return false;

        m_tabs[index].text.setString(text);
        recalculateTabsSize();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TabsBase::select(const String& text)
    {
        for (std::size_t i = 0; i < m_tabs.size(); ++i)
        {
            if (m_tabs[i].text.getString() == text)
                return select(i);
        }

        deselect();
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TabsBase::select(std::size_t index)
    {
        // Don't select a tab that is already selected
        if (m_selectedTab == static_cast<int>(index))
            return true;

        // If the index is too high or if the tab is invisible or disabled then we can't select it
        if ((index >= m_tabs.size()) || !m_enabled || !m_tabs[index].visible || !m_tabs[index].enabled)
        {
            deselect();
            return false;
        }

        // Select the tab
        m_selectedTab = static_cast<int>(index);
        m_tabs[index].text.setColor(m_selectedTextColorCached);
        updateTextColors();

        // Send the callback
        onTabSelect.emit(this, m_tabs[index].text.getString());
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabsBase::deselect()
    {
        if (m_selectedTab >= 0)
        {
            m_selectedTab = -1;
            updateTextColors();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TabsBase::remove(const String& text)
    {
        for (std::size_t i = 0; i < m_tabs.size(); ++i)
        {
            if (m_tabs[i].text.getString() == text)
                return remove(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TabsBase::remove(std::size_t index)
    {
        // The index can't be too high
        if (index > m_tabs.size() - 1)
            return false;

        // Remove the tab
        m_tabs.erase(m_tabs.begin() + static_cast<std::ptrdiff_t>(index));

        // Check if the selected tab should be updated
        if (m_selectedTab == static_cast<int>(index))
            m_selectedTab = -1;
        else if (m_selectedTab > static_cast<int>(index))
            --m_selectedTab;

        // New hovered tab depends on several factors, we keep it simple and just remove the hover state
        m_hoveringTab = -1;
        recalculateTabsSize();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabsBase::removeAll()
    {
        m_tabs.clear();
        m_selectedTab = -1;
        m_hoveringTab = -1;

        recalculateTabsSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String TabsBase::getSelected() const
    {
        if (m_selectedTab >= 0)
            return m_tabs[static_cast<std::size_t>(m_selectedTab)].text.getString();
        return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int TabsBase::getSelectedIndex() const
    {
        return m_selectedTab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int TabsBase::getHoveredIndex() const
    {
        return m_hoveringTab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabsBase::setTabVisible(std::size_t index, bool visible)
    {
        if (index >= m_tabs.size())
            return;

        m_tabs[index].visible = visible;
        recalculateTabsSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TabsBase::getTabVisible(std::size_t index) const
    {
        if (index >= m_tabs.size())
            return false;

        return m_tabs[index].visible;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabsBase::setTabEnabled(std::size_t index, bool enabled)
    {
        if (index >= m_tabs.size())
            return;

        m_tabs[index].enabled = enabled;
        updateTextColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TabsBase::getTabEnabled(std::size_t index) const
    {
        if (index >= m_tabs.size())
            return false;

        return m_tabs[index].enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabsBase::updateTextSize()
    {
        if ((m_textSize == 0) && !getSharedRenderer()->getTextSize())
            m_textSizeCached = Text::findBestTextSize(m_fontCached, (getSize().y - m_bordersCached.getTopPlusBottom()) * 0.8f);

        for (auto& tab : m_tabs)
            tab.text.setCharacterSize(m_textSizeCached);

        recalculateTabsSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::size_t TabsBase::getTabsCount() const
    {
        return m_tabs.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TabsBase::isMouseOnWidget(Vector2f pos) const
    {
        return FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabsBase::rightMousePressed(Vector2f pos)
    {
        leftMousePressed(pos); // Select the tab on which you clicked

        if (m_selectedTab >= 0)
            onTabRightClick.emit(this, m_tabs[static_cast<std::size_t>(m_selectedTab)].text.getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabsBase::mouseNoLongerOnWidget()
    {
        Widget::mouseNoLongerOnWidget();

        if (m_hoveringTab >= 0)
        {
            m_hoveringTab = -1;
            updateTextColors();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& TabsBase::getSignal(String signalName)
    {
        if (signalName == onTabSelect.getName())
            return onTabSelect;
        if (signalName == onTabRightClick.getName())
            return onTabRightClick;
        return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabsBase::rendererChanged(const String& property)
    {
        if (property == U"Borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            recalculateTabsSize();
        }
        else if (property == U"TextColor")
        {
            m_textColorCached = getSharedRenderer()->getTextColor();
            updateTextColors();
        }
        else if (property == U"TextColorHover")
        {
            m_textColorHoverCached = getSharedRenderer()->getTextColorHover();
            updateTextColors();
        }
        else if (property == U"TextColorDisabled")
        {
            m_textColorDisabledCached = getSharedRenderer()->getTextColorDisabled();
            updateTextColors();
        }
        else if (property == U"SelectedTextColor")
        {
            m_selectedTextColorCached = getSharedRenderer()->getSelectedTextColor();
            updateTextColors();
        }
        else if (property == U"SelectedTextColorHover")
        {
            m_selectedTextColorHoverCached = getSharedRenderer()->getSelectedTextColorHover();
            updateTextColors();
        }
        else if (property == U"TextureTab")
        {
            m_spriteTab.setTexture(getSharedRenderer()->getTextureTab());
        }
        else if (property == U"TextureTabHover")
        {
            m_spriteTabHover.setTexture(getSharedRenderer()->getTextureTabHover());
        }
        else if (property == U"TextureSelectedTab")
        {
            m_spriteSelectedTab.setTexture(getSharedRenderer()->getTextureSelectedTab());
        }
        else if (property == U"TextureSelectedTabHover")
        {
            m_spriteSelectedTabHover.setTexture(getSharedRenderer()->getTextureSelectedTabHover());
        }
        else if (property == U"TextureDisabledTab")
        {
            m_spriteDisabledTab.setTexture(getSharedRenderer()->getTextureDisabledTab());
        }
        else if (property == U"DistanceToSide")
        {
            m_distanceToSideCached = getSharedRenderer()->getDistanceToSide();
            recalculateTabsSize();
        }
        else if (property == U"BackgroundColor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == U"BackgroundColorHover")
        {
            m_backgroundColorHoverCached = getSharedRenderer()->getBackgroundColorHover();
        }
        else if (property == U"BackgroundColorDisabled")
        {
            m_backgroundColorDisabledCached = getSharedRenderer()->getBackgroundColorDisabled();
        }
        else if (property == U"SelectedBackgroundColor")
        {
            m_selectedBackgroundColorCached = getSharedRenderer()->getSelectedBackgroundColor();
        }
        else if (property == U"SelectedBackgroundColorHover")
        {
            m_selectedBackgroundColorHoverCached = getSharedRenderer()->getSelectedBackgroundColorHover();
        }
        else if (property == U"BorderColor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == U"BorderColorHover")
        {
            m_borderColorHoverCached = getSharedRenderer()->getBorderColorHover();
        }
        else if (property == U"SelectedBorderColor")
        {
            m_selectedBorderColorCached = getSharedRenderer()->getSelectedBorderColor();
        }
        else if (property == U"SelectedBorderColorHover")
        {
            m_selectedBorderColorHoverCached = getSharedRenderer()->getSelectedBorderColorHover();
        }
        else if (property == U"RoundedBorderRadius")
        {
            m_roundedBorderRadiusCached = getSharedRenderer()->getRoundedBorderRadius();
        }
        else if ((property == U"Opacity") || (property == U"OpacityDisabled"))
        {
            Widget::rendererChanged(property);

            m_spriteTab.setOpacity(m_opacityCached);
            m_spriteTabHover.setOpacity(m_opacityCached);
            m_spriteSelectedTab.setOpacity(m_opacityCached);
            m_spriteSelectedTabHover.setOpacity(m_opacityCached);
            m_spriteDisabledTab.setOpacity(m_opacityCached);

            for (auto& tab : m_tabs)
                tab.text.setOpacity(m_opacityCached);
        }
        else if (property == U"Font")
        {
            Widget::rendererChanged(property);

            for (auto& tab : m_tabs)
                tab.text.setFont(m_fontCached);

            // Recalculate the size when the text is auto sizing
            if ((m_textSize == 0) && !getSharedRenderer()->getTextSize())
                updateTextSize();
            else
                recalculateTabsSize();
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> TabsBase::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (!m_tabs.empty())
        {
            bool allTabsVisible = true;
            bool allTabsEnabled = true;
            String tabList = "[" + Serializer::serialize(getText(0));
            String tabVisibleList = "[" + Serializer::serialize(getTabVisible(0));
            String tabEnabledList = "[" + Serializer::serialize(getTabEnabled(0));
            for (std::size_t i = 1; i < m_tabs.size(); ++i)
            {
                tabList += ", " + Serializer::serialize(getText(i));
                tabVisibleList += ", " + Serializer::serialize(getTabVisible(i));
                tabEnabledList += ", " + Serializer::serialize(getTabEnabled(i));

                if (!getTabVisible(i))
                    allTabsVisible = false;
                if (!getTabEnabled(i))
                    allTabsEnabled = false;
            }

            tabList += "]";
            tabVisibleList += "]";
            tabEnabledList += "]";

            node->propertyValuePairs[U"Tabs"] = std::make_unique<DataIO::ValueNode>(tabList);
            if (!allTabsVisible)
                node->propertyValuePairs[U"TabsVisible"] = std::make_unique<DataIO::ValueNode>(tabVisibleList);
            if (!allTabsEnabled)
                node->propertyValuePairs[U"TabsEnabled"] = std::make_unique<DataIO::ValueNode>(tabEnabledList);
        }

        if (getSelectedIndex() >= 0)
            node->propertyValuePairs[U"Selected"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(getSelectedIndex()));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabsBase::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs[U"Tabs"])
        {
            if (!node->propertyValuePairs[U"Tabs"]->listNode)
                throw Exception{U"Failed to parse 'Tabs' property, expected a list as value"};

            for (const auto& tabText : node->propertyValuePairs[U"Tabs"]->valueList)
                add(Deserializer::deserialize(ObjectConverter::Type::String, tabText).getString(), false);
        }

        if (node->propertyValuePairs[U"TabsVisible"])
        {
            if (!node->propertyValuePairs[U"TabsVisible"]->listNode)
                throw Exception{U"Failed to parse 'TabsVisible' property, expected a list as value"};

            const auto& values = node->propertyValuePairs[U"TabsVisible"]->valueList;
            for (std::size_t i = 0; i < values.size(); ++i)
                setTabVisible(i, Deserializer::deserialize(ObjectConverter::Type::Bool, values[i]).getBool());
        }

        if (node->propertyValuePairs[U"TabsEnabled"])
        {
            if (!node->propertyValuePairs[U"TabsEnabled"]->listNode)
                throw Exception{U"Failed to parse 'TabsEnabled' property, expected a list as value"};

            const auto& values = node->propertyValuePairs[U"TabsEnabled"]->valueList;
            for (std::size_t i = 0; i < values.size(); ++i)
                setTabEnabled(i, Deserializer::deserialize(ObjectConverter::Type::Bool, values[i]).getBool());
        }

        if (node->propertyValuePairs[U"Selected"])
            select(node->propertyValuePairs[U"Selected"]->value.toUInt());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TabsBase::updateTextColors()
    {
        for (auto& tab : m_tabs)
        {
            if ((!m_enabled || !tab.enabled) && m_textColorDisabledCached.isSet())
                tab.text.setColor(m_textColorDisabledCached);
            else
                tab.text.setColor(m_textColorCached);
        }

        if (m_enabled)
        {
            if (m_selectedTab >= 0)
            {
                if ((m_selectedTab == m_hoveringTab) && m_selectedTextColorHoverCached.isSet())
                    m_tabs[static_cast<std::size_t>(m_selectedTab)].text.setColor(m_selectedTextColorHoverCached);
                else if (m_selectedTextColorCached.isSet())
                    m_tabs[static_cast<std::size_t>(m_selectedTab)].text.setColor(m_selectedTextColorCached);
            }

            if ((m_hoveringTab >= 0) && (m_selectedTab != m_hoveringTab))
            {
                if (m_textColorHoverCached.isSet())
                    m_tabs[static_cast<std::size_t>(m_hoveringTab)].text.setColor(m_textColorHoverCached);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
