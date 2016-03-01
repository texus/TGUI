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
/// @brief Metaprogramming and static polymorphism on std::tuple

#ifndef AURORA_TUPLE_HPP
#define AURORA_TUPLE_HPP

#include <TGUI/Aurora/Config.hpp>
#include <TGUI/Aurora/Meta/Templates.hpp>
#include <TGUI/Aurora/Meta/Variadic.hpp>

#include <tuple>


namespace aurora
{
namespace detail
{

	// Metafunction to access tuple elements with dynamic index
	template <typename Tuple, typename Result, std::size_t N>
	struct TupleDynamic
	{
		static Result* get(Tuple& t, std::size_t i)
		{
			if (N-1 == i)
				return &std::get<N-1>(t);
			else
				return TupleDynamic<Tuple, Result, N-1>::get(t, i);
		}
	};

	template <typename Tuple, typename Result>
	struct TupleDynamic<Tuple, Result, 0u>
	{
		static Result* get(Tuple&, std::size_t)
		{
			return nullptr;
		}
	};


	// Metafunction to transform each element of a tuple and infer the return type
	template <typename SrcTuple, typename Transformer, std::size_t N>
	struct TupleTransform
	{
		static auto apply(const SrcTuple& src) -> AURORA_AUTO_RETURN
		(
			std::tuple_cat(
				TupleTransform<SrcTuple, Transformer, N-1>::apply(src),
				std::make_tuple(
					Transformer::transform(std::get<N-1>(src))
				))
		)
	};

	template <typename SrcTuple, typename Transformer>
	struct TupleTransform<SrcTuple, Transformer, 0u>
	{
		static std::tuple<> apply(const SrcTuple&)
		{
			return std::make_tuple();
		}
	};


} // namespace detail

/// @addtogroup Meta
/// @{

/// @brief Access tuple with dynamic index
/// @details Usually, you can only access std::tuple by means of std::get<N>, where N must be a constant expression.
///  This function allows you to pass a dynamic index.
/// @tparam Result A type to which every element in the tuple is convertible, e.g. pointer to common base class.
/// @param t The tuple of which you want to get an element
/// @param i The index of the element, can be known at runtime
template <typename Result, typename Tuple>
Result* dynamicGet(Tuple& t, std::size_t i)
{
	const std::size_t N = std::tuple_size<Tuple>::value;
	return detail::TupleDynamic<Tuple, Result, N>::get(t, i);
}

/// @brief Transform one tuple into another
/// @details Transforms each element of a tuple using a given function.
/// @tparam Transformer Class that has a method <tt>static R transform(T element)</tt> that takes an element of the source tuple and
///  returns a corresponding value of the new tuple. @c transform() can be a function template to account for different element types.
/// @param src Input tuple being transformed.
/// @return Output tuple, result of the transform. Has the same size as @c src, but possibly different element types.
/// @details Example code:
/// @code
/// struct ToDouble
/// {
/// 	static double transform(int i)
/// 	{
/// 		return 2.5 * i;
/// 	}
/// };
///
/// auto tuple = std::make_tuple(1, 2, 4);
/// auto result = aurora::tupleTransform<ToDouble>(tuple);
/// @endcode
template <typename Transformer, typename SrcTuple>
auto tupleTransform(const SrcTuple& src) -> AURORA_AUTO_RETURN
(
	detail::TupleTransform<SrcTuple, Transformer, std::tuple_size<SrcTuple>::value>::apply(src)
)

/// @brief Return the first element of a tuple
///
template <typename Tuple>
auto tupleFront(Tuple&& t) -> AURORA_AUTO_RETURN
(
	std::get<0>(t)
)

/// @brief Return the last element of a tuple
///
template <typename Tuple>
auto tupleBack(Tuple&& t) -> AURORA_AUTO_RETURN
(
	std::get<std::tuple_size<typename std::remove_reference<Tuple>::type>::value - 1>(t)
)

/// @brief Return tuple element with type T
/// @details Does the same thing as std::get<T>(tuple). Some compilers don't support that overload yet.
template <typename T, typename... Us>
T& tupleGet(std::tuple<Us...>& tuple)
{
	return std::get<detail::IndexOfType<T, Us...>::value>(tuple);
}

/// @}

} // namespace aurora

#endif // AURORA_TEMPLATES_HPP
