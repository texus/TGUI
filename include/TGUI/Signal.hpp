/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
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


#include <TGUI/Global.hpp>
#include <TGUI/Callback.hpp>

#include <map>
#include <deque>
#include <memory>
#include <cassert>
#include <functional>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class Widget;
    class ChildWindow;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <typename... Types>
    struct TypeSet;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace priv
    {
        extern TGUI_API std::deque<const void*> data;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename T>
        std::string convertTypeToString();

        template <> inline std::string convertTypeToString<int>() { return "int"; }
        template <> inline std::string convertTypeToString<sf::Vector2f>() { return "sf::Vector2f"; }
        template <> inline std::string convertTypeToString<sf::String>() { return "sf::String"; }
        template <> inline std::string convertTypeToString<std::vector<sf::String>>() { return "std::vector<sf::String>"; }
        template <> inline std::string convertTypeToString<std::shared_ptr<ChildWindow>>() { return "std::shared_ptr<ChildWindow>"; }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename... Types>
        struct extractTypes
        {
            static std::vector<std::vector<std::string>> get() { return {}; }
            static std::vector<std::string> getRow() { return {}; }
        };

        template <typename Type>
        struct extractTypes<Type>
        {
            static std::vector<std::vector<std::string>> get() { return {{convertTypeToString<Type>()}}; }
            static std::vector<std::string> getRow() { return {convertTypeToString<Type>()}; }
        };

        template <typename Type, typename... OtherTypes>
        struct extractTypes<Type, OtherTypes...>
        {
            static std::vector<std::vector<std::string>> get()
            {
                auto types = extractTypes<OtherTypes...>::get();
                types.insert(types.begin(), {convertTypeToString<Type>()});
                return types;
            }

            static std::vector<std::string> getRow()
            {
                auto types = extractTypes<OtherTypes...>::getRow();
                types.insert(types.begin(), convertTypeToString<Type>());
                return types;
            }
        };

        template <typename... T>
        struct extractTypes<TypeSet<T...>>
        {
            static std::vector<std::vector<std::string>> get() { return {extractTypes<T...>::getRow()}; }
        };

        template <typename... T, typename... U>
        struct extractTypes<TypeSet<T...>, U...>
        {
            static std::vector<std::vector<std::string>> get()
            {
                auto types = {extractTypes<T...>::getRow()};
                types.insert(types.end(), extractTypes<U...>::get());
                return types;
            }
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename Func, typename TypeA, typename TypeB, typename... Args>
        struct isConvertible;

        template <typename Func, typename... TypesA, typename... TypesB, typename... Args>
        struct isConvertible<Func, TypeSet<TypesA...>, TypeSet<TypesB...>, Args...>
        {
            using type = typename std::conditional<std::is_convertible<Func, std::function<void(Args..., TypesA...)>>::value, TypeSet<TypesA...>, TypeSet<TypesB...>>::type;
        };

        template <typename Func, typename... Type, typename... Args>
        struct isConvertible<Func, TypeSet<>, TypeSet<Type...>, Args...>
        {
            using type = typename std::conditional<std::is_convertible<Func, std::function<void(Args...)>>::value || std::is_convertible<Func, std::function<void(Args...)>>::value, TypeSet<>, TypeSet<Type...>>::type;
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename Type>
        const Type& dereference(const void* obj)
        {
            return *static_cast<const Type*>(obj);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Ignore warning "C4800: 'const int': forcing value to bool 'true' or 'false' (performance warning)" in Visual Studio
#if defined SFML_SYSTEM_WINDOWS && defined _MSC_VER
    #pragma warning(push)
    #pragma warning(disable : 4800)
#endif

        template <typename... T>
        struct connector;

        template <typename Func, typename... Args>
        struct connector<TypeSet<>, Func, Args...>
        {
            static std::function<void()> connect(Func func, std::size_t, Args... args)
            {
                return std::bind(func, args...);
            }
        };

        template <typename Func, typename... Args, typename Type>
        struct connector<TypeSet<Type>, Func, Args...>
        {
            static std::function<void()> connect(Func func, std::size_t argPos, Args... args)
            {
                return std::bind(func, args..., std::bind(dereference<Type>, std::cref(data[argPos])));
            }
        };

        template <typename Func, typename... Args, typename TypeA, typename TypeB>
        struct connector<TypeSet<TypeA, TypeB>, Func, Args...>
        {
            static std::function<void()> connect(Func func, std::size_t argPos, Args... args)
            {
                return std::bind(func, args...,
                                 std::bind(dereference<TypeA>, std::cref(data[argPos])),
                                 std::bind(dereference<TypeB>, std::cref(data[argPos+1])));
            }
        };

#if defined SFML_SYSTEM_WINDOWS && defined _MSC_VER
    #pragma warning(pop)
#endif

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename Arg, typename = void>
        struct bindRemover
        {
            static Arg remove(Arg arg)
            {
                return arg;
            }
        };

        template <typename Arg>
        struct bindRemover<Arg, typename std::enable_if<std::is_bind_expression<Arg>::value>::type>
        {
            static auto remove(Arg arg) -> decltype(arg())
            {
                return arg();
            }
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <typename Func, typename... Args>
        struct isFunctionConvertible
        {
            using type = typename isConvertible<Func, TypeSet<>,
                            typename isConvertible<Func, TypeSet<int>,
                                typename isConvertible<Func, TypeSet<sf::Vector2f>,
                                    typename isConvertible<Func, TypeSet<sf::String>,
                                        typename isConvertible<Func, TypeSet<std::vector<sf::String>>,
                                            typename isConvertible<Func, TypeSet<std::shared_ptr<ChildWindow>>,
                                                typename isConvertible<Func, TypeSet<sf::String, sf::String>,
                                                    TypeSet<void>,
                                                    Args...>::type,
                                                Args...>::type,
                                            Args...>::type,
                                        Args...>::type,
                                    Args...>::type,
                                Args...>::type,
                            Args...>::type;
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @internal
    // Stores the actual callback functions and checks if their parameters are valid
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API Signal
    {
    public:

        Signal(std::vector<std::vector<std::string>>&& types);

        template <typename Func, typename... Args>
        void connect(unsigned int id, Func func, Args... args)
        {
            using type = typename priv::isFunctionConvertible<Func, decltype(priv::bindRemover<Args>::remove(args))...>::type;
            static_assert(!std::is_same<type, TypeSet<void>>::value, "Parameters passed to the connect function are wrong!");

            auto argPos = checkCompatibleParameterType<type>();
            m_functions[id] = priv::connector<type, Func, Args...>::connect(func, argPos, args...);
        }

        template <typename Func, typename... Args>
        void connectEx(unsigned int id, Func func, Args... args)
        {
            m_functionsEx[id] = std::bind(func, args..., std::placeholders::_1);
        }

        bool disconnect(unsigned int id);

        void disconnectAll();

        bool isEmpty() const;

        void operator()(unsigned int count);

        template <typename T, typename... Args>
        void operator()(unsigned int count, const T& value, Args... args)
        {
            priv::data[count] = static_cast<const void*>(&value);
            (*this)(count+1, args...);
        }

    protected:

        template <typename Type>
        std::size_t checkCompatibleParameterType()
        {
            if (std::is_same<Type, TypeSet<>>::value)
                return 0;

            auto acceptedType = priv::extractTypes<Type>::get();
            assert(acceptedType.size() == 1);

            std::size_t count = 0;
            for (std::size_t i = 0; i < m_allowedTypes.size(); ++i)
            {
                if (acceptedType[0] == m_allowedTypes[i])
                    return count;

                count += m_allowedTypes[i].size();
            }

            throw Exception{"Failed to bind parameter to callback function. Parameter is of wrong type."};
        }

    private:

        std::map<unsigned int, std::function<void()>> m_functions;
        std::map<unsigned int, std::function<void(const Callback&)>> m_functionsEx;

        std::vector<std::vector<std::string>> m_allowedTypes;

        friend class SignalWidgetBase; // Only needed for m_functionsEx
    };


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
        /// @param signalNames Name of the signal, or multiple names split by spaces
        /// @param func        The function to connect
        /// @param args        The arguments that should be bound to the function
        ///
        /// @return Id of this connection, which you need if you want to disconnect it later
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... Args>
        unsigned int connect(const std::string& signalNames, Func func, Args... args)
        {
            auto signalNameList = extractSignalNames(signalNames);
            if (signalNameList.empty())
                throw Exception{"connect function called with empty string"};

            for (auto& signalName : signalNameList)
            {
                if (m_signals.find(toLower(signalName)) != m_signals.end())
                {
                    try {
                        m_signals[toLower(signalName)]->connect(m_lastId, func, args...);
                        m_lastId++;
                    }
                    catch (const Exception& e) {
                        throw Exception{e.what() + (" The parameters are not valid for the '" + signalName + "' signal.")};
                    }
                }
                else
                {
                    if (toLower(signalName) != "all")
                        throw Exception{"Cannot connect to unknown signal '" + signalName + "'."};
                    else
                    {
                        assert(!m_signals.empty());

                        for (auto& signal : m_signals)
                        {
                            try {
                                signal.second->connect(m_lastId, func, args...);
                                m_lastId++;
                            }
                            catch (const Exception& e) {
                                throw Exception{e.what() + (" The parameters are not valid for the '" + signalName + "' signal.")};
                            }
                        }
                    }
                }
            }

            return m_lastId - 1;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler function to one or more signals
        ///
        /// @param signalName Name of the signal, or multiple names split by spaces
        /// @param func       The function to connect.
        ///                   This last parameter of the function must be of type "const tgui::Callback&".
        /// @param args       The arguments that should be bound to the function.
        ///                   The amount of arguments should be exactly one less than the function needs.
        ///
        /// @return Id of this connection, which you need if you want to disconnect it later
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... Args>
        unsigned int connectEx(const std::string& signalName, Func func, Args... args)
        {
            auto signalNameList = extractSignalNames(signalName);
            if (signalNameList.empty())
                throw Exception{"connect function called with empty string"};

            for (auto& name : signalNameList)
            {
                if (m_signals.find(toLower(name)) != m_signals.end())
                {
                    try {
                        m_signals[toLower(name)]->connectEx(m_lastId, func, args...);
                        m_lastId++;
                    }
                    catch (const Exception& e) {
                        throw Exception{e.what() + (" since it is not valid for the '" + name + "' signal.")};
                    }
                }
                else // Signal name does not exist
                {
                    if (toLower(name) != "all")
                        throw Exception{"Cannot connect to unknown signal '" + name + "'."};
                    else
                    {
                        assert(!m_signals.empty());

                        for (auto& signal : m_signals)
                        {
                            try {
                                signal.second->connectEx(m_lastId, func, args...);
                                m_lastId++;
                            }
                            catch (const Exception& e) {
                                throw Exception{e.what() + (" since it is not valid for the '" + name + "' signal.")};
                            }
                        }
                    }
                }
            }

            return m_lastId - 1;
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
        // Add a new signal that people can bind.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename... T>
        void addSignal(std::string&& name)
        {
            assert(m_signals[toLower(name)] == nullptr);
            m_signals[toLower(name)] = std::make_shared<Signal>(priv::extractTypes<T...>::get());
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Check if some signal handler has been bound to the signal.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool isSignalBound(std::string&& name);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Send a signal to all signal handlers that are connected with this signal.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename... Args>
        void sendSignal(std::string&& name, Args... args)
        {
            assert(m_signals[toLower(name)] != nullptr);
            auto& signal = *m_signals[toLower(name)];

            if (signal.m_functionsEx.empty())
            {
                if (!signal.isEmpty())
                    signal(0, args...);
            }
            else // Legacy functions are used
            {
                // Copy signal to avoid problems with lifetime when signal handler destroys this object
                Signal signalCopy = signal;

                m_callback.trigger = name;
                for (const auto& function : signalCopy.m_functionsEx)
                    function.second(m_callback);

                if (!signalCopy.isEmpty())
                    signalCopy(0, args...);
            }
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        std::vector<std::string> extractSignalNames(std::string input);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        std::map<std::string, std::shared_ptr<Signal>> m_signals;

        static unsigned int m_lastId;

        Callback m_callback;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_SIGNAL_HPP
