/////////////////////////////////////////////////////////////////////////////////
//
// Aurora C++ Library
// Copyright (c) 2012-2016 Jan Haller
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////

/// @file
/// @brief Configuration header of the library

#ifndef AURORA_CONFIG_HPP
#define AURORA_CONFIG_HPP


// Version of the library
#define AURORA_VERSION_MAJOR 1
#define AURORA_VERSION_MINOR 0


// Mislead doxygen to keep documentation clean from internals
#define AURORA_FAKE_DOC(real, fake) real
#define AURORA_IMPL_DEF(...) __VA_ARGS__


// Tell the compiler that a code path is unreachable, to disable warnings and enable optimizations
// Most common case: default label in switch statements
#if defined(_MSC_VER)
	#define AURORA_UNREACHABLE __assume(false)
#elif defined(__GNUC__) || defined(__clang__)
	#define AURORA_UNREACHABLE __builtin_unreachable()
#endif


// Output useful error message if MSVC, Clang or g++ compilers do not support C++11
// Cascaded because symbols are not 100% reliable, clang sometimes defines g++ macros
#if defined(_MSC_VER)
	#if _MSC_VER < 1600
		#error At least Visual Studio 2010 is required.
	#endif
#elif defined(__clang__)
	#if 100*__clang_major__ + __clang_minor__ < 301
		#error At least Clang 3.1 is required.
	#endif
#elif defined(__GNUC__)
	#if 100*__GNUC__ + __GNUC_MINOR__ < 406
		#error At least g++ 4.6 is required.
	#endif
#endif


// Find out whether variadic templates are supported (VC++, gcc, clang)
#if defined(_MSC_VER) && _MSC_VER >= 1800
	#define AURORA_HAS_VARIADIC_TEMPLATES
#elif defined(__clang__)
	#if __has_feature(cxx_variadic_templates)
		#define AURORA_HAS_VARIADIC_TEMPLATES
	#endif
#elif defined(__GNUG__) && (defined(__VARIADIC_TEMPLATES) || __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4 && defined(__GXX_EXPERIMENTAL_CXX0X__)))
	#define AURORA_HAS_VARIADIC_TEMPLATES
#endif

#endif // AURORA_CONFIG_HPP
