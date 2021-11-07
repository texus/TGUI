#define CATCH_CONFIG_RUNNER
#include "Tests.hpp"

#include <TGUI/TextureManager.hpp>
#include <TGUI/DefaultBackendWindow.hpp>

const unsigned int windowWidth = 250;
const unsigned int windowHeight = 100;
const char* windowTitle = "TGUI Tests";

struct TestsWindowBase
{
    virtual ~TestsWindowBase() = default;
};

struct TestsWindowDefault : public TestsWindowBase
{
    std::shared_ptr<tgui::DefaultBackendWindow> window = tgui::DefaultBackendWindow::create(windowWidth, windowHeight, windowTitle);
};

#if TGUI_HAS_BACKEND_SFML_GRAPHICS
    #include <TGUI/Backend/SFML-Graphics.hpp>
    struct TestsWindowSfmlGraphics : public TestsWindowBase
    {
        sf::RenderWindow window{sf::VideoMode{windowWidth, windowHeight}, windowTitle};
        tgui::SFML_GRAPHICS::Gui gui{window};
    };
#endif
#if TGUI_HAS_BACKEND_SFML_OPENGL3
    #include <TGUI/Backend/SFML-OpenGL3.hpp>
    struct TestsWindowSfmlOpenGL3 : public TestsWindowBase
    {
        sf::Window window{sf::VideoMode{windowWidth, windowHeight}, windowTitle, sf::Style::Default, sf::ContextSettings(0, 0, 0, 3, 3, sf::ContextSettings::Attribute::Core)};
        tgui::SFML_OPENGL3::Gui gui{window};
    };
