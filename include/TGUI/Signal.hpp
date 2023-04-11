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


#ifndef TGUI_SIGNAL_HPP
#define TGUI_SIGNAL_HPP

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/Global.hpp>
#include <TGUI/String.hpp>
#include <TGUI/Color.hpp>
#include <TGUI/Vector2.hpp>
#include <TGUI/Animation.hpp>
#include <TGUI/Filesystem.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <unordered_map>
    #include <type_traits>
    #include <functional>
    #include <typeindex>
    #include <memory>
    #include <vector>
    #include <deque>
#endif

#undef MessageBox  // windows.h defines MessageBox when NOMB isn't defined before including windows.h

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    class Widget;
    class ChildWindow;
    class Panel;

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
        /// @param args  Additional arguments to pass to the function
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
        TGUI_NODISCARD String getName() const
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
        TGUI_NODISCARD bool isEnabled() const
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
        TGUI_NODISCARD static const std::decay_t<Type>& dereferenceParam(std::size_t paramIndex)
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
        template <typename Func, typename... Args, typename std::enable_if_t<std::is_member_pointer<std::decay_t<Func>>::value>* = nullptr>
        static void invokeFunc(Func&& func, Args&&... args)
        {
            (std::mem_fn(func))(std::forward<Args>(args)...);
        }

        template <typename Func, typename... Args, typename std::enable_if_t<!std::is_member_pointer<std::decay_t<Func>>::value>* = nullptr>
        static void invokeFunc(Func&& func, Args&&... args)
        {
            std::forward<Func>(func)(std::forward<Args>(args)...);
        }
