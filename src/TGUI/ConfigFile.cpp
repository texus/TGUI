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

// Avoid "decorated name length exceeded, name was truncated" warnings in visual studio
#if defined(SFML_SYSTEM_WINDOWS) && defined(_MSC_VER)
    #pragma warning(disable: 4503)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    std::map<std::string, std::map<std::string, std::pair<std::vector<std::string>, std::vector<std::string>>>> ConfigFile::m_Cache;

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

        // The file may be cached
        if (m_Cache.find(filename) != m_Cache.end())
            return true;

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
        // Only parse the file once
        if (m_Cache.find(m_Filename) == m_Cache.end())
        {
            m_Cache[m_Filename] = std::map<std::string, std::pair<std::vector<std::string>, std::vector<std::string>>>();
            if (!readAndCache())
                return false;
        }

        // Output an error when the section wasn't found
        std::string lowercaseSection = toLower(section);
        if (m_Cache[m_Filename].find(lowercaseSection) == m_Cache[m_Filename].end())
        {
            TGUI_OUTPUT("TGUI error: Section '" + section + "' was not found in the config file '" + m_Filename + "'.");
            return false;
        }

        // Fill in properties and values
        properties = m_Cache[m_Filename][lowercaseSection].first;
        values = m_Cache[m_Filename][lowercaseSection].second;
        return true;
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

    void ConfigFile::flushCache()
    {
        m_Cache.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ConfigFile::readAndCache()
    {
        bool error = false;
        unsigned int lineNumber = 0;
        std::string sectionName;

        // Stop reading when we reach the end of the file
        while (!m_File.eof())
        {
            // Get the next line
            std::string line;
            std::getline(m_File, line);
            std::string::const_iterator c = line.begin();
            lineNumber++;

            // If the lines contains a '\r' at the end then remove it
            if (!line.empty() && line[line.size()-1] == '\r')
                line.erase(line.size()-1);

            // Skip empty lines
            if (line.empty() || !removeWhitespace(line, c))
                continue;

            if (!isSection(line, c, sectionName))
            {
                std::string property;
                std::string value;
                bool lineError = false;

                // Read the property in lowercase
                property = readWord(line, c);
                property = toLower(property);

                if (!removeWhitespace(line, c))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse line " + to_string(lineNumber) + ".");
                    lineError = true;
                }

                // There has to be an assignment character
                if (*c == '=')
                    ++c;
                else
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse line " + to_string(lineNumber) + ".");
                    lineError = true;
                }

                if (!removeWhitespace(line, c))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse line " + to_string(lineNumber) + ".");
                    lineError = true;
                }

                if (!lineError)
                {
                    int pos = c - line.begin();
                    value = line.substr(pos, line.length() - pos);

                    m_Cache[m_Filename][sectionName].first.push_back(property);
                    m_Cache[m_Filename][sectionName].second.push_back(value);
                }

                error |= lineError;
            }
        }

        return !error;
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

        std::string name = readWord(line, c);

        removeWhitespace(line, c);

        if (c != line.end())
            return false;

        if (name[name.length()-1] == ':')
        {
            sectionName = toLower(name.substr(0, name.length()-1));
            return true;
        }
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
