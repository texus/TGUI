/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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
    std::map<std::pair<std::string, std::string>, std::pair<std::vector<std::string>, std::vector<std::string>>> ConfigFile::m_Cache;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ConfigFile::~ConfigFile()
    {
        // If a file is still open then close it
        if (m_File.is_open())
            m_File.close();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ConfigFile::open(const std::string& filename)
    {
        m_Filename = filename;

        // If a file is already open then close it
        if (m_File.is_open())
            m_File.close();

        // Open the file
        m_File.open(filename.c_str(), std::ifstream::in);

        // Check if the file was opened
        if (m_File.is_open())
            return true;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ConfigFile::read(const std::string& section, std::vector<std::string>& properties, std::vector<std::string>& values)
    {
        // Don't read and parse the file every time
        if (!m_Cache[std::make_pair(m_Filename, section)].first.empty())
        {
            properties = m_Cache[std::make_pair(m_Filename, section)].first;
            values = m_Cache[std::make_pair(m_Filename, section)].second;
            return true;
        }

        std::string lowercaseSection = toLower(section);

        bool error = false;
        bool sectionFound = false;
        unsigned int lineNumber = 0;

        // Stop reading when we reach the end of the file
        while (!m_File.eof())
        {
            // Get the next line
            std::string line;
            std::getline(m_File, line);
            lineNumber++;

            if (line.empty())
                continue;

            // If the lines contains a '\r' at the end then remove it
            if (line[line.size()-1] == '\r')
                line.erase(line.size()-1);

            std::string::const_iterator c = line.begin();

            // Check if we are reading a section
            std::string sectionName;
            if (isSection(line, c, sectionName))
            {
                // If we already found our section then this would be the next section
                if (sectionFound)
                    break;

                // Convert the section names to lowercase in order to compare them
                sectionName = toLower(sectionName);

                // If this is the section we were looking for then start reading the properties
                if ((lowercaseSection + ":") == sectionName)
                    sectionFound = true;
            }
            else // This isn't a section
            {
                // We are only interested in one section
                if (!sectionFound)
                    continue;

                std::string property;
                std::string value;

                if (!removeWhitespace(line, c))
                    continue; // empty line

                // Read the property in lowercase
                property = readWord(line, c);
                property = toLower(property);

                if (!removeWhitespace(line, c))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse line " + to_string(lineNumber) + ".");
                    error = true;
                }

                // There has to be an assignment character
                if (*c == '=')
                    ++c;
                else
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse line " + to_string(lineNumber) + ".");
                    error = true;
                }

                if (!removeWhitespace(line, c))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse line " + to_string(lineNumber) + ".");
                    error = true;
                }

                int pos = c - line.begin();
                value = line.substr(pos, line.length() - pos);

                properties.push_back(property);
                values.push_back(value);
            }
        }

        // Output an error when the section wasn't found
        if (!sectionFound)
        {
            TGUI_OUTPUT("TGUI error: Section '" + section + "' was not found in the config file.");
            error = true;
        }

        if (!error)
        {
            m_Cache[std::make_pair(m_Filename, section)].first = properties;
            m_Cache[std::make_pair(m_Filename, section)].second = values;
        }

        return !error;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ConfigFile::readBool(const std::string& value, bool defaultValue) const
    {
        if ((value == "true") || (value == "True") || (value == "TRUE") || (value == "1"))
            return true;
        else if ((value == "false") || (value == "False") || (value == "FALSE") || (value == "0"))
            return false;
        else
            return defaultValue;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Color ConfigFile::readColor(const std::string& value) const
    {
        return extractColor(value);
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
                    rect.left = atoi(value.substr(0, commaPos).c_str());
                    value.erase(0, commaPos+1);

                    // Search for the second comma
                    commaPos = value.find(',');
                    if (commaPos != std::string::npos)
                    {
                        // Get the top value and delete this part of the string
                        rect.top = atoi(value.substr(0, commaPos).c_str());
                        value.erase(0, commaPos+1);

                        // Search for the third comma
                        commaPos = value.find(',');
                        if (commaPos != std::string::npos)
                        {
                            // Get the width value and delete this part of the string
                            rect.width = atoi(value.substr(0, commaPos).c_str());
                            value.erase(0, commaPos+1);

                            // Get the height value
                            rect.height = atoi(value.c_str());

                            return true;
                        }
                    }
                }
            }
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ConfigFile::readTexture(const std::string& value, const std::string& rootPath, Texture& texture) const
    {
        std::string::const_iterator c = value.begin();

        // Remove all whitespaces (string should still contains something)
        if (!removeWhitespace(value, c))
            return false;

        // There has to be a quote
        if (*c == '"')
            ++c;
        else
            return false;

        std::string filename;
        char prev = '\0';

        // Look for the end quote
        bool filenameFound = false;
        while (c != value.end())
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
            return false;

        // There may be an optional parameter
        sf::IntRect rect;
        if (removeWhitespace(value, c))
        {
            if (!readIntRect(value.substr(c - value.begin(), value.length() - (c - value.begin())), rect))
                return false;
        }

        // Load the texture
        return TGUI_TextureManager.getTexture(rootPath + filename, texture, rect);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ConfigFile::close()
    {
        // Close the file (if it is open)
        if (m_File.is_open())
            m_File.close();
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
