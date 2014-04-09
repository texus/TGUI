/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2014 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <algorithm>
#include <cctype>
#include <functional>

#include <TGUI/ConfigFile.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ConfigFile::ConfigFile(const std::string& filename, const std::string& section) :
        m_filename(filename),
        m_section (section)
    {
        std::ifstream file(filename.c_str());

        if (!file.is_open())
            throw Exception("Failed to open config file '" + filename + "'.");

        bool sectionFound = false;
        unsigned int lineNumber = 0;
        std::string lowercaseSection = toLower(section);

        // Stop reading when we reach the end of the file
        while (!file.eof())
        {
            // Get the next line
            std::string line;
            std::getline(file, line);
            lineNumber++;

            std::string::const_iterator c = line.begin();

            // Check if we are reading a section
            std::string sectionName;
            if (isSection(line, c, sectionName))
            {
                // If we already found our section then this would be the next section
                if (sectionFound)
                    break;

                // If this is the section we were looking for then start reading the properties
                if ((lowercaseSection + ":") == toLower(sectionName))
                    sectionFound = true;
            }
            else // This isn't a section
            {
                // We are only interested in one section
                if (!sectionFound)
                    continue;

                if (!removeWhitespace(line, c))
                    continue; // empty line

                // Read the property in lowercase
                std::string property = toLower(readWord(line, c));

                if (!removeWhitespace(line, c))
                    throw Exception("Failed to parse line " + tgui::to_string(lineNumber) + " in section " + section + " in file " + filename + ".");

                // There has to be an assignment character
                if (*c == '=')
                    ++c;
                else
                    throw Exception("Failed to parse line " + tgui::to_string(lineNumber) + " in section " + section + " in file " + filename + ".");

                if (!removeWhitespace(line, c))
                    throw Exception("Failed to parse line " + tgui::to_string(lineNumber) + " in section " + section + " in file " + filename + ".");

                int pos = c - line.begin();
                std::string value = line.substr(pos, line.length() - pos);

                m_properties.push_back(std::make_pair(property, value));
            }
        }

        // Throw an exception when the section wasn't found
        if (!sectionFound)
            throw Exception("Section '" + section + "' was not found in " + filename + ".");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<std::pair<std::string, std::string>>& ConfigFile::getProperties() const
    {
        return m_properties;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ConfigFile::readBool(std::vector<std::pair<std::string, std::string>>::const_iterator it) const
    {
        if ((it->second == "true") || (it->second == "True") || (it->second == "TRUE") || (it->second == "1"))
            return true;
        else if ((it->second == "false") || (it->second == "False") || (it->second == "FALSE") || (it->second == "0"))
            return false;
        else
            throw Exception("Failed to parse property " + it->first + " in section " + m_section + " in file " + m_filename + ".");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Color ConfigFile::readColor(std::vector<std::pair<std::string, std::string>>::const_iterator it) const
    {
        try
        {
            return extractColor(it->second);
        }
        catch (const Exception& e)
        {
            throw Exception("Failed to read the color value from property " + it->first + " in section " + m_section + " in file " + m_filename);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ConfigFile::readTexture(std::vector<std::pair<std::string, std::string>>::const_iterator it, const std::string& rootPath, Texture& texture) const
    {
        std::string::const_iterator c = it->second.begin();

        // Remove all whitespaces (string should still contains something)
        if (!removeWhitespace(it->second, c))
            throw Exception("Failed to parse property " + it->first + " in section " + m_section + " in file " + m_filename + ". Value is empty.");

        // There has to be a quote
        if (*c == '"')
            ++c;
        else
        {
            throw Exception("Failed to parse property " + it->first + " in section " + m_section + " in file " + m_filename
                            + ". Expected an opening quote for the filename.");
        }

        std::string filename;
        char prev = '\0';

        // Look for the end quote
        bool filenameFound = false;
        while (c != it->second.end())
        {
            if ((*c != '"') || (prev == '\\'))
            {
                prev = *c;
                filename.push_back(*c);
                ++c;
            }
            else
            {
                ++c;
                filenameFound = true;
                break;
            }
        }

        if (!filenameFound)
        {
            throw Exception("Failed to parse property " + it->first + " in section " + m_section + " in file " + m_filename
                            + ". Failed to find the closing quote of the filename.");
        }

        // There may be optional parameters
        sf::IntRect partRect;
        sf::IntRect middleRect;
        bool repeat = false;

        while (removeWhitespace(it->second, c))
        {
            std::string word;
            auto openingBracketPos = it->second.find('(', c - it->second.begin());
            if (openingBracketPos != std::string::npos)
                word = it->second.substr(c - it->second.begin(), openingBracketPos - (c - it->second.begin()));
            else
                word = it->second.substr(c - it->second.begin(), it->second.length() - (c - it->second.begin()));

            if ((word == "Stretch") || (word == "stretch"))
            {
                repeat = false;
                std::advance(c, 7);
            }
            else if ((word == "Repeat") || (word == "repeat"))
            {
                repeat = true;
                std::advance(c, 6);
            }
            else
            {
                sf::IntRect* rect = nullptr;

                if ((word == "Part") || (word == "part"))
                {
                    rect = &partRect;
                    std::advance(c, 4);
                }
                else if ((word == "Middle") || (word == "middle"))
                {
                    rect = &middleRect;
                    std::advance(c, 6);
                }
                else
                {
                    throw Exception("Failed to parse property " + it->first + " in section " + m_section + " in file " + m_filename
                                    + ". Unexpected word '" + word + "' in front of opening bracket.");
                }

                auto closeBracketPos = it->second.find(')', c - it->second.begin());
                if (closeBracketPos != std::string::npos)
                {
                    if (!readIntRect(it->second.substr(c - it->second.begin(), closeBracketPos - (c - it->second.begin()) + 1), *rect))
                        throw Exception("Failed to parse " + word + " rectangle for property " + it->first + " in section " + m_section + " in file " + m_filename + ".");
                }
                else
                {
                    throw Exception("Failed to parse property " + it->first + " in section " + m_section + " in file " + m_filename
                                    + ". Failed to find closing bracket for " + word + " rectangle.");
                }

                std::advance(c, closeBracketPos - (c - it->second.begin()) + 1);
            }
        }

        // Load the texture
        TGUI_TextureManager.getTexture(texture, rootPath + filename, partRect, middleRect, repeat);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ConfigFile::readIntRect(std::string value, sf::IntRect& rect) const
    {
        // Make sure that the line isn't empty
        if (value.empty() == false)
        {
            // The first and last character have to be brackets
            if ((value[0] == '(') && (value[value.length()-1] == ')'))
            {
                // Remove the brackets
                value.erase(0, 1);
                value.erase(value.length()-1);

                // Search for the first comma
                std::string::size_type commaPos = value.find(',');
                if (commaPos != std::string::npos)
                {
                    // Get the left value and delete this part of the string
                    rect.left = tgui::stoi(value.substr(0, commaPos));
                    value.erase(0, commaPos+1);

                    // Search for the second comma
                    commaPos = value.find(',');
                    if (commaPos != std::string::npos)
                    {
                        // Get the top value and delete this part of the string
                        rect.top = tgui::stoi(value.substr(0, commaPos));
                        value.erase(0, commaPos+1);

                        // Search for the third comma
                        commaPos = value.find(',');
                        if (commaPos != std::string::npos)
                        {
                            // Get the width value and delete this part of the string
                            rect.width = tgui::stoi(value.substr(0, commaPos));
                            value.erase(0, commaPos+1);

                            // Get the height value
                            rect.height = tgui::stoi(value);

                            return true;
                        }
                    }
                }
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ConfigFile::removeWhitespace(const std::string& line, std::string::const_iterator& c) const
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

    std::string ConfigFile::readWord(const std::string& line, std::string::const_iterator& c) const
    {
        std::string word;
        while (c != line.end())
        {
            if ((*c != ' ') && (*c != '\t') && (*c != '\r'))
            {
                word.push_back(*c);
                ++c;
            }
            else
                return word;
        }

        return word;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ConfigFile::isSection(const std::string& line, std::string::const_iterator c, std::string& sectionName) const
    {
        if (!removeWhitespace(line, c))
            return false;

        sectionName = readWord(line, c);

        removeWhitespace(line, c);

        if (c != line.end())
            return false;

        if (sectionName[sectionName.length()-1] == ':')
            return true;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
