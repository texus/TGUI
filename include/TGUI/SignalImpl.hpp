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


#ifndef TGUI_SIGNAL_IMPL_HPP
#define TGUI_SIGNAL_IMPL_HPP

#include <TGUI/Signal.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
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
              static_assert(std::is_integral<T>::value, "std::make_integer_sequence can only be instantiated with an integral type");
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

    #ifdef TGUI_USE_CPP17
        using std::void_t;
    #else
        // void_t only exists in c++17 so we use our own implementation to support c++14 compilers
        template<typename...>
        using void_t = void;
    #endif

        // Type to pass a list of template types
        template <typename...>
        struct TypeSet;

        // The dereference function turns the void* elements in the parameters list back into its original type right before calling the signal handler
#ifdef TGUI_USE_CPP17
        template <typename Type>
        decltype(auto) dereference(const void* obj)
        {
            if constexpr (std::is_same_v<Type, std::string>) // Signal handlers are allowed to have std::string parameters while the signal sends sf::String
                return static_cast<std::string>(*static_cast<const sf::String*>(obj));
            else if constexpr (std::is_same_v<Type, sf::Vector2f>) // Signal handlers are allowed to have sf::Vector2f parameters while the signal sends tgui::Vector2f
                return static_cast<sf::Vector2f>(*static_cast<const Vector2f*>(obj));
            else
                return *static_cast<const std::decay_t<Type>*>(obj);
        }
