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


#include <TGUI/Global.hpp>
#include <TGUI/Clipboard.hpp>
#include <TGUI/Texture.hpp>

#include <cctype>
#include <cmath>

#ifdef SFML_SYSTEM_ANDROID
    #include "SFML/System/Android/Activity.hpp"
    #include <android/native_activity.h>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    Clipboard TGUI_Clipboard;

    bool TGUI_TabKeyUsageEnabled = true;

    TGUI_API std::string TGUI_ResourcePath = "";

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool readIntRect(std::string value, sf::IntRect& rect)
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
                        rect.left = tgui::stoi(value.substr(0, commaPos));
                        value.erase(0, commaPos+1);

                        // Search for the second comma
                        commaPos = value.find(',');
                        if (commaPos != std::string::npos)
                        {
                            // Get the top value and delete this part of the string
                            rect.top = tgui::stoi(value.substr(0, commaPos));
                            value.erase(0, commaPos+1);

                            // Search for the third comma
                            commaPos = value.find(',');
                            if (commaPos != std::string::npos)
                            {
                                // Get the width value and delete this part of the string
                                rect.width = tgui::stoi(value.substr(0, commaPos));
                                value.erase(0, commaPos+1);

                                // Get the height value
                                rect.height = tgui::stoi(value);

                                return true;
                            }
                        }
                    }
                }
            }

            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void enableTabKeyUsage()
    {
        TGUI_TabKeyUsageEnabled = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void disableTabKeyUsage()
    {
        TGUI_TabKeyUsageEnabled = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void setResourcePath(const std::string& path)
    {
        TGUI_ResourcePath = path;

        if (!TGUI_ResourcePath.empty())
        {
            if (TGUI_ResourcePath[TGUI_ResourcePath.length()-1] != '/')
                TGUI_ResourcePath.push_back('/');
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& getResourcePath()
    {
        return TGUI_ResourcePath;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool compareFloats(float x, float y)
    {
        return (std::abs(x - y) < 0.00000001f);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool isWhitespace(char character)
    {
        if (character == ' ' || character == '\t' || character == '\n')
            return true;
        else
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int stoi(const std::string& value)
    {
        return std::atoi(value.c_str());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float stof(const std::string& value)
    {
        return static_cast<float>(std::atof(value.c_str()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned long stoul(const std::string& value)
    {
        return static_cast<unsigned long>(std::atoi(value.c_str()));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool extractBoolFromString(const std::string& property, const std::string& value)
    {
        if ((value == "true") || (value == "True") || (value == "TRUE") || (value == "1"))
            return true;
        else if ((value == "false") || (value == "False") || (value == "FALSE") || (value == "0"))
            return false;
        else
            throw Exception{"Failed to parse boolean value of property '" + property + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Color extractColorFromString(const std::string& property, const std::string& value)
    {
        std::string string = value;
        sf::Color color;

        // Make sure that the line isn't empty
        if (string.empty() == false)
        {
            // The first and last character have to be brackets
            if ((string[0] == '(') && (string[string.length()-1] == ')'))
            {
                // Remove the brackets
                string.erase(0, 1);
                string.erase(string.length()-1);

                // Search for the first comma
                std::string::size_type commaPos = string.find(',');
                if (commaPos != std::string::npos)
                {
                    // Get the red value and delete this part of the string
                    color.r = tgui::stoi(string.substr(0, commaPos));
                    string.erase(0, commaPos+1);

                    // Search for the second comma
                    commaPos = string.find(',');
                    if (commaPos != std::string::npos)
                    {
                        // Get the green value and delete this part of the string
                        color.g = tgui::stoi(string.substr(0, commaPos));
                        string.erase(0, commaPos+1);

                        // Search for the third comma (optional)
                        commaPos = string.find(',');
                        if (commaPos != std::string::npos)
                        {
                            // Get the blue value and delete this part of the string
                            color.b = tgui::stoi(string.substr(0, commaPos));
                            string.erase(0, commaPos+1);

                            // Get the alpha value
                            color.a = tgui::stoi(string);
                        }
                        else // No alpha value was passed
                        {
                            // Get the blue value
                            color.b = tgui::stoi(string.substr(0, commaPos));
                        }

                        return color;
                    }
                }
            }
        }

        // If you pass here then something is wrong about the line
        throw Exception{"Failed to parse the color value from property '" + property + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Borders extractBordersFromString(const std::string& property, const std::string& value)
    {
        std::string string = value;
        tgui::Borders borders;

        // Make sure that the line isn't empty
        if (string.empty() == false)
        {
            // The first and last character have to be brackets
            if ((string[0] == '(') && (string[string.length()-1] == ')'))
            {
                // Remove the brackets
                string.erase(0, 1);
                string.erase(string.length()-1);

                // Search for the first comma
                std::string::size_type commaPos = string.find(',');
                if (commaPos != std::string::npos)
                {
                    // Get the first value and delete this part of the string
                    borders.left = tgui::stof(string.substr(0, commaPos));
                    string.erase(0, commaPos+1);

                    // Search for the second comma
                    commaPos = string.find(',');
                    if (commaPos != std::string::npos)
                    {
                        // Get the second value and delete this part of the string
                        borders.top = tgui::stof(string.substr(0, commaPos));
                        string.erase(0, commaPos+1);

                        // Search for the third comma
                        commaPos = string.find(',');
                        if (commaPos != std::string::npos)
                        {
                            // Get the third value and delete this part of the string
                            borders.right = tgui::stof(string.substr(0, commaPos));
                            string.erase(0, commaPos+1);

                            // Get the fourth value
                            borders.bottom = tgui::stof(string);

                            return borders;
                        }
                    }
                }
            }
        }

        throw Exception{"Failed to parse the value from property '" + property + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void extractTextureFromString(const std::string& property, const std::string& value, const std::string& rootPath, Texture& texture)
    {
        std::string::const_iterator c = value.begin();

        // Remove all whitespaces (string should still contains something)
        if (!removeWhitespace(value, c))
            throw Exception{"Failed to parse texture property '" + property + "'. Value is empty."};

        // There has to be a quote
        if (*c == '"')
            ++c;
        else
        {
            throw Exception{"Failed to parse texture property '" + property + "'. Expected an opening quote for the filename."};
        }

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
            throw Exception{"Failed to parse texture property '" + property + "'. Failed to find the closing quote of the filename."};

        // There may be optional parameters
        sf::IntRect partRect;
        sf::IntRect middleRect;
        bool repeat = false;

        while (removeWhitespace(value, c))
        {
            std::string word;
            auto openingBracketPos = value.find('(', c - value.begin());
            if (openingBracketPos != std::string::npos)
                word = value.substr(c - value.begin(), openingBracketPos - (c - value.begin()));
            else
                word = value.substr(c - value.begin(), value.length() - (c - value.begin()));

            if ((word == "Stretch") || (word == "stretch"))
            {
                repeat = false;
                std::advance(c, 7);
            }
            else if ((word == "Repeat") || (word == "repeat"))
            {
                repeat = true;
                std::advance(c, 6);
            }
            else
            {
                sf::IntRect* rect = nullptr;

                if ((word == "Part") || (word == "part"))
                {
                    rect = &partRect;
                    std::advance(c, 4);
                }
                else if ((word == "Middle") || (word == "middle"))
                {
                    rect = &middleRect;
                    std::advance(c, 6);
                }
                else
                    throw Exception{"Failed to parse texture property '" + property + "'. Unexpected word '" + word + "' in front of opening bracket."};

                auto closeBracketPos = value.find(')', c - value.begin());
                if (closeBracketPos != std::string::npos)
                {
                    if (!readIntRect(value.substr(c - value.begin(), closeBracketPos - (c - value.begin()) + 1), *rect))
                        throw Exception{"Failed to parse " + word + " rectangle for texture property '" + property + "'."};
                }
                else
                    throw Exception{"Failed to parse texture property '" + property + "'. Failed to find closing bracket for " + word + " rectangle."};

                std::advance(c, closeBracketPos - (c - value.begin()) + 1);
            }
        }

        texture.load(rootPath + filename, partRect, middleRect, repeat);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string toLower(std::string str)
    {
        for (std::string::iterator i = str.begin(); i != str.end(); ++i)
            *i = static_cast<char>(std::tolower(*i));

        return str;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef SFML_SYSTEM_ANDROID
    // This function contains a slightly edited version of http://stackoverflow.com/a/6004049/3161376
    void displayKeyboard(bool show)
    {
        /// TODO: Workaround until SFML makes native activity publically accessible
        /// When this happens, extra SFML folder in include can be removed as well.
        ANativeActivity* activity = sf::priv::getActivity(NULL)->activity;

        // Attaches the current thread to the JVM.
        jint lResult;
        jint lFlags = 0;

        JavaVM* lJavaVM = activity->vm;
        JNIEnv* lJNIEnv = activity->env;

        JavaVMAttachArgs lJavaVMAttachArgs;
        lJavaVMAttachArgs.version = JNI_VERSION_1_6;
        lJavaVMAttachArgs.name = "NativeThread";
        lJavaVMAttachArgs.group = NULL;

        lResult = lJavaVM->AttachCurrentThread(&lJNIEnv, &lJavaVMAttachArgs);
        if (lResult == JNI_ERR)
            return;

        // Retrieves NativeActivity.
        jobject lNativeActivity = activity->clazz;
        jclass ClassNativeActivity = lJNIEnv->GetObjectClass(lNativeActivity);

        // Retrieves Context.INPUT_METHOD_SERVICE.
        jclass ClassContext = lJNIEnv->FindClass("android/content/Context");
        jfieldID FieldINPUT_METHOD_SERVICE = lJNIEnv->GetStaticFieldID(ClassContext, "INPUT_METHOD_SERVICE", "Ljava/lang/String;");
        jobject INPUT_METHOD_SERVICE = lJNIEnv->GetStaticObjectField(ClassContext, FieldINPUT_METHOD_SERVICE);
        //jniCheck(INPUT_METHOD_SERVICE);

        // Runs getSystemService(Context.INPUT_METHOD_SERVICE).
        jclass ClassInputMethodManager = lJNIEnv->FindClass("android/view/inputmethod/InputMethodManager");
        jmethodID MethodGetSystemService = lJNIEnv->GetMethodID(ClassNativeActivity, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
        jobject lInputMethodManager = lJNIEnv->CallObjectMethod(lNativeActivity, MethodGetSystemService, INPUT_METHOD_SERVICE);

        // Runs getWindow().getDecorView().
        jmethodID MethodGetWindow = lJNIEnv->GetMethodID(ClassNativeActivity, "getWindow", "()Landroid/view/Window;");
        jobject lWindow = lJNIEnv->CallObjectMethod(lNativeActivity, MethodGetWindow);
        jclass ClassWindow = lJNIEnv->FindClass("android/view/Window");
        jmethodID MethodGetDecorView = lJNIEnv->GetMethodID(ClassWindow, "getDecorView", "()Landroid/view/View;");
        jobject lDecorView = lJNIEnv->CallObjectMethod(lWindow, MethodGetDecorView);

        if (show)
        {
            // Runs lInputMethodManager.showSoftInput(...).
            jmethodID MethodShowSoftInput = lJNIEnv->GetMethodID(ClassInputMethodManager, "showSoftInput", "(Landroid/view/View;I)Z");
            jboolean lResult = lJNIEnv->CallBooleanMethod(lInputMethodManager, MethodShowSoftInput, lDecorView, lFlags);
        }
        else // The keyboard should be hidden
        {
            // Runs lWindow.getViewToken()
            jclass ClassView = lJNIEnv->FindClass("android/view/View");
            jmethodID MethodGetWindowToken = lJNIEnv->GetMethodID(ClassView, "getWindowToken", "()Landroid/os/IBinder;");
            jobject lBinder = lJNIEnv->CallObjectMethod(lDecorView, MethodGetWindowToken);

            // lInputMethodManager.hideSoftInput(...).
            jmethodID MethodHideSoftInput = lJNIEnv->GetMethodID(ClassInputMethodManager, "hideSoftInputFromWindow", "(Landroid/os/IBinder;I)Z");
            jboolean lRes = lJNIEnv->CallBooleanMethod(lInputMethodManager, MethodHideSoftInput, lBinder, lFlags);
        }

        // Finished with the JVM.
        lJavaVM->DetachCurrentThread();
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
