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


#include <TGUI/DefaultBackendWindow.hpp>

#if TGUI_BUILD_AS_CXX_MODULE
    #if TGUI_HAS_BACKEND_SFML_GRAPHICS
        import tgui.backend.sfml_graphics;
    #elif TGUI_HAS_BACKEND_SFML_OPENGL3
        import tgui.backend.sfml_opengl3;
    #elif TGUI_HAS_BACKEND_SDL_RENDERER
        import tgui.backend.sdl_renderer;
    #elif TGUI_HAS_BACKEND_SDL_TTF_OPENGL3
        import tgui.backend.sdl_ttf_opengl3;
    #elif TGUI_HAS_BACKEND_SDL_TTF_GLES2
        import tgui.backend.sdl_ttf_gles2;
    #elif TGUI_HAS_BACKEND_SDL_OPENGL3
        import tgui.backend.sdl_opengl3;
    #elif TGUI_HAS_BACKEND_SDL_GLES2
        import tgui.backend.sdl_gles2;
    #elif TGUI_HAS_BACKEND_GLFW_OPENGL3 || TGUI_HAS_BACKEND_GLFW_GLES2
        #define GLFW_INCLUDE_NONE // Don't let GLFW include an OpenGL extention loader
        #include <GLFW/glfw3.h>

        #if !TGUI_EXPERIMENTAL_USE_STD_MODULE
            #include <queue>
        #endif

        #if TGUI_HAS_BACKEND_GLFW_OPENGL3
            import tgui.backend.glfw_opengl3;
        #else
            import tgui.backend.glfw_gles2;
        #endif
    #endif
#else
    #include <TGUI/Loading/ImageLoader.hpp>
    #include <TGUI/Backend/Window/BackendGui.hpp>
    #include <TGUI/Event.hpp>

    #if TGUI_HAS_BACKEND_SFML_GRAPHICS
        #include <TGUI/Backend/SFML-Graphics.hpp>
    #elif TGUI_HAS_BACKEND_SFML_OPENGL3
        #include <TGUI/Backend/SFML-OpenGL3.hpp>
    #elif TGUI_HAS_BACKEND_SDL_RENDERER
        #include <TGUI/Backend/SDL-Renderer.hpp>
    #elif TGUI_HAS_BACKEND_SDL_TTF_OPENGL3
        #include <TGUI/Backend/SDL-TTF-OpenGL3.hpp>
    #elif TGUI_HAS_BACKEND_SDL_TTF_GLES2
        #include <TGUI/Backend/SDL-TTF-GLES2.hpp>
    #elif TGUI_HAS_BACKEND_SDL_OPENGL3
        #include <TGUI/Backend/SDL-OpenGL3.hpp>
    #elif TGUI_HAS_BACKEND_SDL_GLES2
        #include <TGUI/Backend/SDL-GLES2.hpp>
    #elif TGUI_HAS_BACKEND_GLFW_OPENGL3 || TGUI_HAS_BACKEND_GLFW_GLES2
        #if TGUI_HAS_BACKEND_GLFW_OPENGL3
            #include <TGUI/Backend/GLFW-OpenGL3.hpp>
        #else
            #include <TGUI/Backend/GLFW-GLES2.hpp>
        #endif

        #define GLFW_INCLUDE_NONE // Don't let GLFW include an OpenGL extention loader
        #include <GLFW/glfw3.h>

        #if !TGUI_EXPERIMENTAL_USE_STD_MODULE
            #include <queue>
        #endif
    #endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_HAS_BACKEND_SFML_GRAPHICS || TGUI_HAS_BACKEND_SFML_OPENGL3

    class BackendWindowSFML : public DefaultBackendWindow
    {
    public:
        BackendWindowSFML(unsigned int width, unsigned int height, const String& title)
        {
            sf::ContextSettings settings;
#if !TGUI_HAS_BACKEND_SFML_GRAPHICS
            settings.majorVersion = 3;
            settings.minorVersion = 3;
            settings.attributeFlags = sf::ContextSettings::Attribute::Core;
#endif

#if SFML_VERSION_MAJOR >= 3
            m_window.create(sf::VideoMode{{width, height}}, title.toStdString(), sf::Style::Default, settings);
#else
            m_window.create(sf::VideoMode{width, height}, title.toStdString(), sf::Style::Default, settings);
#endif

            m_gui = std::make_unique<Gui>(m_window);
            m_gui->getBackendRenderTarget()->setClearColor({200, 200, 200});
        }

        ~BackendWindowSFML() override
        {
            // Destroy the gui before closing the window, as closing the window destroys the OpenGL context
            // that will still be used when destructing the backend renderer.
            m_gui = nullptr;
            close();
            tgui::BackendSFML::cleanupLeakedCursors();
        }

        BackendGui* getGui() const override
        {
            return m_gui.get();
        }

        bool isOpen() const override
        {
            return m_window.isOpen();
        }

        void close() override
        {
            m_window.close();
        }

        bool pollEvent(Event& event) override
        {
            sf::Event eventSFML;
            if (m_window.pollEvent(eventSFML))
                return m_gui->convertEvent(eventSFML, event);
            else // No new events
                return false;
        }

        void draw() override
        {
            m_gui->getBackendRenderTarget()->clearScreen();
            m_gui->draw();
            m_window.display();
        }

        void mainLoop(Color clearColor) override
        {
            m_gui->mainLoop(clearColor);
        }

        void setIcon(const String& filename) override
        {
            Vector2u iconSize;
            auto pixelPtr = ImageLoader::loadFromFile((getResourcePath() / filename).asString(), iconSize);
            if (pixelPtr)
#if SFML_VERSION_MAJOR >= 3
                m_window.setIcon({iconSize.x, iconSize.y}, pixelPtr.get());
#else
                m_window.setIcon(iconSize.x, iconSize.y, pixelPtr.get());
#endif
        }

    private:
#if TGUI_HAS_BACKEND_SFML_GRAPHICS
        sf::RenderWindow m_window;
#else
        sf::Window m_window;
#endif
        std::unique_ptr<Gui> m_gui;
    };

