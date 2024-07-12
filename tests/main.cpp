
#include <TGUI/Config.hpp>

#if defined(TGUI_SYSTEM_WINDOWS) && defined(TGUI_HAS_BACKEND_RAYLIB)
  // raylib.h and windows.h don't mix well
  #define NOGDI
  #define NOUSER
#endif

#if TGUI_HAS_BACKEND_SFML_GRAPHICS
    #include <SFML/Graphics.hpp>
#endif
#if TGUI_HAS_BACKEND_SFML_OPENGL3
    #include <SFML/Window.hpp>
#endif
#if TGUI_HAS_BACKEND_SDL_GLES2 || TGUI_HAS_BACKEND_SDL_OPENGL3 || TGUI_HAS_BACKEND_SDL_TTF_GLES2 || TGUI_HAS_BACKEND_SDL_TTF_OPENGL3 || TGUI_HAS_BACKEND_SDL_RENDERER
    // If the program links to sfml-main then we shouldn't let SDL redefine "main" as "SDL_main"
    #if TGUI_HAS_BACKEND_SFML_GRAPHICS || TGUI_HAS_BACKEND_SFML_OPENGL3
        #define SDL_MAIN_HANDLED
    #endif
    #include <TGUI/extlibs/IncludeSDL.hpp>
    #if SDL_MAJOR_VERSION >= 3
        #include <SDL3/SDL_main.h>
    #endif
#endif
#if TGUI_HAS_BACKEND_SDL_TTF_GLES2 || TGUI_HAS_BACKEND_SDL_TTF_OPENGL3 || TGUI_HAS_BACKEND_SDL_RENDERER
    #if SDL_MAJOR_VERSION >= 3
        #include <SDL3_ttf/SDL_ttf.h>
    #else
        #include <SDL_ttf.h>
    #endif
#endif
#if TGUI_HAS_BACKEND_GLFW_OPENGL3 || TGUI_HAS_BACKEND_GLFW_GLES2
    #ifdef TGUI_SYSTEM_WINDOWS
        #include <TGUI/extlibs/IncludeWindows.hpp>
    #endif

    #define GLFW_INCLUDE_NONE
    #include <GLFW/glfw3.h>
#endif
#if TGUI_HAS_BACKEND_RAYLIB
    #include <raylib.h>
#endif

#define CATCH_CONFIG_RUNNER
#include "Tests.hpp"

#if TGUI_BUILD_AS_CXX_MODULE
    import tgui.default_backend_window;
#else
    #include <TGUI/TextureManager.hpp>
    #include <TGUI/DefaultBackendWindow.hpp>
#endif

const unsigned int windowWidth = 400;
const unsigned int windowHeight = 300;
const char* windowTitle = "TGUI Tests";

tgui::BackendGui* globalGui = nullptr;  // Declared as extern in Tests.hpp

struct TestsWindowBase
{
    virtual ~TestsWindowBase() = default;
    TGUI_NODISCARD virtual tgui::BackendGui* getGui() const { return gui.get(); }
    virtual void close() = 0;

protected:
    std::unique_ptr<tgui::BackendGui> gui;
};

struct TestsWindowDefault : public TestsWindowBase
{
    TGUI_NODISCARD tgui::BackendGui* getGui() const override { return window->getGui(); }
    void close() override { window->close(); }

    std::shared_ptr<tgui::DefaultBackendWindow> window = tgui::DefaultBackendWindow::create(windowWidth, windowHeight, windowTitle);
};

#if TGUI_HAS_BACKEND_SFML_GRAPHICS
    #if TGUI_BUILD_AS_CXX_MODULE
        import tgui.backend.sfml_graphics;
    #else
        #include <TGUI/Backend/SFML-Graphics.hpp>
    #endif
    struct TestsWindowSfmlGraphics : public TestsWindowBase
    {
        TestsWindowSfmlGraphics()
        {
            gui = std::make_unique<tgui::SFML_GRAPHICS::Gui>(window);
        }

        ~TestsWindowSfmlGraphics() override
        {
            gui = nullptr; // Destroy the gui before the window
            window.close();
            tgui::BackendSFML::cleanupLeakedCursors();
        }

        void close() override
        {
            window.close();
        }

    #if SFML_VERSION_MAJOR >= 3
        sf::RenderWindow window{sf::VideoMode{{windowWidth, windowHeight}}, windowTitle};
    #else
        sf::RenderWindow window{sf::VideoMode{windowWidth, windowHeight}, windowTitle};
    #endif
    };
