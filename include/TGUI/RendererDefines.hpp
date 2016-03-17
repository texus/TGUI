/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_RENDERER_DEFINES_HPP
#define TGUI_RENDERER_DEFINES_HPP


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TGUI_RENDERER_PROPERTY_GET_BORDERS(CLASS, NAME, DEFAULT) \
    Borders CLASS::get##NAME() const \
    { \
        auto it = m_data->propertyValuePairs.find(toLower(#NAME)); \
        if (it != m_data->propertyValuePairs.end()) \
            return it->second.getBorders(); \
        else \
        { \
            m_data->propertyValuePairs[toLower(#NAME)] = {DEFAULT}; \
            return m_data->propertyValuePairs[toLower(#NAME)].getBorders(); \
        } \
    }

#define TGUI_RENDERER_PROPERTY_BORDERS(CLASS, NAME, DEFAULT) \
    TGUI_RENDERER_PROPERTY_GET_BORDERS(CLASS, NAME, DEFAULT) \
    void CLASS::set##NAME(const Borders& borders) \
    { \
        setProperty(toLower(#NAME), {borders}); \
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TGUI_RENDERER_PROPERTY_GET_COLOR(CLASS, NAME, DEFAULT) \
    sf::Color CLASS::get##NAME() const \
    { \
        auto it = m_data->propertyValuePairs.find(toLower(#NAME)); \
        if (it != m_data->propertyValuePairs.end()) \
            return it->second.getColor(); \
        else \
        { \
            m_data->propertyValuePairs[toLower(#NAME)] = {DEFAULT}; \
            return m_data->propertyValuePairs[toLower(#NAME)].getColor(); \
        } \
    }

#define TGUI_RENDERER_PROPERTY_COLOR(CLASS, NAME, DEFAULT) \
    TGUI_RENDERER_PROPERTY_GET_COLOR(CLASS, NAME, DEFAULT) \
    void CLASS::set##NAME(const Color& color) \
    { \
        setProperty(toLower(#NAME), {color}); \
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TGUI_RENDERER_PROPERTY_GET_NUMBER(CLASS, NAME, DEFAULT) \
    float CLASS::get##NAME() const \
    { \
        auto it = m_data->propertyValuePairs.find(toLower(#NAME)); \
        if (it != m_data->propertyValuePairs.end()) \
            return it->second.getNumber(); \
        else \
        { \
            m_data->propertyValuePairs[toLower(#NAME)] = {DEFAULT}; \
            return m_data->propertyValuePairs[toLower(#NAME)].getNumber(); \
        } \
    }

#define TGUI_RENDERER_PROPERTY_NUMBER(CLASS, NAME, DEFAULT) \
    TGUI_RENDERER_PROPERTY_GET_NUMBER(CLASS, NAME, DEFAULT) \
    void CLASS::set##NAME(float number) \
    { \
        setProperty(toLower(#NAME), {number}); \
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TGUI_RENDERER_PROPERTY_GET_TEXTURE(CLASS, NAME) \
    Texture& CLASS::get##NAME() const \
    { \
        auto it = m_data->propertyValuePairs.find(toLower(#NAME)); \
        if (it != m_data->propertyValuePairs.end()) \
            return it->second.getTexture(); \
        else \
        { \
            m_data->propertyValuePairs[toLower(#NAME)] = {Texture{}}; \
            return m_data->propertyValuePairs[toLower(#NAME)].getTexture(); \
        } \
    }

#define TGUI_RENDERER_PROPERTY_TEXTURE(CLASS, NAME) \
    TGUI_RENDERER_PROPERTY_GET_TEXTURE(CLASS, NAME) \
    void CLASS::set##NAME(const Texture& texture) \
    { \
        setProperty(toLower(#NAME), {texture}); \
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_RENDERER_DEFINES_HPP
