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


#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Loading/Serializer.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/ChatBox.hpp>
#include <TGUI/Widgets/CheckBox.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/ComboBox.hpp>
#include <TGUI/Widgets/Knob.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/ListBox.hpp>
#include <TGUI/Widgets/MenuBar.hpp>
#include <TGUI/Widgets/MessageBox.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/ProgressBar.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/RadioButton.hpp>
#include <TGUI/Widgets/Scrollbar.hpp>
#include <TGUI/Widgets/Slider.hpp>
#include <TGUI/Widgets/SpinButton.hpp>
#include <TGUI/Widgets/Tab.hpp>
#include <TGUI/Widgets/TextBox.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, std::function<Widget::Ptr()>> BaseTheme::m_constructors =
        {
            {"button", std::make_shared<Button>},
            {"chatbox", std::make_shared<ChatBox>},
            {"checkbox", std::make_shared<CheckBox>},
            {"childwindow", std::make_shared<ChildWindow>},
            {"combobox", std::make_shared<ComboBox>},
            {"editbox", std::make_shared<EditBox>},
            {"knob", std::make_shared<Knob>},
            {"label", std::make_shared<Label>},
            {"listbox", std::make_shared<ListBox>},
            {"menubar", std::make_shared<MenuBar>},
            {"messagebox", std::make_shared<MessageBox>},
            {"panel", std::make_shared<Panel>},
            {"progressbar", std::make_shared<ProgressBar>},
            {"radiobutton", std::make_shared<RadioButton>},
            {"scrollbar", std::make_shared<Scrollbar>},
            {"slider", std::make_shared<Slider>},
            {"spinbutton", std::make_shared<SpinButton>},
            {"tab", std::make_shared<Tab>},
            {"textbox", std::make_shared<TextBox>}
        };

    std::shared_ptr<BaseThemeLoader> BaseTheme::m_themeLoader = std::make_shared<DefaultThemeLoader>();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BaseTheme::widgetAttached(Widget* widget)
    {
        widget->attachTheme(shared_from_this());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BaseTheme::widgetDetached(Widget*)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BaseTheme::setConstructFunction(const std::string& type, const std::function<Widget::Ptr()>& constructor)
    {
        m_constructors[toLower(type)] = constructor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BaseTheme::setThemeLoader(const std::shared_ptr<BaseThemeLoader>& themeLoader)
    {
        m_themeLoader = themeLoader;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BaseTheme::widgetReload(Widget* widget, const std::string& primary, const std::string& secondary, bool force)
    {
        widget->reload(primary, secondary, force);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme::Theme(const std::string& filename) :
        m_filename(filename)
    {
        std::string::size_type slashPos = m_filename.find_last_of("/\\");
        if (slashPos != std::string::npos)
            m_resourcePath = m_filename.substr(0, slashPos+1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Theme::Ptr Theme::create(const std::string& filename)
    {
        return std::make_shared<Theme>(filename);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    WidgetConverter Theme::load(std::string className)
    {
        className = toLower(className);

        std::string widgetType;
        if (m_filename != "")
        {
            if (m_widgetTypes.find(className) != m_widgetTypes.end())
                widgetType = m_widgetTypes[className];
            else
                widgetType = toLower(m_themeLoader->load(m_filename, className, m_widgetProperties[className]));
        }
        else // Load the white theme
        {
            widgetType = className;
        }

        auto constructor = m_constructors[widgetType];
        if (constructor)
        {
            Widget::Ptr widget = constructor();
            m_widgets[widget.get()] = className;
            m_widgetTypes[className] = widgetType;

            widgetAttached(widget.get());
            widgetReload(widget.get(), m_filename, className, true);

            return WidgetConverter{widget};
        }
        else
            throw Exception{"Failed to load widget of type '" + widgetType + "'. No constructor function was set for that type."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::reload(const std::string& filename)
    {
        m_filename = filename;

        m_resourcePath = "";
        std::string::size_type slashPos = m_filename.find_last_of("/\\");
        if (slashPos != std::string::npos)
            m_resourcePath = m_filename.substr(0, slashPos+1);

        m_widgetTypes.clear();
        m_widgetProperties.clear();

        for (auto& widget : m_widgets)
        {
            std::string widgetType;
            if (m_filename != "")
            {
                if (m_widgetTypes.find(widget.second) != m_widgetTypes.end())
                    widgetType = m_widgetTypes[widget.second];
                else
                {
                    m_widgetProperties[widget.second].clear();
                    widgetType = toLower(m_themeLoader->load(m_filename, widget.second, m_widgetProperties[widget.second]));
                }
            }
            else
                widgetType = widget.second;

            m_widgetTypes[widget.second] = widgetType;
            widgetReload(widget.first, filename, widget.second, false);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::reload(std::string oldClassName, std::string newClassName)
    {
        oldClassName = toLower(oldClassName);
        newClassName = toLower(newClassName);

        // If we don't have the new class name in the cache then check if the theme loader has it
        if (m_filename != "")
        {
            if (m_widgetTypes.find(newClassName) == m_widgetTypes.end())
            {
                m_widgetProperties[newClassName].clear();
                m_themeLoader->load(m_filename, newClassName, m_widgetProperties[newClassName]);
            }
        }

        // Reload all the widget that match the old class name
        for (auto& widget : m_widgets)
        {
            if (widget.second == oldClassName)
            {
                widget.second = newClassName;
                widgetReload(widget.first, m_filename, newClassName, false);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::reload(Widget::Ptr widget, std::string className)
    {
        className = toLower(className);

        // If we don't have the class name in the cache then check if the theme loader has it
        std::string widgetType;
        if (m_filename != "")
        {
            if (m_widgetTypes.find(className) != m_widgetTypes.end())
                widgetType = m_widgetTypes[className];
            else
            {
                m_widgetProperties[className].clear();
                widgetType = toLower(m_themeLoader->load(m_filename, className, m_widgetProperties[className]));
            }
        }
        else // Load the white theme
        {
            widgetType = className;
            if (!m_constructors[widgetType])
                throw Exception{"Failed to reload widget of type '" + widgetType + "'. No constructor function was set for that type."};
        }

        widgetAttached(widget.get());
        widgetReload(widget.get(), m_filename, className, false);

        m_widgetTypes[className] = widgetType;
        m_widgets[widget.get()] = className;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<Theme> Theme::clone() const
    {
        auto theme = std::make_shared<Theme>(*this);
        theme->m_widgets.clear();
        return theme;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::widgetDetached(Widget* widget)
    {
        auto it = m_widgets.find(widget);
        if (it != m_widgets.end())
            m_widgets.erase(it);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    WidgetConverter Theme::internalLoad(const std::string& filename, const std::string& className)
    {
        if (filename != m_filename)
            throw Exception{"Internal load failed in theme because wrong filename was given"};

        return load(className);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::setProperty(std::string className, const std::string& property, const std::string& value)
    {
        className = toLower(className);
        m_widgetProperties[className][toLower(property)] = value;

        for (auto& pair : m_widgets)
        {
            if (pair.second == className)
                pair.first->getRenderer()->setProperty(property, value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::setProperty(std::string className, const std::string& property, ObjectConverter&& value)
    {
        className = toLower(className);
        m_widgetProperties[className][toLower(property)] = Serializer::serialize(std::move(value));

        for (auto& pair : m_widgets)
        {
            if (pair.second == className)
                pair.first->getRenderer()->setProperty(property, std::move(value));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Theme::getProperty(std::string className, std::string property) const
    {
        className = toLower(className);
        property = toLower(property);
        if (m_widgetProperties.find(className) != m_widgetProperties.end())
        {
            if (m_widgetProperties.at(className).find(property) != m_widgetProperties.at(className).end())
                return m_widgetProperties.at(className).at(property);
        }

        return "";
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, std::string> Theme::getPropertyValuePairs(std::string className) const
    {
        className = toLower(className);
        if (m_widgetProperties.find(className) != m_widgetProperties.end())
            return m_widgetProperties.at(className);
        else
            return std::map<std::string, std::string>{};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Theme::initWidget(Widget* widget, std::string filename, std::string className)
    {
        if (filename != m_filename)
            throw Exception{"Theme tried to init widget which gave a wrong filename"};

        // Temporarily change the resource path to load relative from the theme file
        std::string oldResourcePath = getResourcePath();

        bool resourcePathChanged = false;
        if (!m_resourcePathLock && !m_resourcePath.empty())
        {
            m_resourcePathLock = true;
            resourcePathChanged = true;
            setResourcePath(oldResourcePath + m_resourcePath);
        }

        try
        {
            for (auto& property : m_widgetProperties[className])
                widget->getRenderer()->setProperty(property.first, property.second);
        }
        catch (Exception& e)
        {
            // Restore the resource path before throwing
            if (resourcePathChanged)
            {
                setResourcePath(oldResourcePath);
                m_resourcePathLock = false;
            }
            throw e;
        }

        // Restore the resource path
        if (resourcePathChanged)
        {
            setResourcePath(oldResourcePath);
            m_resourcePathLock = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
