#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/GLFW-GLES2.hpp>
#include <TGUI/Backend/Renderer/OpenGL.hpp>
#include <emscripten/emscripten.h>
#include <iostream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

EM_JS(int, canvas_get_width, (), { return canvas.width; });
EM_JS(int, canvas_get_height, (), { return canvas.height; });

GLFWwindow* window = nullptr;
std::unique_ptr<tgui::Gui> gui = nullptr;

void init()
{
    glfwInit();

std::cout << "glfwGetVersionString PROG " << glfwGetVersionString() << " " << GLFW_VERSION_MAJOR << "." << GLFW_VERSION_MINOR << "." << GLFW_VERSION_REVISION << "\n";

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "TGUI - Emscripten - GLFW_GLES2", NULL, NULL);
    if (window == nullptr)
        throw std::runtime_error("Failed to create window");

    glfwMakeContextCurrent(window);

    gui = std::make_unique<tgui::Gui>(window);

    // We set the user pointer so that the callbacks can access the gui
    glfwSetWindowUserPointer(window, gui.get());

    // When we receive an event callback from GLFW, we need to pass the event to TGUI
    glfwSetWindowFocusCallback(window, [](GLFWwindow* wnd, int focused) {
        static_cast<tgui::Gui*>(glfwGetWindowUserPointer(wnd))->windowFocusCallback(focused);
    });
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* wnd, int width, int height) {
        static_cast<tgui::Gui*>(glfwGetWindowUserPointer(wnd))->sizeCallback(width, height);
    });
    glfwSetCharCallback(window, [](GLFWwindow* wnd, unsigned int codepoint) {
        static_cast<tgui::Gui*>(glfwGetWindowUserPointer(wnd))->charCallback(codepoint);
    });
    glfwSetKeyCallback(window, [](GLFWwindow* wnd, int key, int scancode, int action, int mods) {
        static_cast<tgui::Gui*>(glfwGetWindowUserPointer(wnd))->keyCallback(key, scancode, action, mods);
    });
    glfwSetScrollCallback(window, [](GLFWwindow* wnd, double xoffset, double yoffset) {
        static_cast<tgui::Gui*>(glfwGetWindowUserPointer(wnd))->scrollCallback(xoffset, yoffset);
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* wnd, double xpos, double ypos) {
        static_cast<tgui::Gui*>(glfwGetWindowUserPointer(wnd))->cursorPosCallback(xpos, ypos);
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* wnd, int button, int action, int mods) {
        std::cout << glfwGetWindowUserPointer(wnd) << "\n";
        static_cast<tgui::Gui*>(glfwGetWindowUserPointer(wnd))->mouseButtonCallback(button, action, mods);
    });
    glfwSetCursorEnterCallback(window, [](GLFWwindow* wnd, int entered) {
        static_cast<tgui::Gui*>(glfwGetWindowUserPointer(wnd))->cursorEnterCallback(entered);
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* wnd, int button, int action, int mods) {
        static_cast<tgui::Gui*>(glfwGetWindowUserPointer(wnd))->mouseButtonCallback(button, action, mods);
    });

    auto picture = tgui::Picture::create("data/background.jpg");
    picture->setSize({400, 300});
    gui->add(picture);

    auto button = tgui::Button::create("Hello world");
    button->setPosition({20, 10});
    button->setSize({200, 30});
    button->onClick([] { std::cout << "Button clicked\n"; });
    gui->add(button);
}

void render()
{
    glClearColor(91.f / 255.f, 10.f / 255.f, 145.f / 255.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    gui->draw();
    glfwSwapBuffers(window);
}

void mainLoop()
{
    render();
    glfwPollEvents();
}

int main()
{
    init();

    emscripten_set_main_loop(mainLoop, -1, true);

    gui = nullptr;
    glfwDestroyWindow(window);
    glfwTerminate();
}
