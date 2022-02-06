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


#include <TGUI/Backends/SDL/BackendSDL.hpp>
#include <TGUI/Backends/SDL/BackendFontSDL.hpp>
#include <TGUI/Backends/SDL/BackendTextSDL.hpp>
#include <TGUI/Backends/SDL/BackendTextureSDL.hpp>
#include <TGUI/Backends/SDL/BackendRenderTargetSDL.hpp>
#include <TGUI/Backends/SDL/GuiSDL.hpp>
#include <TGUI/DefaultFont.hpp>
#include <TGUI/Timer.hpp>
#include <TGUI/OpenGL.hpp>

#include <SDL.h>
#include <SDL_syswm.h>

#ifdef TGUI_SYSTEM_WINDOWS
    #include <TGUI/WindowsInclude.hpp>
#endif

#ifdef TGUI_SYSTEM_LINUX
    #include <X11/Xlib.h>
    #include <X11/cursorfont.h>
#endif

#ifdef TGUI_SYSTEM_ANDROID
    #include <jni.h>
    #include <android/asset_manager.h>
    #include <android/asset_manager_jni.h>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#ifndef TGUI_REMOVE_DEPRECATED_CODE
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

    bool BackendSDL::convertEvent(const SDL_Event& eventSDL, Event& eventTGUI)
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

                return true;
            }
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            {
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
                eventTGUI.type = Event::Type::MouseMoved;
                eventTGUI.mouseMove.x = eventSDL.motion.x;
                eventTGUI.mouseMove.y = eventSDL.motion.y;
                return true;
            }
            default: // This event is not handled by TGUI
                return false;
        }
    }
