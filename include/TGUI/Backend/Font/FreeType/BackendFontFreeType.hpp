/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2021 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_BACKEND_FONT_FREETYPE_HPP
#define TGUI_BACKEND_FONT_FREETYPE_HPP

#include <TGUI/Backend/Font/BackendFont.hpp>
#include <unordered_map>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct FT_LibraryRec_* FT_Library;
typedef struct FT_FaceRec_*    FT_Face;
typedef struct FT_StrokerRec_* FT_Stroker;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Font implementations that uses FreeType directly to load glyphs
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class BackendFontFreetype : public BackendFont
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Destructor that cleans up the FreeType resources
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ~BackendFontFreetype();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Loads a font from memory
        ///
        /// @param data         Moved pointer to the file data in memory
        /// @param sizeInBytes  Size of the data to load, in bytes
        ///
        /// @return True if the font was loaded successfully, false otherwise
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool loadFromMemory(std::unique_ptr<std::uint8_t[]> data, std::size_t sizeInBytes) override;
        using BackendFont::loadFromMemory;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns whether a font contains a certain glyph
        ///
        /// @param codePoint  Character to check
        ///
        /// @return Does the font contain this character?
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool hasGlyph(char32_t codePoint) const override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Retrieve a glyph of the font
        ///
        /// If the font is a bitmap font, not all character sizes might be available. If the glyph is not available at the
        /// requested size, an empty glyph is returned.
        ///
        /// @param codePoint        Unicode code point of the character to get
        /// @param characterSize    Reference character size
        /// @param bold             Retrieve the bold version or the regular one?
        /// @param outlineThickness Thickness of outline (when != 0 the glyph will not be filled)
        ///
        /// @return The glyph corresponding to codePoint and characterSize
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual FontGlyph getGlyph(char32_t codePoint, unsigned int characterSize, bool bold, float outlineThickness = 0) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the kerning offset of two glyphs
        ///
        /// The kerning is an extra offset (negative) to apply between two glyphs when rendering them, to make the pair look
        /// more "natural". For example, the pair "AV" have a special kerning to make them closer than other characters.
        /// Most of the glyphs pairs have a kerning offset of zero, though.
        ///
        /// @param first         Unicode code point of the first character
        /// @param second        Unicode code point of the second character
        /// @param characterSize Size of the characters
        /// @param bold          Are the glyphs bold or regular?
        ///
        /// @return Kerning value for first and second, in pixels
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual float getKerning(char32_t first, char32_t second, unsigned int characterSize, bool bold) override;
#ifndef TGUI_REMOVE_DEPRECATED_CODE
        using BackendFont::getKerning; // Import version without bold parameter
#endif

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the line spacing
        ///
        /// Line spacing is the vertical offset to apply between two consecutive lines of text.
        ///
        /// @param characterSize Size of the characters
        ///
        /// @return Line spacing, in pixels
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual float getLineSpacing(unsigned int characterSize) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Get the position of the underline
        ///
        /// Underline position is the vertical offset to apply between the baseline and the underline.
        ///
        /// @param characterSize  Reference character size
        ///
        /// @return Underline position, in pixels
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        float getUnderlinePosition(unsigned int characterSize) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Get the thickness of the underline
        ///
        /// Underline thickness is the vertical size of the underline.
        ///
        /// @param characterSize  Reference character size
        ///
        /// @return Underline thickness, in pixels
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        float getUnderlineThickness(unsigned int characterSize) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the texture that is used to store glyphs of the given character size
        ///
        /// @param characterSize  Size of the characters that should be part of the texture
        ///
        /// @return Texture to render text glyphs with
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::shared_ptr<BackendTexture> getTexture(unsigned int characterSize) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Enable or disable the smooth filter
        ///
        /// When the filter is activated, the font appears smoother so that pixels are less noticeable. However if you want
        /// the font to look exactly the same as its source file then you should disable it.
        /// The smooth filter is enabled by default.
        ///
        /// @param smooth  True to enable smoothing, false to disable it
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setSmooth(bool smooth) override;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        struct Glyph
        {
            float     advance = 0;   //!< Offset to move horizontally to the next character
            float     lsbDelta = 0;  //!< Left offset after forced autohint. Internally used by getKerning()
            float     rsbDelta = 0;  //!< Right offset after forced autohint. Internally used by getKerning()
            FloatRect bounds;        //!< Bounding rectangle of the glyph, in coordinates relative to the baseline
            IntRect   textureRect;   //!< Texture coordinates of the glyph inside the font's texture
        };


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Loads a glyph with freetype
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Glyph loadGlyph(char32_t codePoint, unsigned int characterSize, bool bold, float outlineThickness);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Returns a cached glyph or calls loadGlyph to load it when this is the first time the glyph is requested
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Glyph getInternalGlyph(char32_t codePoint, unsigned int characterSize, bool bold, float outlineThickness);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Reserves space in the texture to place the glyph
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        IntRect findAvailableGlyphRect(unsigned int width, unsigned int height);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Sets the character size on which the freetype operations are performed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool setCurrentSize(unsigned int characterSize);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Destroys freetype resources
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void cleanup();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        struct Row
        {
            Row(unsigned int rowTop, unsigned int rowHeight) : width(0), top(rowTop), height(rowHeight) {}

            unsigned int width;  //!< Current width of the row
            unsigned int top;    //!< Y position of the row into the texture
            unsigned int height; //!< Height of the row
        };

        FT_Library  m_library = nullptr;  // Handle to the freetype library
        FT_Face     m_face    = nullptr;  // Contains the font (typeface and style)
        FT_Stroker  m_stroker = nullptr;  // Used for rendering outlines

        std::unordered_map<unsigned int, float> m_cachedLineSpacing;

        std::unordered_map<std::uint64_t, Glyph> m_glyphs;
        unsigned int     m_nextRow = 3; //!< Y position of the next new row in the texture (first 2 rows contain pixels for underlining)
        std::vector<Row> m_rows;

        std::unique_ptr<std::uint8_t[]> m_fileContents;
        std::unique_ptr<std::uint8_t[]> m_pixels;
        std::shared_ptr<BackendTexture> m_texture;
        unsigned int m_textureSize = 0;
        bool m_textureUpToDate = false;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_BACKEND_FONT_FREETYPE_HPP
