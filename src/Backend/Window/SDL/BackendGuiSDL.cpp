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


#include <TGUI/Backend/Window/SDL/BackendGuiSDL.hpp>
#include <TGUI/Backend/Window/SDL/BackendSDL.hpp>

#include <TGUI/extlibs/IncludeSDL.hpp>

#if TGUI_BUILD_AS_CXX_MODULE
    import tgui;
#else
    #include <TGUI/Timer.hpp>
#endif

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <thread>
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_NODISCARD static Event::KeyboardKey convertKeyCode(SDL_Keycode key)
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
        case SDLK_RETURN:
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

    bool BackendGuiSDL::convertEvent(const SDL_Event& eventSDL, Event& eventTGUI)
    {
        switch (eventSDL.type)
        {
#if SDL_MAJOR_VERSION >= 3
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
            {
                eventTGUI.type = Event::Type::GainedFocus;
                return true;
            }
            case SDL_EVENT_WINDOW_FOCUS_LOST:
            {
                eventTGUI.type = Event::Type::LostFocus;
                return true;
            }
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            {
                eventTGUI.type = Event::Type::Resized;
                eventTGUI.size.width = static_cast<unsigned int>(eventSDL.window.data1);
                eventTGUI.size.height = static_cast<unsigned int>(eventSDL.window.data2);
                return true;
            }
            case SDL_EVENT_WINDOW_MOUSE_ENTER:
            {
                eventTGUI.type = Event::Type::MouseEntered;
                return true;
            }
            case SDL_EVENT_WINDOW_MOUSE_LEAVE:
            {
                eventTGUI.type = Event::Type::MouseLeft;
                return true;
            }
#else
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
                else if (eventSDL.window.event == SDL_WINDOWEVENT_ENTER)
                {
                    eventTGUI.type = Event::Type::MouseEntered;
                    return true;
                }
                else if (eventSDL.window.event == SDL_WINDOWEVENT_LEAVE)
                {
                    eventTGUI.type = Event::Type::MouseLeft;
                    return true;
                }
                else // This window event is not handled by TGUI
                    return false;
            }
#endif
            case SDL_EVENT_QUIT:
            {
                eventTGUI.type = Event::Type::Closed;
                return true;
            }
            case SDL_EVENT_TEXT_INPUT:
            {
                const String unicodeStr(static_cast<const char*>(eventSDL.text.text));
                if (unicodeStr.empty())
                    return false;

                // This code assumes eventSDL.text.text never contains more than one UTF-32 character.
                // If there are more characters (which can happen when an IME is used) then this
                // will be handled in BackendGuiSDL::handleEvent.
                eventTGUI.type = Event::Type::TextEntered;
                eventTGUI.text.unicode = unicodeStr.back();
                return true;
            }
            case SDL_EVENT_KEY_DOWN:
            {
                const Event::KeyboardKey code = convertKeyCode(eventSDL.key.keysym.sym);
                if (code == Event::KeyboardKey::Unknown)
                    return false; // This key isn't handled by TGUI

                eventTGUI.type = Event::Type::KeyPressed;
                eventTGUI.key.code = code;
                eventTGUI.key.alt = ((eventSDL.key.keysym.mod & SDL_KMOD_ALT) != 0);
                eventTGUI.key.control = ((eventSDL.key.keysym.mod & SDL_KMOD_CTRL) != 0);
                eventTGUI.key.shift = ((eventSDL.key.keysym.mod & SDL_KMOD_SHIFT) != 0);
                eventTGUI.key.system = ((eventSDL.key.keysym.mod & SDL_KMOD_GUI) != 0);
                return true;
            }
            case SDL_EVENT_MOUSE_WHEEL:
            {
                eventTGUI.type = Event::Type::MouseWheelScrolled;

#if SDL_MAJOR_VERSION >= 3
                if (eventSDL.wheel.y == 0)
                    return false; // TGUI only handles the vertical mouse wheel

                if (eventSDL.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
                    eventTGUI.mouseWheel.delta = -eventSDL.wheel.y;
                else
                    eventTGUI.mouseWheel.delta = eventSDL.wheel.y;
#elif ((SDL_MAJOR_VERSION == 2) && (SDL_MINOR_VERSION > 0)) \
    || ((SDL_MAJOR_VERSION == 2) && (SDL_MINOR_VERSION == 0) && (SDL_PATCHLEVEL >= 18))
                if (eventSDL.wheel.preciseY == 0)
                    return false; // TGUI only handles the vertical mouse wheel

                if (eventSDL.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
                    eventTGUI.mouseWheel.delta = -eventSDL.wheel.preciseY;
                else
                    eventTGUI.mouseWheel.delta = eventSDL.wheel.preciseY;
#else
                if (eventSDL.wheel.y == 0)
                    return false; // TGUI only handles the vertical mouse wheel

                if (eventSDL.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
                    eventTGUI.mouseWheel.delta = -static_cast<float>(eventSDL.wheel.y);
                else
                    eventTGUI.mouseWheel.delta = static_cast<float>(eventSDL.wheel.y);
#endif

#if (SDL_MAJOR_VERSION > 2) || ((SDL_MAJOR_VERSION == 2) && (SDL_MINOR_VERSION >= 26))
                eventTGUI.mouseWheel.x = static_cast<int>(eventSDL.wheel.mouseX * m_dpiScale);
                eventTGUI.mouseWheel.y = static_cast<int>(eventSDL.wheel.mouseY * m_dpiScale);
#else
                // SDL didn't include the mouse position in mouse wheel events, so we add the last known position ourself
                eventTGUI.mouseWheel.x = static_cast<int>(m_lastMousePos.x);
                eventTGUI.mouseWheel.y = static_cast<int>(m_lastMousePos.y);
#endif

                return true;
            }
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                // Ignore synthetic mouse events that SDL generates for touches on mobile platforms,
                // because we handle them ourselves in the SDL_EVENT_FINGER_DOWN and SDL_EVENT_FINGER_UP events.
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

                if (eventSDL.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
                    eventTGUI.type = Event::Type::MouseButtonPressed;
                else
                    eventTGUI.type = Event::Type::MouseButtonReleased;

                eventTGUI.mouseButton.x = static_cast<int>(eventSDL.button.x * m_dpiScale);
                eventTGUI.mouseButton.y = static_cast<int>(eventSDL.button.y * m_dpiScale);
                return true;
            }
            case SDL_EVENT_MOUSE_MOTION:
            {
                // Ignore synthetic mouse events that SDL generates for touches on mobile platforms,
                // because we handle them ourselves in the SDL_EVENT_FINGER_MOTION event.
                if (eventSDL.motion.which == static_cast<std::uint32_t>(-1))
                    return false;

                eventTGUI.type = Event::Type::MouseMoved;
                eventTGUI.mouseMove.x = static_cast<int>(eventSDL.motion.x * m_dpiScale);
                eventTGUI.mouseMove.y = static_cast<int>(eventSDL.motion.y * m_dpiScale);
                return true;
            }
            case SDL_EVENT_FINGER_DOWN:
            {
                // Ignore this finger if another finger is already down
                if (m_touchFirstFingerDown)
                    return false;

                // Remember which finger this is, for when we receive SDL_EVENT_FINGER_MOTION and SDL_EVENT_FINGER_UP events
                m_touchFirstFingerDown = true;
                m_touchFirstFingerId = eventSDL.tfinger.fingerId;
                m_touchFirstFingerTouchId = eventSDL.tfinger.touchId;

                // Simulate a MouseButtonPressed event
                eventTGUI.type = Event::Type::MouseButtonPressed;
                eventTGUI.mouseButton.button = Event::MouseButton::Left;
                eventTGUI.mouseButton.x = static_cast<int>(std::round(eventSDL.tfinger.x * m_framebufferSize.x));
                eventTGUI.mouseButton.y = static_cast<int>(std::round(eventSDL.tfinger.y * m_framebufferSize.y));
                return true;
            }
            case SDL_EVENT_FINGER_UP:
            {
                // Only handle the event if this is the first finger
                if (!m_touchFirstFingerDown || (m_touchFirstFingerId != eventSDL.tfinger.fingerId) || (m_touchFirstFingerTouchId != eventSDL.tfinger.touchId))
                    return false;

                m_touchFirstFingerDown = false;

                // Simulate a MouseButtonReleased event
                eventTGUI.type = Event::Type::MouseButtonReleased;
                eventTGUI.mouseButton.button = Event::MouseButton::Left;
                eventTGUI.mouseButton.x = static_cast<int>(std::round(eventSDL.tfinger.x * m_framebufferSize.x));
                eventTGUI.mouseButton.y = static_cast<int>(std::round(eventSDL.tfinger.y * m_framebufferSize.y));
                return true;
            }
            case SDL_EVENT_FINGER_MOTION:
            {
                // Only handle the event if this is the first finger
                if (!m_touchFirstFingerDown || (m_touchFirstFingerId != eventSDL.tfinger.fingerId) || (m_touchFirstFingerTouchId != eventSDL.tfinger.touchId))
                    return false;

                // Simulate a MouseMoved event
                eventTGUI.type = Event::Type::MouseMoved;
                eventTGUI.mouseMove.x = static_cast<int>(std::round(eventSDL.tfinger.x * m_framebufferSize.x));
                eventTGUI.mouseMove.y = static_cast<int>(std::round(eventSDL.tfinger.y * m_framebufferSize.y));
                return true;
            }
            default: // This event is not handled by TGUI
                return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGuiSDL::handleEvent(const SDL_Event& sdlEvent)
    {
        // Detect scrolling with two fingers by examining touch events
        if ((sdlEvent.type == SDL_EVENT_FINGER_DOWN) || (sdlEvent.type == SDL_EVENT_FINGER_UP) || (sdlEvent.type == SDL_EVENT_FINGER_MOTION))
        {
            const bool wasScrolling = m_twoFingerScroll.isScrolling();

            const auto fingerId = static_cast<std::intptr_t>(sdlEvent.tfinger.fingerId);
            const float x = sdlEvent.tfinger.x * m_framebufferSize.x;
            const float y = sdlEvent.tfinger.y * m_framebufferSize.y;

            if (sdlEvent.type == SDL_EVENT_FINGER_DOWN)
                m_twoFingerScroll.reportFingerDown(fingerId, x, y);
            else if (sdlEvent.type == SDL_EVENT_FINGER_UP)
                m_twoFingerScroll.reportFingerUp(fingerId);
            else if (sdlEvent.type == SDL_EVENT_FINGER_MOTION)
            {
                m_twoFingerScroll.reportFingerMotion(fingerId, x, y);
                if (m_twoFingerScroll.isScrolling())
                    return handleTwoFingerScroll(wasScrolling);
            }
        }

        // Convert the event to our own type so that we can process it in a backend-independent way afterwards
        Event event;
        if (!convertEvent(sdlEvent, event))
            return false; // We don't process this type of event

        // For touches, always send a mouse move event before the mouse press,
        // because widgets may assume that the mouse had to move to the clicked location first
        if ((event.type == Event::Type::MouseButtonPressed) && (sdlEvent.type == SDL_EVENT_FINGER_DOWN))
        {
            Event mouseMoveEvent;
            mouseMoveEvent.type = Event::Type::MouseMoved;
            mouseMoveEvent.mouseMove.x = event.mouseButton.x;
            mouseMoveEvent.mouseMove.y = event.mouseButton.y;
            handleEvent(mouseMoveEvent);
        }

        // If a text event consists of multiple unicode characters (which can happen when an IME is used) then our
        // converted event only contains the last character. We will send all other unicode characters here.
        if ((event.type == Event::Type::TextEntered) && (sdlEvent.type == SDL_EVENT_TEXT_INPUT) && (sdlEvent.text.text[1] != '\0'))
        {
            // Note that we also pass here if sdlEvent.text.text consists of multiple UTF-8 characters that still fit
            // within a single UTF-32 codepoint. So we still need to check that there are multiple UTF-32 codepoints below.
            const String unicodeStr(static_cast<const char*>(sdlEvent.text.text));
            if (unicodeStr.length() >= 2)
            {
                // We send all characters except the last one here, as that one is part of the TGUI event
                // that will be processed at the end of this function.
                Event textEvent;
                textEvent.type = Event::Type::TextEntered;
                for (unsigned int i = 0; i < unicodeStr.length() - 1; ++i)
                {
                    textEvent.text.unicode = unicodeStr[i];
                    handleEvent(textEvent);
                }
            }
        }

        return handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGuiSDL::mainLoop(Color clearColor)
    {
        TGUI_ASSERT(m_window && m_backendRenderTarget, "BackendGuiSDL must be given an SDL_Window (either at construction or via setWindow function) before mainLoop() is called");

        m_backendRenderTarget->setClearColor(clearColor);

        // Helper function that calculates the amount of time to sleep, which is 10ms unless a timer will expire before this time
        const auto getTimerWakeUpTime = []{
            Optional<Duration> duration = Timer::getNextScheduledTime();
            if (duration && (*duration < std::chrono::milliseconds(10)))
                return *duration;
            else
                return Duration{std::chrono::milliseconds(10)};
        };

        setDrawingUpdatesTime(false);

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

                    if (event.type == SDL_EVENT_QUIT)
                    {
                        quit = true;
                        eventProcessed = true;
                    }
#if SDL_MAJOR_VERSION >= 3
                    else if ((event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) || (event.type == SDL_EVENT_WINDOW_MOUSE_LEAVE))
#else
                    else if ((event.type == SDL_WINDOWEVENT) && ((event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) || (event.window.event == SDL_WINDOWEVENT_LEAVE)))
#endif
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
            const auto timePointNextAllowed = lastRenderTime + std::chrono::milliseconds(15);
            if (timePointNextAllowed > timePointNow)
            {
                const auto timerWakeUpTime = getTimerWakeUpTime();
                if (timePointNextAllowed - timePointNow < timerWakeUpTime)
                    std::this_thread::sleep_for(timePointNextAllowed - timePointNow);
                else
                    std::this_thread::sleep_for(std::chrono::nanoseconds(timerWakeUpTime));

                continue;
            }

            m_backendRenderTarget->clearScreen();
            draw();
            presentScreen(); // Call SDL_GL_SwapWindow or SDL_RenderPresent depending on the backend renderer

            refreshRequired = false;
            lastRenderTime = std::chrono::steady_clock::now(); // Don't use timePointNow to provide enough rest on low-end hardware
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SDL_Window* BackendGuiSDL::getWindow() const
    {
        return m_window;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGuiSDL::startTextInput(FloatRect inputRect)
    {
        if (!m_textInputStarted)
        {
            // We need to call SDL_StartTextInput here, but we reuse the updateTextCursorPosition code as it needs the same calculations
            updateTextCursorPosition(inputRect, {});

            SDL_StartTextInput();
            m_textInputStarted = true;

            // For some weird reason the IME candidates window might remain on the old location when we move focus from
            // one widget to another (even though SDL_StopTextInput was even called before we got here). The solution seems
            // to be to call SDL_SetTextInputRect here again, after SDL_StartTextInput was called.
            // We don't need to do this explicitly here because updateTextCursorPosition will be called shortly after
            // startTextInput finishes, which already makes another SDL_SetTextInputRect call.
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGuiSDL::stopTextInput()
    {
        if (m_textInputStarted)
        {
            SDL_StopTextInput();
            m_textInputStarted = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGuiSDL::updateTextCursorPosition(FloatRect inputRect, Vector2f caretPos)
    {
        // We only support high-DPI in this code with SDL 2.26 or newer.
        // The rectangle passed to SDL_SetTextInputRect will be wrong in older SDL versions on high-DPI screens.
        float dpiScale = 1;

#if (SDL_MAJOR_VERSION > 2) || ((SDL_MAJOR_VERSION == 2) && (SDL_MINOR_VERSION >= 26))
        // This code was originally added for iOS (https://github.com/texus/TGUI/issues/183).
        // Apparently it isn't needed for the calculation of the IME window position on Windows, so maybe this code isn't correct anymore?
        // Until more information is available, the code is disabled now except for when running on iOS.
    #ifdef TGUI_SYSTEM_IOS
        // On a high-DPI screen, we work in pixel coordinates while the SDL rectangle needs to be provided in screen coordinates.
        // So we need to calculate the DPI scaling of the window.
        if (m_window)
        {
            Vector2i windowSizeScreenCoords;
            SDL_GetWindowSize(m_window, &windowSizeScreenCoords.x, &windowSizeScreenCoords.y);

            Vector2i windowSizePixels;
            SDL_GetWindowSizeInPixels(m_window, &windowSizePixels.x, &windowSizePixels.y);

            if ((windowSizeScreenCoords.y != 0) && (windowSizeScreenCoords.y != windowSizePixels.y))
                dpiScale = static_cast<float>(windowSizePixels.y) / static_cast<float>(windowSizeScreenCoords.y);
        }
    #endif
#endif

        const Vector2f topLeft = mapCoordsToPixel(inputRect.getPosition());
        const Vector2f bottomRight = mapCoordsToPixel(inputRect.getPosition() + inputRect.getSize());
        const Vector2f caretPosPixels = mapCoordsToPixel(caretPos);

        // SDL positions the IME window at the left side of the input rect, but we want the window to follow the caret.
        // So we set the left side of the rectangle to the current location of the caret.
        SDL_Rect sdlRect;
        sdlRect.x = static_cast<int>(std::round(std::max(caretPosPixels.x, topLeft.x) / dpiScale));
        sdlRect.y = static_cast<int>(std::round(topLeft.y / dpiScale));
        sdlRect.w = static_cast<int>(std::round(std::max(caretPosPixels.x, (bottomRight - topLeft).x) / dpiScale));
        sdlRect.h = static_cast<int>(std::round((bottomRight - topLeft).y / dpiScale));
        SDL_SetTextInputRect(&sdlRect);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGuiSDL::setGuiWindow(SDL_Window* window)
    {
        TGUI_ASSERT(std::dynamic_pointer_cast<BackendSDL>(getBackend()), "BackendGuiSDL requires system backend of type BackendSDL");

        m_window = window;

        getBackend()->attachGui(this);
        std::static_pointer_cast<BackendSDL>(getBackend())->setGuiWindow(this, window);

        updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGuiSDL::updateContainerSize()
    {
        // We can't do anything yet if we don't have a window
        if (!m_window)
            return;

#if (SDL_MAJOR_VERSION > 2) || ((SDL_MAJOR_VERSION == 2) && (SDL_MINOR_VERSION >= 26))
        SDL_GetWindowSizeInPixels(m_window, &m_framebufferSize.x, &m_framebufferSize.y);
#else
        updateFramebufferSize();
#endif

        Vector2i windowSize;
        SDL_GetWindowSize(m_window, &windowSize.x, &windowSize.y);
        if ((windowSize.y != 0) && (windowSize.y != m_framebufferSize.y))
            m_dpiScale = static_cast<float>(m_framebufferSize.y) / static_cast<float>(windowSize.y);
        else
            m_dpiScale = 1;

        BackendGui::updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
