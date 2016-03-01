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
/// @brief Macros to simplify container iteration

#ifndef AURORA_FOREACH_HPP
#define AURORA_FOREACH_HPP

#include <TGUI/Aurora/Meta/Preprocessor.hpp>


// Preprocessor metaprogramming to ensure line-unique identifiers
#define AURORA_ID(identifier) AURORA_PP_CAT(auroraDetail_, identifier)
#define AURORA_LINE_ID(identifier) AURORA_PP_CAT(AURORA_ID(identifier), __LINE__)


/// @addtogroup Tools
/// @{

/// @brief Macro that emulates C++11 range-based for loop.
/// @details Code example:
/// @code
/// std::vector<int> v = createVector();
/// AURORA_FOREACH(int& i, v)
/// {
///     i += 2;
/// }
/// @endcode
/// @hideinitializer
#define AURORA_FOREACH(declaration, container)																											\
	if (bool AURORA_LINE_ID(broken) = false) {} else																									\
	for (auto AURORA_LINE_ID(itr) = (container).begin(); AURORA_LINE_ID(itr) != (container).end() && !AURORA_LINE_ID(broken); ++AURORA_LINE_ID(itr))	\
	if (bool AURORA_LINE_ID(passed) = false) {} else																									\
	if (AURORA_LINE_ID(broken) = true, false) {} else																									\
	for (declaration = *AURORA_LINE_ID(itr); !AURORA_LINE_ID(passed); AURORA_LINE_ID(passed) = true, AURORA_LINE_ID(broken) = false)

/// @}

#endif // AURORA_FOREACH_HPP
