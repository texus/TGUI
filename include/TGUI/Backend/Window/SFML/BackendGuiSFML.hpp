/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2025 Bruno Van de Velde (vdv_b@tgui.eu)
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

#ifndef TGUI_BACKEND_GUI_SFML_HPP
#define TGUI_BACKEND_GUI_SFML_HPP

#include <TGUI/Config.hpp>
#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/Backend/Window/BackendGui.hpp>
#endif

#include <SFML/Window.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API BackendGuiSFML : public BackendGui
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default constructor
        ///
        /// @warning setGuiWindow has to be called by the subclass that inherits from this base class before the gui is used
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        BackendGuiSFML();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ~BackendGuiSFML();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Passes the event to the widgets
        ///
        /// @param event  The event that was polled from the window
        ///
        /// @return Has the event been consumed?
        ///         When this function returns false, then the event was ignored by all widgets.
        ///
        /// You should call this function in your event loop.
        ///
        /// SFML 2:
        /// @code
        /// sf::Event event;
        /// while (window.pollEvent(event) {
        ///     gui.handleEvent(event);
        /// }
        /// @endcode
        ///
        /// SFML 3:
        /// @code
        /// while (const std::optional event = window.pollEvent()) {
        ///     gui.handleEvent(*event);
        /// }
        /// @endcode
        ///
        /// If you use SFML 3 and want to use window.handleEvents instead of window.pollEvent or window.waitEvent then
        /// check out the handleWindowEvents function instead.
        ///
        /// @see handleWindowEvents
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool handleEvent(sf::Event event);
        using BackendGui::handleEvent;

#if SFML_VERSION_MAJOR >= 3
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Handle all pending window events at once using callbacks, as alternative to polling events
        ///
        /// Using SFML's window.handleEvents directly is not practical in combination with TGUI because the gui needs access to
        /// almost all events. You would thus need to have a handler for all event types and call gui.handleEvent in each one.
        /// This handleWindowEvents function can be used as alternative to window.handleEvents to do it all for you.
        ///
        /// Call this function with any amount of parameters, each a callable that takes an event of a certain type as parameter.
        /// For each pending event in the window, the callable that takes that event as parameter is called and the handleEvent
        /// function is executed (unless the callable returned false).
        ///
        /// The callables must always take an SFML event as first parameter, but there are 3 variations of allowed handlers:
        /// 1) A void function that only takes the event as parameter will be called after gui.handleEvent is executed.
        /// 2) A void function with the event as first parameter and a bool as second parameter will also be called after
        ///    the gui.handleEvent function finished, but the bool argument will contain the return value of handleEvent.
        /// 3) A function that returns a bool and takes the event as parameter will be called before gui.handleEvent is executed.
        ///    When the handler returns false, the call to handleEvent will be skipped and the gui thus ignores the event.
        ///
        /// This function is not blocking: if there's no pending event then it will return without calling any of the handlers.
        ///
        /// Usage:
        /// @code
        /// gui.handleWindowEvents(
        ///     // A function that returns nothing will be called after the gui has handled the event
        ///     [](const sf::Event::Closed&) { },
        ///
        ///     // The function can take an optional extra parameter that indicates whether the gui processed the event.
        ///     // The value of the boolean is what gets returned by the handleEvent function that is called internally.
        ///     [](sf::Event::TextEntered&, bool /*consumedByGui*/) { },
        ///
        ///     // If you don't want the gui to process some events, you can let the function return a bool.
        ///     // In this case, the function will be called earlier and the return value determines if TGUI handles the event or ignores it.
        ///     // The handleEvent function will only be called when the function returns true.
        ///     [](sf::Event::MouseMoved) { return false; },
        ///
        ///     // Generic lambdas are also supported. Note that multiple matching functions can be executed for the same event,
        ///     // so this generic lambda is still called for Closed events even though a lambda above is also executed for Closed events.
        ///     [](auto&& event) {
        ///         if constexpr (std::is_same_v<std::decay_t<decltype(event)>, sf::Event::MouseButtonReleased>)
        ///             return false;
        ///         else
        ///             return true;
        ///     }
        /// );
        /// @endcode
        ///
        /// @warning Multiple handlers may be called for the same event, because every handler with the right parameter type gets called.
        ///
        /// @see handleEvent
        ///
        /// @since TGUI 1.8
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename... Ts>
        void handleWindowEvents(Ts&&... handlers)
        {
            if (!m_window)
                return;

            m_window->handleEvents([this,&handlers...](auto&& event) {
                using EventType = decltype(event);

                // We can't properly detect whether the handlers are valid (e.g. when using an invalid event type),
                // but we can do some tests that will give errors on handlers that are wrong but almost correct.
                static_assert(!std::disjunction_v<std::is_invocable_r<bool, Ts, EventType, bool>...>, "Handler for handleWindowEvents can't have both an extra bool argument and a bool return type");

                // If any of the handlers return a bool then they will be executed before the event is passed to the gui.
                // The returned boolean then decides whether the gui should still process the event or not.
                // Only one of the handlers should match at most, but letting the compiler figure out which handler to
                // call by creating an overload set is tricky because only handlers with a bool returns should be called here.
                auto callIfMatchesAndReturnsBool = [&event](auto&& handler)
                {
                    // std::is_invocable_r_v would still return true if the type is convertable to bool, so we use std::invoke_result_t to test the return type
                    using FuncType = decltype(handler);
                    if constexpr (std::is_invocable_v<FuncType, EventType>)
                    {
                        if constexpr (std::is_same_v<std::invoke_result_t<FuncType, EventType>, bool>)
                            return std::invoke(std::forward<FuncType>(handler), std::forward<EventType>(event));
                        else
                        {
                            static_assert(std::is_same_v<std::invoke_result_t<FuncType, EventType>, void>, "Handler for handleWindowEvents must have either 'void' or 'bool' return type");
                            return true;
                        }
                    }
                    else
                        return true;
                };
                const bool passEventToGui = (callIfMatchesAndReturnsBool(std::forward<Ts>(handlers)) && ...);

                // Let the gui handle the event
                bool eventHandledByGui = false;
                if (passEventToGui)
                    eventHandledByGui = handleEvent(std::forward<EventType>(event));

                // After the gui has handled the events, we call the handlers that return nothing.
                // These handlers can have an optional bool parameter that indicates whether the event was processed by the gui.
                auto callIfMatchesAndReturnsVoid = [&event](auto&& handler, auto&&... extraArgs)
                {
                    using FuncType = decltype(handler);
                    if constexpr (std::is_invocable_v<FuncType, EventType, decltype(extraArgs)...>)
                    {
                        if constexpr (std::is_same_v<std::invoke_result_t<FuncType, EventType, decltype(extraArgs)...>, void>)
                            std::invoke(std::forward<FuncType>(handler), std::forward<EventType>(event), std::forward<decltype(extraArgs)>(extraArgs)...);
                    }
                };
                (callIfMatchesAndReturnsVoid(std::forward<Ts>(handlers)), ...);
                (callIfMatchesAndReturnsVoid(std::forward<Ts>(handlers), eventHandledByGui), ...);
            });
        }
