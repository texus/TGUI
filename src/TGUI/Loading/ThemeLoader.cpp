/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
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


#include <TGUI/Loading/ThemeLoader.hpp>
#include <TGUI/Loading/DataIO.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/EditBox.hpp>

#include <cassert>
#include <sstream>
#include <fstream>

#ifdef SFML_SYSTEM_ANDROID
    #include "SFML/System/Android/Activity.hpp"
    #include <android/asset_manager_jni.h>
    #include <android/asset_manager.h>
    #include <android/native_activity.h>
    #include <android/configuration.h>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, std::map<std::string, std::map<sf::String, sf::String>>> DefaultThemeLoader::m_propertiesCache;

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

            for (auto& child : root->children)
            {
                std::string name = Deserializer::deserialize(ObjectConverter::Type::String, toLower(child->name)).getString();

                if (child->children.size() != 0)
                    throw Exception{"Unexpected result while loading theme file '" + filename + "'. Nested section encountered."};

                for (auto& pair : child->propertyValuePairs)
                {
                    // Inject relative path to the theme file into texture filenames
                    if (!resourcePath.empty() && (pair.first.size() >= 7) && (toLower(pair.first.substr(0, 7)) == "texture"))
                    {
                        auto quotePos = pair.second->value.find('"');
                        if (quotePos != std::string::npos)
                        {
                            ///TODO: Detect absolute pathname on windows
                            if ((pair.second->value.size() > quotePos + 1) && (pair.second->value[quotePos+1] != '/'))
                                pair.second->value = pair.second->value.substr(0, quotePos+1) + resourcePath + pair.second->value.substr(quotePos+1);
                        }
                    }

                    m_propertiesCache[filename][name][toLower(pair.first)] = pair.second->value;
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::map<sf::String, sf::String>& DefaultThemeLoader::load(const std::string& filename, const std::string& section)
    {
        preload(filename);

        std::string lowercaseClassName = toLower(section);
        if (m_propertiesCache[filename].find(lowercaseClassName) == m_propertiesCache[filename].end())
            throw Exception{"No section '" + section + "' was found in " + filename + "."};

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
            /// TODO: Workaround until SFML makes native activity publicly accessible
            /// When this happens, extra SFML folder in include can be removed as well.
            ANativeActivity* activity = sf::priv::getActivity(NULL)->activity;

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
                throw Exception{ "Failed to open theme file '" + fullFilename + "'." };

            contents << file.rdbuf();
            file.close();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
