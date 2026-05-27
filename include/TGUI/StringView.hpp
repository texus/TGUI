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

#ifndef TGUI_STRING_VIEW_HPP
#define TGUI_STRING_VIEW_HPP

#include <TGUI/Config.hpp>

#include <algorithm> // equal, min
#include <cctype>    // tolower
#include <string>
#include <string_view>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    inline namespace literals
    {
        inline namespace string_view_literals
        {
            // Allow using operator ""sv
            // Note that this only affects code placed inside the tgui namespace.
            using namespace std::literals::string_view_literals;
        } // namespace string_view_literals
    } // namespace literals

    using StringView = std::u32string_view;
    using CharStringView = std::string_view;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Returns whether two view are equal if letters would have been lowercase
    ///
    /// @param view1  First view to compare
    /// @param view2  Second view to compare
    ///
    /// @return Are the views equal except for the case of letters?
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] inline bool viewEqualIgnoreCase(CharStringView view1, CharStringView view2)
    {
        return std::equal(view1.begin(),
                          view1.end(),
                          view2.begin(),
                          view2.end(),
                          [](char char1, char char2)
                          {
                              if (char1 == char2)
                                  return true;
                              return std::tolower(static_cast<unsigned char>(char1))
                                     == std::tolower(static_cast<unsigned char>(char2));
                          });
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Returns whether two view are equal if ASCII letters would have been lowercase
    ///
    /// @param view1  First view to compare
    /// @param view2  Second view to compare
    ///
    /// @return Are the views equal except for the case of ASCII letters?
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] inline bool viewEqualIgnoreCase(StringView view1, StringView view2)
    {
        return std::equal(view1.cbegin(),
                          view1.cend(),
                          view2.cbegin(),
                          view2.cend(),
                          [](char32_t char1, char32_t char2)
                          {
                              if (char1 == char2)
                                  return true;
                              if ((char1 < 128) && (char2 < 128))
                                  return std::tolower(static_cast<unsigned char>(char1))
                                         == std::tolower(static_cast<unsigned char>(char2));
                              return false;
                          });
    }

