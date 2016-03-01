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

#ifndef AURORA_DISPATCHTRAITS_HPP
#define AURORA_DISPATCHTRAITS_HPP

/// @file
/// @brief Class template aurora::DispatchTraits

#include <TGUI/Aurora/Meta/Templates.hpp>
#include <TGUI/Aurora/Config.hpp>

#include <typeindex>
#include <functional>


namespace aurora
{
namespace detail
{
	// Type-information for dereferenced type
	// Important: Do not const-qualify parameters, or the first overload will accept const U*
	template <typename T>
	std::type_index derefTypeid(T& reference)
	{
		return typeid(reference);
	}

	template <typename T>
	std::type_index derefTypeid(T* pointer)
	{
		return typeid(*pointer);
	}

} // namespace detail

// ---------------------------------------------------------------------------------------------------------------------------


/// @addtogroup Dispatch
/// @{

/// @brief Helper base class to implement custom traits for dispatchers
/// @details This class provides some default type definitions and static member functions. Inherit it
///  to customize and extend the traits -- for example, you have to define keyFromBase().
/// @tparam K The key type that identifies the objects to dispatch.
/// @see SingleDispatcher
template <typename K>
struct DispatchTraits
{
	/// @brief Key to differentiate objects
	///
	typedef K Key;

	/// @brief Maps a key to itself (assuming key and type identifier are the same)
	///
	static const Key& keyFromId(const Key& k)
	{
		return k;
	}

	/// @brief Maps a function to itself (no trampoline needed)
	///
	template <typename UnusedId, typename Fn>
	static Fn trampoline1(Fn f)
	{
		return f;
	}

	/// @brief Maps a function to itself (no trampoline needed)
	///
	template <typename UnusedId1, typename UnusedId2, typename Fn>
	static Fn trampoline2(Fn f)
	{
		return f;
	}

	/// @brief Returns a string representation of the key, for debugging
	///
	static const char* name(Key)
	{
		return "unknown";
	}
};

/// @brief Identifies a class using RTTI.
/// @details Default key for SingleDispatcher and DoubleDispatcher. With it, classes are identified using the compiler's
///  RTTI capabilities (in particular, the @c typeid operator).
template <typename S, std::size_t N>
class RttiDispatchTraits
{
	// ---------------------------------------------------------------------------------------------------------------------------
	// Private types
	private:
		typedef typename FunctionResult<S>::Type R;
		typedef typename FunctionParam<S, 0>::Type B;
		typedef typename FunctionParam<S, N>::Type U;

		static_assert(std::is_polymorphic<typename std::remove_pointer<typename std::remove_reference<B>::type>::type>::value,
			"B must be a pointer or reference to a polymorphic base class.");


	// ---------------------------------------------------------------------------------------------------------------------------
	// Public types and static member functions
	public:
		/// @brief Key type.
		///
		typedef std::type_index Key;

		/// @brief Function that takes an object to identify and returns the corresponding std::type_index object.
		///
		static Key keyFromBase(B m)
		{
			return detail::derefTypeid(m);
		}

		/// @brief Function that takes static type information and returns a type-erased std::type_index object.
		///
		template <typename T>
		static Key keyFromId(Type<T> id)
		{
			static_cast<void>(id); // unused parameter
			return typeid(T);
		}

		/// @brief Wraps a function such that the argument is downcast before being passed
		///
		template <typename Id, typename Fn>
		static std::function<S> trampoline1(Fn f)
		{
			return trampoline1<Id, Fn>(f, Int<FunctionArity<S>::value - N>());
		}

		/// @brief Wraps a function such that both arguments are downcast before being passed
		///
		template <typename Id1, typename Id2, typename Fn>
		static std::function<S> trampoline2(Fn f)
		{
			return trampoline2<Id1, Id2, Fn>(f, Int<FunctionArity<S>::value - N>());
		}

		/// @brief Returns a string representation of the key, for debugging
		///
		static const char* name(Key k)
		{
			return k.name();
		}


	// ---------------------------------------------------------------------------------------------------------------------------
	// Private types and static member functions
	private:
		// Implementation for signature without additional argument
		template <typename Id, typename Fn>
		static std::function<S> trampoline1(Fn f, Int<0>)
		{
			return [f] (B arg) mutable -> R
			{
				typedef AURORA_REPLICATE(B, typename Id::type) Derived;
				return f(static_cast<Derived>(arg));
			};
		}

		// Implementation for signature with a user-defined argument
		template <typename Id, typename Fn>
		static std::function<S> trampoline1(Fn f, Int<1>)
		{
			return [f] (B arg, U userData) mutable -> R
			{
				typedef AURORA_REPLICATE(B, typename Id::type) Derived;
				return f(static_cast<Derived>(arg), userData);
			};
		}

		// Implementation for signature without additional argument
		template <typename Id1, typename Id2, typename Fn>
		static std::function<S> trampoline2(Fn f, Int<0>)
		{
			return [f] (B arg1, B arg2) mutable -> R
			{
				typedef AURORA_REPLICATE(B, typename Id1::type) Derived1;
				typedef AURORA_REPLICATE(B, typename Id2::type) Derived2;
				return f(static_cast<Derived1>(arg1), static_cast<Derived2>(arg2));
			};
		}

		// Implementation for signature with a user-defined argument
		template <typename Id1, typename Id2, typename Fn>
		static std::function<S> trampoline2(Fn f, Int<1>)
		{
			return [f] (B arg1, B arg2, U userData) mutable -> R
			{
				typedef AURORA_REPLICATE(B, typename Id1::type) Derived1;
				typedef AURORA_REPLICATE(B, typename Id2::type) Derived2;
				return f(static_cast<Derived1>(arg1), static_cast<Derived2>(arg2), userData);
			};
		}
};

/// @brief Functor doing nothing
/// @tparam R Return type
/// @tparam N Arity (number of arguments)
/// @details Can be used as a fallback function for the dynamic dispatchers. The functor returns a
///  default-constructed object of type R, or nothing if R is void.
template <typename R, unsigned int N>
struct NoOp
{
};

/// @}

template <typename R>
struct NoOp<R, 0>
{
	R operator() () const
	{
		return R();
	}
};

template <typename R>
struct NoOp<R, 1>
{
	template <typename T1>
	R operator() (const T1&) const
	{
		return R();
	}
};

template <typename R>
struct NoOp<R, 2>
{
	template <typename T1, typename T2>
	R operator() (const T1&, const T2&) const
	{
		return R();
	}
};

} // namespace aurora

#endif // AURORA_DISPATCHTRAITS_HPP
