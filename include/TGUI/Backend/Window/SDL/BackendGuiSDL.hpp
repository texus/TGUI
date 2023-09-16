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


#ifndef TGUI_BACKEND_GUI_SDL_HPP
#define TGUI_BACKEND_GUI_SDL_HPP

#include <TGUI/extlibs/IncludeSDL.hpp>
#if SDL_MAJOR_VERSION < 3
    #include <SDL_touch.h>
#endif

#include <TGUI/Config.hpp>
#if TGUI_BUILD_AS_CXX_MODULE
    import tgui;
#else
    #include <TGUI/Backend/Window/BackendGui.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API BackendGuiSDL : public BackendGui
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default constructor
        ///
        /// @warning setGuiWindow has to be called by the subclass that inherits from this base class before the gui is used
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        BackendGuiSDL() = default;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Passes the event to the widgets
        ///
        /// @param event  The event that was polled from the window
        ///
        /// @return Has the event been consumed?
        ///         When this function returns false, then the event was ignored by all widgets.
        ///
        /// You should call this function in your event loop.
        /// @code
        /// SDL_Event event;
        /// while (SDL_PollEvent(&event) != 0) {
        ///     gui.handleEvent(event);
        /// }
        /// @endcode
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool handleEvent(const SDL_Event& event);
        using BackendGui::handleEvent;


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
        /// while (!quit)
        /// {
        ///     SDL_Event event;
        ///     while (SDL_PollEvent(&event) != 0)
        ///     {
        ///         gui.handleEvent(event);
        ///         if (e.type == SDL_EVENT_QUIT)  // SDL_QUIT in SDL2, SDL_EVENT_QUIT in SDL3
        ///             quit = true;
        ///     }
        ///
        ///     glClear(GL_COLOR_BUFFER_BIT);
        ///     gui.draw();
        ///     SDL_GL_SwapWindow(window);
        /// }
        /// @endcode
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void mainLoop(Color clearColor = {240, 240, 240}) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Helper function that converts an SDL event to a TGUI event
        /// @param eventSFML  The input SDL event
        /// @param eventTGUI  The output TGUI event
        /// @return Did the SDL event match on a TGUI event and has the output event been written to?
        ///
        /// You typically don't need this function as you can call handleEvent directly with an SDL_Event.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD bool convertEvent(const SDL_Event& eventSDL, Event& eventTGUI);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the window that was provided to the gui
        ///
        /// @return Window that was set via the constructor or setWindow
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        SDL_Window* getWindow() const;


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
        /// @brief This function is called by TGUI when the position of the caret changes in a text field (EditBox or TextArea).
        ///        If an IME is used then this function may move the IME candidate list to the text cursor position.
        ///
        /// @param inputRect The rectangle where text is being inputted
        /// @param caretPos  Location of the text cursor, relative to the gui view
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateTextCursorPosition(FloatRect inputRect, Vector2f caretPos) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the window on which the gui should be drawn
        ///
        /// @param window  The SDL window that will be used by the gui
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setGuiWindow(SDL_Window* window);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Updates the view and changes the size of the root container when needed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateContainerSize() override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Shows the drawn content with SDL_GL_SwapWindow or SDL_RenderPresent, depending on the renderer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void presentScreen() = 0;

#if ((SDL_MAJOR_VERSION == 2) && (SDL_MINOR_VERSION < 26))
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Updates m_framebufferSize by calling an equivalent of SDL_GetRendererOutputSize, depending on the renderer
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void updateFramebufferSize() = 0;
#endif

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        SDL_Window* m_window = nullptr;
        float m_dpiScale = 1;

        bool m_textInputStarted = false;
        SDL_Rect m_textInputRect = {};

        bool m_touchFirstFingerDown = false;
        SDL_FingerID m_touchFirstFingerId = 0;      // Only valid if m_touchFirstFingerDown is true
        SDL_TouchID  m_touchFirstFingerTouchId = 0; // Only valid if m_touchFirstFingerDown is true

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_BACKEND_GUI_SDL_HPP