#elif TGUI_HAS_BACKEND_SDL_RENDERER || TGUI_HAS_BACKEND_SDL_OPENGL3 || TGUI_HAS_BACKEND_SDL_GLES2 || TGUI_HAS_BACKEND_SDL_TTF_OPENGL3 || TGUI_HAS_BACKEND_SDL_TTF_GLES2

    class BackendWindowSDL : public DefaultBackendWindow
    {
    public:
        BackendWindowSDL(unsigned int width, unsigned int height, const String& title)
        {
            SDL_Init(SDL_INIT_VIDEO);

#if TGUI_HAS_BACKEND_SDL_RENDERER || TGUI_HAS_BACKEND_SDL_TTF_OPENGL3 || TGUI_HAS_BACKEND_SDL_TTF_GLES2
            TTF_Init();
#endif

#if TGUI_HAS_BACKEND_SDL_OPENGL3 || TGUI_HAS_BACKEND_SDL_TTF_OPENGL3
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#elif TGUI_HAS_BACKEND_SDL_GLES || TGUI_HAS_BACKEND_SDL_TTF_GLES2
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#endif

#if SDL_MAJOR_VERSION >= 3
            m_window = SDL_CreateWindow(title.toStdString().c_str(),
                                        static_cast<int>(width), static_cast<int>(height),
                                        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
#else
            m_window = SDL_CreateWindow(title.toStdString().c_str(),
                                        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                        static_cast<int>(width), static_cast<int>(height),
                                        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
#endif

#if TGUI_HAS_BACKEND_SDL_RENDERER
    #if SDL_MAJOR_VERSION >= 3
            m_renderer = SDL_CreateRenderer(m_window, nullptr, SDL_RENDERER_ACCELERATED);
    #else
            m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    #endif
            m_gui = std::make_unique<Gui>(m_window, m_renderer);
#else
            m_glContext = SDL_GL_CreateContext(m_window);
            m_gui = std::make_unique<Gui>(m_window);
#endif
            m_gui->getBackendRenderTarget()->setClearColor({200, 200, 200});
            m_windowOpen = true;
        }

        ~BackendWindowSDL() override
        {
            m_gui = nullptr; // Gui must be destroyed before destroying SDL window

#if TGUI_HAS_BACKEND_SDL_RENDERER
            if (m_renderer)
                SDL_DestroyRenderer(m_renderer);
#else
            if (m_glContext)
                SDL_GL_DeleteContext(m_glContext);
#endif
            if (m_window)
                SDL_DestroyWindow(m_window);

#if TGUI_HAS_BACKEND_SDL_RENDERER || TGUI_HAS_BACKEND_SDL_TTF_OPENGL3 || TGUI_HAS_BACKEND_SDL_TTF_GLES2
            TTF_Quit();
#endif
            SDL_Quit();
        }

        BackendGui* getGui() const override
        {
            return m_gui.get();
        }

        bool isOpen() const override
        {
            return m_windowOpen;
        }

        void close() override
        {
            m_windowOpen = false;

            // We send a quit event in case we are using the main loop of the gui.
            // We can't put a boolean in the mainLoop function of this class because it could be called directly on the gui object
            SDL_Event event;
            event.type = SDL_EVENT_QUIT;
            SDL_PushEvent(&event);
        }

        bool pollEvent(Event& event) override
        {
            SDL_Event eventSDL;
            if (SDL_PollEvent(&eventSDL) != 0)
                return m_gui->convertEvent(eventSDL, event);
            else // No new events
                return false;
        }

        void draw() override
        {
            m_gui->getBackendRenderTarget()->clearScreen();
            m_gui->draw();
#if TGUI_HAS_BACKEND_SDL_RENDERER
            SDL_RenderPresent(m_renderer);
#else
            SDL_GL_SwapWindow(m_window);
#endif
        }

        void mainLoop(Color clearColor) override
        {
            m_gui->mainLoop(clearColor);
        }

        void setIcon(const String& filename) override
        {
            Vector2u iconSize;
            auto pixelPtr = ImageLoader::loadFromFile((Filesystem::Path(getResourcePath()) / filename).asString(), iconSize);
            if (!pixelPtr)
                return;

#if SDL_MAJOR_VERSION >= 3
            SDL_Surface* icon = SDL_CreateSurfaceFrom(pixelPtr.get(), static_cast<int>(iconSize.x), static_cast<int>(iconSize.y),
                                                      4 * static_cast<int>(iconSize.x), SDL_PIXELFORMAT_RGBA32);
#else
            SDL_Surface* icon = SDL_CreateRGBSurfaceWithFormatFrom(pixelPtr.get(), static_cast<int>(iconSize.x), static_cast<int>(iconSize.y),
                                                                   32, 4 * static_cast<int>(iconSize.x), SDL_PIXELFORMAT_RGBA32);
#endif
            if (!icon)
                return;

            SDL_SetWindowIcon(m_window, icon);
            SDL_DestroySurface(icon);
        }

    private:
        SDL_Window* m_window = nullptr;
#if TGUI_HAS_BACKEND_SDL_RENDERER
        SDL_Renderer* m_renderer = nullptr;
#else
        SDL_GLContext m_glContext = nullptr;
#endif
        std::unique_ptr<Gui> m_gui;
        bool m_windowOpen = false;
    };

#elif TGUI_HAS_BACKEND_GLFW_OPENGL3 || TGUI_HAS_BACKEND_GLFW_GLES2

    class BackendWindowGLFW : public DefaultBackendWindow
    {
    public:
        BackendWindowGLFW(unsigned int width, unsigned int height, const String& title)
        {
            glfwInit();

#if TGUI_HAS_BACKEND_GLFW_OPENGL3
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // Required for macOS
#else
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

            m_window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), title.toStdString().c_str(), nullptr, nullptr);
            glfwMakeContextCurrent(m_window);

            glfwSwapInterval(1);

            m_gui = std::make_unique<Gui>(m_window);
            m_gui->getBackendRenderTarget()->setClearColor({200, 200, 200});

            // Set callback functions to push new events on a queue that is accessed by pollEvent
            glfwSetWindowUserPointer(m_window, this);
            glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused){
                auto backendWindow = static_cast<BackendWindowGLFW*>(glfwGetWindowUserPointer(window));
                const auto event = static_cast<Gui*>(backendWindow->getGui())->convertWindowFocusEvent(focused);
                if (event)
                    backendWindow->pushEvent(*event);
            });
            glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int fbWidth, int fbHeight){
                auto backendWindow = static_cast<BackendWindowGLFW*>(glfwGetWindowUserPointer(window));
                const auto event = static_cast<Gui*>(backendWindow->getGui())->convertSizeEvent(fbWidth, fbHeight);
                if (event)
                    backendWindow->pushEvent(*event);
            });
            glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int codepoint){
                auto backendWindow = static_cast<BackendWindowGLFW*>(glfwGetWindowUserPointer(window));
                const auto event = static_cast<Gui*>(backendWindow->getGui())->convertCharEvent(codepoint);
                if (event)
                    backendWindow->pushEvent(*event);
            });
            glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods){
                auto backendWindow = static_cast<BackendWindowGLFW*>(glfwGetWindowUserPointer(window));
                const auto event = static_cast<Gui*>(backendWindow->getGui())->convertKeyEvent(key, scancode, action, mods);
                if (event)
                    backendWindow->pushEvent(*event);
            });
            glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset){
                auto backendWindow = static_cast<BackendWindowGLFW*>(glfwGetWindowUserPointer(window));
                const auto event = static_cast<Gui*>(backendWindow->getGui())->convertScrollEvent(xoffset, yoffset);
                if (event)
                    backendWindow->pushEvent(*event);
            });
            glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos){
                auto backendWindow = static_cast<BackendWindowGLFW*>(glfwGetWindowUserPointer(window));
                const auto event = static_cast<Gui*>(backendWindow->getGui())->convertCursorPosEvent(xpos, ypos);
                if (event)
                    backendWindow->pushEvent(*event);
            });
            glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods){
                auto backendWindow = static_cast<BackendWindowGLFW*>(glfwGetWindowUserPointer(window));
                const auto event = static_cast<Gui*>(backendWindow->getGui())->convertMouseButtonEvent(button, action, mods);
                if (event)
                    backendWindow->pushEvent(*event);
            });
        }

        ~BackendWindowGLFW() override
        {
            m_gui = nullptr; // Gui must be destroyed before destroying GLFW window

            if (m_window)
                glfwDestroyWindow(m_window);

            glfwTerminate();
        }

        BackendGui* getGui() const override
        {
            return m_gui.get();
        }

        bool isOpen() const override
        {
            return !glfwWindowShouldClose(m_window);
        }

        void close() override
        {
            glfwSetWindowShouldClose(m_window, 1);
        }

        bool pollEvent(Event& event) override
        {
            if (m_events.empty())
                glfwPollEvents();

            if (!m_events.empty())
            {
                event = m_events.front();
                m_events.pop();
                return true;
            }

            // No new events
            return false;
        }

        void draw() override
        {
            m_gui->getBackendRenderTarget()->clearScreen();
            m_gui->draw();
            glfwSwapBuffers(m_window);
        }

        void mainLoop(Color clearColor) override
        {
            m_gui->mainLoop(clearColor);
        }

        void setIcon(const String& filename) override
        {
            Vector2u iconSize;
            auto pixelPtr = ImageLoader::loadFromFile((Filesystem::Path(getResourcePath()) / filename).asString(), iconSize);
            if (!pixelPtr)
                return;

            GLFWimage image;
            image.width = static_cast<int>(iconSize.x);
            image.height = static_cast<int>(iconSize.y);
            image.pixels = static_cast<unsigned char*>(pixelPtr.get());

            glfwSetWindowIcon(m_window, 1, &image);
        }

        void pushEvent(Event event)
        {
            m_events.emplace(event);
        }

    private:
        GLFWwindow* m_window = nullptr;
        std::unique_ptr<Gui> m_gui;
        std::queue<Event> m_events;
    };

#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<DefaultBackendWindow> DefaultBackendWindow::create(unsigned int width, unsigned int height, const String& title)
    {
#if TGUI_HAS_BACKEND_SFML_GRAPHICS || TGUI_HAS_BACKEND_SFML_OPENGL3
        return std::make_shared<BackendWindowSFML>(width, height, title);
#elif TGUI_HAS_BACKEND_SDL_RENDERER || TGUI_HAS_BACKEND_SDL_OPENGL3 || TGUI_HAS_BACKEND_SDL_GLES2 || TGUI_HAS_BACKEND_SDL_TTF_OPENGL3 || TGUI_HAS_BACKEND_SDL_TTF_GLES2
        return std::make_shared<BackendWindowSDL>(width, height, title);
#elif TGUI_HAS_BACKEND_GLFW_OPENGL3 || TGUI_HAS_BACKEND_GLFW_GLES2
        return std::make_shared<BackendWindowGLFW>(width, height, title);
#else
        TGUI_ASSERT(false, "DefaultBackendWindow can't be used when TGUI was build without a backend");
        (void)width;
        (void)height;
        (void)title;
        return nullptr;
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
