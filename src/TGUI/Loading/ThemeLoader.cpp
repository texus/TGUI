/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2016 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Loading/Deserializer.hpp>
#include <TGUI/Loading/ThemeLoader.hpp>
#include <TGUI/Loading/DataIO.hpp>
#include <TGUI/Exception.hpp>
#include <TGUI/Global.hpp>

#include <cassert>
#include <sstream>
#include <fstream>
#include <set>

#ifdef SFML_SYSTEM_ANDROID
    #include <SFML/System/NativeActivity.hpp>
    #include <android/asset_manager_jni.h>
    #include <android/asset_manager.h>
    #include <android/native_activity.h>
    #include <android/configuration.h>
#endif

// Ignore warning "C4503: decorated name length exceeded, name was truncated" in Visual Studio
#if defined _MSC_VER
    #pragma warning(disable : 4503)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, std::map<std::string, std::map<sf::String, sf::String>>> DefaultThemeLoader::m_propertiesCache;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        void injectRelativePathInTextures(std::set<std::shared_ptr<DataIO::Node>>& handledSections, std::shared_ptr<DataIO::Node> node, const std::string& path)
        {
            for (const auto& pair : node->propertyValuePairs)
            {
                if ((pair.first.size() >= 7) && (toLower(pair.first.substr(0, 7)) == "texture"))
                {
                    auto quotePos = pair.second->value.find('"');
                    if (quotePos != std::string::npos)
                    {
                        ///TODO: Detect absolute pathname on windows
                        if ((pair.second->value.getSize() > quotePos + 1) && (pair.second->value[quotePos+1] != '/'))
                            pair.second->value = pair.second->value.substring(0, quotePos+1) + path + pair.second->value.substring(quotePos+1);
                    }
                }
            }

            for (const auto& child : node->children)
            {
                if (handledSections.find(child) == handledSections.end())
                {
                    handledSections.insert(child);
                    injectRelativePathInTextures(handledSections, child, path);
                }
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void resolveReferences(std::map<std::string, std::shared_ptr<DataIO::Node>>& sections, std::shared_ptr<DataIO::Node> node)
        {
            for (const auto& pair : node->propertyValuePairs)
            {
                // Check if this property is a reference to another section
                if (!pair.second->value.isEmpty() && (pair.second->value[0] == '&'))
                {
                    std::string name = toLower(Deserializer::deserialize(ObjectConverter::Type::String, pair.second->value.substring(1)).getString());

                    auto sectionsIt = sections.find(name);
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

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BaseThemeLoader::preload(const std::string&)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void DefaultThemeLoader::flushCache(const std::string& filename)
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

    void DefaultThemeLoader::preload(const std::string& filename)
    {
        if (filename == "")
            return;

        // Load the file when not already in cache
        if (m_propertiesCache.find(filename) == m_propertiesCache.end())
        {
            std::string resourcePath;
            auto slashPos = filename.find_last_of("/\\");
            if (slashPos != std::string::npos)
                resourcePath = filename.substr(0, slashPos+1);

            std::stringstream fileContents;
            readFile(filename, fileContents);

            std::shared_ptr<DataIO::Node> root = DataIO::parse(fileContents);

            if (root->propertyValuePairs.size() != 0)
                throw Exception{"Unexpected result while loading theme file '" + filename + "'. Root property-value pair found."};

            // Inject relative path to the theme file into texture filenames
            if (!resourcePath.empty())
            {
                std::set<std::shared_ptr<DataIO::Node>> handledSections;
                injectRelativePathInTextures(handledSections, root, resourcePath);
            }

            // Get a list of section names and map them to their nodes (needed for resolving references)
            std::map<std::string, std::shared_ptr<DataIO::Node>> sections;
            for (const auto& child : root->children)
            {
                std::string name = toLower(Deserializer::deserialize(ObjectConverter::Type::String, child->name).getString());
                sections[name] = child;
            }

            // Resolve references to sections
            resolveReferences(sections, root);

            // Cache all propery value pairs
            for (const auto& section : sections)
            {
                auto& child = section.second;
                const std::string& name = section.first;
                for (const auto& pair : child->propertyValuePairs)
                    m_propertiesCache[filename][name][toLower(pair.first)] = pair.second->value;

                for (const auto& nestedProperty : child->children)
                {
                    std::stringstream ss;
                    DataIO::emit(nestedProperty, ss);
                    m_propertiesCache[filename][name][toLower(nestedProperty->name)] = "{\n" + ss.str() + "}";
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::map<sf::String, sf::String>& DefaultThemeLoader::load(const std::string& filename, const std::string& section)
    {
        preload(filename);

        std::string lowercaseClassName = toLower(section);

        // An empty filename is not considered an error and will result in an empty property list
        if (filename == "")
            return m_propertiesCache[""][lowercaseClassName];

        if (m_propertiesCache[filename].find(lowercaseClassName) == m_propertiesCache[filename].end())
            throw Exception{"No section '" + section + "' was found in file '" + filename + "'."};

        return m_propertiesCache[filename][lowercaseClassName];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void DefaultThemeLoader::readFile(const std::string& filename, std::stringstream& contents) const
    {
        std::string fullFilename = getResourcePath() + filename;

    #ifdef SFML_SYSTEM_ANDROID
        // If the file does not start with a slash then load it from the assets
        if (!fullFilename.empty() && (fullFilename[0] != '/'))
        {
            ANativeActivity* activity = sf::getNativeActivity();

            JNIEnv* env = 0;
            activity->vm->AttachCurrentThread(&env, NULL);
            jclass clazz = env->GetObjectClass(activity->clazz);

            jmethodID methodID = env->GetMethodID(clazz, "getAssets", "()Landroid/content/res/AssetManager;");
            jobject assetManagerObject = env->CallObjectMethod(activity->clazz, methodID);
            jobject globalAssetManagerRef = env->NewGlobalRef(assetManagerObject);
            AAssetManager* assetManager = AAssetManager_fromJava(env, globalAssetManagerRef);
            assert(assetManager);

            AAsset* asset = AAssetManager_open(assetManager, fullFilename.c_str(), AASSET_MODE_UNKNOWN);
            if (!asset)
                throw Exception{ "Failed to open theme file '" + fullFilename + "' from assets." };

            off_t assetLength = AAsset_getLength(asset);

            char* buffer = new char[assetLength + 1];
            AAsset_read(asset, buffer, assetLength);
            buffer[assetLength] = 0;

            contents << buffer;

            AAsset_close(asset);
            delete[] buffer;

            activity->vm->DetachCurrentThread();
        }
        else
    #endif
        {
            std::ifstream file{fullFilename};
            if (!file.is_open())
                throw Exception{"Failed to open theme file '" + fullFilename + "'."};

            contents << file.rdbuf();
            file.close();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
