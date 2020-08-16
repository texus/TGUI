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


#ifndef TGUI_SIGNAL_HPP
#define TGUI_SIGNAL_HPP

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/Global.hpp>
#include <TGUI/String.hpp>
#include <TGUI/Vector2.hpp>
#include <TGUI/Animation.hpp>
#include <type_traits>
#include <functional>
#include <typeindex>
#include <memory>
#include <vector>
#include <deque>
#include <map>

#undef MessageBox  // windows.h defines MessageBox when NOMB isn't defined before including windows.h

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class Widget;
    class ChildWindow;

    /// @brief Lists the names of all signals for all widgets
    namespace Signals
    {
        struct Widget
        {
            static constexpr const char* const PositionChanged = "PositionChanged";     //!< The position of the widget changed. Optional parameter: new position
            static constexpr const char* const SizeChanged = "SizeChanged";             //!< The size of the widget changed. Optional parameter: new size
            static constexpr const char* const Focused = "Focused";                     //!< The widget was focused
            static constexpr const char* const Unfocused = "Unfocused";                 //!< The widget was unfocused
            static constexpr const char* const MouseEntered = "MouseEntered";           //!< The mouse entered the widget
            static constexpr const char* const MouseLeft = "MouseLeft";                 //!< The mouse left the widget
            static constexpr const char* const AnimationFinished = "AnimationFinished"; //!< A show or hide animation finished. Optional parameters: animation type, new widget visibility or both
        };

        struct ClickableWidget : public Widget
        {
            static constexpr const char* const MousePressed = "MousePressed";               //!< The mouse went down on the widget. Optional parameter: mouse position relative to widget
            static constexpr const char* const MouseReleased = "MouseReleased";             //!< The mouse was released on top of the widget. Optional parameter: mouse position relative to widget
            static constexpr const char* const Clicked = "Clicked";                         //!< The widget was clicked. Optional parameter: mouse position relative to widget
            static constexpr const char* const RightMousePressed = "RightMousePressed";     //!< The right mouse button went down on the widget. Optional parameter: mouse position relative to widget
            static constexpr const char* const RightMouseReleased = "RightMouseReleased";   //!< The right mouse button was released on top of the widget. Optional parameter: mouse position relative to widget
            static constexpr const char* const RightClicked = "RightClicked";               //!< The widget was right clicked. Optional parameter: mouse position relative to widget
        };

        struct Button : public ClickableWidget
        {
            static constexpr const char* const Pressed = "Pressed"; //!< The button was pressed. Optional parameter: text of the button
        };

        struct ChildWindow : public Widget
        {
            static constexpr const char* const MousePressed = "MousePressed";           //!< The mouse went down on the widget
            static constexpr const char* const Closed = "Closed";                       //!< The window was closed. Optional parameter: pointer to the window
            static constexpr const char* const Minimized = "Minimized";                 //!< The window was minimized. Optional parameter: pointer to the window
            static constexpr const char* const Maximized = "Maximized";                 //!< The window was maximized. Optional parameter: pointer to the window
            static constexpr const char* const EscapeKeyPressed = "EscapeKeyPressed";   //!< The escape key was pressed while the child window was focused. Optional parameter: pointer to the window
        };

        struct ComboBox : public Widget
        {
            static constexpr const char* const ItemSelected = "ItemSelected";   //!< An item was selected in the combo box. Optional parameter: selected item or its index
        };

        struct EditBox : public ClickableWidget
        {
            static constexpr const char* const TextChanged = "TextChanged";             //!< The text was changed. Optional parameter: new text
            static constexpr const char* const ReturnKeyPressed = "ReturnKeyPressed";   //!< The return key was pressed. Optional parameter: text in the edit box
            static constexpr const char* const ReturnOrUnfocused = "ReturnOrUnfocused"; //!< The return key was pressed or the edit box was unfocused. Optional parameter: text in the edit box
        };

        struct Knob : public Widget
        {
            static constexpr const char* const ValueChanged = "ValueChanged";   //!< The value of the know changed. Optional parameter: new value
        };

        struct Label : public ClickableWidget
        {
            static constexpr const char* const DoubleClicked = "DoubleClicked"; //!< The label was double clicked. Optional parameter: text of the label
        };

        struct ListBox : public Widget
        {
            static constexpr const char* const ItemSelected = "ItemSelected";   //!< An item was selected in the list box. Optional parameter: selected item or its index
            static constexpr const char* const MousePressed = "MousePressed";   //!< The mouse went down on an item. Optional parameter: selected item or its index
            static constexpr const char* const MouseReleased = "MouseReleased"; //!< The mouse was released on one of the items. Optional parameter: selected item or its index
            static constexpr const char* const DoubleClicked = "DoubleClicked"; //!< An item was double clicked. Optional parameter: selected item or its index
        };

        struct ListView : public Widget
        {
            static constexpr const char* const ItemSelected = "ItemSelected";   //!< An item was selected in the list view. Optional parameter: selected item index (-1 when deselecting)
            static constexpr const char* const DoubleClicked = "DoubleClicked"; //!< An item was double clicked. Optional parameter: selected item index
            static constexpr const char* const RightClicked = "RightClicked";   //!< Right mouse clicked. Optional parameter: index of item below mouse (-1 when not on top of item)
            static constexpr const char* const HeaderClicked = "HeaderClicked"; //!< The header was clicked. Optional parameter: column index
        };

        struct MenuBar : public Widget
        {
            /// One of the menu items was clicked.
            /// Optional parameters:
            ///     - The text of the clicked menu item
            ///     - List containing both the name of the menu and the menu item that was clicked
            static constexpr const char* const MenuItemClicked = "MenuItemClicked";
        };

        struct MessageBox : public ChildWindow
        {
            static constexpr const char* const ButtonPressed = "ButtonPressed"; //!< One of the buttons was pressed. Optional parameter: text of the pressed button
        };

        struct Panel : public Widget
        {
            static constexpr const char* const MousePressed = "MousePressed";               //!< The mouse went down on the panel. Optional parameter: mouse position relative to panel
            static constexpr const char* const MouseReleased = "MouseReleased";             //!< The mouse was released on top of the panel. Optional parameter: mouse position relative to panel
            static constexpr const char* const Clicked = "Clicked";                         //!< The panel was clicked. Optional parameter: mouse position relative to panel
            static constexpr const char* const RightMousePressed = "RightMousePressed";     //!< The right mouse button went down on the panel. Optional parameter: mouse position relative to panel
            static constexpr const char* const RightMouseReleased = "RightMouseReleased";   //!< The right mouse button was released on top of the panel. Optional parameter: mouse position relative to panel
            static constexpr const char* const RightClicked = "RightClicked";               //!< The panel was right clicked. Optional parameter: mouse position relative to panel
        };

        struct Picture : public ClickableWidget
        {
            static constexpr const char* const DoubleClicked = "DoubleClicked"; //!< The picture was double clicked. Optional parameter: mouse position relative to picture
        };

        struct ProgressBar : public ClickableWidget
        {
            static constexpr const char* const ValueChanged = "ValueChanged";   //!< Value of the progress bar changed. Optional parameter: new value
            static constexpr const char* const Full = "Full";                   //!< Value of the progress bar changed and he progress bar became full
        };

        struct RadioButton : public ClickableWidget
        {
            static constexpr const char* const Checked = "Checked";     //!< Radio button was checked. Optional parameter: bool which is always true
            static constexpr const char* const Unchecked = "Unchecked"; //!< Radio button was unchecked. Optional parameter: bool which is always false
            static constexpr const char* const Changed = "Changed";     //!< Radio button was checked or unchecked. Optional parameter: bool indicating whether it is checked
        };

        struct RangeSlider : public Widget
        {
            static constexpr const char* const RangeChanged = "RangeChanged";   //!< Value of the slider changed. Optional parameter: new value
        };

        struct Scrollbar : public Widget
        {
            static constexpr const char* const ValueChanged = "ValueChanged";   //!< Value of the scrollbar changed. Optional parameter: new value
        };

        struct Slider : public Widget
        {
            static constexpr const char* const ValueChanged = "ValueChanged";   //!< Value of the slider changed. Optional parameter: new value
        };

        struct SpinButton : public ClickableWidget
        {
            static constexpr const char* const ValueChanged = "ValueChanged";   //!< Value of the spin button changed. Optional parameter: new value
        };

        struct Tabs : public Widget
        {
            static constexpr const char* const TabSelected = "TabSelected";   //!< A tab that was selected. Optional parameter: selected item
        };

        struct TextArea : public Widget
        {
            static constexpr const char* const TextChanged = "TextChanged";             //!< The text was changed. Optional parameter: new text
            static constexpr const char* const SelectionChanged = "SelectionChanged";   //!< Selected text changed
        };

        struct TreeView : public Widget
        {
            static constexpr const char* const ItemSelected = "ItemSelected";   //!< An node was selected in the tree view. Optional parameter: selected node
            static constexpr const char* const DoubleClicked = "DoubleClicked"; //!< A leaf node was double clicked. Optional parameter: selected node
            static constexpr const char* const Expanded = "Expanded";           //!< A branch node was expanded in the tree view. Optional parameter: expanded node
            static constexpr const char* const Collapsed = "Collapsed";         //!< A branch node was collapsed in the tree view. Optional parameter: collapsed node
            static constexpr const char* const RightClicked = "RightClicked";   //!< A node was right clicked. Optional parameter: node below mouse
        };

        using BitmapButton = Button;
        using CheckBox = RadioButton;
        using ScrollablePanel = Panel;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Signal to which the user can subscribe to get callbacks from
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API Signal
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~Signal() = default;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        ///
        /// @param name             Name of the signal
        /// @param extraParameters  Amount of extra parameters to reserve space for
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Signal(String&& name, std::size_t extraParameters = 0) :
            m_name{std::move(name)}
        {
            if (1 + extraParameters > m_parameters.size())
                m_parameters.resize(1 + extraParameters);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Copy constructor which will not copy the signal handlers
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Signal(const Signal& other);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default move constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Signal(Signal&& other) noexcept = default;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Copy assignment operator which will not copy the signal handlers
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Signal& operator=(const Signal& other);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default move assignment operator
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Signal& operator=(Signal&& other) noexcept = default;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that can be passed to the connect function
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs>
        unsigned int operator()(const Func& func, const BoundArgs&... args)
        {
            return connect(func, args...);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            const auto id = ++m_lastSignalId;
#if defined(__cpp_if_constexpr) && (__cpp_if_constexpr >= 201606L)
            if constexpr(sizeof...(BoundArgs) == 0)
                m_handlers[id] = func;
            else
#endif
            {
                m_handlers[id] = [=]{ invokeFunc(func, args...); };
            }

            return id;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that is given a pointer to the widget and the name of the signal as arguments
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., std::shared_ptr<Widget>, const String&)>>::value>* = nullptr>
        unsigned int connectEx(const Func& func, const BoundArgs&... args)
        {
            // The name is copied so that the lambda does not depend on the 'this' pointer
            return connect([func, name=m_name, args...](){ invokeFunc(func, args..., getWidget(), name); });
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Disconnect a signal handler from this signal
        ///
        /// @param id  Unique id of the connection returned by the connect function
        ///
        /// @return True when a connection with this id existed and was removed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool disconnect(unsigned int id);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Disconnect all signal handler from this signal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void disconnectAll();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Call all connected signal handlers
        ///
        /// @param widget  The sender of the signal
        ///
        /// @return True when at least one signal handler was called
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool emit(const Widget* widget);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the name given to the signal
        ///
        /// @return signal name
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        String getName() const
        {
            return m_name;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes whether this signal calls the connected functions when triggered
        ///
        /// @param enabled  Is the signal enabled?
        ///
        /// Signals are enabled by default. Temporarily disabling the signal is the better alternative to disconnecting the
        /// handler and connecting it again a few lines later.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setEnabled(bool enabled)
        {
            m_enabled = enabled;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns whether this signal calls the connected functions when triggered
        ///
        /// @return Is the signal enabled?
        ///
        /// Signals are enabled by default. Temporarily disabling the signal is the better alternative to disconnecting the
        /// handler and connecting it again a few lines later.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool isEnabled() const
        {
            return m_enabled;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Extracts the widget stored in the first parameter
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<Widget> getWidget();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Turns the void* parameters back into its original type right before calling the callback function
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Type>
        static const std::decay_t<Type>& dereferenceParam(std::size_t paramIndex)
        {
            return *static_cast<const std::decay_t<Type>*>(m_parameters[paramIndex]);
        }


#if defined(__cpp_lib_invoke) && (__cpp_lib_invoke >= 201411L)
        template <typename Func, typename... Args>
        static void invokeFunc(Func&& func, Args&&... args)
        {
            std::invoke(std::forward<Func>(func), std::forward<Args>(args)...);
        }
#else
        // std::invoke only exists in c++17 so we use our own implementation to support c++14 compilers
        template <typename Func, typename... Args, typename std::enable_if<std::is_member_pointer<typename std::decay<Func>::type>::value>::type* = nullptr>
        static void invokeFunc(Func&& func, Args&&... args)
        {
            std::mem_fn(func)(std::forward<Args>(args)...);
        }

        template <typename Func, typename... Args, typename std::enable_if<!std::is_member_pointer<typename std::decay<Func>::type>::value>::type* = nullptr>
        static void invokeFunc(Func&& func, Args&&... args)
        {
            std::forward<Func>(func)(std::forward<Args>(args)...);
        }
#endif


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        bool m_enabled = true;
        String m_name;
        std::map<unsigned int, std::function<void()>> m_handlers;

        static unsigned int m_lastSignalId;
        static std::deque<const void*> m_parameters;
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Signal to which the user can subscribe to get callbacks from
    ///
    /// Optional unbound parameters:
    /// - T (value according to template parameter)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename T>
    class SignalTyped : public Signal
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalTyped(String&& name) :
            Signal{std::move(name), 1}
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that can be passed to the connect function
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs>
        unsigned int operator()(const Func& func, const BoundArgs&... args)
        {
            return connect(func, args...);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function without unbound parameters
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect(func, args...);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that have their last parameter unbound of type T
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=]{ invokeFunc(func, args..., dereferenceParam<T>(1)); });
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Call all connected signal handlers
        ///
        /// @param widget  Widget that is triggering the signal
        /// @param param   Parameter that will be passed to callback function if it has an unbound parameter
        ///
        /// @return True when a callback function was executed, false when there weren't any connected callback functions
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool emit(const Widget* widget, T param)
        {
            if (m_handlers.empty())
                return false;

            m_parameters[1] = static_cast<const void*>(&param);
            return Signal::emit(widget);
        }
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Signal to which the user can subscribe to get callbacks from
    ///
    /// Optional unbound parameters:
    /// - T1, T2  (always occur together, having only one unbound parameter is unsupported)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename T1, typename T2>
    class SignalTyped2 : public Signal
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalTyped2(String&& name) :
            Signal{std::move(name), 2}
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that can be passed to the connect function
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs>
        unsigned int operator()(const Func& func, const BoundArgs&... args)
        {
            return connect(func, args...);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function without unbound parameters
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect(func, args...);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that have their last two parameters unbound of types T1 and T2
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., T1, T2)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=]{ invokeFunc(func, args..., dereferenceParam<T1>(1), dereferenceParam<T2>(2)); });
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Call all connected signal handlers
        ///
        /// @param widget  Widget that is triggering the signal
        /// @param param1  First parameter that will be passed to callback function if it has two unbound parameters
        /// @param param2  Second parameter that will be passed to callback function if it has two unbound parameters
        ///
        /// @return True when a callback function was executed, false when there weren't any connected callback functions
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool emit(const Widget* widget, T1 param1, T2 param2)
        {
            if (m_handlers.empty())
                return false;

            m_parameters[1] = static_cast<const void*>(&param1);
            m_parameters[2] = static_cast<const void*>(&param2);
            return Signal::emit(widget);
        }
    };

    using SignalInt = SignalTyped<int>; //!< Signal with one "int" as optional unbound parameter
    using SignalUInt = SignalTyped<unsigned int>; //!< Signal with one "unsigned int" as optional unbound parameter
    using SignalBool = SignalTyped<bool>; //!< Signal with one "bool" as optional unbound parameter
    using SignalFloat = SignalTyped<float>; //!< Signal with one "float" as optional unbound parameter
    using SignalString = SignalTyped<const String&>; //!< Signal with one "String" as optional unbound parameter
    using SignalVector2f = SignalTyped<Vector2f>; //!< Signal with one "Vector2f" as optional unbound parameter
    using SignalRange = SignalTyped2<float, float>; //!< Signal with two floats as optional unbound parameters


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Signal to which the user can subscribe to get callbacks from
    ///
    /// Optional unbound parameters:
    /// - ChildWindow::Ptr
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API SignalChildWindow : public Signal
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalChildWindow(String&& name) :
            Signal{std::move(name), 1}
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that can be passed to the connect function
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs>
        unsigned int operator()(const Func& func, const BoundArgs&... args)
        {
            return connect(func, args...);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function without unbound parameters
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect(func, args...);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that has an unbound ChildWindow::Ptr as last parameter
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., const std::shared_ptr<ChildWindow>&)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=]{ invokeFunc(func, args..., dereferenceChildWindow()); });
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Call all connected signal handlers
        ///
        /// @param param  Widget triggering the signal, will be passed to callback function if it has an unbound parameter
        ///
        /// @return True when a callback function was executed, false when there weren't any connected callback functions
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool emit(ChildWindow* param);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Turns the void* parameter back into the ChildWindow::Ptr right before calling the callback function
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<ChildWindow> dereferenceChildWindow();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Signal to which the user can subscribe to get callbacks from
    ///
    /// Optional unbound parameters:
    /// - int (item index)
    /// - String (item text)
    /// - String, String (item text and item id)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API SignalItem : public Signal
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalItem(String&& name) :
            Signal{std::move(name), 3}
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that can be passed to the connect function
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs>
        unsigned int operator()(const Func& func, const BoundArgs&... args)
        {
            return connect(func, args...);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function without unbound parameters
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect(func, args...);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that has an unbound integer (for the index) as last parameter
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., int)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=]{ invokeFunc(func, args..., dereferenceParam<int>(1)); });
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that has an unbound string (for the item text) as last parameter
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., const String&)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=]{ invokeFunc(func, args..., dereferenceParam<String>(2)); });
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that has an unbound string (for the item text) as last parameter
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., const String&, const String&)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=]{ invokeFunc(func, args..., dereferenceParam<String>(2), dereferenceParam<String>(3)); });
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Call all connected signal handlers
        ///
        /// @param widget Widget that is triggering the signal
        /// @param index  Index of the item, or -1 if there is no item
        /// @param item   Text of the item, or an empty string if there is no item
        /// @param id     Id of the item, or an empty string if there is no item
        ///
        /// @return True when a callback function was executed, false when there weren't any connected callback functions
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool emit(const Widget* widget, int index, const String& item, const String& id);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Signal to which the user can subscribe to get callbacks from
    ///
    /// Optional unbound parameters:
    /// - ShowAnimationType
    /// - bool (true for showing and false for hiding)
    /// - ShowAnimationType, bool
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API SignalAnimation : public Signal
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalAnimation(String&& name) :
            Signal{std::move(name), 2}
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that can be passed to the connect function
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs>
        unsigned int operator()(const Func& func, const BoundArgs&... args)
        {
            return connect(func, args...);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function without unbound parameters
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect(func, args...);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that has an unbound ShowAnimationType as last parameter
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., ShowAnimationType)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=]{ invokeFunc(func, args..., dereferenceParam<ShowAnimationType>(1)); });
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that has an unbound bool (showing widget or not) as last parameter
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., bool)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=]{ invokeFunc(func, args..., dereferenceParam<bool>(2)); });
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that has an ShowAnimationType and bool (showing widget or not) as two last parameters
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., ShowAnimationType, bool)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=]{ invokeFunc(func, args..., dereferenceParam<ShowAnimationType>(1), dereferenceParam<bool>(2)); });
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Call all connected signal handlers
        ///
        /// @param widget  Widget that is triggering the signal
        /// @param type    Type of the animation
        /// @param visible True when the widget became visible, false if the widget is now hidden
        ///
        /// @return True when a callback function was executed, false when there weren't any connected callback functions
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool emit(const Widget* widget, ShowAnimationType type, bool visible);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Signal to which the user can subscribe to get callbacks from
    ///
    /// Optional unbound parameters:
    /// - String (text of the leaf)
    /// - std::vector<String> (texts of hierarchy from root to leaf)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API SignalItemHierarchy : public Signal
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalItemHierarchy(String&& name) :
            Signal{std::move(name), 2}
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that can be passed to the connect function
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs>
        unsigned int operator()(const Func& func, const BoundArgs&... args)
        {
            return connect(func, args...);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function without unbound parameters
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect(func, args...);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that has an unbound string (for the leaf item text) as last parameter
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., const String&)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=]{ invokeFunc(func, args..., dereferenceParam<String>(1)); });
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that has an unbound std::vector<String> (for hierarchy) as last parameter
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., const std::vector<String>&)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=]{ invokeFunc(func, args..., dereferenceParam<std::vector<String>>(2)); });
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Call all connected signal handlers
        ///
        /// @param widget   Widget that is triggering the signal
        /// @param item     Text of the leaf item
        /// @param fullItem Hierarchy from root to leaf
        ///
        /// @return True when a callback function was executed, false when there weren't any connected callback functions
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool emit(const Widget* widget, const String& item, const std::vector<String>& fullItem);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_SIGNAL_HPP
