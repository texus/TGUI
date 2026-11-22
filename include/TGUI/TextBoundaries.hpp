////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2026 Bruno Van de Velde (vdv_b@tgui.eu)
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef TGUI_TEXT_BOUNDARIES_HPP
#define TGUI_TEXT_BOUNDARIES_HPP

#include <TGUI/String.hpp>

#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Finds the positions in a text where the characters, as the user sees them, start
        ///
        /// @param text  Text in which to search
        ///
        /// @return Sorted positions of the first code point of each character, which is empty when the text is empty
        ///
        /// On Windows, a character can consist of multiple code points (e.g. a letter followed by combining accents, or an
        /// emoji sequence). The ICU library from Windows is used to find these characters when it is available.
        /// On other platforms, and on Windows versions without ICU, every code point is a separate character.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] TGUI_API std::vector<std::size_t> findCharacterBoundaries(const String& text);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Finds where the character in front of a position starts
        ///
        /// @param text      Text in which to search
        /// @param position  Position in the text, which is not allowed to be 0 or larger than the length of the text
        ///
        /// @return Position of the first code point of the character in front of the given position
        ///
        /// @see findCharacterBoundaries
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] TGUI_API std::size_t findPreviousCharacterBoundary(const String& text, std::size_t position);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Finds where the character behind a position ends
        ///
        /// @param text      Text in which to search
        /// @param position  Position in the text, which has to be smaller than the length of the text
        ///
        /// @return Position behind the last code point of the character that starts at or contains the given position
        ///
        /// @see findCharacterBoundaries
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] TGUI_API std::size_t findNextCharacterBoundary(const String& text, std::size_t position);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Finds the positions in a text where the words start
        ///
        /// @param text  Text in which to search
        ///
        /// @return Sorted positions where each word starts, which is empty when the text is empty
        ///
        /// Words are separated by whitespace. The whitespace behind a word is part of that word, and whitespace at the
        /// start of the text is a word of its own.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] TGUI_API std::vector<std::size_t> findWordBoundaries(const String& text);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Finds where the caret has to be placed when moving it to the previous word
        ///
        /// @param text      Text in which to search
        /// @param position  Position of the caret in the text
        ///
        /// @return Start of the word in front of the position, or the position itself when only whitespace precedes it
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] TGUI_API std::size_t findPreviousWordBoundary(const String& text, std::size_t position);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Finds where the caret has to be placed when moving it to the next word
        ///
        /// @param text      Text in which to search
        /// @param position  Position of the caret in the text
        ///
        /// @return On Windows, the start of the next word (or the end of the text), like in the native Windows controls.
        ///         On other platforms, the end of the word behind the position, or the position itself when only
        ///         whitespace follows it.
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        [[nodiscard]] TGUI_API std::size_t findNextWordBoundary(const String& text, std::size_t position);
    } // namespace priv
} // namespace tgui

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_TEXT_BOUNDARIES_HPP
