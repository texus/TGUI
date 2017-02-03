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


#include <TGUI/Loading/DataIO.hpp>
#include <TGUI/Global.hpp>

#include <cctype>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<DataIO::Node> DataIO::parse(std::stringstream& stream)
    {
        auto root = std::make_shared<Node>();
        auto node = root;

        std::string error;
        while (error.empty())
        {
            stream >> std::ws;
            if (stream.peek() == EOF)
                break;

            if (stream.peek() == '/')
            {
                char c;
                stream.read(&c, 1);
                if (stream.peek() == '/')
                {
                    while (stream.peek() != EOF)
                    {
                        stream.read(&c, 1);
                        if (c == '\n')
                            break;
                    }
                }
                else if (stream.peek() == '*')
                {
                    while (stream.peek() != EOF)
                    {
                        stream.read(&c, 1);
                        if (stream.peek() == '*')
                        {
                            stream.read(&c, 1);
                            if (stream.peek() == '/')
                            {
                                stream.read(&c, 1);
                                break;
                            }
                        }
                    }
                    continue;
                }
                else
                    error = "Found '/' while trying to read new section.";

                continue;
            }

            std::string word = readWord(stream);
            if (word != "")
            {
                stream >> std::ws;
                if (stream.peek() == '{')
                    error = parseSection(stream, node, word);
                else if (stream.peek() == ':')
                    error = parseKeyValue(stream, node, word);
                else if (stream.peek() == '}')
                {
                    char c;
                    stream.read(&c, 1);
                }
                else if (stream.peek() == EOF)
                    error = "Found EOF while trying to read new section.";
                else
                    error = "Expected '{' or ':', found '" + std::string(1, stream.peek()) + "' instead.";
            }
            else // Something went wrong while reading the word
            {
                if (stream.peek() == EOF)
                    error = "Found EOF while trying to read new section.";
                else
                    error = "Expected section name, found '" + std::string(1, stream.peek()) + "' instead.";
            }
        }

        if (!error.empty()) {
            throw Exception{"Error while parsing input. " + error};
        }

        return root;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void DataIO::emit(std::shared_ptr<Node> rootNode, std::stringstream& stream)
    {
        for (auto& pair : rootNode->propertyValuePairs)
            stream << pair.first << ": " << pair.second->value << ";" << std::endl;

        if (rootNode->propertyValuePairs.size() > 0 && rootNode->children.size() > 0)
            stream << std::endl;

        std::vector<std::string> output;
        for (std::size_t i = 0; i < rootNode->children.size(); ++i)
        {
            for (auto& line : convertNodesToLines(rootNode->children[i]))
                output.emplace_back(std::move(line));

            if (i < rootNode->children.size()-1)
                output.emplace_back("");
        }

        for (auto& line : output)
            stream << line << std::endl;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::string> DataIO::convertNodesToLines(std::shared_ptr<Node> node)
    {
        std::vector<std::string> output;
        if (node->name.empty())
            output.emplace_back("{");
        else
            output.emplace_back(node->name + " {");

        if (node->propertyValuePairs.size())
        {
            for (auto& pair : node->propertyValuePairs)
                output.emplace_back("    " + pair.first + ": " + pair.second->value + ";");
        }

        if (node->propertyValuePairs.size() > 0 && node->children.size() > 0)
            output.emplace_back("");

        if (node->children.size())
        {
            for (std::size_t i = 0; i < node->children.size(); ++i)
            {
                for (auto& line : convertNodesToLines(node->children[i]))
                    output.emplace_back("    " + line);

                if (i < node->children.size() - 1)
                    output.emplace_back("");
            }
        }

        output.emplace_back("}");
        return output;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string DataIO::parseSection(std::stringstream& stream, std::shared_ptr<Node> node, const std::string& sectionName)
    {
        // Read the brace from the stream and remove the whitespace behind it
        char c;
        stream.read(&c, 1);
        stream >> std::ws;

        // Create a new node for this section
        auto sectionNode = std::make_shared<Node>();
        sectionNode->parent = node.get();
        sectionNode->name = sectionName;
        node->children.push_back(sectionNode);
        node = sectionNode;

        while (stream.peek() != EOF)
        {
            stream >> std::ws;
            if (stream.peek() == EOF)
                break;

            if (stream.peek() == '/')
            {
                stream.read(&c, 1);
                if (stream.peek() == '/')
                {
                    while (stream.peek() != EOF)
                    {
                        stream.read(&c, 1);
                        if (c == '\n')
                            break;
                    }
                }
                else if (stream.peek() == '*')
                {
                    while (stream.peek() != EOF)
                    {
                        stream.read(&c, 1);
                        if (stream.peek() == '*')
                        {
                            stream.read(&c, 1);
                            if (stream.peek() == '/')
                            {
                                stream.read(&c, 1);
                                break;
                            }
                        }
                    }
                    continue;
                }
                else
                    return "Found '/' while trying to read new section.";

                continue;
            }

            std::string word = readWord(stream);
            if (word != "")
            {
                stream >> std::ws;
                if (stream.peek() == '{')
                {
                    std::string error = parseSection(stream, node, word);
                    if (!error.empty())
                        return error;
                }
                else if (stream.peek() == ':')
                {
                    std::string error = parseKeyValue(stream, node, word);
                    if (!error.empty())
                        return error;
                }
                else if (stream.peek() == '}')
                {
                    stream.read(&c, 1);
                    return "";
                }
                else if (stream.peek() == EOF)
                    return "Found EOF while trying to read new section.";
                else
                    return "Expected '{' or ':', found '" + std::string(1, stream.peek()) + "' instead.";
            }
            else // Something went wrong while reading the word
            {
                if (stream.peek() == EOF)
                    return "Found EOF while trying to read new section.";
                else if (stream.peek() == '}')
                {
                    stream.read(&c, 1);
                    return "";
                }
                else
                    return "Expected section name, found '" + std::string(1, stream.peek()) + "' instead.";
            }
        }

        return "Found EOF while reading section.";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string DataIO::parseKeyValue(std::stringstream& stream, std::shared_ptr<Node> node, const std::string& key)
    {
        // Read the colon from the stream and remove the whitespace behind it
        char c;
        stream.read(&c, 1);
        stream >> std::ws;

        // Read the value
        std::string line = trim(readLine(stream));
        if (!line.empty())
        {
            // Remove the ';' if it is there
            if (stream.peek() == ';')
                stream.read(&c, 1);

            // Create a value node to store the value
            auto valueNode = std::make_shared<ValueNode>();
            valueNode->parent = node.get();
            valueNode->value = line;
            node->propertyValuePairs[toLower(key)] = valueNode;

            // It might be a list node
            if ((line.size() >= 2) && (line[0] == '[') && (line.back() == ']'))
            {
                valueNode->listNode = true;
                if (line.size() >= 3)
                {
                    valueNode->valueList.push_back("");

                    std::size_t i = 1;
                    while (i < line.size()-1)
                    {
                        if (line[i] == ',')
                        {
                            i++;
                            valueNode->valueList.back() = trim(valueNode->valueList.back());
                            valueNode->valueList.push_back("");
                        }
                        else if (line[i] == '"')
                        {
                            valueNode->valueList.back().push_back(line[i]);
                            i++;

                            bool backslash = false;
                            while (i < line.size()-1)
                            {
                                valueNode->valueList.back().push_back(line[i]);

                                if (line[i] == '"' && !backslash)
                                {
                                    i++;
                                    break;
                                }

                                if (line[i] == '\\' && !backslash)
                                    backslash = true;
                                else
                                    backslash = false;

                                i++;
                            }
                        }
                        else
                        {
                            valueNode->valueList.back().push_back(line[i]);
                            i++;
                        }
                    }

                    valueNode->valueList.back() = trim(valueNode->valueList.back());
                }
            }

            return "";
        }
        else
        {
            if (stream.peek() == EOF)
                return "Found EOF while trying to read a value.";
            else
            {
                c = stream.peek();
                if (c == ':')
                    return "Found ':' while trying to read a value.";
                else if (c == '{')
                    return "Found '{' while trying to read a value.";
                else
                    return "Found empty value.";
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string DataIO::readLine(std::stringstream& stream)
    {
        std::string line;
        bool whitespaceFound = false;
        while (stream.peek() != EOF)
        {
            char c = stream.peek();

            if (stream.peek() == '/')
            {
                stream.read(&c, 1);
                if (stream.peek() == '/')
                {
                    while (stream.peek() != EOF)
                    {
                        stream.read(&c, 1);
                        if (c == '\n')
                            break;
                    }
                }
                else if (stream.peek() == '*')
                {
                    while (stream.peek() != EOF)
                    {
                        stream.read(&c, 1);
                        if (stream.peek() == '*')
                        {
                            stream.read(&c, 1);
                            if (stream.peek() == '/')
                            {
                                stream.read(&c, 1);
                                break;
                            }
                        }
                    }
                    continue;
                }
                else
                    return "";

                continue;
            }

            if (c == '"')
            {
                stream.read(&c, 1);
                line.push_back(c);

                bool backslash = false;
                while (stream.peek() != EOF)
                {
                    stream.read(&c, 1);
                    line.push_back(c);

                    if (c == '"' && !backslash)
                        break;

                    if (c == '\\' && !backslash)
                        backslash = true;
                    else
                        backslash = false;
                }

                if (stream.peek() == EOF)
                    return "";

                c = stream.peek();
            }

            if ((c == ':') || (c == '{'))
                return "";
            else if ((c == ';') || (c == '}'))
            {
                // Remove trailing whitespace before returning the line
                line.erase(line.find_last_not_of(" \n\r\t")+1);
                return line;
            }
            else if (::isspace(c))
            {
                stream.read(&c, 1);
                if (!whitespaceFound)
                {
                    whitespaceFound = true;
                    line.push_back(' ');
                }
            }
            else
            {
                whitespaceFound = false;
                line.push_back(c);
                stream.read(&c, 1);
            }
        }

        return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string DataIO::readWord(std::stringstream& stream)
    {
        std::string word = "";
        while (stream.peek() != EOF)
        {
            char c = stream.peek();
            if (!::isspace(c) && (c != ':') && (c != ';') && (c != '{') && (c != '}'))
            {
                stream.read(&c, 1);

                if ((c == '/') && (stream.peek() == '/'))
                {
                    while (stream.peek() != EOF)
                    {
                        stream.read(&c, 1);
                        if (c == '\n')
                        {
                            if (!word.empty())
                                return word;
                            else
                                break;
                        }
                    }
                }
                else if ((c == '/') && (stream.peek() == '*'))
                {
                    while (stream.peek() != EOF)
                    {
                        stream.read(&c, 1);
                        if (c == '*')
                        {
                            if (stream.peek() == '/')
                            {
                                stream.read(&c, 1);
                                break;
                            }
                        }
                    }
                }
                else if (c == '"')
                {
                    word.push_back(c);
                    bool backslash = false;
                    while (stream.peek() != EOF)
                    {
                        stream.read(&c, 1);
                        word.push_back(c);

                        if (c == '"' && !backslash)
                            break;

                        if (c == '\\' && !backslash)
                            backslash = true;
                        else
                            backslash = false;
                    }
                }
                else
                    word.push_back(c);
            }
            else
            {
                if (c == '\r')
                    stream.read(&c, 1);

                return word;
            }
        }

        return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
