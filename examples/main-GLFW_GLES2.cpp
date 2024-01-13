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
#include <TGUI/Backend/GLFW-GLES2.hpp>

// Optional: include OpenGL ES functions via TGUI (which you can call AFTER creating the Gui object)
// This will include a built-in GLAD header that defines all functions that exist in GLES 3.2 (and OpenGL 4.6)
//#include <TGUI/Backend/Renderer/OpenGL.hpp>

#define GLFW_INCLUDE_NONE // Don't let GLFW include an OpenGL extention loader
#include <GLFW/glfw3.h>

bool runExample(tgui::BackendGui& gui);

// We don't put this code in main() to make sure that all TGUI resources are destroyed before destroying GLFW
void run_application(GLFWwindow* window)
{
    tgui::Gui gui(window);

    runExample(gui);
    gui.mainLoop();
}

int main()
{
    glfwInit();

    // The OpenGL ES renderer backend in TGUI requires at least OpenGL ES 2.0
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(800, 600, "TGUI example (GLFW-GLES2)", NULL, NULL);
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    run_application(window);

    glfwDestroyWindow(window);
    glfwTerminate();
}
