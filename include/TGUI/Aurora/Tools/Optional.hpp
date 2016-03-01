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
/// @brief Class template aurora::Optional

#ifndef AURORA_OPTIONAL_HPP
#define AURORA_OPTIONAL_HPP

#include <TGUI/Aurora/Config.hpp>
#include <TGUI/Aurora/Tools/SafeBool.hpp>
#include <TGUI/Aurora/Tools/Swap.hpp>

#include <type_traits> // std::aligned_storage
#include <cassert>
#include <new> // placement new (g++ non-conformance, not required by C++ standard)


namespace aurora
{

/// @addtogroup Tools
/// @{

/// @relates Optional
/// @brief Null literal for optional objects
/// @details @c aurora::nullopt can be used to expressively refer to empty optionals. It is convertible to @ref aurora::Optional<T>
///  instances of any type T.
/// @code
/// aurora::Optional<MyClass> function()
/// {
///     if (...)
///         return MyClass(...);
///     else
///         return aurora::nullopt;
/// }
/// @endcode
const struct NulloptType {} nullopt;

/// @relates Optional
/// @brief Tag to construct optional objects in-place
/// @details @c aurora::inplace is used in the <tt>Optional::Optional(InplaceType, Args&&... args)</tt> constructor.
/// @see makeOptional()
const struct InplaceType {} inplace;

/// @brief Represents optional values.
/// @details This class extends objects with an additional @a null state, which explicitly denotes the absence of a valid object.
///  There are many situations where an object can either carry a valid state or none, especially in function return values.
///  For example, let's assume we have a function @c find() that searches for a T object, but doesn't always find one.
///  C++ offers many ways to deal with this situation, but the usual ones come with certain drawbacks:
///  * <tt>T* find()</tt> -- the pointer must refer to a stored object (no temporary) or require dynamic allocation
///  * <tt>iterator find()</tt> + comparison with @c end() -- requires an underlying container and exposes implementation details
///  * <tt>bool has()</tt> + <tt>T find()</tt> -- requires two calls, thus two possibly expensive searches instead of one
///  * <tt>bool find(T& out)</tt> -- user must construct T up-front, requiring a default constructor, incurring useless initialization
///    and preventing const qualifier
///
/// This class offers a new way:
///  * <tt>Optional<T> find()</tt>
///
/// The syntax is very similar to a pointer, with dereferencing operators and conversion to bool contexts:
/// @code
/// aurora::Optional<T> opt = find();
/// if (opt) // check if 'opt' has a value
///     use(*opt); // dereference 'opt' to access the T object
/// @endcode
///
/// You can even simplify this code further, using type inference and the little-known C++ feature of declarations inside if statements:
/// @code
/// if (auto opt = find())
///     use(*opt);
/// @endcode
///
/// This class can be used as an interim solution until <tt>std::optional</tt> becomes part of a C++ standard and is supported by all
///  major compilers. aurora::Optional uses a very similar (although slightly smaller) API.
template <typename T>
class Optional
{
	// ---------------------------------------------------------------------------------------------------------------------------
	// Public member functions
	public:
		/// @brief Construct empty optional with default constructor
		///
		Optional()
		: mFilled(false)
		{
		}

		/// @brief Construct empty optional from @ref aurora::NulloptType "aurora::nullopt"
		///
		Optional(NulloptType)
		: mFilled(false)
		{
		}

#ifdef AURORA_HAS_VARIADIC_TEMPLATES

		/// @brief Construct object in-place, forwarding arguments
		///
		template <typename... Args>
		explicit Optional(InplaceType, Args&&... args)
		: mFilled(false)
		{
			construct(std::forward<Args>(args)...);
		}

#endif // AURORA_HAS_VARIADIC_TEMPLATES

		/// @brief Construct implicitly from T object (move or copy)
		///
		Optional(T object)
		: mFilled(false)
		{
			construct(std::move(object));
		}

		/// @brief Copy constructor
		///
		Optional(const Optional& origin)
		: mFilled(false)
		{
			if (origin.mFilled)
				construct(origin.content());
		}

		/// @brief Move constructor
		///
		Optional(Optional&& source)
		: mFilled(false)
		{
			if (source.mFilled)
			{
				construct(std::move(source.content()));
				source.destroy();
			}
		}

		/// @brief Assign from T object (move or copy)
		///
		Optional& operator= (T object)
		{
			Optional(std::move(object)).swap(*this);
			return *this;
		}

		/// @brief Copy assignment operator
		///
		Optional& operator= (const Optional& origin)
		{
			Optional(origin).swap(*this);
			return *this;
		}