#endif

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Give the gui control over the main loop
        ///
        /// @param clearColor  background color of the window
        ///
        /// This function is only intended in cases where your program only needs to respond to gui events.
        /// For multimedia applications, games, or other programs where you want a high framerate or do a lot of processing
        /// in the main loop, you should use your own main loop.
        ///
        /// You can consider this function to execute something similar (but not identical) to the code below.
        ///
        /// SFML 2:
        /// @code
        /// while (window.isOpen())
        /// {
        ///     while (window.pollEvent(event))
        ///     {
        ///         gui.handleEvent(event);
        ///         if (event.type == sf::Event::Closed)
        ///             window.close();
        ///     }
        ///
        ///     window.clear(clearColor);
        ///     gui.draw();
        ///     window.display();
        /// }
        /// @endcode
        ///
        /// SFML 3:
        /// @code
        /// while (window.isOpen())
        /// {
        ///     while (const std::optional event = window.pollEvent())
        ///     {
        ///         gui.handleEvent(*event);
        ///         if (event->is<sf::Event::Closed>())
        ///             window.close();
        ///     }
        ///
        ///     window.clear(clearColor);
        ///     gui.draw();
        ///     window.display();
        /// }
        /// @endcode
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void mainLoop(Color clearColor = {240, 240, 240}) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Helper function that converts an SFML event to a TGUI event
        /// @param eventSFML  The input SFML event
        /// @param eventTGUI  The output TGUI event
        /// @return Did the SFML event match on a TGUI event and has the output event been written to?
        ///
        /// You typically don't need this function as you can call handleEvent directly with an sf::Event.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD bool convertEvent(const sf::Event& eventSFML, Event& eventTGUI);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the window that was provided to the gui
        ///
        /// @return Window that was set via the constructor, setWindow or setTarget
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        sf::Window* getWindow() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief This function is called by TGUI when focusing a text field (EditBox or TextArea).
        ///        It will result in the software keyboard being opened on Android and iOS.
        ///
        /// @param inputRect  The rectangle where text is being inputted
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void startTextInput(FloatRect inputRect) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief This function is called by TGUI when unfocusing a text field (EditBox or TextArea).
        ///        It will result in the software keyboard being closed on Android and iOS.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void stopTextInput() override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief This function is called by TGUI when the position of the caret changes in a text field (EditBox or TextArea).
        ///        If an IME is used then this function may move the IME candidate list to the text cursor position.
        ///
        /// @param inputRect The rectangle where text is being inputted
        /// @param caretPos  Location of the text cursor, relative to the gui view
        ///
        /// This function currently only has effect on Windows.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateTextCursorPosition(FloatRect inputRect, Vector2f caretPos) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Checks the state for one of the modifier keys
        ///
        /// @param modifierKey  The modifier key of which the state is being queried
        ///
        /// @return Whether queries modifier key is being pressed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD bool isKeyboardModifierPressed(Event::KeyModifier modifierKey) const override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the window which the gui should use
        ///
        /// @param window  The SFML window that will be used by the gui
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setGuiWindow(sf::Window& window);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Updates the view and changes the size of the root container when needed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateContainerSize() override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        sf::Window* m_window = nullptr;

        bool m_modifierKeySystemPressed = false;
        bool m_modifierKeyControlPressed = false;
        bool m_modifierKeyShiftPressed = false;
        bool m_modifierKeyAltPressed = false;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_BACKEND_GUI_SFML_HPP
