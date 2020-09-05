/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Signal.hpp>
#include <TGUI/Widget.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>

#include <set>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace Signals
    {
        constexpr const char* const Widget::PositionChanged;
        constexpr const char* const Widget::SizeChanged;
        constexpr const char* const Widget::Focused;
        constexpr const char* const Widget::Unfocused;
        constexpr const char* const Widget::MouseEntered;
        constexpr const char* const Widget::MouseLeft;
        constexpr const char* const Widget::AnimationFinished;
        constexpr const char* const ClickableWidget::MousePressed;
        constexpr const char* const ClickableWidget::MouseReleased;
        constexpr const char* const ClickableWidget::Clicked;
        constexpr const char* const ClickableWidget::RightMousePressed;
        constexpr const char* const ClickableWidget::RightMouseReleased;
        constexpr const char* const ClickableWidget::RightClicked;
        constexpr const char* const Button::Pressed;
        constexpr const char* const ChildWindow::MousePressed;
        constexpr const char* const ChildWindow::Closed;
        constexpr const char* const ChildWindow::Minimized;
        constexpr const char* const ChildWindow::Maximized;
        constexpr const char* const ChildWindow::EscapeKeyPressed;
        constexpr const char* const ColorPicker::ColorChange;
        constexpr const char* const ColorPicker::OkPressed;
        constexpr const char* const ComboBox::ItemSelected;
        constexpr const char* const EditBox::TextChanged;
        constexpr const char* const EditBox::ReturnKeyPressed;
        constexpr const char* const Knob::ValueChanged;
        constexpr const char* const Label::DoubleClicked;
        constexpr const char* const ListBox::ItemSelected;
        constexpr const char* const ListBox::MousePressed;
        constexpr const char* const ListBox::MouseReleased;
        constexpr const char* const ListBox::DoubleClicked;
        constexpr const char* const ListView::ItemSelected;
        constexpr const char* const ListView::DoubleClicked;
        constexpr const char* const ListView::RightClicked;
        constexpr const char* const ListView::HeaderClicked;
        constexpr const char* const MenuBar::MenuItemClicked;
        constexpr const char* const MessageBox::ButtonPressed;
        constexpr const char* const Panel::MousePressed;
        constexpr const char* const Panel::MouseReleased;
        constexpr const char* const Panel::Clicked;
        constexpr const char* const Panel::RightMousePressed;
        constexpr const char* const Panel::RightMouseReleased;
        constexpr const char* const Panel::RightClicked;
        constexpr const char* const Picture::DoubleClicked;
        constexpr const char* const ProgressBar::ValueChanged;
        constexpr const char* const ProgressBar::Full;
        constexpr const char* const RadioButton::Checked;
        constexpr const char* const RadioButton::Unchecked;
        constexpr const char* const RadioButton::Changed;
        constexpr const char* const RangeSlider::RangeChanged;
        constexpr const char* const Scrollbar::ValueChanged;
        constexpr const char* const Slider::ValueChanged;
        constexpr const char* const SpinButton::ValueChanged;
        constexpr const char* const Tabs::TabSelected;
        constexpr const char* const TextArea::TextChanged;
        constexpr const char* const TextArea::SelectionChanged;
        constexpr const char* const TreeView::ItemSelected;
        constexpr const char* const TreeView::DoubleClicked;
        constexpr const char* const TreeView::Expanded;
        constexpr const char* const TreeView::Collapsed;
        constexpr const char* const TreeView::RightClicked;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Signal::m_lastSignalId = 0;
    std::deque<const void*> Signal::m_parameters;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal::Signal(const Signal& other) :
        m_enabled {other.m_enabled},
        m_name    {other.m_name},
        m_handlers{} // signal handlers are not copied with the widget
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Signal::operator=(const Signal& other)
    {
        if (this != &other)
        {
            m_enabled = other.m_enabled;
            m_name = other.m_name;
            m_handlers.clear(); // signal handlers are not copied with the widget
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Signal::disconnect(unsigned int id)
    {
        return (m_handlers.erase(id) != 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Signal::disconnectAll()
    {
        m_handlers.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Signal::emit(const Widget* widget)
    {
        if (m_handlers.empty() || !m_enabled)
            return false;

        m_parameters[0] = static_cast<const void*>(&widget);

        // Copy the handlers before calling them in case the widget (and this signal) gets destroyed during the handler
        auto handlers = m_handlers;
        for (const auto& handler : handlers)
            handler.second();

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Signal::getWidget()
    {
        return dereferenceParam<Widget*>(0)->shared_from_this();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalChildWindow::emit(ChildWindow* childWindow)
    {
        if (m_handlers.empty())
            return false;

        m_parameters[1] = static_cast<const void*>(&childWindow);
        return Signal::emit(childWindow);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<ChildWindow> SignalChildWindow::dereferenceChildWindow()
    {
        return dereferenceParam<ChildWindow*>(1)->shared_from_this()->cast<ChildWindow>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalItem::emit(const Widget* widget, int index, const String& item, const String& id)
    {
        if (m_handlers.empty())
            return false;

        m_parameters[1] = static_cast<const void*>(&index);
        m_parameters[2] = static_cast<const void*>(&item);
        m_parameters[3] = static_cast<const void*>(&id);
        return Signal::emit(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalAnimation::emit(const Widget* widget, ShowAnimationType type, bool visible)
    {
        if (m_handlers.empty())
            return false;

        m_parameters[1] = static_cast<const void*>(&type);
        m_parameters[2] = static_cast<const void*>(&visible);
        return Signal::emit(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SignalItemHierarchy::emit(const Widget* widget, const String& item, const std::vector<String>& fullItem)
    {
        if (m_handlers.empty())
            return false;

        m_parameters[1] = static_cast<const void*>(&item);
        m_parameters[2] = static_cast<const void*>(&fullItem);
        return Signal::emit(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
