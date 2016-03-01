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
/// @brief Utilities for preprocessor metaprogramming

// Minimalistic implementation inspired by Boost.Preprocessor
// Tricks used:
// - Map numbers/values to other values: Concatenate macro with value; result is a predefined macro which is then substituted
// - Force expansion: Forward macros to others that effectively do the same (often 2 layers of indirection)
// - Unpack tuples: Call macro without parenthesis around argument
// - Count VA arguments: Forward them to another VA macro with enumerated argument list
// - Detect empty arguments: Exploit property of function-style macros that are not substituted without an argument list
// - Get rid of garbage passed as part of argument: Forward to VA macro that consumes all arguments
//
// Notes:
// - Preprocessor metaprogramming is a whole different approach than anything else from C++; nothing is intuitive
// - AURORA_PP_AT() can be implemented without O(n^2) helper macros, e.g. in <boost/preprocessor/array/elem.hpp>
// - Passing __VA_ARGS__ to non-variadic macros does not work reliably
//   (see http://www.boost.org/doc/libs/1_55_0/libs/preprocessor/doc/topics/variadic_macros.html).
// - Some macros behave differently when deeply nested; it may be difficult to combine them
// - Many macros behave differently on VC++ and g++, just in case it gets boring


#ifndef AURORA_PREPROCESSOR_HPP
#define AURORA_PREPROCESSOR_HPP

#include <TGUI/Aurora/Config.hpp>


/// @addtogroup Meta
/// @{

/// @brief Limit for preprocessor metaprogramming arguments
/// @details This number is the maximum size of preprocessor tuples and the highest number that can be passed to repetition macros.
#define AURORA_PP_LIMIT 5


// Concatenation
#define AURORA_PP_CAT_IMPL(a, b)		a ## b

/// @brief Concatenate two expressions (evaluated ## operator)
/// @details This macro does the same as the built-in ## preprocessor operator, however the arguments are expanded before concatenation.
/// @code
/// #define FIRST  [evaluated 1st]
/// #define SECOND [evaluated 2nd]
/// #define LAZY_CAT() FIRST ## SECOND
/// #define EXP_CAT()  AURORA_PP_CAT(FIRST, SECOND)
///
/// // Comparison between ## and Aurora's catenation macro
/// LAZY_CAT()  // generates FIRSTSECOND
/// EXP_CAT()   // generates [evaluated 1st][evaluated 2nd]
/// @endcode
/// @hideinitializer
#define AURORA_PP_CAT(a, b)				AURORA_PP_CAT_IMPL(a, b)
#define AURORA_PP_CAT3(a, b, c)			AURORA_PP_CAT(AURORA_PP_CAT(a, b), c)
#define AURORA_PP_CAT4(a, b, c, d)		AURORA_PP_CAT3(AURORA_PP_CAT(a, b), c, d)
#define AURORA_PP_CAT5(a, b, c, d, e)	AURORA_PP_CAT4(AURORA_PP_CAT(a, b), c, d, e)


// Convert to string literal
#define AURORA_PP_STRINGIZE_IMPL(a)		#a

/// @brief Convert expression to string (evaluated # operator)
/// @details This macro does the same as the built-in # preprocessor operator, however the argument is expanded before stringization.
/// @code
/// #define MACRO(expr) [evaluated expr]
/// #define LAZY_STRINGIZE(expr) #expr
/// #define EXP_STRINGIZE(expr)  AURORA_PP_STRINGIZE(expr)
///
/// // Comparison between # and Aurora's stringize macro
/// LAZY_STRINGIZE(MACRO(42))  // generates "MACRO(42)"
/// EXP_STRINGIZE(MACRO(42))   // generates "[evaluated 42]"
/// @endcode
/// @hideinitializer
#define AURORA_PP_STRINGIZE(a)			AURORA_PP_STRINGIZE_IMPL(a)


// Object and function macro that evaluate to empty space
#define AURORA_PP_NOTHING
#define AURORA_PP_VA_CONSUME(...)


// Macro that evaluates to its argument without changing it
#define AURORA_PP_ID_IMPL(expr) expr
#define AURORA_PP_ID(expr) AURORA_PP_ID_IMPL(expr)


