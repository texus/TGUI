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
/// @brief Utilities for template metaprogramming

#ifndef AURORA_TEMPLATES_HPP
#define AURORA_TEMPLATES_HPP

#include <TGUI/extlibs/Aurora/Config.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
	#include <type_traits>
#endif


namespace aurora
{
namespace detail
{

	// Get type by index in list
	template <std::size_t N, typename T0, typename T1, typename T2>
	struct NthType;

	template <typename T0, typename T1, typename T2>
	struct NthType<0, T0, T1, T2>
	{
		typedef T0 Type;
	};

	template <typename T0, typename T1, typename T2>
	struct NthType<1, T0, T1, T2>
	{
		typedef T1 Type;
	};

	template <typename T0, typename T1, typename T2>
	struct NthType<2, T0, T1, T2>
	{
		typedef T2 Type;
	};


	// Helper for FunctionSignature metafunction
	template <typename R, std::size_t N, typename P0, typename P1, typename P2>
	struct FunctionSignatureBase
	{
		typedef R ResultType;
		static const std::size_t arity = N;

		template <std::size_t M>
		struct Param
		{
			typedef typename NthType<M, P0, P1, P2>::Type Type;
		};
	};


	// Type denoting "nothing"
	// We don't use void because it can't be used in some contexts (parameter lists)
	struct EmptyType {};


	// Provides member typedefs for function return and parameter types:
	// FunctionSignature<S>::ResultType      return type
	// FunctionSignature<S>::Param<N>::Type  N-th parameter type (use ::template Param in dependent context)
	template <typename S>
	struct FunctionSignature;

	template <typename R>
	struct FunctionSignature<R()> : FunctionSignatureBase<R, 0, EmptyType, EmptyType, EmptyType>
	{
	};

	template <typename R, typename P0>
	struct FunctionSignature<R(P0)> : FunctionSignatureBase<R, 1, P0, EmptyType, EmptyType>
	{
	};

	template <typename R, typename P0, typename P1>
	struct FunctionSignature<R(P0, P1)> : FunctionSignatureBase<R, 2, P0, P1, EmptyType>
	{
	};

	template <typename R, typename P0, typename P1, typename P2>
	struct FunctionSignature<R(P0, P1, P2)> : FunctionSignatureBase<R, 3, P0, P1, P2>
	{
	};

} // namespace detail


/// @addtogroup Meta
/// @{

/// @brief Simple type wrapper, can be used for overload resolution.
///
template <typename T>
struct Type
{
	typedef T type;
};

/// @brief Simple integer wrapper, can be used for overload resolution.
///
template <int N>
struct Int
{
	static const int value = N;
};

/// @brief Find out the return type of a function.
/// @details Contains a member typedef @c Type. Example:
/// @code
/// FunctionResult<void(int, double)>::Type // is void
/// @endcode
template <typename Signature>
struct FunctionResult
{
	typedef typename detail::FunctionSignature<Signature>::ResultType Type;
};

/// @brief Find out the N-th parameter type of a function.
/// @details Contains a member typedef @c Type. The type is unspecified if there exists no such parameter. Example:
/// @code
/// FunctionParam<void(int, double), 1>::Type // is double
/// @endcode
template <typename Signature, std::size_t N>
struct FunctionParam
{
	typedef typename detail::FunctionSignature<Signature>::template Param<N>::Type Type;
};

/// @brief Find out the number of parameters of a function.
/// @details Contains a member constant @c value. Example:
/// @code
/// FunctionArity<void(int, double)>::value // is 2
/// @endcode
template <typename Signature>
struct FunctionArity
{
	static const std::size_t value = detail::FunctionSignature<Signature>::arity;
};


/// @brief SFINAE Enable If Macro for parameter lists
/// @details Usage:
/// @code
/// template <typename T>
/// void Function(T value
///     AURORA_ENABLE_IF(std::is_integral<T>::value));
/// @endcode
#define AURORA_ENABLE_IF(...)  , typename std::enable_if<__VA_ARGS__>::type* = nullptr


#if defined(AURORA_DOXYGEN_SECTION)

/// @brief Macro to ensure complete type
/// @details Usage:
/// @code
/// template <typename T>
/// void CheckedDelete(T* pointer)
/// {
///     AURORA_REQUIRE_COMPLETE_TYPE(T);
///     delete pointer;
/// }
/// @endcode
/// @hideinitializer
#define AURORA_REQUIRE_COMPLETE_TYPE(Type) ImplementationDefined

#elif defined(__GNUC__) || defined(__clang__)

	// g++ and clang issue a warning regarding the unused typedef
	#define AURORA_REQUIRE_COMPLETE_TYPE(Type) typedef char auroraRequireCompleteType[(sizeof(Type))] __attribute__((unused))

#else

	#define AURORA_REQUIRE_COMPLETE_TYPE(Type) typedef char auroraRequireCompleteType[(sizeof(Type))]

#endif


/// @brief Function declaration with inferred return type
/// @details Based on the C++11 trailing return types feature. Since a lot of functions that benefit from trailing return
///  types must repeat the return type by means of decltype, this macro has been written to avoid the code duplication.
///  @n Example:
/// @code
/// template <typename A, typename B>
/// auto sum(A a, B b) -> AURORA_AUTO_RETURN(a + b)
/// @endcode
#define AURORA_AUTO_RETURN(...) decltype(__VA_ARGS__) { return (__VA_ARGS__); }


/// @}

// ---------------------------------------------------------------------------------------------------------------------------


namespace detail
{

	// Removes reference and pointer attribute from type
	template <typename T>
	struct RemoveIndirection
	{
		typedef typename std::remove_pointer<
			typename std::remove_reference<T>::type
		>::type type;
	};


	// Removes reference, pointer and const attribute from type
	template <typename T>
	struct RawType
	{
		typedef typename std::remove_const<
			typename RemoveIndirection<T>::type
		>::type type;
	};


	// Adjusts New such that it has the same const, pointer, reference attributes as Origin
	template <typename Origin, typename New>
	struct Replicate
	{
		typedef typename RawType<New>::type																			raw;

		typedef typename std::conditional<
			std::is_const<typename RemoveIndirection<Origin>::type>::value,
			const raw,
			raw>::type																								c_qualified;

		typedef typename std::conditional<std::is_pointer<Origin>::value, c_qualified*, c_qualified>::type			cp_qualified;
		typedef typename std::conditional<std::is_reference<Origin>::value, cp_qualified&, cp_qualified>::type		cpr_qualified;
		typedef typename std::conditional<std::is_const<Origin>::value, const cpr_qualified, cpr_qualified>::type	type;
	};

	// Human-readable form
	#define AURORA_REPLICATE(Origin, New) typename aurora::detail::Replicate<Origin, New>::type

} // namespace detail
} // namespace aurora

#endif // AURORA_TEMPLATES_HPP
