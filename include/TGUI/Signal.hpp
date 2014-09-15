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


#ifndef TGUI_SIGNAL_HPP
#define TGUI_SIGNAL_HPP


#include <TGUI/Global.hpp>

#include <map>
#include <memory>
#include <cassert>
#include <functional>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class Widget;
    class ChildWindow;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class Signal
    {
    private:

        #define PARAMETERS(Name) using Name##Parameter = struct Name##ParametersStruct {};

        PARAMETERS(None)
        PARAMETERS(Int)
        PARAMETERS(Vector2f)
        PARAMETERS(String)
        PARAMETERS(DoubleString)
        PARAMETERS(StringList)
        PARAMETERS(ChildWindowPtr)
        PARAMETERS(Error)

        #undef PARAMETERS

    public:

        template <typename Func>
        void connect(unsigned int id, Func func)
        {
            #define TGUI_CONDITINAL(TypeA, TypeB, ...) typename std::conditional<std::is_convertible<Func, std::function<void(__VA_ARGS__)>>::value, TypeA, TypeB>::type

            m_functions[id] = connectInternal(
                TGUI_CONDITINAL(
                    NoneParameter,
                    TGUI_CONDITINAL(
                        IntParameter,
                        TGUI_CONDITINAL(
                            Vector2fParameter,
                            TGUI_CONDITINAL(
                                StringParameter,
                                TGUI_CONDITINAL(
                                    DoubleStringParameter,
                                        TGUI_CONDITINAL(StringListParameter,
                                            TGUI_CONDITINAL(ChildWindowPtrParameter, ErrorParameter, std::shared_ptr<ChildWindow>),
                                        std::vector<sf::String>),
                                    sf::String, sf::String),
                                sf::String),
                            sf::Vector2f),
                        int)
                    ){},
                func
            );

            #undef TGUI_CONDITINAL
        }

        template <typename Func, typename... Args>
        void connect(unsigned int id, Func func, Args... args)
        {
            #define TGUI_CONDITINAL(TypeA, TypeB, ...) typename std::conditional<std::is_convertible<decltype(std::bind(func, __VA_ARGS__)), std::function<void()>>::value, TypeA, TypeB>::type

            m_functions[id] = connectInternal(
                TGUI_CONDITINAL(
                    NoneParameter,
                    TGUI_CONDITINAL(
                        IntParameter,
                        TGUI_CONDITINAL(
                            Vector2fParameter,
                            TGUI_CONDITINAL(
                                StringParameter,
                                TGUI_CONDITINAL(
                                    DoubleStringParameter,
                                        TGUI_CONDITINAL(StringListParameter,
                                            TGUI_CONDITINAL(ChildWindowPtrParameter, ErrorParameter, args..., m_childWindowPtr),
                                        args..., m_stringList),
                                    args..., m_string, m_string2),
                                args..., m_string),
                            args..., m_vector2f),
                        args..., m_int),
                    args...){},
                func, args...
            );

            #undef TGUI_CONDITINAL
        }

        bool disconnect(unsigned int id);

        void disconnectAll();

        bool isEmpty();

        void operator()();

        #define SEND_SIGNAL_ONE_PARAMETER(Type, Variable) \
            void operator()(const Type& value) \
            { \
                Variable = value; \
                (*this)(); \
            }

        #define SEND_SIGNAL_TWO_PARAMETERS(Type1, Type2, Variable1, Variable2) \
            void operator()(const Type1& value1, const Type2& value2) \
            { \
                Variable1 = value1; \
                Variable2 = value2; \
                (*this)(); \
            }

        SEND_SIGNAL_ONE_PARAMETER(int, m_int)
        SEND_SIGNAL_ONE_PARAMETER(sf::Vector2f, m_vector2f)
        SEND_SIGNAL_ONE_PARAMETER(sf::String, m_string)
        SEND_SIGNAL_ONE_PARAMETER(std::vector<sf::String>, m_stringList)

        SEND_SIGNAL_TWO_PARAMETERS(sf::String, sf::String, m_string, m_string2)
        SEND_SIGNAL_TWO_PARAMETERS(std::vector<sf::String>, sf::String, m_stringList, m_string)

        void operator()(const std::shared_ptr<ChildWindow>& value)
        {
            m_childWindowPtr = value;
            (*this)();
            m_childWindowPtr = nullptr;
        }

        #undef SEND_SIGNAL_ONE_PARAMETER
        #undef SEND_SIGNAL_TWO_PARAMETERS

    protected:

        virtual void checkCompatibleParameterType(const std::string& type);

        #define CONNECT_INTERNAL_ONE_PARAMETER(ClassType, Type, Variable) \
            template <typename Func, typename... Args> \
            std::function<void()> connectInternal(ClassType##Parameter, Func func, Args... args) \
            { \
                checkCompatibleParameterType(#Type); \
                return std::bind(func, args..., std::cref(Variable)); \
            }

        #define CONNECT_INTERNAL_TWO_PARAMETERS(ClassType, Type1, Type2, Variable1, Variable2) \
            template <typename Func, typename... Args> \
            std::function<void()> connectInternal(ClassType##Parameter, Func func, Args... args) \
            { \
                checkCompatibleParameterType(#Type1 ", " #Type2); \
                return std::bind(func, args..., std::cref(Variable1), std::cref(Variable2)); \
            }

        // Parameters passed to connect function are wrong.
        // Most likely one has a slightly wrong type, but you may also be providing the wrong amount of arguments for the function.
        template <typename Func, typename... Args>
        std::function<void()> connectInternal(ErrorParameter, Func, Args...);

        template <typename Func, typename... Args>
        std::function<void()> connectInternal(NoneParameter, Func func, Args... args)
        {
            return std::bind(func, args...);
        }

        CONNECT_INTERNAL_ONE_PARAMETER(Int, int, m_int)
        CONNECT_INTERNAL_ONE_PARAMETER(Vector2f, sf::Vector2f, m_vector2f)
        CONNECT_INTERNAL_ONE_PARAMETER(String, sf::String, m_string)
        CONNECT_INTERNAL_ONE_PARAMETER(StringList, std::vector<sf::String>, m_stringList)
        CONNECT_INTERNAL_ONE_PARAMETER(ChildWindowPtr, std::shared_ptr<ChildWindow>, m_childWindowPtr)

        CONNECT_INTERNAL_TWO_PARAMETERS(DoubleString, sf::String, sf::String, m_string, m_string2)

        #undef CONNECT_INTERNAL_ONE_PARAMETER
        #undef CONNECT_INTERNAL_TWO_PARAMETERS

    private:

        std::map<unsigned int, std::function<void()>> m_functions;

        static int          m_int;
        static sf::Vector2f m_vector2f;
        static sf::String   m_string;
        static sf::String   m_string2;
        static std::vector<sf::String>      m_stringList;
        static std::shared_ptr<ChildWindow> m_childWindowPtr;

        friend class Widget;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    #define SIGNAL_DERIVED_ONE_TYPE(ClassType, Type) \
        class Signal##ClassType : public Signal \
        { \
            void checkCompatibleParameterType(const std::string& type) \
            { \
                if (type != Type) \
                    Signal::checkCompatibleParameterType(type); \
            } \
        };

    #define SIGNAL_DERIVED_TWO_TYPES(ClassType, Type1, Type2) \
        class Signal##ClassType : public Signal \
        { \
            void checkCompatibleParameterType(const std::string& type) \
            { \
                if ((type != Type1) && (type != Type2)) \
                    Signal::checkCompatibleParameterType(type); \
            } \
        };

    SIGNAL_DERIVED_ONE_TYPE(Bool, "int")
    SIGNAL_DERIVED_ONE_TYPE(Int, "int")
    SIGNAL_DERIVED_ONE_TYPE(Vector2f, "sf::Vector2f")
    SIGNAL_DERIVED_ONE_TYPE(String, "sf::String")
    SIGNAL_DERIVED_ONE_TYPE(ChildWindowPtr, "std::shared_ptr<ChildWindow>")

    SIGNAL_DERIVED_TWO_TYPES(MenuBar, "std::vector<sf::String>", "sf::String")
    SIGNAL_DERIVED_TWO_TYPES(DoubleString, "sf::String, sf::String", "sf::String")
    SIGNAL_DERIVED_TWO_TYPES(Container, "std::shared_ptr<Widget>, sf::String", "std::shared_ptr<Widget>")

    #undef SIGNAL_DERIVED_ONE_TYPE
    #undef SIGNAL_DERIVED_TWO_TYPES


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Base class for widgets to enable signal handling.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API SignalWidgetBase
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default constructor
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalWidgetBase() = default;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Copy constructor
        ///
        /// @param copy  Instance to copy
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalWidgetBase(const SignalWidgetBase& copy);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Overload of assignment operator
        ///
        /// @param right  Instance to assign
        ///
        /// @return Reference to itself
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalWidgetBase& operator=(const SignalWidgetBase& right);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler function to one or more signals
        ///
        /// @param signalName Name of the signal, or multiple names split by spaces
        /// @param func       The function to connect
        /// @param args       The arguments that should be bound to the function
        ///
        /// @return Id of this connection, which you need if you want to disconnect it later
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... Args>
        unsigned int connect(const std::string& signalName, Func func, Args... args)
        {
            for (auto& name : extractSignalNames(signalName))
                connectInternal(name, func, args...);

            return m_lastId;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Disconnects a connection
        ///
        /// @param id  The id that was returned by the connect function when this connection was made
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void disconnect(unsigned int id);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Disconnect all connections from a certain signal
        ///
        /// @param signalName  Name of the signal, or multiple names split by spaces,
        ///                    from which you want to disconnect all function handlers
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void disconnectAll(const std::string& signalName);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Disconnect all connections from a all signals
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void disconnectAll();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Handle a new connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... Args>
        void connectInternal(const std::string& signalName, Func func, Args... args)
        {
            if (m_signals.find(toLower(signalName)) == m_signals.end())
                throw Exception{"Cannot connect to unknown signal '" + signalName + "'."};

            try {
                m_signals[toLower(signalName)]->connect(++m_lastId, func, args...);
            }
            catch (const Exception& e) {
                throw Exception{e.what() + (" since it is not valid for the '" + signalName + "' signal.")};
            }
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Add a new signal that people can bind.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename SignalType = Signal>
        void addSignal(const std::string& name)
        {
            m_signals[toLower(name)] = std::make_shared<SignalType>();
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Check if some signal handler has been bound to the signal.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool isSignalBound(const std::string& name);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Send a signal to all signal handlers that are connected with this signal.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename... Args>
        void sendSignal(const std::string& name, Args... args)
        {
            auto& signal = *m_signals[toLower(name)];
            if (!signal.isEmpty())
                signal(args...);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        std::vector<std::string> extractSignalNames(std::string input);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        std::map<std::string, std::shared_ptr<Signal>> m_signals;

        static unsigned int m_lastId;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_SIGNAL_HPP
