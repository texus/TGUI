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
/// @brief Class template aurora::PImpl

#ifndef AURORA_PIMPL_HPP
#define AURORA_PIMPL_HPP

#include <TGUI/extlibs/Aurora/Config.hpp>
#include <TGUI/extlibs/Aurora/Tools/Swap.hpp>

#include <type_traits> // std::aligned_storage


namespace aurora
{

/// @addtogroup Tools
/// @{

/// @brief Fast %PImpl idiom
/// @details Enables the %PImpl (pointer to implementation, aka. handle-body, compiler firewall) idiom without dynamic allocation. Storage
///   for the implementation object is provided within the front-end object, but the type need not be complete at time of instantiation.
/// @tparam T %Type of implementation object. Is incomplete at time of instantiation.
/// @tparam Size Storage size large enough to hold a type T: @c Size >= sizeof(T)
/// @tparam Align Alignment for type T: @c Align == alignof(T)
///
/// @details This class mimicks value semantics, not pointer semantics. In particular, the class invariant is that a PImpl<T, ...> object holds
///  a fully constructed T object at any time. Its constructor forwards the arguments directly to T's constructor. Copy semantics
///  and move semantics are provided, as long as the underlying type T supports them. The implementation object can be accessed using
///  dereferencing operators.
/// @n@n Usage is completely safe, as the size and alignment are validated at compile time. In order to find out suitable values for @c Size
///  and @c Align, you can instantiate a @c PImpl<T, 1, 0> object, which will always lead to a failed static assertion. When reading the
///  compiler error, watch out for a @c correctSizeAndAlignWouldBe<S, A>() function instantiation, where @c S and @c A hold the correct values.
/// @n@n Keep in mind that using this technique comes with certain trade-offs. Since the implementation objects are constructed in-place,
///  they increase the size of the front-end object. Its copy and move operations are thus more expensive than simple pointer copy.
//// Constructors, destructors and assignment operators of the front-end class holding the PImpl instance as a member must be
///  defined in a place where T is a complete type -- usually the .cpp file that includes T's definition. So, you cannot use the
///  compiler-generated special member functions, and it's necessary to define even empty destructors in the .cpp file. As the size
///  and alignment of T must be hardcoded, additional maintenance may be necessary (but you cannot miss it because of static assertions).
///  Furthermore, your code becomes less portable, because different compilers may use different sizes and alignments for the same type.
///  Using PImpl instances in the front-end class makes revisions of the latter potentially binary incompatible, if the size or
///  alignment of the implementation object changes. You can mitigate this issue by providing a larger size than necessary, at
///  the expense of wasted memory.
template <typename T, std::size_t Size, std::size_t Align>
class PImpl
{
	// ---------------------------------------------------------------------------------------------------------------------------
	// Public member functions
	public:
		/// @brief Construct implementation object, forwarding arguments
		///
		template <typename... Args>
		explicit PImpl(Args&&... args)
		{
			check();
			new (storage()) T(std::forward<Args>(args)...);
		}

		/// @brief Copy constructor
		///
		PImpl(const PImpl& origin)
		{
			check();
			new (storage()) T(*origin.storage());
		}

		/// @brief Move constructor
		///
		PImpl(PImpl&& source)
		{
			check();
			new (storage()) T(std::move(*source.storage()));
		}

		/// @brief Copy assignment operator
		///
		PImpl& operator= (const PImpl& origin)
		{
			PImpl(origin).swap(*this);
			return *this;
		}

		/// @brief Move assignment operator
		///
		PImpl& operator= (PImpl&& source)
		{
			PImpl(std::move(source)).swap(*this);
			return *this;
		}

		/// @brief Destructor
		///
		~PImpl()
		{
			storage()->~T();
		}

		/// @brief Swaps two implementation objects.
		///
		void swap(PImpl& other)
		{
			adlSwap(*storage(), *other.storage());
		}

		/// @brief Returns the implementation object.
		///
		T& operator* ()
		{
			return *storage();
		}

		/// @brief Returns the implementation object (const overload).
		///
		const T& operator* () const
		{
			return *storage();
		}

		/// @brief Returns the implementation object for member access.
		///
		T* operator-> ()
		{
			return storage();
		}

		/// @brief Returns the implementation object for member access (const overload).
		///
		const T* operator-> () const
		{
			return storage();
		}


	// ---------------------------------------------------------------------------------------------------------------------------
	// Private member functions
	private:
		T* storage()
		{
			return reinterpret_cast<T*>(&mStorage);
		}

		const T* storage() const
		{
			return reinterpret_cast<const T*>(&mStorage);
		}

		void check()
		{
			correctSizeAndAlignWouldBe<sizeof(T), std::alignment_of<T>::value>();
		}

		template <std::size_t ActualSize, std::size_t ActualAlign>
		void correctSizeAndAlignWouldBe()
		{
			static_assert(ActualSize <= Size, "PImpl: Size too small to hold type T");
			static_assert(ActualAlign == Align, "PImpl: Alignment mismatch for type T");
		}


	// ---------------------------------------------------------------------------------------------------------------------------
	// Private variables
	private:
		typename std::aligned_storage<Size, Align>::type mStorage;
};

/// @relates PImpl
/// @brief Swaps two implementation objects.
template <typename T, std::size_t Size, std::size_t Align>
void swap(PImpl<T, Size, Align>& lhs, PImpl<T, Size, Align>& rhs)
{
	lhs.swap(rhs);
}

/// @}

} // namespace aurora

#endif // AURORA_PIMPL_HPP
