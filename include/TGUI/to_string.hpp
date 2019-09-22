/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2019 Bruno Van de Velde (vdv_b@tgui.eu)
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

#ifndef TGUI_TO_STRING_HPP
#define TGUI_TO_STRING_HPP


#include <sstream>
#include <locale>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(SFML_SYSTEM_WINDOWS) && defined(_MSC_VER)
    #pragma warning(disable:26444)  // Ignore "Avoid unnamed objects with custom construction and destruction" warning on imbue call
#endif

namespace tgui
{
    template <typename T>
    std::string to_string(T value)
    {
        std::ostringstream oss;
        oss.imbue(std::locale::classic());
        oss << value;
        return oss.str();
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_TO_STRING_HPP

