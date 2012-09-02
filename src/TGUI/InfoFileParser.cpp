/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (VDV_B@hotmail.com)
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


#include <TGUI/TGUI.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    InfoFileParser::~InfoFileParser()
    {
        // If a file is still open then close it
        if (m_File.is_open())
            m_File.close();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool InfoFileParser::openFile(const std::string filename)
    {
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

    bool InfoFileParser::readProperty(std::string& property, std::string& value)
    {
      readNextLine:

        // Stop reading when we reach the end of the file
        if (!m_File.eof())
        {
            // Get the next line
            std::string line;
            std::getline(m_File, line);

            // Remove all spaces and tabs from the line
            line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
            line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());

            // Search for comments
            std::string::size_type commentPos = line.find('#');

            // Erase the comment (if there is one)
            if (commentPos != std::string::npos)
                line.erase(commentPos);

            // Only continue when the line is not empty
            if (line.empty())
                goto readNextLine;

            // Convert the whole line to lowercase
            unsigned int len = line.length();
            for (unsigned int i = 0; i < len; i++)
            {
                if ((line[i] > 64) && (line[i] < 91))
                    line[i] += 32;
            }

            // Search for the equal mark in the line
            std::string::size_type equalMarkPos = line.find('=', 0);

            // Only continue when it was found
            if (equalMarkPos != std::string::npos)
            {
                // Split the line in two parts
                property = line.substr(0, equalMarkPos);
                value = line.substr(equalMarkPos + 1);
            }
            else // The line is corrupted -> read the next one
                goto readNextLine;
        }
        else // The end was reached
            return false;

        // The line has been read
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void InfoFileParser::closeFile()
    {
        // Close the file (if it is open)
        if (m_File.is_open())
            m_File.close();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
