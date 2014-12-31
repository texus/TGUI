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

#include <TGUI/ThemeFileParser.hpp>
#include <TGUI/Texture.hpp>
#include <TGUI/Global.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    std::map<std::pair<std::string, std::string>, std::vector<std::pair<std::string, std::string>>> ThemeFileParser::m_cache;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace
    {
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
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ThemeFileParser::ThemeFileParser(const std::string& filename, const std::string& section) :
        m_filename(filename),
        m_section (section)
    {
        // Don't read and parse the file every time
        if (!m_cache[{filename, section}].empty())
        {
            m_properties = m_cache[{filename, section}];
            return;
        }

        std::ifstream file{filename};

        if (!file.is_open())
            throw Exception{"Failed to open theme file '" + filename + "'."};

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
                    throw Exception{"Failed to parse line " + tgui::to_string(lineNumber) + " in section " + section + " in file " + filename + "."};

                // There has to be an assignment character
                if (*c == '=')
                    ++c;
                else
                    throw Exception{"Failed to parse line " + tgui::to_string(lineNumber) + " in section " + section + " in file " + filename + "."};

                if (!removeWhitespace(line, c))
                    throw Exception{"Failed to parse line " + tgui::to_string(lineNumber) + " in section " + section + " in file " + filename + "."};

                int pos = c - line.begin();
                std::string value = line.substr(pos, line.length() - pos);

                m_properties.push_back(std::make_pair(property, value));
            }
        }

        // Throw an exception when the section wasn't found
        if (!sectionFound)
            throw Exception{"Section '" + section + "' was not found in " + filename + "."};
        else
            m_cache[{filename, section}] = m_properties;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<std::pair<std::string, std::string>>& ThemeFileParser::getProperties() const
    {
        return m_properties;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string ThemeFileParser::readWord(const std::string& line, std::string::const_iterator& c) const
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

    bool ThemeFileParser::isSection(const std::string& line, std::string::const_iterator c, std::string& sectionName) const
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

    void ThemeFileParser::flushCache()
    {
        m_cache.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
