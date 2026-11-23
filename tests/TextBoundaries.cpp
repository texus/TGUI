/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Tests.hpp"

#include <TGUI/TextBoundaries.hpp>

TEST_CASE("[TextBoundaries]")
{
    // On Windows, characters consisting of multiple code points are kept together. Other platforms use code points for now.
#if defined(TGUI_SYSTEM_WINDOWS)
    constexpr bool combinedCharacters = true;
#else
    constexpr bool combinedCharacters = false;
#endif

    // "e" followed by a combining acute accent, an emoji sequence of a man and a woman joined with a zero-width joiner,
    // and a newline character that is always a separate character
    const tgui::String text = U"ae\u0301b\U0001F468\u200D\U0001F469\nc";

    SECTION("findCharacterBoundaries")
    {
        REQUIRE(tgui::priv::findCharacterBoundaries(U"").empty());

        if (combinedCharacters)
            REQUIRE(tgui::priv::findCharacterBoundaries(text) == std::vector<std::size_t>{0, 1, 3, 4, 7, 8});
        else
            REQUIRE(tgui::priv::findCharacterBoundaries(text) == std::vector<std::size_t>{0, 1, 2, 3, 4, 5, 6, 7, 8});
    }

    SECTION("findPreviousCharacterBoundary")
    {
        REQUIRE(tgui::priv::findPreviousCharacterBoundary(text, 1) == 0);
        REQUIRE(tgui::priv::findPreviousCharacterBoundary(text, 3) == (combinedCharacters ? 1 : 2));
        REQUIRE(tgui::priv::findPreviousCharacterBoundary(text, 7) == (combinedCharacters ? 4 : 6));
        REQUIRE(tgui::priv::findPreviousCharacterBoundary(text, 8) == 7);
        REQUIRE(tgui::priv::findPreviousCharacterBoundary(text, 9) == 8);
    }

    SECTION("findNextCharacterBoundary")
    {
        REQUIRE(tgui::priv::findNextCharacterBoundary(text, 0) == 1);
        REQUIRE(tgui::priv::findNextCharacterBoundary(text, 1) == (combinedCharacters ? 3 : 2));
        REQUIRE(tgui::priv::findNextCharacterBoundary(text, 4) == (combinedCharacters ? 7 : 5));
        REQUIRE(tgui::priv::findNextCharacterBoundary(text, 7) == 8);
        REQUIRE(tgui::priv::findNextCharacterBoundary(text, 8) == 9);
    }

    SECTION("Words")
    {
        // On Windows, moving to the next word places the caret at the start of that word instead of at the end of the word
#if defined(TGUI_SYSTEM_WINDOWS)
        constexpr bool nextWordStart = true;
#else
        constexpr bool nextWordStart = false;
#endif
        const tgui::String words = " ab  cd";

        REQUIRE(tgui::priv::findWordBoundaries("").empty());
        REQUIRE(tgui::priv::findWordBoundaries(words) == std::vector<std::size_t>{0, 1, 5});

        REQUIRE(tgui::priv::findPreviousWordBoundary(words, 7) == 5);
        REQUIRE(tgui::priv::findPreviousWordBoundary(words, 5) == 1);
        REQUIRE(tgui::priv::findPreviousWordBoundary(words, 3) == 1);
        REQUIRE(tgui::priv::findPreviousWordBoundary(words, 1) == 1);
        REQUIRE(tgui::priv::findPreviousWordBoundary(words, 0) == 0);

        REQUIRE(tgui::priv::findNextWordBoundary(words, 0) == (nextWordStart ? 1 : 3));
        REQUIRE(tgui::priv::findNextWordBoundary(words, 1) == (nextWordStart ? 5 : 3));
        REQUIRE(tgui::priv::findNextWordBoundary(words, 3) == (nextWordStart ? 5 : 7));
        REQUIRE(tgui::priv::findNextWordBoundary(words, 5) == 7);
        REQUIRE(tgui::priv::findNextWordBoundary(words, 7) == 7);
        REQUIRE(tgui::priv::findNextWordBoundary("ab  ", 2) == (nextWordStart ? 4 : 2));
    }
}
