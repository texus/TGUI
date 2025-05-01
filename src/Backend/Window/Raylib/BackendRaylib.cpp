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

#include <TGUI/Backend/Window/Raylib/BackendRaylib.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    void BackendRaylib::setMouseCursorStyle(Cursor::Type, const std::uint8_t*, Vector2u, Vector2u)
    {
        // Not supported by raylib
        return;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRaylib::resetMouseCursorStyle(Cursor::Type)
    {
        // Not supported by raylib
        return;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRaylib::setMouseCursor(BackendGui*, Cursor::Type type)
    {
        int typeRaylib = MOUSE_CURSOR_DEFAULT;
        switch (type)
        {
        case Cursor::Type::Arrow:
            typeRaylib = MOUSE_CURSOR_ARROW;
            break;
        case Cursor::Type::Text:
            typeRaylib = MOUSE_CURSOR_IBEAM;
            break;
        case Cursor::Type::Help: // BackendRaylib doesn't support Cursor::Type::Help
        case Cursor::Type::Hand:
            typeRaylib = MOUSE_CURSOR_POINTING_HAND;
            break;
        case Cursor::Type::SizeLeft:
        case Cursor::Type::SizeRight:
        case Cursor::Type::SizeHorizontal:
            typeRaylib = MOUSE_CURSOR_RESIZE_EW;
            break;
        case Cursor::Type::SizeTop:
        case Cursor::Type::SizeBottom:
        case Cursor::Type::SizeVertical:
            typeRaylib = MOUSE_CURSOR_RESIZE_NS;
            break;
        case Cursor::Type::SizeBottomRight:
        case Cursor::Type::SizeTopLeft:
            typeRaylib = MOUSE_CURSOR_RESIZE_NWSE;
            break;
        case Cursor::Type::SizeBottomLeft:
        case Cursor::Type::SizeTopRight:
            typeRaylib = MOUSE_CURSOR_RESIZE_NESW;
            break;
        case Cursor::Type::Crosshair:
            typeRaylib = MOUSE_CURSOR_CROSSHAIR;
            break;
        case Cursor::Type::NotAllowed:
            typeRaylib = MOUSE_CURSOR_NOT_ALLOWED;
            break;
        }

        SetMouseCursor(typeRaylib);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool BackendRaylib::isKeyboardModifierPressed(Event::KeyModifier modifierKey)
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

        TGUI_ASSERT(false, "BackendRaylib::isKeyboardModifierPressed called with an invalid value");
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRaylib::setClipboard(const String& contents)
    {
        SetClipboardText(contents.toStdString().c_str());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    String BackendRaylib::getClipboard() const
    {
        const char* contents = GetClipboardText();
        if (contents)
            return contents;
        else
            return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef TGUI_SYSTEM_ANDROID
    std::unique_ptr<std::uint8_t[]> BackendRaylib::readFileFromAndroidAssets(const String& filename, std::size_t& fileSize) const
    {
        int dataSize = 0;
        unsigned char* fileData = LoadFileData(filename.toStdString().c_str(), &dataSize);
        if (!fileData || (dataSize <= 0))
            return nullptr;

        auto buffer = MakeUniqueForOverwrite<std::uint8_t[]>(static_cast<std::size_t>(dataSize));
        std::memcpy(buffer.get(), fileData, static_cast<std::size_t>(dataSize));

        UnloadFileData(fileData);

        fileSize = static_cast<std::size_t>(dataSize);
        return buffer;
    }
#endif
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
