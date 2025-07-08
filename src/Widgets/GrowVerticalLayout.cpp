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

#include <TGUI/Widgets/GrowVerticalLayout.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char GrowVerticalLayout::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GrowVerticalLayout::GrowVerticalLayout(const char* typeName, bool initRenderer) :
        BoxLayout{typeName, false}
    {
        if (initRenderer)
        {
            m_renderer = makeCopied<BoxLayoutRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GrowVerticalLayout::GrowVerticalLayout(const GrowVerticalLayout& other) :
        BoxLayout      {other},
        m_widgetLayouts{}
    {
        GrowVerticalLayout::updateWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GrowVerticalLayout::GrowVerticalLayout(GrowVerticalLayout&& other) noexcept :
        BoxLayout      {std::move(other)},
        m_widgetLayouts{}
    {
        GrowVerticalLayout::updateWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GrowVerticalLayout& GrowVerticalLayout::operator=(const GrowVerticalLayout& other)
    {
        if (&other != this)
        {
            BoxLayout::operator=(other);
            m_widgetLayouts = {};

            GrowVerticalLayout::updateWidgets();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GrowVerticalLayout& GrowVerticalLayout::operator=(GrowVerticalLayout&& other) noexcept
    {
        if (&other != this)
        {
            BoxLayout::operator=(std::move(other));
            m_widgetLayouts = {};

            GrowVerticalLayout::updateWidgets();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GrowVerticalLayout::Ptr GrowVerticalLayout::create(const Layout& width)
    {
        auto layout = std::make_shared<GrowVerticalLayout>();
        layout->setWidth(width);
        return layout;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GrowVerticalLayout::Ptr GrowVerticalLayout::copy(const GrowVerticalLayout::ConstPtr& layout)
    {
        if (layout)
            return std::static_pointer_cast<GrowVerticalLayout>(layout->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GrowVerticalLayout::updateWidgets()
    {
        m_widgetLayouts.clear();

        const float contentWidth = std::max(0.f, getSize().x - m_paddingCached.getLeftPlusRight());

        float currentOffset = 0;
        for (const auto& widget : m_widgets)
        {
            widget->setPosition({0, currentOffset});
            widget->setWidth(contentWidth);

            // Correct the size for widgets that are bigger than what you set (e.g. have borders around it or a text next to them)
            if (widget->getFullSize().x != widget->getSize().x)
            {
                const float newWidth = widget->getSize().x - (widget->getFullSize().x - widget->getSize().x);
                if (newWidth > 0)
                {
                    widget->setWidth(newWidth);
                    widget->setPosition(widget->getPosition() - widget->getWidgetOffset());
                }
            }

            currentOffset += widget->getFullSize().y + m_spaceBetweenWidgetsCached;
        }

        float layoutHeight = currentOffset + m_paddingCached.getTopPlusBottom();
        if (!m_widgets.empty())
            layoutHeight -= m_spaceBetweenWidgetsCached; // There is no padding below the last widget

        // Call the function from Widget instead of BoxLayout to prevent an infinite loop
        // NOLINTNEXTLINE(bugprone-parent-virtual-call)
        Widget::setSize({getSizeLayout().x, layoutHeight});

        // Bind layouts to the child widgets so that we can recalculate the positions when the height of one of them changes
        m_widgetLayouts.reserve(m_widgets.size());
        for (const auto& widget : m_widgets)
        {
            m_widgetLayouts.push_back(bindHeight(widget));
            m_widgetLayouts.back().connectWidget(this, false, [this]{ updateWidgets(); });
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr GrowVerticalLayout::clone() const
    {
        return std::make_shared<GrowVerticalLayout>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f GrowVerticalLayout::getInnerSize() const
    {
        return {BoxLayout::getInnerSize().x, 0.f};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GrowVerticalLayout::removeAllWidgets()
    {
        BoxLayout::removeAllWidgets();
        updateWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
