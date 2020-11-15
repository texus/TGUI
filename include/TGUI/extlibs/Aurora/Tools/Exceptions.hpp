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
/// @brief Definition of Aurora exception classes

#ifndef AURORA_EXCEPTIONS_HPP
#define AURORA_EXCEPTIONS_HPP

#include <TGUI/extlibs/Aurora/Config.hpp>

#include <stdexcept>
#include <string>


namespace aurora
{

/// @addtogroup Tools
/// @{

// MSVC: Disable DLL warning because we derive from std::runtime_error
#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4275)
#endif

/// @brief %Exception base class.
/// @details All exceptions thrown by the Aurora Library are derived from this class. Exception inherits from
///  std::runtime_error, hence all Aurora exceptions contain a (here undocumented) virtual function what().
class Exception : public std::runtime_error
{
	// ---------------------------------------------------------------------------------------------------------------------------
	// Public member functions
	public:
		/// @brief Constructor
		/// @param message The exception message (how the error occurred).
		explicit Exception(const std::string& message)
		: std::runtime_error(message)
		{
		}
};


/// @brief %Exception class for failed function calls.
/// @details Is used inside aurora::SingleDispatcher and aurora::DoubleDispatcher.
class FunctionCallException : public Exception
{
	// ---------------------------------------------------------------------------------------------------------------------------
	// Public member functions
	public:
		/// @brief Constructor
		/// @param message The exception message (how the error occurred).
		explicit FunctionCallException(const std::string& message)
		: Exception(message)
		{
		}
};

#ifdef _MSC_VER
	#pragma warning(pop)
#endif

/// @}

} // namespace aurora

#endif // AURORA_EXCEPTIONS_HPP
