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
/// @brief Class aurora::NonCopyable

#ifndef AURORA_NONCOPYABLE_HPP
#define AURORA_NONCOPYABLE_HPP


namespace aurora
{

/// @addtogroup Tools
/// @{

/// @brief Non-copyable base class.
/// @details Copy constructor and copy assignment operator are not accessible.
///  Derive from this class to prevent copying of your class.
class NonCopyable
{
	// ---------------------------------------------------------------------------------------------------------------------------
	// Protected member functions
	protected:
		// Default constructor
									NonCopyable() {}
									~NonCopyable() {}

	// ---------------------------------------------------------------------------------------------------------------------------
	// Private member functions
	private:
		// Copy constructor (forbidden)
									NonCopyable(const NonCopyable& origin);

		// Assignment operator (forbidden)
		NonCopyable&				operator= (const NonCopyable& origin);
};

/// @}

} // namespace aurora

#endif // AURORA_NONCOPYABLE_HPP
