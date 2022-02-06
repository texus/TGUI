/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Loading/ThemeLoader.hpp>
#include <TGUI/Loading/Deserializer.hpp>
#include <TGUI/Global.hpp>

#include <sstream>
#include <fstream>

// Ignore warning "C4503: decorated name length exceeded, name was truncated" in Visual Studio
#if defined _MSC_VER
    #pragma warning(disable : 4503)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<String, std::map<String, std::map<String, String>>> DefaultThemeLoader::m_propertiesCache;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BaseThemeLoader::preload(const String&)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BaseThemeLoader::injectThemePath(const std::unique_ptr<DataIO::Node>& node, const String& path) const
    {
        for (const auto& pair : node->propertyValuePairs)
        {
            if (((pair.first.size() >= 7) && (pair.first.substr(0, 7) == U"Texture")) || (pair.first == U"Font") || (pair.first == U"Image"))
            {
                if (pair.second->value.empty() || pair.second->value.equalIgnoreCase(U"none") || pair.second->value.equalIgnoreCase(U"null") || pair.second->value.equalIgnoreCase(U"nullptr"))
                    continue;

                // Insert the path into the filename unless the filename is already an absolute path.
                // We can't just deserialize the value to get rid of the quotes as it may contain things behind the filename.
                if (pair.second->value[0] != '"')
                {
                #ifdef TGUI_SYSTEM_WINDOWS
                    if ((pair.second->value[0] != '/') && (pair.second->value[0] != '\\') && ((pair.second->value.size() <= 1) || (pair.second->value[1] != ':')))
                #else
                    if (pair.second->value[0] != '/')
                #endif
                        pair.second->value = path + pair.second->value;
                }
                else // The filename is between quotes
                {
                    if (pair.second->value.size() <= 1)
                        continue;

                #ifdef TGUI_SYSTEM_WINDOWS
                    if ((pair.second->value[1] != '/') && (pair.second->value[1] != '\\') && ((pair.second->value.size() <= 2) || (pair.second->value[2] != ':')))
                #else
                    if (pair.second->value[1] != '/')
                #endif
                        pair.second->value = '"' + path + pair.second->value.substr(1);
                }
            }
        }

        for (const auto& child : node->children)
            injectThemePath(child, path);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BaseThemeLoader::resolveReferences(std::map<String, std::reference_wrapper<const std::unique_ptr<DataIO::Node>>>& sections, const std::unique_ptr<DataIO::Node>& node) const
    {
        for (const auto& pair : node->propertyValuePairs)
        {
            // Check if this property is a reference to another section
            if (!pair.second->value.empty() && (pair.second->value[0] == '&'))
            {
                const String name = Deserializer::deserialize(ObjectConverter::Type::String, pair.second->value.substr(1)).getString();
                const auto sectionsIt = sections.find(name);
                if (sectionsIt == sections.end())
                    throw Exception{"Undefined reference to '" + name + "' encountered."};

                // Resolve references recursively
                resolveReferences(sections, sectionsIt->second);

                // Make a copy of the section
                std::stringstream ss;
                DataIO::emit(sectionsIt->second, ss);
                pair.second->value = "{\n" + ss.str() + "}";
            }
        }

        for (const auto& child : node->children)
            resolveReferences(sections, child);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void DefaultThemeLoader::flushCache(const String& filename)
    {
        if (filename != "")
        {
            auto propertiesCacheIt = m_propertiesCache.find(filename);
            if (propertiesCacheIt != m_propertiesCache.end())
                m_propertiesCache.erase(propertiesCacheIt);
        }
        else
        {
            m_propertiesCache.clear();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void DefaultThemeLoader::preload(const String& filename)
    {
        if (filename == "")
            return;

        // Load the file when not already in cache
        if (m_propertiesCache.find(filename) == m_propertiesCache.end())
        {
            std::unique_ptr<DataIO::Node> root = readFile(filename);
            if (!root)
                throw Exception{"DefaultThemeLoader::preload failed to load file, readFile returned nullptr."};

            if (root->propertyValuePairs.size() != 0)
                throw Exception{"Unexpected result while loading theme file '" + filename + "'. Root property-value pair found."};

            // Get a list of section names and map them to their nodes (needed for resolving references)
            std::map<String, std::reference_wrapper<const std::unique_ptr<DataIO::Node>>> sections;
            for (const auto& child : root->children)
            {
                const String name = Deserializer::deserialize(ObjectConverter::Type::String, child->name).getString();
                sections.emplace(name, std::cref(child));
            }

            // Resolve references to sections
            resolveReferences(sections, root);

            // Cache all propery value pairs
            for (const auto& section : sections)
            {
                const auto& child = section.second;
                const String& name = section.first;
                for (const auto& pair : child.get()->propertyValuePairs)
                    m_propertiesCache[filename][name][pair.first] = pair.second->value;

                for (const auto& nestedProperty : child.get()->children)
                {
                    std::stringstream ss;
                    DataIO::emit(nestedProperty, ss);
                    m_propertiesCache[filename][name][nestedProperty->name] = "{\n" + ss.str() + "}";
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::map<String, String>& DefaultThemeLoader::load(const String& filename, const String& section)
    {
        preload(filename);

        // An empty filename is not considered an error and will result in an empty property list
        if (filename.empty())
            return m_propertiesCache[""][section];

        if (m_propertiesCache[filename].find(section) == m_propertiesCache[filename].end())
            throw Exception{"No section '" + section + "' was found in file '" + filename + "'."};

        return m_propertiesCache[filename][section];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool DefaultThemeLoader::canLoad(const String& filename, const String& section)
    {
        if (filename.empty())
            return true;
        else
            return m_propertiesCache[filename].find(section) != m_propertiesCache[filename].end();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> DefaultThemeLoader::readFile(const String& filename) const
    {
        if (filename.empty())
            return nullptr;

        String fullFilename;
#ifdef TGUI_SYSTEM_WINDOWS
        if ((filename[0] != '/') && (filename[0] != '\\') && ((filename.size() <= 1) || (filename[1] != ':')))
#else
        if (filename[0] != '/')
#endif
            fullFilename = (getResourcePath() / filename).asString();
        else
            fullFilename = filename;

        std::size_t fileSize;
        auto fileContents = readFileToMemory(fullFilename, fileSize);
        if (!fileContents)
            throw Exception{"Failed to open theme file '" + fullFilename + "'."};

        std::stringstream stream;
        stream.write(reinterpret_cast<char*>(fileContents.get()), fileSize);

        std::unique_ptr<DataIO::Node> root = DataIO::parse(stream);

        String resourcePath;
        auto slashPos = filename.find_last_of("/\\");
        if (slashPos != String::npos)
            resourcePath = filename.substr(0, slashPos+1);

        // Turn texture and font filenames into paths relative to the theme file
        if (!resourcePath.empty())
            injectThemePath(root, resourcePath);

        return root;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
