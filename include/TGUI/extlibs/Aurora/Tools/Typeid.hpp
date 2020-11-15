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
/// @brief Function template aurora::typeIndex()

#ifndef AURORA_TYPEID_HPP
#define AURORA_TYPEID_HPP

#include <TGUI/extlibs/Aurora/Meta/Templates.hpp>

#include <typeindex>
#include <type_traits>


namespace aurora
{
namespace detail
{

	template <typename T>
	void verifyRttiType()
	{
		AURORA_REQUIRE_COMPLETE_TYPE(T);
		static_assert(std::is_polymorphic<T>::value, "T must be a polymorphic type.");
#ifndef AURORA_TYPEID_ALLOW_NONVIRTUAL_DTOR
		static_assert(std::has_virtual_destructor<T>::value,  "Polymorphic types should have a virtual destructor. "
			"Define AURORA_TYPEID_ALLOW_NONVIRTUAL_DTOR to let this code compile.");
#endif
	}

} // namespace detail

/// @addtogroup Tools
/// @{

/// @brief Safe typeid operator for references
/// @details The C++ typeid operator can easily be used incorrectly in RTTI scenarios, leading to tedious bugs at runtime:
/// * The operand is a value instead of a reference.
/// * The operand is a pointer which was not dereferenced.
/// * The operand is an incomplete type. The compiler has no knowledge about the polymorphic class hierarchy at this point.
/// * The class is not polymorphic. This can happen when a destructor was not declared virtual.
///
/// This implementation makes sure that the above-mentioned cases do not occur. %Type verification happens at compile time, so
/// there is no performance overhead in using this function. This function is specifically designed for the use of RTTI in
/// polymorphic class hierarchies, it cannot be used in other scenarios (e.g. printing arbitrary type names for debugging).
///
/// Usage:
/// @code
/// Base& baseRef = ...;
/// std::type_index t = aurora::typeIndex(baseRef);
/// @endcode
template <typename T>
std::type_index typeIndex(T& reference)
{
	detail::verifyRttiType<T>();
	return typeid(reference);
}

/// @brief Safe typeid operator for types
/// @details Same as <c>typeIndex(T& reference)</c>, but for types.
///
/// Usage:
/// @code
/// std::type_index t = aurora::typeIndex<Base>();
/// @endcode
template <typename T>
std::type_index typeIndex()
{
	detail::verifyRttiType<T>();
	return typeid(T);
}


/// @}

} // namespace aurora

#endif // AURORA_TYPEID_HPP
