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


#include <TGUI/Widgets/Panel.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char Panel::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Panel(const char* typeName, bool initRenderer) :
        Group{typeName, false}
    {
        m_isolatedFocus = true;

        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<PanelRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr Panel::create(const Layout2d& size)
    {
        auto panel = std::make_shared<Panel>();
        panel->setSize(size);
        return panel;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Ptr Panel::copy(const Panel::ConstPtr& panel)
    {
        if (panel)
            return std::static_pointer_cast<Panel>(panel->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    PanelRenderer* Panel::getSharedRenderer()
    {
        return aurora::downcast<PanelRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const PanelRenderer* Panel::getSharedRenderer() const
    {
        return aurora::downcast<const PanelRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    PanelRenderer* Panel::getRenderer()
    {
        return aurora::downcast<PanelRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::setSize(const Layout2d& size)
    {
        m_bordersCached.updateParentSize(size.getValue());

        Group::setSize(size);

        m_spriteBackground.setSize({getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                    getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Panel::getInnerSize() const
    {
        return {std::max(0.f, getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight() - m_paddingCached.getLeft() - m_paddingCached.getRight()),
                std::max(0.f, getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom() - m_paddingCached.getTop() - m_paddingCached.getBottom())};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Panel::getChildWidgetsOffset() const
    {
        return {m_paddingCached.getLeft() + m_bordersCached.getLeft(),
                m_paddingCached.getTop() + m_bordersCached.getTop()};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Panel::isMouseOnWidget(Vector2f pos) const
    {
        return FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Panel::leftMousePressed(Vector2f pos)
    {
        onMousePress.emit(this, pos - getPosition());

        return Container::leftMousePressed(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::leftMouseReleased(Vector2f pos)
    {
        const bool mouseDown = m_mouseDown;

        onMouseRelease.emit(this, pos - getPosition());

        if (m_mouseDown)
            onClick.emit(this, pos - getPosition());

        Container::leftMouseReleased(pos);

        if (mouseDown)
        {
            // Check if you double-clicked
            if (m_possibleDoubleClick)
            {
                m_possibleDoubleClick = false;
                onDoubleClick.emit(this, pos - getPosition());
            }
            else // This is the first click
            {
                m_animationTimeElapsed = {};
                m_possibleDoubleClick = true;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::rightMousePressed(Vector2f pos)
    {
        m_rightMouseDown = true;
        onRightMousePress.emit(this, pos - getPosition());

        Container::rightMousePressed(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::rightMouseReleased(Vector2f pos)
    {
        onRightMouseRelease.emit(this, pos - getPosition());

        if (m_rightMouseDown)
            onRightClick.emit(this, pos - getPosition());

        m_rightMouseDown = false;

        Container::rightMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::rightMouseButtonNoLongerDown()
    {
        m_rightMouseDown = false;
        Container::rightMouseButtonNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Panel::getSignal(String signalName)
    {
        if (signalName == onMousePress.getName())
            return onMousePress;
        else if (signalName == onMouseRelease.getName())
            return onMouseRelease;
        else if (signalName == onClick.getName())
            return onClick;
        else if (signalName == onDoubleClick.getName())
            return onDoubleClick;
        else if (signalName == onRightMousePress.getName())
            return onRightMousePress;
        else if (signalName == onRightMouseRelease.getName())
            return onRightMouseRelease;
        else if (signalName == onRightClick.getName())
            return onRightClick;
        else
            return Group::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::rendererChanged(const String& property)
    {
        if (property == U"Borders")
        {
            m_bordersCached = getSharedRenderer()->getBorders();
            setSize(m_size);
        }

        else if (property == U"RoundedBorderRadius")
        {
            m_roundedBorderRadius = getSharedRenderer()->getRoundedBorderRadius();
        }
        else if (property == U"BorderColor")
        {
            m_borderColorCached = getSharedRenderer()->getBorderColor();
        }
        else if (property == U"BackgroundColor")
        {
            m_backgroundColorCached = getSharedRenderer()->getBackgroundColor();
        }
        else if (property == U"TextureBackground")
        {
            m_spriteBackground.setTexture(getSharedRenderer()->getTextureBackground());
        }
        else if ((property == U"Opacity") || (property == U"OpacityDisabled"))
        {
            Group::rendererChanged(property);
            m_spriteBackground.setOpacity(m_opacityCached);
        }
        else
            Group::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Panel::updateTime(Duration elapsedTime)
    {
        const bool screenRefreshRequired = Group::updateTime(elapsedTime);

        if (m_animationTimeElapsed >= getDoubleClickTime())
        {
            m_animationTimeElapsed = {};
            m_possibleDoubleClick = false;
        }

        return screenRefreshRequired;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::draw(BackendRenderTarget& target, RenderStates states) const
    {
        const Vector2f innerSize = {getSize().x - m_bordersCached.getLeft() - m_bordersCached.getRight(),
                                    getSize().y - m_bordersCached.getTop() - m_bordersCached.getBottom()};

        if ((m_roundedBorderRadius > 0) && !m_spriteBackground.isSet())
        {
            target.drawRoundedRectangle(states, getSize(), Color::applyOpacity(m_backgroundColorCached, m_opacityCached),
                                        m_roundedBorderRadius, m_bordersCached, Color::applyOpacity(m_borderColorCached, m_opacityCached));
            states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
        }
        else
        {
            // Draw the borders
            if (m_bordersCached != Borders{0})
            {
                target.drawBorders(states, m_bordersCached, getSize(), Color::applyOpacity(m_borderColorCached, m_opacityCached));
                states.transform.translate({m_bordersCached.getLeft(), m_bordersCached.getTop()});
            }

            // Draw the background
            if (m_spriteBackground.isSet())
                target.drawSprite(states, m_spriteBackground);
            else
                target.drawFilledRect(states, innerSize, Color::applyOpacity(m_backgroundColorCached, m_opacityCached));
        }

        states.transform.translate({m_paddingCached.getLeft(), m_paddingCached.getTop()});
        const Vector2f contentSize = {innerSize.x - m_paddingCached.getLeft() - m_paddingCached.getRight(),
                                      innerSize.y - m_paddingCached.getTop() - m_paddingCached.getBottom()};

        // Draw the child widgets
        target.addClippingLayer(states, {{}, contentSize});
        Container::draw(target, states); // NOLINT(bugprone-parent-virtual-call)
        target.removeClippingLayer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Panel::clone() const
    {
        return std::make_shared<Panel>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
