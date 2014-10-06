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
#include <TGUI/Gui.hpp>

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
                widget->connect("PositionChanged", &LayoutCallbackManager::positionChanged, this, widget);
            else
                widget->connect("SizeChanged", &LayoutCallbackManager::sizeChanged, this, widget);
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

    LayoutBind::LayoutBind(const Widget::Ptr& widget, Param param, float fraction, LayoutChangeTrigger trigger) :
        m_widget  (widget),
        m_fraction(fraction),
        m_trigger (trigger),
        m_param   (param)
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

    void LayoutBind::setCallbackFunction(const std::function<void()>& callback, const Layout* layout) const
    {
        if (m_widget != nullptr)
            m_layoutCallbackManager.bindCallback(m_widget, m_trigger, layout, callback);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LayoutBind::unbindCallback(const Layout* layout)
    {
        if (m_widget != nullptr)
            m_layoutCallbackManager.unbindCallback(m_widget, m_trigger, layout);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d::Layout1d(const LayoutBind& binding)
    {
        m_bindings.push_back(binding);

        m_value = m_bindings.back().getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d::Layout1d(const Layout1d& layout) :
        m_bindings (layout.m_bindings), // Did not compile in VS2013 when using braces
        m_operators(layout.m_operators), // Did not compile in VS2013 when using braces
        m_value    {layout.m_value},
        m_constant {layout.m_constant}
    {
        for (auto& group : layout.m_groups)
            m_groups.push_back(group.clone(*this));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d& Layout1d::operator=(const Layout1d& right)
    {
        if (this != &right)
        {
            m_bindings  = right.m_bindings;
            m_operators = right.m_operators;
            m_value     = right.m_value;
            m_constant  = right.m_constant;

            for (auto& group : right.m_groups)
                m_groups.push_back(group.clone(*this));
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout1d::setGroup(LayoutGroup&& group)
    {
        m_groups.push_back(std::move(group));

        recalculateValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout1d::recalculateValue()
    {
        m_value = m_constant;

        // Constants don't need to be updated
        if (!m_bindings.empty())
        {
            assert(m_bindings.size() == m_operators.size() + 1);

            auto bindingsIt = m_bindings.cbegin();
            m_value += bindingsIt->getValue();
            bindingsIt++;

            // Apply the math operations between the bindings
            for (auto operatorsIt = m_operators.cbegin(); operatorsIt != m_operators.cend(); ++operatorsIt, ++bindingsIt)
            {
                switch (*operatorsIt)
                {
                case Operator::Add:
                    m_value += bindingsIt->getValue();
                    break;

                case Operator::Subtract:
                    m_value -= bindingsIt->getValue();
                    break;

                case Operator::Multiply:
                    m_value *= bindingsIt->getValue();
                    break;

                case Operator::Divide:
                    m_value /= bindingsIt->getValue();
                    break;
                }
            }
        }

        // It is possible that another layout in the group should be used
        for (auto& group : m_groups)
        {
            group.determineActiveLayout();

            if (&group.getActiveLayout() != this)
                m_value = group.getActiveLayout().getValue();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout1d::setCallbackFunction(const std::function<void()>& callback, const Layout* layout) const
    {
        if (m_groups.empty())
        {
            for (auto& binding : m_bindings)
                binding.setCallbackFunction(callback, layout);
        }
        else
        {
            for (auto& group : m_groups)
            {
                for (auto& binding : group.getActiveLayout().m_bindings)
                    binding.setCallbackFunction(callback, layout);

                for (auto& binding : group.getNonActiveLayout().m_bindings)
                    binding.setCallbackFunction(callback, layout);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Layout1d::unbindCallback(const Layout* layout)
    {
        if (m_groups.empty())
        {
            for (auto& binding : m_bindings)
                binding.unbindCallback(layout);
        }
        else
        {
            for (auto& group : m_groups)
            {
                for (auto& binding : group.getActiveLayout().m_bindings)
                    binding.unbindCallback(layout);

                for (auto& binding : group.getNonActiveLayout().m_bindings)
                    binding.unbindCallback(layout);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LayoutGroup::LayoutGroup(Layout1d& first, const Layout1d& second, Selector selector) :
        m_first   (first), // Did not compile on gcc 4.8 when using braces
        m_second  {second},
        m_selector{selector}
    {
        determineActiveLayout();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LayoutGroup::LayoutGroup(LayoutGroup&& group) :
        m_first   (group.m_first), // Did not compile on gcc 4.8 when using braces
        m_second  {group.m_second},
        m_selector{group.m_selector}
    {
        determineActiveLayout();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LayoutGroup LayoutGroup::clone(Layout1d& layout) const
    {
        return {layout, m_second, m_selector};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LayoutGroup::determineActiveLayout()
    {
        m_second.recalculateValue();

        switch (m_selector)
        {
            case Selector::Minimum:
            {
                if (m_first.getValue() < m_second.getValue())
                    m_active = &m_first;
                else
                    m_active = &m_second;

                break;
            }
            case Selector::Maximum:
            {
                if (m_first.getValue() > m_second.getValue())
                    m_active = &m_first;
                else
                    m_active = &m_second;

                break;
            }
        }

        if (m_active == &m_first)
            m_nonActive = &m_second;
        else if (m_active == &m_second)
            m_nonActive = &m_first;
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
        layout.m_constant += right.m_constant;

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
        layout.m_constant -= right.m_constant;

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

        if (!left.m_bindings.empty() || !right.m_bindings.empty())
        {
            if (left.m_bindings.empty())
                layout.m_bindings.push_back({nullptr, LayoutBind::Param::None, left.m_value, LayoutChangeTrigger::Position});

            if (right.m_bindings.empty())
                layout.m_bindings.push_back({nullptr, LayoutBind::Param::None, right.m_value, LayoutChangeTrigger::Position});

            layout.m_bindings.insert(layout.m_bindings.end(), right.m_bindings.begin(), right.m_bindings.end());
            layout.m_operators.push_back(Layout1d::Operator::Multiply);
        }
        else
            layout.m_constant *= right.m_constant;

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

        if (!left.m_bindings.empty() || !right.m_bindings.empty())
        {
            if (left.m_bindings.empty())
                layout.m_bindings.push_back({nullptr, LayoutBind::Param::None, left.m_value, LayoutChangeTrigger::Position});

            if (right.m_bindings.empty())
                layout.m_bindings.push_back({nullptr, LayoutBind::Param::None, right.m_value, LayoutChangeTrigger::Position});

            layout.m_bindings.insert(layout.m_bindings.end(), right.m_bindings.begin(), right.m_bindings.end());
            layout.m_operators.push_back(Layout1d::Operator::Divide);
        }
        else
            layout.m_constant /= right.m_constant;

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
        return Layout1d{{widget, LayoutBind::Param::X, fraction, LayoutChangeTrigger::Position}};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindTop(const Widget::Ptr& widget, float fraction)
    {
        return Layout1d{{widget, LayoutBind::Param::Y, fraction, LayoutChangeTrigger::Position}};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindRight(const Widget::Ptr& widget, float fraction)
    {
        return Layout1d{{widget, LayoutBind::Param::X, fraction, LayoutChangeTrigger::Position}}
             + Layout1d{{widget, LayoutBind::Param::X, fraction, LayoutChangeTrigger::Size}};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindBottom(const Widget::Ptr& widget, float fraction)
    {
        return Layout1d{{widget, LayoutBind::Param::Y, fraction, LayoutChangeTrigger::Position}}
             + Layout1d{{widget, LayoutBind::Param::Y, fraction, LayoutChangeTrigger::Size}};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindWidth(const Widget::Ptr& widget, float fraction)
    {
        return Layout1d{{widget, LayoutBind::Param::X, fraction, LayoutChangeTrigger::Size}};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindHeight(const Widget::Ptr& widget, float fraction)
    {
        return Layout1d{{widget, LayoutBind::Param::Y, fraction, LayoutChangeTrigger::Size}};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindLeft(const Gui& gui, float fraction)
    {
        return Layout1d{{gui.getContainer(), LayoutBind::Param::X, fraction, LayoutChangeTrigger::Position}};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindTop(const Gui& gui, float fraction)
    {
        return Layout1d{{gui.getContainer(), LayoutBind::Param::Y, fraction, LayoutChangeTrigger::Position}};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindRight(const Gui& gui, float fraction)
    {
        return Layout1d{{gui.getContainer(), LayoutBind::Param::X, fraction, LayoutChangeTrigger::Position}}
             + Layout1d{{gui.getContainer(), LayoutBind::Param::X, fraction, LayoutChangeTrigger::Size}};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindBottom(const Gui& gui, float fraction)
    {
        return Layout1d{{gui.getContainer(), LayoutBind::Param::Y, fraction, LayoutChangeTrigger::Position}}
             + Layout1d{{gui.getContainer(), LayoutBind::Param::Y, fraction, LayoutChangeTrigger::Size}};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindWidth(const Gui& gui, float fraction)
    {
        return Layout1d{{gui.getContainer(), LayoutBind::Param::X, fraction, LayoutChangeTrigger::Size}};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindHeight(const Gui& gui, float fraction)
    {
        return Layout1d{{gui.getContainer(), LayoutBind::Param::Y, fraction, LayoutChangeTrigger::Size}};
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

    Layout bindPosition(const Gui& gui, const sf::Vector2f& fraction)
    {
        return {bindLeft(gui.getContainer(), fraction.x), bindTop(gui.getContainer(), fraction.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout bindSize(const Gui& gui, const sf::Vector2f& fraction)
    {
        return {bindWidth(gui.getContainer(), fraction.x), bindHeight(gui.getContainer(), fraction.y)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindMinimum(const Layout1d& first, const Layout1d& second)
    {
        Layout1d layout = first;
        layout.setGroup({layout, second, LayoutGroup::Selector::Minimum});
        return layout;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindMaximum(const Layout1d& first, const Layout1d& second)
    {
        Layout1d layout = first;
        layout.setGroup({layout, second, LayoutGroup::Selector::Maximum});
        return layout;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Layout1d bindLimits(const Layout1d& minimum, const Layout1d& maximum, const Layout1d& value)
    {
        return bindMinimum(maximum, bindMaximum(minimum, value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
