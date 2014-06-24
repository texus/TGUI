/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2014 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Widget.hpp>

#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    LayoutCallbackManager LayoutBind::m_layoutCallbackManager;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LayoutCallbackManager::bindCallback(const Widget::Ptr& widget, LayoutChangeTrigger trigger, const Layout* layout, const std::function<void()>& function)
    {
        // The first time we get this trigger for this widget, we have to bind the callback
        if (m_callbacks[widget.get()][trigger].empty())
        {
            if (trigger == LayoutChangeTrigger::Position)
                widget->bindCallback(Widget::PositionChanged, std::bind(&LayoutCallbackManager::positionChanged, this, widget));
            else
                widget->bindCallback(Widget::SizeChanged, std::bind(&LayoutCallbackManager::sizeChanged, this, widget));
        }

        m_callbacks[widget.get()][trigger][layout] = function;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LayoutCallbackManager::unbindCallback(const Widget::Ptr& widget, LayoutChangeTrigger trigger, const Layout* layout)
    {
        m_callbacks[widget.get()][trigger].erase(layout);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LayoutCallbackManager::positionChanged(Widget::Ptr widget)
    {
        for (auto& callback : m_callbacks[widget.get()][LayoutChangeTrigger::Position])
            callback.second();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LayoutCallbackManager::sizeChanged(Widget::Ptr widget)
    {
        for (auto& callback : m_callbacks[widget.get()][LayoutChangeTrigger::Size])
            callback.second();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LayoutBind::LayoutBind(const Widget::Ptr& widget, Param param, float fraction) :
        m_widget(widget),
        m_fraction(fraction),
        m_param(param)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float LayoutBind::getValue() const
    {
        if (m_widget != nullptr)
        {
            if (m_trigger == LayoutChangeTrigger::Position)
            {
                if (m_param == Param::X)
                    return m_widget->getPosition().x * m_fraction;
                else
                    return m_widget->getPosition().y * m_fraction;
            }
            else
            {
                if (m_param == Param::X)
                    return m_widget->getSize().x * m_fraction;
                else
                    return m_widget->getSize().y * m_fraction;
            }
        }
        else
            return m_fraction;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LayoutBind::setTrigger(LayoutChangeTrigger trigger)
    {
        m_trigger = trigger;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LayoutBind::setCallbackFunction(const std::function<void()>& callback, const Layout* layout) const
    {
        m_layoutCallbackManager.bindCallback(m_widget, m_trigger, layout, callback);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LayoutBind::unbindCallback(const Layout* layout)
    {
        m_layoutCallbackManager.unbindCallback(m_widget, m_trigger, layout);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d::Layout1d(const std::shared_ptr<LayoutBind>& layout, LayoutChangeTrigger trigger)
    {
        layout->setTrigger(trigger);
        m_value = layout->getValue();

        m_bindings.push_back(layout);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout1d::recalculateValue()
    {
        // Constants don't need to be updated
        if (m_bindings.empty())
            return;

        assert(m_bindings.size() == m_operators.size() + 1);

        auto bindingsIt = m_bindings.cbegin();
        m_value = (*bindingsIt)->getValue();
        bindingsIt++;

        // Apply the math operations between the bindings
        for (auto operatorsIt = m_operators.cbegin(); operatorsIt != m_operators.cend(); ++operatorsIt, ++bindingsIt)
        {
            switch (*operatorsIt)
            {
            case Operator::Add:
                m_value += (*bindingsIt)->getValue();
                break;

            case Operator::Subtract:
                m_value -= (*bindingsIt)->getValue();
                break;

            case Operator::Multiply:
                m_value *= (*bindingsIt)->getValue();
                break;

            case Operator::Divide:
                m_value /= (*bindingsIt)->getValue();
                break;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout1d::setCallbackFunction(const std::function<void()>& callback, const Layout* layout) const
    {
        for (auto& binding : m_bindings)
            binding->setCallbackFunction(callback, layout);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout1d::unbindCallback(const Layout* layout)
    {
        for (auto& binding : m_bindings)
            binding->unbindCallback(layout);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout::~Layout()
    {
        x.unbindCallback(this);
        y.unbindCallback(this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::recalculateValue()
    {
        x.recalculateValue();
        y.recalculateValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout::setCallbackFunction(const std::function<void()>& callback) const
    {
        x.setCallbackFunction(callback, this);
        y.setCallbackFunction(callback, this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d operator+(float left, const Layout1d& right)
    {
        return Layout1d{left} + right;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d operator+(const Layout1d& left, const Layout1d& right)
    {
        Layout1d layout = left;
        layout.m_value += right.m_value;

        layout.m_bindings.insert(layout.m_bindings.end(), right.m_bindings.begin(), right.m_bindings.end());

        if (!left.m_bindings.empty() && !right.m_bindings.empty())
            layout.m_operators.push_back(Layout1d::Operator::Add);

        return layout;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d operator-(float left, const Layout1d& right)
    {
        return Layout1d{left} - right;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d operator-(const Layout1d& left, const Layout1d& right)
    {
        Layout1d layout = left;
        layout.m_value -= right.m_value;

        layout.m_bindings.insert(layout.m_bindings.end(), right.m_bindings.begin(), right.m_bindings.end());

        if (!left.m_bindings.empty() && !right.m_bindings.empty())
            layout.m_operators.push_back(Layout1d::Operator::Subtract);

        return layout;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d operator*(float left, const Layout1d& right)
    {
        return Layout1d{left} * right;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d operator*(const Layout1d& left, const Layout1d& right)
    {
        Layout1d layout = left;
        layout.m_value *= right.m_value;

        layout.m_bindings.insert(layout.m_bindings.end(), right.m_bindings.begin(), right.m_bindings.end());

        if (!left.m_bindings.empty() && !right.m_bindings.empty())
            layout.m_operators.push_back(Layout1d::Operator::Multiply);

        return layout;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d operator/(float left, const Layout1d& right)
    {
        return Layout1d{left} / right;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d operator/(const Layout1d& left, const Layout1d& right)
    {
        Layout1d layout = left;
        layout.m_value /= right.m_value;

        layout.m_bindings.insert(layout.m_bindings.end(), right.m_bindings.begin(), right.m_bindings.end());

        if (!left.m_bindings.empty() && !right.m_bindings.empty())
            layout.m_operators.push_back(Layout1d::Operator::Divide);

        return layout;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Layout operator+(const Layout& left, const Layout& right)
    {
        return {left.x + right.x, left.y + right.y};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Layout operator-(const Layout& left, const Layout& right)
    {
        return {left.x - right.x, left.y - right.y};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Layout operator*(const Layout& left, const Layout1d& right)
    {
        return {left.x * right, left.y * right};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Layout operator/(const Layout& left, const Layout1d& right)
    {
        return {left.x / right, left.y / right};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindLeft(const Widget::Ptr& widget, float fraction)
    {
        return Layout1d{std::shared_ptr<LayoutBind>{new LayoutBind{widget, LayoutBind::Param::X, fraction}}, LayoutChangeTrigger::Position};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindTop(const Widget::Ptr& widget, float fraction)
    {
        return Layout1d{std::shared_ptr<LayoutBind>{new LayoutBind{widget, LayoutBind::Param::Y, fraction}}, LayoutChangeTrigger::Position};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindRight(const Widget::Ptr& widget, float fraction)
    {
        return Layout1d{std::shared_ptr<LayoutBind>{new LayoutBind{widget, LayoutBind::Param::X, fraction}}, LayoutChangeTrigger::Position}
             + Layout1d{std::shared_ptr<LayoutBind>{new LayoutBind{widget, LayoutBind::Param::X, fraction}}, LayoutChangeTrigger::Size};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindBottom(const Widget::Ptr& widget, float fraction)
    {
        return Layout1d{std::shared_ptr<LayoutBind>{new LayoutBind{widget, LayoutBind::Param::Y, fraction}}, LayoutChangeTrigger::Position}
             + Layout1d{std::shared_ptr<LayoutBind>{new LayoutBind{widget, LayoutBind::Param::Y, fraction}}, LayoutChangeTrigger::Size};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindWidth(const Widget::Ptr& widget, float fraction)
    {
        return Layout1d{std::shared_ptr<LayoutBind>{new LayoutBind{widget, LayoutBind::Param::X, fraction}}, LayoutChangeTrigger::Size};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindHeight(const Widget::Ptr& widget, float fraction)
    {
        return Layout1d{std::shared_ptr<LayoutBind>{new LayoutBind{widget, LayoutBind::Param::Y, fraction}}, LayoutChangeTrigger::Size};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindPosition(const Widget::Ptr& widget, const sf::Vector2f& fraction)
    {
        return {bindLeft(widget, fraction.x), bindTop(widget, fraction.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindSize(const Widget::Ptr& widget, const sf::Vector2f& fraction)
    {
        return {bindWidth(widget, fraction.x), bindHeight(widget, fraction.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
