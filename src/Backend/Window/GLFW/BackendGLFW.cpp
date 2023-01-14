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


#include <TGUI/Backend/Window/GLFW/BackendGLFW.hpp>

#define GLFW_INCLUDE_NONE // Don't let GLFW include an OpenGL extention loader
#include <GLFW/glfw3.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendGLFW::~BackendGLFW()
    {
        for (auto& cursor : m_mouseCursors)
        {
            if (cursor.second)
                glfwDestroyCursor(cursor.second);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGLFW::setGuiWindow(BackendGui* gui, GLFWwindow* window)
    {
        TGUI_ASSERT(m_guis.find(gui) != m_guis.end(), "BackendGLFW::setGuiWindow called with a gui that wasn't attached");
        m_guiResources[gui].window = window;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGLFW::detatchGui(BackendGui* gui)
    {
        // Don't check if it existed, detach is called for every gui while attached is only called for properly initialized guis
        m_guiResources.erase(gui);

        Backend::detatchGui(gui);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGLFW::setMouseCursorStyle(Cursor::Type type, const std::uint8_t* pixels, Vector2u size, Vector2u hotspot)
    {
        GLFWimage image;
        image.width = static_cast<int>(size.x);
        image.height = static_cast<int>(size.y);
        image.pixels = static_cast<unsigned char*>(const_cast<std::uint8_t*>(pixels)); // NOLINT(cppcoreguidelines-pro-type-const-cast)

        GLFWcursor* cursor = glfwCreateCursor(&image, static_cast<int>(hotspot.x), static_cast<int>(hotspot.y));
        if (!cursor)
            return;

        updateMouseCursorStyle(type, cursor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGLFW::resetMouseCursorStyle(Cursor::Type type)
    {
        updateMouseCursorStyle(type, createSystemCursor(type));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGLFW::setMouseCursor(BackendGui* gui, Cursor::Type type)
    {
        TGUI_ASSERT(m_guis.find(gui) != m_guis.end(), "BackendGLFW::setMouseCursor called with a gui that wasn't attached");
        if (type == m_guiResources[gui].mouseCursor)
            return;

        m_guiResources[gui].mouseCursor = type;

        // If the gui has no access to the window then we can't change the mouse cursor
        if (!m_guiResources[gui].window)
            return;

        updateShownMouseCursor(m_guiResources[gui].window, type);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendGLFW::isKeyboardModifierPressed(Event::KeyModifier modifierKey)
    {
        /// TODO: If there are multiple windows then we need to do this on the focused window
        GLFWwindow* window = getAnyWindow();
        if (!window)
            return false;

        switch (modifierKey)
        {
        case Event::KeyModifier::System:
            return (glfwGetKey(window, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS);
        case Event::KeyModifier::Control:
            return (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS);
        case Event::KeyModifier::Shift:
            return (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
        case Event::KeyModifier::Alt:
            return (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) || (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS);
        }

        TGUI_ASSERT(false, "BackendGLFW::isKeyboardModifierPressed called with an invalid value");
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGLFW::setClipboard(const String& contents)
    {
#if GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 3)
        glfwSetClipboardString(nullptr, std::string(contents).c_str());
#else
        GLFWwindow* window = getAnyWindow();
        if (!window)
            return;

        glfwSetClipboardString(window, std::string(contents).c_str());
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String BackendGLFW::getClipboard() const
    {
#if GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 3)
        const char* contents = glfwGetClipboardString(nullptr);
#else
        GLFWwindow* window = getAnyWindow();
        if (!window)
            return "";

        const char* contents = glfwGetClipboardString(window);
#endif
        if (!contents)
            return "";

        return contents;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GLFWcursor* BackendGLFW::createSystemCursor(Cursor::Type type)
    {
        int typeGLFW = GLFW_ARROW_CURSOR;

#if GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 4)
        switch (type)
        {
        case Cursor::Type::Arrow:
            typeGLFW = GLFW_ARROW_CURSOR;
            break;
        case Cursor::Type::Text:
            typeGLFW = GLFW_IBEAM_CURSOR;
            break;
        case Cursor::Type::Hand:
            typeGLFW = GLFW_POINTING_HAND_CURSOR;
            break;
        case Cursor::Type::SizeLeft:
        case Cursor::Type::SizeRight:
        case Cursor::Type::SizeHorizontal:
            typeGLFW = GLFW_RESIZE_EW_CURSOR;
            break;
        case Cursor::Type::SizeTop:
        case Cursor::Type::SizeBottom:
        case Cursor::Type::SizeVertical:
            typeGLFW = GLFW_RESIZE_NS_CURSOR;
            break;
        case Cursor::Type::SizeBottomRight:
        case Cursor::Type::SizeTopLeft:
            typeGLFW = GLFW_RESIZE_NWSE_CURSOR;
            break;
        case Cursor::Type::SizeBottomLeft:
        case Cursor::Type::SizeTopRight:
            typeGLFW = GLFW_RESIZE_NESW_CURSOR;
            break;
        case Cursor::Type::Crosshair:
            typeGLFW = GLFW_CROSSHAIR_CURSOR;
            break;
        case Cursor::Type::Help:
            TGUI_PRINT_WARNING("BackendGLFW doesn't support Cursor::Type::Help");
            break;
        case Cursor::Type::NotAllowed:
            typeGLFW = GLFW_NOT_ALLOWED_CURSOR;
            break;
        }
#else
        switch (type)
        {
        case Cursor::Type::SizeBottomRight:
        case Cursor::Type::SizeTopLeft:
        case Cursor::Type::SizeBottomLeft:
        case Cursor::Type::SizeTopRight:
        case Cursor::Type::Arrow:
            typeGLFW = GLFW_ARROW_CURSOR;
            break;
        case Cursor::Type::Text:
            typeGLFW = GLFW_IBEAM_CURSOR;
            break;
        case Cursor::Type::Hand:
            typeGLFW = GLFW_HAND_CURSOR;
            break;
        case Cursor::Type::SizeLeft:
        case Cursor::Type::SizeRight:
        case Cursor::Type::SizeHorizontal:
            typeGLFW = GLFW_HRESIZE_CURSOR;
            break;
        case Cursor::Type::SizeTop:
        case Cursor::Type::SizeBottom:
        case Cursor::Type::SizeVertical:
            typeGLFW = GLFW_VRESIZE_CURSOR;
            break;
        case Cursor::Type::Crosshair:
            typeGLFW = GLFW_CROSSHAIR_CURSOR;
            break;
        case Cursor::Type::Help: // In release mode, Help and NotAllowed branches are identical. NOLINT(bugprone-branch-clone)
            TGUI_PRINT_WARNING("BackendGLFW doesn't support Cursor::Type::Help");
            break;
        case Cursor::Type::NotAllowed:
            TGUI_PRINT_WARNING("BackendGLFW doesn't support Cursor::Type::NotAllowed with GLFW < 3.4");
            break;
        }
#endif

        return glfwCreateStandardCursor(typeGLFW);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGLFW::updateMouseCursorStyle(Cursor::Type type, GLFWcursor* cursor)
    {
        if (m_mouseCursors[type])
            glfwDestroyCursor(m_mouseCursors[type]);

        m_mouseCursors[type] = cursor;

        // Update the cursor on the screen if the cursor was in use
        for (const auto& pair : m_guiResources)
        {
            if (pair.second.mouseCursor == type)
            {
                if (pair.second.window)
                    updateShownMouseCursor(pair.second.window, type);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendGLFW::updateShownMouseCursor(GLFWwindow* window, Cursor::Type type)
    {
        TGUI_ASSERT(window != nullptr, "BackendGLFW::updateShownMouseCursor requires a valid window");

        // If the cursor doesn't exist yet then create it now
        if (!m_mouseCursors[type])
        {
            GLFWcursor* cursor = createSystemCursor(type);
            if (!cursor)
                return;

            m_mouseCursors[type] = cursor;
        }

        // Pass the cursor to GLFW to set it while the mouse is on top of the window
        glfwSetCursor(window, m_mouseCursors[type]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GLFWwindow* BackendGLFW::getAnyWindow() const
    {
        GLFWwindow* window = nullptr;
        for (const auto& pair : m_guiResources)
        {
            if (pair.second.window)
            {
                window = pair.second.window;
                break;
            }
        }

        return window;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