// Conditional evaluation
#define AURORA_PP_IF_0(trueCase, falseCase)					falseCase
#define AURORA_PP_IF_1(trueCase, falseCase)					trueCase
#define AURORA_PP_IF_2(trueCase, falseCase)					trueCase
#define AURORA_PP_IF_3(trueCase, falseCase)					trueCase
#define AURORA_PP_IF_4(trueCase, falseCase)					trueCase
#define AURORA_PP_IF_5(trueCase, falseCase)					trueCase
#define AURORA_PP_IF_IMPL2(condition, trueCase, falseCase)	AURORA_PP_IF_ ## condition(trueCase, falseCase)
#define AURORA_PP_IF_IMPL(condition, trueCase, falseCase)	AURORA_PP_IF_IMPL2(condition, trueCase, falseCase)

/// @brief Conditional evaluation
/// @details If @c condition evaluates to a positive number, then the expression will be replaced with @c trueCase. If @c condition is zero,
///  then it will be @c falseCase.
/// @n@n When you use function style macros for @c trueCase and @c falseCase, put the argument list after the invocation, i.e. AURORA_PP_IF(...)(args)
/// @code
/// AURORA_PP_IF(0, true, false) == false
/// AURORA_PP_IF(1, true, false) == true
/// AURORA_PP_IF(2, true, false) == true
/// AURORA_PP_IF(3, true, false) == true
/// @endcode
/// @hideinitializer
#define AURORA_PP_IF(condition, trueCase, falseCase)		AURORA_PP_IF_IMPL(condition, trueCase, falseCase)


// Apply macro repeated times, counting from 0 to n
#define AURORA_PP_ENUMERATE_0(macro)
#define AURORA_PP_ENUMERATE_1(macro)			                             macro(0)
#define AURORA_PP_ENUMERATE_2(macro)			AURORA_PP_ENUMERATE_1(macro) macro(1)
#define AURORA_PP_ENUMERATE_3(macro)			AURORA_PP_ENUMERATE_2(macro) macro(2)
#define AURORA_PP_ENUMERATE_4(macro)			AURORA_PP_ENUMERATE_3(macro) macro(3)
#define AURORA_PP_ENUMERATE_5(macro)			AURORA_PP_ENUMERATE_4(macro) macro(4)

/// @brief Apply a macro repeated times
/// @param n Number of invocations
/// @param macro Macro with signature <tt>macro(index)</tt>, where @c index is a number from 0 to n-1.
/// @details Invokes the macro n times, passing it the indices from 0 to n-1 in this order.
/// @code
/// #define GENERATOR(index) [index]
///
/// AURORA_PP_ENUMERATE(4, GENERATOR)
/// // generates:
/// [0] [1] [2] [3]
/// @endcode
/// @hideinitializer
#define AURORA_PP_ENUMERATE(n, macro)			AURORA_PP_CAT(AURORA_PP_ENUMERATE_, n)(macro)


// Apply macro repeated times, counting from 0 to n and using a comma separator
#define AURORA_PP_ENUMERATE_COMMA_0(macro)
#define AURORA_PP_ENUMERATE_COMMA_1(macro)		                                    macro(0)
#define AURORA_PP_ENUMERATE_COMMA_2(macro)		AURORA_PP_ENUMERATE_COMMA_1(macro), macro(1)
#define AURORA_PP_ENUMERATE_COMMA_3(macro)		AURORA_PP_ENUMERATE_COMMA_2(macro), macro(2)
#define AURORA_PP_ENUMERATE_COMMA_4(macro)		AURORA_PP_ENUMERATE_COMMA_3(macro), macro(3)
#define AURORA_PP_ENUMERATE_COMMA_5(macro)		AURORA_PP_ENUMERATE_COMMA_4(macro), macro(4)

/// @brief Apply a macro repeated times, comma-separated
/// @param n Number of invocations
/// @param macro Macro with signature <tt>macro(index)</tt>, where @c index is a number from 0 to n-1.
/// @details Invokes the macro n times, passing it the indices from 0 to n-1 in this order. Puts a comma
///  between each invocation.
/// @code
/// #define GENERATOR(index) [index]
///
/// AURORA_PP_ENUMERATE_COMMA(4, GENERATOR)
/// // generates:
/// [0], [1], [2], [3]
/// @endcode
/// @hideinitializer
#define AURORA_PP_ENUMERATE_COMMA(n, macro)		AURORA_PP_ENUMERATE_COMMA_ ## n(macro)


