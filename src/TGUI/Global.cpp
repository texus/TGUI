/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Global.hpp>
#include <TGUI/Clipboard.hpp>
#include <TGUI/Texture.hpp>
#include <TGUI/Loading/Deserializer.hpp>

#include <functional>
#include <cctype>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    Clipboard TGUI_Clipboard;

    bool TGUI_TabKeyUsageEnabled = true;

    std::string TGUI_ResourcePath = "";

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void enableTabKeyUsage()
    {
        TGUI_TabKeyUsageEnabled = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void disableTabKeyUsage()
    {
        TGUI_TabKeyUsageEnabled = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void setResourcePath(const std::string& path)
    {
        TGUI_ResourcePath = path;

        if (!TGUI_ResourcePath.empty())
        {
            if (TGUI_ResourcePath[TGUI_ResourcePath.length()-1] != '/')
                TGUI_ResourcePath.push_back('/');
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& getResourcePath()
    {
        return TGUI_ResourcePath;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool compareFloats(float x, float y)
    {
        return (std::abs(x - y) < 0.0000001f);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool isWhitespace(char character)
    {
        if (character == ' ' || character == '\t' || character == '\r' || character == '\n')
            return true;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int stoi(const std::string& value)
    {
        int result = 0;

        std::istringstream iss(value);
        iss.imbue(std::locale::classic());
        iss >> result;

        if (iss.fail())
            result = 0;

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float stof(const std::string& value)
    {
        float result = 0;

        std::istringstream iss(value);
        iss.imbue(std::locale::classic());
        iss >> result;

        if (iss.fail())
            result = 0;

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool extractBoolFromString(const std::string& property, const std::string& value)
    {
        if ((value == "true") || (value == "True") || (value == "TRUE") || (value == "1"))
            return true;
        else if ((value == "false") || (value == "False") || (value == "FALSE") || (value == "0"))
            return false;
        else
            throw Exception{"Failed to parse boolean value of property '" + property + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool removeWhitespace(const std::string& line, std::string::const_iterator& c)
    {
        while (c != line.end())
        {
            if ((*c == ' ') || (*c == '\t') || (*c == '\r'))
                ++c;
            else
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string toLower(std::string str)
    {
        for (std::string::iterator i = str.begin(); i != str.end(); ++i)
            *i = static_cast<char>(std::tolower(*i));

        return str;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string trim(std::string str)
    {
        str.erase(str.begin(), std::find_if(str.begin(), str.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), str.end());
        return str;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::string> split(const std::string& str, char delim)
    {
        std::vector<std::string> tokens;

        std::size_t start = 0;
        std::size_t end = 0;
        while ((end = str.find(delim, start)) != std::string::npos) {
            tokens.push_back(str.substr(start, end - start));
            start = end + 1;
        }

        tokens.push_back(str.substr(start));
        return tokens;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Color calcColorOpacity(const sf::Color& color, float alpha)
    {
        if (alpha == 1)
            return color;
        else
            return {color.r, color.g, color.b, static_cast<sf::Uint8>(color.a * alpha)};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float getTextVerticalCorrection(const std::shared_ptr<sf::Font>& font, unsigned int characterSize, sf::Uint32 style)
    {
        if (!font)
            return 0;

        bool bold = (style & sf::Text::Bold) != 0;

        // Calculate the height of the first line (char size = everything above baseline, height + top = part below baseline)
        float lineHeight = characterSize
                           + font->getGlyph('g', characterSize, bold).bounds.height
                           + font->getGlyph('g', characterSize, bold).bounds.top;

        // Get the line spacing sfml returns
        float lineSpacing = font->getLineSpacing(characterSize);

        // Calculate the offset of the text
        return lineHeight - lineSpacing;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int findBestTextSize(const std::shared_ptr<sf::Font>& font, float height, int fit)
    {
        if (!font)
            return 0;

        if (height < 2)
            return 1;

        std::vector<unsigned int> textSizes(static_cast<std::size_t>(height));
        for (unsigned int i = 0; i < static_cast<unsigned int>(height); ++i)
            textSizes[i] = i + 1;

        auto high = std::lower_bound(textSizes.begin(), textSizes.end(), height, [&font](unsigned int charSize, float h){ return font->getLineSpacing(charSize) < h; });
        if (high == textSizes.end())
            return static_cast<unsigned int>(height);

        float highLineSpacing = font->getLineSpacing(*high);
        if (highLineSpacing == height)
            return *high;

        auto low = high - 1;
        float lowLineSpacing = font->getLineSpacing(*low);

        if (fit < 0)
            return *low;
        else if (fit > 0)
            return *high;
        else
        {
            if (std::abs(height - lowLineSpacing) < std::abs(height - highLineSpacing))
                return *low;
            else
                return *high;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
