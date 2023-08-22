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


#ifndef TGUI_COLOR_HPP
#define TGUI_COLOR_HPP

#include <TGUI/String.hpp>
#include <TGUI/Vertex.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cstdint>
    #include <string>
    #include <map>
#endif

#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS && !TGUI_DISABLE_SFML_CONVERSIONS
    #include <SFML/Graphics/Color.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    class Color;
}

namespace tgui
{
    namespace priv
    {
        TGUI_API Color constructColorFromString(const String& string);
    }
}

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Wrapper for colors
    ///
    /// The class is used for 2 purposes:
    /// - Implicit converter for parameters. A function taking a Color as parameter can be given an sf::Color, RGB values or
    ///   even a serialized string as argument.
    /// - Storing no color at all. Some colors may be optionally set and can thus remain unspecified.
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if TGUI_COMPILED_WITH_CPP_VER >= 17
    class Color
#else
    class TGUI_API Color
#endif
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the color with its alpha channel multiplied with the alpha parameter
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD static constexpr Color applyOpacity(const Color& color, float alpha)
        {
            return {color.getRed(), color.getGreen(), color.getBlue(), static_cast<std::uint8_t>(color.getAlpha() * alpha)};
        }


    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates the object without a color
        ///
        /// The isSet() function will return false when the object was created using this constructor.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        constexpr Color() :
            m_isSet{false},
            m_red  {0},
            m_green{0},
            m_blue {0},
            m_alpha{0}
        {
        }

#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS && !TGUI_DISABLE_SFML_CONVERSIONS
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates the object from an sf::Color
        ///
        /// @param color  Color to set
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        constexpr Color(const sf::Color& color) :
            m_isSet{true},
            m_red  {color.r},
            m_green{color.g},
            m_blue {color.b},
            m_alpha{color.a}
        {
        }
#endif

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates the object from an the RGB or RGBA values
        ///
        /// @param red   Red component
        /// @param green Green component
        /// @param blue  Blue component
        /// @param alpha Alpha component
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        constexpr Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha = 255) :
            m_isSet{true},
            m_red  {red},
            m_green{green},
            m_blue {blue},
            m_alpha{alpha}
        {
        }


#if defined (_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable: 26495) // Ignore "Variable is uninitialized" warning to surpress incorrect code analysis
#endif
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates the object from a string
        ///
        /// @param string  String to be deserialized as color
        ///
        /// The Deserializer class is used to convert the string into a color.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Color(const String& string) :
            Color{priv::constructColorFromString(string)}
        {
        }
#if defined (_MSC_VER)
#   pragma warning(pop)
#endif

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates the object from a string
        ///
        /// @param string  String to be deserialized as color
        ///
        /// The Deserializer class is used to convert the string into a color.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Color(const char* string) :
            Color{String{string}}
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Checks if a color was set
        ///
        /// @return True if a color was passed to the constructor, false when the default constructor was used
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD constexpr bool isSet() const
        {
            return m_isSet;
        }

#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS && !TGUI_DISABLE_SFML_CONVERSIONS
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Converts this object into an sf::Color object
        ///
        /// @return The color stored in this object, or the default color if no color was set
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        operator sf::Color() const
        {
            return {m_red, m_green, m_blue, m_alpha};
        }
#endif

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Converts this object into a Vertex::Color object
        ///
        /// @return The color stored in this object, or the default color if no color was set
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        explicit operator Vertex::Color() const
        {
            return Vertex::Color{m_red, m_green, m_blue, m_alpha};
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the red component of the color
        ///
        /// @return Red component of the stored color or the one from the default color is no color was set
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD constexpr std::uint8_t getRed() const
        {
            return m_red;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the green component of the color
        ///
        /// @return Green component of the stored color or the one from the default color is no color was set
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD constexpr std::uint8_t getGreen() const
        {
            return m_green;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the blue component of the color
        ///
        /// @return Blue component of the stored color or the one from the default color is no color was set
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD constexpr std::uint8_t getBlue() const
        {
            return m_blue;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the alpha component of the color
        ///
        /// @return Alpha component of the stored color or the one from the default color is no color was set
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD constexpr std::uint8_t getAlpha() const
        {
            return m_alpha;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Compares the color with another one
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD constexpr bool operator==(const Color& rhs) const
        {
            return (m_isSet == rhs.m_isSet)
                && (m_red == rhs.m_red)
                && (m_green == rhs.m_green)
                && (m_blue == rhs.m_blue)
                && (m_alpha == rhs.m_alpha);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Compares the color with another one
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD constexpr bool operator!=(const Color& right) const
        {
            return !(*this == right);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:

        static const Color Black;       //!< Black predefined color
        static const Color White;       //!< White predefined color
        static const Color Red;         //!< Red predefined color
        static const Color Green;       //!< Green predefined color
        static const Color Blue;        //!< Blue predefined color
        static const Color Yellow;      //!< Yellow predefined color
        static const Color Magenta;     //!< Magenta predefined color
        static const Color Cyan;        //!< Cyan predefined color
        static const Color Transparent; //!< Transparent (black) predefined color

        static const std::array<std::pair<StringView, Color>, 9> colorNamesMap;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        bool m_isSet = false;
        std::uint8_t m_red = 0;
        std::uint8_t m_green = 0;
        std::uint8_t m_blue = 0;
        std::uint8_t m_alpha = 0;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#if TGUI_COMPILED_WITH_CPP_VER >= 17
    inline constexpr const Color Color::Black      {  0,   0,   0};
    inline constexpr const Color Color::White      {255, 255, 255};
    inline constexpr const Color Color::Red        {255,   0,   0};
    inline constexpr const Color Color::Green      {  0, 255,   0};
    inline constexpr const Color Color::Blue       {  0,   0, 255};
    inline constexpr const Color Color::Yellow     {255, 255,   0};
    inline constexpr const Color Color::Magenta    {255,   0, 255};
    inline constexpr const Color Color::Cyan       {  0, 255, 255};
    inline constexpr const Color Color::Transparent{  0,   0,   0,   0};

    inline constexpr const std::array<std::pair<StringView, Color>, 9> Color::colorNamesMap
    {
        {{U"black"sv, Color::Black},
         {U"white"sv, Color::White},
         {U"red"sv, Color::Red},
         {U"yellow"sv, Color::Yellow},
         {U"green"sv, Color::Green},
         {U"cyan"sv, Color::Cyan},
         {U"blue"sv, Color::Blue},
         {U"magenta"sv, Color::Magenta},
         {U"transparent"sv, Color::Transparent}}
    };
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace priv
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Construct a color object from a string
        ///
        /// @param string  String to be deserialized as color
        ///
        /// @return Color described by the string
        ///
        /// This is called internally when passing a string to the constructor of the Color class.
        /// This function will simply use Deserializer::deserialize internally, but we don't call that function directly
        /// to avoid including that class here.
        ///
        /// Note that having this function allows keeping the Color class outside of a DLL, which allows the color constants
        /// to be defined as "inline constexpr".
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD TGUI_API Color constructColorFromString(const String& string);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_COLOR_HPP
