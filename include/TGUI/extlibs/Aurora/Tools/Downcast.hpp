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
/// @brief Function template aurora::downcast()

#ifndef AURORA_DOWNCAST_HPP
#define AURORA_DOWNCAST_HPP

#include <cassert>
#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
	#include <type_traits>
#endif


namespace aurora
{

/// @addtogroup Tools
/// @{

/// @brief Safe polymorphic downcast for references
/// @details Can be used in place of a static_cast from a base class to a derived class -- that is,
/// you expect the downcast to succeed. In debug mode, types are checked at runtime using dynamic_cast.
/// In release mode (with the NDBEBUG macro defined), a static_cast at full speed is used.
template <typename To, typename From>
To downcast(From& base)
{
	assert(dynamic_cast<typename std::remove_reference<To>::type*>(&base));
	return static_cast<To>(base);
}

/// @brief Safe polymorphic downcast for pointers
/// @details Can be used in place of a static_cast from a base class to a derived class -- that is,
/// you expect the downcast to succeed. In debug mode, types are checked at runtime using dynamic_cast.
/// In release mode (with the NDBEBUG macro defined), a static_cast at full speed is used.
template <typename To, typename From>
To downcast(From* base)
{
	assert(dynamic_cast<To>(base));
	return static_cast<To>(base);
}

/// @}

} // namespace aurora

#endif // AURORA_DOWNCAST_HPP