#endif
#if TGUI_HAS_BACKEND_SDL_RENDERER
    #include <TGUI/Backend/SDL-Renderer.hpp>
    struct TestsWindowSdlRenderer : public TestsWindowBase
    {
        TestsWindowSdlRenderer()
        {
            SDL_Init(SDL_INIT_VIDEO);
            TTF_Init();
            window = SDL_CreateWindow(windowTitle,
                                      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                      windowWidth, windowHeight,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            gui = std::make_unique<tgui::SDL_RENDERER::Gui>(window, renderer);
        }

        ~TestsWindowSdlRenderer()
        {
            gui = nullptr;
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            TTF_Quit();
            SDL_Quit();
        }

        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
        std::unique_ptr<tgui::SDL_RENDERER::Gui> gui;
    };
#endif
#if TGUI_HAS_BACKEND_SDL_TTF_OPENGL3
    #include <TGUI/Backend/SDL-TTF-OpenGL3.hpp>
    struct TestsWindowSdlTtfOpenGL3 : public TestsWindowBase
    {
        TestsWindowSdlTtfOpenGL3()
        {
            SDL_Init(SDL_INIT_VIDEO);
            TTF_Init();
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            window = SDL_CreateWindow(windowTitle,
                                      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                      windowWidth, windowHeight,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
            glContext = SDL_GL_CreateContext(window);
            gui = std::make_unique<tgui::SDL_TTF_OPENGL3::Gui>(window);
        }

        ~TestsWindowSdlTtfOpenGL3()
        {
            gui = nullptr;
            SDL_GL_DeleteContext(glContext);
            SDL_DestroyWindow(window);
            TTF_Quit();
            SDL_Quit();
        }

        SDL_Window* window = nullptr;
        SDL_GLContext glContext = nullptr;
        std::unique_ptr<tgui::SDL_TTF_OPENGL3::Gui> gui;
    };
#endif
#if TGUI_HAS_BACKEND_SDL_TTF_GLES2
    #include <TGUI/Backend/SDL-TTF-GLES2.hpp>
    struct TestsWindowSdlTtfGLES2 : public TestsWindowBase
    {
        TestsWindowSdlTtfGLES2()
        {
            SDL_Init(SDL_INIT_VIDEO);
            TTF_Init();
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
            window = SDL_CreateWindow(windowTitle,
                                      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                      windowWidth, windowHeight,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
            glContext = SDL_GL_CreateContext(window);
            gui = std::make_unique<tgui::SDL_TTF_GLES2::Gui>(window);
        }

        ~TestsWindowSdlTtfGLES2()
        {
            gui = nullptr;
            SDL_GL_DeleteContext(glContext);
            SDL_DestroyWindow(window);
            TTF_Quit();
            SDL_Quit();
        }

        SDL_Window* window = nullptr;
        SDL_GLContext glContext = nullptr;
        std::unique_ptr<tgui::SDL_TTF_GLES2::Gui> gui;
    };
#endif
#if TGUI_HAS_BACKEND_SDL_OPENGL3
    #include <TGUI/Backend/SDL-OpenGL3.hpp>
    struct TestsWindowSdlOpenGL3 : public TestsWindowBase
    {
        TestsWindowSdlOpenGL3()
        {
            SDL_Init(SDL_INIT_VIDEO);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            window = SDL_CreateWindow(windowTitle,
                                      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                      windowWidth, windowHeight,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
            glContext = SDL_GL_CreateContext(window);
            gui = std::make_unique<tgui::SDL_OPENGL3::Gui>(window);
        }

        ~TestsWindowSdlOpenGL3()
        {
            gui = nullptr;
            SDL_GL_DeleteContext(glContext);
            SDL_DestroyWindow(window);
            SDL_Quit();
        }

        SDL_Window* window = nullptr;
        SDL_GLContext glContext = nullptr;
        std::unique_ptr<tgui::SDL_OPENGL3::Gui> gui;
    };
#endif
#if TGUI_HAS_BACKEND_SDL_GLES2
    #include <TGUI/Backend/SDL-GLES2.hpp>
    struct TestsWindowSdlGLES2 : public TestsWindowBase
    {
        TestsWindowSdlGLES2()
        {
            SDL_Init(SDL_INIT_VIDEO);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
            window = SDL_CreateWindow(windowTitle,
                                      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                      windowWidth, windowHeight,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
            glContext = SDL_GL_CreateContext(window);
            gui = std::make_unique<tgui::SDL_GLES2::Gui>(window);
        }

        ~TestsWindowSdlGLES2()
        {
            gui = nullptr;
            SDL_GL_DeleteContext(glContext);
            SDL_DestroyWindow(window);
            SDL_Quit();
        }

        SDL_Window* window = nullptr;
        SDL_GLContext glContext = nullptr;
        std::unique_ptr<tgui::SDL_GLES2::Gui> gui;
    };
#endif
#if TGUI_HAS_BACKEND_GLFW_OPENGL3
    #include <TGUI/Backend/GLFW-OpenGL3.hpp>
    #define GLFW_INCLUDE_NONE
    #include <GLFW/glfw3.h>
    struct TestsWindowGlfwOpenGL3 : public TestsWindowBase
    {
        TestsWindowGlfwOpenGL3()
        {
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
            window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);
            glfwMakeContextCurrent(window);
            gui = std::make_unique<tgui::GLFW_OPENGL3::Gui>(window);
        }

        ~TestsWindowGlfwOpenGL3()
        {
            gui = nullptr;
            glfwDestroyWindow(window);
            glfwTerminate();
        }

        GLFWwindow* window = nullptr;
        std::unique_ptr<tgui::GLFW_OPENGL3::Gui> gui;
    };
#endif
#if TGUI_HAS_BACKEND_GLFW_GLES2
    #include <TGUI/Backend/GLFW-GLES2.hpp>
    #define GLFW_INCLUDE_NONE
    #include <GLFW/glfw3.h>
    struct TestsWindowGlfwGLES2 : public TestsWindowBase
    {
        TestsWindowGlfwGLES2()
        {
            glfwInit();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
            window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);
            glfwMakeContextCurrent(window);
            gui = std::make_unique<tgui::GLFW_GLES2::Gui>(window);
        }

        ~TestsWindowGlfwGLES2()
        {
            gui = nullptr;
            glfwDestroyWindow(window);
            glfwTerminate();
        }

        GLFWwindow* window = nullptr;
        std::unique_ptr<tgui::GLFW_GLES2::Gui> gui;
    };
#endif

int main(int argc, char * argv[])
{
    std::string selectedBackend;
    Catch::Session session;

    auto cli = session.cli() | Catch::clara::Opt(selectedBackend, "backend")["--backend"]("TGUI backend to use (empty to let TGUI pick one)");

    session.cli(cli);
    if (session.applyCommandLine(argc, argv) != 0)
      return 1;

    std::unique_ptr<TestsWindowBase> window;
    if (selectedBackend.empty())
        window = std::make_unique<TestsWindowDefault>();
    else
    {
#if TGUI_HAS_BACKEND_SFML_GRAPHICS
        if (selectedBackend == "SFML_GRAPHICS")
            window = std::make_unique<TestsWindowSfmlGraphics>();
#endif
#if TGUI_HAS_BACKEND_SFML_OPENGL3
        if (selectedBackend == "SFML_OPENGL3")
            window = std::make_unique<TestsWindowSfmlOpenGL3>();
#endif
#if TGUI_HAS_BACKEND_SDL_RENDERER
        if (selectedBackend == "SDL_RENDERER")
            window = std::make_unique<TestsWindowSdlRenderer>();
#endif
#if TGUI_HAS_BACKEND_SDL_TTF_OPENGL3
        if (selectedBackend == "SDL_TTF_OPENGL3")
            window = std::make_unique<TestsWindowSdlTtfOpenGL3>();
#endif
#if TGUI_HAS_BACKEND_SDL_TTF_GLES2
        if (selectedBackend == "SDL_TTF_GLES2")
            window = std::make_unique<TestsWindowSdlTtfGLES2>();
#endif
#if TGUI_HAS_BACKEND_SDL_OPENGL3
        if (selectedBackend == "SDL_OPENGL3")
            window = std::make_unique<TestsWindowSdlOpenGL3>();
#endif
#if TGUI_HAS_BACKEND_SDL_GLES2
        if (selectedBackend == "SDL_GLES2")
            window = std::make_unique<TestsWindowSdlGLES2>();
#endif
#if TGUI_HAS_BACKEND_GLFW_OPENGL3
        if (selectedBackend == "GLFW_OPENGL3")
            window = std::make_unique<TestsWindowGlfwOpenGL3>();
#endif
#if TGUI_HAS_BACKEND_GLFW_GLES2
        if (selectedBackend == "GLFW_GLES2")
            window = std::make_unique<TestsWindowGlfwGLES2>();
#endif

        if (!window)
        {
            std::cerr << "Backend parameter was provided but no matching backend was enabled TGUI\n";
            throw std::runtime_error("Backend parameter was provided but no matching backend was enabled TGUI");
        }
    }

    return session.run();
}

TEST_CASE("[Memory leak test]")
{
    REQUIRE(tgui::TextureManager::getCachedImagesCount() == 0);
}
