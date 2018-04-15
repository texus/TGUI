/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
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

#ifndef TGUI_CONFIG_HPP
#define TGUI_CONFIG_HPP

#include <SFML/Config.hpp>

// TGUI will link in the same way as SFML, unless TGUI_DYNAMIC or TGUI_STATIC is defined
#if !defined(TGUI_DYNAMIC) && !defined(TGUI_STATIC)
    #ifdef SFML_STATIC
        #define TGUI_STATIC
    #endif
#endif

#ifndef TGUI_STATIC
    #ifdef SFML_SYSTEM_WINDOWS
        // Windows compilers need specific (and different) keywords for export and import
        #ifdef TGUI_EXPORTS
            #define TGUI_API __declspec(dllexport)
        #else
            #define TGUI_API __declspec(dllimport)
        #endif
    #else
        #define TGUI_API __attribute__ ((__visibility__ ("default")))
    #endif
#else
    // Static build doesn't need import/export macros
    #define TGUI_API
    #define TGUI_API
#endif


// Version of the library
#define TGUI_VERSION_MAJOR 0
#define TGUI_VERSION_MINOR 8
#define TGUI_VERSION_PATCH 0

// The signal system detects whether it can provide unbound parameters by checking the arguments of the function at runtime.
// This comparion is made by checking the typeid of the parameters with the typeid of the value which the widget can transmit.
// Although typeid returns a unique value and the operator== is guarenteed to only be true for the same type, it may not always work correctly.
// Dynamically linked libraries may have a different type_info internally than in the code using the library. In such case the comparison will always be false.
// This behavior has so far only been seen on android (using NDK 12b), so the alternative is currently only used when compiling on android.
// The alternative that is used on android is to compare the strings returned by type_info.name(). This is however considered undefined behavior since the compiler
// is not guarenteed to have unique names for different types. The names will however be the same inside and outside the library so this method solves the issue.
// I am also not aware of any supported compiler that does not create unique names.
#ifdef __ANDROID__
    #define TGUI_UNSAFE_TYPE_INFO_COMPARISON
#endif

// Enable constexpr when using Clang or at least GCC 5 or MSVC++ 14.1 (VS2017)
#if defined(_MSC_VER)
    #if _MSC_VER >= 1910
        #define TGUI_CONSTEXPR constexpr
    #else
        #define TGUI_CONSTEXPR
    #endif
#elif defined(__GNUC__)
    #if __cpp_constexpr >= 201304
        #define TGUI_CONSTEXPR constexpr
    #else
        #define TGUI_CONSTEXPR
    #endif
#else
    #define TGUI_CONSTEXPR constexpr
#endif

#ifndef TGUI_NO_DEPRECATED_WARNINGS
    #define TGUI_DEPRECATED(msg) [[deprecated(msg)]]
#else
    #define TGUI_DEPRECATED(msg)
#endif

#ifndef TGUI_NO_RUNTIME_WARNINGS
    #define TGUI_PRINT_WARNING(msg) sf::err() << "TGUI Warning: " << msg << std::endl;
#else
    #define TGUI_PRINT_WARNING(msg)
#endif

#endif // TGUI_CONFIG_HPP
