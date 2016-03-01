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

// Contains preprocessor metaprograms for code generation of smart pointer makeXY() factories

#ifndef AURORA_FACTORIES_HPP
#define AURORA_FACTORIES_HPP

#include <TGUI/Aurora/Meta/Preprocessor.hpp>


#define AURORA_DETAIL_TYPENAME(n) typename A ## n
#define AURORA_DETAIL_PARAMETER(n) A ## n && arg ## n
#define AURORA_DETAIL_FORWARD_ARG(n) std::forward<A ## n>(arg ## n)

#define AURORA_DETAIL_SMARTPTR_FACTORY(SmartPtr, factoryFunction, n)									\
	template <typename T AURORA_PP_COMMA_IF(n) AURORA_PP_ENUMERATE_COMMA(n, AURORA_DETAIL_TYPENAME)>	\
	SmartPtr<T> factoryFunction(AURORA_PP_ENUMERATE_COMMA(n, AURORA_DETAIL_PARAMETER))					\
	{																									\
		return SmartPtr<T>(new T( AURORA_PP_ENUMERATE_COMMA(n, AURORA_DETAIL_FORWARD_ARG) ));			\
	}

#endif // AURORA_FACTORIES_HPP