		/// @brief Move assignment operator
		///
		Optional& operator= (Optional&& source)
		{
			Optional(std::move(source)).swap(*this);
			return *this;
		}

		/// @brief Destructor
		///
		~Optional()
		{
			if (mFilled)
				destroy();
		}

		/// @brief Swaps two optional objects.
		///
		void swap(Optional& other)
		{
			// TODO: Exception safety in 2nd and 3rd case

			if (mFilled && other.mFilled)
			{
				adlSwap(content(), other.content());
			}
			else if (mFilled)
			{
				other.construct(std::move(content()));
				this->destroy();
			}
			else if (other.mFilled)
			{
				this->construct(std::move(other.content()));
				other.destroy();
			}
		}

		/// @brief Returns the contained object.
		/// @warning Dereferencing empty optionals yields undefined behavior.
		T& operator* ()
		{
			return content();
		}

		/// @brief Returns the contained object (const overload).
		/// @warning Dereferencing empty optionals yields undefined behavior.
		const T& operator* () const
		{
			return content();
		}

		/// @brief Returns the contained object for member access.
		/// @warning Dereferencing empty optionals yields undefined behavior.
		T* operator-> ()
		{
			return &content();
		}

		/// @brief Returns the contained object for member access (const overload).
		/// @warning Dereferencing empty optionals yields undefined behavior.
		const T* operator-> () const
		{
			return &content();
		}

		/// @brief Check if the optional object contains a value.
		///
		operator SafeBool() const
		{
			return toSafeBool(mFilled);
		}


	// ---------------------------------------------------------------------------------------------------------------------------
	// Private member functions
	private:
		T& content()
		{
			assert(mFilled);
			return reinterpret_cast<T&>(mStorage);
		}

		const T& content() const
		{
			assert(mFilled);
			return reinterpret_cast<const T&>(mStorage);
		}

#ifdef AURORA_HAS_VARIADIC_TEMPLATES

		template <typename... Args>
		void construct(Args&&... args)
		{
			// Exception safety: set mFilled only to true after successful construction

			assert(!mFilled);
			new (&content()) T(std::forward<Args>(args)...);
			mFilled = true;
		}

#else

		void construct(T& source)
		{
			assert(!mFilled);
			new (&content()) T(std::move(source);
			mFilled = true;
		}

#endif // AURORA_HAS_VARIADIC_TEMPLATES

		void destroy()
		{
			assert(mFilled);
			content().~T();
			mFilled = false;
		}

	// ---------------------------------------------------------------------------------------------------------------------------
	// Private variables
	private:
		typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type	mStorage;
		bool																		mFilled;
};

/// @relates Optional
/// @brief Swaps two optionals.
template <typename T>
void swap(Optional<T>& lhs, Optional<T>& rhs)
{
	lhs.swap(rhs);
}

#ifdef AURORA_HAS_VARIADIC_TEMPLATES

/// @relates Optional
/// @brief Emplaces an object directly inside the aurora::Optional object.
/// @details Example:
/// @code
/// // These declarations are semantically equivalent (however different constructors can be called):
/// aurora::Optional<MyClass> opt(aurora::inplace, arg1, arg2);
/// auto opt = aurora::makeOptional<MyClass>(arg1, arg2); // may invoke move constructor
/// aurora::Optional<MyClass> opt = MyClass(arg1, arg2); // may invoke move or copy constructor
/// @endcode
/// @param args Variable argument list, the single arguments are forwarded to T's constructor.
/// @see InplaceType
template <typename T, typename... Args>
Optional<T> makeOptional(Args&&... args)
{
	return Optional<T>(inplace, std::forward<Args>(args)...);
}

#endif // AURORA_HAS_VARIADIC_TEMPLATES

/// @relates Optional
/// @brief Equality comparison
/// @details Semantics: Returns true if both operands are empty, or if both are non-empty
///  and the expression <tt>*lhs == *rhs<tt> returns true.
template <typename T>
bool operator== (const Optional<T>& lhs, const Optional<T>& rhs)
{
	return !lhs && !rhs
		|| lhs && rhs && *lhs == *rhs;
}

/// @relates Optional
/// @brief Inequality comparison
/// @see operator==(const Optional<T>& lhs, const Optional<T>& rhs)
template <typename T>
bool operator!= (const Optional<T>& lhs, const Optional<T>& rhs)
{
	return !(lhs == rhs);
}

/// @}

} // namespace aurora

#endif // AURORA_OPTIONAL_HPP
