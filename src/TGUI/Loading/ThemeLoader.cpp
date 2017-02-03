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

#include <TGUI/Loading/ThemeLoader.hpp>
#include <TGUI/Loading/DataIO.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/EditBox.hpp>

#include <cassert>
#include <sstream>
#include <fstream>

#ifdef SFML_SYSTEM_ANDROID
    #if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
        #include "SFML/System/NativeActivity.hpp"
    #else
        #include "SFML/System/Android/Activity.hpp"
    #endif
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

    std::map<std::string, std::map<std::string, DefaultThemeLoader::PropertyValuePairs>> DefaultThemeLoader::m_propertiesCache;
    std::map<std::string, std::map<std::string, std::string>> DefaultThemeLoader::m_widgetTypeCache;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void DefaultThemeLoader::flushCache(const std::string& filename)
    {
        if (filename != "")
        {
            auto propertiesCacheIt = m_propertiesCache.find(filename);
            if (propertiesCacheIt != m_propertiesCache.end())
                m_propertiesCache.erase(propertiesCacheIt);

            auto widgetTypeCacheIt = m_widgetTypeCache.find(filename);
            if (widgetTypeCacheIt != m_widgetTypeCache.end())
                m_widgetTypeCache.erase(widgetTypeCacheIt);
        }
        else
        {
            m_propertiesCache.clear();
            m_widgetTypeCache.clear();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string DefaultThemeLoader::load(const std::string& filename, const std::string& className, std::map<std::string, std::string>& propertyValuePair)
    {
        std::string lowercaseClassName = toLower(className);

        // The file may be cached
        if (m_propertiesCache.find(filename) == m_propertiesCache.end())
        {
            std::stringstream fileContents;
            readFile(filename, fileContents);

            std::shared_ptr<DataIO::Node> root = DataIO::parse(fileContents);

            if (root->propertyValuePairs.size() != 0)
                throw Exception{"Unexpected result while loading theme file '" + filename + "'. Root property-value pair found."};

            for (auto& child : root->children)
            {
                if (child->children.size() != 0)
                    throw Exception{"Unexpected result while loading theme file '" + filename + "'. Nested section encountered."};

                auto pos = child->name.find('.');
                std::string parsedClassName;
                std::string widgetType = toLower(child->name.substr(0, pos));
                if (pos != std::string::npos)
                {
                    if ((child->name.size() >= pos + 2) && (child->name[pos+1] == '"') && (child->name.back() == '"'))
                        parsedClassName = toLower(Deserializer::deserialize(ObjectConverter::Type::String, child->name.substr(pos + 1)).getString());
                    else
                        parsedClassName = toLower(child->name.substr(pos + 1));
                }
                else
                    parsedClassName = widgetType;

                for (auto& pair : child->propertyValuePairs)
                {
                    m_propertiesCache[filename][parsedClassName][toLower(pair.first)] = pair.second->value;
                    m_widgetTypeCache[filename][parsedClassName] = widgetType;
                }
            }
        }

        // The class name should be in the cache now
        if (m_propertiesCache[filename].find(lowercaseClassName) == m_propertiesCache[filename].end())
            throw Exception{"No class '" + className + "' was found in " + filename + "."};

        // Copy the properties that were not already set
        for (auto& pair : m_propertiesCache[filename][lowercaseClassName])
        {
            if (propertyValuePair.find(pair.first) == propertyValuePair.end())
                propertyValuePair[pair.first] = pair.second;
        }

        return m_widgetTypeCache[filename][lowercaseClassName];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void DefaultThemeLoader::readFile(const std::string& filename, std::stringstream& contents) const
    {
        std::string fullFilename = getResourcePath() + filename;

    #ifdef SFML_SYSTEM_ANDROID
        // If the file does not start with a slash then load it from the assets
        if (!fullFilename.empty() && (fullFilename[0] != '/'))
        {
            ANativeActivity* activity;
            #if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 4)
                activity = sf::getNativeActivity();
            #else
                activity = sf::priv::getActivity(NULL)->activity;
            #endif

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