#endif


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        bool m_enabled = true;
        String m_name;
        std::unordered_map<unsigned int, std::function<void()>> m_handlers;

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
    using SignalColor = SignalTyped<Color>; //!< Signal with one "Color" as optional unbound parameter
    using SignalString = SignalTyped<const String&>; //!< Signal with one "String" as optional unbound parameter
    using SignalVector2f = SignalTyped<Vector2f>; //!< Signal with one "Vector2f" as optional unbound parameter
    using SignalFloatRect = SignalTyped<FloatRect>; //!< Signal with one "FloatRect" as optional unbound parameter
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
        /// @param func  Callback function that has 2 unbound strings (for the item text and id) as last parameters
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
    /// - int (item index)
    /// - std::shared_ptr<Panel> (item ptr)
    /// - String (item id)
    /// - int, std::shared_ptr<Panel> (item index and item ptr)
    /// - std::shared_ptr<Panel>, String (item ptr and item id)
    /// - int, std::shared_ptr<Panel>, String (item index, item ptr and item id)
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API SignalPanelListBoxItem : public Signal
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalPanelListBoxItem(String&& name) :
            Signal{std::move(name), 3}
        { }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that can be passed to the connect function
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template<typename Func, typename... BoundArgs>
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
        template<typename Func, typename... BoundArgs, std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect(func, args...);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that has an unbound integer (for the item index) as last parameter
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template<typename Func, typename... BoundArgs, std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., int)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=] { invokeFunc(func, args..., dereferenceParam<int>(1)); });
        }

        
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that has an unbound std::shared_ptr<Panel> (for the item ptr) as last parameter
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template<typename Func, typename... BoundArgs, std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., const std::shared_ptr<Panel>&)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=] { invokeFunc(func, args..., dereferencePanel()); });
        }

        
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that has an unbound String (for the item id) as last parameter
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template<typename Func, typename... BoundArgs, std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., const String&)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=] { invokeFunc(func, args..., dereferenceParam<String>(3)); });
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that has unbound integer and std::shared_ptr<Panel> (for the item index and ptr) as last parameters
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template<typename Func, typename... BoundArgs, std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., int, const std::shared_ptr<Panel>&)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=] { invokeFunc(func, args..., dereferenceParam<int>(1), dereferencePanel()); });
        }

        
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that has unbound std::shared_ptr<Panel> and String (for the item ptr and id) as last parameters
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template<typename Func, typename... BoundArgs, std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., const std::shared_ptr<Panel>&, const String&)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=] { invokeFunc(func, args..., dereferencePanel(), dereferenceParam<String>(3)); });
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that has unbound integer, std::shared_ptr<Panel> and String (for the item index, ptr and id) as last parameters
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template<typename Func, typename... BoundArgs, std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., int, const std::shared_ptr<Panel>&, const String&)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=] { invokeFunc(func, args..., dereferenceParam<int>(1), dereferencePanel(), dereferenceParam<String>(3)); });
        }

        
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Call all connected signal handlers
        ///
        /// @param widget Widget that is triggering the signal
        /// @param index  Index of the item, or -1 if there is no item
        /// @param panel  Pointer of the item, or an nullptr if there is no item
        /// @param id     Id of the item, or an empty string if there is no item
        ///
        /// @return True when a callback function was executed, false when there weren't any connected callback functions
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool emit(const Widget* widget, int index, const std::shared_ptr<Panel>& panel, const String& id);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Turns the void* parameter back into the std::shared_ptr<Panel> right before calling the callback function
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::shared_ptr<Panel> dereferencePanel();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Signal to which the user can subscribe to get callbacks from
    ///
    /// Optional unbound parameters:
    /// - vector<Filesystem::Path>
    /// - Filesystem::Path
    /// - String
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API SignalFileDialogPaths : public Signal
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalFileDialogPaths(String&& name) :
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
        /// @param func  Callback function that has an unbound string (for the first path) as last parameter
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
        /// @param func  Callback function that has an unbound Filesystem::Path (for the first path) as last parameter
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., const Filesystem::Path&)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=]{ invokeFunc(func, args..., dereferenceParam<Filesystem::Path>(2)); });
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param func  Callback function that has an unbound vector of Filesystem::Path (for all the paths) as last parameter
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., const std::vector<Filesystem::Path>&)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=]{ invokeFunc(func, args..., dereferenceParam<std::vector<Filesystem::Path>>(3)); });
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Call all connected signal handlers
        ///
        /// @param widget Widget that is triggering the signal
        /// @param paths  List of paths to selected files
        ///
        /// @return True when a callback function was executed, false when there weren't any connected callback functions
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool emit(const Widget* widget, const std::vector<Filesystem::Path>& paths);
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Signal to which the user can subscribe to get callbacks from
    ///
    /// Optional unbound parameters:
    /// - ShowEffectType
    /// - bool (true for showing and false for hiding)
    /// - ShowEffectType, bool
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API SignalShowEffect : public Signal
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalShowEffect(String&& name) :
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
        /// @param func  Callback function that has an unbound ShowEffectType as last parameter
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., ShowEffectType)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=]{ invokeFunc(func, args..., dereferenceParam<ShowEffectType>(1)); });
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
        /// @param func  Callback function that has an ShowEffectType and bool (showing widget or not) as two last parameters
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., ShowEffectType, bool)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=]{ invokeFunc(func, args..., dereferenceParam<ShowEffectType>(1), dereferenceParam<bool>(2)); });
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
        bool emit(const Widget* widget, ShowEffectType type, bool visible);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Signal to which the user can subscribe to get callbacks from
    ///
    /// Optional unbound parameters:
    /// - AnimationType
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API SignalAnimationType : public Signal
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalAnimationType(String&& name) :
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
        /// @param func  Callback function that has an unbound AnimationType as last parameter
        /// @param args  Additional arguments to pass to the function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const BoundArgs&..., AnimationType)>>::value>* = nullptr>
        unsigned int connect(const Func& func, const BoundArgs&... args)
        {
            return Signal::connect([=]{ invokeFunc(func, args..., dereferenceParam<AnimationType>(1)); });
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Call all connected signal handlers
        ///
        /// @param widget  Widget that is triggering the signal
        /// @param type    Type of the animation
        ///
        /// @return True when a callback function was executed, false when there weren't any connected callback functions
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool emit(const Widget* widget, AnimationType type);


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
