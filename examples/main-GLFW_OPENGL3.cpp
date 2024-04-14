/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/GLFW-OpenGL3.hpp>

// Optional: include (modern) OpenGL functions via TGUI (which you can call AFTER creating the Gui object)
// This will include a built-in GLAD header that defines all functions that exist in OpenGL 4.6 (and GLES 3.2)
//#include <TGUI/Backend/Renderer/OpenGL.hpp>

#define GLFW_INCLUDE_NONE // Don't let GLFW include an OpenGL extention loader
#include <GLFW/glfw3.h>

bool runExample(tgui::BackendGui& gui);

// We don't put this code in main() to make sure that all TGUI resources are destroyed before destroying GLFW
void run_application(GLFWwindow* window)
{
    tgui::Gui gui(window);

    runExample(gui);
    gui.mainLoop(); // To use your own main loop, see https://tgui.eu/tutorials/latest-stable/backend-glfw-opengl3/#main-loop
}

int main()
{
    glfwInit();

    // The OpenGL renderer backend in TGUI requires at least OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // Required for macOS

    GLFWwindow* window = glfwCreateWindow(800, 600, "TGUI example (GLFW-OpenGL3)", NULL, NULL);
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    run_application(window);

    glfwDestroyWindow(window);
    glfwTerminate();
}
