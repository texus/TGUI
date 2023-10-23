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


#include <TGUI/Backend/Window/SFML/BackendSFML.hpp>

#if !TGUI_BUILD_AS_CXX_MODULE
    #include <TGUI/Timer.hpp>
#endif

#include <SFML/Config.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Clipboard.hpp>

#ifdef TGUI_SYSTEM_WINDOWS
    #include <TGUI/extlibs/IncludeWindows.hpp>
#endif

#if defined(TGUI_SYSTEM_LINUX) && (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 6) && defined(TGUI_USE_X11)
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
    std::vector<std::unique_ptr<sf::Cursor>> BackendSFML::m_leakedCursors;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSFML::setGuiWindow(BackendGui* gui, sf::Window* window)
    {
        TGUI_ASSERT(m_guis.find(gui) != m_guis.end(), "BackendSFML::setGuiWindow called with a gui that wasn't attached");
        m_guiResources[gui].window = window;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSFML::detatchGui(BackendGui* gui)
    {
        // Proper cleanup of TGUI resources requires that the gui is destroyed before the SFML window.
        // Unfortunately, SFML requires the cursor lifetime to exceed that of the window.
        // We are creating a memory leak here to allow cursors to exist long enough.
        if (m_guiResources[gui].window && m_mouseCursors[m_guiResources[gui].mouseCursor])
        {
            auto cursor = std::make_unique<sf::Cursor>();
            (void)cursor->loadFromSystem(sf::Cursor::Type::Arrow);
            m_guiResources[gui].window->setMouseCursor(*cursor);
            m_leakedCursors.push_back(std::move(cursor));
        }

        // Don't check if it existed, detach is called for every gui while attached is only called for properly initialized guis
        m_guiResources.erase(gui);

        Backend::detatchGui(gui);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSFML::setMouseCursorStyle(Cursor::Type type, const std::uint8_t* pixels, Vector2u size, Vector2u hotspot)
    {
        // Replace the cursor resource
        auto newCursor = std::make_unique<sf::Cursor>();
        if (newCursor->loadFromPixels(pixels, size, hotspot))
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

    void BackendSFML::setMouseCursor(BackendGui* gui, Cursor::Type type)
    {
        TGUI_ASSERT(m_guis.find(gui) != m_guis.end(), "BackendSFML::setMouseCursor called with a gui that wasn't attached");
        if (type == m_guiResources[gui].mouseCursor)
            return;

        m_guiResources[gui].mouseCursor = type;

        // If the gui has no access to the window then we can't change the mouse cursor
        if (!m_guiResources[gui].window)
            return;

        updateMouseCursor(m_guiResources[gui].window, type);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TGUI_REMOVE_DEPRECATED_CODE
    void BackendSFML::openVirtualKeyboard(const FloatRect&)
    {
        sf::Keyboard::setVirtualKeyboardVisible(true);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSFML::closeVirtualKeyboard()
    {
        sf::Keyboard::setVirtualKeyboardVisible(false);
    }
#endif
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
        activity->vm->AttachCurrentThread(&env, nullptr);
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
        if (assetLength <= 0)
            return nullptr;

        auto buffer = MakeUniqueForOverwrite<std::uint8_t[]>(static_cast<std::size_t>(assetLength));
        if (AAsset_read(asset, buffer.get(), static_cast<std::size_t>(assetLength)) < 0)
            return nullptr;

        AAsset_close(asset);

        activity->vm->DetachCurrentThread();

        fileSize = static_cast<std::size_t>(assetLength);
        return buffer;
    }
#endif
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
        case Cursor::Type::SizeHorizontal:
            typeSFML = sf::Cursor::Type::SizeHorizontal;
            break;
        case Cursor::Type::SizeVertical:
            typeSFML = sf::Cursor::Type::SizeVertical;
            break;
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
        (void)cursor->loadFromSystem(typeSFML);
        return cursor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendSFML::updateMouseCursor(Cursor::Type type, std::unique_ptr<sf::Cursor> cursor)
    {
#ifdef TGUI_SYSTEM_WINDOWS
        // Make sure the old cursor isn't still being used before we destroy it
        bool cursorInUse = false;
        for (auto& pair : m_guiResources)
        {
            if (pair.second.mouseCursor == type)
                cursorInUse = true;
        }
        if (cursorInUse)
            SetCursor(static_cast<HCURSOR>(LoadImage(nullptr, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED)));
#endif

        m_mouseCursors[type] = std::move(cursor);

        // Update the cursor on the screen if the cursor was in use
        for (auto& pair : m_guiResources)
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
#if defined(TGUI_SYSTEM_LINUX) && (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 6) && defined(TGUI_USE_X11)
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

    void BackendSFML::cleanupLeakedCursors()
    {
        m_leakedCursors.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
