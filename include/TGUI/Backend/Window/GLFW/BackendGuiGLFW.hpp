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


#ifndef TGUI_BACKEND_GUI_GLFW_HPP
#define TGUI_BACKEND_GUI_GLFW_HPP

#include <TGUI/Config.hpp>
#if TGUI_BUILD_AS_CXX_MODULE
    import tgui;
#else
    #include <TGUI/Backend/Window/BackendGui.hpp>
    #include <TGUI/Optional.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using GLFWwindow = struct GLFWwindow;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_IGNORE_DEPRECATED_WARNINGS_START // Required for VS2017 due to inheriting a function that we deprecated

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API BackendGuiGLFW : public BackendGui
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default constructor
        ///
        /// @warning setGuiWindow has to be called by the subclass that inherits from this base class before the gui is used
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        BackendGuiGLFW();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ~BackendGuiGLFW();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Inform the gui about a focus event when a callback from glfwSetWindowFocusCallback occurs
        /// @param focused  GLFW_TRUE if the window was given input focus, or GLFW_FALSE if it lost it
        /// @return Has the event been consumed? When this function returns false, then the event was ignored by all widgets.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool windowFocusCallback(int focused);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Inform the gui about a framebuffer size event when a callback from glfwSetFramebufferSizeCallback occurs
        /// @param width  The new width, in pixels, of the framebuffer
        /// @param height The new height, in pixels, of the framebuffer
        /// @return Has the event been consumed? When this function returns false, then the event was ignored by all widgets.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool sizeCallback(int width, int height);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Inform the gui about a char event when a callback from glfwSetCharCallback occurs
        /// @param codepoint  The Unicode code point of the character
        /// @return Has the event been consumed? When this function returns false, then the event was ignored by all widgets.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool charCallback(unsigned int codepoint);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Inform the gui about a key event when a callback from glfwSetKeyCallback occurs
        /// @param key      The keyboard key that was pressed or released
        /// @param scancode The system-specific scancode of the key
        /// @param action   GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
        /// @param mods     Bit field describing which modifier keys were held down
        /// @return Has the event been consumed? When this function returns false, then the event was ignored by all widgets.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool keyCallback(int key, int scancode, int action, int mods);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Inform the gui about a scroll event when a callback from glfwSetScrollCallback occurs
        /// @param xoffset  The scroll offset along the x-axis
        /// @param yoffset  The scroll offset along the y-axis
        /// @return Has the event been consumed? When this function returns false, then the event was ignored by all widgets.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool scrollCallback(double xoffset, double yoffset);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Inform the gui about a cursor position event when a callback from glfwSetCursorPosCallback occurs
        /// @param xpos  The new cursor x-coordinate, relative to the left edge of the content area
        /// @param ypos  The new cursor y-coordinate, relative to the top edge of the content area
        /// @return Has the event been consumed? When this function returns false, then the event was ignored by all widgets.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool cursorPosCallback(double xpos, double ypos);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Inform the gui about a mouse button event when a callback from glfwSetMouseButtonCallback occurs
        /// @param button  The mouse button that was pressed or released
        /// @param action  One of GLFW_PRESS or GLFW_RELEASE
        /// @param mods    Bit field describing which modifier keys were held down
        /// @return Has the event been consumed? When this function returns false, then the event was ignored by all widgets.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool mouseButtonCallback(int button, int action, int mods);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Inform the gui that the mouse has left or entered the window when a callback from glfwSetCursorEnterCallback occurs
        /// @param entered  Indicates whether the mouse has entered or left the content area of the window
        /// @return Has the event been consumed? When this function returns false, then the event was ignored by all widgets.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool cursorEnterCallback(int entered);

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
        /// while (!glfwWindowShouldClose(window))
        /// {
        ///     glClear(GL_COLOR_BUFFER_BIT);
        ///     gui.draw();
        ///     glfwSwapBuffers(window);
        ///     glfwWaitEvents();
        /// }
        /// @endcode
        ///
        /// Note that this function calls glfwSetWindowUserPointer and overwrites callback handlers.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void mainLoop(Color clearColor = {240, 240, 240}) override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Create an event that can be passed to handleEvent when a callback from glfwSetWindowFocusCallback occurs
        /// @param focused  GLFW_TRUE if the window was given input focus, or GLFW_FALSE if it lost it
        /// @see windowFocusCallback
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Optional<Event> convertWindowFocusEvent(int focused);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Create an event that can be passed to handleEvent when a callback from glfwSetFramebufferSizeCallback occurs
        /// @param width  The new width, in pixels, of the framebuffer
        /// @param height The new height, in pixels, of the framebuffer
        /// @see sizeCallback
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Optional<Event> convertSizeEvent(int width, int height);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Create an event that can be passed to handleEvent when a callback from glfwSetCharCallback occurs
        /// @param codepoint  The Unicode code point of the character
        /// @see charCallback
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Optional<Event> convertCharEvent(unsigned int codepoint);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Create an event that can be passed to handleEvent when a callback from glfwSetKeyCallback occurs
        /// @param key      The keyboard key that was pressed or released
        /// @param scancode The system-specific scancode of the key
        /// @param action   GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
        /// @param mods     Bit field describing which modifier keys were held down
        /// @see keyCallback
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Optional<Event> convertKeyEvent(int key, int scancode, int action, int mods);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Create an event that can be passed to handleEvent when a callback from glfwSetScrollCallback occurs
        /// @param xoffset  The scroll offset along the x-axis
        /// @param yoffset  The scroll offset along the y-axis
        /// @see scrollCallback
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Optional<Event> convertScrollEvent(double xoffset, double yoffset);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Create an event that can be passed to handleEvent when a callback from glfwSetCursorPosCallback occurs
        /// @param xpos  The new cursor x-coordinate, relative to the left edge of the content area
        /// @param ypos  The new cursor y-coordinate, relative to the top edge of the content area
        /// @see cursorPosCallback
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Optional<Event> convertCursorPosEvent(double xpos, double ypos);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Create an event that can be passed to handleEvent when a callback from glfwSetMouseButtonCallback occurs
        /// @param button  The mouse button that was pressed or released
        /// @param action  One of GLFW_PRESS or GLFW_RELEASE
        /// @param mods    Bit field describing which modifier keys were held down
        /// @see mouseButtonCallback
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Optional<Event> convertMouseButtonEvent(int button, int action, int mods);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Create an event that can be passed to handleEvent when a callback from glfwSetCursorEnterCallback occurs
        /// @param entered  Indicates whether the mouse has entered or left the content area of the window
        /// @see cursorEnterCallback
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Optional<Event> convertCursorEnterEvent(int entered);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the window that was provided to the gui
        ///
        /// @return Window that was set via the constructor or setWindow
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        GLFWwindow* getWindow() const;

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
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the window on which the gui should be drawn
        ///
        /// @param window  The GLFW window that will be used by the gui
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setGuiWindow(GLFWwindow* window);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Updates the view and changes the size of the root container when needed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void updateContainerSize() override;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        GLFWwindow* m_window = nullptr;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };
}

TGUI_IGNORE_DEPRECATED_WARNINGS_END

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_BACKEND_GUI_GLFW_HPP
