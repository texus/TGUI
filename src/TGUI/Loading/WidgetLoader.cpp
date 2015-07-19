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


#include <TGUI/Loading/WidgetLoader.hpp>
#include <TGUI/Loading/Deserializer.hpp>
#include <TGUI/Widgets/Button.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    bool parseBoolean(std::string str)
    {
        str = tgui::toLower(str);
        if (str == "true" || str == "yes" || str == "on" || str == "1")
            return true;
        else if (str == "false" || str == "no" || str == "off" || str == "0")
            return false;
        else
            throw tgui::Exception{"Failed to parse boolean in '" + str + "'"};
    }

    sf::Vector2f parseVector2f(std::string str)
    {
        if (str.empty() || str.front() != '(' || str.back() != ')')
            throw tgui::Exception{"Failed to parse position '" + str + "'. Expected brackets."};

        str = str.substr(1, str.length()-2);

        auto commaPos = str.find(',');
        if (commaPos == std::string::npos)
            throw tgui::Exception{"Failed to parse position '" + str + "'. Expected numbers separated with a comma."};

        if (str.find(',', commaPos + 1) != std::string::npos)
            throw tgui::Exception{"Failed to parse position '" + str + "'. Expected only one comma."};

        return {tgui::stof(str.substr(0, commaPos)), tgui::stof(str.substr(commaPos + 1))};
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Hidden functions
namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadWidget(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
    {
        assert(widget != nullptr);

        if (node->propertyValuePairs["Visible"])
        {
            bool visible = parseBoolean(node->propertyValuePairs["Visible"]->value);
            if (visible)
                widget->show();
            else
                widget->hide();
        }
        if (node->propertyValuePairs["Enabled"])
        {
            bool enabled = parseBoolean(node->propertyValuePairs["Enabled"]->value);
            if (enabled)
                widget->enable();
            else
                widget->disable();
        }
        if (node->propertyValuePairs["Position"])
            widget->setPosition(parseVector2f(node->propertyValuePairs["Position"]->value));
        if (node->propertyValuePairs["Size"])
            widget->setSize(parseVector2f(node->propertyValuePairs["Size"]->value));
        if (node->propertyValuePairs["Transparency"])
        {
            auto transparency = tgui::stoi(node->propertyValuePairs["Transparency"]->value);
            if (transparency < 0 || transparency > 255)
                throw Exception{"Transparency value has to lie between 0 and 255"};

            widget->setTransparency(transparency);
        }

        /// TODO: Tooltip, Font, Theme

        for (auto& childNode : node->children)
        {
            if (childNode->name == "Renderer")
            {
                for (auto& pair : childNode->propertyValuePairs)
                    widget->getRenderer()->setProperty(pair.first, pair.second->value);
            }
        }

        /**
        if (widget->getRenderer())
        {
            node->children.emplace_back(std::make_shared<DataIO::Node>());
            node->children.back()->name = "Renderer";
            for (auto& pair : widget->getRenderer()->getPropertyValuePairs())
                node->children.back()->propertyValuePairs[pair.first] = std::make_shared<DataIO::ValueNode>(node->children.back().get(), Serializer::serialize(pair.second));
        }
        */
        return widget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadButton(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        Button::Ptr button;
        if (widget)
            button = std::static_pointer_cast<Button>(widget);
        else
            button = std::make_shared<Button>();

        loadWidget(node, button);
        if (node->propertyValuePairs["Text"])
            button->setText(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["Text"]->value).getString());
        if (node->propertyValuePairs["TextSize"])
            button->setTextSize(tgui::stoi(node->propertyValuePairs["TextSize"]->value));

        return button;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadContainer(std::shared_ptr<DataIO::Node> node, Container::Ptr container)
    {
        assert(container != nullptr);
        loadWidget(node, container);

        for (auto& childNode : node->children)
        {
            if (childNode->name != "Renderer")
            {
                auto nameSeparator = childNode->name.find('.');
                auto widgetType = childNode->name.substr(0, nameSeparator);
                auto& loadFunction = WidgetLoader::getLoadFunction(toLower(widgetType));
                if (loadFunction)
                {
                    std::string className;
                    if (nameSeparator != std::string::npos)
                        className = childNode->name.substr(nameSeparator + 1);

                    tgui::Widget::Ptr childWidget = loadFunction(childNode);
                    container->add(childWidget, className);
                }
                else
                    throw Exception{"No load function exists for widget type '" + widgetType + "'."};
            }
        }

        return container;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, WidgetLoader::LoadFunction> WidgetLoader::m_loadFunctions =
        {
            {"widget", std::bind(loadWidget, std::placeholders::_1, nullptr)},
            {"button", std::bind(loadButton, std::placeholders::_1, nullptr)},
            {"container", std::bind(loadContainer, std::placeholders::_1, nullptr)}
        };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetLoader::load(Container::Ptr parent, std::stringstream& stream)
    {
        auto rootNode = DataIO::parse(stream);

        if (rootNode->propertyValuePairs.size() != 0)
            throw Exception{"Root property-value pairs not yet supported when loading widgets"};

        for (auto& node : rootNode->children)
        {
            auto nameSeparator = node->name.find('.');
            auto widgetType = node->name.substr(0, nameSeparator);
            auto& loadFunction = m_loadFunctions[toLower(widgetType)];
            if (loadFunction)
            {
                std::string className;
                if (nameSeparator != std::string::npos)
                    className = node->name.substr(nameSeparator + 1);

                tgui::Widget::Ptr widget = loadFunction(node);
                parent->add(widget, className);
            }
            else
                throw Exception{"No load function exists for widget type '" + widgetType + "'."};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetLoader::setLoadFunction(const std::string& type, const LoadFunction& loadFunction)
    {
        m_loadFunctions[toLower(type)] = loadFunction;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const WidgetLoader::LoadFunction& WidgetLoader::getLoadFunction(const std::string& type)
    {
        return m_loadFunctions[toLower(type)];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
