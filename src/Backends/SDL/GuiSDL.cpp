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

#include <TGUI/Backends/SDL/GuiSDL.hpp>
#include <TGUI/Backend.hpp>
#include <TGUI/ToolTip.hpp>
#include <TGUI/Timer.hpp>
#include <TGUI/OpenGL.hpp>

#include <TGUI/Backends/SDL/BackendSDL.hpp>
#include <TGUI/Backends/SDL/BackendRenderTargetSDL.hpp>

#include <thread>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GuiSDL::GuiSDL()
    {
        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GuiSDL::GuiSDL(SDL_Window* window)
    {
        init();
        setWindow(window);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiSDL::setWindow(SDL_Window* window)
    {
        std::shared_ptr<BackendSDL> backend = std::dynamic_pointer_cast<BackendSDL>(getBackend());
        m_renderTarget = backend->createGuiRenderTarget(this, window);

        updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool GuiSDL::handleEvent(const SDL_Event& sdlEvent)
    {
        Event event;
        if (!BackendSDL::convertEvent(sdlEvent, event))
            return false; // We don't process this type of event

        // SDL doesn't include the mouse position in mouse wheel events, so we add the last known position ourself
        if (event.type == Event::Type::MouseWheelScrolled)
        {
            event.mouseWheel.x = static_cast<int>(m_lastMousePos.x);
            event.mouseWheel.y = static_cast<int>(m_lastMousePos.y);
        }

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wold-style-cast"  // SDL_TOUCH_MOUSEID is a define for "((Uint32)-1)"
#endif
        if ((sdlEvent.type == SDL_MOUSEBUTTONDOWN) && (sdlEvent.button.which == SDL_TOUCH_MOUSEID))
#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif
        {
            // For touches, always send a mouse move event before the mouse press,
            // because widgets may assume that the mouse had to move to the clicked location first
            Event mouseMoveEvent;
            mouseMoveEvent.type = Event::Type::MouseMoved;
            mouseMoveEvent.mouseMove.x = event.mouseButton.x;
            mouseMoveEvent.mouseMove.y = event.mouseButton.y;
            handleEvent(mouseMoveEvent);
        }

        return handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiSDL::draw()
    {
        if (m_drawUpdatesTime)
            updateTime();

        TGUI_ASSERT(m_renderTarget != nullptr, "GuiSDL must be given an SDL_Window (either at construction or via setWindow function) before calling draw()");
        m_renderTarget->drawGui(m_container);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiSDL::mainLoop()
    {
        TGUI_ASSERT(m_renderTarget != nullptr, "GuiSDL must be given an SDL_Window (either at construction or via setWindow function) before mainLoop() is called");

        // Helper function that calculates the amount of time to sleep, which is 10ms unless a timer will expire before this time
        const auto getTimerWakeUpTime = []{
            Optional<Duration> duration = Timer::getNextScheduledTime();
            if (duration && (*duration < std::chrono::milliseconds(10)))
                return *duration;
            else
                return Duration{std::chrono::milliseconds(10)};
        };

        setDrawingUpdatesTime(false);

        SDL_Window* window = m_renderTarget->getWindow();
        bool quit = false;
        bool refreshRequired = true;
        std::chrono::steady_clock::time_point lastRenderTime;
        while (!quit)
        {
            bool eventProcessed = false;
            while (true)
            {
                // We could use SDL_WaitEventTimeout instead of SDL_PollEvent and custom timing in the future, but we would
                // have to be careful not to keep calling the function with the same timeout while events keep coming.
                SDL_Event event;
                while (SDL_PollEvent(&event) != 0)
                {
                    if (handleEvent(event))
                        eventProcessed = true;

                    if (event.type == SDL_QUIT)
                    {
                        quit = true;
                        eventProcessed = true;
                    }
                    else if ((event.type == SDL_WINDOWEVENT) && (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED))
                    {
                        eventProcessed = true;
                    }
                }

                if (updateTime())
                    break;

                if (eventProcessed || refreshRequired)
                    break;

                std::this_thread::sleep_for(std::chrono::nanoseconds(getTimerWakeUpTime()));
            }

            refreshRequired = true;

            // Don't try to render too often, even when the screen is changing (e.g. during animation)
            const auto timePointNow = std::chrono::steady_clock::now();
            const auto timePointNextAllowed = lastRenderTime + std::chrono::milliseconds(35);
            if (timePointNextAllowed > timePointNow)
            {
                const auto timerWakeUpTime = getTimerWakeUpTime();
                if (timePointNextAllowed - timePointNow < timerWakeUpTime)
                    std::this_thread::sleep_for(timePointNextAllowed - timePointNow);
                else
                    std::this_thread::sleep_for(std::chrono::nanoseconds(timerWakeUpTime));

                continue;
            }

            glClear(GL_COLOR_BUFFER_BIT);
            draw();
            SDL_GL_SwapWindow(window);

            refreshRequired = false;
            lastRenderTime = std::chrono::steady_clock::now(); // Don't use timePointNow to provide enough rest on low-end hardware
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiSDL::init()
    {
        if (!isBackendSet())
        {
            setBackend(std::make_shared<BackendSDL>());
            getBackend()->setDestroyOnLastGuiDetatch(true);
        }

        GuiBase::init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiSDL::updateContainerSize()
    {
        TGUI_ASSERT(m_renderTarget != nullptr, "GuiSDL must be given an SDL_Window (either at construction or via setWindow function) before updateContainerSize() is called");

        int windowWidth;
        int windowHeight;
        SDL_GetWindowSize(m_renderTarget->getWindow(), &windowWidth, &windowHeight);
        m_viewport.updateParentSize({static_cast<float>(windowWidth), static_cast<float>(windowHeight)});
        m_view.updateParentSize({m_viewport.getWidth(), m_viewport.getHeight()});
        m_renderTarget->setView(m_view.getRect(), m_viewport.getRect());
        m_container->setSize(Vector2f{m_view.getWidth(), m_view.getHeight()});

        GuiBase::updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