#else
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
        sf::Vector2f dereference(const void* obj)
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
#endif

    #ifndef TGUI_USE_CPP17
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
    #endif

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
                    invokeFunc(f,
            #elif defined TGUI_USE_CPP17
                return [=](const std::shared_ptr<Widget>& widget, const std::string& signalName) {
                    std::invoke(func,
            #else
                return [=, f=func](const std::shared_ptr<Widget>& widget, const std::string& signalName) { // f=func is needed to decay free functions
                    invokeFunc(f,
            #endif
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
                    invokeFunc(f,
            #elif defined TGUI_USE_CPP17
                return [=]() {
                    std::invoke(func,
            #else
                return [=, f=func]() {  // f=func is needed to decay free functions
                    invokeFunc(f,
            #endif
                               args...,
                               internal_signal::dereference<UnboundArgs>(internal_signal::parameters[offset + Indices])...);
                };
            }
        };


        // Error case (function signature did not match anything)
        template <typename Enable, typename Func, typename... BoundArgs>
        struct func_traits;

        // std::function or lambda
        template <typename Func, typename... BoundArgs>
        struct func_traits<void_t<decltype(&Func::operator())>, Func, BoundArgs...>
            : public func_traits<void, decltype(&Func::operator()), Func*, BoundArgs...>
        {
        };

        // Non-const member function
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...), BoundArgs...>
        #ifdef TGUI_USE_CPP17
            : binder<TypeSet<Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>>
        #else
            : binder<TypeSet<Class*, typename std::decay<Args>::type...>, TypeSet<BoundArgs...>>
        #endif
        {
        };

        // Const member function
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) const, BoundArgs...>
        #ifdef TGUI_USE_CPP17
            : binder<TypeSet<const Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>>
        #else
            : binder<TypeSet<const Class*, typename std::decay<Args>::type...>, TypeSet<BoundArgs...>>
        #endif
        {
        };

        // Free function
        template <typename... Args, typename... BoundArgs>
        struct func_traits<void, void(*)(Args...), BoundArgs...>
        #ifdef TGUI_USE_CPP17
            : binder<TypeSet<std::decay_t<Args>...>, TypeSet<BoundArgs...>>
        #else
            : binder<TypeSet<typename std::decay<Args>::type...>, TypeSet<BoundArgs...>>
        #endif
        {
        };
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TGUI_USE_CPP17
    template <typename Func, typename... BoundArgs>
    unsigned int SignalWidgetBase::connect(std::string signalName, Func&& handler, const BoundArgs&... args)
    {
        unsigned int id;
        Signal& signal = getSignal(toLower(signalName));

        if constexpr (std::is_convertible_v<Func, std::function<void(const BoundArgs&...)>>
                   && std::is_invocable_v<decltype(&handler), BoundArgs...>
                   && !std::is_function_v<Func>)
        {
            // Reference to function, all parameters bound
            id = signal.connect([=, f=std::function<void(const BoundArgs&...)>(handler)]{ std::invoke(f, args...); });
        }
        else if constexpr (std::is_convertible_v<Func, std::function<void(const BoundArgs&...)>>)
        {
            // Function with all parameters bound
            id = signal.connect([=]{ std::invoke(handler, args...); });
        }
        else if constexpr (std::is_convertible_v<Func, std::function<void(const BoundArgs&..., const std::shared_ptr<Widget>&, const std::string&)>>
                        && std::is_invocable_v<decltype(&handler), BoundArgs..., const std::shared_ptr<Widget>&, const std::string&>
                        && !std::is_function_v<Func>)
        {
            // Reference to function with caller arguments, all parameters bound
            id = signal.connect([=, f=std::function<void(const BoundArgs&..., const std::shared_ptr<Widget>& w, const std::string& s)>(handler)](const std::shared_ptr<Widget>& w, const std::string& s){ std::invoke(f, args..., w, s); });
        }
        else if constexpr (std::is_convertible_v<Func, std::function<void(const BoundArgs&..., const std::shared_ptr<Widget>&, const std::string&)>>)
        {
            // Function with caller arguments, all parameters bound
            id = signal.connect([=](const std::shared_ptr<Widget>& w, const std::string& s){ std::invoke(handler, args..., w, s); });
        }
        else
        {
            // Function with unbound arguments
            using binder = internal_signal::func_traits<void, std::decay_t<Func>, BoundArgs...>;
            id = signal.connect(binder::bind(signal, std::forward<Func>(handler), args...));
        }

        m_connectedSignals[id] = toLower(signalName);
        return id;
    }

#else
    template <typename Func, typename... Args, typename std::enable_if<std::is_convertible<Func, std::function<void(const Args&...)>>::value>::type*>
    unsigned int SignalWidgetBase::connect(std::string signalName, Func&& handler, const Args&... args)
    {
    #ifdef TGUI_NO_CPP14
        const unsigned int id = getSignal(toLower(signalName)).connect([=](){ (std::function<void(const Args&...)>(handler))(args...); });
    #else
        const unsigned int id = getSignal(toLower(signalName)).connect([f=std::function<void(const Args&...)>(handler),args...](){ f(args...); });
    #endif
        m_connectedSignals[id] = toLower(signalName);
        return id;
    }

    template <typename Func, typename... BoundArgs, typename std::enable_if<std::is_convertible<Func, std::function<void(const BoundArgs&..., std::shared_ptr<Widget>, const std::string&)>>::value>::type*>
    unsigned int SignalWidgetBase::connect(std::string signalName, Func&& handler, BoundArgs&&... args)
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

    template <typename Func, typename... BoundArgs, typename std::enable_if<!std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value
                                                                         && !std::is_convertible<Func, std::function<void(const BoundArgs&..., std::shared_ptr<Widget>, const std::string&)>>::value>::type*>
    unsigned int SignalWidgetBase::connect(std::string signalName, Func&& handler, BoundArgs&&... args)
    {
        Signal& signal = getSignal(toLower(signalName));
        using binder = internal_signal::func_traits<void, typename std::decay<Func>::type, BoundArgs...>;
        const unsigned int id = signal.connect(binder::bind(signal, std::forward<Func>(handler), std::forward<BoundArgs>(args)...));
        m_connectedSignals[id] = toLower(signalName);
        return id;
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <typename Func, typename... BoundArgs>
    unsigned int SignalWidgetBase::connect(std::initializer_list<std::string> signalNames, Func&& handler, BoundArgs&&... args)
    {
        unsigned int lastId = 0;
        for (auto& signalName : signalNames)
            lastId = connect(std::move(signalName), handler, args...);

        return lastId;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_SIGNAL_IMPL_HPP
