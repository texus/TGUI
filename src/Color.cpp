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


#include <TGUI/Color.hpp>
#include <TGUI/Loading/Deserializer.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    const Color Color::Black      {  0,   0,   0};
    const Color Color::White      {255, 255, 255};
    const Color Color::Red        {255,   0,   0};
    const Color Color::Green      {  0, 255,   0};
    const Color Color::Blue       {  0,   0, 255};
    const Color Color::Yellow     {255, 255,   0};
    const Color Color::Magenta    {255,   0, 255};
    const Color Color::Cyan       {  0, 255, 255};
    const Color Color::Transparent{  0,   0,   0,   0};

    const std::array<std::pair<StringView, Color>, 9> Color::colorNamesMap
    {
        {{U"black"sv, Color::Black},
         {U"white"sv, Color::White},
         {U"red"sv, Color::Red},
         {U"yellow"sv, Color::Yellow},
         {U"green"sv, Color::Green},
         {U"cyan"sv, Color::Cyan},
         {U"blue"sv, Color::Blue},
         {U"magenta"sv, Color::Magenta},
         {U"transparent"sv, Color::Transparent}}
    };
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace priv
    {
        TGUI_API Color constructColorFromString(const String& string)
        {
            return Deserializer::deserialize(ObjectConverter::Type::Color, string).getColor();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
