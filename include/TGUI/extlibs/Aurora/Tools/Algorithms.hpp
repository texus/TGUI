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
/// @brief Algorithms to assist the STL

#ifndef AURORA_ALGORITHMS_HPP
#define AURORA_ALGORITHMS_HPP

#include <TGUI/extlibs/Aurora/Tools/Swap.hpp>

#include <algorithm>
#include <cassert>


namespace aurora
{

/// @addtogroup Tools
/// @{

/// @brief Determines whether two values are considered equivalent in sorting
/// @details Equal implies equivalent, but not vice versa. Two values are considered
///  equivalent if neither appears before the other (w.r.t. sorting criterion, here operator<)
template <typename T>
bool equivalent(const T& lhs, const T& rhs)
{
	return !(lhs < rhs) && !(rhs < lhs);
}

/// @brief Binary search for value in iterator range
/// @details Performs a binary search with a useful return value, in contrast to std::binary_search().
/// @param first,last Iterator range
/// @param value Value to search for
/// @return Either iterator to first found element, or end-iterator of sequence
template <typename ForwardIterator, typename T>
ForwardIterator binarySearch(ForwardIterator first, ForwardIterator last, const T& value)
{
	// Note: std::lower_bound() has O(log n) on random access iterators
	ForwardIterator result = std::lower_bound(first, last, value);

	// std::lower_bound() returns iterator to first element >= value, which can be inequal to value
	if (result == last || !equivalent(*result, value))
		return last;
	else
		return result;
}

/// @brief Erase element using swap-and-pop_back idiom
/// @details Erases the element at position @c itr by swapping it with the last element in the container
///  and erasing the last element. This O(1) operation is particularly useful for std::vector or std::deque
///  as long as the element order is not relevant.
template <typename Container, typename Iterator>
void eraseUnordered(Container& c, Iterator itr)
{
	adlSwap(*itr, c.back());
	c.pop_back();
}

/// @brief Erase-remove idiom
/// @details Removes value @c v from sequential container @c c (std::vector or std::deque)
template <typename Container, typename Value>
void remove(Container& c, const Value& v)
{
	auto begin = std::remove(c.begin(), c.end(), v);
	c.erase(begin, c.end());
}

/// @brief Erase-remove-if idiom
/// @details Removes value @c v from sequential container @c c (std::vector or std::deque)
template <typename Container, typename Predicate>
void removeIf(Container& c, const Predicate& p)
{
	auto begin = std::remove_if(c.begin(), c.end(), p);
	c.erase(begin, c.end());
}

/// @brief Pop from queue with return value
/// @details Combines std::queue's %pop() and front() operations.
template <typename Queue>
typename Queue::value_type pop(Queue& q)
{
	auto value = std::move(q.front());
	q.pop();
	return value;
}

/// @brief Clear std::queue
/// @details Calls pop() repeatedly until the queue is empty.
template <typename Queue>
void clearQueue(Queue& q)
{
	while (!q.empty())
		q.pop();
}

/// @brief Returns the value type for a specific key.
/// @details Assumes existence of the key, returns corresponding mapped type without inserting it.
///  In contrast to <tt>std::[unordered_]map::at()</tt>, this does not throw exceptions. If the key
///  is not available, the behavior is undefined, in favor of optimized performance.
template <typename AssocContainer, typename Key>
typename AssocContainer::mapped_type& mapAt(AssocContainer& map, const Key& k)
{
	auto itr = map.find(k);
	assert(itr != map.end());
	return itr->second;
}

// const overload
template <typename AssocContainer, typename Key>
const typename AssocContainer::mapped_type& mapAt(const AssocContainer& map, const Key& k)
{
	auto itr = map.find(k);
	assert(itr != map.end());
	return itr->second;
}

/// @}

} // namespace aurora

#endif // AURORA_ALGORITHMS_HPP
