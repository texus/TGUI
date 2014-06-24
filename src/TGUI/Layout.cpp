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
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LayoutBind::LayoutBind(const SharedWidgetPtr<Widget>& widget, Param param, float fraction) :
        m_widget(widget),
        m_fraction(fraction),
        m_param(param)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LayoutBind::bind(Trigger trigger)
    {
        assert(m_widget != nullptr);
        assert(trigger == Trigger::PositionChanged || trigger == Trigger::SizeChanged);

        if (trigger == Trigger::PositionChanged)
            m_getter = std::bind(&Widget::getPosition, m_widget.get());
        else
            m_getter = std::bind(&Widget::getSize, m_widget.get());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float LayoutBind::getValue() const
    {
        assert(m_param == Param::X || m_param == Param::Y);

        if (m_widget != nullptr)
        {
            if (m_param == Param::X)
                return m_getter().x * m_fraction;
            else
                return m_getter().y * m_fraction;
        }
        else
            return m_fraction;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d::Layout1d(const std::shared_ptr<LayoutBind>& layout, LayoutBind::Trigger trigger)
    {
        layout->bind(trigger);

        m_bindings.push_back(layout);
        recalculateResult();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout1d::recalculateResult()
    {
        assert(m_bindings.size() == m_operators.size() + 1);

        auto bindingsIt = m_bindings.cbegin();

        m_value = (*bindingsIt)->getValue();
        bindingsIt++;

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

    Layout1d bindLeft(const SharedWidgetPtr<Widget>& widget, float fraction)
    {
        return Layout1d{std::shared_ptr<LayoutBind>{new LayoutBind{widget, LayoutBind::Param::X, fraction}}, LayoutBind::Trigger::PositionChanged};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindTop(const SharedWidgetPtr<Widget>& widget, float fraction)
    {
        return Layout1d{std::shared_ptr<LayoutBind>{new LayoutBind{widget, LayoutBind::Param::Y, fraction}}, LayoutBind::Trigger::PositionChanged};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindRight(const SharedWidgetPtr<Widget>& widget, float fraction)
    {
        return Layout1d{std::shared_ptr<LayoutBind>{new LayoutBind{widget, LayoutBind::Param::X, fraction}}, LayoutBind::Trigger::PositionChanged}
             + Layout1d{std::shared_ptr<LayoutBind>{new LayoutBind{widget, LayoutBind::Param::X, fraction}}, LayoutBind::Trigger::SizeChanged};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindBottom(const SharedWidgetPtr<Widget>& widget, float fraction)
    {
        return Layout1d{std::shared_ptr<LayoutBind>{new LayoutBind{widget, LayoutBind::Param::Y, fraction}}, LayoutBind::Trigger::PositionChanged}
             + Layout1d{std::shared_ptr<LayoutBind>{new LayoutBind{widget, LayoutBind::Param::Y, fraction}}, LayoutBind::Trigger::SizeChanged};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindWidth(const SharedWidgetPtr<Widget>& widget, float fraction)
    {
        return Layout1d{std::shared_ptr<LayoutBind>{new LayoutBind{widget, LayoutBind::Param::X, fraction}}, LayoutBind::Trigger::SizeChanged};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindHeight(const SharedWidgetPtr<Widget>& widget, float fraction)
    {
        return Layout1d{std::shared_ptr<LayoutBind>{new LayoutBind{widget, LayoutBind::Param::Y, fraction}}, LayoutBind::Trigger::SizeChanged};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindPosition(const SharedWidgetPtr<Widget>& widget, const sf::Vector2f& fraction)
    {
        return {bindLeft(widget, fraction.x), bindTop(widget, fraction.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindSize(const SharedWidgetPtr<Widget>& widget, const sf::Vector2f& fraction)
    {
        return {bindWidth(widget, fraction.x), bindHeight(widget, fraction.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