#endif
#if TGUI_HAS_BACKEND_SFML_OPENGL3
    #if TGUI_BUILD_AS_CXX_MODULE
        import tgui.backend.sfml_opengl3;
    #else
        #include <TGUI/Backend/SFML-OpenGL3.hpp>
    #endif
    struct TestsWindowSfmlOpenGL3 : public TestsWindowBase
    {
        TestsWindowSfmlOpenGL3()
        {
            gui = std::make_unique<tgui::SFML_OPENGL3::Gui>(window);
        }

        ~TestsWindowSfmlOpenGL3() override
        {
            gui = nullptr; // Destroy the gui before the window
            window.close();
            tgui::BackendSFML::cleanupLeakedCursors();
        }

        void close() override
        {
            window.close();
        }

    #if SFML_VERSION_MAJOR >= 3
        sf::Window window{sf::VideoMode{{windowWidth, windowHeight}}, windowTitle, sf::Style::Default, sf::State::Windowed, sf::ContextSettings{0, 0, 0, 3, 3, sf::ContextSettings::Attribute::Core}};
    #else
        sf::Window window{sf::VideoMode{windowWidth, windowHeight}, windowTitle, sf::Style::Default, sf::ContextSettings{0, 0, 0, 3, 3, sf::ContextSettings::Attribute::Core}};
    #endif
    };
#endif

