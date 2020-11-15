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
/// @brief Factory function aurora::makeUnique()

#ifndef AURORA_MAKEUNIQUE_HPP
#define AURORA_MAKEUNIQUE_HPP

#include <memory>


namespace aurora
{

// For documentation and modern compilers
#ifdef AURORA_HAS_VARIADIC_TEMPLATES

/// @addtogroup SmartPtr
/// @{

/// @brief Emplaces an object directly inside the unique pointer.
/// @param args Variable argument list, the single arguments are forwarded to T's constructor. If your compiler does not
/// support variadic templates, the number of arguments must be smaller than @ref AURORA_PP_LIMIT.
/// @details Does the same as std::make_unique() in C++14. Example:
/// @code
/// auto ptr = aurora::makeUnique<MyClass>(arg1, arg2); // instead of
/// std::unique_ptr<MyClass> ptr(new MyClass(arg1, arg2));
/// @endcode
template <typename T, typename... Args>
std::unique_ptr<T> makeUnique(Args&&... args)
{
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

/// @}

// ---------------------------------------------------------------------------------------------------------------------------


// Unoptimized fallback for compilers that don't support variadic templates, emulated by preprocessor metaprogramming
#else // AURORA_HAS_VARIADIC_TEMPLATES

#include <TGUI/Aurora/SmartPtr/Detail/Factories.hpp>

// Define metafunction to generate overloads for aurora::CopiedPtr
#define AURORA_DETAIL_UNIQUEPTR_FACTORY(n) AURORA_DETAIL_SMARTPTR_FACTORY(std::unique_ptr, makeUnique, n)

// Generate code
AURORA_PP_ENUMERATE(AURORA_PP_LIMIT, AURORA_DETAIL_UNIQUEPTR_FACTORY)

#endif // AURORA_HAS_VARIADIC_TEMPLATES

} // namespace aurora

#endif // AURORA_MAKEUNIQUE_HPP
