/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/Backends/SFML/GuiSFML.hpp>
#include <TGUI/Backend.hpp>
#include <TGUI/ToolTip.hpp>
#include <TGUI/Timer.hpp>
#include <thread>

#include <TGUI/Backends/SFML/BackendSFML.hpp>
#include <TGUI/Backends/SFML/BackendRenderTargetSFML.hpp>

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GuiSFML::GuiSFML()
    {
        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GuiSFML::GuiSFML(sf::RenderTarget& target)
    {
        init();
        setTarget(target);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiSFML::setTarget(sf::RenderTarget& target)
    {
        std::shared_ptr<BackendSFML> backend = std::dynamic_pointer_cast<BackendSFML>(getBackend());
        m_renderTarget = backend->createGuiRenderTarget(this, target);

        updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::RenderTarget* GuiSFML::getTarget() const
    {
        if (m_renderTarget)
            return m_renderTarget->getTarget();
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool GuiSFML::handleEvent(sf::Event sfmlEvent)
    {
        Event event;
        if (!BackendSFML::convertEvent(sfmlEvent, event))
            return false; // We don't process this type of event

        if ((event.type == Event::Type::MouseButtonPressed) && (sfmlEvent.type == sf::Event::TouchBegan))
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

    void GuiSFML::mainLoop()
    {
        sf::RenderWindow* window = dynamic_cast<sf::RenderWindow*>(getTarget());
        if (!window)
            return;

        // Helper function that calculates the amount of time to sleep, which is 10ms unless a timer will expire before this time
        const auto getTimerWakeUpTime = []{
            Optional<Duration> duration = Timer::getNextScheduledTime();
            if (duration && (*duration < std::chrono::milliseconds(10)))
                return *duration;
            else
                return Duration{std::chrono::milliseconds(10)};
        };

        setDrawingUpdatesTime(false);

        sf::Event event;
        bool refreshRequired = true;
        std::chrono::steady_clock::time_point lastRenderTime;
        while (window->isOpen())
        {
            bool eventProcessed = false;
            while (true)
            {
                while (window->pollEvent(event))
                {
                    if (handleEvent(event))
                        eventProcessed = true;

                    if (event.type == sf::Event::Closed)
                    {
                        window->close();
                        eventProcessed = true;
                    }
                    else if (event.type == sf::Event::Resized)
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

            window->clear({240, 240, 240});
            draw();
            window->display();

            refreshRequired = false;
            lastRenderTime = std::chrono::steady_clock::now(); // Don't use timePointNow to provide enough rest on low-end hardware
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiSFML::draw()
    {
        if (m_drawUpdatesTime)
            updateTime();

        TGUI_ASSERT(m_renderTarget != nullptr, "GuiSFML must be given an sf::RenderTarget (either at construction or via setTarget function) before calling draw()");
        m_renderTarget->drawGui(m_container);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiSFML::init()
    {
        if (!isBackendSet())
        {
            setBackend(std::make_shared<BackendSFML>());
            getBackend()->setDestroyOnLastGuiDetatch(true);
        }

        GuiBase::init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void GuiSFML::updateContainerSize()
    {
        TGUI_ASSERT(m_renderTarget != nullptr, "GuiSFML must be given an sf::RenderTarget (either at construction or via setTarget function) before updateContainerSize() is called");

        const auto& target = getTarget();
        m_viewport.updateParentSize({static_cast<float>(target->getSize().x), static_cast<float>(target->getSize().y)});
        m_view.updateParentSize({m_viewport.getWidth(), m_viewport.getHeight()});
        m_renderTarget->setView(m_view.getRect(), m_viewport.getRect());
        m_container->setSize(Vector2f{m_view.getWidth(), m_view.getHeight()});

        GuiBase::updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
