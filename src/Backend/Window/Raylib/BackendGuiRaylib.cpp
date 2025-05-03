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

#include <TGUI/Backend/Window/Raylib/BackendGuiRaylib.hpp>
#include <TGUI/Backend/Window/Raylib/BackendRaylib.hpp>

#if !TGUI_BUILD_AS_CXX_MODULE
    #ifdef TGUI_SYSTEM_WINDOWS
        // raylib.h and windows.h give conflicts
        #define NOGDI
        #define NOUSER

        #include <TGUI/WindowsIMM.hpp>
    #endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    TGUI_NODISCARD static Event::KeyboardKey convertKeyCode(int key)
    {
        switch (key)
        {
            case KEY_APOSTROPHE:     return Event::KeyboardKey::Quote;
            case KEY_COMMA:          return Event::KeyboardKey::Comma;
            case KEY_MINUS:          return Event::KeyboardKey::Minus;
            case KEY_PERIOD:         return Event::KeyboardKey::Period;
            case KEY_SLASH:          return Event::KeyboardKey::Slash;
            case KEY_ZERO:           return Event::KeyboardKey::Num0;
            case KEY_ONE:            return Event::KeyboardKey::Num1;
            case KEY_TWO:            return Event::KeyboardKey::Num2;
            case KEY_THREE:          return Event::KeyboardKey::Num3;
            case KEY_FOUR:           return Event::KeyboardKey::Num4;
            case KEY_FIVE:           return Event::KeyboardKey::Num5;
            case KEY_SIX:            return Event::KeyboardKey::Num6;
            case KEY_SEVEN:          return Event::KeyboardKey::Num7;
            case KEY_EIGHT:          return Event::KeyboardKey::Num8;
            case KEY_NINE:           return Event::KeyboardKey::Num9;
            case KEY_SEMICOLON:      return Event::KeyboardKey::Semicolon;
            case KEY_EQUAL:          return Event::KeyboardKey::Equal;
            case KEY_A:              return Event::KeyboardKey::A;
            case KEY_B:              return Event::KeyboardKey::B;
            case KEY_C:              return Event::KeyboardKey::C;
            case KEY_D:              return Event::KeyboardKey::D;
            case KEY_E:              return Event::KeyboardKey::E;
            case KEY_F:              return Event::KeyboardKey::F;
            case KEY_G:              return Event::KeyboardKey::G;
            case KEY_H:              return Event::KeyboardKey::H;
            case KEY_I:              return Event::KeyboardKey::I;
            case KEY_J:              return Event::KeyboardKey::J;
            case KEY_K:              return Event::KeyboardKey::K;
            case KEY_L:              return Event::KeyboardKey::L;
            case KEY_M:              return Event::KeyboardKey::M;
            case KEY_N:              return Event::KeyboardKey::N;
            case KEY_O:              return Event::KeyboardKey::O;
            case KEY_P:              return Event::KeyboardKey::P;
            case KEY_Q:              return Event::KeyboardKey::Q;
            case KEY_R:              return Event::KeyboardKey::R;
            case KEY_S:              return Event::KeyboardKey::S;
            case KEY_T:              return Event::KeyboardKey::T;
            case KEY_U:              return Event::KeyboardKey::U;
            case KEY_V:              return Event::KeyboardKey::V;
            case KEY_W:              return Event::KeyboardKey::W;
            case KEY_X:              return Event::KeyboardKey::X;
            case KEY_Y:              return Event::KeyboardKey::Y;
            case KEY_Z:              return Event::KeyboardKey::Z;
            case KEY_LEFT_BRACKET:   return Event::KeyboardKey::LBracket;
            case KEY_BACKSLASH:      return Event::KeyboardKey::Backslash;
            case KEY_RIGHT_BRACKET:  return Event::KeyboardKey::RBracket;
            case KEY_SPACE:          return Event::KeyboardKey::Space;
            case KEY_ESCAPE:         return Event::KeyboardKey::Escape;
            case KEY_ENTER:          return Event::KeyboardKey::Enter;
            case KEY_TAB:            return Event::KeyboardKey::Tab;
            case KEY_BACKSPACE:      return Event::KeyboardKey::Backspace;
            case KEY_INSERT:         return Event::KeyboardKey::Insert;
            case KEY_DELETE:         return Event::KeyboardKey::Delete;
            case KEY_RIGHT:          return Event::KeyboardKey::Right;
            case KEY_LEFT:           return Event::KeyboardKey::Left;
            case KEY_DOWN:           return Event::KeyboardKey::Down;
            case KEY_UP:             return Event::KeyboardKey::Up;
            case KEY_PAGE_UP:        return Event::KeyboardKey::PageUp;
            case KEY_PAGE_DOWN:      return Event::KeyboardKey::PageDown;
            case KEY_HOME:           return Event::KeyboardKey::Home;
            case KEY_END:            return Event::KeyboardKey::End;
            case KEY_PAUSE:          return Event::KeyboardKey::Pause;
            case KEY_F1:             return Event::KeyboardKey::F1;
            case KEY_F2:             return Event::KeyboardKey::F2;
            case KEY_F3:             return Event::KeyboardKey::F3;
            case KEY_F4:             return Event::KeyboardKey::F4;
            case KEY_F5:             return Event::KeyboardKey::F5;
            case KEY_F6:             return Event::KeyboardKey::F6;
            case KEY_F7:             return Event::KeyboardKey::F7;
            case KEY_F8:             return Event::KeyboardKey::F8;
            case KEY_F9:             return Event::KeyboardKey::F9;
            case KEY_F10:            return Event::KeyboardKey::F10;
            case KEY_F11:            return Event::KeyboardKey::F11;
            case KEY_F12:            return Event::KeyboardKey::F12;
            case KEY_LEFT_SHIFT:     return Event::KeyboardKey::LShift;
            case KEY_LEFT_CONTROL:   return Event::KeyboardKey::LControl;
            case KEY_LEFT_ALT:       return Event::KeyboardKey::LAlt;
            case KEY_LEFT_SUPER:     return Event::KeyboardKey::LSystem;
            case KEY_RIGHT_SHIFT:    return Event::KeyboardKey::RShift;
            case KEY_RIGHT_CONTROL:  return Event::KeyboardKey::RControl;
            case KEY_RIGHT_ALT:      return Event::KeyboardKey::RAlt;
            case KEY_RIGHT_SUPER:    return Event::KeyboardKey::RSystem;
            case KEY_KB_MENU:        return Event::KeyboardKey::Menu;
            case KEY_KP_0:           return Event::KeyboardKey::Numpad0;
            case KEY_KP_1:           return Event::KeyboardKey::Numpad1;
            case KEY_KP_2:           return Event::KeyboardKey::Numpad2;
            case KEY_KP_3:           return Event::KeyboardKey::Numpad3;
            case KEY_KP_4:           return Event::KeyboardKey::Numpad4;
            case KEY_KP_5:           return Event::KeyboardKey::Numpad5;
            case KEY_KP_6:           return Event::KeyboardKey::Numpad6;
            case KEY_KP_7:           return Event::KeyboardKey::Numpad7;
            case KEY_KP_8:           return Event::KeyboardKey::Numpad8;
            case KEY_KP_9:           return Event::KeyboardKey::Numpad9;
            case KEY_KP_DECIMAL:     return Event::KeyboardKey::Period;
            case KEY_KP_DIVIDE:      return Event::KeyboardKey::Divide;
            case KEY_KP_MULTIPLY:    return Event::KeyboardKey::Multiply;
            case KEY_KP_SUBTRACT:    return Event::KeyboardKey::Subtract;
            case KEY_KP_ADD:         return Event::KeyboardKey::Add;
            case KEY_KP_ENTER:       return Event::KeyboardKey::Enter;
            case KEY_KP_EQUAL:       return Event::KeyboardKey::Equal;
            default: // We don't process the other keys
                return Event::KeyboardKey::Unknown;
        };
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendGuiRaylib::BackendGuiRaylib() :
        m_mouseOnWindow(IsCursorOnScreen())
    {
#ifdef TGUI_SYSTEM_WINDOWS
        WindowsIMM::initialize();
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendGuiRaylib::~BackendGuiRaylib()
    {
#ifdef TGUI_SYSTEM_WINDOWS
        WindowsIMM::release();
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGuiRaylib::handleEvents()
    {
        bool handled = false;
        for (const auto& event : generateEventQueue(false))
            handled |= handleEvent(event);

        return handled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGuiRaylib::handleCharPressed(int pressedChar)
    {
        if (pressedChar <= 0)
            return false;

        Event event;
        event.type = Event::Type::TextEntered;
        event.text.unicode = static_cast<char32_t>(pressedChar);
        return handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGuiRaylib::handleKeyPressed(int pressedKey)
    {
        if (pressedKey <= 0)
            return false;

        if ((pressedKey == KEY_LEFT_ALT) || (pressedKey == KEY_RIGHT_ALT)
         || (pressedKey == KEY_LEFT_CONTROL) || (pressedKey == KEY_RIGHT_CONTROL)
         || (pressedKey == KEY_LEFT_SHIFT) || (pressedKey == KEY_RIGHT_SHIFT)
         || (pressedKey == KEY_LEFT_SUPER) || (pressedKey == KEY_RIGHT_SUPER))
        {
            return false;
        }

        const Event::KeyboardKey code = convertKeyCode(pressedKey);
        if (code == Event::KeyboardKey::Unknown)
            return false;

        Event event;
        event.type = Event::Type::KeyPressed;
        event.key.code = code;
        event.key.alt = IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT);
        event.key.control = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
        event.key.shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        event.key.system = IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER);
        return handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGuiRaylib::mainLoop(Color clearColor)
    {
        m_endMainLoop = false;
        m_backendRenderTarget->setClearColor(clearColor);

        // In raylib the EndDrawing function handles event polling and it has no equivalent to a glfwWaitEventsTimeout function.
        // This means that we have no way of waiting for events while still running our timers (EnableEventWaiting will wait for
        // an event but won't wake up when one of our timers expires).
        // So we have to rely on SetTargetFPS to reduce drawing. This isn't ideal, because no events are processed during sleeping
        // (causing delays in input/timer processing when the FPS is set too low), and no sleeping is performed at all on
        // low end systems that can't reach the target FPS. We let the user call the function themselves before calling mainLoop
        // so that they can choose the compromise instead of hardcoding a value here.

        while (!WindowShouldClose() && !m_endMainLoop)
        {
            for (const auto& event : generateEventQueue(true))
                handleEvent(event);

            BeginDrawing();
            m_backendRenderTarget->clearScreen();
            draw();
            EndDrawing();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef TGUI_SYSTEM_WINDOWS
    void BackendGuiRaylib::updateTextCursorPosition(FloatRect, Vector2f caretPos)
    {
        WindowsIMM::setCandidateWindowPosition(static_cast<HWND>(GetWindowHandle()), mapCoordsToPixel(caretPos));
    }
#else
    void BackendGuiRaylib::updateTextCursorPosition(FloatRect inputRect, Vector2f caretPos)
    {
        BackendGui::updateTextCursorPosition(inputRect, caretPos);
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGuiRaylib::isKeyboardModifierPressed(Event::KeyModifier modifierKey) const
    {
        switch (modifierKey)
        {
        case Event::KeyModifier::System:
            return IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER);
        case Event::KeyModifier::Control:
            return IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
        case Event::KeyModifier::Shift:
            return IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        case Event::KeyModifier::Alt:
            return IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT);
        }

        TGUI_ASSERT(false, "BackendGuiRaylib::isKeyboardModifierPressed called with an invalid value");
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGuiRaylib::endMainLoop()
    {
        m_endMainLoop = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<Event> BackendGuiRaylib::generateEventQueue(bool processKeyEvents)
    {
        std::vector<Event> events;

        const bool focused = IsWindowFocused();
        if (focused != m_windowFocused)
        {
            TGUI_EMPLACE_BACK(event, events)
            if (focused)
                event.type = Event::Type::GainedFocus;
            else
                event.type = Event::Type::LostFocus;
        }

        if (IsWindowResized())
        {
            TGUI_EMPLACE_BACK(event, events)
            event.type = Event::Type::Resized;
            event.size.width = static_cast<unsigned int>(GetRenderWidth());
            event.size.height = static_cast<unsigned int>(GetRenderHeight());
        }

        const bool mouseOnWindow = IsCursorOnScreen();
        if (mouseOnWindow != m_mouseOnWindow)
        {
            TGUI_EMPLACE_BACK(event, events)
            if (mouseOnWindow)
                event.type = Event::Type::MouseEntered;
            else
                event.type = Event::Type::MouseLeft;

            m_mouseOnWindow = mouseOnWindow;
        }

        const int mouseX = GetMouseX();
        const int mouseY = GetMouseY();
        if ((mouseX != m_lastMousePos.x) || (mouseY != m_lastMousePos.y))
        {
            TGUI_EMPLACE_BACK(event, events)
            event.type = Event::Type::MouseMoved;
            event.mouseMove.x = mouseX;
            event.mouseMove.y = mouseY;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            TGUI_EMPLACE_BACK(event, events)
            event.type = Event::Type::MouseButtonPressed;
            event.mouseButton.button = Event::MouseButton::Left;
            event.mouseButton.x = mouseX;
            event.mouseButton.y = mouseY;
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE))
        {
            TGUI_EMPLACE_BACK(event, events)
            event.type = Event::Type::MouseButtonPressed;
            event.mouseButton.button = Event::MouseButton::Middle;
            event.mouseButton.x = mouseX;
            event.mouseButton.y = mouseY;
        }
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            TGUI_EMPLACE_BACK(event, events)
            event.type = Event::Type::MouseButtonPressed;
            event.mouseButton.button = Event::MouseButton::Right;
            event.mouseButton.x = mouseX;
            event.mouseButton.y = mouseY;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            TGUI_EMPLACE_BACK(event, events)
            event.type = Event::Type::MouseButtonReleased;
            event.mouseButton.button = Event::MouseButton::Left;
            event.mouseButton.x = mouseX;
            event.mouseButton.y = mouseY;
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE))
        {
            TGUI_EMPLACE_BACK(event, events)
            event.type = Event::Type::MouseButtonReleased;
            event.mouseButton.button = Event::MouseButton::Middle;
            event.mouseButton.x = mouseX;
            event.mouseButton.y = mouseY;
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
        {
            TGUI_EMPLACE_BACK(event, events)
            event.type = Event::Type::MouseButtonReleased;
            event.mouseButton.button = Event::MouseButton::Right;
            event.mouseButton.x = mouseX;
            event.mouseButton.y = mouseY;
        }

        const float mouseWheelDelta = GetMouseWheelMoveV().y;
        if (mouseWheelDelta != 0)
        {
            TGUI_EMPLACE_BACK(event, events)
            event.type = Event::Type::MouseWheelScrolled;
            event.mouseWheel.delta = mouseWheelDelta;
            event.mouseWheel.x = mouseX;
            event.mouseWheel.y = mouseY;
        }

        if (processKeyEvents)
        {
            int pressedChar = GetCharPressed();
            while (pressedChar > 0)
            {
                TGUI_EMPLACE_BACK(event, events)
                event.type = Event::Type::TextEntered;
                event.text.unicode = static_cast<char32_t>(pressedChar);

                pressedChar = GetCharPressed();
            }

            int pressedKey = GetKeyPressed();
            while (pressedKey > 0)
            {
                if ((pressedKey == KEY_LEFT_ALT) || (pressedKey == KEY_RIGHT_ALT)
                 || (pressedKey == KEY_LEFT_CONTROL) || (pressedKey == KEY_RIGHT_CONTROL)
                 || (pressedKey == KEY_LEFT_SHIFT) || (pressedKey == KEY_RIGHT_SHIFT)
                 || (pressedKey == KEY_LEFT_SUPER) || (pressedKey == KEY_RIGHT_SUPER))
                {
                    continue;
                }

                const Event::KeyboardKey code = convertKeyCode(pressedKey);
                if (code == Event::KeyboardKey::Unknown)
                    continue;

                TGUI_EMPLACE_BACK(event, events)
                event.type = Event::Type::KeyPressed;
                event.key.code = code;
                event.key.alt = IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT);
                event.key.control = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
                event.key.shift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
                event.key.system = IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER);

                pressedKey = GetKeyPressed();
            }

            if (m_getLastSoftKeyUnicodeFunction)
            {
                pressedChar = m_getLastSoftKeyUnicodeFunction();
                if (pressedChar > 0)
                {
                    TGUI_EMPLACE_BACK(event, events)
                    event.type = Event::Type::TextEntered;
                    event.text.unicode = static_cast<char32_t>(pressedChar);

                    if (m_clearLastSoftKeyFunction)
                        m_clearLastSoftKeyFunction();
                }
            }
        }

        // TODO: Touch events

        return events;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGuiRaylib::startTextInput(FloatRect)
    {
        if (m_showSoftKeyboardFunction)
            m_showSoftKeyboardFunction();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGuiRaylib::stopTextInput()
    {
        if (m_hideSoftKeyboardFunction)
            m_hideSoftKeyboardFunction();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGuiRaylib::setSoftKeyboardFunctions(std::function<void()> showKeyboardFunc,
                                                    std::function<void()> hideKeyboardFunc,
                                                    std::function<int()> getKeyUnicodeFunc,
                                                    std::function<void()> clearLastKeyFunc)
    {
        m_showSoftKeyboardFunction = showKeyboardFunc;
        m_hideSoftKeyboardFunction = hideKeyboardFunc;
        m_getLastSoftKeyUnicodeFunction = getKeyUnicodeFunc;
        m_clearLastSoftKeyFunction = clearLastKeyFunc;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGuiRaylib::updateContainerSize()
    {
        m_framebufferSize = {GetRenderWidth(), GetRenderHeight()};

        BackendGui::updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
