/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <SFML/System/String.hpp>
#include <SFML/System/Vector2.hpp>
#include <functional>
#include <cstddef>
#include <memory>
#include <vector>
#include <map>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class Widget;
    class ChildWindow;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @internal
    /// @brief Helper function to dereference a void* back to its correct type
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename Type>
    const Type& dereference(const void* obj)
    {
        return *static_cast<const Type*>(obj);
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Signal to which the user can subscribe to get callbacks from
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API Signal
    {
    public:

        using Delegate = std::function<void()>;
        using DelegateEx = std::function<void(std::shared_ptr<Widget>, const std::string&)>;

        Signal(const Signal& other) = delete;
        Signal(Signal&&) = delete;
        Signal& operator=(const Signal& other) = delete;
        Signal& operator=(Signal&&) = delete;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        ///
        /// @param name             Name of the signal
        /// @param extraParameters  Amount of extra parameters to reserve space for
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Signal(std::string&& name, std::size_t extraParameters = 0) :
            m_name{std::move(name)}
        {
            if (1 + extraParameters > m_parameters.size())
                m_parameters.resize(1 + extraParameters);
        }


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

            m_parameters[0] = static_cast<const void*>(&widget);

            for (auto& handler : m_handlers)
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
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Extracts the widget stored in the first parameter
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::shared_ptr<Widget> getWidget() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        std::string m_name;
        std::map<unsigned int, std::function<void()>> m_handlers;

        static std::vector<const void*> m_parameters;
    };


    #define TGUI_SIGNAL_VALUE_DECLARATION(TypeName, Type) \
    /**************************************************************************************************************************** \
     * @brief Signal to which the user can subscribe to get callbacks from
     *
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
         *
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
         *
         ************************************************************************************************************************/ \
        unsigned int connect(const Delegate##TypeName& handler); \
        \
        /************************************************************************************************************************ \
         * @brief Connects a signal handler that will be called when this signal is emitted
         *
         * @param handler  Callback function that is given a pointer to the widget, the name of the signal and a Vector2f as arguments
         *
         * @return Unique id of the connection
         *
         ************************************************************************************************************************/ \
        unsigned int connect(const Delegate##TypeName##Ex& handler); \
        \
        /************************************************************************************************************************ \
         * @internal
         * @brief Call all connected signal handlers
         *
         ************************************************************************************************************************/ \
        bool emit(const Widget* widget, Type strParam) \
        { \
            if (m_handlers.empty()) \
                return false; \
         \
            m_parameters[1] = static_cast<const void*>(&strParam); \
            return Signal::emit(widget); \
        } \
    };


    TGUI_SIGNAL_VALUE_DECLARATION(Int, int)
    TGUI_SIGNAL_VALUE_DECLARATION(UInt, unsigned int)
    TGUI_SIGNAL_VALUE_DECLARATION(Bool, bool)
    TGUI_SIGNAL_VALUE_DECLARATION(Float, float)
    TGUI_SIGNAL_VALUE_DECLARATION(String, const sf::String&)
    TGUI_SIGNAL_VALUE_DECLARATION(Vector2f, sf::Vector2f)


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

            m_parameters[1] = static_cast<const void*>(&item);
            m_parameters[2] = static_cast<const void*>(&id);
            return Signal::emit(widget);
        }

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

            m_parameters[1] = static_cast<const void*>(&item);
            m_parameters[2] = static_cast<const void*>(&fullItem);
            return Signal::emit(widget);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };



    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Wrapper around signals to allow copying and moving
    ///
    /// The signal itself can't be copied of moved. The wrapper will just copy the pointer to the signal when moving but will
    /// reset the signal when being copied. A copied widget has no signal handlers at all.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    template <typename SignalType>
    class SignalWrapper
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalWrapper(std::string&& name) :
            m_signal{std::make_unique<SignalType>(std::move(name))}
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Copy constructor that will default initialize the signal (as handlers are not copied)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalWrapper(const SignalWrapper& other) :
            m_signal{std::make_unique<SignalType>(other->getName())}
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default move constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalWrapper(SignalWrapper&&) = default;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Copy assignment operator that will default initialize the signal (as handlers are not copied)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalWrapper& operator=(const SignalWrapper& other)
        {
            if (this != &other)
                m_signal = std::make_unique<SignalType>(other->getName());

            return *this;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default move assignment operator
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalWrapper& operator=(SignalWrapper&&) = default;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Access the signal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalType& operator*() const
        {
            return *m_signal;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Access the signal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalType* operator->() const
        {
            return m_signal.get();
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        std::unique_ptr<SignalType> m_signal = std::make_unique<SignalType>();
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Base class for Widget which provides functionality to connect signals based on their name
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API SignalWidgetBase
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param signalName   Name of the signal
        /// @param handler      Callback function
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int connect(std::string signalName, const Signal::Delegate& handler)
        {
            return getSignal(toLower(std::move(signalName))).connect(handler);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param signalName   Name of the signal
        /// @param handler      Callback function that is given a pointer to the widget and the name of the signal as arguments
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int connect(std::string signalName, const Signal::DelegateEx& handler)
        {
            return getSignal(toLower(std::move(signalName))).connect(handler);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param signalName   Name of the signal
        /// @param handler      Callback function that is giventhe extra arguments provided to this function as arguments
        /// @param args         Optional extra arguments to pass to the signal handler when the signal is emitted
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... Args, typename std::enable_if<std::is_convertible<Func, std::function<void(Args...)>>::value>::type* = nullptr>
        unsigned int connect(std::string signalName, Func&& handler, Args&&... args)
        {
            return getSignal(toLower(std::move(signalName))).connect([f=std::function<void(Args...)>(handler),args...](){ f(args...); });
        }


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
        template <typename Func, typename... Args, typename std::enable_if<std::is_convertible<Func, std::function<void(Args..., std::shared_ptr<Widget>, const std::string&)>>::value>::type* = nullptr>
        unsigned int connect(std::string signalName, Func&& handler, Args&&... args)
        {
            return getSignal(toLower(std::move(signalName))).connect(
                [f=std::function<void(Args..., std::shared_ptr<Widget>, const std::string&)>(handler), args...]
                (const std::shared_ptr<Widget>& w, const std::string& s)
                { f(args..., w, s); }
            );
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Disconnects a signal handler
        ///
        /// @param signalName   Name of the signal
        /// @param id           Id of the connection
        ///
        /// @return True when a connection with this id existed and was removed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool disconnect(std::string signalName, unsigned int id)
        {
            return getSignal(toLower(std::move(signalName))).disconnect(id);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Disconnects all signal handler from a certain signal
        ///
        /// @param signalName   Name of the signal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void disconnectAll(std::string signalName)
        {
            return getSignal(toLower(std::move(signalName))).disconnectAll();
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Retrieves a signal based on its name
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual Signal& getSignal(std::string&& signalName) = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_SIGNAL_HPP
