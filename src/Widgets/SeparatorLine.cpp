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


#include <TGUI/Widgets/SeparatorLine.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char SeparatorLine::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SeparatorLine::SeparatorLine(const char* typeName, bool initRenderer) :
        ClickableWidget{typeName, false}
    {
        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<SeparatorLineRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SeparatorLine::Ptr SeparatorLine::create(const Layout2d& size)
    {
        auto separator = std::make_shared<SeparatorLine>();
        separator->setSize(size);
        return separator;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SeparatorLine::Ptr SeparatorLine::copy(const SeparatorLine::ConstPtr& separator)
    {
        if (separator)
            return std::static_pointer_cast<SeparatorLine>(separator->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SeparatorLineRenderer* SeparatorLine::getSharedRenderer()
    {
        return aurora::downcast<SeparatorLineRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const SeparatorLineRenderer* SeparatorLine::getSharedRenderer() const
    {
        return aurora::downcast<const SeparatorLineRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SeparatorLineRenderer* SeparatorLine::getRenderer()
    {
        return aurora::downcast<SeparatorLineRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SeparatorLine::rendererChanged(const String& property)
    {
        if (property == U"Color")
            m_colorCached = getSharedRenderer()->getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SeparatorLine::draw(BackendRenderTarget& target, RenderStates states) const
    {
        target.drawFilledRect(states, getSize(), Color::applyOpacity(m_colorCached, m_opacityCached));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr SeparatorLine::clone() const
    {
        return std::make_shared<SeparatorLine>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
