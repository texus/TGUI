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

#ifndef TGUI_BACKEND_GUI_RAYLIB_HPP
#define TGUI_BACKEND_GUI_RAYLIB_HPP

#include <TGUI/Config.hpp>
#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/Backend/Window/BackendGui.hpp>
#endif

#include <raylib.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @since TGUI 1.3
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API BackendGuiRaylib : public BackendGui
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        BackendGuiRaylib();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ~BackendGuiRaylib();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Processes events that occured during the last frame and passes the event to the widgets
        ///
        /// @return Has the event been consumed?
        ///         When this function returns false, then the event was ignored by all widgets.
        ///
        /// @warning This function does not process key events, those are processed by repeatedly calling the handleCharPressed
        ///          and handleKeyPressed functions with the values returned from GetCharPressed and GetKeyPressed (until these
        ///          functions return 0).
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool handleEvents();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Passes the typed character to the focused widget
        ///
        /// @return Has the event been consumed?
        ///         When this function returns false, then the event was ignored by all widgets.
        ///
        /// This function may need to be called multiple times each frame. It is intended to be used as follows:
        /// @code
        /// int pressedChar = GetCharPressed();
        /// while (pressedChar)
        /// {
        ///     gui.handleCharPressed(pressedChar);
        ///     pressedChar = GetCharPressed();
        /// }
        /// @endcode
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool handleCharPressed(int pressedChar);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Passes the key press event to the focused widget
        ///
        /// @return Has the event been consumed?
        ///         When this function returns false, then the event was ignored by all widgets.
        ///
        /// This function may need to be called multiple times each frame. It is intended to be used as follows:
        /// @code
        /// int pressedKey = GetKeyPressed();
        /// while (pressedKey)
        /// {
        ///     gui.handleKeyPressed(pressedKey);
        ///     pressedKey = GetKeyPressed();
        /// }
        /// @endcode
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool handleKeyPressed(int pressedKey);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Give the gui control over the main loop
        ///
        /// @param clearColor  background color of the window
        ///
        /// This function is only intended in cases where your program only needs to respond to gui events.
        /// For multimedia applications, games, or other programs where you want a high framerate or do a lot of processing
        /// in the main loop, you should use your own main loop.
        ///
        /// You can consider this function to execute something similar (but not identical) to the following code:
        /// @code
        /// while (!WindowShouldClose())
        /// {
        ///     gui.handleEvents(); // Handles all non-keyboard events
        ///
        ///     int pressedChar = GetCharPressed();
        ///     while (pressedChar)
        ///     {
        ///         gui.handleCharPressed(pressedChar);
        ///         pressedChar = GetCharPressed();
        ///     }
        ///
        ///     int pressedKey = GetKeyPressed();
        ///     while (pressedKey)
        ///     {
        ///         gui.handleKeyPressed(pressedKey);
        ///         pressedKey = GetKeyPressed();
        ///     }
        ///
        ///     BeginDrawing();
        ///     ClearBackground({240, 240, 240, 255});
        ///     gui.draw();
        ///     EndDrawing();
        /// }
        /// @endcode
        ///
        /// @warning You should call SetTargetFPS before calling this function if you don't want the main loop to render as
        ///          many frames as possible and consume all cpu power in doing so.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void mainLoop(Color clearColor = {240, 240, 240}) override;

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
        /// @brief Breaks out of the main loop that is implemented inside the mainLoop function
        ///
        /// After calling gui.mainLoop(), this function can be called from an event handler to stop the program. It will set a
        /// flag so that the next iteration of the main loop will exit the mainLoop function.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void endMainLoop();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Processes events that occured during the last frame and turns them into TGUI events for handleEvent
        ///
        /// @note This function is mostly intended for internal use. It is an alternative to calling handleEvents().
        ///
        /// @param processKeyEvents  Should this function call GetCharPressed and GetKeyPressed or only process non-keyboard events?
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<Event> generateEventQueue(bool processKeyEvents);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief This function is called by TGUI when focusing a text field (EditBox or TextArea).
        ///        It may result in the software keyboard being opened.
        ///
        /// @param inputRect  The rectangle where text is being inputted
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void startTextInput(FloatRect inputRect) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief This function is called by TGUI when unfocusing a text field (EditBox or TextArea).
        ///        It may result in the software keyboard being closed.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void stopTextInput() override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets functions that are called to control the soft/virtual keyboard.
        ///
        /// @param showKeyboardFunc  Function to call when the keyboard needs to be shown. ShowSoftKeyboard in raymob.
        /// @param hideKeyboardFunc  Function to call when the keyboard needs to be hidden. HideSoftKeyboard in raymob.
        /// @param getKeyUnicodeFunc Function that returnes the last pressed key (or 0 if none). GetLastSoftKeyUnicode in raymob.
        /// @param clearLastKeyFunc  Function that resets the last pressed key. ClearLastSoftKey in raymob.
        ///
        /// Example usage when using raymob:
        /// @code
        /// gui.setSoftKeyboardFunctions(ShowSoftKeyboard, HideSoftKeyboard, GetLastSoftKeyUnicode, ClearLastSoftKey);
        /// @endcode
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSoftKeyboardFunctions(std::function<void()> showKeyboardFunc,
                                      std::function<void()> hideKeyboardFunc,
                                      std::function<int()> getKeyUnicodeFunc,
                                      std::function<void()> clearLastKeyFunc);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Updates the view and changes the size of the root container when needed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateContainerSize() override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        bool m_mouseOnWindow = true;
        bool m_endMainLoop = false;

        std::function<void()> m_showSoftKeyboardFunction;
        std::function<void()> m_hideSoftKeyboardFunction;
        std::function<int()> m_getLastSoftKeyUnicodeFunction;
        std::function<void()> m_clearLastSoftKeyFunction;
    };
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_BACKEND_GUI_RAYLIB_HPP
