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
    static Event::KeyboardKey convertKeyCode(SDL_Keycode key)
    {
        switch (key)
        {
        case SDLK_a:            return Event::KeyboardKey::A;
        case SDLK_b:            return Event::KeyboardKey::B;
        case SDLK_c:            return Event::KeyboardKey::C;
        case SDLK_d:            return Event::KeyboardKey::D;
        case SDLK_e:            return Event::KeyboardKey::E;
        case SDLK_f:            return Event::KeyboardKey::F;
        case SDLK_g:            return Event::KeyboardKey::G;
        case SDLK_h:            return Event::KeyboardKey::H;
        case SDLK_i:            return Event::KeyboardKey::I;
        case SDLK_j:            return Event::KeyboardKey::J;
        case SDLK_k:            return Event::KeyboardKey::K;
        case SDLK_l:            return Event::KeyboardKey::L;
        case SDLK_m:            return Event::KeyboardKey::M;
        case SDLK_n:            return Event::KeyboardKey::N;
        case SDLK_o:            return Event::KeyboardKey::O;
        case SDLK_p:            return Event::KeyboardKey::P;
        case SDLK_q:            return Event::KeyboardKey::Q;
        case SDLK_r:            return Event::KeyboardKey::R;
        case SDLK_s:            return Event::KeyboardKey::S;
        case SDLK_t:            return Event::KeyboardKey::T;
        case SDLK_u:            return Event::KeyboardKey::U;
        case SDLK_v:            return Event::KeyboardKey::V;
        case SDLK_w:            return Event::KeyboardKey::W;
        case SDLK_x:            return Event::KeyboardKey::X;
        case SDLK_y:            return Event::KeyboardKey::Y;
        case SDLK_z:            return Event::KeyboardKey::Z;
        case SDLK_0:            return Event::KeyboardKey::Num0;
        case SDLK_1:            return Event::KeyboardKey::Num1;
        case SDLK_2:            return Event::KeyboardKey::Num2;
        case SDLK_3:            return Event::KeyboardKey::Num3;
        case SDLK_4:            return Event::KeyboardKey::Num4;
        case SDLK_5:            return Event::KeyboardKey::Num5;
        case SDLK_6:            return Event::KeyboardKey::Num6;
        case SDLK_7:            return Event::KeyboardKey::Num7;
        case SDLK_8:            return Event::KeyboardKey::Num8;
        case SDLK_9:            return Event::KeyboardKey::Num9;
        case SDLK_ESCAPE:       return Event::KeyboardKey::Escape;
        case SDLK_LCTRL:        return Event::KeyboardKey::LControl;
        case SDLK_LSHIFT:       return Event::KeyboardKey::LShift;
        case SDLK_LALT:         return Event::KeyboardKey::LAlt;
        case SDLK_LGUI:         return Event::KeyboardKey::LSystem;
        case SDLK_RCTRL:        return Event::KeyboardKey::RControl;
        case SDLK_RSHIFT:       return Event::KeyboardKey::RShift;
        case SDLK_RALT:         return Event::KeyboardKey::RAlt;
        case SDLK_RGUI:         return Event::KeyboardKey::RSystem;
        case SDLK_MENU:         return Event::KeyboardKey::Menu;
        case SDLK_LEFTBRACKET:  return Event::KeyboardKey::LBracket;
        case SDLK_RIGHTBRACKET: return Event::KeyboardKey::RBracket;
        case SDLK_SEMICOLON:    return Event::KeyboardKey::Semicolon;
        case SDLK_COMMA:        return Event::KeyboardKey::Comma;
        case SDLK_PERIOD:       return Event::KeyboardKey::Period;
        case SDLK_QUOTE:        return Event::KeyboardKey::Quote;
        case SDLK_SLASH:        return Event::KeyboardKey::Slash;
        case SDLK_BACKSLASH:    return Event::KeyboardKey::Backslash;
        case SDLK_EQUALS:       return Event::KeyboardKey::Equal;
        case SDLK_MINUS:        return Event::KeyboardKey::Minus;
        case SDLK_SPACE:        return Event::KeyboardKey::Space;
        case SDLK_RETURN:       return Event::KeyboardKey::Enter;
        case SDLK_KP_ENTER:     return Event::KeyboardKey::Enter;
        case SDLK_BACKSPACE:    return Event::KeyboardKey::Backspace;
        case SDLK_TAB:          return Event::KeyboardKey::Tab;
        case SDLK_PAGEUP:       return Event::KeyboardKey::PageUp;
        case SDLK_PAGEDOWN:     return Event::KeyboardKey::PageDown;
        case SDLK_END:          return Event::KeyboardKey::End;
        case SDLK_HOME:         return Event::KeyboardKey::Home;
        case SDLK_INSERT:       return Event::KeyboardKey::Insert;
        case SDLK_DELETE:       return Event::KeyboardKey::Delete;
        case SDLK_KP_PLUS:      return Event::KeyboardKey::Add;
        case SDLK_KP_MINUS:     return Event::KeyboardKey::Subtract;
        case SDLK_KP_MULTIPLY:  return Event::KeyboardKey::Multiply;
        case SDLK_KP_DIVIDE:    return Event::KeyboardKey::Divide;
        case SDLK_LEFT:         return Event::KeyboardKey::Left;
        case SDLK_RIGHT:        return Event::KeyboardKey::Right;
        case SDLK_UP:           return Event::KeyboardKey::Up;
        case SDLK_DOWN:         return Event::KeyboardKey::Down;
        case SDLK_KP_0:         return Event::KeyboardKey::Numpad0;
        case SDLK_KP_1:         return Event::KeyboardKey::Numpad1;
        case SDLK_KP_2:         return Event::KeyboardKey::Numpad2;
        case SDLK_KP_3:         return Event::KeyboardKey::Numpad3;
        case SDLK_KP_4:         return Event::KeyboardKey::Numpad4;
        case SDLK_KP_5:         return Event::KeyboardKey::Numpad5;
        case SDLK_KP_6:         return Event::KeyboardKey::Numpad6;
        case SDLK_KP_7:         return Event::KeyboardKey::Numpad7;
        case SDLK_KP_8:         return Event::KeyboardKey::Numpad8;
        case SDLK_KP_9:         return Event::KeyboardKey::Numpad9;
        case SDLK_F1:           return Event::KeyboardKey::F1;
        case SDLK_F2:           return Event::KeyboardKey::F2;
        case SDLK_F3:           return Event::KeyboardKey::F3;
        case SDLK_F4:           return Event::KeyboardKey::F4;
        case SDLK_F5:           return Event::KeyboardKey::F5;
        case SDLK_F6:           return Event::KeyboardKey::F6;
        case SDLK_F7:           return Event::KeyboardKey::F7;
        case SDLK_F8:           return Event::KeyboardKey::F8;
        case SDLK_F9:           return Event::KeyboardKey::F9;
        case SDLK_F10:          return Event::KeyboardKey::F10;
        case SDLK_F11:          return Event::KeyboardKey::F11;
        case SDLK_F12:          return Event::KeyboardKey::F12;
        case SDLK_F13:          return Event::KeyboardKey::F13;
        case SDLK_F14:          return Event::KeyboardKey::F14;
        case SDLK_F15:          return Event::KeyboardKey::F15;
        case SDLK_PAUSE:        return Event::KeyboardKey::Pause;
        default: // We don't process the other keys
            return Event::KeyboardKey::Unknown;
        }
    }

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

    bool GuiSDL::convertEvent(const SDL_Event& eventSDL, Event& eventTGUI)
    {
        switch (eventSDL.type)
        {
            case SDL_WINDOWEVENT:
            {
                if (eventSDL.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                {
                    eventTGUI.type = Event::Type::GainedFocus;
                    return true;
                }
                else if (eventSDL.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
                {
                    eventTGUI.type = Event::Type::LostFocus;
                    return true;
                }
                else if (eventSDL.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    eventTGUI.type = Event::Type::Resized;
                    eventTGUI.size.width = static_cast<unsigned int>(eventSDL.window.data1);
                    eventTGUI.size.height = static_cast<unsigned int>(eventSDL.window.data2);
                    return true;
                }
                else // This window event is not handled by TGUI
                    return false;
            }
            case SDL_QUIT:
            {
                eventTGUI.type = Event::Type::Closed;
                return true;
            }
            case SDL_TEXTINPUT:
            {
                // This code assumes eventSDL.text.text never contains more than one UTF-32 character
                eventTGUI.type = Event::Type::TextEntered;
                eventTGUI.text.unicode = String(eventSDL.text.text)[0];
                return true;
            }
            case SDL_KEYDOWN:
            {
                const Event::KeyboardKey code = convertKeyCode(eventSDL.key.keysym.sym);
                if (code == Event::KeyboardKey::Unknown)
                    return false; // This key isn't handled by TGUI

                eventTGUI.type = Event::Type::KeyPressed;
                eventTGUI.key.code = code;
                eventTGUI.key.alt = ((eventSDL.key.keysym.mod & KMOD_ALT) != 0);
                eventTGUI.key.control = ((eventSDL.key.keysym.mod & KMOD_CTRL) != 0);
                eventTGUI.key.shift = ((eventSDL.key.keysym.mod & KMOD_SHIFT) != 0);
                eventTGUI.key.system = ((eventSDL.key.keysym.mod & KMOD_GUI) != 0);
                return true;
            }
            case SDL_MOUSEWHEEL:
            {
                if (eventSDL.wheel.y == 0)
                    return false; // TGUI only handles the vertical mouse wheel

                eventTGUI.type = Event::Type::MouseWheelScrolled;

                if (eventSDL.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
                    eventTGUI.mouseWheel.delta =  -static_cast<float>(eventSDL.wheel.y);
                else
                    eventTGUI.mouseWheel.delta = static_cast<float>(eventSDL.wheel.y);

                // SDL doesn't include the mouse position in mouse wheel events, so we add the last known position ourself
                eventTGUI.mouseWheel.x = static_cast<int>(m_lastMousePos.x);
                eventTGUI.mouseWheel.y = static_cast<int>(m_lastMousePos.y);

                return true;
            }
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            {
                // Ignore synthetic mouse events that SDL generates for touches on mobile platforms,
                // because we handle them ourselves in the SDL_FINGERDOWN and SDL_FINGERUP events.
                if (eventSDL.button.which == static_cast<std::uint32_t>(-1))
                    return false;

                switch (eventSDL.button.button)
                {
                case SDL_BUTTON_LEFT:
                    eventTGUI.mouseButton.button = Event::MouseButton::Left;
                    break;
                case SDL_BUTTON_MIDDLE:
                    eventTGUI.mouseButton.button = Event::MouseButton::Middle;
                    break;
                case SDL_BUTTON_RIGHT:
                    eventTGUI.mouseButton.button = Event::MouseButton::Right;
                    break;
                default: // This mouse button isn't handled by TGUI
                    return false;
                }

                if (eventSDL.type == SDL_MOUSEBUTTONDOWN)
                    eventTGUI.type = Event::Type::MouseButtonPressed;
                else
                    eventTGUI.type = Event::Type::MouseButtonReleased;

                eventTGUI.mouseButton.x = eventSDL.button.x;
                eventTGUI.mouseButton.y = eventSDL.button.y;
                return true;
            }
            case SDL_MOUSEMOTION:
            {
                // Ignore synthetic mouse events that SDL generates for touches on mobile platforms,
                // because we handle them ourselves in the SDL_FINGERMOTION event.
                if (eventSDL.motion.which == static_cast<std::uint32_t>(-1))
                    return false;

                eventTGUI.type = Event::Type::MouseMoved;
                eventTGUI.mouseMove.x = eventSDL.motion.x;
                eventTGUI.mouseMove.y = eventSDL.motion.y;
                return true;
            }
            case SDL_FINGERDOWN:
            {
                // Ignore this finger if another finger is already down
                if (m_touchFirstFingerDown)
                    return false;

                // Remember which finger this is, for when we receive SDL_FINGERMOTION and SDL_FINGERUP events
                m_touchFirstFingerDown = true;
                m_touchFirstFingerId = eventSDL.tfinger.fingerId;
                m_touchFirstFingerTouchId = eventSDL.tfinger.touchId;

                // Simulate a MouseButtonPressed event.
                // Note that the coordinate can be wrong if the window isn't fullscreen!
                // If event.tfinger.windowID (added in 2.0.12) differs from 0 then the coordinates are
                // relative to the window, otherwise they are relative to the touch display itself.
                eventTGUI.type = Event::Type::MouseButtonPressed;
                eventTGUI.mouseButton.button = Event::MouseButton::Left;
                eventTGUI.mouseButton.x = static_cast<int>(eventSDL.tfinger.x * m_windowSize.x);
                eventTGUI.mouseButton.y = static_cast<int>(eventSDL.tfinger.y * m_windowSize.y);
                return true;
            }
            case SDL_FINGERUP:
            {
                // Only handle the event if this is the first finger
                if (!m_touchFirstFingerDown || (m_touchFirstFingerId != eventSDL.tfinger.fingerId) || (m_touchFirstFingerTouchId != eventSDL.tfinger.touchId))
                    return false;

                m_touchFirstFingerDown = false;

                // Simulate a MouseButtonReleased event.
                // Note that the coordinate can be wrong if the window isn't fullscreen!
                // If event.tfinger.windowID (added in 2.0.12) differs from 0 then the coordinates are
                // relative to the window, otherwise they are relative to the touch display itself.
                eventTGUI.type = Event::Type::MouseButtonReleased;
                eventTGUI.mouseButton.button = Event::MouseButton::Left;
                eventTGUI.mouseButton.x = static_cast<int>(eventSDL.tfinger.x * m_windowSize.x);
                eventTGUI.mouseButton.y = static_cast<int>(eventSDL.tfinger.y * m_windowSize.y);
                return true;
            }
            case SDL_FINGERMOTION:
            {
                // Only handle the event if this is the first finger
                if (!m_touchFirstFingerDown || (m_touchFirstFingerId != eventSDL.tfinger.fingerId) || (m_touchFirstFingerTouchId != eventSDL.tfinger.touchId))
                    return false;

                // Simulate a MouseMoved event.
                // Note that the coordinate can be wrong if the window isn't fullscreen!
                // If event.tfinger.windowID (added in 2.0.12) differs from 0 then the coordinates are
                // relative to the window, otherwise they are relative to the touch display itself.
                eventTGUI.type = Event::Type::MouseMoved;
                eventTGUI.mouseMove.x = static_cast<int>(eventSDL.tfinger.x * m_windowSize.x);
                eventTGUI.mouseMove.y = static_cast<int>(eventSDL.tfinger.y * m_windowSize.y);
                return true;
            }
            default: // This event is not handled by TGUI
                return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool GuiSDL::handleEvent(const SDL_Event& sdlEvent)
    {
        Event event;
        if (!convertEvent(sdlEvent, event))
            return false; // We don't process this type of event

        // For touches, always send a mouse move event before the mouse press,
        // because widgets may assume that the mouse had to move to the clicked location first
        if ((event.type == Event::Type::MouseButtonPressed) && (sdlEvent.type == SDL_FINGERDOWN))
        {
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

        SDL_GetWindowSize(m_renderTarget->getWindow(), &m_windowSize.x, &m_windowSize.y);

        m_viewport.updateParentSize({static_cast<float>(m_windowSize.x), static_cast<float>(m_windowSize.y)});
        m_view.updateParentSize({m_viewport.getWidth(), m_viewport.getHeight()});
        m_renderTarget->setView(m_view.getRect(), m_viewport.getRect(),
                                {static_cast<float>(m_windowSize.x), static_cast<float>(m_windowSize.y)});

        GuiBase::updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
