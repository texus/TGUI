/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2025 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/Widgets/GrowHorizontalLayout.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char GrowHorizontalLayout::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GrowHorizontalLayout::GrowHorizontalLayout(const char* typeName, bool initRenderer) :
        BoxLayout{typeName, false}
    {
        if (initRenderer)
        {
            m_renderer = makeCopied<BoxLayoutRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GrowHorizontalLayout::GrowHorizontalLayout(const GrowHorizontalLayout& other) :
        BoxLayout      {other},
        m_widgetLayouts{}
    {
        GrowHorizontalLayout::updateWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GrowHorizontalLayout::GrowHorizontalLayout(GrowHorizontalLayout&& other) noexcept :
        BoxLayout      {std::move(other)},
        m_widgetLayouts{}
    {
        GrowHorizontalLayout::updateWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GrowHorizontalLayout& GrowHorizontalLayout::operator=(const GrowHorizontalLayout& other)
    {
        if (&other != this)
        {
            BoxLayout::operator=(other);
            m_widgetLayouts = {};

            GrowHorizontalLayout::updateWidgets();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GrowHorizontalLayout& GrowHorizontalLayout::operator=(GrowHorizontalLayout&& other) noexcept
    {
        if (&other != this)
        {
            BoxLayout::operator=(std::move(other));
            m_widgetLayouts = {};

            GrowHorizontalLayout::updateWidgets();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GrowHorizontalLayout::Ptr GrowHorizontalLayout::create(const Layout& height)
    {
        auto layout = std::make_shared<GrowHorizontalLayout>();
        layout->setHeight(height);
        return layout;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GrowHorizontalLayout::Ptr GrowHorizontalLayout::copy(const GrowHorizontalLayout::ConstPtr& layout)
    {
        if (layout)
            return std::static_pointer_cast<GrowHorizontalLayout>(layout->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GrowHorizontalLayout::updateWidgets()
    {
        m_widgetLayouts.clear();

        const float contentHeight = std::max(0.f, getSize().y - m_paddingCached.getTopPlusBottom());

        float currentOffset = 0;
        for (const auto& widget : m_widgets)
        {
            widget->setPosition({currentOffset, 0});
            widget->setHeight(contentHeight);

            // Correct the size for widgets that are bigger than what you set (e.g. have borders around it or a text next to them)
            if (widget->getFullSize() != widget->getSize())
            {
                const float newHeight = widget->getSize().y - (widget->getFullSize().y - widget->getSize().y);
                if (newHeight > 0)
                {
                    widget->setHeight(newHeight);
                    widget->setPosition(widget->getPosition() - widget->getWidgetOffset());
                }
            }

            currentOffset += widget->getFullSize().x + m_spaceBetweenWidgetsCached;
        }

        float layoutWidth = currentOffset + m_paddingCached.getLeftPlusRight();
        if (!m_widgets.empty())
            layoutWidth -= m_spaceBetweenWidgetsCached; // There is no padding below the last widget

        // Call the function from Widget instead of BoxLayout to prevent an infinite loop
        // NOLINTNEXTLINE(bugprone-parent-virtual-call)
        Widget::setSize({layoutWidth, getSizeLayout().y});

        // Bind layouts to the child widgets so that we can recalculate the positions when the width of one of them changes
        m_widgetLayouts.reserve(m_widgets.size());
        for (const auto& widget : m_widgets)
        {
            m_widgetLayouts.push_back(bindWidth(widget));
            m_widgetLayouts.back().connectWidget(this, true, [this]{ updateWidgets(); });
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr GrowHorizontalLayout::clone() const
    {
        return std::make_shared<GrowHorizontalLayout>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f GrowHorizontalLayout::getInnerSize() const
    {
        return {0.f, BoxLayout::getInnerSize().y};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GrowHorizontalLayout::removeAllWidgets()
    {
        BoxLayout::removeAllWidgets();
        updateWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
