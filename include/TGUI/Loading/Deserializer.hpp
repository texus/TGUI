/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2019 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef TGUI_DESERIALIZER_HPP
#define TGUI_DESERIALIZER_HPP


#include <TGUI/ObjectConverter.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Deserializes a settable property
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API Deserializer
    {
    public:
        using DeserializeFunc = std::function<ObjectConverter(const std::string&)>;

        static ObjectConverter deserialize(ObjectConverter::Type type, const std::string& serializedString);

        static void setFunction(ObjectConverter::Type type, const DeserializeFunc& deserializer);
        static const DeserializeFunc& getFunction(ObjectConverter::Type type);

    public:
        static std::vector<std::string> split(const std::string& str, char delim);

    private:
        static std::map<ObjectConverter::Type, DeserializeFunc> m_deserializers;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_DESERIALIZER_HPP
