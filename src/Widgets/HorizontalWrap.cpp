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


#include <TGUI/Widgets/HorizontalWrap.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char HorizontalWrap::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    HorizontalWrap::HorizontalWrap(const char* typeName, bool initRenderer) :
        BoxLayout{typeName, false}
    {
        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<BoxLayoutRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    HorizontalWrap::Ptr HorizontalWrap::create(const Layout2d& size)
    {
        auto layout = std::make_shared<HorizontalWrap>();
        layout->setSize(size);
        return layout;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    HorizontalWrap::Ptr HorizontalWrap::copy(const HorizontalWrap::ConstPtr& layout)
    {
        if (layout)
            return std::static_pointer_cast<HorizontalWrap>(layout->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void HorizontalWrap::updateWidgets()
    {
        const Vector2f contentSize = {getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                                      getSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()};

        float currentHorizontalOffset = 0;
        float currentVerticalOffset = 0;
        float lineHeight = 0;
        for (const auto& widget : m_widgets)
        {
            const auto size = widget->getSize();

            if (currentHorizontalOffset + size.x > contentSize.x)
            {
                currentVerticalOffset += lineHeight + m_spaceBetweenWidgetsCached;
                currentHorizontalOffset = 0;
                lineHeight = 0;
            }

            widget->setPosition({currentHorizontalOffset, currentVerticalOffset});

            currentHorizontalOffset += size.x + m_spaceBetweenWidgetsCached;

            if (lineHeight < size.y)
                lineHeight = size.y;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr HorizontalWrap::clone() const
    {
        return std::make_shared<HorizontalWrap>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
