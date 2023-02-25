/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_SERIALIZER_HPP
#define TGUI_SERIALIZER_HPP


#include <TGUI/ObjectConverter.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <map>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Serializes an settable property
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API Serializer
    {
    public:
        using SerializeFunc = std::function<String(ObjectConverter&&)>;

        TGUI_NODISCARD static String serialize(ObjectConverter&& object);

        static void setFunction(ObjectConverter::Type type, const SerializeFunc& serializer);
        TGUI_NODISCARD static const SerializeFunc& getFunction(ObjectConverter::Type type);

    private:
        static std::map<ObjectConverter::Type, SerializeFunc> m_serializers;  /// We can't use unordered_map with enum class in GCC < 6
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_SERIALIZER_HPP
