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


#include <TGUI/Widgets/Group.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char Group::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group::Group(const char* typeName, bool initRenderer) :
        Container{typeName, false}
    {
        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<GroupRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group::Ptr Group::create(const Layout2d& size)
    {
        auto group = std::make_shared<Group>();
        group->setSize(size);
        return group;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Group::Ptr Group::copy(const Group::ConstPtr& group)
    {
        if (group)
            return std::static_pointer_cast<Group>(group->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GroupRenderer* Group::getSharedRenderer()
    {
        return aurora::downcast<GroupRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const GroupRenderer* Group::getSharedRenderer() const
    {
        return aurora::downcast<const GroupRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GroupRenderer* Group::getRenderer()
    {
        return aurora::downcast<GroupRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::setSize(const Layout2d& size)
    {
        m_paddingCached.updateParentSize(size.getValue());

        Container::setSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Group::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight()),
                std::max(0.f, getSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Group::getChildWidgetsOffset() const
    {
        return {m_paddingCached.getLeft(), m_paddingCached.getTop()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Group::isMouseOnWidget(Vector2f pos) const
    {
        pos -= getPosition();

        if (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos))
        {
            const Vector2f offset = getChildWidgetsOffset();
            for (const auto& widget : m_widgets)
            {
                if (widget->isVisible())
                {
                    if (widget->isMouseOnWidget(transformMousePos(widget, pos - offset)))
                        return true;
                }
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::rendererChanged(const String& property)
    {
        if (property == U"Padding")
        {
            m_paddingCached = getSharedRenderer()->getPadding();
            setSize(m_size);
        }
        else
            Container::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Group::draw(BackendRenderTarget& target, RenderStates states) const
    {
        states.transform.translate({m_paddingCached.getLeft(), m_paddingCached.getTop()});

        const Vector2f innerSize = {getSize().x - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                                    getSize().y - m_paddingCached.getTop() - m_paddingCached.getBottom()};

        target.addClippingLayer(states, {{}, innerSize});

        Container::draw(target, states);

        target.removeClippingLayer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Group::clone() const
    {
        return std::make_shared<Group>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
