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

#include <TGUI/Widgets/VerticalTabs.hpp>
#include <TGUI/Optional.hpp>

#include <algorithm>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char VerticalTabs::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VerticalTabs::VerticalTabs(const char* typeName, bool initRenderer) :
        TabsBase{typeName, false}
    {
        if (initRenderer)
        {
            m_renderer = makeCopied<TabsRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

            m_distanceToSideCached = 0;
            setTextSize(getGlobalTextSize());
            setTabHeight(std::round(Text::getLineHeight(m_fontCached, m_textSizeCached) * 1.25f) + m_bordersCached.getTopPlusBottom());
            setTabWidth(Text::getLineHeight(m_fontCached, m_textSizeCached) * 10);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VerticalTabs::Ptr VerticalTabs::create()
    {
        return std::make_shared<VerticalTabs>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    VerticalTabs::Ptr VerticalTabs::copy(const VerticalTabs::ConstPtr& tabs)
    {
        if (tabs)
            return std::static_pointer_cast<VerticalTabs>(tabs->clone());
        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void VerticalTabs::setSize(const Layout2d& size)
    {
        TabsBase::setSize(size);
        recalculateTabsSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void VerticalTabs::setTabWidth(float width)
    {
        setSize({width, m_size.y});
        m_bordersCached.updateParentSize(getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void VerticalTabs::setTabHeight(float height)
    {
        m_tabHeight = height;
        recalculateTabsSize();

        // Recalculate the size when the text is auto sizing
        if ((m_textSize == 0) && !getSharedRenderer()->getTextSize())
            updateTextSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float VerticalTabs::getTabHeight() const
    {
        return m_tabHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool VerticalTabs::leftMousePressed(Vector2f pos)
    {
        pos -= getPosition();

        float height = m_bordersCached.getTop() / 2.f;
        for (std::size_t i = 0; i < m_tabs.size(); ++i)
        {
            if (!m_tabs[i].visible)
                continue;

            // Append the width of the tab
            height += (m_bordersCached.getTop() / 2.f) + m_tabHeight + (m_bordersCached.getBottom() / 2.0f);

            // If the mouse went down on this tab then select it
            if (pos.y < height)
            {
                select(i);
                break;
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void VerticalTabs::mouseMoved(Vector2f pos)
    {
        TabsBase::mouseMoved(pos);

        const int oldHoveringTab = m_hoveringTab;

        pos -= getPosition();
        m_hoveringTab = -1;
        float height = m_bordersCached.getTop() / 2.f;

        for (std::size_t i = 0; i < m_tabs.size(); ++i)
        {
            if (!m_tabs[i].visible)
                continue;

            // Append the width of the tab
            height += (m_bordersCached.getTop() / 2.f) + m_tabHeight + (m_bordersCached.getBottom() / 2.0f);

            // If the mouse is on top of this tab then remember it
            if (pos.y < height)
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

    void VerticalTabs::recalculateTabsSize()
    {
        std::size_t visibleTabs = 0;
        for (const auto& tab : m_tabs)
        {
            if (tab.visible)
                visibleTabs++;
        }

        m_autoLayout = AutoLayout::Manual;
        if (visibleTabs == 0)
            TabsBase::setSize({getSizeLayout().x, m_bordersCached.getTopPlusBottom()});
        else
        {
            const float totalHeight = (visibleTabs * m_tabHeight) + ((visibleTabs + 1) * m_bordersCached.getTopPlusBottom() / 2.f);
            TabsBase::setSize({getSizeLayout().x, totalHeight});
        }

        m_bordersCached.updateParentSize(getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> VerticalTabs::save(SavingRenderersMap& renderers) const
    {
        auto node = TabsBase::save(renderers);
        node->propertyValuePairs[U"TabHeight"] = std::make_unique<DataIO::ValueNode>(String::fromNumber(getTabHeight()));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void VerticalTabs::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        TabsBase::load(node, renderers);

        if (node->propertyValuePairs[U"TabHeight"])
            setTabHeight(node->propertyValuePairs[U"TabHeight"]->value.toFloat());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void VerticalTabs::draw(BackendRenderTarget& target, RenderStates states) const
    {
        const float borderHeight = (m_bordersCached.getTopPlusBottom()) / 2.f;
        const bool roundedCorners = (m_roundedBorderRadiusCached > 0) && !m_spriteTab.isSet();
        if (!roundedCorners)
        {
            // Draw the borders around the tabs
            if (m_bordersCached != Borders{0})
            {
                target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorCached, m_opacityCached));
                states.transform.translate(m_bordersCached.getOffset());
            }
        }
        else
            states.transform.translate({0, borderHeight});

        const float usableWidth = getSize().x - m_bordersCached.getLeftPlusRight();
        for (std::size_t i = 0; i < m_tabs.size(); ++i)
        {
            if (!m_tabs[i].visible)
                continue;

            RenderStates textStates = states;
            if (roundedCorners)
                textStates.transform.translate({m_bordersCached.getLeft(), 0});

            Color backgroundColor;
            if ((!m_enabled || !m_tabs[i].enabled) && m_backgroundColorDisabledCached.isSet())
                backgroundColor = m_backgroundColorDisabledCached;
            else if (m_selectedTab == static_cast<int>(i))
            {
                if ((m_hoveringTab == static_cast<int>(i)) && m_selectedBackgroundColorHoverCached.isSet())
                    backgroundColor = m_selectedBackgroundColorHoverCached;
                else
                    backgroundColor = m_selectedBackgroundColorCached;
            }
            else if ((m_hoveringTab == static_cast<int>(i)) && m_backgroundColorHoverCached.isSet())
                backgroundColor = m_backgroundColorHoverCached;
            else
                backgroundColor = m_backgroundColorCached;

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

            if (roundedCorners)
            {
                states.transform.translate({0, -borderHeight});
                target.drawRoundedRectangle(states, {getSize().x, m_tabHeight + (2 * borderHeight)}, Color::applyOpacity(backgroundColor, m_opacityCached),
                                            m_roundedBorderRadiusCached, m_bordersCached, Color::applyOpacity(m_borderColorCached, m_opacityCached));
                states.transform.translate({0, m_tabHeight + 2*borderHeight});
            }
            else
            {
                if (spriteTab)
                {
                    Sprite spriteTabCopy = *spriteTab;
                    spriteTabCopy.setSize({usableWidth, m_tabHeight});
                    target.drawSprite(states, spriteTabCopy);
                }
                else // No texture was loaded
                    target.drawFilledRect(states, {usableWidth, m_tabHeight}, Color::applyOpacity(backgroundColor, m_opacityCached));

                // Draw the borders between the tabs
                states.transform.translate({0, m_tabHeight});
                if ((borderHeight != 0) && (i < m_tabs.size() - 1))
                {
                    target.drawFilledRect(states, {usableWidth, borderHeight}, Color::applyOpacity(m_borderColorCached, m_opacityCached));
                    states.transform.translate({0, borderHeight});
                }
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
                    float topBorderHeight = borderHeight;
                    float bottomBorderHeight = borderHeight;
                    if (i == 0)
                        topBorderHeight = m_bordersCached.getTop();
                    if (i == m_tabs.size() - 1)
                        bottomBorderHeight = m_bordersCached.getBottom();

                    if ((m_selectedTab >= 0) && (m_hoveringTab >= 0) && (m_borderColorHoverCached.isSet()
                        && (m_selectedBorderColorCached.isSet() || m_selectedBorderColorHoverCached.isSet())))
                    {
                        if ((m_selectedTab == static_cast<int>(i - 1)) || (m_hoveringTab == static_cast<int>(i - 1)))
                            topBorderHeight /= 2;
                        else if ((m_selectedTab == static_cast<int>(i + 1)) || (m_hoveringTab == static_cast<int>(i + 1)))
                            bottomBorderHeight /= 2;
                    }

                    RenderStates highlightStates = states;
                    if (i < m_tabs.size() - 1)
                        highlightStates.transform.translate({0, -borderHeight});
                    highlightStates.transform.translate({-m_bordersCached.getLeft(), -m_tabHeight - topBorderHeight});
                    target.drawBorders(highlightStates, {m_bordersCached.getLeft(), topBorderHeight, m_bordersCached.getRight(), bottomBorderHeight},
                                       {getSize().x, m_tabHeight + topBorderHeight + bottomBorderHeight}, highlightColor);
                }
            }

            // Apply clipping if required for the text in this tab
            const float usableHeight = m_tabHeight - m_bordersCached.getTopPlusBottom();
            const float usableTextWidth = usableWidth - (2 * m_distanceToSideCached);
            const bool clippingRequired = (m_tabs[i].text.getSize().x > usableTextWidth);
            if (clippingRequired)
                target.addClippingLayer(textStates, {{m_distanceToSideCached, 0}, {usableTextWidth, usableHeight}});

            // Draw the text
            textStates.transform.translate({m_distanceToSideCached + ((usableTextWidth - m_tabs[i].text.getSize().x) / 2.f), ((usableHeight - m_tabs[i].text.getSize().y) / 2.f)});
            target.drawText(textStates, m_tabs[i].text);

            if (clippingRequired)
                target.removeClippingLayer();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr VerticalTabs::clone() const
    {
        return std::make_shared<VerticalTabs>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
