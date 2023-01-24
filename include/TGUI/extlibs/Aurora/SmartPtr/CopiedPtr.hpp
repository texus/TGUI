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
/// @brief Class template aurora::CopiedPtr

#ifndef AURORA_COPIEDPTR_HPP
#define AURORA_COPIEDPTR_HPP

#include <TGUI/extlibs/Aurora/SmartPtr/ClonersAndDeleters.hpp>
#include <TGUI/extlibs/Aurora/SmartPtr/Detail/PtrOwner.hpp>
#include <TGUI/extlibs/Aurora/Tools/SafeBool.hpp>
#include <TGUI/extlibs/Aurora/Tools/Swap.hpp>
#include <TGUI/extlibs/Aurora/Config.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
	#include <algorithm>
	#include <type_traits>
	#include <utility>
#endif


namespace aurora
{

/// @addtogroup SmartPtr
/// @{

/// @brief Copyable smart pointer template
/// @tparam T %Type of the stored pointee object. Can be void.
/// @details This class supports automatic destruction and copying. It has full value semantics, i.e. the stored object is
///  destroyed in the smart pointer's destructor and copied in the smart pointer's copy constructor. The concept of custom
///  cloners and deleters allows you to specify the way how destructions and copies are performed.
/// @n@n This smart pointer is always unique, no two %CopiedPtr instances share the same object.
/// @n@n Every operation provides at least the strong exception guarantee: When copies of the pointee object throw an exception,
///  the smart pointer will remain in a valid (uncommitted) state and no memory will be leaked. Move construction and move assignment
///  from the same CopiedPtr<T> type additionally offer the nothrow guarantee; they involve neither copies nor moves of the pointee object.
///  Operations on empty smart pointers (such as default constructor, copies/moves from null pointers) also provide the nothrow
///  guarantee; they never involve any dynamic allocation.
template <typename T>
class CopiedPtr
{
	// ---------------------------------------------------------------------------------------------------------------------------
	// Public member functions
	public:
		/// @brief Default constructor
		/// @details Initializes the smart pointer with a null pointer.
		CopiedPtr()
		: mOwner(nullptr)
		, mPointer(nullptr)
		{
		}

		/// @brief Construct from nullptr
		/// @details Allows conversions from the @c nullptr literal to a CopiedPtr.
		CopiedPtr(std::nullptr_t)
		: mOwner(nullptr)
		, mPointer(nullptr)
		{
		}

		/// @brief Construct from raw pointer
		/// @param pointer Initial pointer value, can be nullptr. Must be convertible to T*.
		template <typename U>
		explicit CopiedPtr(U* pointer)
		: mOwner( detail::newPtrOwner<T>(pointer, OperatorNewCopy<U>(), OperatorDelete<U>()) )
		, mPointer(pointer)
		{
		}

		/// @brief Construct from raw pointer with cloner
		/// @param pointer Initial pointer value, can be nullptr. Must be convertible to T*.
		/// @param cloner Callable with signature <b>T*(const T*)</b> that is invoked during CopiedPtr copies.
		///  Must return a pointer to a copy of the argument.
		/// @details Uses OperatorDelete<U> as deleter. Make sure your cloner returns an object allocated with new.
		template <typename U, typename C>
		CopiedPtr(U* pointer, C cloner)
		: mOwner( detail::newPtrOwner<T>(pointer, cloner, OperatorDelete<U>()) )
		, mPointer(pointer)
		{
		}

		/// @brief Construct from raw pointer with cloner and deleter
		/// @param pointer Initial pointer value, can be nullptr. Must be convertible to T*.
		/// @param cloner Callable with signature <b>T*(const T*)</b> that is invoked during CopiedPtr copies.
		///  Must return a pointer to a copy of the argument.
		/// @param deleter Callable with signature <b>void(T*)</b> that is invoked during CopiedPtr destruction.
		template <typename U, typename C, typename D>
		CopiedPtr(U* pointer, C cloner, D deleter)
		: mOwner( detail::newPtrOwner<T>(pointer, cloner, deleter) )
		, mPointer(pointer)
		{
		}

		/// @brief Copy constructor
		/// @param origin Original smart pointer
		/// @details If the origin's pointer is @c nullptr, this pointer will also be @c nullptr.
		///  Otherwise, this instance will hold the pointer returned by the cloner.
		CopiedPtr(const CopiedPtr& origin)
		: mOwner(origin ? origin.mOwner->clone() : nullptr)
		, mPointer(mOwner ? mOwner->getPointer() : nullptr)
		{
		}

