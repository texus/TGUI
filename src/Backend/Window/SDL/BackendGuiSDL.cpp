/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2025 Bruno Van de Velde (vdv_b@tgui.eu)
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

#if !TGUI_BUILD_AS_CXX_MODULE
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
#if SDL_MAJOR_VERSION >= 3
        case SDLK_A:            return Event::KeyboardKey::A;
        case SDLK_B:            return Event::KeyboardKey::B;
        case SDLK_C:            return Event::KeyboardKey::C;
        case SDLK_D:            return Event::KeyboardKey::D;
        case SDLK_E:            return Event::KeyboardKey::E;
        case SDLK_F:            return Event::KeyboardKey::F;
        case SDLK_G:            return Event::KeyboardKey::G;
        case SDLK_H:            return Event::KeyboardKey::H;
        case SDLK_I:            return Event::KeyboardKey::I;
        case SDLK_J:            return Event::KeyboardKey::J;
        case SDLK_K:            return Event::KeyboardKey::K;
        case SDLK_L:            return Event::KeyboardKey::L;
        case SDLK_M:            return Event::KeyboardKey::M;
        case SDLK_N:            return Event::KeyboardKey::N;
        case SDLK_O:            return Event::KeyboardKey::O;
        case SDLK_P:            return Event::KeyboardKey::P;
        case SDLK_Q:            return Event::KeyboardKey::Q;
        case SDLK_R:            return Event::KeyboardKey::R;
        case SDLK_S:            return Event::KeyboardKey::S;
        case SDLK_T:            return Event::KeyboardKey::T;
        case SDLK_U:            return Event::KeyboardKey::U;
        case SDLK_V:            return Event::KeyboardKey::V;
        case SDLK_W:            return Event::KeyboardKey::W;
        case SDLK_X:            return Event::KeyboardKey::X;
        case SDLK_Y:            return Event::KeyboardKey::Y;
        case SDLK_Z:            return Event::KeyboardKey::Z;
#else
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
#endif
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
#if SDL_MAJOR_VERSION >= 3
        case SDLK_APOSTROPHE:   return Event::KeyboardKey::Quote;
#else
        case SDLK_QUOTE:        return Event::KeyboardKey::Quote;
#endif
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

#if SDL_MAJOR_VERSION < 3
    static std::uint32_t getWindowIdFromEvent(const SDL_Event& eventSDL)
    {
        switch (eventSDL.type)
        {
            case SDL_WINDOWEVENT:
                return eventSDL.window.windowID;

            case SDL_EVENT_TEXT_INPUT:
                return eventSDL.text.windowID;

            case SDL_EVENT_KEY_DOWN:
                return eventSDL.key.windowID;

            case SDL_EVENT_MOUSE_WHEEL:
                return eventSDL.wheel.windowID;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                return eventSDL.button.windowID;

            case SDL_EVENT_MOUSE_MOTION:
                return eventSDL.motion.windowID;

    #if ((SDL_MAJOR_VERSION == 2) && (SDL_MINOR_VERSION > 0)) || ((SDL_MAJOR_VERSION == 2) && (SDL_MINOR_VERSION == 0) && (SDL_PATCHLEVEL >= 12))
            case SDL_EVENT_FINGER_DOWN:
            case SDL_EVENT_FINGER_UP:
            case SDL_EVENT_FINGER_MOTION:
                return eventSDL.tfinger.windowID;
    #endif
            default:
                return 0;
        }
    }
#endif

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
#if SDL_MAJOR_VERSION >= 3
                const std::uint16_t modifiers = eventSDL.key.mod;
                const Event::KeyboardKey code = convertKeyCode(eventSDL.key.key);
#else
                const std::uint16_t modifiers = eventSDL.key.keysym.mod;
                const Event::KeyboardKey code = convertKeyCode(eventSDL.key.keysym.sym);
#endif
                if (code == Event::KeyboardKey::Unknown)
                    return false; // This key isn't handled by TGUI

                eventTGUI.type = Event::Type::KeyPressed;
                eventTGUI.key.code = code;
                eventTGUI.key.alt = ((modifiers & SDL_KMOD_ALT) != 0);
                eventTGUI.key.control = ((modifiers & SDL_KMOD_CTRL) != 0);
                eventTGUI.key.shift = ((modifiers & SDL_KMOD_SHIFT) != 0);
                eventTGUI.key.system = ((modifiers & SDL_KMOD_GUI) != 0);

                // If the NumLock is off then we will translate keypad key events to key events for text cursor navigation.
                // We only do this for SDL 2.0.22 or newer, because the NumLock state was incorrect on Linux prior to this version.
