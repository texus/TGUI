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


#include <TGUI/Loading/WidgetSaver.hpp>
#include <TGUI/Loading/Serializer.hpp>
#include <TGUI/Widgets/Button.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Hidden functions
namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::shared_ptr<DataIO::Node> saveWidget(Widget::Ptr widget)
    {
        std::string widgetName;
        if (widget->getParent())
            widget->getParent()->getWidgetName(widget, widgetName);

        auto node = std::make_shared<DataIO::Node>();
        if (widgetName.empty())
            node->name = widget->getWidgetType();
        else
            node->name = widget->getWidgetType() + "." + widgetName;

        if (!widget->isVisible())
            node->propertyValuePairs["Visible"] = std::make_shared<DataIO::ValueNode>(node.get(), "false");
        if (!widget->isEnabled())
            node->propertyValuePairs["Enabled"] = std::make_shared<DataIO::ValueNode>(node.get(), "false");
        if (widget->getPosition() != sf::Vector2f{0, 0})
            node->propertyValuePairs["Position"] = std::make_shared<DataIO::ValueNode>(node.get(), "(" + tgui::to_string(widget->getPosition().x) + ", " + tgui::to_string(widget->getPosition().y) + ")");
        if (widget->getSize() != sf::Vector2f{0, 0})
            node->propertyValuePairs["Size"] = std::make_shared<DataIO::ValueNode>(node.get(), "(" + tgui::to_string(widget->getSize().x) + ", " + tgui::to_string(widget->getSize().y) + ")");
        if (widget->getTransparency() != 255)
            node->propertyValuePairs["Transparency"] = std::make_shared<DataIO::ValueNode>(node.get(), tgui::to_string(widget->getTransparency()));

        /// TODO: Tooltip, Font, Theme

        if (widget->getRenderer())
        {
            node->children.emplace_back(std::make_shared<DataIO::Node>());
            node->children.back()->name = "Renderer";
            for (auto& pair : widget->getRenderer()->getPropertyValuePairs())
                node->children.back()->propertyValuePairs[pair.first] = std::make_shared<DataIO::ValueNode>(node->children.back().get(), Serializer::serialize(std::move(pair.second)));
        }

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::shared_ptr<DataIO::Node> saveButton(Button::Ptr button)
    {
        auto node = saveWidget(button);
        if (!button->getText().isEmpty())
            node->propertyValuePairs["Text"] = std::make_shared<DataIO::ValueNode>(node.get(), "\"" + button->getText() + "\"");
        node->propertyValuePairs["TextSize"] = std::make_shared<DataIO::ValueNode>(node.get(), tgui::to_string(button->getTextSize()));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::shared_ptr<DataIO::Node> saveContainer(Container::Ptr container)
    {
        auto node = saveWidget(container);
        for (auto& child : container->getWidgets())
        {
            auto& saveFunction = WidgetSaver::getSaveFunction(toLower(child->getWidgetType()));
            if (saveFunction)
                node->children.emplace_back(saveFunction(WidgetConverter{child}));
            else
                throw Exception{"No save function exists for widget type '" + child->getWidgetType() + "'."};
        }

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::shared_ptr<DataIO::Node> saveGuiContainer(GuiContainer::Ptr container)
    {
        auto node = std::make_shared<DataIO::Node>();
        for (auto& child : container->getWidgets())
        {
            auto& saveFunction = WidgetSaver::getSaveFunction(toLower(child->getWidgetType()));
            if (saveFunction)
                node->children.emplace_back(saveFunction(WidgetConverter{child}));
            else
                throw Exception{"No save function exists for widget type '" + child->getWidgetType() + "'."};
        }

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, WidgetSaver::SaveFunction> WidgetSaver::m_saveFunctions =
        {
            {"widget", saveWidget},
            {"button", saveButton},
            {"container", saveContainer},
            {"guicontainer", saveGuiContainer}
        };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetSaver::save(Container::Ptr widget, std::stringstream& stream)
    {
        auto& saveFunction = m_saveFunctions[toLower(widget->getWidgetType())];
        if (saveFunction)
        {
            auto node = saveFunction(WidgetConverter{widget});
            if (node->propertyValuePairs.size() > 0)
            {
                auto root = std::make_shared<DataIO::Node>();
                root->children.push_back(node);
                node->parent = root.get();
                node = root;
            }

            DataIO::emit(node, stream);
        }
        else
            throw Exception{"No save function exists for widget type '" + widget->getWidgetType() + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetSaver::setSaveFunction(const std::string& type, const SaveFunction& saveFunction)
    {
        m_saveFunctions[toLower(type)] = saveFunction;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const WidgetSaver::SaveFunction& WidgetSaver::getSaveFunction(const std::string& type)
    {
        return m_saveFunctions[toLower(type)];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