// Put a comma, if the argument is not zero
#define AURORA_PP_COMMA_IF_0()
#define AURORA_PP_COMMA_IF_1()	,
#define AURORA_PP_COMMA_IF_2()	,
#define AURORA_PP_COMMA_IF_3()	,
#define AURORA_PP_COMMA_IF_4()	,
#define AURORA_PP_COMMA_IF_5()	,

/// @brief Comma if non-zero argument
/// @details Evaluates to a comma if n is positive, and to nothing if n is zero.
/// @hideinitializer
#define AURORA_PP_COMMA_IF(n)	AURORA_PP_COMMA_IF_ ## n()


// Index access to tuple
#define AURORA_PP_AT_S1_N0(v0)					v0
#define AURORA_PP_AT_S2_N0(v0, v1)				v0
#define AURORA_PP_AT_S2_N1(v0, v1)				v1
#define AURORA_PP_AT_S3_N0(v0, v1, v2)			v0
#define AURORA_PP_AT_S3_N1(v0, v1, v2)			v1
#define AURORA_PP_AT_S3_N2(v0, v1, v2)			v2
#define AURORA_PP_AT_S4_N0(v0, v1, v2, v3)		v0
#define AURORA_PP_AT_S4_N1(v0, v1, v2, v3)		v1
#define AURORA_PP_AT_S4_N2(v0, v1, v2, v3)		v2
#define AURORA_PP_AT_S4_N3(v0, v1, v2, v3)		v3
#define AURORA_PP_AT_S5_N0(v0, v1, v2, v3, v4)	v0
#define AURORA_PP_AT_S5_N1(v0, v1, v2, v3, v4)	v1
#define AURORA_PP_AT_S5_N2(v0, v1, v2, v3, v4)	v2
#define AURORA_PP_AT_S5_N3(v0, v1, v2, v3, v4)	v3
#define AURORA_PP_AT_S5_N4(v0, v1, v2, v3, v4)	v4

/// @brief Access element of tuple
/// @param size Size of the tuple. Can be inferred using @ref AURORA_PP_SIZE.
/// @param n Index of the element to access, 0 <= n < size
/// @param tuple Preprocessor tuple such as (a, b, c)
/// @details Returns the n-th element of a tuple.
/// @code
/// AURORA_PP_AT(3, 0, (a, b, c)) == a
/// AURORA_PP_AT(3, 1, (a, b, c)) == b
/// AURORA_PP_AT(3, 2, (a, b, c)) == c
/// @endcode
/// @hideinitializer
#define AURORA_PP_AT(size, n, tuple) AURORA_PP_CAT4(AURORA_PP_AT_S, size, _N, n) tuple


// Iterate over tuple, apply each macro once. macro is MACRO(value, index)
#define AURORA_PP_FOREACH_0(macro, size, tuple)
#define AURORA_PP_FOREACH_1(macro, size, tuple)		                                        macro(AURORA_PP_AT(size, 0, tuple), 0)
#define AURORA_PP_FOREACH_2(macro, size, tuple)		AURORA_PP_FOREACH_1(macro, size, tuple) macro(AURORA_PP_AT(size, 1, tuple), 1)
#define AURORA_PP_FOREACH_3(macro, size, tuple)		AURORA_PP_FOREACH_2(macro, size, tuple) macro(AURORA_PP_AT(size, 2, tuple), 2)
#define AURORA_PP_FOREACH_4(macro, size, tuple)		AURORA_PP_FOREACH_3(macro, size, tuple) macro(AURORA_PP_AT(size, 3, tuple), 3)
#define AURORA_PP_FOREACH_5(macro, size, tuple)		AURORA_PP_FOREACH_4(macro, size, tuple) macro(AURORA_PP_AT(size, 4, tuple), 4)

#define AURORA_PP_FOREACH_SIZED(macro, size, tuple)	AURORA_PP_CAT(AURORA_PP_FOREACH_, size) (macro, size, tuple)

