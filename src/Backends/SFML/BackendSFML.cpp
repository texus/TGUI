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


#include <TGUI/Backends/SFML/BackendSFML.hpp>
#include <TGUI/Backends/SFML/BackendFontSFML.hpp>
#include <TGUI/Backends/SFML/BackendTextSFML.hpp>
#include <TGUI/Backends/SFML/BackendTextureSFML.hpp>
#include <TGUI/Backends/SFML/BackendRenderTargetSFML.hpp>
#include <TGUI/Backends/SFML/GuiSFML.hpp>
#include <TGUI/DefaultFont.hpp>
#include <TGUI/Timer.hpp>

#include <SFML/Config.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Clipboard.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#ifdef TGUI_SYSTEM_WINDOWS
    #include <TGUI/WindowsInclude.hpp>
#endif

#ifdef TGUI_SYSTEM_LINUX
    #include <X11/Xlib.h>
    #include <X11/cursorfont.h>
#endif

#ifdef TGUI_SYSTEM_ANDROID
    #include <SFML/System/NativeActivity.hpp>
    #include <android/asset_manager_jni.h>
    #include <android/asset_manager.h>
    #include <android/native_activity.h>
    #include <android/configuration.h>
#endif

#if SFML_VERSION_MAJOR < 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5)
    #error BackendSFML requires at least SFML >= 2.5.0
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#ifndef TGUI_REMOVE_DEPRECATED_CODE
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

    bool BackendSFML::convertEvent(const sf::Event& eventSFML, Event& eventTGUI)
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
#endif // TGUI_REMOVE_DEPRECATED_CODE

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSFML::attachGui(GuiBase* gui)
    {
        m_guis[gui] = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSFML::detatchGui(GuiBase* gui)
    {
        TGUI_ASSERT(m_guis.find(gui) != m_guis.end(), "BackendSFML::detatchGui called with a gui that wasn't attached");
        m_guis.erase(gui);

        if (m_destroyOnLastGuiDetatch && m_guis.empty())
            setBackend(nullptr);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Font BackendSFML::createDefaultFont()
    {
        auto font = std::make_shared<BackendFontSFML>();
        font->loadFromMemory(defaultFontBytes, sizeof(defaultFontBytes));
        return Font(font, "");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendFontBase> BackendSFML::createFont()
    {
        return std::make_shared<BackendFontSFML>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendTextBase> BackendSFML::createText()
    {
        return std::make_shared<BackendTextSFML>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendTextureBase> BackendSFML::createTexture()
    {
        return std::make_shared<BackendTextureSFML>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSFML::setMouseCursorStyle(Cursor::Type type, const std::uint8_t* pixels, Vector2u size, Vector2u hotspot)
    {
        // Replace the cursor resource
        auto newCursor = std::make_unique<sf::Cursor>();
        newCursor->loadFromPixels(pixels, size, hotspot);
        updateMouseCursor(type, std::move(newCursor));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSFML::resetMouseCursorStyle(Cursor::Type type)
    {
#ifdef TGUI_SYSTEM_LINUX
        // On Linux we use directional resize arrows, but SFML has no support for them
        if ((type == Cursor::Type::SizeLeft) || (type == Cursor::Type::SizeRight)
         || (type == Cursor::Type::SizeTop) || (type == Cursor::Type::SizeBottom)
         || (type == Cursor::Type::SizeBottomRight) || (type == Cursor::Type::SizeTopLeft)
         || (type == Cursor::Type::SizeBottomLeft) || (type == Cursor::Type::SizeTopRight))
        {
            // If the cursor was previously set to a bitmap then release its resources
            m_mouseCursors.erase(type);

            updateMouseCursor(type, nullptr);
            return;
        }
#endif

        updateMouseCursor(type, createSystemCursor(type));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSFML::setMouseCursor(GuiBase* gui, Cursor::Type type)
    {
        TGUI_ASSERT(m_guis.find(gui) != m_guis.end(), "BackendSFML::setMouseCursor called with a gui that wasn't attached");
        if (type == m_guis[gui].mouseCursor)
            return;

        m_guis[gui].mouseCursor = type;

        // If the gui has no access to the window then we can't change the mouse cursor
        if (!m_guis[gui].window)
            return;

        updateMouseCursor(m_guis[gui].window, type);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSFML::openVirtualKeyboard(const FloatRect&)
    {
        sf::Keyboard::setVirtualKeyboardVisible(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSFML::closeVirtualKeyboard()
    {
        sf::Keyboard::setVirtualKeyboardVisible(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendSFML::isKeyboardModifierPressed(Event::KeyModifier modifierKey)
    {
        switch (modifierKey)
        {
        case Event::KeyModifier::System:
            return sf::Keyboard::isKeyPressed(sf::Keyboard::LSystem) || sf::Keyboard::isKeyPressed(sf::Keyboard::RSystem);
        case Event::KeyModifier::Control:
            return sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
        case Event::KeyModifier::Shift:
            return sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
        case Event::KeyModifier::Alt:
            return sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt);
        }

        TGUI_ASSERT(false, "BackendSFML::isKeyboardModifierPressed called with an invalid value");
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSFML::setClipboard(const String& contents)
    {
        sf::Clipboard::setString(sf::String(contents));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String BackendSFML::getClipboard() const
    {
        return String(sf::Clipboard::getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef TGUI_SYSTEM_ANDROID
    std::unique_ptr<std::uint8_t[]> BackendSFML::readFileFromAndroidAssets(const String& filename, std::size_t& fileSize) const
    {
        ANativeActivity* activity = sf::getNativeActivity();

        JNIEnv* env = 0;
        activity->vm->AttachCurrentThread(&env, NULL);
        jclass clazz = env->GetObjectClass(activity->clazz);

        jmethodID methodID = env->GetMethodID(clazz, "getAssets", "()Landroid/content/res/AssetManager;");
        jobject assetManagerObject = env->CallObjectMethod(activity->clazz, methodID);
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

        activity->vm->DetachCurrentThread();

        fileSize = static_cast<std::size_t>(assetLength);
        return buffer;
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<BackendRenderTargetSFML> BackendSFML::createGuiRenderTarget(GuiSFML* gui, sf::RenderTarget& target)
    {
        TGUI_ASSERT(m_guis.find(gui) != m_guis.end(), "BackendSFML::createGuiRenderTarget called with a gui that wasn't attached");
        m_guis[gui].window = dynamic_cast<sf::Window*>(&target);
        return std::make_shared<BackendRenderTargetSFML>(target);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<sf::Cursor> BackendSFML::createSystemCursor(Cursor::Type type)
    {
        sf::Cursor::Type typeSFML = sf::Cursor::Type::Arrow;
        switch (type)
        {
        case Cursor::Type::Arrow:
            typeSFML = sf::Cursor::Type::Arrow;
            break;
        case Cursor::Type::Text:
            typeSFML = sf::Cursor::Type::Text;
            break;
        case Cursor::Type::Hand:
            typeSFML = sf::Cursor::Type::Hand;
            break;
#if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 6 // SFML < 2.6 has no directional arrows for Linux
        case Cursor::Type::SizeLeft:
        case Cursor::Type::SizeRight:
            typeSFML = sf::Cursor::Type::SizeHorizontal;
            break;
        case Cursor::Type::SizeTop:
        case Cursor::Type::SizeBottom:
            typeSFML = sf::Cursor::Type::SizeVertical;
            break;
        case Cursor::Type::SizeBottomRight:
        case Cursor::Type::SizeTopLeft:
            typeSFML = sf::Cursor::Type::SizeTopLeftBottomRight;
            break;
        case Cursor::Type::SizeBottomLeft:
        case Cursor::Type::SizeTopRight:
            typeSFML = sf::Cursor::Type::SizeBottomLeftTopRight;
            break;
#else // SFML >= 2.6
        case Cursor::Type::SizeLeft:
            typeSFML = sf::Cursor::Type::SizeLeft;
            break;
        case Cursor::Type::SizeRight:
            typeSFML = sf::Cursor::Type::SizeRight;
            break;
        case Cursor::Type::SizeTop:
            typeSFML = sf::Cursor::Type::SizeTop;
            break;
        case Cursor::Type::SizeBottom:
            typeSFML = sf::Cursor::Type::SizeBottom;
            break;
        case Cursor::Type::SizeBottomRight:
            typeSFML = sf::Cursor::Type::SizeBottomRight;
            break;
        case Cursor::Type::SizeTopLeft:
            typeSFML = sf::Cursor::Type::SizeTopLeft;
            break;
        case Cursor::Type::SizeBottomLeft:
            typeSFML = sf::Cursor::Type::SizeBottomLeft;
            break;
        case Cursor::Type::SizeTopRight:
            typeSFML = sf::Cursor::Type::SizeTopRight;
            break;
#endif
        case Cursor::Type::Crosshair:
            typeSFML = sf::Cursor::Type::Cross;
            break;
        case Cursor::Type::Help:
            typeSFML = sf::Cursor::Type::Help;
            break;
        case Cursor::Type::NotAllowed:
            typeSFML = sf::Cursor::Type::NotAllowed;
            break;
        }

        auto cursor = std::make_unique<sf::Cursor>();
        cursor->loadFromSystem(typeSFML);
        return cursor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSFML::updateMouseCursor(Cursor::Type type, std::unique_ptr<sf::Cursor> cursor)
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

        m_mouseCursors[type] = std::move(cursor);

        // Update the cursor on the screen if the cursor was in use
        for (auto& pair : m_guis)
        {
            if (pair.second.mouseCursor == type)
            {
                if (pair.second.window)
                    updateMouseCursor(pair.second.window, type);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSFML::updateMouseCursor(sf::Window* window, Cursor::Type type)
    {
        // On Linux we use directional resize arrows, but SFML < 2.6 had no support for them
#if defined(TGUI_SYSTEM_LINUX) && (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 6)
        if ((type == Cursor::Type::SizeLeft) || (type == Cursor::Type::SizeRight)
            || (type == Cursor::Type::SizeTop) || (type == Cursor::Type::SizeBottom)
            || (type == Cursor::Type::SizeBottomRight) || (type == Cursor::Type::SizeTopLeft)
            || (type == Cursor::Type::SizeBottomLeft) || (type == Cursor::Type::SizeTopRight))
        {
            if (!m_mouseCursors[type]) // Only bypass SFML when system cursors are used
            {
                ::Display* displayX11 = XOpenDisplay(nullptr);
                if (displayX11)
                {
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

                    ::Cursor cursorX11 = XCreateFontCursor(displayX11, shapeX11);
                    if (cursorX11 != None)
                    {
                        XDefineCursor(displayX11, window->getSystemHandle(), cursorX11);
                        XFreeCursor(displayX11, cursorX11);
                    }
                    XFlush(displayX11);
                    XCloseDisplay(displayX11);
                }
                return;
            }
        }
#endif

        // If the cursor doesn't exist yet then create it now
        if (!m_mouseCursors[type])
            m_mouseCursors[type] = createSystemCursor(type);

        // Pass the cursor to SFML to set it while the mouse is on top of the window
        window->setMouseCursor(*m_mouseCursors[type]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
