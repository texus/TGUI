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


#ifndef TGUI_RENDERER_DEFINES_HPP
#define TGUI_RENDERER_DEFINES_HPP


#include <TGUI/Loading/Theme.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TGUI_RENDERER_PROPERTY_OUTLINE(CLASS, NAME) \
    Outline CLASS::get##NAME() const \
    { \
        const auto it = m_data->propertyValuePairs.find(toLower(#NAME)); \
        if (it != m_data->propertyValuePairs.end()) \
            return it->second.getOutline(); \
        else \
            return {}; \
    } \
    void CLASS::set##NAME(const Outline& outline) \
    { \
        setProperty(toLower(#NAME), {outline}); \
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TGUI_RENDERER_PROPERTY_COLOR(CLASS, NAME, DEFAULT) \
    Color CLASS::get##NAME() const \
    { \
        const auto it = m_data->propertyValuePairs.find(toLower(#NAME)); \
        if (it != m_data->propertyValuePairs.end()) \
            return it->second.getColor(); \
        else \
            return DEFAULT; \
    } \
    void CLASS::set##NAME(Color color) \
    { \
        setProperty(toLower(#NAME), {color}); \
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TGUI_RENDERER_PROPERTY_TEXT_STYLE(CLASS, NAME, DEFAULT) \
    TextStyle CLASS::get##NAME() const \
    { \
        const auto it = m_data->propertyValuePairs.find(toLower(#NAME)); \
        if (it != m_data->propertyValuePairs.end()) \
            return it->second.getTextStyle(); \
        else \
            return DEFAULT; \
    } \
    void CLASS::set##NAME(TextStyle style) \
    { \
        setProperty(toLower(#NAME), ObjectConverter{style}); \
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TGUI_RENDERER_PROPERTY_GET_NUMBER(CLASS, NAME, DEFAULT) \
    float CLASS::get##NAME() const \
    { \
        const auto it = m_data->propertyValuePairs.find(toLower(#NAME)); \
        if (it != m_data->propertyValuePairs.end()) \
            return it->second.getNumber(); \
        else \
            return DEFAULT; \
    }

#define TGUI_RENDERER_PROPERTY_NUMBER(CLASS, NAME, DEFAULT) \
    TGUI_RENDERER_PROPERTY_GET_NUMBER(CLASS, NAME, DEFAULT) \
    void CLASS::set##NAME(float number) \
    { \
        setProperty(toLower(#NAME), ObjectConverter{number}); \
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TGUI_RENDERER_PROPERTY_GET_BOOL(CLASS, NAME, DEFAULT) \
    bool CLASS::get##NAME() const \
    { \
        const auto it = m_data->propertyValuePairs.find(toLower(#NAME)); \
        if (it != m_data->propertyValuePairs.end()) \
            return it->second.getBool(); \
        else \
            return DEFAULT; \
    }

#define TGUI_RENDERER_PROPERTY_BOOL(CLASS, NAME, DEFAULT) \
    TGUI_RENDERER_PROPERTY_GET_BOOL(CLASS, NAME, DEFAULT) \
    void CLASS::set##NAME(bool flag) \
    { \
        setProperty(toLower(#NAME), ObjectConverter{flag}); \
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// TGUI_NEXT: const reference
#define TGUI_RENDERER_PROPERTY_TEXTURE(CLASS, NAME) \
    Texture& CLASS::get##NAME() const \
    { \
        const auto it = m_data->propertyValuePairs.find(toLower(#NAME)); \
        if (it != m_data->propertyValuePairs.end()) \
            return it->second.getTexture(); \
        else \
        { \
            m_data->propertyValuePairs[toLower(#NAME)] = {Texture{}}; \
            return m_data->propertyValuePairs[toLower(#NAME)].getTexture(); \
        } \
    } \
    void CLASS::set##NAME(const Texture& texture) \
    { \
        setProperty(toLower(#NAME), {texture}); \
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TGUI_RENDERER_PROPERTY_RENDERER(CLASS, NAME, RENDERER) \
    std::shared_ptr<RendererData> CLASS::get##NAME() const \
    { \
        const auto it = m_data->propertyValuePairs.find(toLower(#NAME)); \
        if (it != m_data->propertyValuePairs.end()) \
            return it->second.getRenderer(); \
        else \
        { \
            const auto& renderer = Theme::getDefault()->getRendererNoThrow(RENDERER); \
            m_data->propertyValuePairs[toLower(#NAME)] = {renderer ? renderer : RendererData::create()}; \
            return renderer; \
        } \
    } \
    void CLASS::set##NAME(std::shared_ptr<RendererData> renderer) \
    { \
        setProperty(toLower(#NAME), {renderer}); \
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_RENDERER_DEFINES_HPP
