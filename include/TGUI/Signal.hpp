/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2019 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Vector2f.hpp>
#include <TGUI/Animation.hpp>
#include <SFML/System/String.hpp>
#include <type_traits>
#include <functional>
#include <typeindex>
#include <memory>
#include <vector>
#include <deque>
#include <map>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class Widget;
    class ChildWindow;
    class SignalWidgetBase;

    namespace internal_signal
    {
        extern TGUI_API std::deque<const void*> parameters;
    }

    /// @brief Lists the names of all signals for all widgets
    namespace Signals
    {
        struct Widget
        {
            static constexpr const char* const PositionChanged = "PositionChanged";     ///< The position of the widget changed. Optional parameter: new position
            static constexpr const char* const SizeChanged = "SizeChanged";             ///< The size of the widget changed. Optional parameter: new size
            static constexpr const char* const Focused = "Focused";                     ///< The widget was focused
            static constexpr const char* const Unfocused = "Unfocused";                 ///< The widget was unfocused
            static constexpr const char* const MouseEntered = "MouseEntered";           ///< The mouse entered the widget
            static constexpr const char* const MouseLeft = "MouseLeft";                 ///< The mouse left the widget
            static constexpr const char* const AnimationFinished = "AnimationFinished"; ///< A show or hide animation finished. Optional parameters: animation type, new widget visibility or both
        };

        struct ClickableWidget : public Widget
        {
            static constexpr const char* const MousePressed = "MousePressed";               ///< The mouse went down on the widget. Optional parameter: mouse position relative to widget
            static constexpr const char* const MouseReleased = "MouseReleased";             ///< The mouse was released on top of the widget. Optional parameter: mouse position relative to widget
            static constexpr const char* const Clicked = "Clicked";                         ///< The widget was clicked. Optional parameter: mouse position relative to widget
            static constexpr const char* const RightMousePressed = "RightMousePressed";     ///< The right mouse button went down on the widget. Optional parameter: mouse position relative to widget
            static constexpr const char* const RightMouseReleased = "RightMouseReleased";   ///< The right mouse button was released on top of the widget. Optional parameter: mouse position relative to widget
            static constexpr const char* const RightClicked = "RightClicked";               ///< The widget was right clicked. Optional parameter: mouse position relative to widget
        };

        struct Button : public ClickableWidget
        {
            static constexpr const char* const Pressed = "Pressed"; ///< The button was pressed. Optional parameter: text of the button
        };

        struct ChildWindow : public Widget
        {
            static constexpr const char* const MousePressed = "MousePressed";           ///< The mouse went down on the widget
            static constexpr const char* const Closed = "Closed";                       ///< The window was closed. Optional parameter: pointer to the window
            static constexpr const char* const Minimized = "Minimized";                 ///< The window was minimized. Optional parameter: pointer to the window
            static constexpr const char* const Maximized = "Maximized";                 ///< The window was maximized. Optional parameter: pointer to the window
            static constexpr const char* const EscapeKeyPressed = "EscapeKeyPressed";   ///< The escape key was pressed while the child window was focused. Optional parameter: pointer to the window
        };

        struct ComboBox : public Widget
        {
            static constexpr const char* const ItemSelected = "ItemSelected";   ///< An item was selected in the combo box. Optional parameter: selected item or its index
        };

        struct EditBox : public ClickableWidget
        {
            static constexpr const char* const TextChanged = "TextChanged";             ///< The text was changed. Optional parameter: new text
            static constexpr const char* const ReturnKeyPressed = "ReturnKeyPressed";   ///< The return key was pressed. Optional parameter: text in the edit box
        };

        struct Knob : public Widget
        {
            static constexpr const char* const ValueChanged = "ValueChanged";   ///< The value of the know changed. Optional parameter: new value
        };

        struct Label : public ClickableWidget
        {
            static constexpr const char* const DoubleClicked = "DoubleClicked"; ///< The label was double clicked. Optional parameter: text of the label
        };

        struct ListBox : public Widget
        {
            static constexpr const char* const ItemSelected = "ItemSelected";   ///< An item was selected in the list box. Optional parameter: selected item or its index
            static constexpr const char* const MousePressed = "MousePressed";   ///< The mouse went down on an item. Optional parameter: selected item or its index
            static constexpr const char* const MouseReleased = "MouseReleased"; ///< The mouse was released on one of the items. Optional parameter: selected item or its index
            static constexpr const char* const DoubleClicked = "DoubleClicked"; ///< An item was double clicked. Optional parameter: selected item or its index
        };

        struct ListView : public Widget
        {
            static constexpr const char* const ItemSelected = "ItemSelected";   ///< An item was selected in the list view. Optional parameter: selected item index (-1 when deselecting)
            static constexpr const char* const DoubleClicked = "DoubleClicked"; ///< An item was double clicked. Optional parameter: selected item index
            static constexpr const char* const RightClicked = "RightClicked";   ///< Right mouse clicked. Optional parameter: index of item below mouse (-1 when not on top of item)
            static constexpr const char* const HeaderClicked = "HeaderClicked"; ///< The header was clicked. Optional parameter: column index
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
            static constexpr const char* const ButtonPressed = "ButtonPressed"; ///< One of the buttons was pressed. Optional parameter: text of the pressed button
        };

        struct Panel : public Widget
        {
            static constexpr const char* const MousePressed = "MousePressed";               ///< The mouse went down on the panel. Optional parameter: mouse position relative to panel
            static constexpr const char* const MouseReleased = "MouseReleased";             ///< The mouse was released on top of the panel. Optional parameter: mouse position relative to panel
            static constexpr const char* const Clicked = "Clicked";                         ///< The panel was clicked. Optional parameter: mouse position relative to panel
            static constexpr const char* const RightMousePressed = "RightMousePressed";     ///< The right mouse button went down on the panel. Optional parameter: mouse position relative to panel
            static constexpr const char* const RightMouseReleased = "RightMouseReleased";   ///< The right mouse button was released on top of the panel. Optional parameter: mouse position relative to panel
            static constexpr const char* const RightClicked = "RightClicked";               ///< The panel was right clicked. Optional parameter: mouse position relative to panel
        };

        struct Picture : public ClickableWidget
        {
            static constexpr const char* const DoubleClicked = "DoubleClicked"; ///< The picture was double clicked. Optional parameter: mouse position relative to picture
        };

        struct ProgressBar : public ClickableWidget
        {
            static constexpr const char* const ValueChanged = "ValueChanged";   ///< Value of the progress bar changed. Optional parameter: new value
            static constexpr const char* const Full = "Full";                   ///< Value of the progress bar changed and he progress bar became full
        };

        struct RadioButton : public ClickableWidget
        {
            static constexpr const char* const Checked = "Checked";     ///< Radio button was checked. Optional parameter: bool which is always true
            static constexpr const char* const Unchecked = "Unchecked"; ///< Radio button was unchecked. Optional parameter: bool which is always false
            static constexpr const char* const Changed = "Changed";     ///< Radio button was checked or unchecked. Optional parameter: bool indicating whether it is checked
        };

        struct RangeSlider : public Widget
        {
            static constexpr const char* const RangeChanged = "RangeChanged";   ///< Value of the slider changed. Optional parameter: new value
        };

        struct Scrollbar : public Widget
        {
            static constexpr const char* const ValueChanged = "ValueChanged";   ///< Value of the scrollbar changed. Optional parameter: new value
        };

        struct Slider : public Widget
        {
            static constexpr const char* const ValueChanged = "ValueChanged";   ///< Value of the slider changed. Optional parameter: new value
        };

        struct SpinButton : public ClickableWidget
        {
            static constexpr const char* const ValueChanged = "ValueChanged";   ///< Value of the spin button changed. Optional parameter: new value
        };

        struct Tabs : public Widget
        {
            static constexpr const char* const TabSelected = "TabSelected";   ///< A tab that was selected. Optional parameter: selected item
        };

        struct TextBox : public Widget
        {
            static constexpr const char* const TextChanged = "TextChanged";             ///< The text was changed. Optional parameter: new text
            static constexpr const char* const SelectionChanged = "SelectionChanged";   ///< Selected text changed
        };

        struct TreeView : public Widget
        {
            static constexpr const char* const ItemSelected = "ItemSelected";   ///< An node was selected in the tree view. Optional parameter: selected node
            static constexpr const char* const DoubleClicked = "DoubleClicked"; ///< A leaf node was double clicked. Optional parameter: selected node
            static constexpr const char* const Expanded = "Expanded";           ///< A branch node was expanded in the tree view. Optional parameter: expanded node
            static constexpr const char* const Collapsed = "Collapsed";         ///< A branch node was collapsed in the tree view. Optional parameter: collapsed node
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

        using Delegate = std::function<void()>;
        using DelegateEx = std::function<void(std::shared_ptr<Widget>, const std::string&)>;


        virtual ~Signal() = default;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        ///
        /// @param name             Name of the signal
        /// @param extraParameters  Amount of extra parameters to reserve space for
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Signal(std::string&& name, std::size_t extraParameters = 0) :
            m_name{std::move(name)}
        {
            if (1 + extraParameters > internal_signal::parameters.size())
                internal_signal::parameters.resize(1 + extraParameters);
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
        /// @param handler  Callback function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int connect(const Delegate& handler);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param handler  Callback function that is given a pointer to the widget and the name of the signal as arguments
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int connect(const DelegateEx& handler);


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
        bool emit(const Widget* widget)
        {
            if (m_handlers.empty() || !m_enabled)
                return false;

            internal_signal::parameters[0] = static_cast<const void*>(&widget);

            // Copy the handlers before calling them in case the widget (and this signal) gets destroyed during the handler
            auto handlers = m_handlers;
            for (auto& handler : handlers)
                handler.second();

            return true;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the name given to the signal
        ///
        /// @return signal name
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::string getName() const
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
        /// @internal
        /// @brief Checks whether the unbound parameters match with this signal
        /// @return The index in the parameters list where the parameters will be stored
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual unsigned int validateTypes(std::initializer_list<std::type_index> unboundParameters) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Extracts the widget stored in the first parameter
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<Widget> getWidget();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        bool m_enabled = true;
        std::string m_name;
        std::map<unsigned int, std::function<void()>> m_handlers;
    };

    #define TGUI_SIGNAL_VALUE_DECLARATION(TypeName, Type) \
    /**************************************************************************************************************************** \
     * @brief Signal to which the user can subscribe to get callbacks from
     ****************************************************************************************************************************/ \
    class TGUI_API Signal##TypeName : public Signal \
    { \
    public: \
        using Delegate##TypeName = std::function<void(Type)>; \
        using Delegate##TypeName##Ex = std::function<void(std::shared_ptr<Widget>, const std::string&, Type)>; \
        using Signal::connect; \
        \
        /************************************************************************************************************************ \
         * @brief Constructor
         ************************************************************************************************************************/ \
        Signal##TypeName(std::string&& name) : \
            Signal{std::move(name), 1} \
        { \
        } \
        \
        /************************************************************************************************************************ \
         * @brief Connects a signal handler that will be called when this signal is emitted
         *
         * @param handler  Callback function that is given a Vector2f as argument
         *
         * @return Unique id of the connection
         ************************************************************************************************************************/ \
        unsigned int connect(const Delegate##TypeName& handler); \
        \
        /************************************************************************************************************************ \
         * @brief Connects a signal handler that will be called when this signal is emitted
         *
         * @param handler  Callback function that is given a pointer to the widget, the name of the signal and a Vector2f as arguments
         *
         * @return Unique id of the connection
         ************************************************************************************************************************/ \
        unsigned int connect(const Delegate##TypeName##Ex& handler); \
        \
        /************************************************************************************************************************ \
         * @internal
         * @brief Call all connected signal handlers
         ************************************************************************************************************************/ \
        bool emit(const Widget* widget, Type param) \
        { \
            if (m_handlers.empty()) \
                return false; \
         \
            internal_signal::parameters[1] = static_cast<const void*>(&param); \
            return Signal::emit(widget); \
        } \
        \
    private: \
        /************************************************************************************************************************ \
         * @internal
         * @brief Checks whether the unbound parameters match with this signal
         * @return The index in the parameters list where the parameters will be stored
         ************************************************************************************************************************/ \
        unsigned int validateTypes(std::initializer_list<std::type_index> unboundParameters) const override; \
    };

    TGUI_SIGNAL_VALUE_DECLARATION(Int, int)
    TGUI_SIGNAL_VALUE_DECLARATION(UInt, unsigned int)
    TGUI_SIGNAL_VALUE_DECLARATION(Bool, bool)
    TGUI_SIGNAL_VALUE_DECLARATION(Float, float)
    TGUI_SIGNAL_VALUE_DECLARATION(String, const sf::String&)
    TGUI_SIGNAL_VALUE_DECLARATION(Vector2f, Vector2f)


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Signal to which the user can subscribe to get callbacks from
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API SignalRange : public Signal
    {
    public:

        using DelegateRange = std::function<void(float, float)>;
        using DelegateRangeEx = std::function<void(std::shared_ptr<Widget>, const std::string&, float, float)>;
        using Signal::connect;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalRange(std::string&& name) :
            Signal{std::move(name), 2}
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param handler  Callback function that is given a child window as argument
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int connect(const DelegateRange& handler);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param handler  Callback function that is given a pointer to the widget, the name of the signal and a child window as arguments
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int connect(const DelegateRangeEx& handler);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Call all connected signal handlers
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool emit(const Widget* widget, float value1, float value2);


    private:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Checks whether the unbound parameters match with this signal
        /// @return The index in the parameters list where the parameters will be stored
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int validateTypes(std::initializer_list<std::type_index> unboundParameters) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Signal to which the user can subscribe to get callbacks from
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API SignalChildWindow : public Signal
    {
    public:

        using DelegateChildWindow = std::function<void(std::shared_ptr<ChildWindow>)>;
        using DelegateChildWindowEx = std::function<void(std::shared_ptr<Widget>, const std::string&, std::shared_ptr<ChildWindow>)>;
        using Signal::connect;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalChildWindow(std::string&& name) :
            Signal{std::move(name), 1}
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param handler  Callback function that is given a child window as argument
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int connect(const DelegateChildWindow& handler);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param handler  Callback function that is given a pointer to the widget, the name of the signal and a child window as arguments
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int connect(const DelegateChildWindowEx& handler);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Call all connected signal handlers
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool emit(ChildWindow* param);


    private:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Checks whether the unbound parameters match with this signal
        /// @return The index in the parameters list where the parameters will be stored
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int validateTypes(std::initializer_list<std::type_index> unboundParameters) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Signal to which the user can subscribe to get callbacks from
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API SignalItem : public Signal
    {
    public:

        using DelegateItem = std::function<void(const sf::String&)>;
        using DelegateItemAndId = std::function<void(const sf::String&, const sf::String&)>;
        using DelegateItemIndex = std::function<void(int)>;
        using DelegateItemEx = std::function<void(std::shared_ptr<Widget>, const std::string&, const sf::String&)>;
        using DelegateItemAndIdEx = std::function<void(std::shared_ptr<Widget>, const std::string&, const sf::String&, const sf::String&)>;
        using DelegateItemIndexEx = std::function<void(std::shared_ptr<Widget>, const std::string&, int)>;
        using Signal::connect;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalItem(std::string&& name) :
            Signal{std::move(name), 3}
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param handler  Callback function that is given a string as argument
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int connect(const DelegateItem& handler);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param handler  Callback function that is given a pointer to the widget, the name of the signal and a string as arguments
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int connect(const DelegateItemEx& handler);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param handler  Callback function that is given two strings as argument
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int connect(const DelegateItemAndId& handler);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param handler  Callback function that is given a pointer to the widget, the name of the signal and two strings as arguments
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int connect(const DelegateItemAndIdEx& handler);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param handler  Callback function that is given the index of the item as argument
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int connect(const DelegateItemIndex& handler);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param handler  Callback function that is given a pointer to the widget, the name of the signal and the index of the item as arguments
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int connect(const DelegateItemIndexEx& handler);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Call all connected signal handlers
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool emit(const Widget* widget, const sf::String& item, const sf::String& id, int index)
        {
            if (m_handlers.empty())
                return false;

            internal_signal::parameters[1] = static_cast<const void*>(&item);
            internal_signal::parameters[2] = static_cast<const void*>(&id);
            internal_signal::parameters[3] = static_cast<const void*>(&index);
            return Signal::emit(widget);
        }


    private:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Checks whether the unbound parameters match with this signal
        /// @return The index in the parameters list where the parameters will be stored
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int validateTypes(std::initializer_list<std::type_index> unboundParameters) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Signal to which the user can subscribe to get callbacks from
    ///
    /// Possible optional parameters:
    /// - ShowAnimationType
    /// - bool
    /// - ShowAnimationType, bool
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API SignalAnimation : public Signal
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalAnimation(std::string&& name) :
            Signal{std::move(name), 2}
        {
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Call all connected signal handlers
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool emit(const Widget* widget, ShowAnimationType type, bool visible)
        {
            if (m_handlers.empty())
                return false;

            internal_signal::parameters[1] = static_cast<const void*>(&type);
            internal_signal::parameters[2] = static_cast<const void*>(&visible);
            return Signal::emit(widget);
        }


    private:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Checks whether the unbound parameters match with this signal
        /// @return The index in the parameters list where the parameters will be stored
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int validateTypes(std::initializer_list<std::type_index> unboundParameters) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Signal to which the user can subscribe to get callbacks from
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API SignalItemHierarchy : public Signal
    {
    public:

        using DelegateMenuItem = std::function<void(const sf::String&)>;
        using DelegateMenuItemFull = std::function<void(const std::vector<sf::String>&)>;
        using DelegateMenuItemEx = std::function<void(std::shared_ptr<Widget>, const std::string&, const sf::String&)>;
        using DelegateMenuItemFullEx = std::function<void(std::shared_ptr<Widget>, const std::string&, const std::vector<sf::String>&)>;
        using Signal::connect;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalItemHierarchy(std::string&& name) :
            Signal{std::move(name), 2}
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param handler  Callback function that is given a string as argument
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int connect(const DelegateMenuItem& handler);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param handler  Callback function that is given a pointer to the widget, the name of the signal and a string as arguments
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int connect(const DelegateMenuItemEx& handler);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param handler  Callback function that is given a list of strings as argument
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int connect(const DelegateMenuItemFull& handler);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param handler  Callback function that is given a pointer to the widget, the name of the signal and a list of strings as arguments
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int connect(const DelegateMenuItemFullEx& handler);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Call all connected signal handlers
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool emit(const Widget* widget, const sf::String& item, const std::vector<sf::String>& fullItem)
        {
            if (m_handlers.empty())
                return false;

            internal_signal::parameters[1] = static_cast<const void*>(&item);
            internal_signal::parameters[2] = static_cast<const void*>(&fullItem);
            return Signal::emit(widget);
        }


    private:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Checks whether the unbound parameters match with this signal
        /// @return The index in the parameters list where the parameters will be stored
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int validateTypes(std::initializer_list<std::type_index> unboundParameters) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

#ifndef TGUI_REMOVE_DEPRECATED_CODE
    using SignalMenuItem = SignalItemHierarchy;
#endif


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Base class for Widget which provides functionality to connect signals based on their name
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API SignalWidgetBase
    {
    public:

        virtual ~SignalWidgetBase() = default;

#if defined(__cpp_if_constexpr) && (__cpp_if_constexpr >= 201606L)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param signalName   Name of the signal
        /// @param handler      Callback function that is given the extra arguments provided to this function as arguments
        /// @param args         Optional extra arguments to pass to the signal handler when the signal is emitted
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs>
        unsigned int connect(std::string signalName, Func&& handler, const BoundArgs&... args);

#else
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param signalName   Name of the signal
        /// @param handler      Callback function that is given the extra arguments provided to this function as arguments
        /// @param args         Optional extra arguments to pass to the signal handler when the signal is emitted
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... Args, typename std::enable_if<std::is_convertible<Func, std::function<void(const Args&...)>>::value>::type* = nullptr>
        unsigned int connect(std::string signalName, Func&& handler, const Args&... args);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param signalName   Name of the signal
        /// @param handler      Callback function that is given a pointer to the widget, the name of the signal and the extra
        ///                     arguments provided to this function as arguments
        /// @param args         Optional extra arguments to pass to the signal handler when the signal is emitted
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if<!std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value // Ambigious otherwise when passing bind expression
                                                                             && std::is_convertible<Func, std::function<void(const BoundArgs&..., std::shared_ptr<Widget>, const std::string&)>>::value>::type* = nullptr>
        unsigned int connect(std::string signalName, Func&& handler, BoundArgs&&... args);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param signalName   Name of the signal
        /// @param handler      Callback function
        /// @param args         Optional extra arguments to pass to the signal handler when the signal is emitted
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if<!std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value
                                                                             && !std::is_convertible<Func, std::function<void(const BoundArgs&..., std::shared_ptr<Widget>, const std::string&)>>::value>::type* = nullptr>
        unsigned int connect(std::string signalName, Func&& handler, BoundArgs&&... args);
#endif

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connect a signal handler to multiple signals
        ///
        /// @param signalNames  List of signal names that will trigger the signal handler
        /// @param handler      Callback function
        /// @param args         Optional extra arguments to pass to the signal handler when the signal is emitted
        ///
        /// @return Unique id of the last connection. When passing e.g. 2 signal names, the first signal will correspond to id-1.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs>
        unsigned int connect(std::initializer_list<std::string> signalNames, Func&& handler, BoundArgs&&... args);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Disconnects a signal handler
        ///
        /// @param id           Id of the connection
        ///
        /// @return True when a connection with this id existed and was removed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool disconnect(unsigned int id);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Disconnects all signal handler from a certain signal
        ///
        /// @param signalName   Name of the signal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void disconnectAll(std::string signalName);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Disconnects all signal handlers from signals
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void disconnectAll();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Retrieves a signal based on its name
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual Signal& getSignal(std::string signalName) = 0;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        std::map<unsigned int, std::string> m_connectedSignals;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_SIGNAL_HPP
