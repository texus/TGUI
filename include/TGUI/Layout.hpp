/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <functional>
#include <memory>
#include <vector>
#include <list>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class LayoutGroup;
    class Layout;
    class Widget;
    class Gui;

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
        void bindCallback(const std::shared_ptr<Widget>& widget, LayoutChangeTrigger trigger, const Layout* layout, const std::function<void()>& function);
        void unbindCallback(const std::shared_ptr<Widget>& widget, LayoutChangeTrigger trigger, const Layout* layout);

    private:
        void positionChanged(std::shared_ptr<Widget> widget);
        void sizeChanged(std::shared_ptr<Widget> widget);

    private:
        std::map<Widget*, std::map<LayoutChangeTrigger, std::map<const Layout*, std::function<void()>>>> m_callbacks;
    };

    extern TGUI_API LayoutCallbackManager TGUI_LayoutCallbackManager;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API LayoutBind final
    {
    public:

        enum class Param
        {
            None,
            X,
            Y
        };

        LayoutBind(const std::shared_ptr<Widget>& widget, Param param, float fraction, LayoutChangeTrigger trigger);

        float getValue() const;

        void setCallbackFunction(const std::function<void()>& callback, const Layout* layout) const;

        void unbindCallback(const Layout* layout);

    protected:
        std::shared_ptr<Widget> m_widget;
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

        explicit Layout1d(const LayoutBind& binding);

        Layout1d(const Layout1d& layout);
        Layout1d& operator=(const Layout1d& right);

        float getValue() const
        {
            return m_value;
        }

        void setGroup(LayoutGroup&& group);

        void recalculateValue();

        friend TGUI_API Layout1d operator+(const Layout1d& left, const Layout1d& right);
        friend TGUI_API Layout1d operator-(const Layout1d& left, const Layout1d& right);
        friend TGUI_API Layout1d operator*(const Layout1d& left, const Layout1d& right);
        friend TGUI_API Layout1d operator/(const Layout1d& left, const Layout1d& right);

        friend TGUI_API Layout operator*(const Layout& left, const Layout1d& right);
        friend TGUI_API Layout operator/(const Layout& left, const Layout1d& right);

    private:
        void setCallbackFunction(const std::function<void()>& callback, const Layout* layout) const;
        void unbindCallback(const Layout* layout);

    private:
        std::list<LayoutBind> m_bindings;
        std::list<Operator> m_operators;
        float m_value = 0;
        float m_constant = 0;

        std::vector<LayoutGroup> m_groups;

        friend class Layout;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API LayoutGroup final
    {
    public:

        enum class Selector
        {
            Minimum,
            Maximum
        };

        LayoutGroup(Layout1d& first, const Layout1d& second, Selector selector);

        LayoutGroup(const LayoutGroup&) = delete;
        LayoutGroup& operator=(const LayoutGroup&) = delete;

        LayoutGroup(LayoutGroup&& group);
        LayoutGroup& operator=(LayoutGroup&&);

        LayoutGroup clone(Layout1d& layout) const;

        void determineActiveLayout();

        Layout1d& getActiveLayout() const
        {
            return *m_active;
        }

        Layout1d& getNonActiveLayout() const
        {
            return *m_nonActive;
        }

    private:
        Layout1d* m_first;
        Layout1d m_second;

        Layout1d* m_active = nullptr;
        Layout1d* m_nonActive = nullptr;

        Selector m_selector;
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

        friend TGUI_API Layout operator+(const Layout& left, const Layout& right);
        friend TGUI_API Layout operator-(const Layout& left, const Layout& right);

        friend TGUI_API Layout operator*(const Layout& left, const Layout1d& right);
        friend TGUI_API Layout operator/(const Layout& left, const Layout1d& right);

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

    TGUI_API Layout1d bindLeft(const std::shared_ptr<Widget>& widget, float fraction = 1);
    TGUI_API Layout1d bindTop(const std::shared_ptr<Widget>& widget, float fraction = 1);
    TGUI_API Layout1d bindRight(const std::shared_ptr<Widget>& widget, float fraction = 1);
    TGUI_API Layout1d bindBottom(const std::shared_ptr<Widget>& widget, float fraction = 1);
    TGUI_API Layout1d bindWidth(const std::shared_ptr<Widget>& widget, float fraction = 1);
    TGUI_API Layout1d bindHeight(const std::shared_ptr<Widget>& widget, float fraction = 1);

    TGUI_API Layout1d bindLeft(const Gui& gui, float fraction = 1);
    TGUI_API Layout1d bindTop(const Gui& gui, float fraction = 1);
    TGUI_API Layout1d bindRight(const Gui& gui, float fraction = 1);
    TGUI_API Layout1d bindBottom(const Gui& gui, float fraction = 1);
    TGUI_API Layout1d bindWidth(const Gui& gui, float fraction = 1);
    TGUI_API Layout1d bindHeight(const Gui& gui, float fraction = 1);

    TGUI_API Layout bindPosition(const std::shared_ptr<Widget>& widget, const sf::Vector2f& fraction = {1,1});
    TGUI_API Layout bindSize(const std::shared_ptr<Widget>& widget, const sf::Vector2f& fraction = {1,1});

    TGUI_API Layout bindPosition(const Gui& gui, const sf::Vector2f& fraction = {1,1});
    TGUI_API Layout bindSize(const Gui& gui, const sf::Vector2f& fraction = {1,1});

    TGUI_API Layout1d bindMinimum(const Layout1d& first, const Layout1d& second);
    TGUI_API Layout1d bindMaximum(const Layout1d& first, const Layout1d& second);

    // Alternative for bindMinimum(maximum, bindMaximum(minimum, value))
    TGUI_API Layout1d bindLimits(const Layout1d& minimum, const Layout1d& maximum, const Layout1d& value);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_LAYOUT_HPP