#if (SDL_MAJOR_VERSION > 2) || ((SDL_MAJOR_VERSION == 2) && (SDL_MINOR_VERSION > 0)) || ((SDL_MAJOR_VERSION == 2) && (SDL_MINOR_VERSION == 0) && (SDL_PATCHLEVEL >= 22))
                static_assert(static_cast<int>(Event::KeyboardKey::Numpad0) + 9 == static_cast<int>(Event::KeyboardKey::Numpad9), "Numpad0 to Numpad9 need continous ids in KeyboardKey");
                if (((modifiers & SDL_KMOD_NUM) == 0)
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

#if (SDL_MAJOR_VERSION >= 3)
                eventTGUI.mouseWheel.x = static_cast<int>(eventSDL.wheel.mouse_x * m_dpiScale);
                eventTGUI.mouseWheel.y = static_cast<int>(eventSDL.wheel.mouse_y * m_dpiScale);
#elif (SDL_MAJOR_VERSION > 2) || ((SDL_MAJOR_VERSION == 2) && (SDL_MINOR_VERSION >= 26))
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
#if (SDL_MAJOR_VERSION >= 3)
                m_touchFirstFingerId = eventSDL.tfinger.fingerID;
                m_touchFirstFingerTouchId = eventSDL.tfinger.touchID;
#else
                m_touchFirstFingerId = eventSDL.tfinger.fingerId;
                m_touchFirstFingerTouchId = eventSDL.tfinger.touchId;
#endif

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
#if (SDL_MAJOR_VERSION >= 3)
                if (!m_touchFirstFingerDown || (m_touchFirstFingerId != eventSDL.tfinger.fingerID) || (m_touchFirstFingerTouchId != eventSDL.tfinger.touchID))
                    return false;
#else
                if (!m_touchFirstFingerDown || (m_touchFirstFingerId != eventSDL.tfinger.fingerId) || (m_touchFirstFingerTouchId != eventSDL.tfinger.touchId))
                    return false;
#endif
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
#if (SDL_MAJOR_VERSION >= 3)
                if (!m_touchFirstFingerDown || (m_touchFirstFingerId != eventSDL.tfinger.fingerID) || (m_touchFirstFingerTouchId != eventSDL.tfinger.touchID))
                    return false;
#else
                if (!m_touchFirstFingerDown || (m_touchFirstFingerId != eventSDL.tfinger.fingerId) || (m_touchFirstFingerTouchId != eventSDL.tfinger.touchId))
                    return false;
#endif

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
        // Ignore events from a window other than the one used by the gui.
        // Not all events are related to a window, and manually crafted events will also not have a valid window id,
        // so if the event has no associated window then we will also process it.
#if SDL_MAJOR_VERSION >= 3
        SDL_Window* window = SDL_GetWindowFromEvent(&sdlEvent);
        if (window && (window != m_window))
            return false;
#else
        const std::uint32_t eventWindowId = getWindowIdFromEvent(sdlEvent);
        const std::uint32_t windowId = SDL_GetWindowID(m_window);
        if ((eventWindowId != 0) && (windowId != 0) && (eventWindowId != windowId))
            return false;
#endif

        // Detect scrolling with two fingers by examining touch events
        if ((sdlEvent.type == SDL_EVENT_FINGER_DOWN) || (sdlEvent.type == SDL_EVENT_FINGER_UP) || (sdlEvent.type == SDL_EVENT_FINGER_MOTION))
        {
            const bool wasScrolling = m_twoFingerScroll.isScrolling();

#if (SDL_MAJOR_VERSION >= 3)
            const auto fingerId = static_cast<std::intptr_t>(sdlEvent.tfinger.fingerID);
#else
            const auto fingerId = static_cast<std::intptr_t>(sdlEvent.tfinger.fingerId);
#endif
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
        if ((event.type == Event::Type::TextEntered) && (sdlEvent.type == SDL_EVENT_TEXT_INPUT) && (sdlEvent.text.text[0] != '\0') && (sdlEvent.text.text[1] != '\0'))
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
                // If there are no events then we will wait for a short time
                int timeout = static_cast<int>(std::chrono::milliseconds(getTimerWakeUpTime()).count());

                SDL_Event event;
                while (SDL_WaitEventTimeout(&event, timeout) != 0)
                {
                    // Once we processed one event, we will want to exit this loop as soon as there are no more events left.
                    // Calling SDL_WaitEventTimeout with timeout 0 is the same as calling SDL_PollEvent.
                    timeout = 0;

                    if (handleEvent(event))
                        eventProcessed = true;

                    if (event.type == SDL_EVENT_QUIT)
                    {
                        quit = true;
                        eventProcessed = true;
                    }
#if SDL_MAJOR_VERSION >= 3
                    else if ((event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) || (event.type == SDL_EVENT_WINDOW_EXPOSED)
                          || (event.type == SDL_EVENT_WINDOW_MOUSE_ENTER) || (event.type == SDL_EVENT_WINDOW_MOUSE_LEAVE)
                          || (event.type == SDL_EVENT_WINDOW_FOCUS_GAINED) || (event.type == SDL_EVENT_WINDOW_FOCUS_LOST))
#else
                    else if ((event.type == SDL_WINDOWEVENT)
                          && ((event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) || (event.window.event == SDL_WINDOWEVENT_EXPOSED)
                           || (event.window.event == SDL_WINDOWEVENT_ENTER) || (event.window.event == SDL_WINDOWEVENT_LEAVE)
                           || (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) || (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)))
#endif
                    {
                        eventProcessed = true;
                    }
                }

                if (updateTime())
                    break;

                if (eventProcessed || refreshRequired)
                    break;
            }

            refreshRequired = true;

            // Don't try to render too often, even when the screen is changing (e.g. during animation)
            const auto timePointNow = std::chrono::steady_clock::now();
            const auto timePointNextAllowed = lastRenderTime + std::chrono::milliseconds(15);
            if (timePointNextAllowed > timePointNow)
                continue;

            renderFrame(clearColor);

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

#if SDL_MAJOR_VERSION >= 3
            SDL_StartTextInput(m_window);
#else
            SDL_StartTextInput();
#endif
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
#if SDL_MAJOR_VERSION >= 3
            SDL_StopTextInput(m_window);
#else
            SDL_StopTextInput();
#endif
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
        SDL_Rect sdlRect;
#if SDL_MAJOR_VERSION >= 3
        sdlRect.x = static_cast<int>(std::round(topLeft.x / dpiScale));
        sdlRect.y = static_cast<int>(std::round(topLeft.y / dpiScale));
        sdlRect.w = static_cast<int>(std::round((bottomRight - topLeft).x / dpiScale));
        sdlRect.h = static_cast<int>(std::round((bottomRight - topLeft).y / dpiScale));
        SDL_SetTextInputArea(m_window, &sdlRect, static_cast<int>(std::round(caretPosPixels.x)));
#else
        // SDL positions the IME window at the left side of the input rect, but we want the window to follow the caret.
        // So we set the left side of the rectangle to the current location of the caret.
        sdlRect.x = static_cast<int>(std::round(std::max(caretPosPixels.x, topLeft.x) / dpiScale));
        sdlRect.y = static_cast<int>(std::round(topLeft.y / dpiScale));
        sdlRect.w = static_cast<int>(std::round(std::max(caretPosPixels.x, (bottomRight - topLeft).x) / dpiScale));
        sdlRect.h = static_cast<int>(std::round((bottomRight - topLeft).y / dpiScale));
        SDL_SetTextInputRect(&sdlRect);
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGuiSDL::isKeyboardModifierPressed(Event::KeyModifier modifierKey) const
    {
        const SDL_Keymod pressedModifiers = SDL_GetModState();
        switch (modifierKey)
        {
        case Event::KeyModifier::System:
            return (pressedModifiers & SDL_KMOD_GUI) != 0;
        case Event::KeyModifier::Control:
            return (pressedModifiers & SDL_KMOD_CTRL) != 0;
        case Event::KeyModifier::Shift:
            return (pressedModifiers & SDL_KMOD_SHIFT) != 0;
        case Event::KeyModifier::Alt:
            return (pressedModifiers & SDL_KMOD_ALT) != 0;
        }

        TGUI_ASSERT(false, "BackendGuiSDL::isKeyboardModifierPressed called with an invalid value");
        return false;
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

    void BackendGuiSDL::renderFrame(Color)
    {
        m_backendRenderTarget->clearScreen();
        draw();
        presentScreen(); // Call SDL_GL_SwapWindow or SDL_RenderPresent depending on the backend renderer
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
