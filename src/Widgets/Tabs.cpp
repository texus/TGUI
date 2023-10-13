/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
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

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char Tabs::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tabs::Tabs(const char* typeName, bool initRenderer) :
        Widget{typeName, false},
        m_distanceToSideCached(std::round(Text::getLineHeight(m_fontCached, getGlobalTextSize()) * 0.4f))
    {
        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<TabsRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }

        setTextSize(getGlobalTextSize());
        setTabHeight(std::round(Text::getLineHeight(m_fontCached, m_textSizeCached) * 1.25f) + m_bordersCached.getTop() + m_bordersCached.getBottom());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tabs::Ptr Tabs::create()
    {
        return std::make_shared<Tabs>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tabs::Ptr Tabs::copy(const Tabs::ConstPtr& tabs)
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

    void Tabs::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        // Tabs is no longer auto-sizing
        m_autoSize = false;
        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setEnabled(bool enabled)
    {
        Widget::setEnabled(enabled);
        updateTextColors();
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

    std::size_t Tabs::add(const String& text, bool selectTab)
    {
        // Use the insert function to put the tab in the right place
        insert(m_tabs.size(), text, selectTab);

        // Return the index of the new tab
        return m_tabs.size()-1;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::insert(std::size_t index, const String& text, bool selectTab)
    {
        // If the index is too high then just insert at the end
        if (index > m_tabs.size())
            index = m_tabs.size();

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
        recalculateTabsWidth();

        // New hovered tab depends on several factors, we keep it simple and just remove the hover state
        m_hoveringTab = -1;

        if (m_selectedTab >= static_cast<int>(index))
            m_selectedTab++;

        // If the tab has to be selected then do so
        if (selectTab)
            select(index);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String Tabs::getText(std::size_t index) const
    {
        if (index >= m_tabs.size())
            return "";
        else
            return m_tabs[index].text.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::changeText(std::size_t index, const String& text)
    {
        if (index >= m_tabs.size())
            return false;

        m_tabs[index].text.setString(text);
        recalculateTabsWidth();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::select(const String& text)
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

    bool Tabs::select(std::size_t index)
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

    void Tabs::deselect()
    {
        if (m_selectedTab >= 0)
        {
            updateTextColors();
            m_selectedTab = -1;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::remove(const String& text)
    {
        for (std::size_t i = 0; i < m_tabs.size(); ++i)
        {
            if (m_tabs[i].text.getString() == text)
                return remove(i);
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::remove(std::size_t index)
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
        recalculateTabsWidth();
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::removeAll()
    {
        m_tabs.clear();
        m_selectedTab = -1;
        m_hoveringTab = -1;

        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String Tabs::getSelected() const
    {
        if (m_selectedTab >= 0)
            return m_tabs[static_cast<std::size_t>(m_selectedTab)].text.getString();
        else
            return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int Tabs::getSelectedIndex() const
    {
        return m_selectedTab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setTabVisible(std::size_t index, bool visible)
    {
        if (index >= m_tabs.size())
            return;

        m_tabs[index].visible = visible;
        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::getTabVisible(std::size_t index) const
    {
        if (index >= m_tabs.size())
            return false;

        return m_tabs[index].visible;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setTabEnabled(std::size_t index, bool enabled)
    {
        if (index >= m_tabs.size())
            return;

        m_tabs[index].enabled = enabled;
        updateTextColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::getTabEnabled(std::size_t index) const
    {
        if (index >= m_tabs.size())
            return false;

        return m_tabs[index].enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::updateTextSize()
    {
        if ((m_textSize == 0) && !getSharedRenderer()->getTextSize())
            m_textSizeCached = Text::findBestTextSize(m_fontCached, (getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()) * 0.8f);

        for (auto& tab : m_tabs)
            tab.text.setCharacterSize(m_textSizeCached);

        recalculateTabsWidth();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setTabHeight(float height)
    {
        Widget::setSize({m_size.x, height});
        m_bordersCached.updateParentSize(getSize());

        // Recalculate the size when the text is auto sizing
        if ((m_textSize == 0) && !getSharedRenderer()->getTextSize())
            updateTextSize();
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
        return m_tabs.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::isMouseOnWidget(Vector2f pos) const
    {
        return FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        float width = m_bordersCached.getLeft() / 2.f;
        for (std::size_t i = 0; i < m_tabs.size(); ++i)
        {
            if (!m_tabs[i].visible)
                continue;

            // Append the width of the tab
            width += (m_bordersCached.getLeft() / 2.f) + m_tabs[i].width + (m_bordersCached.getRight() / 2.0f);

            // If the mouse went down on this tab then select it
            if (pos.x < width)
            {
                select(i);
                break;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::mouseMoved(Vector2f pos)
    {
        Widget::mouseMoved(pos);

        const int oldHoveringTab = m_hoveringTab;

        pos -= getPosition();
        m_hoveringTab = -1;
        float width = m_bordersCached.getLeft() / 2.f;

        for (std::size_t i = 0; i < m_tabs.size(); ++i)
        {
            if (!m_tabs[i].visible)
                continue;

            // Append the width of the tab
            width += (m_bordersCached.getLeft() / 2.f) + m_tabs[i].width + (m_bordersCached.getRight() / 2.0f);

            // If the mouse is on top of this tab then remember it
            if (pos.x < width)
            {
                if (m_tabs[i].enabled)
                    m_hoveringTab = static_cast<int>(i);

                break;
            }
        }

        if (m_hoveringTab != oldHoveringTab)
            updateTextColors();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::mouseNoLongerOnWidget()
    {
        Widget::mouseNoLongerOnWidget();

        if (m_hoveringTab >= 0)
        {
            m_hoveringTab = -1;
            updateTextColors();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::recalculateTabsWidth()
    {
        std::size_t visibleTabs = 0;
        for (const auto& tab : m_tabs)
        {
            if (tab.visible)
                visibleTabs++;
        }

        if (m_autoSize)
        {
            m_autoLayout = AutoLayout::Manual;
            if (m_tabs.empty())
                Widget::setSize({m_bordersCached.getLeft() + m_bordersCached.getRight(), getSizeLayout().y});
            else
            {
                // First calculate the width of the tabs as if there aren't any borders
                float totalWidth = 0;
                for (auto& tab : m_tabs)
                {
                    if (!tab.visible)
                        continue;

                    tab.width = tab.text.getSize().x + std::max(m_minimumTabWidth, 2 * m_distanceToSideCached);
                    if ((m_maximumTabWidth > 0) && (m_maximumTabWidth < tab.width))
                        tab.width = m_maximumTabWidth;

                    totalWidth += tab.width;
                }

                // Now add the borders to the tabs
                totalWidth += (visibleTabs + 1) * ((m_bordersCached.getLeft() + m_bordersCached.getRight()) / 2.f);

                Widget::setSize({totalWidth, getSizeLayout().y});
            }
        }
        else // A size was provided
        {
            const float tabWidth = (getSize().x - ((visibleTabs + 1) * ((m_bordersCached.getLeft() + m_bordersCached.getRight()) / 2.f))) / visibleTabs;
            for (auto& tab : m_tabs)
                tab.width = tabWidth;
        }

        m_bordersCached.updateParentSize(getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Tabs::getSignal(String signalName)
    {
        if (signalName == onTabSelect.getName())
            return onTabSelect;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::rendererChanged(const String& property)
    {
        if (property == U"Borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            recalculateTabsWidth();
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
            recalculateTabsWidth();
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
                recalculateTabsWidth();
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Tabs::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (m_tabs.size() > 0)
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

        if (m_maximumTabWidth > 0)
            node->propertyValuePairs[U"MaximumTabWidth"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_maximumTabWidth));

        if (m_autoSize)
        {
            node->propertyValuePairs.erase("Size");
            node->propertyValuePairs[U"TabHeight"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(getSize().y));
        }

        node->propertyValuePairs[U"AutoSize"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(m_autoSize));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
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

        if (node->propertyValuePairs[U"MaximumTabWidth"])
            setMaximumTabWidth(node->propertyValuePairs[U"MaximumTabWidth"]->value.toFloat());
        if (node->propertyValuePairs[U"TabHeight"])
            setTabHeight(node->propertyValuePairs[U"TabHeight"]->value.toFloat());
        if (node->propertyValuePairs[U"Selected"])
            select(node->propertyValuePairs[U"Selected"]->value.toUInt());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::updateTextColors()
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

    void Tabs::draw(BackendRenderTarget& target, RenderStates states) const
    {
        // Draw the borders around the tabs
        if (m_bordersCached != Borders{0})
        {
            target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorCached, m_opacityCached));
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }

        const float borderWidth = (m_bordersCached.getLeft() + m_bordersCached.getRight()) / 2.f;
        const float usableHeight = getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom();
        for (std::size_t i = 0; i < m_tabs.size(); ++i)
        {
            if (!m_tabs[i].visible)
                continue;

            RenderStates textStates = states;

            // Draw the background of the tab
            const Sprite* spriteTab = nullptr;
            if ((!m_enabled || !m_tabs[i].enabled) && m_spriteDisabledTab.isSet())
                spriteTab = &m_spriteDisabledTab;
            else if ((m_selectedTab == static_cast<int>(i)) && m_spriteSelectedTab.isSet())
            {
                if ((m_hoveringTab == static_cast<int>(i)) && m_spriteSelectedTabHover.isSet())
                    spriteTab = &m_spriteSelectedTabHover;
                else
                    spriteTab = &m_spriteSelectedTab;
            }
            else if ((m_hoveringTab == static_cast<int>(i)) && m_spriteTabHover.isSet())
                spriteTab = &m_spriteTabHover;
            else if (m_spriteTab.isSet())
                spriteTab = &m_spriteTab;

            if (spriteTab)
            {
                Sprite spriteTabCopy = *spriteTab;
                spriteTabCopy.setSize({m_tabs[i].width, usableHeight});
                target.drawSprite(states, spriteTabCopy);
            }
            else // No texture was loaded
            {
                if ((!m_enabled || !m_tabs[i].enabled) && m_backgroundColorDisabledCached.isSet())
                    target.drawFilledRect(states, {m_tabs[i].width, usableHeight}, Color::applyOpacity(m_backgroundColorDisabledCached, m_opacityCached));
                else if (m_selectedTab == static_cast<int>(i))
                {
                    if ((m_hoveringTab == static_cast<int>(i)) && m_selectedBackgroundColorHoverCached.isSet())
                        target.drawFilledRect(states, {m_tabs[i].width, usableHeight}, Color::applyOpacity(m_selectedBackgroundColorHoverCached, m_opacityCached));
                    else
                        target.drawFilledRect(states, {m_tabs[i].width, usableHeight}, Color::applyOpacity(m_selectedBackgroundColorCached, m_opacityCached));
                }
                else if ((m_hoveringTab == static_cast<int>(i)) && m_backgroundColorHoverCached.isSet())
                    target.drawFilledRect(states, {m_tabs[i].width, usableHeight}, Color::applyOpacity(m_backgroundColorHoverCached, m_opacityCached));
                else
                    target.drawFilledRect(states, {m_tabs[i].width, usableHeight}, Color::applyOpacity(m_backgroundColorCached, m_opacityCached));
            }

            // Draw the borders between the tabs
            states.transform.translate({m_tabs[i].width, 0});
            if ((borderWidth != 0) && (i < m_tabs.size() - 1))
            {
                target.drawFilledRect(states, {borderWidth, usableHeight}, Color::applyOpacity(m_borderColorCached, m_opacityCached));
                states.transform.translate({borderWidth, 0});
            }

            // Highlight the borders of the selected and hovered tab when requested
            if (m_bordersCached != Borders{0})
            {
                Color highlightColor;
                if ((m_hoveringTab == static_cast<int>(i)) && m_borderColorHoverCached.isSet())
                    highlightColor = m_borderColorHoverCached;
                if (m_selectedTab == static_cast<int>(i))
                {
                    if ((m_hoveringTab == static_cast<int>(i)) && m_selectedBorderColorHoverCached.isSet())
                        highlightColor = m_selectedBorderColorHoverCached;
                    else if (m_selectedBorderColorCached.isSet())
                        highlightColor = m_selectedBorderColorCached;
                }

                if (highlightColor.isSet())
                {
                    float leftBorderWidth = borderWidth;
                    float rightBorderWidth = borderWidth;
                    if (i == 0)
                        leftBorderWidth = m_bordersCached.getLeft();
                    if (i == m_tabs.size() - 1)
                        rightBorderWidth = m_bordersCached.getRight();

                    if ((m_selectedTab >= 0) && (m_hoveringTab >= 0) && (m_borderColorHoverCached.isSet()
                        && (m_selectedBorderColorCached.isSet() || m_selectedBorderColorHoverCached.isSet())))
                    {
                        if ((m_selectedTab == static_cast<int>(i - 1)) || (m_hoveringTab == static_cast<int>(i - 1)))
                            leftBorderWidth /= 2;
                        else if ((m_selectedTab == static_cast<int>(i + 1)) || (m_hoveringTab == static_cast<int>(i + 1)))
                            rightBorderWidth /= 2;
                    }

                    RenderStates highlightStates = states;
                    if (i < m_tabs.size() - 1)
                        highlightStates.transform.translate({-borderWidth, 0});
                    highlightStates.transform.translate({-m_tabs[i].width - leftBorderWidth, -m_bordersCached.getTop()});
                    target.drawBorders(highlightStates, {leftBorderWidth, m_bordersCached.getTop(), rightBorderWidth, m_bordersCached.getBottom()},
                                       {m_tabs[i].width + leftBorderWidth + rightBorderWidth, getSize().y}, highlightColor);
                }
            }

            // Apply clipping if required for the text in this tab
            const float usableWidth = m_tabs[i].width - (2 * m_distanceToSideCached);
            const bool clippingRequired = (m_tabs[i].text.getSize().x > usableWidth);
            if (clippingRequired)
                target.addClippingLayer(textStates, {{m_distanceToSideCached, 0}, {usableWidth, usableHeight}});

            // Draw the text
            textStates.transform.translate({m_distanceToSideCached + ((usableWidth - m_tabs[i].text.getSize().x) / 2.f), ((usableHeight - m_tabs[i].text.getSize().y) / 2.f)});
            target.drawText(textStates, m_tabs[i].text);

            if (clippingRequired)
                target.removeClippingLayer();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Tabs::clone() const
    {
        return std::make_shared<Tabs>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
