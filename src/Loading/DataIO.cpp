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


#include <TGUI/Loading/DataIO.hpp>
#include <TGUI/Global.hpp>
#include <TGUI/String.hpp>

#if TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <stdio.h> // EOF
#else
    #include <cctype> // isspace
    #include <algorithm>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define REMOVE_WHITESPACE_AND_COMMENTS(ReturnErrorOnEOF) \
    for (;;) \
    { \
        stream >> std::ws; \
        if (stream.peek() == EOF) \
            break; \
        \
        if (stream.peek() == '/') \
        { \
            char c; \
            stream.read(&c, 1); \
            if (stream.peek() == '/') \
            { \
                while (stream.peek() != EOF) \
                { \
                    stream.read(&c, 1); \
                    if (c == '\n') \
                        break; \
                } \
            } \
            else if (stream.peek() == '*') \
            { \
                while (stream.peek() != EOF) \
                { \
                    stream.read(&c, 1); \
                    if (stream.peek() == '*') \
                    { \
                        stream.read(&c, 1); \
                        if (stream.peek() == '/') \
                        { \
                            stream.read(&c, 1); \
                            break; \
                        } \
                    } \
                } \
                continue; \
            } \
            else \
                return "Unexpected '/' found."; \
            \
            continue; \
        } \
        break; \
    } \
    \
    if (stream.peek() == EOF) \
    { \
        if (ReturnErrorOnEOF) \
            return "Unexpected EOF while parsing."; \
        else \
            return ""; \
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        // Forward declare one of the functions to solve circular dependency
        String parseSection(std::stringstream& stream, const std::unique_ptr<DataIO::Node>& node, const String& sectionName);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String readWord(std::stringstream& stream)
        {
            String word = "";
            while (stream.peek() != EOF)
            {
                char c = static_cast<char>(stream.peek());
                if (c == '\r')
                {
                    stream.read(&c, 1);
                    return word;
                }
                else if (!std::isspace(static_cast<unsigned char>(c)) && (c != '=') && (c != ';') && (c != ':') && (c != '{') && (c != '}'))
                {
                    stream.read(&c, 1);

                    if ((c == '/') && (stream.peek() == '/'))
                    {
                        while (stream.peek() != EOF)
                        {
                            stream.read(&c, 1);
                            if (c == '\n')
                            {
                                TGUI_ASSERT(!word.empty(), "There is no known case in which you can pass here with an empty word "
                                                           "(comment would have been skipped earlier)");
                                return word;
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
                    return word;
            }

            return "";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String readLine(std::stringstream& stream)
        {
            std::string line;
            bool whitespaceFound = false;
            while (stream.peek() != EOF)
            {
                char c = static_cast<char>(stream.peek());

                if (c == '/')
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
                    else // The slash is part of the value
                    {
                        whitespaceFound = false;
                        line.push_back(c);
                    }

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

                    c = static_cast<char>(stream.peek());
                }

                if ((c == '=') || (c == '{'))
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

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String parseKeyValue(std::stringstream& stream, std::unique_ptr<DataIO::Node>& node, const String& key)
        {
            // Read the assignment symbol from the stream and remove the whitespace behind it
            char chr;
            stream.read(&chr, 1);

            REMOVE_WHITESPACE_AND_COMMENTS(true)

            // Check for subsection as value
            if (stream.peek() == '{')
                return parseSection(stream, node, key);

            // Read the value
            String line = readLine(stream).trim();
            if (!line.empty())
            {
                // Remove the ';' if it is there
                if (stream.peek() == ';')
                    stream.read(&chr, 1);

                // Create a value node to store the value
                auto valueNode = std::make_unique<DataIO::ValueNode>();
                valueNode->value = line;

                // It might be a list node
                if ((line.size() >= 2) && (line[0] == '[') && (line.back() == ']'))
                {
                    valueNode->listNode = true;
                    if (line.size() >= 3)
                    {
                        valueNode->valueList.emplace_back("");

                        std::size_t i = 1;
                        while (i < line.size()-1)
                        {
                            if (line[i] == ',')
                            {
                                i++;
                                valueNode->valueList.back() = valueNode->valueList.back().trim();
                                valueNode->valueList.emplace_back("");
                            }
                            else if (line[i] == '"')
                            {
                                valueNode->valueList.back().insert(valueNode->valueList.back().size(), 1, line[i]);
                                i++;

                                bool backslash = false;
                                while (i < line.size()-1)
                                {
                                    valueNode->valueList.back().insert(valueNode->valueList.back().size(), 1, line[i]);

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
                                valueNode->valueList.back().insert(valueNode->valueList.back().size(), 1, line[i]);
                                i++;
                            }
                        }

                        valueNode->valueList.back() = valueNode->valueList.back().trim();
                    }
                }

                node->propertyValuePairs[key] = std::move(valueNode);
                return "";
            }
            else
            {
                if (stream.peek() == EOF)
                    return "Found EOF while trying to read a value.";
                else
                {
                    chr = static_cast<char>(stream.peek());
                    if (chr == '=')
                        return "Found '=' while trying to read a value.";
                    else if (chr == '{')
                        return "Found '{' while trying to read a value.";
                    else
                        return "Found empty value.";
                }
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String parseInheritance(std::stringstream& stream, const std::unique_ptr<DataIO::Node>& node, const String& sectionName)
        {
            // Read the colon from the stream
            char chr;
            stream.read(&chr, 1);
            REMOVE_WHITESPACE_AND_COMMENTS(true)

            String baseSectionName = readWord(stream);
            if (baseSectionName.empty())
                return "Expected name of base section to inherit from after ':'.";

            const DataIO::Node* parentNode = node.get();
            const DataIO::Node* baseSectionNode = nullptr;
            while (!baseSectionNode && parentNode)
            {
                for (const auto& prevSections : parentNode->children)
                {
                    if (prevSections->name != baseSectionName)
                        continue;

                    baseSectionNode = prevSections.get();
                    break;
                }

                parentNode = parentNode->parent;
            }

            if (!baseSectionNode)
                return "Failed to find base section '" + baseSectionName + "' to inherit from.";

            REMOVE_WHITESPACE_AND_COMMENTS(true)
            if (stream.peek() != '{')
                return "Expected '{' after specifying base section to inherit from.";

            const auto& error = parseSection(stream, node, sectionName);
            if (!error.empty())
                return error;

            const auto& sectionNode = node->children.back(); // This node was added by parseSection

            // Copy properties that aren't overwritten
            for (const auto& pair : baseSectionNode->propertyValuePairs)
            {
                const String& propertyName = pair.first;
                if (sectionNode->propertyValuePairs.find(propertyName) == sectionNode->propertyValuePairs.end())
                    sectionNode->propertyValuePairs[propertyName] = std::make_unique<DataIO::ValueNode>(*pair.second);
            }

            // Copy children that aren't overwritten
            for (const auto& baseChildNode : baseSectionNode->children)
            {
                const auto it = std::find_if(sectionNode->children.begin(), sectionNode->children.end(),
                    [&](const std::unique_ptr<DataIO::Node>& childNode){ return childNode->name == baseChildNode->name; });
                if (it == sectionNode->children.end())
                    sectionNode->children.push_back(std::make_unique<DataIO::Node>(*baseChildNode));
            }

            return "";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String parseSection(std::stringstream& stream, const std::unique_ptr<DataIO::Node>& node, const String& sectionName)
        {
            // Create a new node for this section
            auto sectionNode = std::make_unique<DataIO::Node>();
            sectionNode->parent = node.get();
            sectionNode->name = sectionName;

            // Read the brace from the stream
            char chr;
            stream.read(&chr, 1);

            while (stream.peek() != EOF)
            {
                REMOVE_WHITESPACE_AND_COMMENTS(true)

                String word = readWord(stream);
                if (word == U"")
                {
                    if (stream.peek() == EOF)
                        return "Found EOF while trying to read property or nested section name.";
                    else if (stream.peek() == '}')
                    {
                        node->children.push_back(std::move(sectionNode));

                        stream.read(&chr, 1);

                        // Ignore semicolon behind closing brace
                        REMOVE_WHITESPACE_AND_COMMENTS(false)
                        if (stream.peek() == ';')
                            stream.read(&chr, 1);

                        REMOVE_WHITESPACE_AND_COMMENTS(false)
                        return "";
                    }
                    else if (stream.peek() != '{')
                        return "Expected property or nested section name, found '" + String(1, static_cast<char>(stream.peek())) + "' instead.";
                }

                REMOVE_WHITESPACE_AND_COMMENTS(true)
                if (stream.peek() == '{')
                {
                    String error = parseSection(stream, sectionNode, word);
                    if (!error.empty())
                        return error;
                }
                else if (stream.peek() == '=')
                {
                    String error = parseKeyValue(stream, sectionNode, word);
                    if (!error.empty())
                        return error;
                }
                else if (stream.peek() == ':')
                {
                    String error = parseInheritance(stream, sectionNode, word);
                    if (!error.empty())
                        return error;
                }
                else
                    return "Expected '{', '=' or ':', found '" + String(1, static_cast<char>(stream.peek())) + "' instead.";
            }

            return "Found EOF while reading section.";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        String parseRootSection(std::stringstream& stream, std::unique_ptr<DataIO::Node>& root)
        {
            REMOVE_WHITESPACE_AND_COMMENTS(false)

            String word = readWord(stream);
            if (word == U"")
            {
                REMOVE_WHITESPACE_AND_COMMENTS(true)
                if (stream.peek() != '{')
                    return "Expected section name, found '" + String(1, static_cast<char>(stream.peek())) + "' instead.";
            }

            REMOVE_WHITESPACE_AND_COMMENTS(true)
            if (stream.peek() == '{')
                return parseSection(stream, root, word);
            else if (stream.peek() == '=')
                return parseKeyValue(stream, root, word);
            else if (stream.peek() == ':')
                return parseInheritance(stream, root, word);
            else
                return "Expected '{', '=' or ':', found '" + String(1, static_cast<char>(stream.peek())) + "' instead.";
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<String> convertNodesToLines(const std::unique_ptr<DataIO::Node>& node)
        {
            std::vector<String> output;
            if (node->name.empty())
                output.emplace_back("{");
            else
                output.emplace_back(node->name + " {");

            if (!node->propertyValuePairs.empty())
            {
                for (const auto& pair : node->propertyValuePairs)
                    output.emplace_back("    " + pair.first + " = " + pair.second->value + ";");
            }

            if (node->propertyValuePairs.size() > 0 && node->children.size() > 0)
                output.emplace_back("");

            if (!node->children.empty())
            {
                for (std::size_t i = 0; i < node->children.size(); ++i)
                {
                    for (const auto& line : convertNodesToLines(node->children[i]))
                    {
                        if (!line.empty())
                            output.emplace_back("    " + line);
                        else
                            output.emplace_back("");
                    }

                    if (i < node->children.size() - 1)
                        output.emplace_back("");
                }
            }

            output.emplace_back("}");
            return output;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> DataIO::parse(std::stringstream& stream)
    {
        auto root = std::make_unique<Node>();

        String error;
        while (stream.peek() != EOF)
        {
            error = parseRootSection(stream, root);
            if (!error.empty())
            {
                const auto position = stream.tellg();
                if (position != std::stringstream::pos_type(-1))
                {
                    String str = stream.str();
                    auto lineNumber = std::count(str.begin(), str.begin() + static_cast<std::ptrdiff_t>(position), U'\n') + 1;
                    throw Exception{U"Error while parsing input at line " + String::fromNumber(lineNumber) + U". " + error};
                }
                else
                    throw Exception{U"Error while parsing input. " + error};
            }
        }

        return root;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void DataIO::emit(const std::unique_ptr<Node>& rootNode, std::stringstream& stream)
    {
        for (const auto& pair : rootNode->propertyValuePairs)
            stream << pair.first << " = " << pair.second->value << ";" << std::endl;

        if (rootNode->propertyValuePairs.size() > 0 && rootNode->children.size() > 0)
            stream << std::endl;

        std::vector<String> output;
        for (std::size_t i = 0; i < rootNode->children.size(); ++i)
        {
            for (const auto& line : convertNodesToLines(rootNode->children[i]))
                output.emplace_back(std::move(line));

            if (i < rootNode->children.size()-1)
                output.emplace_back("");
        }

        for (const auto& line : output)
            stream << line << std::endl;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
