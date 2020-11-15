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
/// @brief Helpers to declare and invoke swap() functions

#ifndef AURORA_SWAP_HPP
#define AURORA_SWAP_HPP

#include <utility> // std::swap (until C++11, it was in <algorithm> header)


/// @addtogroup Tools
/// @{

/// @brief Macro to implement a global overload of <tt>swap(lhs, rhs)</tt> to allow argument-dependent lookup.
/// @details Accesses the member function <tt>void Class::swap(Class&)</tt>.
#define AURORA_GLOBAL_SWAP(Class)			\
inline void swap(Class& lhs, Class& rhs)	\
{											\
	lhs.swap(rhs);							\
}

/// @brief %swap() function with argument-dependent lookup
/// @details Chooses the best overload of <tt>swap(lhs, rhs)</tt> with argument-dependent lookup. If none is found, @c std::swap() will be called.
template <typename T>
inline void adlSwap(T& lhs, T& rhs)
{
	using namespace std;
	swap(lhs, rhs);
}

/// @}

#endif // AURORA_SWAP_HPP