#endif // TGUI_REMOVE_DEPRECATED_CODE

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendSDL::BackendSDL()
    {
#if TGUI_USE_GLES
        const int version = tgui_gladLoadGLES2(reinterpret_cast<TGUI_GLADloadfunc>(SDL_GL_GetProcAddress));
        if (TGUI_GLAD_VERSION_MAJOR(version) < 2)
            throw Exception{"BackendSDL expects at least OpenGL ES 2.0"};
#else
        const int version = tgui_gladLoadGL(reinterpret_cast<TGUI_GLADloadfunc>(SDL_GL_GetProcAddress));
        if ((TGUI_GLAD_VERSION_MAJOR(version) < 3) || ((TGUI_GLAD_VERSION_MAJOR(version) == 3) && TGUI_GLAD_VERSION_MINOR(version) < 2))
        {
            if (version == 0)
                throw Exception{"BackendSDL failed to query OpenGL version, or requested OpenGL version wasn't supported. Has an OpenGL context been created with SDL_GL_CreateContext?"};
            else
            {
                throw Exception{"BackendSDL expects at least OpenGL 3.2, found version "
                    + String(TGUI_GLAD_VERSION_MAJOR(version)) + '.' + String(TGUI_GLAD_VERSION_MINOR(version))};
            }
        }
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BackendSDL::~BackendSDL()
    {
        for (auto& cursor : m_mouseCursors)
        {
            if (cursor.second)
                SDL_FreeCursor(cursor.second);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSDL::attachGui(GuiBase* gui)
    {
        m_guis[gui] = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSDL::detatchGui(GuiBase* gui)
    {
        TGUI_ASSERT(m_guis.find(gui) != m_guis.end(), "BackendSDL::detatchGui called with a gui that wasn't attached");
        m_guis.erase(gui);

        if (m_destroyOnLastGuiDetatch && m_guis.empty())
            setBackend(nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font BackendSDL::createDefaultFont()
    {
        auto font = std::make_shared<BackendFontSDL>();
        font->loadFromMemory(defaultFontBytes, sizeof(defaultFontBytes));
        return Font(font, "");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendFontBase> BackendSDL::createFont()
    {
        return std::make_shared<BackendFontSDL>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendTextBase> BackendSDL::createText()
    {
        return std::make_shared<BackendTextSDL>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendTextureBase> BackendSDL::createTexture()
    {
        return std::make_shared<BackendTextureSDL>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSDL::setMouseCursorStyle(Cursor::Type type, const std::uint8_t* pixels, Vector2u size, Vector2u hotspot)
    {
        SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(const_cast<std::uint8_t*>(pixels), static_cast<int>(size.x), static_cast<int>(size.y),
            32, 4 * static_cast<int>(size.x), 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
        if (!surface)
            return;

        SDL_Cursor* bitmapCursor = SDL_CreateColorCursor(surface, static_cast<int>(hotspot.x), static_cast<int>(hotspot.y));
        SDL_FreeSurface(surface);

        if (!bitmapCursor)
            return;

        updateMouseCursorStyle(type, bitmapCursor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSDL::resetMouseCursorStyle(Cursor::Type type)
    {
#ifdef TGUI_SYSTEM_LINUX
        // On Linux we use directional resize arrows, but SDL has no support for them
        if ((type == Cursor::Type::SizeLeft) || (type == Cursor::Type::SizeRight)
         || (type == Cursor::Type::SizeTop) || (type == Cursor::Type::SizeBottom)
         || (type == Cursor::Type::SizeBottomRight) || (type == Cursor::Type::SizeTopLeft)
         || (type == Cursor::Type::SizeBottomLeft) || (type == Cursor::Type::SizeTopRight))
        {
            // If the cursor was previously set to a bitmap then release its resources
            auto it = m_mouseCursors.find(type);
            if ((it != m_mouseCursors.end()) && it->second)
            {
                SDL_FreeCursor(it->second);
                m_mouseCursors.erase(it);
            }

            updateMouseCursorStyle(type, nullptr);
            return;
        }
#endif

        updateMouseCursorStyle(type, createSystemCursor(type));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSDL::setMouseCursor(GuiBase* gui, Cursor::Type type)
    {
        TGUI_ASSERT(m_guis.find(gui) != m_guis.end(), "BackendSDL::setMouseCursor called with a gui that wasn't attached");
        if (type == m_guis[gui].mouseCursor)
            return;

        m_guis[gui].mouseCursor = type;

        // If the gui has no access to the window then we can't change the mouse cursor
        if (!m_guis[gui].window)
            return;

        updateShownMouseCursor(m_guis[gui].window, type);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSDL::openVirtualKeyboard(const FloatRect& rect)
    {
        SDL_Rect inputRect;
        inputRect.x = static_cast<int>(rect.left);
        inputRect.y = static_cast<int>(rect.top);
        inputRect.w = static_cast<int>(rect.width);
        inputRect.h = static_cast<int>(rect.height);

        SDL_StartTextInput();
        SDL_SetTextInputRect(&inputRect);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSDL::closeVirtualKeyboard()
    {
        SDL_StopTextInput();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendSDL::isKeyboardModifierPressed(Event::KeyModifier modifierKey)
    {
        const SDL_Keymod pressedModifiers = SDL_GetModState();
        switch (modifierKey)
        {
        case Event::KeyModifier::System:
            return (pressedModifiers & KMOD_GUI) != 0;
        case Event::KeyModifier::Control:
            return (pressedModifiers & KMOD_CTRL) != 0;
        case Event::KeyModifier::Shift:
            return (pressedModifiers & KMOD_SHIFT) != 0;
        case Event::KeyModifier::Alt:
            return (pressedModifiers & KMOD_ALT) != 0;
        }

        TGUI_ASSERT(false, "BackendSDL::isKeyboardModifierPressed called with an invalid value");
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSDL::setClipboard(const String& contents)
    {
#ifdef TGUI_SYSTEM_WINDOWS
        // If setting the clipboard fails on Windows then sleep a moment and try again
        if (SDL_SetClipboardText(contents.toStdString().c_str()) < 0)
        {
            Sleep(1);
            SDL_SetClipboardText(contents.toStdString().c_str());
        }
#else
        SDL_SetClipboardText(contents.toStdString().c_str());
#endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String BackendSDL::getClipboard() const
    {
        String contents;
        char* text = SDL_GetClipboardText();
        if (text)
        {
            contents = text;
            SDL_free(text);
        }

        return contents;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef TGUI_SYSTEM_ANDROID
    std::unique_ptr<std::uint8_t[]> BackendSDL::readFileFromAndroidAssets(const String& filename, std::size_t& fileSize) const
    {
        JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
        jobject activity = (jobject)SDL_AndroidGetActivity();
        jclass clazz = env->GetObjectClass(activity);

        jmethodID methodID = env->GetMethodID(clazz, "getAssets", "()Landroid/content/res/AssetManager;");
        jobject assetManagerObject = env->CallObjectMethod(activity, methodID);
        jobject globalAssetManagerRef = env->NewGlobalRef(assetManagerObject);
        AAssetManager* assetManager = AAssetManager_fromJava(env, globalAssetManagerRef);
        if (!assetManager)
            return nullptr;

        AAsset* asset = AAssetManager_open(assetManager, filename.toStdString().c_str(), AASSET_MODE_UNKNOWN);
        if (!asset)
            return nullptr;

        const off_t assetLength = AAsset_getLength(asset);

        auto buffer = std::make_unique<std::uint8_t[]>(assetLength);
        if (AAsset_read(asset, buffer.get(), assetLength) < 0)
            return nullptr;

        AAsset_close(asset);

        fileSize = static_cast<std::size_t>(assetLength);
        return buffer;
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendRenderTargetSDL> BackendSDL::createGuiRenderTarget(GuiSDL* gui, SDL_Window* window)
    {
        TGUI_ASSERT(m_guis.find(gui) != m_guis.end(), "BackendSDL::createGuiRenderTarget called with a gui that wasn't attached");
        m_guis[gui].window = window;
        return std::make_shared<BackendRenderTargetSDL>(window);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSDL::changeTexture(GLuint textureId, bool force)
    {
        if ((m_currentTexture == textureId) && !force)
            return;

        glBindTexture(GL_TEXTURE_2D, textureId);
        m_currentTexture = textureId;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SDL_Cursor* BackendSDL::createSystemCursor(Cursor::Type type)
    {
        SDL_SystemCursor typeSDL = SDL_SYSTEM_CURSOR_ARROW;
        switch (type)
        {
        case Cursor::Type::Arrow:
            typeSDL = SDL_SYSTEM_CURSOR_ARROW;
            break;
        case Cursor::Type::Text:
            typeSDL = SDL_SYSTEM_CURSOR_IBEAM;
            break;
        case Cursor::Type::Hand:
            typeSDL = SDL_SYSTEM_CURSOR_HAND;
            break;
        case Cursor::Type::SizeLeft:
        case Cursor::Type::SizeRight:
            typeSDL = SDL_SYSTEM_CURSOR_SIZEWE;
            break;
        case Cursor::Type::SizeTop:
        case Cursor::Type::SizeBottom:
            typeSDL = SDL_SYSTEM_CURSOR_SIZENS;
            break;
        case Cursor::Type::SizeBottomRight:
        case Cursor::Type::SizeTopLeft:
            typeSDL = SDL_SYSTEM_CURSOR_SIZENWSE;
            break;
        case Cursor::Type::SizeBottomLeft:
        case Cursor::Type::SizeTopRight:
            typeSDL = SDL_SYSTEM_CURSOR_SIZENESW;
            break;
        case Cursor::Type::Crosshair:
            typeSDL = SDL_SYSTEM_CURSOR_CROSSHAIR;
            break;
        case Cursor::Type::Help:
            TGUI_PRINT_WARNING("BackendSDL doesn't support Cursor::Type::Help");
            break;
        case Cursor::Type::NotAllowed:
            typeSDL = SDL_SYSTEM_CURSOR_NO;
            break;
        }

        return SDL_CreateSystemCursor(typeSDL);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSDL::updateMouseCursorStyle(Cursor::Type type, SDL_Cursor* cursor)
    {
#ifdef TGUI_SYSTEM_WINDOWS
        // Make sure the old cursor isn't still being used before we destroy it
        bool cursorInUse = false;
        for (auto& pair : m_guis)
        {
            if (pair.second.mouseCursor == type)
                cursorInUse = true;
        }
        if (cursorInUse)
            SetCursor(static_cast<HCURSOR>(LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED)));
#endif

        if (m_mouseCursors[type])
            SDL_FreeCursor(m_mouseCursors[type]);

        m_mouseCursors[type] = cursor;

        // Update the cursor on the screen if the cursor was in use
        for (auto& pair : m_guis)
        {
            if (pair.second.mouseCursor == type)
            {
                if (pair.second.window)
                    updateShownMouseCursor(pair.second.window, type);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSDL::updateShownMouseCursor(SDL_Window* window, Cursor::Type type)
    {
        TGUI_ASSERT(window != nullptr, "BackendSDL::updateShownMouseCursor requires a valid window");

#ifdef TGUI_SYSTEM_LINUX
        // On Linux we use directional resize arrows, but SDL has no support for them
        if ((type == Cursor::Type::SizeLeft) || (type == Cursor::Type::SizeRight)
            || (type == Cursor::Type::SizeTop) || (type == Cursor::Type::SizeBottom)
            || (type == Cursor::Type::SizeBottomRight) || (type == Cursor::Type::SizeTopLeft)
            || (type == Cursor::Type::SizeBottomLeft) || (type == Cursor::Type::SizeTopRight))
        {
            if (!m_mouseCursors[type]) // Only bypass SDL when system cursors are used
            {
                SDL_SysWMinfo sysInfo;
                SDL_VERSION(&sysInfo.version);
                if (SDL_GetWindowWMInfo(window, &sysInfo) && (sysInfo.subsystem == SDL_SYSWM_X11))
                {
                    auto* displayX11 = sysInfo.info.x11.display;
                    if (!displayX11)
                        return;

                    unsigned int shapeX11;
                    if (type == Cursor::Type::SizeLeft)
                        shapeX11 = XC_left_side;
                    else if (type == Cursor::Type::SizeRight)
                        shapeX11 = XC_right_side;
                    else if (type == Cursor::Type::SizeTop)
                        shapeX11 = XC_top_side;
                    else if (type == Cursor::Type::SizeBottom)
                        shapeX11 = XC_bottom_side;
                    else if (type == Cursor::Type::SizeBottomRight)
                        shapeX11 = XC_bottom_right_corner;
                    else if (type == Cursor::Type::SizeTopLeft)
                        shapeX11 = XC_top_left_corner;
                    else if (type == Cursor::Type::SizeBottomLeft)
                        shapeX11 = XC_bottom_left_corner;
                    else // if (type == Cursor::Type::SizeTopRight)
                        shapeX11 = XC_top_right_corner;

                    auto cursorX11 = XCreateFontCursor(displayX11, shapeX11);
                    if (cursorX11 != None)
                    {
                        XDefineCursor(displayX11, sysInfo.info.x11.window, cursorX11);
                        XFreeCursor(displayX11, cursorX11);
                    }

                    XFlush(displayX11);
                }

                return;
            }
        }
#else
        (void)window;
#endif

        // If the cursor doesn't exist yet then create it now
        if (!m_mouseCursors[type])
        {
            SDL_Cursor* cursor = createSystemCursor(type);
            if (!cursor)
                return;

            m_mouseCursors[type] = cursor;
        }

        // Pass the cursor to SDL to set it while the mouse is on top of the window
        SDL_SetCursor(m_mouseCursors[type]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
