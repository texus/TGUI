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


#ifndef TGUI_SIGNAL_IMPL_HPP
#define TGUI_SIGNAL_IMPL_HPP

#include <TGUI/Signal.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace internal_signal
    {
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
        decltype(auto) dereference(const void* obj)
        {
            // Signal handlers are allowed to have std::string parameters while the signal sends sf::String
            return static_cast<std::string>(*static_cast<const sf::String*>(obj));
        }

        template <typename Type, typename std::enable_if<std::is_same<Type, sf::Vector2f>::value>::type* = nullptr>
        decltype(auto) dereference(const void* obj)
        {
            // Signal handlers are allowed to have sf::Vector2f parameters while the signal sends tgui::Vector2f
            return static_cast<sf::Vector2f>(*static_cast<const Vector2f*>(obj));
        }

        template <typename Type, typename std::enable_if<!std::is_same<Type, std::string>::value && !std::is_same<Type, sf::Vector2f>::value>::type* = nullptr>
        decltype(auto) dereference(const void* obj)
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
            static decltype(auto) bind(Signal& signal, Func&& func, BoundArgs&&... args)
            {
                return bindImpl(std::index_sequence_for<UnboundArgs...>{}, signal, std::forward<Func>(func), std::forward<BoundArgs>(args)...);
            }

        private:

            template <typename Func, typename... BoundArgs, std::size_t... Indices>
            static decltype(auto) bindImpl(std::index_sequence<Indices...>, Signal& signal, Func&& func, BoundArgs&&... args)
            {
                const std::size_t offset = (sizeof...(UnboundArgs) > 0) ? signal.validateTypes({typeid(UnboundArgs)...}) : 0;
            #if defined TGUI_USE_CPP17
                return [=](const std::shared_ptr<Widget>& widget, const std::string& signalName) {
                    std::invoke(func, // An error "variable 'func' has function type" here means you passed a reference instead of a function pointer to 'connect'
                                args...,
                                widget,
                                signalName,
                                internal_signal::dereference<UnboundArgs>(internal_signal::parameters[offset + Indices])...);
            #else
                return [=,o=offset](const std::shared_ptr<Widget>& widget, const std::string& signalName) { // MinGW TDM GCC 5.1 won't compile code without "o=offset" hack
                    invokeFunc(func, // An error "variable 'func' has function type" here means you passed a reference instead of a function pointer to 'connect'
                               args...,
                               widget,
                               signalName,
                               internal_signal::dereference<UnboundArgs>(internal_signal::parameters[o + Indices])...);
            #endif
                };
            }
        };

        template <typename... UnboundArgs>
        struct binder<TypeSet<UnboundArgs...>, TypeSet<>>
        {
            template <typename Func, typename... BoundArgs>
            static decltype(auto) bind(Signal& signal, Func&& func, BoundArgs&&... args)
            {
                return bindImpl(std::index_sequence_for<UnboundArgs...>{}, signal, std::forward<Func>(func), std::forward<BoundArgs>(args)...);
            }

        private:

            template <typename Func, typename... BoundArgs, std::size_t... Indices>
            static decltype(auto) bindImpl(std::index_sequence<Indices...>, Signal& signal, Func&& func, BoundArgs&&... args)
            {
                const std::size_t offset = (sizeof...(UnboundArgs) > 0) ? signal.validateTypes({typeid(UnboundArgs)...}) : 0;
            #if defined TGUI_USE_CPP17
                return [=]{
                    std::invoke(func, // An error "variable 'func' has function type" here means you passed a reference instead of a function pointer to 'connect'
                               args...,
                               internal_signal::dereference<UnboundArgs>(internal_signal::parameters[offset + Indices])...);
            #else
                return [=,o=offset]{ // MinGW TDM GCC 5.1 won't compile code without "o=offset" hack
                    invokeFunc(func, // An error "variable 'func' has function type" here means you passed a reference instead of a function pointer to 'connect'
                               args...,
                               internal_signal::dereference<UnboundArgs>(internal_signal::parameters[o + Indices])...);
            #endif
                };
            }
        };


    #ifdef TGUI_USE_CPP17
        // Error case (function signature did not match anything)
        template <typename Enable, typename Func, typename... BoundArgs>
        struct func_traits;

        // Free function
        template <typename... Args, typename... BoundArgs>
        struct func_traits<void, void(*)(Args...), BoundArgs...> : binder<TypeSet<std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename... Args, typename... BoundArgs>
        struct func_traits<void, void(*)(Args...) noexcept, BoundArgs...> : binder<TypeSet<std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};

        // Non-const member function
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...), BoundArgs...> : binder<TypeSet<Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) noexcept, BoundArgs...> : binder<TypeSet<Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) volatile, BoundArgs...> : binder<TypeSet<Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) volatile noexcept, BoundArgs...> : binder<TypeSet<Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) &, BoundArgs...> : binder<TypeSet<Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) & noexcept, BoundArgs...> : binder<TypeSet<Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) volatile &, BoundArgs...> : binder<TypeSet<Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) volatile & noexcept, BoundArgs...> : binder<TypeSet<Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) &&, BoundArgs...> : binder<TypeSet<Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) && noexcept, BoundArgs...> : binder<TypeSet<Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) volatile &&, BoundArgs...> : binder<TypeSet<Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) volatile && noexcept, BoundArgs...> : binder<TypeSet<Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};

        // Const member function
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) const, BoundArgs...> : binder<TypeSet<const Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) const noexcept, BoundArgs...> : binder<TypeSet<const Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) volatile const, BoundArgs...> : binder<TypeSet<const Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) volatile const noexcept, BoundArgs...> : binder<TypeSet<const Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) const &, BoundArgs...> : binder<TypeSet<const Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) const & noexcept, BoundArgs...> : binder<TypeSet<const Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) volatile const &, BoundArgs...> : binder<TypeSet<const Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) volatile const & noexcept, BoundArgs...> : binder<TypeSet<const Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) const &&, BoundArgs...> : binder<TypeSet<const Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) const && noexcept, BoundArgs...> : binder<TypeSet<const Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) volatile const &&, BoundArgs...> : binder<TypeSet<const Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};
        template <typename Class, typename... Args, typename... BoundArgs>
        struct func_traits<void, void(Class::*)(Args...) volatile const && noexcept, BoundArgs...> : binder<TypeSet<const Class*, std::decay_t<Args>...>, TypeSet<BoundArgs...>> {};

        // std::function or lambda
        template <typename Func, typename... BoundArgs>
        struct func_traits<void_t<decltype(&Func::operator())>, Func, BoundArgs...> : public func_traits<void, decltype(&Func::operator()), Func*, BoundArgs...> {};
    #else
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
    #endif
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
        const unsigned int id = getSignal(toLower(signalName)).connect([f=std::function<void(const Args&...)>(handler),args...](){ f(args...); });
        m_connectedSignals[id] = toLower(signalName);
        return id;
    }

    template <typename Func, typename... BoundArgs, typename std::enable_if<std::is_convertible<Func, std::function<void(const BoundArgs&..., std::shared_ptr<Widget>, const std::string&)>>::value>::type*>
    unsigned int SignalWidgetBase::connect(std::string signalName, Func&& handler, BoundArgs&&... args)
    {
        const unsigned int id = getSignal(toLower(signalName)).connect(
                                    [f=std::function<void(const BoundArgs&..., const std::shared_ptr<Widget>&, const std::string&)>(handler), args...]
                                    (const std::shared_ptr<Widget>& w, const std::string& s)
                                    { f(args..., w, s); }
                                );

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
