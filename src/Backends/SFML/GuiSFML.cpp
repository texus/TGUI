/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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
    static Event::KeyboardKey convertKeyCode(sf::Keyboard::Key key)
    {
        switch (key)
        {
        case sf::Keyboard::Key::A:          return Event::KeyboardKey::A;
        case sf::Keyboard::Key::B:          return Event::KeyboardKey::B;
        case sf::Keyboard::Key::C:          return Event::KeyboardKey::C;
        case sf::Keyboard::Key::D:          return Event::KeyboardKey::D;
        case sf::Keyboard::Key::E:          return Event::KeyboardKey::E;
        case sf::Keyboard::Key::F:          return Event::KeyboardKey::F;
        case sf::Keyboard::Key::G:          return Event::KeyboardKey::G;
        case sf::Keyboard::Key::H:          return Event::KeyboardKey::H;
        case sf::Keyboard::Key::I:          return Event::KeyboardKey::I;
        case sf::Keyboard::Key::J:          return Event::KeyboardKey::J;
        case sf::Keyboard::Key::K:          return Event::KeyboardKey::K;
        case sf::Keyboard::Key::L:          return Event::KeyboardKey::L;
        case sf::Keyboard::Key::M:          return Event::KeyboardKey::M;
        case sf::Keyboard::Key::N:          return Event::KeyboardKey::N;
        case sf::Keyboard::Key::O:          return Event::KeyboardKey::O;
        case sf::Keyboard::Key::P:          return Event::KeyboardKey::P;
        case sf::Keyboard::Key::Q:          return Event::KeyboardKey::Q;
        case sf::Keyboard::Key::R:          return Event::KeyboardKey::R;
        case sf::Keyboard::Key::S:          return Event::KeyboardKey::S;
        case sf::Keyboard::Key::T:          return Event::KeyboardKey::T;
        case sf::Keyboard::Key::U:          return Event::KeyboardKey::U;
        case sf::Keyboard::Key::V:          return Event::KeyboardKey::V;
        case sf::Keyboard::Key::W:          return Event::KeyboardKey::W;
        case sf::Keyboard::Key::X:          return Event::KeyboardKey::X;
        case sf::Keyboard::Key::Y:          return Event::KeyboardKey::Y;
        case sf::Keyboard::Key::Z:          return Event::KeyboardKey::Z;
        case sf::Keyboard::Key::Num0:       return Event::KeyboardKey::Num0;
        case sf::Keyboard::Key::Num1:       return Event::KeyboardKey::Num1;
        case sf::Keyboard::Key::Num2:       return Event::KeyboardKey::Num2;
        case sf::Keyboard::Key::Num3:       return Event::KeyboardKey::Num3;
        case sf::Keyboard::Key::Num4:       return Event::KeyboardKey::Num4;
        case sf::Keyboard::Key::Num5:       return Event::KeyboardKey::Num5;
        case sf::Keyboard::Key::Num6:       return Event::KeyboardKey::Num6;
        case sf::Keyboard::Key::Num7:       return Event::KeyboardKey::Num7;
        case sf::Keyboard::Key::Num8:       return Event::KeyboardKey::Num8;
        case sf::Keyboard::Key::Num9:       return Event::KeyboardKey::Num9;
        case sf::Keyboard::Key::Escape:     return Event::KeyboardKey::Escape;
        case sf::Keyboard::Key::LControl:   return Event::KeyboardKey::LControl;
        case sf::Keyboard::Key::LShift:     return Event::KeyboardKey::LShift;
        case sf::Keyboard::Key::LAlt:       return Event::KeyboardKey::LAlt;
        case sf::Keyboard::Key::LSystem:    return Event::KeyboardKey::LSystem;
        case sf::Keyboard::Key::RControl:   return Event::KeyboardKey::RControl;
        case sf::Keyboard::Key::RShift:     return Event::KeyboardKey::RShift;
        case sf::Keyboard::Key::RAlt:       return Event::KeyboardKey::RAlt;
        case sf::Keyboard::Key::RSystem:    return Event::KeyboardKey::RSystem;
        case sf::Keyboard::Key::Menu:       return Event::KeyboardKey::Menu;
        case sf::Keyboard::Key::LBracket:   return Event::KeyboardKey::LBracket;
        case sf::Keyboard::Key::RBracket:   return Event::KeyboardKey::RBracket;
        case sf::Keyboard::Key::Semicolon:  return Event::KeyboardKey::Semicolon;
        case sf::Keyboard::Key::Comma:      return Event::KeyboardKey::Comma;
        case sf::Keyboard::Key::Period:     return Event::KeyboardKey::Period;
        case sf::Keyboard::Key::Quote:      return Event::KeyboardKey::Quote;
        case sf::Keyboard::Key::Slash:      return Event::KeyboardKey::Slash;
        case sf::Keyboard::Key::Backslash:  return Event::KeyboardKey::Backslash;
        case sf::Keyboard::Key::Equal:      return Event::KeyboardKey::Equal;
        case sf::Keyboard::Key::Hyphen:     return Event::KeyboardKey::Minus;
        case sf::Keyboard::Key::Space:      return Event::KeyboardKey::Space;
        case sf::Keyboard::Key::Enter:      return Event::KeyboardKey::Enter;
        case sf::Keyboard::Key::Backspace:  return Event::KeyboardKey::Backspace;
        case sf::Keyboard::Key::Tab:        return Event::KeyboardKey::Tab;
        case sf::Keyboard::Key::PageUp:     return Event::KeyboardKey::PageUp;
        case sf::Keyboard::Key::PageDown:   return Event::KeyboardKey::PageDown;
        case sf::Keyboard::Key::End:        return Event::KeyboardKey::End;
        case sf::Keyboard::Key::Home:       return Event::KeyboardKey::Home;
        case sf::Keyboard::Key::Insert:     return Event::KeyboardKey::Insert;
        case sf::Keyboard::Key::Delete:     return Event::KeyboardKey::Delete;
        case sf::Keyboard::Key::Add:        return Event::KeyboardKey::Add;
        case sf::Keyboard::Key::Subtract:   return Event::KeyboardKey::Subtract;
        case sf::Keyboard::Key::Multiply:   return Event::KeyboardKey::Multiply;
        case sf::Keyboard::Key::Divide:     return Event::KeyboardKey::Divide;
        case sf::Keyboard::Key::Left:       return Event::KeyboardKey::Left;
        case sf::Keyboard::Key::Right:      return Event::KeyboardKey::Right;
        case sf::Keyboard::Key::Up:         return Event::KeyboardKey::Up;
        case sf::Keyboard::Key::Down:       return Event::KeyboardKey::Down;
        case sf::Keyboard::Key::Numpad0:    return Event::KeyboardKey::Numpad0;
        case sf::Keyboard::Key::Numpad1:    return Event::KeyboardKey::Numpad1;
        case sf::Keyboard::Key::Numpad2:    return Event::KeyboardKey::Numpad2;
        case sf::Keyboard::Key::Numpad3:    return Event::KeyboardKey::Numpad3;
        case sf::Keyboard::Key::Numpad4:    return Event::KeyboardKey::Numpad4;
        case sf::Keyboard::Key::Numpad5:    return Event::KeyboardKey::Numpad5;
        case sf::Keyboard::Key::Numpad6:    return Event::KeyboardKey::Numpad6;
        case sf::Keyboard::Key::Numpad7:    return Event::KeyboardKey::Numpad7;
        case sf::Keyboard::Key::Numpad8:    return Event::KeyboardKey::Numpad8;
        case sf::Keyboard::Key::Numpad9:    return Event::KeyboardKey::Numpad9;
        case sf::Keyboard::Key::F1:         return Event::KeyboardKey::F1;
        case sf::Keyboard::Key::F2:         return Event::KeyboardKey::F2;
        case sf::Keyboard::Key::F3:         return Event::KeyboardKey::F3;
        case sf::Keyboard::Key::F4:         return Event::KeyboardKey::F4;
        case sf::Keyboard::Key::F5:         return Event::KeyboardKey::F5;
        case sf::Keyboard::Key::F6:         return Event::KeyboardKey::F6;
        case sf::Keyboard::Key::F7:         return Event::KeyboardKey::F7;
        case sf::Keyboard::Key::F8:         return Event::KeyboardKey::F8;
        case sf::Keyboard::Key::F9:         return Event::KeyboardKey::F9;
        case sf::Keyboard::Key::F10:        return Event::KeyboardKey::F10;
        case sf::Keyboard::Key::F11:        return Event::KeyboardKey::F11;
        case sf::Keyboard::Key::F12:        return Event::KeyboardKey::F12;
        case sf::Keyboard::Key::F13:        return Event::KeyboardKey::F13;
        case sf::Keyboard::Key::F14:        return Event::KeyboardKey::F14;
        case sf::Keyboard::Key::F15:        return Event::KeyboardKey::F15;
        case sf::Keyboard::Key::Pause:      return Event::KeyboardKey::Pause;
        default: // We don't process the other keys
            return Event::KeyboardKey::Unknown;
        }
    }

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
        if (!convertEvent(sfmlEvent, event))
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

    bool GuiSFML::convertEvent(const sf::Event& eventSFML, Event& eventTGUI)
    {
        switch (eventSFML.type)
        {
            case sf::Event::LostFocus:
            {
                eventTGUI.type = Event::Type::LostFocus;
                return true;
            }
            case sf::Event::GainedFocus:
            {
                eventTGUI.type = Event::Type::GainedFocus;
                return true;
            }
            case sf::Event::Resized:
            {
                eventTGUI.type = Event::Type::Resized;
                eventTGUI.size.width = eventSFML.size.width;
                eventTGUI.size.height = eventSFML.size.height;
                return true;
            }
            case sf::Event::Closed:
            {
                eventTGUI.type = Event::Type::Closed;
                return true;
            }
            case sf::Event::TextEntered:
            {
                eventTGUI.type = Event::Type::TextEntered;
                eventTGUI.text.unicode = eventSFML.text.unicode;
                return true;
            }
            case sf::Event::KeyPressed:
            {
                const Event::KeyboardKey code = convertKeyCode(eventSFML.key.code);
                if (code == Event::KeyboardKey::Unknown)
                    return false; // This key isn't handled by TGUI

                eventTGUI.type = Event::Type::KeyPressed;
                eventTGUI.key.code = code;
                eventTGUI.key.alt = eventSFML.key.alt;
                eventTGUI.key.control = eventSFML.key.control;
                eventTGUI.key.shift = eventSFML.key.shift;
                eventTGUI.key.system = eventSFML.key.system;
                return true;
            }
            case sf::Event::MouseWheelScrolled:
            {
                if (eventSFML.mouseWheelScroll.wheel != sf::Mouse::Wheel::VerticalWheel)
                    return false; // TGUI only handles the vertical mouse wheel

                eventTGUI.type = Event::Type::MouseWheelScrolled;
                eventTGUI.mouseWheel.delta = eventSFML.mouseWheelScroll.delta;
                eventTGUI.mouseWheel.x = eventSFML.mouseWheelScroll.x;
                eventTGUI.mouseWheel.y = eventSFML.mouseWheelScroll.y;
                return true;
            }
            case sf::Event::MouseButtonPressed:
            case sf::Event::MouseButtonReleased:
            {
                switch (eventSFML.mouseButton.button)
                {
                case sf::Mouse::Button::Left:
                    eventTGUI.mouseButton.button = Event::MouseButton::Left;
                    break;
                case sf::Mouse::Button::Middle:
                    eventTGUI.mouseButton.button = Event::MouseButton::Middle;
                    break;
                case sf::Mouse::Button::Right:
                    eventTGUI.mouseButton.button = Event::MouseButton::Right;
                    break;
                default: // This mouse button isn't handled by TGUI
                    return false;
                }

                if (eventSFML.type == sf::Event::MouseButtonPressed)
                    eventTGUI.type = Event::Type::MouseButtonPressed;
                else
                    eventTGUI.type = Event::Type::MouseButtonReleased;

                eventTGUI.mouseButton.x = eventSFML.mouseButton.x;
                eventTGUI.mouseButton.y = eventSFML.mouseButton.y;
                return true;
            }
            case sf::Event::MouseMoved:
            {
                eventTGUI.type = Event::Type::MouseMoved;
                eventTGUI.mouseMove.x = eventSFML.mouseMove.x;
                eventTGUI.mouseMove.y = eventSFML.mouseMove.y;
                return true;
            }
            case sf::Event::TouchMoved:
            {
                if (eventSFML.touch.finger != 0)
                    return false; // Only the first finger is handled

                // Simulate a MouseMoved event
                eventTGUI.type = Event::Type::MouseMoved;
                eventTGUI.mouseMove.x = eventSFML.touch.x;
                eventTGUI.mouseMove.y = eventSFML.touch.y;
                return true;
            }
            case sf::Event::TouchBegan:
            case sf::Event::TouchEnded:
            {
                if (eventSFML.touch.finger != 0)
                    return false; // Only the first finger is handled

                // Simulate a MouseButtonPressed or MouseButtonReleased event
                if (eventSFML.type == sf::Event::TouchBegan)
                    eventTGUI.type = Event::Type::MouseButtonPressed;
                else
                    eventTGUI.type = Event::Type::MouseButtonReleased;

                eventTGUI.mouseButton.button = Event::MouseButton::Left;
                eventTGUI.mouseButton.x = eventSFML.touch.x;
                eventTGUI.mouseButton.y = eventSFML.touch.y;
                return true;
            }
            default: // This event is not handled by TGUI
                return false;
        }
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
        m_renderTarget->setView(m_view.getRect(), m_viewport.getRect(),
                                {static_cast<float>(target->getSize().x), static_cast<float>(target->getSize().y)});

        GuiBase::updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
