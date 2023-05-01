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


#include <TGUI/Widgets/ClickableWidget.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char ClickableWidget::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ClickableWidget::ClickableWidget(const char* typeName, bool initRenderer) :
        Widget{typeName, initRenderer}
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ClickableWidget::Ptr ClickableWidget::create(const Layout2d& size)
    {
        auto widget = std::make_shared<ClickableWidget>();
        widget->setSize(size);
        return widget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ClickableWidget::Ptr ClickableWidget::copy(const ClickableWidget::ConstPtr& widget)
    {
        if (widget)
            return std::static_pointer_cast<ClickableWidget>(widget->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ClickableWidget::isMouseOnWidget(Vector2f pos) const
    {
        return FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y}.contains(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ClickableWidget::leftMousePressed(Vector2f pos)
    {
        Widget::leftMousePressed(pos);
        onMousePress.emit(this, pos - getPosition());
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ClickableWidget::leftMouseReleased(Vector2f pos)
    {
        onMouseRelease.emit(this, pos - getPosition());

        if (m_mouseDown)
            onClick.emit(this, pos - getPosition());

        Widget::leftMouseReleased(pos);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ClickableWidget::rightMousePressed(Vector2f pos)
    {
        m_rightMouseDown = true;
        onRightMousePress.emit(this, pos - getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ClickableWidget::rightMouseReleased(Vector2f pos)
    {
        onRightMouseRelease.emit(this, pos - getPosition());

        if (m_rightMouseDown)
            onRightClick.emit(this, pos - getPosition());

        m_rightMouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ClickableWidget::rightMouseButtonNoLongerDown()
    {
        m_rightMouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ClickableWidget::draw(BackendRenderTarget&, RenderStates) const
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& ClickableWidget::getSignal(String signalName)
    {
        if (signalName == onMousePress.getName())
            return onMousePress;
        else if (signalName == onMouseRelease.getName())
            return onMouseRelease;
        else if (signalName == onClick.getName())
            return onClick;
        else if (signalName == onRightMousePress.getName())
            return onRightMousePress;
        else if (signalName == onRightMouseRelease.getName())
            return onRightMouseRelease;
        else if (signalName == onRightClick.getName())
            return onRightClick;
        else
            return Widget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr ClickableWidget::clone() const
    {
        return std::make_shared<ClickableWidget>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
