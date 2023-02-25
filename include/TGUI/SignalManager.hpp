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


#ifndef TGUI_SIGNAL_MANAGER_HPP
#define TGUI_SIGNAL_MANAGER_HPP


#include <TGUI/Widget.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <unordered_map>
    #include <memory>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API SignalManager
    {
    public:

        using Ptr = std::shared_ptr<SignalManager>; //!< Shared widget pointer
        using ConstPtr = std::shared_ptr<const SignalManager>; //!< Shared constant widget pointer

        using Delegate = std::function<void()>;
        using DelegateEx = std::function<void(std::shared_ptr<Widget>, const String&)>;

        using Weak = std::weak_ptr<Widget>;
        using SignalID = unsigned int;

        struct SignalTuple
        {
            String widgetName;
            String signalName;
            std::pair<Delegate, DelegateEx> func;
        };

        struct ConnectedSignalTuple
        {
            SignalID signalId;
            Weak widget;
            unsigned int signalWidgetID;

            ConnectedSignalTuple(SignalID sigId, Weak widgetPtr, unsigned int widgetId) :
                signalId(sigId),
                widget(std::move(widgetPtr)),
                signalWidgetID(widgetId)
            {
            }
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SignalManager() = default;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~SignalManager() = default;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets Signal Manager that will be used to operate a signal
        ///
        /// @param manager Manager that will be used to operate ours signals connected by loadWidgetsFromFile
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void setDefaultSignalManager(const SignalManager::Ptr& manager);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Retrieves Signal Manager that will be used to operate a signal
        ///
        /// @return Signal Manager used to operate the signals
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD static SignalManager::Ptr getSignalManager();

#if defined(__cpp_if_constexpr) && (__cpp_if_constexpr >= 201606L)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param widgetName   Name of the widget to connect to
        /// @param signalName   Name of the signal
        /// @param handler      Callback function that is given the extra arguments provided to this function as arguments
        /// @param args         Optional extra arguments to pass to the signal handler when the signal is emitted
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs>
        unsigned int connect(String widgetName, String signalName, Func&& handler, const BoundArgs&... args);

#else
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param widgetName   Name of the widget to connect to
        /// @param signalName   Name of the signal
        /// @param handler      Callback function that is given the extra arguments provided to this function as arguments
        /// @param args         Optional extra arguments to pass to the signal handler when the signal is emitted
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... Args, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const Args&...)>>::value>* = nullptr>
        unsigned int connect(String widgetName, String signalName, Func&& handler, const Args&... args);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Connects a signal handler that will be called when this signal is emitted
        ///
        /// @param widgetName   Name of the widget to connect to
        /// @param signalName   Name of the signal
        /// @param handler      Callback function that is given a pointer to the widget, the name of the signal and the extra
        ///                     arguments provided to this function as arguments
        /// @param args         Optional extra arguments to pass to the signal handler when the signal is emitted
        ///
        /// @return Unique id of the connection
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename Func, typename... BoundArgs, typename std::enable_if_t<!std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value // Ambigious otherwise when passing bind expression
                                                                                && std::is_convertible<Func, std::function<void(const BoundArgs&..., std::shared_ptr<Widget>, const String&)>>::value>* = nullptr>
        unsigned int connect(String widgetName, String signalName, Func&& handler, BoundArgs&&... args);
#endif

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Disconnect a signal handler
        ///
        /// @param id  Unique id of the connection returned by the connect function
        ///
        /// @return True when a connection with this id existed and was removed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool disconnect(unsigned int id);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Disconnect all signal handler
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void disconnectAll();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void add(const Widget::Ptr& widgetPtr);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void remove(Widget* widget);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        unsigned int generateUniqueId();

        unsigned int m_lastId = 0;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD std::pair<Delegate, DelegateEx> makeSignal(const Delegate&);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD std::pair<Delegate, DelegateEx> makeSignalEx(const DelegateEx&);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Function that is called each time new signal is created with it id
        ///
        /// @param id Id of new signal
        ///
        /// @internal
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void connect(SignalID id);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        static SignalManager::Ptr m_manager;

        std::vector<Weak> m_widgets;
        std::unordered_map<SignalID, SignalTuple> m_signals;
        std::vector<ConnectedSignalTuple> m_connectedSignals;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(__cpp_if_constexpr) && (__cpp_if_constexpr >= 201606L)
    template <typename Func, typename... BoundArgs>
    unsigned int SignalManager::connect(String widgetName, String signalName, Func&& handler, const BoundArgs&... args)
    {
        const unsigned int id = generateUniqueId();

        if constexpr (std::is_convertible_v<Func, std::function<void(const BoundArgs&...)>>
                   && std::is_invocable_v<decltype(&handler), BoundArgs...>
                   && !std::is_function_v<Func>)
        {
            // Reference to function
            m_signals[id] = {widgetName, signalName, makeSignal([=, f=std::function<void(const BoundArgs&...)>(handler)]{ std::invoke(f, args...); })};
        }
        else if constexpr (std::is_convertible_v<Func, std::function<void(const BoundArgs&...)>>)
        {
            // Function pointer
            m_signals[id] = {widgetName, signalName, makeSignal([=]{ std::invoke(handler, args...); })};
        }
        else if constexpr (std::is_convertible_v<Func, std::function<void(const BoundArgs&..., const std::shared_ptr<Widget>&, const String&)>>
                        && std::is_invocable_v<decltype(&handler), BoundArgs..., const std::shared_ptr<Widget>&, const String&>
                        && !std::is_function_v<Func>)
        {
            // Reference to function with caller arguments
            m_signals[id] = {widgetName, signalName, makeSignalEx([=, f=std::function<void(const BoundArgs&..., const std::shared_ptr<Widget>& w, const String& s)>(handler)](const std::shared_ptr<Widget>& w, const String& s){ std::invoke(f, args..., w, s); })};
        }
        else
        {
            // Function pointer with caller arguments
            m_signals[id] = {widgetName, signalName, makeSignalEx([=](const std::shared_ptr<Widget>& w, const String& s){ std::invoke(handler, args..., w, s); })};
        }

        connect(id);
        return id;
    }
#else
    template <typename Func, typename... Args, typename std::enable_if_t<std::is_convertible<Func, std::function<void(const Args&...)>>::value>*>
    unsigned int SignalManager::connect(String widgetName, String signalName, Func&& handler, const Args&... args)
    {
        const unsigned int id = generateUniqueId();
        m_signals[id] = {widgetName, signalName, makeSignal([f=std::function<void(const Args&...)>(handler),args...](){ f(args...); })};

        connect(id);
        return id;
    }

    template <typename Func, typename... BoundArgs, typename std::enable_if_t<!std::is_convertible<Func, std::function<void(const BoundArgs&...)>>::value // Ambigious otherwise when passing bind expression
                                                                            && std::is_convertible<Func, std::function<void(const BoundArgs&..., std::shared_ptr<Widget>, const String&)>>::value>*>
    unsigned int SignalManager::connect(String widgetName, String signalName, Func&& handler, BoundArgs&&... args)
    {
        const unsigned int id = generateUniqueId();
        m_signals[id] = {widgetName, signalName, makeSignalEx(
                [f=std::function<void(const BoundArgs&..., const std::shared_ptr<Widget>&, const String&)>(handler), args...]
                        (const std::shared_ptr<Widget>& w, const String& s)
                { f(args..., w, s); }
        )};

        connect(id);
        return id;
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_SIGNAL_MANAGER_HPP
