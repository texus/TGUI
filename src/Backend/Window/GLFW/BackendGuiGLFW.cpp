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


#include <TGUI/Backend/Window/GLFW/BackendGuiGLFW.hpp>
#include <TGUI/Backend/Window/GLFW/BackendGLFW.hpp>

#if TGUI_BUILD_AS_CXX_MODULE
    import tgui;
#else
    #include <TGUI/Timer.hpp>

    #ifdef TGUI_SYSTEM_WINDOWS
        #include <TGUI/WindowsIMM.hpp>
    #endif
#endif

#define GLFW_INCLUDE_NONE // Don't let GLFW include an OpenGL extention loader
#include <GLFW/glfw3.h>

#ifdef TGUI_SYSTEM_WINDOWS
    #define GLFW_EXPOSE_NATIVE_WIN32
    #define GLFW_NATIVE_INCLUDE_NONE
    #include <GLFW/glfw3native.h>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_NODISCARD static Event::KeyboardKey convertKeyCode(int key)
    {
        switch (key)
        {
        case GLFW_KEY_A:             return Event::KeyboardKey::A;
        case GLFW_KEY_B:             return Event::KeyboardKey::B;
        case GLFW_KEY_C:             return Event::KeyboardKey::C;
        case GLFW_KEY_D:             return Event::KeyboardKey::D;
        case GLFW_KEY_E:             return Event::KeyboardKey::E;
        case GLFW_KEY_F:             return Event::KeyboardKey::F;
        case GLFW_KEY_G:             return Event::KeyboardKey::G;
        case GLFW_KEY_H:             return Event::KeyboardKey::H;
        case GLFW_KEY_I:             return Event::KeyboardKey::I;
        case GLFW_KEY_J:             return Event::KeyboardKey::J;
        case GLFW_KEY_K:             return Event::KeyboardKey::K;
        case GLFW_KEY_L:             return Event::KeyboardKey::L;
        case GLFW_KEY_M:             return Event::KeyboardKey::M;
        case GLFW_KEY_N:             return Event::KeyboardKey::N;
        case GLFW_KEY_O:             return Event::KeyboardKey::O;
        case GLFW_KEY_P:             return Event::KeyboardKey::P;
        case GLFW_KEY_Q:             return Event::KeyboardKey::Q;
        case GLFW_KEY_R:             return Event::KeyboardKey::R;
        case GLFW_KEY_S:             return Event::KeyboardKey::S;
        case GLFW_KEY_T:             return Event::KeyboardKey::T;
        case GLFW_KEY_U:             return Event::KeyboardKey::U;
        case GLFW_KEY_V:             return Event::KeyboardKey::V;
        case GLFW_KEY_W:             return Event::KeyboardKey::W;
        case GLFW_KEY_X:             return Event::KeyboardKey::X;
        case GLFW_KEY_Y:             return Event::KeyboardKey::Y;
        case GLFW_KEY_Z:             return Event::KeyboardKey::Z;
        case GLFW_KEY_0:             return Event::KeyboardKey::Num0;
        case GLFW_KEY_1:             return Event::KeyboardKey::Num1;
        case GLFW_KEY_2:             return Event::KeyboardKey::Num2;
        case GLFW_KEY_3:             return Event::KeyboardKey::Num3;
        case GLFW_KEY_4:             return Event::KeyboardKey::Num4;
        case GLFW_KEY_5:             return Event::KeyboardKey::Num5;
        case GLFW_KEY_6:             return Event::KeyboardKey::Num6;
        case GLFW_KEY_7:             return Event::KeyboardKey::Num7;
        case GLFW_KEY_8:             return Event::KeyboardKey::Num8;
        case GLFW_KEY_9:             return Event::KeyboardKey::Num9;
        case GLFW_KEY_ESCAPE:        return Event::KeyboardKey::Escape;
        case GLFW_KEY_LEFT_CONTROL:  return Event::KeyboardKey::LControl;
        case GLFW_KEY_LEFT_SHIFT:    return Event::KeyboardKey::LShift;
        case GLFW_KEY_LEFT_ALT:      return Event::KeyboardKey::LAlt;
        case GLFW_KEY_LEFT_SUPER:    return Event::KeyboardKey::LSystem;
        case GLFW_KEY_RIGHT_CONTROL: return Event::KeyboardKey::RControl;
        case GLFW_KEY_RIGHT_SHIFT:   return Event::KeyboardKey::RShift;
        case GLFW_KEY_RIGHT_ALT:     return Event::KeyboardKey::RAlt;
        case GLFW_KEY_RIGHT_SUPER:   return Event::KeyboardKey::RSystem;
        case GLFW_KEY_MENU:          return Event::KeyboardKey::Menu;
        case GLFW_KEY_LEFT_BRACKET:  return Event::KeyboardKey::LBracket;
        case GLFW_KEY_RIGHT_BRACKET: return Event::KeyboardKey::RBracket;
        case GLFW_KEY_SEMICOLON:     return Event::KeyboardKey::Semicolon;
        case GLFW_KEY_COMMA:         return Event::KeyboardKey::Comma;
        case GLFW_KEY_PERIOD:        return Event::KeyboardKey::Period;
        case GLFW_KEY_APOSTROPHE:    return Event::KeyboardKey::Quote;
        case GLFW_KEY_SLASH:         return Event::KeyboardKey::Slash;
        case GLFW_KEY_BACKSLASH:     return Event::KeyboardKey::Backslash;
        case GLFW_KEY_EQUAL:         return Event::KeyboardKey::Equal;
        case GLFW_KEY_MINUS:         return Event::KeyboardKey::Minus;
        case GLFW_KEY_SPACE:         return Event::KeyboardKey::Space;
        case GLFW_KEY_ENTER:         return Event::KeyboardKey::Enter;
        case GLFW_KEY_BACKSPACE:     return Event::KeyboardKey::Backspace;
        case GLFW_KEY_TAB:           return Event::KeyboardKey::Tab;
        case GLFW_KEY_PAGE_UP:       return Event::KeyboardKey::PageUp;
        case GLFW_KEY_PAGE_DOWN:     return Event::KeyboardKey::PageDown;
        case GLFW_KEY_END:           return Event::KeyboardKey::End;
        case GLFW_KEY_HOME:          return Event::KeyboardKey::Home;
        case GLFW_KEY_INSERT:        return Event::KeyboardKey::Insert;
        case GLFW_KEY_DELETE:        return Event::KeyboardKey::Delete;
        case GLFW_KEY_KP_ADD:        return Event::KeyboardKey::Add;
        case GLFW_KEY_KP_SUBTRACT:   return Event::KeyboardKey::Subtract;
        case GLFW_KEY_KP_MULTIPLY:   return Event::KeyboardKey::Multiply;
        case GLFW_KEY_KP_DIVIDE:     return Event::KeyboardKey::Divide;
        case GLFW_KEY_KP_DECIMAL:    return Event::KeyboardKey::Period;
        case GLFW_KEY_KP_EQUAL:      return Event::KeyboardKey::Equal;
        case GLFW_KEY_LEFT:          return Event::KeyboardKey::Left;
        case GLFW_KEY_RIGHT:         return Event::KeyboardKey::Right;
        case GLFW_KEY_UP:            return Event::KeyboardKey::Up;
        case GLFW_KEY_DOWN:          return Event::KeyboardKey::Down;
        case GLFW_KEY_KP_0:          return Event::KeyboardKey::Numpad0;
        case GLFW_KEY_KP_1:          return Event::KeyboardKey::Numpad1;
        case GLFW_KEY_KP_2:          return Event::KeyboardKey::Numpad2;
        case GLFW_KEY_KP_3:          return Event::KeyboardKey::Numpad3;
        case GLFW_KEY_KP_4:          return Event::KeyboardKey::Numpad4;
        case GLFW_KEY_KP_5:          return Event::KeyboardKey::Numpad5;
        case GLFW_KEY_KP_6:          return Event::KeyboardKey::Numpad6;
        case GLFW_KEY_KP_7:          return Event::KeyboardKey::Numpad7;
        case GLFW_KEY_KP_8:          return Event::KeyboardKey::Numpad8;
        case GLFW_KEY_KP_9:          return Event::KeyboardKey::Numpad9;
        case GLFW_KEY_F1:            return Event::KeyboardKey::F1;
        case GLFW_KEY_F2:            return Event::KeyboardKey::F2;
        case GLFW_KEY_F3:            return Event::KeyboardKey::F3;
        case GLFW_KEY_F4:            return Event::KeyboardKey::F4;
        case GLFW_KEY_F5:            return Event::KeyboardKey::F5;
        case GLFW_KEY_F6:            return Event::KeyboardKey::F6;
        case GLFW_KEY_F7:            return Event::KeyboardKey::F7;
        case GLFW_KEY_F8:            return Event::KeyboardKey::F8;
        case GLFW_KEY_F9:            return Event::KeyboardKey::F9;
        case GLFW_KEY_F10:           return Event::KeyboardKey::F10;
        case GLFW_KEY_F11:           return Event::KeyboardKey::F11;
        case GLFW_KEY_F12:           return Event::KeyboardKey::F12;
        case GLFW_KEY_F13:           return Event::KeyboardKey::F13;
        case GLFW_KEY_F14:           return Event::KeyboardKey::F14;
        case GLFW_KEY_F15:           return Event::KeyboardKey::F15;
        case GLFW_KEY_PAUSE:         return Event::KeyboardKey::Pause;
        default: // We don't process the other keys
            return Event::KeyboardKey::Unknown;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendGuiGLFW::BackendGuiGLFW()
    {
#ifdef TGUI_SYSTEM_WINDOWS
        WindowsIMM::initialize();
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendGuiGLFW::~BackendGuiGLFW()
    {
#ifdef TGUI_SYSTEM_WINDOWS
        WindowsIMM::release();
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGuiGLFW::windowFocusCallback(int focused)
    {
        const auto event = convertWindowFocusEvent(focused);
        if (event)
            return handleEvent(*event);
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGuiGLFW::sizeCallback(int width, int height)
    {
        const auto event = convertSizeEvent(width, height);
        if (event)
            return handleEvent(*event);
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGuiGLFW::charCallback(unsigned int codepoint)
    {
        const auto event = convertCharEvent(codepoint);
        if (event)
            return handleEvent(*event);
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGuiGLFW::keyCallback(int key, int scancode, int action, int mods)
    {
        const auto event = convertKeyEvent(key, scancode, action, mods);
        if (event)
            return handleEvent(*event);
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGuiGLFW::scrollCallback(double xoffset, double yoffset)
    {
        const auto event = convertScrollEvent(xoffset, yoffset);
        if (event)
            return handleEvent(*event);
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGuiGLFW::cursorPosCallback(double xpos, double ypos)
    {
        const auto event = convertCursorPosEvent(xpos, ypos);
        if (event)
            return handleEvent(*event);
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGuiGLFW::mouseButtonCallback(int button, int action, int mods)
    {
        const auto event = convertMouseButtonEvent(button, action, mods);
        if (event)
            return handleEvent(*event);
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGuiGLFW::cursorEnterCallback(int entered)
    {
        const auto event = convertCursorEnterEvent(entered);
        if (event)
            return handleEvent(*event);
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Optional<Event> BackendGuiGLFW::convertWindowFocusEvent(int focused)
    {
        Event event;
        if (focused == GLFW_TRUE)
            event.type = Event::Type::GainedFocus;
        else
            event.type = Event::Type::LostFocus;

        return event;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Optional<Event> BackendGuiGLFW::convertSizeEvent(int width, int height)
    {
        Event event;
        event.type = Event::Type::Resized;
        event.size.width = static_cast<unsigned int>(width);
        event.size.height = static_cast<unsigned int>(height);
        return event;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Optional<Event> BackendGuiGLFW::convertCharEvent(unsigned int codepoint)
    {
        Event event;
        event.type = Event::Type::TextEntered;
        event.text.unicode = static_cast<char32_t>(codepoint);
        return event;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Optional<Event> BackendGuiGLFW::convertKeyEvent(int key, int, int action, int mods)
    {
        if ((action != GLFW_PRESS) && (action != GLFW_REPEAT))
            return {};

        const Event::KeyboardKey code = convertKeyCode(key);
        if (code == Event::KeyboardKey::Unknown)
            return {}; // This key isn't handled by TGUI

        Event event;
        event.type = Event::Type::KeyPressed;
        event.key.code = code;
        event.key.alt = ((mods & GLFW_MOD_ALT) != 0);
        event.key.control = ((mods & GLFW_MOD_CONTROL) != 0);
        event.key.shift = ((mods & GLFW_MOD_SHIFT) != 0);
        event.key.system = ((mods & GLFW_MOD_SUPER) != 0);
        return event;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Optional<Event> BackendGuiGLFW::convertScrollEvent(double, double yoffset)
    {
        if (yoffset == 0)
            return {}; // TGUI only handles the vertical mouse wheel

        Event event;
        event.type = Event::Type::MouseWheelScrolled;
        event.mouseWheel.delta = static_cast<float>(yoffset);

        // GLFW doesn't include the mouse position in mouse wheel events, so we add the last known position ourself
        event.mouseWheel.x = static_cast<int>(m_lastMousePos.x);
        event.mouseWheel.y = static_cast<int>(m_lastMousePos.y);
        return event;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Optional<Event> BackendGuiGLFW::convertCursorPosEvent(double xpos, double ypos)
    {
        Event event;
        event.type = Event::Type::MouseMoved;
        event.mouseMove.x = static_cast<int>(xpos);
        event.mouseMove.y = static_cast<int>(ypos);
        return event;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Optional<Event> BackendGuiGLFW::convertMouseButtonEvent(int button, int action, int)
    {
        if (((action != GLFW_PRESS) && (action != GLFW_RELEASE))
         || ((button != GLFW_MOUSE_BUTTON_LEFT) && (button != GLFW_MOUSE_BUTTON_RIGHT) && (button != GLFW_MOUSE_BUTTON_MIDDLE)))
            return {};

        Event event;
        if (action == GLFW_PRESS)
            event.type = Event::Type::MouseButtonPressed;
        else
            event.type = Event::Type::MouseButtonReleased;

        if (button == GLFW_MOUSE_BUTTON_LEFT)
            event.mouseButton.button = Event::MouseButton::Left;
        else if (button == GLFW_MOUSE_BUTTON_RIGHT)
            event.mouseButton.button = Event::MouseButton::Right;
        else
            event.mouseButton.button = Event::MouseButton::Middle;

        // GLFW doesn't include the mouse position in mouse button events, so we add the last known position ourself
        event.mouseButton.x = static_cast<int>(m_lastMousePos.x);
        event.mouseButton.y = static_cast<int>(m_lastMousePos.y);
        return event;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Optional<Event> BackendGuiGLFW::convertCursorEnterEvent(int entered)
    {
        Event event;
        if (entered)
            event.type = Event::Type::MouseEntered;
        else
            event.type = Event::Type::MouseLeft;

        return event;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGuiGLFW::mainLoop(Color clearColor)
    {
        TGUI_ASSERT(m_window && m_backendRenderTarget, "Gui must be given an GLFWwindow (either at construction or via setWindow function) before mainLoop() is called");

        m_backendRenderTarget->setClearColor(clearColor);

        glfwSwapInterval(1);
        glfwSetWindowUserPointer(m_window, this);

        glfwSetWindowFocusCallback(m_window, [](GLFWwindow* window, int focused){
            static_cast<BackendGuiGLFW*>(glfwGetWindowUserPointer(window))->windowFocusCallback(focused);
        });
        glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height){
            static_cast<BackendGuiGLFW*>(glfwGetWindowUserPointer(window))->sizeCallback(width, height);
        });
        glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int codepoint){
            static_cast<BackendGuiGLFW*>(glfwGetWindowUserPointer(window))->charCallback(codepoint);
        });
        glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods){
            static_cast<BackendGuiGLFW*>(glfwGetWindowUserPointer(window))->keyCallback(key, scancode, action, mods);
        });
        glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset){
            static_cast<BackendGuiGLFW*>(glfwGetWindowUserPointer(window))->scrollCallback(xoffset, yoffset);
        });
        glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos){
            static_cast<BackendGuiGLFW*>(glfwGetWindowUserPointer(window))->cursorPosCallback(xpos, ypos);
        });
        glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods){
            static_cast<BackendGuiGLFW*>(glfwGetWindowUserPointer(window))->mouseButtonCallback(button, action, mods);
        });
        glfwSetCursorEnterCallback(m_window, [](GLFWwindow* window, int entered) {
            static_cast<BackendGuiGLFW*>(glfwGetWindowUserPointer(window))->cursorEnterCallback(entered);
        });

        setDrawingUpdatesTime(false);

        std::chrono::steady_clock::time_point lastRenderTime;
        while (!glfwWindowShouldClose(m_window))
        {
            // Don't try to render too often, even when the screen is changing (e.g. during animation)
            const auto timePointNow = std::chrono::steady_clock::now();
            const auto timePointNextAllowed = lastRenderTime + std::chrono::milliseconds(15);
            if (timePointNextAllowed <= timePointNow)
            {
                m_backendRenderTarget->clearScreen();
                draw();
                glfwSwapBuffers(m_window);
                lastRenderTime = std::chrono::steady_clock::now(); // Don't use timePointNow to provide enough rest on low-end hardware
            }

            double eventTimeoutSeconds = 0.01;
            Optional<Duration> duration = Timer::getNextScheduledTime();
            if (duration && (*duration < std::chrono::milliseconds(10)))
                eventTimeoutSeconds = static_cast<double>(duration->asSeconds());

            glfwWaitEventsTimeout(eventTimeoutSeconds);
            updateTime();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GLFWwindow* BackendGuiGLFW::getWindow() const
    {
        return m_window;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef TGUI_SYSTEM_WINDOWS
    void BackendGuiGLFW::updateTextCursorPosition(FloatRect, Vector2f caretPos)
    {
        if (!m_window)
            return;

        WindowsIMM::setCandidateWindowPosition(glfwGetWin32Window(m_window), mapCoordsToPixel(caretPos));
    }
#else
    void BackendGuiGLFW::updateTextCursorPosition(FloatRect inputRect, Vector2f caretPos)
    {
        BackendGui::updateTextCursorPosition(inputRect, caretPos);
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGuiGLFW::setGuiWindow(GLFWwindow* window)
    {
        TGUI_ASSERT(std::dynamic_pointer_cast<BackendGLFW>(getBackend()), "BackendGuiGLFW requires system backend of type BackendGLFW");

        m_window = window;

        getBackend()->attachGui(this);
        std::static_pointer_cast<BackendGLFW>(getBackend())->setGuiWindow(this, window);

        updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGuiGLFW::updateContainerSize()
    {
        // We can't do anything yet if we don't have a window
        if (!m_window)
            return;

        glfwGetFramebufferSize(m_window, &m_framebufferSize.x, &m_framebufferSize.y);

        BackendGui::updateContainerSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
