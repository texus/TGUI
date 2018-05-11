/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <SFML/System/String.hpp>
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
        Signal(Signal&& other) = default;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Copy assignment operator which will not copy the signal handlers
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Signal& operator=(const Signal& other);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default move assignment operator
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Signal& operator=(Signal&& other) = default;


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
            if (m_handlers.empty())
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
        bool emit(const Widget* widget, Type strParam) \
        { \
            if (m_handlers.empty()) \
                return false; \
         \
            internal_signal::parameters[1] = static_cast<const void*>(&strParam); \
            return Signal::emit(widget); \
        } \
        \
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
        bool emit(const ChildWindow* param);


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
        using DelegateItemEx = std::function<void(std::shared_ptr<Widget>, const std::string&, const sf::String&)>;
        using DelegateItemAndIdEx = std::function<void(std::shared_ptr<Widget>, const std::string&, const sf::String&, const sf::String&)>;

        using Signal::connect;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalItem(std::string&& name) :
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
        /// @internal
        /// @brief Call all connected signal handlers
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool emit(const Widget* widget, const sf::String& item, const sf::String& id)
        {
            if (m_handlers.empty())
                return false;

            internal_signal::parameters[1] = static_cast<const void*>(&item);
            internal_signal::parameters[2] = static_cast<const void*>(&id);
            return Signal::emit(widget);
        }


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
    class TGUI_API SignalMenuItem : public Signal
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
        SignalMenuItem(std::string&& name) :
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


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Checks whether the unbound parameters match with this signal
        /// @return The index in the parameters list where the parameters will be stored
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int validateTypes(std::initializer_list<std::type_index> unboundParameters) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Base class for Widget which provides functionality to connect signals based on their name
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API SignalWidgetBase
    {
    public:

        virtual ~SignalWidgetBase() = default;

#ifdef TGUI_USE_CPP17
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
        template <typename Func, typename... BoundArgs, typename std::enable_if<std::is_convertible<Func, std::function<void(const BoundArgs&..., std::shared_ptr<Widget>, const std::string&)>>::value>::type* = nullptr>
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
    protected:

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
