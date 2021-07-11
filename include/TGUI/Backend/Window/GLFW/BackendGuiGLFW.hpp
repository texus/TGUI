/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2021 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/Backend/Window/BackendGui.hpp>
#include <TGUI/Optional.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct GLFWwindow GLFWwindow;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /// TODO: Document member functions

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API BackendGuiGLFW : public BackendGui
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default constructor
        ///
        /// @warning If you use this constructor then you will still have to call setWindow before using the gui.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        BackendGuiGLFW() = default;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructs the gui and set the window on which the gui should be drawn
        ///
        /// @param window  The GLFW window that will be used by the gui
        ///
        /// If you use this constructor then you will no longer have to call setWindow yourself.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        BackendGuiGLFW(GLFWwindow* window);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the window on which the gui should be drawn
        ///
        /// @param window  The GLFW window that will be used by the gui
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setWindow(GLFWwindow* window);



        void windowFocusCallback(int focused);

        void sizeCallback(int width, int height);

        void charCallback(unsigned int codepoint);

        void keyCallback(int key, int scancode, int action, int mods);

        void scrollCallback(double xoffset, double yoffset);

        void cursorPosCallback(double xpos, double ypos);

        void mouseButtonCallback(int button, int action, int mods);



        Optional<Event> convertWindowFocusEvent(int focused);

        Optional<Event> convertSizeEvent(int width, int height);

        Optional<Event> convertCharEvent(unsigned int codepoint);

        Optional<Event> convertKeyEvent(int key, int scancode, int action, int mods);

        Optional<Event> convertScrollEvent(double xoffset, double yoffset);

        Optional<Event> convertCursorPosEvent(double xpos, double ypos);

        Optional<Event> convertMouseButtonEvent(int button, int action, int mods);



        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Give the gui control over the main loop
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
        void mainLoop() override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_BACKEND_GUI_GLFW_HPP