#if defined(__cpp_lib_starts_ends_with) && (__cpp_lib_starts_ends_with >= 201711L)
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view starts with the given substring
    ///
    /// @param viewToLookInto  View to check
    /// @param viewToLookFor   String that will be compared with the first characters of the view
    ///
    /// @return True if the view starts with the substring, false otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] inline bool viewStartsWith(CharStringView viewToLookInto, CharStringView viewToLookFor)
    {
        return viewToLookInto.starts_with(viewToLookFor);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view starts with the given character
    ///
    /// @param viewToLookInto  View to check
    /// @param charToLookFor   Character that will be compared with the first character of the view
    ///
    /// @return True if the view starts with the character, false if the view starts with a different character or is empty
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] inline bool viewStartsWith(CharStringView viewToLookInto, char charToLookFor)
    {
        return viewToLookInto.starts_with(charToLookFor);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view ends with the given substring
    ///
    /// @param viewToLookInto  View to check
    /// @param viewToLookFor   String that will be compared with the last characters of the view
    ///
    /// @return True if the view ends with the substring, false otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] inline bool viewEndsWith(CharStringView viewToLookInto, CharStringView viewToLookFor)
    {
        return viewToLookInto.ends_with(viewToLookFor);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view ends with the given character
    ///
    /// @param viewToLookInto  View to check
    /// @param charToLookFor   Character that will be compared with the last character of the view
    ///
    /// @return True if the view ends with the character, false if the view ends with a different character or is empty
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] inline bool viewEndsWith(CharStringView viewToLookInto, char charToLookFor)
    {
        return viewToLookInto.ends_with(charToLookFor);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view starts with the given substring
    ///
    /// @param viewToLookInto  View to check
    /// @param viewToLookFor   String that will be compared with the first characters of the view
    ///
    /// @return True if the view starts with the substring, false otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] inline bool viewStartsWith(StringView viewToLookInto, StringView viewToLookFor)
    {
        return viewToLookInto.starts_with(viewToLookFor);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view starts with the given character
    ///
    /// @param viewToLookInto  View to check
    /// @param charToLookFor   Character that will be compared with the first character of the view
    ///
    /// @return True if the view starts with the character, false if the view starts with a different character or is empty
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] inline bool viewStartsWith(StringView viewToLookInto, char32_t charToLookFor)
    {
        return viewToLookInto.starts_with(charToLookFor);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view ends with the given substring
    ///
    /// @param viewToLookInto  View to check
    /// @param viewToLookFor   String that will be compared with the last characters of the view
    ///
    /// @return True if the view ends with the substring, false otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] inline bool viewEndsWith(StringView viewToLookInto, StringView viewToLookFor)
    {
        return viewToLookInto.ends_with(viewToLookFor);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view ends with the given character
    ///
    /// @param viewToLookInto  View to check
    /// @param charToLookFor   Character that will be compared with the last character of the view
    ///
    /// @return True if the view ends with the character, false if the view ends with a different character or is empty
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] inline bool viewEndsWith(StringView viewToLookInto, char32_t charToLookFor)
    {
        return viewToLookInto.ends_with(charToLookFor);
    }
#else
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view starts with the given substring
    ///
    /// @param viewToLookInto  View to check
    /// @param viewToLookFor   String that will be compared with the first characters of the view
    ///
    /// @return True if the view starts with the substring, false otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] inline bool viewStartsWith(CharStringView viewToLookInto, CharStringView viewToLookFor)
    {
        if (viewToLookFor.length() > viewToLookInto.length())
            return false;

        return viewToLookInto.substr(0, viewToLookFor.length()) == viewToLookFor;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view starts with the given character
    ///
    /// @param viewToLookInto  View to check
    /// @param charToLookFor   Character that will be compared with the first character of the view
    ///
    /// @return True if the view starts with the character, false if the view starts with a different character or is empty
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] inline bool viewStartsWith(CharStringView viewToLookInto, char charToLookFor)
    {
        return !viewToLookInto.empty() && (viewToLookInto.front() == charToLookFor);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view ends with the given substring
    ///
    /// @param viewToLookInto  View to check
    /// @param viewToLookFor   String that will be compared with the last characters of the view
    ///
    /// @return True if the view ends with the substring, false otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] inline bool viewEndsWith(CharStringView viewToLookInto, CharStringView viewToLookFor)
    {
        if (viewToLookFor.length() > viewToLookInto.length())
            return false;

        return CharStringView(viewToLookInto.data() + (viewToLookInto.length() - viewToLookFor.length()), viewToLookFor.length())
                   .compare(viewToLookFor)
               == 0;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view ends with the given character
    ///
    /// @param viewToLookInto  View to check
    /// @param charToLookFor   Character that will be compared with the last character of the view
    ///
    /// @return True if the view ends with the character, false if the view ends with a different character or is empty
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] inline bool viewEndsWith(CharStringView viewToLookInto, char charToLookFor)
    {
        return !viewToLookInto.empty() && (viewToLookInto.back() == charToLookFor);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view starts with the given substring
    ///
    /// @param viewToLookInto  View to check
    /// @param viewToLookFor   String that will be compared with the first characters of the view
    ///
    /// @return True if the view starts with the substring, false otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] inline bool viewStartsWith(StringView viewToLookInto, StringView viewToLookFor)
    {
        if (viewToLookFor.length() > viewToLookInto.length())
            return false;

        return viewToLookInto.substr(0, viewToLookFor.length()) == viewToLookFor;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view starts with the given character
    ///
    /// @param viewToLookInto  View to check
    /// @param charToLookFor   Character that will be compared with the first character of the view
    ///
    /// @return True if the view starts with the character, false if the view starts with a different character or is empty
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] inline bool viewStartsWith(StringView viewToLookInto, char32_t charToLookFor)
    {
        return !viewToLookInto.empty() && (viewToLookInto.front() == charToLookFor);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view ends with the given substring
    ///
    /// @param viewToLookInto  View to check
    /// @param viewToLookFor   String that will be compared with the last characters of the view
    ///
    /// @return True if the view ends with the substring, false otherwise
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] inline bool viewEndsWith(StringView viewToLookInto, StringView viewToLookFor)
    {
        if (viewToLookFor.length() > viewToLookInto.length())
            return false;

        return StringView(viewToLookInto.data() + (viewToLookInto.length() - viewToLookFor.length()), viewToLookFor.length())
                   .compare(viewToLookFor)
               == 0;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Checks whether the view ends with the given character
    ///
    /// @param viewToLookInto  View to check
    /// @param charToLookFor   Character that will be compared with the last character of the view
    ///
    /// @return True if the view ends with the character, false if the view ends with a different character or is empty
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    [[nodiscard]] inline bool viewEndsWith(StringView viewToLookInto, char32_t charToLookFor)
    {
        return !viewToLookInto.empty() && (viewToLookInto.back() == charToLookFor);
    }
#endif
} // namespace tgui

#endif // TGUI_STRING_VIEW_HPP