/// @brief Apply macro for each element in tuple
/// @param macro Macro with signature <tt>macro(value, index)</tt>, where @c value is the current element of the tuple and
///  @c index is its index as a number from 0 to n-1.
/// @param tuple Parenthesized tuple, such as (a, b, c). May be empty.
/// @details Applies a macro repeated times, passing it every element in a tuple.
/// @code
/// #define MACRO(value, index)  [index]->value
/// #define TUPLE                (first, second, third)
///
/// AURORA_PP_FOREACH(MACRO, TUPLE)
/// // generates:
/// [0]->first
/// [1]->second
/// [2]->third
/// @endcode
/// @hideinitializer
#define AURORA_PP_FOREACH(macro, tuple)				AURORA_PP_FOREACH_SIZED(macro, AURORA_PP_SIZE(tuple), tuple)


// Iterate over tuple, passing additional data to each macro. macro is MACRO(value, index, data)
#define AURORA_PP_FOREACH_DATA_0(macro, size, tuple, data)
#define AURORA_PP_FOREACH_DATA_1(macro, size, tuple, data)		                                                   macro(AURORA_PP_AT(size, 0, tuple), 0, data)
#define AURORA_PP_FOREACH_DATA_2(macro, size, tuple, data)		AURORA_PP_FOREACH_DATA_1(macro, size, tuple, data) macro(AURORA_PP_AT(size, 1, tuple), 1, data)
#define AURORA_PP_FOREACH_DATA_3(macro, size, tuple, data)		AURORA_PP_FOREACH_DATA_2(macro, size, tuple, data) macro(AURORA_PP_AT(size, 2, tuple), 2, data)
#define AURORA_PP_FOREACH_DATA_4(macro, size, tuple, data)		AURORA_PP_FOREACH_DATA_3(macro, size, tuple, data) macro(AURORA_PP_AT(size, 3, tuple), 3, data)
#define AURORA_PP_FOREACH_DATA_5(macro, size, tuple, data)		AURORA_PP_FOREACH_DATA_4(macro, size, tuple, data) macro(AURORA_PP_AT(size, 4, tuple), 4, data)

#define AURORA_PP_FOREACH_DATA_SIZED(macro, size, tuple, data)	AURORA_PP_CAT(AURORA_PP_FOREACH_DATA_, size) (macro, size, tuple, data)

/// @brief Apply macro for each element in tuple, with additional argument
/// @param macro Macro with signature <tt>macro(value, index, data)</tt>, where @c value is the current element of the tuple,
///  @c index is its index as a number from 0 to n-1, and @c data is the additional argument being forwarded.
/// @param tuple Parenthesized tuple, such as (a, b, c). May be empty.
/// @param data Additional argument to forward.
/// @details Applies a macro repeated times, passing it every element in a tuple. Additional data can be specified that doesn't affect the repetition, but is
///  passed to each macro invocation as an additional argument.
/// @code
/// #define MACRO(value, index, data)  [index]->(value, data)
/// #define TUPLE                      (first, second, third)
///
/// AURORA_PP_FOREACH_DATA(MACRO, TUPLE, 42)
/// // generates:
/// [0]->(first, 42)
/// [1]->(second, 42)
/// [2]->(third, 42)
/// @endcode
/// @hideinitializer
#define AURORA_PP_FOREACH_DATA(macro, tuple, data)				AURORA_PP_FOREACH_DATA_SIZED(macro, AURORA_PP_SIZE(tuple), tuple, data)


// Check whether a variadic argument list starts with (
// Warning: From now on, you are entering the deep abyss of the preprocessor. The following macros depend on black magic; their functioning is probabilistic.
#define AURORA_PP_AURORA_PP_NO				0 AURORA_PP_VA_CONSUME(
#define AURORA_PP_AURORA_PP_YES				1 AURORA_PP_VA_CONSUME(
#define AURORA_PP_VA_CAT2(a, b, ...)		AURORA_PP_CAT(a, b)
#define AURORA_PP_VA_YESNO_TO_10(a, ...)	AURORA_PP_VA_CAT2(AURORA_PP_, a) )
#define AURORA_PP_NO(...)					AURORA_PP_YES

