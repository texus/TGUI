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
#include <TGUI/Vector2f.hpp>
#include <SFML/System/String.hpp>
#include <functional>
#include <typeindex>
#include <cstddef>
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
        /// @internal
        /// @brief Checks whether the unbound parameters match with this signal
        /// @return The index in the parameters list where the parameters will be stored
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual unsigned int validateTypes(std::initializer_list<std::type_index> unboundParameters);


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
        unsigned int validateTypes(std::initializer_list<std::type_index> unboundParameters) override; \
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

        using DelegateRange = std::function<void(int, int)>;
        using DelegateRangeEx = std::function<void(std::shared_ptr<Widget>, const std::string&, int, int)>;

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
        bool emit(const Widget* widget, int value1, int value2);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Checks whether the unbound parameters match with this signal
        /// @return The index in the parameters list where the parameters will be stored
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int validateTypes(std::initializer_list<std::type_index> unboundParameters) override;


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
        unsigned int validateTypes(std::initializer_list<std::type_index> unboundParameters) override;


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
        unsigned int validateTypes(std::initializer_list<std::type_index> unboundParameters) override;


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
        unsigned int validateTypes(std::initializer_list<std::type_index> unboundParameters) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };


    namespace internal_signal
    {
        // make_integer_sequence implementation taken from https://gist.github.com/jappa/62f30b6da5adea60bad3
    #ifdef TGUI_NO_CPP14
        template <class Type, Type... Indices>
        struct integer_sequence
        {
            typedef Type value_type;
            static std::size_t size()
            {
                return sizeof...(Indices);
            }
        };

        template<std::size_t... Ints>
        using index_sequence = integer_sequence<std::size_t, Ints...>;

        namespace integer_sequence_detail
        {
            template <typename T, std::size_t ..._Extra>
            struct repeat;

            template <typename T, T ...N, std::size_t ..._Extra>
            struct repeat<integer_sequence<T, N...>, _Extra...>
            {
              typedef integer_sequence<T, N...,
                1 * sizeof...(N) + N...,
                2 * sizeof...(N) + N...,
                3 * sizeof...(N) + N...,
                4 * sizeof...(N) + N...,
                5 * sizeof...(N) + N...,
                6 * sizeof...(N) + N...,
                7 * sizeof...(N) + N...,
                _Extra...> type;
            };

            template <std::size_t N> struct parity;
            template <std::size_t N> struct make:parity<N % 8>::template pmake<N> {};

            template <> struct make<0> { typedef integer_sequence<std::size_t> type; };
            template <> struct make<1> { typedef integer_sequence<std::size_t, 0> type; };
            template <> struct make<2> { typedef integer_sequence<std::size_t, 0, 1> type; };
            template <> struct make<3> { typedef integer_sequence<std::size_t, 0, 1, 2> type; };
            template <> struct make<4> { typedef integer_sequence<std::size_t, 0, 1, 2, 3> type; };
            template <> struct make<5> { typedef integer_sequence<std::size_t, 0, 1, 2, 3, 4> type; };
            template <> struct make<6> { typedef integer_sequence<std::size_t, 0, 1, 2, 3, 4, 5> type; };
            template <> struct make<7> { typedef integer_sequence<std::size_t, 0, 1, 2, 3, 4, 5, 6> type; };

            template <> struct parity<0> { template <std::size_t N> struct pmake:repeat<typename make<N / 8>::type> {}; };
            template <> struct parity<1> { template <std::size_t N> struct pmake:repeat<typename make<N / 8>::type, N - 1> {}; };
            template <> struct parity<2> { template <std::size_t N> struct pmake:repeat<typename make<N / 8>::type, N - 2, N - 1> {}; };
            template <> struct parity<3> { template <std::size_t N> struct pmake:repeat<typename make<N / 8>::type, N - 3, N - 2, N - 1> {}; };
            template <> struct parity<4> { template <std::size_t N> struct pmake:repeat<typename make<N / 8>::type, N - 4, N - 3, N - 2, N - 1> {}; };
            template <> struct parity<5> { template <std::size_t N> struct pmake:repeat<typename make<N / 8>::type, N - 5, N - 4, N - 3, N - 2, N - 1> {}; };
            template <> struct parity<6> { template <std::size_t N> struct pmake:repeat<typename make<N / 8>::type, N - 6, N - 5, N - 4, N - 3, N - 2, N - 1> {}; };
            template <> struct parity<7> { template <std::size_t N> struct pmake:repeat<typename make<N / 8>::type, N - 7, N - 6, N - 5, N - 4, N - 3, N - 2, N - 1> {}; };

            template <typename T, typename U>
            struct convert
            {
              template <typename>
              struct result;

              template <T ...N>
              struct result<integer_sequence<T, N...> >
              {
                typedef integer_sequence<U, N...> type;
              };
            };

            template <typename T>
            struct convert<T, T>
            {
              template <typename U>
              struct result
              {
                typedef U type;
              };
            };

            template <typename T, T N>
            using make_integer_sequence_unchecked = typename convert<std::size_t, T>::template result<typename make<N>::type>::type;

            template <typename T, T N>
            struct make_integer_sequence
            {
              static_assert(std::is_integral<T>::value,
                "std::make_integer_sequence can only be instantiated with an integral type");
              static_assert(0 <= N,"std::make_integer_sequence input shall not be negative");

              typedef make_integer_sequence_unchecked<T, N> type;
            };
        }

        template <typename T, T N>
        using make_integer_sequence = typename integer_sequence_detail::make_integer_sequence<T, N>::type;

        template<std::size_t N>
        using make_index_sequence = make_integer_sequence<std::size_t, N>;

        template<class... T>
        using index_sequence_for = make_index_sequence<sizeof...(T)>;
    #else
        using std::index_sequence;
        using std::index_sequence_for;
    #endif

        // void_t only exists in c++17 so we use our own implementation to support c++14 compilers
        template<typename...>
        using void_t = void;

        // Type to pass a list of template types
        template <typename...>
        struct TypeSet;

        // Used for static_assert errors inside template functions
        template <typename...>
        struct always_false : std::false_type {};

        // The dereference function turns the void* elements in the parameters list back into its original type right before calling the signal handler
        template <typename Type, typename std::enable_if<std::is_same<Type, std::string>::value>::type* = nullptr>
    #ifdef TGUI_NO_CPP14
        std::string dereference(const void* obj)
    #else
        decltype(auto) dereference(const void* obj)
    #endif
        {
            // Signal handlers are allowed to have std::string parameters while the signal sends sf::String
            return static_cast<std::string>(*static_cast<const sf::String*>(obj));
        }

        template <typename Type, typename std::enable_if<std::is_same<Type, sf::Vector2f>::value>::type* = nullptr>
    #ifdef TGUI_NO_CPP14
        std::string dereference(const void* obj)
    #else
        decltype(auto) dereference(const void* obj)
    #endif
        {
            // Signal handlers are allowed to have sf::Vector2f parameters while the signal sends tgui::Vector2f
            return static_cast<sf::Vector2f>(*static_cast<const Vector2f*>(obj));
        }

        template <typename Type, typename std::enable_if<!std::is_same<Type, std::string>::value && !std::is_same<Type, sf::Vector2f>::value>::type* = nullptr>
    #ifdef TGUI_NO_CPP14
        const Type& dereference(const void* obj)
    #else
        decltype(auto) dereference(const void* obj)
    #endif
        {
            return *static_cast<const typename std::decay<Type>::type*>(obj);
        }

        // std::invoke only exists in c++17 so we use our own implementation to support c++14 compilers
        // Visual Studio compiler did not like it when the function was called "invoke"
        template <typename Func, typename... Args, typename std::enable_if<std::is_member_pointer<typename std::decay<Func>::type>::value>::type* = nullptr>
        void invokeFunc(Func&& func, Args&&... args)
        {
            std::mem_fn(func)(std::forward<Args>(args)...);
        }

        template <typename Func, typename... Args, typename std::enable_if<!std::is_member_pointer<typename std::decay<Func>::type>::value>::type* = nullptr>
        void invokeFunc(Func&& func, Args&&... args)
        {
            std::forward<Func>(func)(std::forward<Args>(args)...);
        }

        // The binder will figure out the unbound parameters and bind them if they correspond to what the signal sends
        template <typename... Args>
        struct binder;

        template <typename Arg, typename... AllArgs, typename BoundArg, typename... BoundArgs>
        struct binder<TypeSet<Arg, AllArgs...>, TypeSet<BoundArg, BoundArgs...>>
            : binder<TypeSet<AllArgs...>, TypeSet<BoundArgs...>>
        {
        };

        template <typename... UnboundArgs>
        struct binder<TypeSet<std::shared_ptr<Widget>, std::string, UnboundArgs...>, TypeSet<>>
        {
            template <typename Func, typename... BoundArgs>
        #ifdef TGUI_NO_CPP14
            static std::function<void(const std::shared_ptr<Widget>& widget, const std::string& signalName)> bind(Signal& signal, Func&& func, BoundArgs&&... args)
        #else
            static decltype(auto) bind(Signal& signal, Func&& func, BoundArgs&&... args)
        #endif
            {
                return bindImpl(index_sequence_for<UnboundArgs...>{}, signal, std::forward<Func>(func), std::forward<BoundArgs>(args)...);
            }

        private:

            template <typename Func, typename... BoundArgs, std::size_t... Indices>
        #ifdef TGUI_NO_CPP14
            static std::function<void(const std::shared_ptr<Widget>& widget, const std::string& signalName)> bindImpl(index_sequence<Indices...>, Signal& signal, Func&& func, BoundArgs&&... args)
        #else
            static decltype(auto) bindImpl(index_sequence<Indices...>, Signal& signal, Func&& func, BoundArgs&&... args)
        #endif
            {
                const std::size_t offset = (sizeof...(UnboundArgs) > 0) ? signal.validateTypes({typeid(UnboundArgs)...}) : 0;
            #ifdef TGUI_NO_CPP14
                return [=](const std::shared_ptr<Widget>& widget, const std::string& signalName) {
                    auto f = func;
            #else
                return [=, f=func](const std::shared_ptr<Widget>& widget, const std::string& signalName) {  // f=func is needed to decay free functions
            #endif
                    invokeFunc(f,
                               args...,
                               widget,
                               signalName,
                               internal_signal::dereference<UnboundArgs>(internal_signal::parameters[offset + Indices])...);
                };
            }
        };

        template <typename... UnboundArgs>
        struct binder<TypeSet<UnboundArgs...>, TypeSet<>>
        {
            template <typename Func, typename... BoundArgs>
        #ifdef TGUI_NO_CPP14
            static std::function<void()> bind(Signal& signal, Func&& func, BoundArgs&&... args)
        #else
            static decltype(auto) bind(Signal& signal, Func&& func, BoundArgs&&... args)
        #endif
            {
                return bindImpl(index_sequence_for<UnboundArgs...>{}, signal, std::forward<Func>(func), std::forward<BoundArgs>(args)...);
            }

        private:

            template <typename Func, typename... BoundArgs, std::size_t... Indices>
        #ifdef TGUI_NO_CPP14
            static std::function<void()> bindImpl(index_sequence<Indices...>, Signal& signal, Func&& func, BoundArgs&&... args)
        #else
            static decltype(auto) bindImpl(index_sequence<Indices...>, Signal& signal, Func&& func, BoundArgs&&... args)
        #endif
            {
                const std::size_t offset = (sizeof...(UnboundArgs) > 0) ? signal.validateTypes({typeid(UnboundArgs)...}) : 0;
            #ifdef TGUI_NO_CPP14
                return [=]() {
                    auto f = func;
            #else
                return [=, f=func]() {  // f=func is needed to decay free functions
            #endif
                    invokeFunc(f,
                               args...,
                               internal_signal::dereference<UnboundArgs>(internal_signal::parameters[offset + Indices])...);
                };
            }
        };


        // Error case (function signature did not match anything)
        template <typename Enable, typename Func, typename... BoundArgs>
        struct func_traits;

        // Normal function or lambda
        template <typename Func, typename... BoundArgs>
        struct func_traits<void_t<decltype(&Func::operator())>, Func, BoundArgs...>
            : public func_traits<void, decltype(&Func::operator()), Func*, BoundArgs...>
        {
        };

        // Non-const member function
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...), BoundArgs...>
            : binder<TypeSet<Class*, typename std::decay<Args>::type...>, TypeSet<BoundArgs...>>
        {
        };

        // Const member function
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) const, BoundArgs...>
            : binder<TypeSet<const Class*, typename std::decay<Args>::type...>, TypeSet<BoundArgs...>>
        {
        };

        // Free function
        template <typename... Args, typename... BoundArgs>
        struct func_traits<void, void(*)(Args...), BoundArgs...>
            : binder<TypeSet<typename std::decay<Args>::type...>, TypeSet<BoundArgs...>>
        {
        };
    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Base class for Widget which provides functionality to connect signals based on their name
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API SignalWidgetBase
    {
    public:

        virtual ~SignalWidgetBase() = default;


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
        unsigned int connect(std::string signalName, Func&& handler, const Args&... args)
        {
        #ifdef TGUI_NO_CPP14
            const unsigned int id = getSignal(toLower(signalName)).connect([=](){ (std::function<void(const Args&...)>(handler))(args...); });
        #else
            const unsigned int id = getSignal(toLower(signalName)).connect([f=std::function<void(const Args&...)>(handler),args...](){ f(args...); });
        #endif
            m_connectedSignals[id] = toLower(signalName);
            return id;
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
        template <typename Func, typename... BoundArgs, typename std::enable_if<std::is_convertible<Func, std::function<void(const BoundArgs&..., std::shared_ptr<Widget>, const std::string&)>>::value>::type* = nullptr>
        unsigned int connect(std::string signalName, Func&& handler, BoundArgs&&... args)
        {
        #ifdef TGUI_NO_CPP14
            const unsigned int id = getSignal(toLower(signalName)).connect(
                                        [=](const std::shared_ptr<Widget>& w, const std::string& s) {
                                            (std::function<void(const BoundArgs&..., const std::shared_ptr<Widget>&, const std::string&)>(handler))(args..., w, s);
                                         }
                                    );
        #else
            const unsigned int id = getSignal(toLower(signalName)).connect(
                                        [f=std::function<void(const BoundArgs&..., const std::shared_ptr<Widget>&, const std::string&)>(handler), args...]
                                        (const std::shared_ptr<Widget>& w, const std::string& s)
                                        { f(args..., w, s); }
                                    );
        #endif

            m_connectedSignals[id] = toLower(signalName);
            return id;
        }


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
        unsigned int connect(std::string signalName, Func&& handler, BoundArgs&&... args)
        {
            Signal& signal = getSignal(toLower(signalName));
            using binder = internal_signal::func_traits<void, typename std::decay<Func>::type, BoundArgs...>;
            const unsigned int id = signal.connect(binder::bind(signal, std::forward<Func>(handler), std::forward<BoundArgs>(args)...));
            m_connectedSignals[id] = toLower(signalName);
            return id;
        }


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
        unsigned int connect(std::initializer_list<std::string> signalNames, Func&& handler, BoundArgs&&... args)
        {
            unsigned int lastId = 0;
            for (auto& signalName : signalNames)
                lastId = connect(std::move(signalName), handler, args...);

            return lastId;
        }


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
