/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2026 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/Backend/Window/SFML/BackendGuiSFML.hpp>
#include <TGUI/Backend/Window/SFML/BackendSFML.hpp>

#include <TGUI/ToolTip.hpp>
#include <TGUI/Timer.hpp>

#ifdef TGUI_SYSTEM_WINDOWS
    #include <TGUI/WindowsIMM.hpp>
#endif

#include <thread>

namespace
{
    TGUI_NODISCARD tgui::Event::KeyboardKey convertKeyCode(sf::Keyboard::Key key)
    {
        switch (key)
        {
        case sf::Keyboard::Key::A:          return tgui::Event::KeyboardKey::A;
        case sf::Keyboard::Key::B:          return tgui::Event::KeyboardKey::B;
        case sf::Keyboard::Key::C:          return tgui::Event::KeyboardKey::C;
        case sf::Keyboard::Key::D:          return tgui::Event::KeyboardKey::D;
        case sf::Keyboard::Key::E:          return tgui::Event::KeyboardKey::E;
        case sf::Keyboard::Key::F:          return tgui::Event::KeyboardKey::F;
        case sf::Keyboard::Key::G:          return tgui::Event::KeyboardKey::G;
        case sf::Keyboard::Key::H:          return tgui::Event::KeyboardKey::H;
        case sf::Keyboard::Key::I:          return tgui::Event::KeyboardKey::I;
        case sf::Keyboard::Key::J:          return tgui::Event::KeyboardKey::J;
        case sf::Keyboard::Key::K:          return tgui::Event::KeyboardKey::K;
        case sf::Keyboard::Key::L:          return tgui::Event::KeyboardKey::L;
        case sf::Keyboard::Key::M:          return tgui::Event::KeyboardKey::M;
        case sf::Keyboard::Key::N:          return tgui::Event::KeyboardKey::N;
        case sf::Keyboard::Key::O:          return tgui::Event::KeyboardKey::O;
        case sf::Keyboard::Key::P:          return tgui::Event::KeyboardKey::P;
        case sf::Keyboard::Key::Q:          return tgui::Event::KeyboardKey::Q;
        case sf::Keyboard::Key::R:          return tgui::Event::KeyboardKey::R;
        case sf::Keyboard::Key::S:          return tgui::Event::KeyboardKey::S;
        case sf::Keyboard::Key::T:          return tgui::Event::KeyboardKey::T;
        case sf::Keyboard::Key::U:          return tgui::Event::KeyboardKey::U;
        case sf::Keyboard::Key::V:          return tgui::Event::KeyboardKey::V;
        case sf::Keyboard::Key::W:          return tgui::Event::KeyboardKey::W;
        case sf::Keyboard::Key::X:          return tgui::Event::KeyboardKey::X;
        case sf::Keyboard::Key::Y:          return tgui::Event::KeyboardKey::Y;
        case sf::Keyboard::Key::Z:          return tgui::Event::KeyboardKey::Z;
        case sf::Keyboard::Key::Num0:       return tgui::Event::KeyboardKey::Num0;
        case sf::Keyboard::Key::Num1:       return tgui::Event::KeyboardKey::Num1;
        case sf::Keyboard::Key::Num2:       return tgui::Event::KeyboardKey::Num2;
        case sf::Keyboard::Key::Num3:       return tgui::Event::KeyboardKey::Num3;
        case sf::Keyboard::Key::Num4:       return tgui::Event::KeyboardKey::Num4;
        case sf::Keyboard::Key::Num5:       return tgui::Event::KeyboardKey::Num5;
        case sf::Keyboard::Key::Num6:       return tgui::Event::KeyboardKey::Num6;
        case sf::Keyboard::Key::Num7:       return tgui::Event::KeyboardKey::Num7;
        case sf::Keyboard::Key::Num8:       return tgui::Event::KeyboardKey::Num8;
        case sf::Keyboard::Key::Num9:       return tgui::Event::KeyboardKey::Num9;
        case sf::Keyboard::Key::Escape:     return tgui::Event::KeyboardKey::Escape;
        case sf::Keyboard::Key::LControl:   return tgui::Event::KeyboardKey::LControl;
        case sf::Keyboard::Key::LShift:     return tgui::Event::KeyboardKey::LShift;
        case sf::Keyboard::Key::LAlt:       return tgui::Event::KeyboardKey::LAlt;
        case sf::Keyboard::Key::LSystem:    return tgui::Event::KeyboardKey::LSystem;
        case sf::Keyboard::Key::RControl:   return tgui::Event::KeyboardKey::RControl;
        case sf::Keyboard::Key::RShift:     return tgui::Event::KeyboardKey::RShift;
        case sf::Keyboard::Key::RAlt:       return tgui::Event::KeyboardKey::RAlt;
        case sf::Keyboard::Key::RSystem:    return tgui::Event::KeyboardKey::RSystem;
        case sf::Keyboard::Key::Menu:       return tgui::Event::KeyboardKey::Menu;
        case sf::Keyboard::Key::LBracket:   return tgui::Event::KeyboardKey::LBracket;
        case sf::Keyboard::Key::RBracket:   return tgui::Event::KeyboardKey::RBracket;
        case sf::Keyboard::Key::Semicolon:  return tgui::Event::KeyboardKey::Semicolon;
        case sf::Keyboard::Key::Comma:      return tgui::Event::KeyboardKey::Comma;
        case sf::Keyboard::Key::Period:     return tgui::Event::KeyboardKey::Period;
#if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 6
        case sf::Keyboard::Key::Quote:      return tgui::Event::KeyboardKey::Quote;
#else
        case sf::Keyboard::Key::Apostrophe: return tgui::Event::KeyboardKey::Quote;
#endif
        case sf::Keyboard::Key::Slash:      return tgui::Event::KeyboardKey::Slash;
        case sf::Keyboard::Key::Backslash:  return tgui::Event::KeyboardKey::Backslash;
        case sf::Keyboard::Key::Equal:      return tgui::Event::KeyboardKey::Equal;
        case sf::Keyboard::Key::Hyphen:     return tgui::Event::KeyboardKey::Minus;
        case sf::Keyboard::Key::Space:      return tgui::Event::KeyboardKey::Space;
        case sf::Keyboard::Key::Enter:      return tgui::Event::KeyboardKey::Enter;
        case sf::Keyboard::Key::Backspace:  return tgui::Event::KeyboardKey::Backspace;
        case sf::Keyboard::Key::Tab:        return tgui::Event::KeyboardKey::Tab;
        case sf::Keyboard::Key::PageUp:     return tgui::Event::KeyboardKey::PageUp;
        case sf::Keyboard::Key::PageDown:   return tgui::Event::KeyboardKey::PageDown;
        case sf::Keyboard::Key::End:        return tgui::Event::KeyboardKey::End;
        case sf::Keyboard::Key::Home:       return tgui::Event::KeyboardKey::Home;
        case sf::Keyboard::Key::Insert:     return tgui::Event::KeyboardKey::Insert;
        case sf::Keyboard::Key::Delete:     return tgui::Event::KeyboardKey::Delete;
        case sf::Keyboard::Key::Add:        return tgui::Event::KeyboardKey::Add;
        case sf::Keyboard::Key::Subtract:   return tgui::Event::KeyboardKey::Subtract;
        case sf::Keyboard::Key::Multiply:   return tgui::Event::KeyboardKey::Multiply;
        case sf::Keyboard::Key::Divide:     return tgui::Event::KeyboardKey::Divide;
        case sf::Keyboard::Key::Left:       return tgui::Event::KeyboardKey::Left;
        case sf::Keyboard::Key::Right:      return tgui::Event::KeyboardKey::Right;
        case sf::Keyboard::Key::Up:         return tgui::Event::KeyboardKey::Up;
        case sf::Keyboard::Key::Down:       return tgui::Event::KeyboardKey::Down;
        case sf::Keyboard::Key::Numpad0:    return tgui::Event::KeyboardKey::Numpad0;
        case sf::Keyboard::Key::Numpad1:    return tgui::Event::KeyboardKey::Numpad1;
        case sf::Keyboard::Key::Numpad2:    return tgui::Event::KeyboardKey::Numpad2;
        case sf::Keyboard::Key::Numpad3:    return tgui::Event::KeyboardKey::Numpad3;
        case sf::Keyboard::Key::Numpad4:    return tgui::Event::KeyboardKey::Numpad4;
        case sf::Keyboard::Key::Numpad5:    return tgui::Event::KeyboardKey::Numpad5;
        case sf::Keyboard::Key::Numpad6:    return tgui::Event::KeyboardKey::Numpad6;
        case sf::Keyboard::Key::Numpad7:    return tgui::Event::KeyboardKey::Numpad7;
        case sf::Keyboard::Key::Numpad8:    return tgui::Event::KeyboardKey::Numpad8;
        case sf::Keyboard::Key::Numpad9:    return tgui::Event::KeyboardKey::Numpad9;
        case sf::Keyboard::Key::F1:         return tgui::Event::KeyboardKey::F1;
        case sf::Keyboard::Key::F2:         return tgui::Event::KeyboardKey::F2;
        case sf::Keyboard::Key::F3:         return tgui::Event::KeyboardKey::F3;
        case sf::Keyboard::Key::F4:         return tgui::Event::KeyboardKey::F4;
        case sf::Keyboard::Key::F5:         return tgui::Event::KeyboardKey::F5;
        case sf::Keyboard::Key::F6:         return tgui::Event::KeyboardKey::F6;
        case sf::Keyboard::Key::F7:         return tgui::Event::KeyboardKey::F7;
        case sf::Keyboard::Key::F8:         return tgui::Event::KeyboardKey::F8;
        case sf::Keyboard::Key::F9:         return tgui::Event::KeyboardKey::F9;
        case sf::Keyboard::Key::F10:        return tgui::Event::KeyboardKey::F10;
        case sf::Keyboard::Key::F11:        return tgui::Event::KeyboardKey::F11;
        case sf::Keyboard::Key::F12:        return tgui::Event::KeyboardKey::F12;
        case sf::Keyboard::Key::F13:        return tgui::Event::KeyboardKey::F13;
        case sf::Keyboard::Key::F14:        return tgui::Event::KeyboardKey::F14;
        case sf::Keyboard::Key::F15:        return tgui::Event::KeyboardKey::F15;
        case sf::Keyboard::Key::Pause:      return tgui::Event::KeyboardKey::Pause;
        default: // We don't process the other keys
            return tgui::Event::KeyboardKey::Unknown;
        }
    }
} // anonymous namespace

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendGuiSFML::BackendGuiSFML()
    {
#ifdef TGUI_SYSTEM_WINDOWS
        WindowsIMM::initialize();
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendGuiSFML::~BackendGuiSFML()
    {
#ifdef TGUI_SYSTEM_WINDOWS
        WindowsIMM::release();
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGuiSFML::handleEvent(sf::Event sfmlEvent)
    {
        // Convert the event to our own type so that we can process it in a backend-independent way afterwards
        Event event;
        if (!convertEvent(sfmlEvent, event))
            return false; // We don't process this type of event

        return handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGuiSFML::mainLoop(Color clearColor)
    {
        if (!m_window)
            return;

        m_backendRenderTarget->setClearColor(clearColor);

        // Helper function that calculates the amount of time to sleep, which is 10ms unless a timer will expire before this time
        const auto getTimerWakeUpTime = []{
            Optional<Duration> duration = Timer::getNextScheduledTime();
            if (duration && (*duration < std::chrono::milliseconds(10)))
                return *duration;
            return Duration{std::chrono::milliseconds(10)};
        };

        setDrawingUpdatesTime(false);

        bool refreshRequired = true;
        std::chrono::steady_clock::time_point lastRenderTime;
        bool windowOpen = m_window->isOpen();
        while (m_window->isOpen()) // Don't just check windowOpen, user code can also call window.close()
        {
            bool eventProcessed = false;
            while (true)
            {
#if SFML_VERSION_MAJOR >= 3
                while (const auto event = m_window->pollEvent())
                {
                    if (handleEvent(*event))
                        eventProcessed = true;

                    if (event->is<sf::Event::Closed>())
                    {
                        // We don't call m_window->close() as it would destroy the OpenGL context, which will cause messages to be
                        // printed in the terminal later when we try to destroy our backend renderer (which tries to clean up OpenGL resources).
                        // The close function will be called by the window destructor.
                        windowOpen = false;
                        eventProcessed = true;
                    }
                    else if (event->is<sf::Event::Resized>()
                          || event->is<sf::Event::MouseEntered>() || event->is<sf::Event::MouseLeft>()
                          || event->is<sf::Event::FocusGained>() || event->is<sf::Event::FocusLost>())
                    {
                        eventProcessed = true;
                    }
                }
#else
                sf::Event event;
                while (m_window->pollEvent(event))
                {
                    if (handleEvent(event))
                        eventProcessed = true;

                    if (event.type == sf::Event::Closed)
                    {
                        // We don't call m_window->close() as it would destroy the OpenGL context, which will cause messages to be
                        // printed in the terminal later when we try to destroy our backend renderer (which tries to clean up OpenGL resources).
                        // The close function will be called by the window destructor.
                        windowOpen = false;
                        eventProcessed = true;
                    }
                    else if ((event.type == sf::Event::Resized)
                          || (event.type == sf::Event::MouseEntered) || (event.type == sf::Event::MouseLeft)
                          || (event.type == sf::Event::GainedFocus) || (event.type == sf::Event::LostFocus))
                    {
                        eventProcessed = true;
                    }
                }
#endif
                if (updateTime())
                    break;

                if (eventProcessed || refreshRequired)
                    break;

                std::this_thread::sleep_for(std::chrono::nanoseconds(getTimerWakeUpTime()));
            }

            if (!windowOpen)
                break;

            refreshRequired = true;

            // Don't try to render too often, even when the screen is changing (e.g. during animation)
            const auto timePointNow = std::chrono::steady_clock::now();
            const auto timePointNextAllowed = lastRenderTime + std::chrono::milliseconds(15);
            if (timePointNextAllowed > timePointNow)
                continue;

            m_backendRenderTarget->clearScreen();
            draw();
            m_window->display();

            refreshRequired = false;
            lastRenderTime = std::chrono::steady_clock::now(); // Don't use timePointNow to provide enough rest on low-end hardware
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGuiSFML::convertEvent(const sf::Event& eventSFML, Event& eventTGUI)
    {
#if SFML_VERSION_MAJOR >= 3
        if (eventSFML.is<sf::Event::FocusLost>())
        {
            eventTGUI.type = Event::Type::LostFocus;
            return true;
        }

        if (eventSFML.is<sf::Event::FocusGained>())
        {
            eventTGUI.type = Event::Type::GainedFocus;
            return true;
        }

        if (const auto* eventResized = eventSFML.getIf<sf::Event::Resized>())
        {
            eventTGUI.type = Event::Type::Resized;
            eventTGUI.size.width = eventResized->size.x;
            eventTGUI.size.height = eventResized->size.y;
            return true;
        }

        if (eventSFML.is<sf::Event::Closed>())
        {
            eventTGUI.type = Event::Type::Closed;
            return true;
        }

        if (const auto* eventTextEntered = eventSFML.getIf<sf::Event::TextEntered>())
        {
            eventTGUI.type = Event::Type::TextEntered;
            eventTGUI.text.unicode = eventTextEntered->unicode;
            return true;
        }

        if (const auto* eventKeyPressed = eventSFML.getIf<sf::Event::KeyPressed>())
        {
            const Event::KeyboardKey code = convertKeyCode(eventKeyPressed->code);
            if (code == Event::KeyboardKey::Unknown)
                return false; // This key isn't handled by TGUI

            m_modifierKeySystemPressed = eventKeyPressed->system;
            m_modifierKeyControlPressed = eventKeyPressed->control;
            m_modifierKeyShiftPressed = eventKeyPressed->shift;
            m_modifierKeyAltPressed = eventKeyPressed->alt;

            // When pressing a modifier key, the modifier state in the KeyEvent may still be false
            if ((eventKeyPressed->code == sf::Keyboard::Key::LSystem) || (eventKeyPressed->code == sf::Keyboard::Key::RSystem))
                m_modifierKeySystemPressed = true;
            else if ((eventKeyPressed->code == sf::Keyboard::Key::LControl) || (eventKeyPressed->code == sf::Keyboard::Key::RControl))
                m_modifierKeyControlPressed = true;
            else if ((eventKeyPressed->code == sf::Keyboard::Key::LShift) || (eventKeyPressed->code == sf::Keyboard::Key::RShift))
                m_modifierKeyShiftPressed = true;
            else if ((eventKeyPressed->code == sf::Keyboard::Key::LAlt) || (eventKeyPressed->code == sf::Keyboard::Key::RAlt))
                m_modifierKeyAltPressed = true;

            eventTGUI.type = Event::Type::KeyPressed;
            eventTGUI.key.code = code;
            eventTGUI.key.alt = eventKeyPressed->alt;
            eventTGUI.key.control = eventKeyPressed->control;
            eventTGUI.key.shift = eventKeyPressed->shift;
            eventTGUI.key.system = eventKeyPressed->system;

            // If the NumLock is off then we will translate keypad key events to key events for text cursor navigation.
            // This functionality is not yet part of SFML, but is available in PR #3238 (https://github.com/SFML/SFML/pull/3238)
#if 0
            static_assert(static_cast<int>(Event::KeyboardKey::Numpad0) + 9 == static_cast<int>(Event::KeyboardKey::Numpad9), "Numpad0 to Numpad9 need continous ids in KeyboardKey");
            if (!eventKeyPressed->numLock
             && (static_cast<int>(eventTGUI.key.code) >= static_cast<int>(Event::KeyboardKey::Numpad0))
             && (static_cast<int>(eventTGUI.key.code) <= static_cast<int>(Event::KeyboardKey::Numpad9)))
            {
                eventTGUI.key.code = translateKeypadKey(eventTGUI.key.code);
                if (eventTGUI.key.code == Event::KeyboardKey::Unknown) // Numpad5 was pressed which has no function
                    return false; // We didn't handle this key press
            }
#endif
            return true;
        }

        if (const auto* eventKeyReleased = eventSFML.getIf<sf::Event::KeyReleased>())
        {
            m_modifierKeySystemPressed = eventKeyReleased->system;
            m_modifierKeyControlPressed = eventKeyReleased->control;
            m_modifierKeyShiftPressed = eventKeyReleased->shift;
            m_modifierKeyAltPressed = eventKeyReleased->alt;

            // When releasing a modifier key, the modifier state in the KeyEvent may still be true
            if ((eventKeyReleased->code == sf::Keyboard::Key::LSystem) || (eventKeyReleased->code == sf::Keyboard::Key::RSystem))
                m_modifierKeySystemPressed = false;
            else if ((eventKeyReleased->code == sf::Keyboard::Key::LControl) || (eventKeyReleased->code == sf::Keyboard::Key::RControl))
                m_modifierKeyControlPressed = false;
            else if ((eventKeyReleased->code == sf::Keyboard::Key::LShift) || (eventKeyReleased->code == sf::Keyboard::Key::RShift))
                m_modifierKeyShiftPressed = false;
            else if ((eventKeyReleased->code == sf::Keyboard::Key::LAlt) || (eventKeyReleased->code == sf::Keyboard::Key::RAlt))
                m_modifierKeyAltPressed = false;

            return false;
        }

        if (const auto* eventMouseWheelScrolled = eventSFML.getIf<sf::Event::MouseWheelScrolled>())
        {
            if (eventMouseWheelScrolled->wheel != sf::Mouse::Wheel::Vertical)
                return false; // TGUI only handles the vertical mouse wheel

            eventTGUI.type = Event::Type::MouseWheelScrolled;
            eventTGUI.mouseWheel.delta = eventMouseWheelScrolled->delta;
            eventTGUI.mouseWheel.x = eventMouseWheelScrolled->position.x;
            eventTGUI.mouseWheel.y = eventMouseWheelScrolled->position.y;
            return true;
        }

        if (const auto* eventMousePressed = eventSFML.getIf<sf::Event::MouseButtonPressed>())
        {
            if (eventMousePressed->button == sf::Mouse::Button::Left)
                eventTGUI.mouseButton.button = Event::MouseButton::Left;
            else if (eventMousePressed->button == sf::Mouse::Button::Middle)
                eventTGUI.mouseButton.button = Event::MouseButton::Middle;
            else if (eventMousePressed->button == sf::Mouse::Button::Right)
                eventTGUI.mouseButton.button = Event::MouseButton::Right;
            else // This mouse button isn't handled by TGUI
                return false;

            eventTGUI.type = Event::Type::MouseButtonPressed;
            eventTGUI.mouseButton.x = eventMousePressed->position.x;
            eventTGUI.mouseButton.y = eventMousePressed->position.y;
            return true;
        }

        if (const auto* eventMouseReleased = eventSFML.getIf<sf::Event::MouseButtonReleased>())
        {
            if (eventMouseReleased->button == sf::Mouse::Button::Left)
                eventTGUI.mouseButton.button = Event::MouseButton::Left;
            else if (eventMouseReleased->button == sf::Mouse::Button::Middle)
                eventTGUI.mouseButton.button = Event::MouseButton::Middle;
            else if (eventMouseReleased->button == sf::Mouse::Button::Right)
                eventTGUI.mouseButton.button = Event::MouseButton::Right;
            else // This mouse button isn't handled by TGUI
                return false;

            eventTGUI.type = Event::Type::MouseButtonReleased;
            eventTGUI.mouseButton.x = eventMouseReleased->position.x;
            eventTGUI.mouseButton.y = eventMouseReleased->position.y;
            return true;
        }

        if (const auto* eventMouseMoved = eventSFML.getIf<sf::Event::MouseMoved>())
        {
            eventTGUI.type = Event::Type::MouseMoved;
            eventTGUI.mouseMove.x = eventMouseMoved->position.x;
            eventTGUI.mouseMove.y = eventMouseMoved->position.y;
            return true;
        }

        if (eventSFML.is<sf::Event::MouseEntered>())
        {
            eventTGUI.type = Event::Type::MouseEntered;
            return true;
        }

        if (eventSFML.is<sf::Event::MouseLeft>())
        {
            eventTGUI.type = Event::Type::MouseLeft;
            return true;
        }

        if (const auto* eventTouchMoved = eventSFML.getIf<sf::Event::TouchMoved>())
        {
            eventTGUI.type = Event::Type::FingerMoved;
            eventTGUI.touch.fingerId = static_cast<std::uintptr_t>(eventTouchMoved->finger) + 1;
            eventTGUI.touch.x = eventTouchMoved->position.x;
            eventTGUI.touch.y = eventTouchMoved->position.y;
            return true;
        }

        if (const auto* eventTouchBegan = eventSFML.getIf<sf::Event::TouchBegan>())
        {
            eventTGUI.type = Event::Type::FingerDown;
            eventTGUI.touch.fingerId = static_cast<std::uintptr_t>(eventTouchBegan->finger) + 1;
            eventTGUI.touch.x = eventTouchBegan->position.x;
            eventTGUI.touch.y = eventTouchBegan->position.y;
            return true;
        }

        if (const auto* eventTouchEnded = eventSFML.getIf<sf::Event::TouchEnded>())
        {
            eventTGUI.type = Event::Type::FingerUp;
            eventTGUI.touch.fingerId = static_cast<std::uintptr_t>(eventTouchEnded->finger) + 1;
            eventTGUI.touch.x = eventTouchEnded->position.x;
            eventTGUI.touch.y = eventTouchEnded->position.y;
            return true;
        }

        return false;
#else
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

                m_modifierKeySystemPressed = eventSFML.key.system;
                m_modifierKeyControlPressed = eventSFML.key.control;
                m_modifierKeyShiftPressed = eventSFML.key.shift;
                m_modifierKeyAltPressed = eventSFML.key.alt;

                // When pressing a modifier key, the modifier state in the KeyEvent is still false
                if ((eventSFML.key.code == sf::Keyboard::Key::LSystem) || (eventSFML.key.code == sf::Keyboard::Key::RSystem))
                    m_modifierKeySystemPressed = true;
                else if ((eventSFML.key.code == sf::Keyboard::Key::LControl) || (eventSFML.key.code == sf::Keyboard::Key::RControl))
                    m_modifierKeyControlPressed = true;
                else if ((eventSFML.key.code == sf::Keyboard::Key::LShift) || (eventSFML.key.code == sf::Keyboard::Key::RShift))
                    m_modifierKeyShiftPressed = true;
                else if ((eventSFML.key.code == sf::Keyboard::Key::LAlt) || (eventSFML.key.code == sf::Keyboard::Key::RAlt))
                    m_modifierKeyAltPressed = true;

                eventTGUI.type = Event::Type::KeyPressed;
                eventTGUI.key.code = code;
                eventTGUI.key.alt = eventSFML.key.alt;
                eventTGUI.key.control = eventSFML.key.control;
                eventTGUI.key.shift = eventSFML.key.shift;
                eventTGUI.key.system = eventSFML.key.system;
                return true;
            }
            case sf::Event::KeyReleased:
            {
                m_modifierKeySystemPressed = eventSFML.key.system;
                m_modifierKeyControlPressed = eventSFML.key.control;
                m_modifierKeyShiftPressed = eventSFML.key.shift;
                m_modifierKeyAltPressed = eventSFML.key.alt;

                // When releasing a modifier key, the modifier state in the KeyEvent is still true
                if ((eventSFML.key.code == sf::Keyboard::Key::LSystem) || (eventSFML.key.code == sf::Keyboard::Key::RSystem))
                    m_modifierKeySystemPressed = false;
                else if ((eventSFML.key.code == sf::Keyboard::Key::LControl) || (eventSFML.key.code == sf::Keyboard::Key::RControl))
                    m_modifierKeyControlPressed = false;
                else if ((eventSFML.key.code == sf::Keyboard::Key::LShift) || (eventSFML.key.code == sf::Keyboard::Key::RShift))
                    m_modifierKeyShiftPressed = false;
                else if ((eventSFML.key.code == sf::Keyboard::Key::LAlt) || (eventSFML.key.code == sf::Keyboard::Key::RAlt))
                    m_modifierKeyAltPressed = false;

                return false;
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
            case sf::Event::MouseEntered:
            {
                eventTGUI.type = Event::Type::MouseEntered;
                return true;
            }
            case sf::Event::MouseLeft:
            {
                eventTGUI.type = Event::Type::MouseLeft;
                return true;
            }
            case sf::Event::TouchMoved:
            {
                eventTGUI.type = Event::Type::FingerMoved;
                eventTGUI.touch.fingerId = static_cast<std::uintptr_t>(eventSFML.touch.finger) + 1;
                eventTGUI.touch.x = eventSFML.touch.x;
                eventTGUI.touch.y = eventSFML.touch.y;
                return true;
            }
            case sf::Event::TouchBegan:
            {
                eventTGUI.type = Event::Type::FingerDown;
                eventTGUI.touch.fingerId = static_cast<std::uintptr_t>(eventSFML.touch.finger) + 1;
                eventTGUI.touch.x = eventSFML.touch.x;
                eventTGUI.touch.y = eventSFML.touch.y;
                return true;
            }
            case sf::Event::TouchEnded:
            {
                eventTGUI.type = Event::Type::FingerUp;
                eventTGUI.touch.fingerId = static_cast<std::uintptr_t>(eventSFML.touch.finger) + 1;
                eventTGUI.touch.x = eventSFML.touch.x;
                eventTGUI.touch.y = eventSFML.touch.y;
                return true;
            }
            default: // This event is not handled by TGUI
                return false;
        }
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Window* BackendGuiSFML::getWindow() const
    {
        return m_window;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGuiSFML::startTextInput(FloatRect)
    {
        // Open the software keyboard on Android and iOS
        sf::Keyboard::setVirtualKeyboardVisible(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGuiSFML::stopTextInput()
    {
        // Open the software keyboard on Android and iOS
        sf::Keyboard::setVirtualKeyboardVisible(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TGUI_SYSTEM_WINDOWS
    void BackendGuiSFML::updateTextCursorPosition(FloatRect, Vector2f caretPos)
    {
        if (!m_window)
            return;

    #if SFML_VERSION_MAJOR >= 3
        WindowsIMM::setCandidateWindowPosition(m_window->getNativeHandle(), mapCoordsToPixel(caretPos));
    #else
        WindowsIMM::setCandidateWindowPosition(m_window->getSystemHandle(), mapCoordsToPixel(caretPos));
    #endif
    }
#else
    void BackendGuiSFML::updateTextCursorPosition(FloatRect inputRect, Vector2f caretPos)
    {
        BackendGui::updateTextCursorPosition(inputRect, caretPos);
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGuiSFML::isKeyboardModifierPressed(Event::KeyModifier modifierKey) const
    {
        switch (modifierKey)
        {
        case Event::KeyModifier::System:
            return m_modifierKeySystemPressed;
        case Event::KeyModifier::Control:
            return m_modifierKeyControlPressed;
        case Event::KeyModifier::Shift:
            return m_modifierKeyShiftPressed;
        case Event::KeyModifier::Alt:
            return m_modifierKeyAltPressed;
        }

        TGUI_ASSERT(false, "BackendGuiSFML::isKeyboardModifierPressed called with an invalid value");
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGuiSFML::setGuiWindow(sf::Window& window)
    {
        TGUI_ASSERT(std::dynamic_pointer_cast<BackendSFML>(getBackend()), "BackendGuiSFML requires system backend of type BackendSFML");

        m_window = &window;

        getBackend()->attachGui(this);
        std::static_pointer_cast<BackendSFML>(getBackend())->setGuiWindow(this, m_window);

        updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGuiSFML::updateContainerSize()
    {
        // We can't do anything yet if we don't have a window
        if (!m_window)
            return;

        m_framebufferSize = {static_cast<int>(m_window->getSize().x), static_cast<int>(m_window->getSize().y)};

        BackendGui::updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
