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


#include <TGUI/ThemeFileParser.hpp>
#include <TGUI/Texture.hpp>
#include <TGUI/Global.hpp>

#include <algorithm>
#include <cctype>
#include <functional>
#include <cassert>

#ifdef SFML_SYSTEM_ANDROID
    #include "SFML/System/Android/Activity.hpp"
    #include <android/asset_manager_jni.h>
    #include <android/asset_manager.h>
    #include <android/native_activity.h>
    #include <android/configuration.h>
#endif

#pragma warning(disable: 4503)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    std::map<std::string, std::map<std::string, std::vector<std::pair<std::string, std::string>>>> ThemeFileParser::m_cache;

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
        std::stringstream fileContents;
        std::string lowercaseSection = toLower(section);

        // The file may be cached
        if (m_cache.find(m_filename) == m_cache.end())
        {
        #ifdef SFML_SYSTEM_ANDROID
            // If the file does not start with a slash then load it from the assets
            if (!filename.empty() && (filename[0] != '/'))
            {
                /// TODO: Workaround until SFML makes native activity publically accessible
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

                AAsset* asset = AAssetManager_open(assetManager, filename.c_str(), AASSET_MODE_UNKNOWN);
                if (!asset)
                    throw Exception{"Failed to open theme file '" + filename + "' from assets."};

                off_t assetLength = AAsset_getLength(asset);

                char* buffer = new char[assetLength + 1];
                AAsset_read(asset, buffer, assetLength);
                buffer[assetLength] = 0;

                fileContents << buffer;

                AAsset_close(asset);
                delete[] buffer;

                activity->vm->DetachCurrentThread();
            }
            else
        #endif
            {
                std::ifstream file{filename};
                if (!file.is_open())
                    throw Exception{"Failed to open theme file '" + filename + "'."};

                fileContents << file.rdbuf();
                file.close();
            }

            std::string sectionName;
            unsigned int lineNumber = 0;

            // Stop reading when we reach the end of the file
            while (!fileContents.eof())
            {
                // Get the next line
                std::string line;
                std::getline(fileContents, line);
                lineNumber++;

                // If the lines contains a '\r' at the end then remove it
                if (!line.empty() && line[line.size()-1] == '\r')
                    line.erase(line.size()-1);

                std::string::const_iterator c = line.begin();

                // Skip empty lines
                if (!removeWhitespace(line, c))
                    continue;

                if (!isSection(line, c, sectionName))
                {
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

                    m_cache[m_filename][toLower(sectionName)].push_back(std::make_pair(property, value));
                }
            }
        }

        // The section should be in the cache now
        if (m_cache[m_filename].find(lowercaseSection) == m_cache[m_filename].end())
            throw Exception{"Section '" + m_section + "' was not found in " + m_filename + "."};

        m_properties = m_cache[m_filename][lowercaseSection];
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

    void ThemeFileParser::flushCache()
    {
        m_cache.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