#define AURORA_PP_VA_HAS_PARENTHESIS(...)	AURORA_PP_ID(AURORA_PP_VA_YESNO_TO_10(AURORA_PP_NO __VA_ARGS__))


// VA size inference (argument lists with at least one argument)
#define AURORA_PP_VA_POSITIVE_SIZE_IMPL(e0, e1, e2, e3, e4, e5, size, ...) size
#define AURORA_PP_VA_POSITIVE_SIZE(...) AURORA_PP_CAT(AURORA_PP_VA_POSITIVE_SIZE_IMPL(__VA_ARGS__, 6, 5, 4, 3, 2, 1,), AURORA_PP_NOTHING)


// VA size inference, including empty argument lists
#define AURORA_PP_VA_SIZE_1 1
#define AURORA_PP_VA_SIZE_2 2
#define AURORA_PP_VA_SIZE_3 3
#define AURORA_PP_VA_SIZE_4 4
#define AURORA_PP_VA_SIZE_5 5
#define AURORA_PP_VA_SIZE_6 0

#define AURORA_PP_VA_5COMMAS(...)		,,,,,
#define AURORA_PP_VA_SIZE_IMPL(...)		AURORA_PP_CAT(AURORA_PP_VA_SIZE_, AURORA_PP_VA_POSITIVE_SIZE(__VA_ARGS__))
#define AURORA_PP_VA_SIZE(...)			AURORA_PP_VA_SIZE_IMPL(AURORA_PP_VA_5COMMAS __VA_ARGS__ ())


// Tuple size

/// @brief Size of a non-empty preprocessor tuple
/// @param tuple Preprocessor tuple such as (a, b, c), of which the size is inferred.
/// @details Returns a non-zero number of elements in a tuple. Use this macro whenever you are sure that a tuple will not be empty.
/// @code
/// AURORA_PP_POSITIVE_SIZE( () )     == undefined
/// AURORA_PP_POSITIVE_SIZE( (a) )    == 1
/// AURORA_PP_POSITIVE_SIZE( (a, b) ) == 2
/// AURORA_PP_POSITIVE_SIZE( (()) )   == 1
/// @endcode
/// @hideinitializer
#define AURORA_PP_POSITIVE_SIZE(tuple)	AURORA_PP_VA_POSITIVE_SIZE tuple

#define AURORA_PP_SIZE_IMPL2(tuple)		AURORA_PP_IF(AURORA_PP_VA_HAS_PARENTHESIS tuple, AURORA_PP_VA_POSITIVE_SIZE, AURORA_PP_VA_SIZE) tuple
#define AURORA_PP_SIZE_IMPL1(tuple)		AURORA_PP_SIZE_IMPL2(tuple)

#ifdef _MSC_VER
	#define AURORA_PP_SIZE_IMPL(tuple)	AURORA_PP_CAT(AURORA_PP_SIZE_IMPL1(tuple),)	// works with nested tuples, but not deep inside other macros (especially IF)
	#define AURORA_PP_FLAT_SIZE(tuple)	AURORA_PP_CAT(AURORA_PP_VA_SIZE tuple,)		// doesn't work with nested tuples
#else
	#define AURORA_PP_SIZE_IMPL(tuple)	AURORA_PP_SIZE_IMPL1(tuple)
	#define AURORA_PP_FLAT_SIZE(tuple)	AURORA_PP_CAT(AURORA_PP_VA_SIZE tuple,)
#endif

/// @brief Size of a preprocessor tuple
/// @param tuple Preprocessor tuple such as (a, b, c), of which the size is inferred.
/// @details Returns the number of elements in a tuple. The size inference may fail in special cases with nested macros.
///  If you are sure that a tuple is non-empty, use @ref AURORA_PP_POSITIVE_SIZE instead, which
///  is simpler and more robust.
/// @code
/// AURORA_PP_SIZE( () )     == 0
/// AURORA_PP_SIZE( (a) )    == 1
/// AURORA_PP_SIZE( (a, b) ) == 2
/// AURORA_PP_SIZE( (()) )   == 1
/// @endcode
/// @hideinitializer
#define AURORA_PP_SIZE(tuple) AURORA_PP_SIZE_IMPL(tuple)

/// @}

#endif // AURORA_PREPROCESSOR_HPP
