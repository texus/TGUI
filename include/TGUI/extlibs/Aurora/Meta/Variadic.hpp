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
/// @brief Metaprogramming helpers for variadic templates

#ifndef AURORA_VARIADIC_HPP
#define AURORA_VARIADIC_HPP

#include <TGUI/extlibs/Aurora/Config.hpp>

#include <utility> // std::forward()

#ifdef AURORA_HAS_VARIADIC_TEMPLATES

namespace aurora
{

template <typename... Ts>
struct Typelist;

namespace detail
{

	// Reverse of NthType: get index of type T in typelist Types
	template <typename T, typename... Types>
	struct IndexOfType;

	template <typename T, typename Head, typename... Tail>
	struct IndexOfType<T, Head, Tail...>
	{
		static const std::size_t value = 1 + IndexOfType<T, Tail...>::value;
	};

	template <typename T, typename... Tail>
	struct IndexOfType<T, T, Tail...>
	{
		static const std::size_t value = 0;
	};


	// Metafunction to apply a class template 'Function' to each type of variadic pack typelist
	template <typename Function, typename... Ts>
	struct ForEachType;

	template <typename Function, typename T, typename... Ts>
	struct ForEachType<Function, T, Ts...>
	{
		static void apply(Function&& fn)
		{
			// Note: Calling member operator template with explicit template argument is not too elegant
			// Possible alternative is aurora::Type<T> argument.
#ifdef _MSC_VER
			fn.operator()<T>();
#else
			fn.template operator()<T>();
#endif
			ForEachType<Function, Ts...>::apply(std::forward<Function>(fn));
		}
	};

	template <typename Function>
	struct ForEachType<Function>
	{
		static void apply(Function&& /*fn*/)
		{
		}
	};

	// Specialization for typelists: Unpack arguments and use ForEachType as usual
	template <typename Function, typename... Ts>
	struct ForEachType<Function, Typelist<Ts...>> : ForEachType<Function, Ts...>
	{
	};


	// Metafunction to apply a class template 'Function' to each value of variadic pack
	template <typename Function, typename... Ts>
	struct ForEachValue;

	template <typename Function, typename T, typename... Ts>
	struct ForEachValue<Function, T, Ts...>
	{
		static void apply(Function&& fn, T&& t, Ts&&... ts)
		{
			fn(std::forward<T>(t));
			ForEachValue<Function, Ts...>::apply(std::forward<Function>(fn), std::forward<Ts>(ts)...);
		}
	};

	template <typename Function>
	struct ForEachValue<Function>
	{
		static void apply(Function&& /*fn*/)
		{
		}
	};

} // namespace detail


/// @addtogroup Meta
/// @{

/// @brief Class template to store a list of types
/// @details Variadic parameter packs are neither types nor templates, which makes them sometimes uncomfortable to work with.
/// Typelists mitigate this by giving parameter packs first-class semantics. For example, they can be used for overload resolution
/// (similar to aurora::Type), or they make it possible to pass around more than a single list of types. Of course, it is also
/// possible to apply a wide variety of metafunctions on a typelist.
/// @n@n Typelist instances carry no data.
template <typename... Ts>
struct Typelist
{
};

/// @brief Concatenate two typelists
/// @details Metafunction that puts two typelists together. The @c Type member holds the resulting aurora::Typelist type.
template <typename LhsTypelist, typename RhsTypelist>
struct TypelistCat;

template <typename... Ts, typename... Us>
struct TypelistCat<Typelist<Ts...>, Typelist<Us...>>
{
	typedef Typelist<Ts..., Us...> Type;
};


/// @brief Check if type is part of typelist
/// @details The boolean @c value member constant is true when @c T is a member of @c Types, false when not.
template <typename Typelist1, typename T>
struct TypelistContains;

template <typename U, typename... Us, typename T>
struct TypelistContains<Typelist<U, Us...>, T>
{
	static const bool value =
		std::is_same<U, T>::value || TypelistContains<Typelist<Us...>, T>::value;
};

template <typename T>
struct TypelistContains<Typelist<>, T>
{
	static const bool value = false;
};


/// @brief Apply function for each type in variadic parameter pack
/// @tparam Ts List of types to iterate through. Can also be a single type of aurora::Typelist<...>.
/// @param fn Function object with a member function template <b>void operator() ()</b>.
/// @details For each type @c T in @c Ts, the @c fn's operator() is called with explicit template argument @c T. Example:
/// @code
/// struct Example
/// {
///     template <typename T>
///     void operator() ()
///     {
///         std::cout << typeid(T).name() << ' ';
///     }
/// };
///
/// aurora::foreach<int, float>(Example()); // output: "int float"
/// @endcode
template <typename... Ts, typename Function>
void foreach(Function&& fn)
{
	detail::ForEachType<Function, Ts...>::apply(std::forward<Function>(fn));
}

/// @brief Apply function for each value in variadic parameter pack
/// @param fn Function object with a member function template <b>void operator() (T& value)</b>, or C++14 generic lambda.
///  The parameter must be compatible to type @c T, however pass by value or by (const) (lvalue|rvalue) reference is allowed.
/// @param args Arguments to iterate through.
/// @details For each argument @c arg in @c args, the expression <b>fn(arg)</b> is evaluated. Example:
/// @code
/// struct Example
/// {
///     template <typename T>
///     void operator() (const T& arg)
///     {
///         std::cout << typeid(T).name() << '=' << arg << ' ';
///     }
/// };
///
/// aurora::foreach(Example(), 4, 3.25f); // output: "int=4 float=3.25"
/// @endcode
template <typename... Ts, typename Function>
void foreach(Function&& fn, Ts&&... args)
{
	detail::ForEachValue<Function, Ts...>::apply(std::forward<Function>(fn), std::forward<Ts>(args)...);
}

/// @}

} // namespace aurora

#endif // AURORA_HAS_VARIADIC_TEMPLATES
#endif // AURORA_VARIADIC_HPP
