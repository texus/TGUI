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


#ifndef TGUI_LAYOUT_HPP
#define TGUI_LAYOUT_HPP

#include <memory>

#include <TGUI/SharedWidgetPtr.hpp>

/// TODO: Add documentation.
/// WARNING: File still under heavy development

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class Widget;
    class Layout;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    enum class LayoutChangeTrigger
    {
        Position,
        Size
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API LayoutCallbackManager final
    {
    public:
        void bindCallback(const SharedWidgetPtr<Widget>& widget, LayoutChangeTrigger trigger, const Layout* layout, const std::function<void()>& function);
        void unbindCallback(const SharedWidgetPtr<Widget>& widget, LayoutChangeTrigger trigger, const Layout* layout);

    private:
        void positionChanged(SharedWidgetPtr<Widget> widget);
        void sizeChanged(SharedWidgetPtr<Widget> widget);

    private:
        /// Use std::shared_ptr<Widget> when the changes have been made?
        std::map<Widget*, std::map<LayoutChangeTrigger, std::map<const Layout*, std::function<void()>>>> m_callbacks;
    };

    extern TGUI_API LayoutCallbackManager TGUI_LayoutCallbackManager;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API LayoutBind final
    {
    public:

        enum class Param
        {
            X,
            Y
        };

        LayoutBind(const SharedWidgetPtr<Widget>& widget, Param param, float fraction = 1);

        float getValue() const;

        void setTrigger(LayoutChangeTrigger trigger);

        void setCallbackFunction(const std::function<void()>& callback, const Layout* layout) const;

        void unbindCallback(const Layout* layout);

    protected:
        SharedWidgetPtr<Widget> m_widget;
        float m_fraction;

        LayoutChangeTrigger m_trigger;
        Param m_param;

        static LayoutCallbackManager m_layoutCallbackManager;
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API Layout1d final
    {
    public:

        enum class Operator
        {
            Add,
            Subtract,
            Multiply,
            Divide
        };

        Layout1d(float value = 0) : m_value{value}, m_constant{value} {}

        explicit Layout1d(const std::shared_ptr<LayoutBind>& layout, LayoutChangeTrigger trigger);

        float getValue() const
        {
            return m_value;
        }

        friend Layout1d operator+(const Layout1d& left, const Layout1d& right);
        friend Layout1d operator-(const Layout1d& left, const Layout1d& right);
        friend Layout1d operator*(const Layout1d& left, const Layout1d& right);
        friend Layout1d operator/(const Layout1d& left, const Layout1d& right);

        friend Layout operator*(const Layout& left, const Layout1d& right);
        friend Layout operator/(const Layout& left, const Layout1d& right);

    private:
        void recalculateValue();
        void setCallbackFunction(const std::function<void()>& callback, const Layout* layout) const;
        void unbindCallback(const Layout* layout);

    private:
        std::list<std::shared_ptr<LayoutBind>> m_bindings;
        std::list<Operator> m_operators;
        float m_value = 0;
        float m_constant = 0;

        friend class Layout;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API Layout final
    {
    public:

        Layout(const sf::Vector2f& value = {0,0}) : x(value.x), y(value.y) {}

        Layout(const Layout1d& valueX, const Layout1d& valueY) : x(valueX), y(valueY) {}

        ~Layout();

        sf::Vector2f getValue() const
        {
            return {x.getValue(), y.getValue()};
        }

        void recalculateValue();
        void setCallbackFunction(const std::function<void()>& callback) const;

        friend Layout operator+(const Layout& left, const Layout& right);
        friend Layout operator-(const Layout& left, const Layout& right);

        friend Layout operator*(const Layout& left, const Layout1d& right);
        friend Layout operator/(const Layout& left, const Layout1d& right);

    public:
        Layout1d x;
        Layout1d y;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Layout1d operator+(const Layout1d& left, const Layout1d& right);
    TGUI_API Layout1d operator-(const Layout1d& left, const Layout1d& right);
    TGUI_API Layout1d operator*(const Layout1d& left, const Layout1d& right);
    TGUI_API Layout1d operator/(const Layout1d& left, const Layout1d& right);

    TGUI_API Layout operator+(const Layout& left, const Layout& right);
    TGUI_API Layout operator-(const Layout& left, const Layout& right);

    TGUI_API Layout operator*(const Layout& left, const Layout1d& right);
    TGUI_API Layout operator/(const Layout& left, const Layout1d& right);

    TGUI_API Layout1d bindLeft(const SharedWidgetPtr<Widget>& widget, float fraction = 1);
    TGUI_API Layout1d bindTop(const SharedWidgetPtr<Widget>& widget, float fraction = 1);
    TGUI_API Layout1d bindRight(const SharedWidgetPtr<Widget>& widget, float fraction = 1);
    TGUI_API Layout1d bindBottom(const SharedWidgetPtr<Widget>& widget, float fraction = 1);
    TGUI_API Layout1d bindWidth(const SharedWidgetPtr<Widget>& widget, float fraction = 1);
    TGUI_API Layout1d bindHeight(const SharedWidgetPtr<Widget>& widget, float fraction = 1);

    TGUI_API Layout bindPosition(const SharedWidgetPtr<Widget>& widget, const sf::Vector2f& fraction = {1,1});
    TGUI_API Layout bindSize(const SharedWidgetPtr<Widget>& widget, const sf::Vector2f& fraction = {1,1});

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_LAYOUT_HPP
