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


#include <TGUI/DefaultBackendWindow.hpp>
#include <TGUI/GuiBase.hpp>
#include <TGUI/Event.hpp>

#if TGUI_HAS_BACKEND_SFML
    #include <TGUI/Backends/SFML.hpp>
#elif TGUI_HAS_BACKEND_SDL
    #include <TGUI/Backends/SDL.hpp>

    #include <SDL.h>
    #include <SDL_image.h>
    #include <SDL_ttf.h>

    #if TGUI_USE_GLES
        #include <SDL_opengles2.h>
    #else
        #include <SDL_opengl.h>
    #endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_HAS_BACKEND_SFML

    class BackendWindowSFML : public DefaultBackendWindow
    {
    public:
        BackendWindowSFML(unsigned int width, unsigned int height, const String& title) :
            m_window({width, height}, title.toStdString()),
            m_gui(std::make_unique<GuiSFML>(m_window))
        {
        }

        GuiBase* getGui() const
        {
            return m_gui.get();
        }

        bool isOpen() const
        {
            return m_window.isOpen();
        }

        void close()
        {
            m_window.close();
        }

        bool pollEvent(Event& event) override
        {
            sf::Event eventSFML;
            if (m_window.pollEvent(eventSFML))
                return BackendSFML::convertEvent(eventSFML, event);
            else // No new events
                return false;
        }

        void draw() override
        {
            m_window.clear({200, 200, 200});
            m_gui->draw();
            m_window.display();
        }

        void mainLoop()
        {
            m_gui->mainLoop();
        }

        void setIcon(const String& filename)
        {
            sf::Image icon;
            if (icon.loadFromFile(sf::String(filename)))
                m_window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
        }

    private:
        sf::RenderWindow m_window;
        std::unique_ptr<GuiSFML> m_gui;
    };

#elif TGUI_HAS_BACKEND_SDL

    class BackendWindowSDL : public DefaultBackendWindow
    {
    public:
        BackendWindowSDL(unsigned int width, unsigned int height, const String& title)
        {
            SDL_Init(SDL_INIT_VIDEO);

            IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
            TTF_Init();

#if TGUI_USE_GLES
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#else
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
            m_window = SDL_CreateWindow(title.toStdString().c_str(),
                                      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                      width, height,
                                      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
            m_glContext = SDL_GL_CreateContext(m_window);

            glClearColor(0.8f, 0.8f, 0.8f, 1.f);

            m_gui = std::make_unique<GuiSDL>(m_window);
            m_windowOpen = true;
        }

        ~BackendWindowSDL()
        {
            m_gui = nullptr; // Gui must be destroyed before destroying SDL window

            if (m_glContext)
                SDL_GL_DeleteContext(m_glContext);

            if (m_window)
                SDL_DestroyWindow(m_window);

            TTF_Quit();
            IMG_Quit();

            SDL_Quit();
        }

        GuiBase* getGui() const
        {
            return m_gui.get();
        }

        bool isOpen() const
        {
            return m_windowOpen;
        }

        void close()
        {
            m_windowOpen = false;
        }

        bool pollEvent(Event& event) override
        {
            SDL_Event eventSDL;
            if (SDL_PollEvent(&eventSDL) != 0)
                return BackendSDL::convertEvent(eventSDL, event);
            else // No new events
                return false;
        }

        void draw() override
        {
            glClear(GL_COLOR_BUFFER_BIT);
            m_gui->draw();
            SDL_GL_SwapWindow(m_window);
        }

        void mainLoop()
        {
            m_gui->mainLoop();
        }

        void setIcon(const String& filename)
        {
            SDL_Surface* icon = IMG_Load(filename.toStdString().c_str());
            if (!icon)
                return;

            SDL_SetWindowIcon(m_window, icon);
            SDL_FreeSurface(icon);
        }

    private:
        SDL_Window* m_window = nullptr;
        SDL_GLContext m_glContext = nullptr;
        std::unique_ptr<GuiSDL> m_gui;
        bool m_windowOpen = false;
    };

#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<DefaultBackendWindow> DefaultBackendWindow::create(unsigned int width, unsigned int height, const String& title)
    {
#if TGUI_HAS_BACKEND_SFML
        return std::make_shared<BackendWindowSFML>(width, height, title);
#elif TGUI_HAS_BACKEND_SDL
        return std::make_shared<BackendWindowSDL>(width, height, title);
#else
        TGUI_ASSERT(false, "DefaultBackendWindow can't be used when TGUI was build without a backend");
        return nullptr;
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
