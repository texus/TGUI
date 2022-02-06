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


#ifndef TGUI_KEYBOARD_HPP
#define TGUI_KEYBOARD_HPP

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/Config.hpp>
#include <TGUI/Event.hpp>
#include <TGUI/Backend.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace keyboard
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isShiftPressed()
        {
            return getBackend()->isKeyboardModifierPressed(Event::KeyModifier::Shift);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline void openVirtualKeyboard(const FloatRect& inputRect)
        {
            getBackend()->openVirtualKeyboard(inputRect);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline void closeVirtualKeyboard()
        {
            getBackend()->closeVirtualKeyboard();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isMultiselectModifierPressed()
        {
#ifdef TGUI_SYSTEM_MACOS
            return getBackend()->isKeyboardModifierPressed(Event::KeyModifier::System);
#else
            return getBackend()->isKeyboardModifierPressed(Event::KeyModifier::Control);
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressCopy(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return (event.code == Event::KeyboardKey::C) && !event.control && !event.alt && !event.shift && event.system;
#else
            return (event.code == Event::KeyboardKey::C) && event.control && !event.alt && !event.shift && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressCut(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return (event.code == Event::KeyboardKey::X) && !event.control && !event.alt && !event.shift && event.system;
#else
            return (event.code == Event::KeyboardKey::X) && event.control && !event.alt && !event.shift && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressPaste(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return (event.code == Event::KeyboardKey::V) && !event.control && !event.alt && !event.shift && event.system;
#else
            return (event.code == Event::KeyboardKey::V) && event.control && !event.alt && !event.shift && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressSelectAll(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return (event.code == Event::KeyboardKey::A) && !event.control && !event.alt && !event.shift && event.system;
#else
            return (event.code == Event::KeyboardKey::A) && event.control && !event.alt && !event.shift && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressMoveCaretLeft(const Event::KeyEvent& event)
        {
            return (event.code == Event::KeyboardKey::Left) && !event.control && !event.alt && !event.system;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressMoveCaretRight(const Event::KeyEvent& event)
        {
            return (event.code == Event::KeyboardKey::Right) && !event.control && !event.alt && !event.system;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressMoveCaretWordBegin(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return (event.code == Event::KeyboardKey::Left) && !event.control && event.alt && !event.system;
#else
            return (event.code == Event::KeyboardKey::Left) && event.control && !event.alt && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressMoveCaretWordEnd(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return (event.code == Event::KeyboardKey::Right) && !event.control && event.alt && !event.system;
#else
            return (event.code == Event::KeyboardKey::Right) && event.control && !event.alt && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressMoveCaretUp(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            // Option+UpArrow should actually move to the beginning of the paragraph (or the previous one), but we don't support this
            return (event.code == Event::KeyboardKey::Up) && !event.control && !event.system;
#else
            return (event.code == Event::KeyboardKey::Up) && !event.alt && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressMoveCaretDown(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            // Option+DownArrow should actually move to the end of the paragraph (or the next one), but we don't support this
            return (event.code == Event::KeyboardKey::Down) && !event.control && !event.system;
#else
            return (event.code == Event::KeyboardKey::Down) && !event.alt && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressMoveCaretLineStart(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            if ((event.code == Event::KeyboardKey::Left) && !event.control && !event.alt && event.system)
                return true;
#endif
            return (event.code == Event::KeyboardKey::Home) && !event.control && !event.alt && !event.system;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressMoveCaretLineEnd(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            if ((event.code == Event::KeyboardKey::Right) && !event.control && !event.alt && event.system)
                return true;
#endif
            return (event.code == Event::KeyboardKey::End) && !event.control && !event.alt && !event.system;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressMoveCaretDocumentBegin(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return ((event.code == Event::KeyboardKey::Up) && !event.control && !event.alt && event.system)
                || ((event.code == Event::KeyboardKey::Home) && !event.control && !event.alt && event.system);
#else
            return (event.code == Event::KeyboardKey::Home) && event.control && !event.alt && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        inline bool isKeyPressMoveCaretDocumentEnd(const Event::KeyEvent& event)
        {
#ifdef TGUI_SYSTEM_MACOS
            return ((event.code == Event::KeyboardKey::Down) && !event.control && !event.alt && event.system)
                || ((event.code == Event::KeyboardKey::End) && !event.control && !event.alt && event.system);
#else
            return (event.code == Event::KeyboardKey::End) && event.control && !event.alt && !event.system;
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_KEYBOARD_HPP

