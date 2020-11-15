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
/// @brief Class aurora::Any

#ifndef AURORA_ANY_HPP
#define AURORA_ANY_HPP

#include <TGUI/extlibs/Aurora/Config.hpp>
#include <TGUI/extlibs/Aurora/Tools/Swap.hpp>
#include <TGUI/extlibs/Aurora/Tools/SafeBool.hpp>
#include <TGUI/extlibs/Aurora/SmartPtr/CopiedPtr.hpp>

#include <typeindex>


namespace aurora
{

/// @addtogroup Tools
/// @{

/// @brief Type-erased class holding any value
/// @details This class can either hold a single value of arbitrary type, or it can be empty.
///  Any erases the contained type at compile time while preserving value semantics. It is thus a type-safe and RAII-managed
///  alternative to @c void* pointers.
/// @n@n Usage examples:
/// @code
/// aurora::Any any = 32; // store int
/// any = 44l;            // store long
///
/// long& ref = any.get<long>();   // extract reference (UB if wrong type)
/// long* ptr = any.check<long>(); // extract pointer (nullptr if wrong type)
///
/// @endcode
/// @warning Types must match exactly when extracted -- implicit conversions are not supported.
class Any
{
	// ---------------------------------------------------------------------------------------------------------------------------
	// Public member functions
	public:
		/// @brief Construct empty value
		///
		Any()
		: mPointer()
		, mType(typeid(void))
		{
		}

		/// @brief Construct from arbitrary value
		///
		template <typename T>
		Any(const T& value)
		: mPointer(makeCopied<T>(value))
		, mType(typeid(T))
		{
		}

		/// @brief Copy constructor
		///
		Any(const Any& origin)
		: mPointer(origin.mPointer)
		, mType(origin.mType)
		{
		}

		/// @brief Move constructor
		///
		Any(Any&& source)
		: mPointer(std::move(source.mPointer))
		, mType(source.mType)
		{
		}

		/// @brief Assignment operator from value
		///
		template <typename T>
		Any& operator= (const T& value)
		{
			Any(value).swap(*this);
			return *this;
		}

		/// @brief Copy assignment operator
		///
		Any& operator= (const Any& origin)
		{
			Any(origin).swap(*this);
			return *this;
		}

		/// @brief Move assignment operator
		///
		Any& operator= (Any&& source)
		{
			Any(std::move(source)).swap(*this);
			return *this;
		}

		/// @brief Destructor
		///
		~Any()
		{
		}

		/// @brief Swaps the Any with another Any of the same type.
		///
		void swap(Any& other)
		{
			adlSwap(mPointer, other.mPointer);
			adlSwap(mType, other.mType);
		}

		/// @brief Returns a reference to the contained value.
		/// @warning You must be certain that the contained value is actually of type T. If this assumption does not hold,
		///  the behavior will be undefined.
		/// @see check()
		template <typename T>
		T& get()
		{
			assert(mType == typeid(T));
			return *static_cast<T*>(mPointer.get());
		}

		/// @brief Returns a pointer to the contained value.
		/// @return Address of contained value, if this instance is not empty and its value is of type T; nullptr otherwise.
		///  The behavior is always well-defined.
		/// @see get()
		template <typename T>
		T* check()
		{
			if (mType == typeid(T))
				return static_cast<T*>(mPointer.get());
			else
				return nullptr;
		}

		/// @brief Check if the object is not empty.
		///
		operator SafeBool() const
		{
			return mPointer;
		}

	// ---------------------------------------------------------------------------------------------------------------------------
	// Private variables
	private:
		CopiedPtr<void>				mPointer;
		std::type_index				mType;
};

/// @relates Any
/// @brief Swaps two anies.
AURORA_GLOBAL_SWAP(Any)

/// @}

} // namespace aurora

#endif // AURORA_ANY_HPP
