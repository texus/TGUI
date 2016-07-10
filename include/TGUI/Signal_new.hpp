/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <initializer_list>
#include <functional>
#include <typeindex>
#include <algorithm>
#include <vector>
#include <deque>
#include <map>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class SignalWidgetBase;

    namespace Signal
    {
        extern TGUI_API std::deque<const void*> data;

        extern TGUI_API SignalWidgetBase* m_currentWidget;
        extern TGUI_API std::size_t m_currentId;
        extern TGUI_API std::string m_currentSignalName;

        template <typename... Types>
        struct TypeList;

        template <typename Arg, typename = void>
        auto bindRemover(Arg&& arg) -> Arg;

        template <typename Arg, typename std::enable_if<std::is_bind_expression<Arg>::value>::type>
        auto bindRemover(Arg&& arg) -> decltype(arg());

        template <typename... Args>
        std::size_t findBoundParametersLocation();

        template <typename Type>
        const Type& dereference(const void* obj)
        {
            return *static_cast<const Type*>(obj);
        }

        template <typename... BoundArgs>
        struct Binder;

        template <>
        struct Binder<TypeList<>, TypeList<>>
        {
            template <typename Func, typename... Args>
            static auto bind(Func&& func, Args&&... args)
            {
                return std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
            }
        };

        template <typename... UnboundArgs>
        struct Binder<TypeList<UnboundArgs...>, TypeList<>>
        {
            template <typename Func, typename... Args>
            static auto bind(Func&& func, Args&&... args)
            {
                std::size_t argPos = findBoundParametersLocation<typename std::remove_reference<UnboundArgs>::type...>();

                if (argPos + sizeof...(UnboundArgs) > data.size())
                    data.resize(argPos + sizeof...(UnboundArgs), nullptr);

                return binder(std::make_index_sequence<sizeof...(UnboundArgs)>{}, argPos, std::forward<Func>(func), std::forward<Args>(args)...);
            }

        private:

            template <typename Func, typename... Args, std::size_t... Indices>
            static auto binder(std::index_sequence<Indices...>, std::size_t argPos, Func&& func, Args&&... args)
            {
                return std::bind(func, args..., std::bind(dereference<UnboundArgs>, std::cref(data[argPos+Indices]))...);
            }
        };

        template <typename Arg, typename... MoreArgs, typename BoundArg, typename... MoreBoundArgs>
        struct Binder<TypeList<Arg, MoreArgs...>, TypeList<BoundArg, MoreBoundArgs...>>
        {
            template <typename Func, typename... Args>
            static auto bind(Func&& func, Args&&... args)
            {
                return Binder<TypeList<MoreArgs...>, TypeList<MoreBoundArgs...>>::bind(std::forward<Func>(func), std::forward<Args>(args)...);
            }
        };

        template <typename Func, typename... AllArgs, typename... BoundArgs>
        static auto bindArguments(Func&& func, BoundArgs&&... args)
        {
            static_assert(sizeof...(BoundArgs) <= sizeof...(AllArgs), "More arguments were passed to the connect function than there are parameters in the callback function");

            return Binder<TypeList<AllArgs...>, TypeList<BoundArgs...>>::bind(std::forward<Func>(func), std::forward<BoundArgs>(args)...);
        }

        // Const call operator
        template <typename Class, typename... Args, typename... BoundArgs>
        static auto bindCallOperator(void(Class::*)(Args...) const, Class&& func, BoundArgs&&... args)
        {
            return bindArguments<Class, Args...>(std::forward<Class>(func), std::forward<BoundArgs>(args)...);
        }

        // Non-const call operator
        template <typename Class, typename... Args, typename... BoundArgs>
        static auto bindCallOperator(void(Class::*)(Args...), Class&& func, BoundArgs&&... args)
        {
            return bindArguments<Class, Args...>(std::forward<Class>(func), std::forward<BoundArgs>(args)...);
        }

        // Generic lambdas and other functors with templated call operators
        template <typename Class, typename = void>
        struct ArgExtractor
        {
            template <typename... BoundArgs>
            static auto bind(Class&& func, BoundArgs&&... args)
            {
                // One-liner did not compile on gcc 6.1.1
                // Use of "template" in this context does not compile on VS2015
                auto callOperator = &Class::template operator()<BoundArgs...>;
                return bindCallOperator(callOperator, std::forward<Class>(func), std::forward<BoundArgs>(args)...);
            }
        };

        // Lambda functions, functors and std::function
        template <typename Class>
        struct ArgExtractor<Class, typename std::enable_if<!std::is_void<decltype(&Class::operator())>::value>::type>
        {
            template <typename... BoundArgs>
            static auto bind(Class&& func, BoundArgs&&... args)
            {
                return bindCallOperator(&Class::operator(), std::forward<Class>(func), std::forward<BoundArgs>(args)...);
            }
        };

        // Free function (pointer)
        template <typename... Args>
        struct ArgExtractor<void(*)(Args...)>
        {
            template <typename Func, typename... BoundArgs>
            static auto bind(Func&& func, BoundArgs&&... args)
            {
                return bindArguments<Func, Args...>(std::forward<Func>(func), std::forward<BoundArgs>(args)...);
            }
        };

        // Free function (directly passed)
        template <typename... Args>
        struct ArgExtractor<void(&)(Args...)> : public ArgExtractor<void(*)(Args...)> {};

        // Const class member function
        template <typename Class, typename... Args>
        struct ArgExtractor<void(Class::*)(Args...) const>
        {
            template <typename Func, typename... BoundArgs>
            static auto bind(Func&& func, BoundArgs&&... args)
            {
                return bindArguments<Func, Class*, Args...>(std::forward<Func>(func), std::forward<BoundArgs>(args)...);
            }
        };

        // Non-const class member function
        template <typename Class, typename... Args>
        struct ArgExtractor<void(Class::*)(Args...)> : public ArgExtractor<void(Class::*)(Args...) const> {};

        template <typename Func, typename... BoundArgs>
        auto connect(std::true_type, SignalWidgetBase*, std::size_t, const std::string&, Func&& func, BoundArgs&&... args)
        {
            return std::bind(std::forward<Func>(func), std::forward<BoundArgs>(args)...);
        }

        template <typename Func, typename... BoundArgs>
        auto connect(std::false_type, SignalWidgetBase* widget, std::size_t id, const std::string& signalName, Func&& func, BoundArgs&&... args)
        {
            m_currentWidget = widget;
            m_currentSignalName = signalName;
            m_currentId = id;
            return ArgExtractor<Func>::bind(std::forward<Func>(func), std::forward<BoundArgs>(args)...);
        }
    } // namespace Signal


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Base class for widgets to enable signal handling
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API SignalWidgetBase
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Virtual destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~SignalWidgetBase() = default;


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
        template <typename Func, typename... BoundArgs>
        auto connect(const std::string& signalNames, Func&& func, BoundArgs&&... args)
        {
            auto signalNameList = extractSignalNames(signalNames);
            if (signalNameList.empty())
                throw Exception{"connect function called with empty string"};

            for (auto& name : signalNameList)
            {
                m_signals[name][m_lastId] = Signal::connect(std::integral_constant<bool, std::is_convertible<Func, std::function<void(decltype(Signal::bindRemover(args))...)>>::value>{},
                                                            this, m_lastId, name, std::forward<Func>(func), std::forward<BoundArgs>(args)...);
                m_lastId++;
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
        /// @brief Disconnects all connections from a certain signal
        ///
        /// @param signalNames Name of the signal, or multiple names split by spaces,
        ///                    from which you want to disconnect all function handlers
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void disconnectAll(const std::string& signalNames);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Disconnects all connections from a all signals
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void disconnectAll();


        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Find at which location the parameters are to be bound
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual std::size_t findBoundParametersLocation(const std::string& signal, const std::initializer_list<std::type_index>& unboundParameters);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Checks if some signal handler has been bound to the signal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool isSignalBound(std::string&& name);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Check if the unbound parameters match a certain type list
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool matchTypes(std::initializer_list<std::type_index> unboundParameters, std::initializer_list<std::type_index> typeList)
        {
            return std::equal(unboundParameters.begin(), unboundParameters.end(), typeList.begin(), typeList.end());
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sends a signal to all signal handlers that are connected with this signal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename... Args>
        void sendSignal(std::string&& name, Args&&... args)
        {
            if (!m_signals[toLower(name)].empty())
            {
                // m_data has to be large enough to hold all the data
                if (sizeof...(Args) > Signal::data.size())
                    Signal::data.resize(sizeof...(Args), nullptr);

                // Convert the data to void* and store them in m_data
                // One-liner did not compile with clang 3.8
                auto&& argList = std::initializer_list<const void*>{static_cast<const void*>(&args)...};
                std::copy_n(argList.begin(), sizeof...(Args), Signal::data.begin());

                // Execute all connected signal handlers
                for (auto& function : m_signals[toLower(name)])
                    function.second();
            }
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Extract the potentially multiple signal names from the parameter passed to the connect function
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<std::string> extractSignalNames(std::string input);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        std::map<std::string, std::map<std::size_t, std::function<void()>>> m_signals;
        static unsigned int m_lastId;
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <typename... Args>
    std::size_t Signal::findBoundParametersLocation()
    {
        return m_currentWidget->findBoundParametersLocation(m_currentSignalName, {typeid(Args)...});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*
    std::size_t Widget::findBoundParametersLocation(const std::string& signal, const std::initializer_list<std::type_index>& unboundParameters)
    {
        if ((signal == "positionchanged") || (signal == "sizechanged"))
        {
            if (matchTypes(unboundParameters, {typeid(sf::Vector2f)}))
                return 0;
        }
        else
            return SignalWidgetBase::findBoundParametersLocation(signal, unboundParameters);

        throw Exception("TGUI Error: Wrong parameters passed to connect function for signal '" + signal + "'");
    }
    */

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_SIGNAL_HPP
