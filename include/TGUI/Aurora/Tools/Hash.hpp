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
/// @brief Extensions to std::hash

#ifndef AURORA_HASH_HPP
#define AURORA_HASH_HPP

#include <TGUI/Aurora/Meta/Templates.hpp>

#include <functional>


namespace aurora
{

/// @addtogroup Tools
/// @{

/// @brief Computes the hash value of an object (short-hand notation).
///
template <typename T>
std::size_t hashValue(const T& object
	AURORA_ENABLE_IF(!std::is_enum<T>::value))
{
	std::hash<T> hasher;
	return hasher(object);
}

// Overload for enums
template <typename T>
std::size_t hashValue(const T& enumerator
	AURORA_ENABLE_IF(std::is_enum<T>::value))
{
	return hashValue(static_cast<typename std::underlying_type<T>::type>(enumerator));
}

/// @brief Combines a hash with the hash value of another object.
///
template <typename T>
void hashCombine(std::size_t& seed, const T& object)
{
	// Implementation from Boost.Functional/Hash
	seed ^= hashValue(object) + 0x9e3779b9u + (seed << 6) + (seed >> 2);
}

/// @brief Combines a hash with the hash value of a range of objects.
///
template <typename Itr>
void hashRange(std::size_t& seed, Itr begin, Itr end)
{
	for (; begin != end; ++begin)
		hashCombine(seed, *begin);
}

/// @brief Hash object for std::pair
///
struct PairHasher
{
	template <typename T, typename U>
	std::size_t operator() (const std::pair<T, U>& pair) const
	{
		std::size_t hash = 0u;
		hashCombine(hash, pair.first);
		hashCombine(hash, pair.second);

		return hash;
	}
};

/// @}

} // namespace aurora

#endif // AURORA_HASH_HPP
