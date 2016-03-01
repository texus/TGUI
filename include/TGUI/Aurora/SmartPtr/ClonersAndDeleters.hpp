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
/// @brief Cloner and deleter functors for smart pointers

#ifndef AURORA_PTRFUNCTORS_HPP
#define AURORA_PTRFUNCTORS_HPP

#include <TGUI/Aurora/Meta/Templates.hpp>


namespace aurora
{

/// @addtogroup SmartPtr
/// @{

/// @brief Cloner that invokes the copy constructor and new operator.
/// @details Makes a copy of the origin using the copy constructor. The resulting object is allocated with new, so the
///  recommended deleter to use together with this cloner is OperatorDelete.
template <typename T>
struct OperatorNewCopy
{
	T* operator() (const T* pointer) const
	{
		return new T(*pointer);
	}
};

/// @brief Cloner that invokes a member function clone().
/// @details clone() shall be a virtual function with the signature <tt>C* C::clone() const</tt>, where C is the class type.
///  The returned pointer has to point to a new copy of the object on which clone() is invoked. In case you do not call
///  the new operator to allocate the copy, make sure you also use a corresponding deleter in CopiedPtr.
template <typename T>
struct VirtualClone
{
	T* operator() (const T* pointer) const
	{
		return pointer->clone();
	}
};

/// @brief Deleter that invokes the delete operator.
/// @details If you use this deleter, ensure that the object has been allocated with new. Note that for CopiedPtr, also the
///  cloner has to return a copy allocated with new (like OperatorNewCopy). The type T shall be complete at the time of deletion.
template <typename T>
struct OperatorDelete
{
	void operator() (T* pointer)
	{
		AURORA_REQUIRE_COMPLETE_TYPE(T);
		delete pointer;
	}
};

/// @}

} // namespace aurora

#endif // AURORA_PTRFUNCTORS_HPP
