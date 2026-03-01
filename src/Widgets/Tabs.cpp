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

#include <algorithm>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr char Tabs::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Tabs::Tabs(const char* typeName, bool initRenderer) :
        TabsBase{typeName, false}
    {
        if (initRenderer)
        {
            m_renderer = makeCopied<TabsRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));

            setTextSize(getGlobalTextSize());
            setTabHeight(std::round(Text::getLineHeight(m_fontCached, m_textSizeCached) * 1.25f) + m_bordersCached.getTopPlusBottom());
        }
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
        return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setSize(const Layout2d& size)
    {
        TabsBase::setSize(size);

        // Tabs is no longer auto-sizing
        m_autoSize = false;
        recalculateTabsSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setAutoSize(bool autoSize)
    {
        if (m_autoSize != autoSize)
        {
            m_autoSize = autoSize;
            recalculateTabsSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Tabs::getAutoSize() const
    {
        return m_autoSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setTabHeight(float height)
    {
        TabsBase::setSize({m_size.x, height});
        m_bordersCached.updateParentSize(getSize());

        // Recalculate the size when the text is auto sizing
        if ((m_textSize == 0) && !getSharedRenderer()->getTextSize())
            updateTextSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::setMaximumTabWidth(float maximumWidth)
    {
        m_maximumTabWidth = maximumWidth;

        recalculateTabsSize();
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

        recalculateTabsSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Tabs::getMinimumTabWidth() const
    {
        return m_minimumTabWidth;
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
        TabsBase::mouseMoved(pos);

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

    void Tabs::recalculateTabsSize()
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
            if (visibleTabs == 0)
                TabsBase::setSize({m_bordersCached.getLeftPlusRight(), getSizeLayout().y});
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
                totalWidth += (visibleTabs + 1) * ((m_bordersCached.getLeftPlusRight()) / 2.f);

                TabsBase::setSize({totalWidth, getSizeLayout().y});
            }
        }
        else // A size was provided
        {
            if (visibleTabs > 0)
            {
                const float tabWidth = (getSize().x - ((visibleTabs + 1) * ((m_bordersCached.getLeftPlusRight()) / 2.f))) / visibleTabs;
                for (auto& tab : m_tabs)
                    tab.width = tabWidth;
            }
        }

        m_bordersCached.updateParentSize(getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Tabs::save(SavingRenderersMap& renderers) const
    {
        auto node = TabsBase::save(renderers);

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
        TabsBase::load(node, renderers);

        if (node->propertyValuePairs[U"MaximumTabWidth"])
            setMaximumTabWidth(node->propertyValuePairs[U"MaximumTabWidth"]->value.toFloat());
        if (node->propertyValuePairs[U"TabHeight"])
            setTabHeight(node->propertyValuePairs[U"TabHeight"]->value.toFloat());
        if (node->propertyValuePairs[U"AutoSize"])
            setAutoSize(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"AutoSize"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Tabs::draw(BackendRenderTarget& target, RenderStates states) const
    {
        const float borderWidth = (m_bordersCached.getLeftPlusRight()) / 2.f;
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
            states.transform.translate({borderWidth, 0});

        const float usableHeight = getSize().y - m_bordersCached.getTopPlusBottom();
        for (std::size_t i = 0; i < m_tabs.size(); ++i)
        {
            if (!m_tabs[i].visible)
                continue;

            RenderStates textStates = states;
            if (roundedCorners)
                textStates.transform.translate({0, m_bordersCached.getTop()});

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
                states.transform.translate({-borderWidth, 0});
                target.drawRoundedRectangle(states,
                                            {m_tabs[i].width + (2 * borderWidth), getSize().y},
                                            Color::applyOpacity(backgroundColor, m_opacityCached),
                                            m_roundedBorderRadiusCached,
                                            m_bordersCached,
                                            Color::applyOpacity(m_borderColorCached, m_opacityCached));
                states.transform.translate({m_tabs[i].width + (2 * borderWidth), 0});
            }
            else
            {
                if (spriteTab)
                {
                    Sprite spriteTabCopy = *spriteTab;
                    spriteTabCopy.setSize({m_tabs[i].width, usableHeight});
                    target.drawSprite(states, spriteTabCopy);
                }
                else // No texture was loaded
                    target.drawFilledRect(states, {m_tabs[i].width, usableHeight}, Color::applyOpacity(backgroundColor, m_opacityCached));

                // Draw the borders between the tabs
                states.transform.translate({m_tabs[i].width, 0});
                if ((borderWidth != 0) && (i < m_tabs.size() - 1))
                {
                    target.drawFilledRect(states, {borderWidth, usableHeight}, Color::applyOpacity(m_borderColorCached, m_opacityCached));
                    states.transform.translate({borderWidth, 0});
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
                    float leftBorderWidth = borderWidth;
                    float rightBorderWidth = borderWidth;
                    if (i == 0)
                        leftBorderWidth = m_bordersCached.getLeft();
                    if (i == m_tabs.size() - 1)
                        rightBorderWidth = m_bordersCached.getRight();

                    if ((m_selectedTab >= 0) && (m_hoveringTab >= 0)
                        && (m_borderColorHoverCached.isSet()
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
                    target.drawBorders(highlightStates,
                                       {leftBorderWidth, m_bordersCached.getTop(), rightBorderWidth, m_bordersCached.getBottom()},
                                       {m_tabs[i].width + leftBorderWidth + rightBorderWidth, getSize().y},
                                       highlightColor);
                }
            }

            // Apply clipping if required for the text in this tab
            const float usableWidth = m_tabs[i].width - (2 * m_distanceToSideCached);
            const bool clippingRequired = (m_tabs[i].text.getSize().x > usableWidth);
            if (clippingRequired)
                target.addClippingLayer(textStates, {{m_distanceToSideCached, 0}, {usableWidth, usableHeight}});

            // Draw the text
            textStates.transform.translate({m_distanceToSideCached + ((usableWidth - m_tabs[i].text.getSize().x) / 2.f),
                                            ((usableHeight - m_tabs[i].text.getSize().y) / 2.f)});
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
} // namespace tgui

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
