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


#ifndef TGUI_THEME_HPP
#define TGUI_THEME_HPP


#include <TGUI/Widget.hpp>
#include <TGUI/Loading/WidgetConverter.hpp>
#include <TGUI/Loading/ThemeLoader.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Base class for the Theme classes
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API BaseTheme : public std::enable_shared_from_this<BaseTheme>
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Virtual destructor
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~BaseTheme() = default;


        /// TODO: Documentation
        virtual void widgetAttached(Widget* widget);
        virtual void widgetDetached(Widget* widget);

        /// TODO: Documentation
        virtual void initWidget(Widget* widget, std::string primary, std::string secondary) = 0;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the construct function of a specific widget type
        ///
        /// @param type    Type of the widget which will cause this construct function to be used
        /// @param loader  New function to be called when this type of widget is being loaded
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void setConstructFunction(const std::string& type, const std::function<Widget::Ptr()>& constructor);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Change the function that will load the widget theme data
        ///
        /// @param themeLoader  Pointer to the new loader
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void setThemeLoader(const std::shared_ptr<BaseThemeLoader>& themeLoader);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Calls the protected widget->reload(primary, secondary, force) function
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void widgetReload(Widget* widget, const std::string& primary = "", const std::string& secondary = "", bool force = false);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        static std::map<std::string, std::function<Widget::Ptr()>> m_constructors; ///< Widget creator functions
        static std::shared_ptr<BaseThemeLoader> m_themeLoader;  ///< Theme loading functions, they read the theme file
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Default Theme class
    ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API Theme : public BaseTheme
    {
    public:
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Construct the theme class, with an optional theme file to load
        ///
        /// @param filename  Filename of the theme file
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Theme(const std::string& filename = "");


        /// TODO: Documentation
        static std::shared_ptr<Theme> create(const std::string& filename = "");


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Load the widget from the theme
        ///
        /// @param className  Name of the class inside the theme file (equals widget type when no class is given)
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        WidgetConverter load(std::string className);

        /// TODO: Documentation
        void reload(const std::string& filename);

        void reload(std::string oldClassName, std::string newClassName);

        void reload(Widget::Ptr widget, std::string className);

        virtual void widgetDetached(Widget* widget) override;

        virtual void initWidget(Widget* widget, std::string filename, std::string className) override;

        template <typename T>
        void setProperty(std::string className, const std::string& property, const T& value)
        {
            className = toLower(className);
            for (auto& pair : m_widgets)
            {
                if (pair.second == className)
                    pair.first->getRenderer()->setProperty(property, value);
            }
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        std::string m_filename;
        std::string m_resourcePath;
        std::map<Widget*, std::string> m_widgets; // Map widget to class name
        std::map<std::string, std::string> m_widgetTypes; // Map class name to type
        std::map<std::string, std::map<std::string, std::string>> m_widgetProperties; // Map class name to property-value pairs
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_THEME_HPP