		/// @brief Construct from different %CopiedPtr
		/// @param origin Original smart pointer, where U* convertible to T*. Can refer to a derived object.
		/// @details If the origin's pointer is @c nullptr, this pointer will also be @c nullptr.
		///  Otherwise, this instance will hold the pointer returned by the cloner.
		template <typename U>
		CopiedPtr(const CopiedPtr<U>& origin)
		: mOwner(origin ? new detail::PtrIndirection<T, U>(origin.mOwner, detail::CopyTag()) : nullptr)
		, mPointer(origin ? mOwner->getPointer() : nullptr)
		{
		}

		/// @brief Move constructor
		/// @param source RValue reference to object of which the ownership is taken.
		CopiedPtr(CopiedPtr&& source) noexcept
		: mOwner(source.mOwner)
		, mPointer(source.mPointer)
		{
			source.mOwner = nullptr;
			source.mPointer = nullptr;
		}

		/// @brief Move from different %CopiedPtr
		/// @param source RValue reference to object of which the ownership is taken.
		/// @details In contrast to the move constructor, this constructor does not offer the nothrow guarantee. Only
		/// strong exception guarantee is provided (because of an internal allocation; the pointee object is not copied).
		template <typename U>
		CopiedPtr(CopiedPtr<U>&& source)
		: mOwner(source ? new detail::PtrIndirection<T, U>(source.mOwner, detail::MoveTag()) : nullptr)
		, mPointer(source.mPointer)
		{
			source.mOwner = nullptr;
			source.mPointer = nullptr;
		}

#ifdef AURORA_HAS_VARIADIC_TEMPLATES

		// [Implementation detail]
		// Emplacement constructor: Used to implement makeCopied<T>(args)
		template <typename... Args>
		CopiedPtr(detail::EmplaceTag, Args&&... args)
		: mOwner(new detail::CompactOwner<T>(detail::EmplaceTag(), std::forward<Args>(args)...))
		, mPointer(mOwner->getPointer())
		{
		}

#endif // AURORA_HAS_VARIADIC_TEMPLATES

		/// @brief Copy assignment operator
		/// @param origin Original smart pointer
		/// @details Can imply a copy and a destruction, invoking origin's cloner and this deleter.
		CopiedPtr& operator= (const CopiedPtr& origin)
		{
			CopiedPtr(origin).swap(*this);
			return *this;
		}

		/// @brief Copy-assign from different CopiedPtr
		/// @param origin Original smart pointer, where U* convertible to T*. Can refer to a derived object.
		/// @details Can imply a copy and a destruction, invoking origin's cloner and this deleter.
		template <typename U>
		CopiedPtr& operator= (const CopiedPtr<U>& origin)
		{
			CopiedPtr(origin).swap(*this);
			return *this;
		}

		/// @brief Move assignment operator
		/// @param source RValue reference to object of which the ownership is taken.
		CopiedPtr& operator= (CopiedPtr&& source) noexcept
		{
			CopiedPtr(std::move(source)).swap(*this);
			return *this;
		}

		/// @brief Move-assign from different CopiedPtr
		/// @param source RValue reference to object of which the ownership is taken.
		/// @details In contrast to the move assignment operator, this assignment operator does not offer the nothrow guarantee.
		/// Only strong exception guarantee is provided (because of an internal allocation; the pointee object is not copied).
		template <typename U>
		CopiedPtr& operator= (CopiedPtr<U>&& source)
		{
			CopiedPtr(std::move(source)).swap(*this);
			return *this;
		}

		/// @brief Destructor
		/// @details Invokes the deleter with the stored pointer as argument.
		~CopiedPtr()
		{
			// Destroy owner, whose destructor also deletes the pointer
			delete mOwner;
		}

		/// @brief Exchanges the values of *this and @c other.
		///
		void swap(CopiedPtr& other)
		{
			adlSwap(mOwner, other.mOwner);
			adlSwap(mPointer, other.mPointer);
		}

		/// @brief Dereferences the pointer.
		///
		AURORA_FAKE_DOC(typename std::add_lvalue_reference<T>::type, T&) operator* () const
		{
			assert(mPointer);
			return *mPointer;
		}

		/// @brief Dereferences the pointer for member access.
		///
		T* operator-> () const
		{
			assert(mPointer);
			return mPointer;
		}

		/// @brief Checks if the smart pointer is not nullptr.
		/// @details Allows expressions of the form <tt>if (ptr)</tt> or <tt>if (!ptr)</tt>.
		/// @return Value convertible to true, if CopiedPtr is not empty; value convertible to false otherwise
		operator SafeBool() const
		{
			return toSafeBool(mPointer != nullptr);
		}