#if TGUI_HAS_BACKEND_SDL_RENDERER
    #if TGUI_BUILD_AS_CXX_MODULE
        import tgui.backend.sdl_renderer;
    #else
        #include <TGUI/Backend/SDL-Renderer.hpp>
    #endif
    struct TestsWindowSdlRenderer : public TestsWindowBase
    {
        TestsWindowSdlRenderer()
        {
            SDL_Init(SDL_INIT_VIDEO);
            TTF_Init();
#if SDL_MAJOR_VERSION >= 3
            window = SDL_CreateWindow(windowTitle,
                                      windowWidth, windowHeight,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
            renderer = SDL_CreateRenderer(window, nullptr);
#else
            window = SDL_CreateWindow(windowTitle,
                                      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                      windowWidth, windowHeight,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
#endif
            gui = std::make_unique<tgui::SDL_RENDERER::Gui>(window, renderer);
        }

        ~TestsWindowSdlRenderer() override
        {
            gui = nullptr;
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            TTF_Quit();
            SDL_Quit();
        }

        void close() override
        {
            SDL_Event event;
            event.type = SDL_EVENT_QUIT;
            SDL_PushEvent(&event);
        }

        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;
    };
#endif
#if TGUI_HAS_BACKEND_SDL_TTF_OPENGL3
    #if TGUI_BUILD_AS_CXX_MODULE
        import tgui.backend.sdl_ttf_opengl3;
    #else
        #include <TGUI/Backend/SDL-TTF-OpenGL3.hpp>
    #endif
    struct TestsWindowSdlTtfOpenGL3 : public TestsWindowBase
    {
        TestsWindowSdlTtfOpenGL3()
        {
            SDL_Init(SDL_INIT_VIDEO);
            TTF_Init();
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#if SDL_MAJOR_VERSION >= 3
            window = SDL_CreateWindow(windowTitle,
                                      windowWidth, windowHeight,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
#else
            window = SDL_CreateWindow(windowTitle,
                                      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                      windowWidth, windowHeight,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
#endif
            glContext = SDL_GL_CreateContext(window);
            gui = std::make_unique<tgui::SDL_TTF_OPENGL3::Gui>(window);
        }

        ~TestsWindowSdlTtfOpenGL3() override
        {
            gui = nullptr;
#if SDL_MAJOR_VERSION >= 3
            SDL_GL_DestroyContext(glContext);
#else
            SDL_GL_DeleteContext(glContext);
#endif
            SDL_DestroyWindow(window);
            TTF_Quit();
            SDL_Quit();
        }

        void close() override
        {
            SDL_Event event;
            event.type = SDL_EVENT_QUIT;
            SDL_PushEvent(&event);
        }

        SDL_Window* window = nullptr;
        SDL_GLContext glContext = nullptr;
    };
#endif
#if TGUI_HAS_BACKEND_SDL_TTF_GLES2
    #if TGUI_BUILD_AS_CXX_MODULE
        import tgui.backend.sdl_ttf_gles2;
    #else
        #include <TGUI/Backend/SDL-TTF-GLES2.hpp>
    #endif
    struct TestsWindowSdlTtfGLES2 : public TestsWindowBase
    {
        TestsWindowSdlTtfGLES2()
        {
            SDL_Init(SDL_INIT_VIDEO);
            TTF_Init();
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#if SDL_MAJOR_VERSION >= 3
            window = SDL_CreateWindow(windowTitle,
                                      windowWidth, windowHeight,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
#else
            window = SDL_CreateWindow(windowTitle,
                                      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                      windowWidth, windowHeight,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
#endif
            glContext = SDL_GL_CreateContext(window);
            gui = std::make_unique<tgui::SDL_TTF_GLES2::Gui>(window);
        }

        ~TestsWindowSdlTtfGLES2() override
        {
            gui = nullptr;
#if SDL_MAJOR_VERSION >= 3
            SDL_GL_DestroyContext(glContext);
#else
            SDL_GL_DeleteContext(glContext);
#endif
            SDL_DestroyWindow(window);
            TTF_Quit();
            SDL_Quit();
        }

        void close() override
        {
            SDL_Event event;
            event.type = SDL_EVENT_QUIT;
            SDL_PushEvent(&event);
        }

        SDL_Window* window = nullptr;
        SDL_GLContext glContext = nullptr;
    };
#endif
#if TGUI_HAS_BACKEND_SDL_OPENGL3
    #if TGUI_BUILD_AS_CXX_MODULE
        import tgui.backend.sdl_opengl3;
    #else
        #include <TGUI/Backend/SDL-OpenGL3.hpp>
    #endif
    struct TestsWindowSdlOpenGL3 : public TestsWindowBase
    {
        TestsWindowSdlOpenGL3()
        {
            SDL_Init(SDL_INIT_VIDEO);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#if SDL_MAJOR_VERSION >= 3
            window = SDL_CreateWindow(windowTitle,
                                      windowWidth, windowHeight,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
#else
            window = SDL_CreateWindow(windowTitle,
                                      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                      windowWidth, windowHeight,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
#endif
            glContext = SDL_GL_CreateContext(window);
            gui = std::make_unique<tgui::SDL_OPENGL3::Gui>(window);
        }

        ~TestsWindowSdlOpenGL3() override
        {
            gui = nullptr;
#if SDL_MAJOR_VERSION >= 3
            SDL_GL_DestroyContext(glContext);
#else
            SDL_GL_DeleteContext(glContext);
#endif
            SDL_DestroyWindow(window);
            SDL_Quit();
        }

        void close() override
        {
            SDL_Event event;
            event.type = SDL_EVENT_QUIT;
            SDL_PushEvent(&event);
        }

        SDL_Window* window = nullptr;
        SDL_GLContext glContext = nullptr;
    };
#endif
#if TGUI_HAS_BACKEND_SDL_GLES2
    #if TGUI_BUILD_AS_CXX_MODULE
        import tgui.backend.sdl_gles2;
    #else
        #include <TGUI/Backend/SDL-GLES2.hpp>
    #endif
    struct TestsWindowSdlGLES2 : public TestsWindowBase
    {
        TestsWindowSdlGLES2()
        {
            SDL_Init(SDL_INIT_VIDEO);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#if SDL_MAJOR_VERSION >= 3
            window = SDL_CreateWindow(windowTitle,
                                      windowWidth, windowHeight,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
#else
            window = SDL_CreateWindow(windowTitle,
                                      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                      windowWidth, windowHeight,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
#endif
            glContext = SDL_GL_CreateContext(window);
            gui = std::make_unique<tgui::SDL_GLES2::Gui>(window);
        }

        ~TestsWindowSdlGLES2() override
        {
            gui = nullptr;
#if SDL_MAJOR_VERSION >= 3
            SDL_GL_DestroyContext(glContext);
#else
            SDL_GL_DeleteContext(glContext);
#endif
            SDL_DestroyWindow(window);
            SDL_Quit();
        }

        void close() override
        {
            SDL_Event event;
            event.type = SDL_EVENT_QUIT;
            SDL_PushEvent(&event);
        }

        SDL_Window* window = nullptr;
        SDL_GLContext glContext = nullptr;
    };
#endif
#if TGUI_HAS_BACKEND_GLFW_OPENGL3
    #if TGUI_BUILD_AS_CXX_MODULE
        import tgui.backend.glfw_opengl3;
    #else
        #include <TGUI/Backend/GLFW-OpenGL3.hpp>
    #endif
    struct TestsWindowGlfwOpenGL3 : public TestsWindowBase
    {
        TestsWindowGlfwOpenGL3()
        {
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
            window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, nullptr, nullptr);
            glfwMakeContextCurrent(window);
            gui = std::make_unique<tgui::GLFW_OPENGL3::Gui>(window);
        }

        ~TestsWindowGlfwOpenGL3() override
        {
            gui = nullptr;
            glfwDestroyWindow(window);
            glfwTerminate();
        }

        void close() override
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        GLFWwindow* window = nullptr;
    };
#endif
#if TGUI_HAS_BACKEND_GLFW_GLES2
    #if TGUI_BUILD_AS_CXX_MODULE
        import tgui.backend.glfw_gles2;
    #else
        #include <TGUI/Backend/GLFW-GLES2.hpp>
    #endif
    struct TestsWindowGlfwGLES2 : public TestsWindowBase
    {
        TestsWindowGlfwGLES2()
        {
            glfwInit();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
            window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, nullptr, nullptr);
            glfwMakeContextCurrent(window);
            gui = std::make_unique<tgui::GLFW_GLES2::Gui>(window);
        }

        ~TestsWindowGlfwGLES2() override
        {
            gui = nullptr;
            glfwDestroyWindow(window);
            glfwTerminate();
        }

        void close() override
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        GLFWwindow* window = nullptr;
    };
#endif
#if TGUI_HAS_BACKEND_RAYLIB
    #if TGUI_BUILD_AS_CXX_MODULE
        import tgui.backend.raylib;
    #else
        #include <TGUI/Backend/raylib.hpp>
    #endif
    struct TestsWindowRaylib : public TestsWindowBase
    {
        TestsWindowRaylib()
        {
            SetTraceLogLevel(LOG_WARNING);
            InitWindow(windowWidth, windowHeight, windowTitle);
            gui = std::make_unique<tgui::RAYLIB::Gui>();
        }

        ~TestsWindowRaylib() override
        {
            gui = nullptr;
            CloseWindow();
        }

        void close() override
        {
            static_cast<tgui::RAYLIB::Gui*>(gui.get())->endMainLoop();
        }
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
#if TGUI_HAS_BACKEND_RAYLIB
        if (selectedBackend == "RAYLIB")
            window = std::make_unique<TestsWindowRaylib>();
#endif

        if (!window)
        {
            std::cerr << "Backend parameter was provided but no matching backend was enabled TGUI\n";
            return 1;
        }
    }

    globalGui = window->getGui();

    // Run the tests
    auto result = session.run();

    // Execute the main loop for a short moment
    // The first timer will be triggered immediately and will start the second timer which will
    // be triggered after rendering the first frame. The second timer closes the window.
    tgui::Timer::scheduleCallback([w=window.get()]{
        tgui::Timer::scheduleCallback([w]{
            w->close();
        });
    });
    globalGui->mainLoop();

    globalGui = nullptr; // Don't keep a pointer to memory that will be destroyed soon
    return result;
}

TEST_CASE("[Memory leak test]")
{
    REQUIRE(tgui::TextureManager::getCachedImagesCount() == 0);
}