		/// @brief Permits access to the internal pointer. Designed for rare use.
		/// @return Internally used pointer, use it wisely not to upset the CopiedPtr's memory management.
		T* get() const
		{
			return mPointer;
		}

		/// @brief Reset to null pointer
		/// @details If this instance currently holds a pointer, the old deleter is invoked.
		void reset()
		{
			CopiedPtr().swap(*this);
		}

		/// @brief Reset to raw pointer
		/// @param pointer Initial pointer value, can be nullptr. Must be convertible to T*.
		/// @details If this instance currently holds a pointer, the old deleter is invoked.
		template <typename U>
		void reset(U* pointer)
		{
			CopiedPtr(pointer).swap(*this);
		}

		/// @brief Reset to raw pointer with cloner
		/// @param pointer Initial pointer value, can be nullptr. Must be convertible to T*.
		/// @param cloner Callable with signature <b>T*(const T*)</b> that is invoked during CopiedPtr copies.
		///  Must return a pointer to a copy of the argument.
		/// @details If this instance currently holds a pointer, the old deleter is invoked.
		///  @n Uses OperatorDelete<U> as deleter. Make sure your cloner returns an object allocated with new.
		template <typename U, typename C>
		void reset(U* pointer, C cloner)
		{
			CopiedPtr(pointer, cloner).swap(*this);
		}

		/// @brief Reset to raw pointer with cloner and deleter
		/// @param pointer Initial pointer value, can be nullptr. Must be convertible to T*.
		/// @param cloner Callable with signature <b>T*(const T*)</b> that is invoked during CopiedPtr copies.
		///  Must return a pointer to a copy of the argument.
		/// @param deleter Callable with signature <b>void(T*)</b> that is invoked during CopiedPtr destruction.
		/// @details If this instance currently holds a pointer, the old deleter is invoked.
		template <typename U, typename C, typename D>
		void reset(U* pointer, C cloner, D deleter)
		{
			CopiedPtr(pointer, cloner, deleter).swap(*this);
		}


	// ---------------------------------------------------------------------------------------------------------------------------
	// Private variables
	private:
		detail::PtrOwnerBase<T>*	mOwner;
		T*							mPointer;

	template <typename T2>
	friend class CopiedPtr;
};


/// @relates CopiedPtr
/// @brief Swaps the contents of two CopiedPtr instances.
template <typename T>
void swap(CopiedPtr<T>& lhs, CopiedPtr<T>& rhs)
{
	return lhs.swap(rhs);
}


// For documentation and modern compilers
#ifdef AURORA_HAS_VARIADIC_TEMPLATES

/// @relates CopiedPtr
/// @brief Emplaces an object directly inside the copied pointer.
/// @param args Variable argument list, the single arguments are forwarded to T's constructor. If your compiler does not
/// support variadic templates, the number of arguments must be smaller than @ref AURORA_PP_LIMIT.
/// @details This function has several advantages over the regular CopiedPtr(U*) constructor:
/// * The new operator is encapsulated, making code exception-safe (in cases where multiple temporary smart pointers are constructed).
/// * This function is considerably more efficient and requires less memory, because pointee and cloner/deleter can be stored together.
/// * You avoid mentioning the pointee type twice.
///
/// Example:
/// @code
/// auto ptr = aurora::makeCopied<MyClass>(arg1, arg2); // instead of
/// aurora::CopiedPtr<MyClass> ptr(new MyClass(arg1, arg2));
/// @endcode
template <typename T, typename... Args>
CopiedPtr<T> makeCopied(Args&&... args)
{
	return CopiedPtr<T>(detail::EmplaceTag(), std::forward<Args>(args)...);
}

// Unoptimized fallback for compilers that don't support variadic templates, emulated by preprocessor metaprogramming
#else  // AURORA_HAS_VARIADIC_TEMPLATES

#include <TGUI/Aurora/SmartPtr/Detail/Factories.hpp>

// Define metafunction to generate overloads for aurora::CopiedPtr
#define AURORA_DETAIL_COPIEDPTR_FACTORY(n) AURORA_DETAIL_SMARTPTR_FACTORY(CopiedPtr, makeCopied, n)

// Generate code
AURORA_PP_ENUMERATE(AURORA_PP_LIMIT, AURORA_DETAIL_COPIEDPTR_FACTORY)

#endif // AURORA_HAS_VARIADIC_TEMPLATES

/// @}

} // namespace aurora

#endif // AURORA_COPIEDPTR_HPP
