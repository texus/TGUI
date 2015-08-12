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


#include <TGUI/Loading/Deserializer.hpp>
#include <TGUI/Loading/WidgetLoader.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Canvas.hpp>
#include <TGUI/Widgets/Checkbox.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/ComboBox.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/Knob.hpp>
#include <TGUI/Widgets/ListBox.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <TGUI/Widgets/ProgressBar.hpp>
#include <TGUI/Widgets/RadioButton.hpp>
#include <TGUI/Widgets/Scrollbar.hpp>
#include <TGUI/Widgets/Slider.hpp>
#include <TGUI/Widgets/SpinButton.hpp>
#include <TGUI/Widgets/Tab.hpp>
#include <TGUI/Widgets/TextBox.hpp>
#include <TGUI/Widgets/Tooltip.hpp>

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

    #define DESERIALIZE_STRING(property) Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[property]->value).getString()

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

        /// TODO: Font (and Theme?)

        for (auto& childNode : node->children)
        {
            if (toLower(childNode->name) == "renderer")
            {
                for (auto& pair : childNode->propertyValuePairs)
                    widget->getRenderer()->setProperty(pair.first, pair.second->value);
            }
            else if (toLower(childNode->name) == "tooltip")
                widget->setTooltip(std::dynamic_pointer_cast<tgui::Tooltip>(tgui::WidgetLoader::getLoadFunction("Tooltip")(childNode)));
        }

        return widget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadContainer(std::shared_ptr<DataIO::Node> node, Container::Ptr container)
    {
        assert(container != nullptr);
        loadWidget(node, container);

        for (auto& childNode : node->children)
        {
            if ((toLower(childNode->name) != "renderer") && (toLower(childNode->name) != "tooltip"))
            {
                auto nameSeparator = childNode->name.find('.');
                auto widgetType = childNode->name.substr(0, nameSeparator);
                auto& loadFunction = WidgetLoader::getLoadFunction(toLower(widgetType));
                if (loadFunction)
                {
                    std::string className;
                    if (nameSeparator != std::string::npos)
                    {
                        if ((childNode->name.size() >= nameSeparator + 2) && (childNode->name[nameSeparator+1] == '"') && (childNode->name.back() == '"'))
                            className = Deserializer::deserialize(ObjectConverter::Type::String, childNode->name.substr(nameSeparator + 1)).getString();
                        else
                            className = childNode->name.substr(nameSeparator + 1);
                    }

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

    TGUI_API Widget::Ptr loadButton(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        Button::Ptr button;
        if (widget)
            button = std::static_pointer_cast<Button>(widget);
        else
            button = std::make_shared<Button>();

        loadWidget(node, button);
        if (node->propertyValuePairs["Text"])
            button->setText(DESERIALIZE_STRING("Text"));
        if (node->propertyValuePairs["TextSize"])
            button->setTextSize(tgui::stoi(node->propertyValuePairs["TextSize"]->value));

        return button;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadCanvas(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        if (widget)
            return loadWidget(node, widget);
        else
            return loadWidget(node, std::make_shared<Canvas>());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadCheckbox(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        Checkbox::Ptr checkbox;
        if (widget)
            checkbox = std::static_pointer_cast<Checkbox>(widget);
        else
            checkbox = std::make_shared<Checkbox>();

        loadWidget(node, checkbox);
        if (node->propertyValuePairs["Text"])
            checkbox->setText(DESERIALIZE_STRING("Text"));
        if (node->propertyValuePairs["TextSize"])
            checkbox->setTextSize(tgui::stoi(node->propertyValuePairs["TextSize"]->value));
        if (node->propertyValuePairs["Checked"])
        {
            if (parseBoolean(node->propertyValuePairs["Checked"]->value))
                checkbox->check();
        }

        return checkbox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadChildWindow(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        ChildWindow::Ptr childWindow;
        if (widget)
            childWindow = std::static_pointer_cast<ChildWindow>(widget);
        else
            childWindow = std::make_shared<ChildWindow>();

        loadWidget(node, childWindow);

        if (node->propertyValuePairs["TitleAlignment"])
        {
            if (toLower(node->propertyValuePairs["TitleAlignment"]->value) == "left")
                childWindow->setTitleAlignment(ChildWindow::TitleAlignment::Left);
            else if (toLower(node->propertyValuePairs["TitleAlignment"]->value) == "center")
                childWindow->setTitleAlignment(ChildWindow::TitleAlignment::Center);
            else if (toLower(node->propertyValuePairs["TitleAlignment"]->value) == "right")
                childWindow->setTitleAlignment(ChildWindow::TitleAlignment::Right);
            else
                throw Exception{"Failed to parse TitleAlignment property. Only the values Left, Center and Right are correct."};
        }

        if (node->propertyValuePairs["Title"])
            childWindow->setTitle(DESERIALIZE_STRING("Title"));

        if (node->propertyValuePairs["Icon"])
            childWindow->setIcon(Deserializer::deserialize(ObjectConverter::Type::Texture, node->propertyValuePairs["Icon"]->value).getTexture());

        if (node->propertyValuePairs["KeepInParent"])
            childWindow->keepInParent(parseBoolean(node->propertyValuePairs["KeepInParent"]->value));

        for (auto& childNode : node->children)
        {
            if (toLower(childNode->name) == "closebutton")
                childWindow->setCloseButton(std::static_pointer_cast<Button>(WidgetLoader::getLoadFunction("button")(childNode)));
        }

        return childWindow;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadClickableWidget(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        if (widget)
            return loadWidget(node, widget);
        else
            return loadWidget(node, std::make_shared<ClickableWidget>());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadComboBox(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        ComboBox::Ptr comboBox;
        if (widget)
            comboBox = std::static_pointer_cast<ComboBox>(widget);
        else
            comboBox = std::make_shared<ComboBox>();

        for (auto& childNode : node->children)
        {
            if (toLower(childNode->name) == "listbox")
                comboBox->setListBox(std::static_pointer_cast<ListBox>(WidgetLoader::getLoadFunction("listbox")(childNode)));
        }

        loadWidget(node, comboBox);

        if (node->propertyValuePairs["ItemsToDisplay"])
            comboBox->setItemsToDisplay(tgui::stoi(node->propertyValuePairs["ItemsToDisplay"]->value));

        return comboBox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadEditBox(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        EditBox::Ptr editBox;
        if (widget)
            editBox = std::static_pointer_cast<EditBox>(widget);
        else
            editBox = std::make_shared<EditBox>();

        loadWidget(node, editBox);

        if (node->propertyValuePairs["Text"])
            editBox->setText(DESERIALIZE_STRING("Text"));
        if (node->propertyValuePairs["DefaultText"])
            editBox->setDefaultText(DESERIALIZE_STRING("DefaultText"));
        if (node->propertyValuePairs["TextSize"])
            editBox->setTextSize(tgui::stoi(node->propertyValuePairs["TextSize"]->value));
        if (node->propertyValuePairs["MaximumCharacters"])
            editBox->setMaximumCharacters(tgui::stoi(node->propertyValuePairs["MaximumCharacters"]->value));
        if (node->propertyValuePairs["TextWidthLimited"])
            editBox->limitTextWidth(parseBoolean(node->propertyValuePairs["TextWidthLimited"]->value));
        if (node->propertyValuePairs["CaretWidth"])
            editBox->setCaretWidth(tgui::stof(node->propertyValuePairs["CaretWidth"]->value));
        if (node->propertyValuePairs["NumbersOnly"])
            editBox->setNumbersOnly(parseBoolean(node->propertyValuePairs["NumbersOnly"]->value));
        if (node->propertyValuePairs["PasswordCharacter"])
        {
            std::string pass = DESERIALIZE_STRING("PasswordCharacter");
            if (!pass.empty())
                editBox->setPasswordCharacter(pass[0]);
        }
        if (node->propertyValuePairs["Alignment"])
        {
            if (toLower(node->propertyValuePairs["Alignment"]->value) == "left")
                editBox->setAlignment(EditBox::Alignment::Left);
            else if (toLower(node->propertyValuePairs["Alignment"]->value) == "center")
                editBox->setAlignment(EditBox::Alignment::Center);
            else if (toLower(node->propertyValuePairs["Alignment"]->value) == "right")
                editBox->setAlignment(EditBox::Alignment::Right);
            else
                throw Exception{"Failed to parse Alignment property. Only the values Left, Center and Right are correct."};
        }

        return editBox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadKnob(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        Knob::Ptr knob;
        if (widget)
            knob = std::static_pointer_cast<Knob>(widget);
        else
            knob = std::make_shared<Knob>();

        loadWidget(node, knob);
        if (node->propertyValuePairs["StartRotation"])
            knob->setStartRotation(tgui::stof(node->propertyValuePairs["StartRotation"]->value));
        if (node->propertyValuePairs["EndRotation"])
            knob->setEndRotation(tgui::stof(node->propertyValuePairs["EndRotation"]->value));
        if (node->propertyValuePairs["Minimum"])
            knob->setMinimum(tgui::stoi(node->propertyValuePairs["Minimum"]->value));
        if (node->propertyValuePairs["Maximum"])
            knob->setMaximum(tgui::stoi(node->propertyValuePairs["Maximum"]->value));
        if (node->propertyValuePairs["Value"])
            knob->setValue(tgui::stoi(node->propertyValuePairs["Value"]->value));
        if (node->propertyValuePairs["ClockwiseTurning"])
            knob->setClockwiseTurning(parseBoolean(node->propertyValuePairs["ClockwiseTurning"]->value));

        return knob;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadLabel(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        Label::Ptr label;
        if (widget)
            label = std::static_pointer_cast<Label>(widget);
        else
            label = std::make_shared<Label>();

        loadWidget(node, label);

        if (node->propertyValuePairs["TextStyle"])
        {
            sf::Uint32 style = sf::Text::Regular;
            std::vector<std::string> styles = tgui::split(node->propertyValuePairs["TextStyle"]->value, '|');
            for (auto& elem : styles)
            {
                std::string requestedStyle = toLower(trim(elem));
                if (requestedStyle == "bold")
                    style |= sf::Text::Bold;
                else if (requestedStyle == "italic")
                    style |= sf::Text::Italic;
                else if (requestedStyle == "underlined")
                    style |= sf::Text::Underlined;
                else if (requestedStyle == toLower("StrikeThrough"))
                    style |= sf::Text::StrikeThrough;
                else if (requestedStyle != "regular")
                    throw Exception{"Failed to parse TextStyle property, found unknown style."};
            }

            label->setTextStyle(style);
        }

        if (node->propertyValuePairs["Text"])
            label->setText(DESERIALIZE_STRING("Text"));
        if (node->propertyValuePairs["TextSize"])
            label->setTextSize(tgui::stoi(node->propertyValuePairs["TextSize"]->value));
        if (node->propertyValuePairs["MaximumTextWidth"])
            label->setMaximumTextWidth(tgui::stof(node->propertyValuePairs["MaximumTextWidth"]->value));
        if (node->propertyValuePairs["AutoSize"])
            label->setAutoSize(parseBoolean(node->propertyValuePairs["AutoSize"]->value));

        return label;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadListBox(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        ListBox::Ptr listBox;
        if (widget)
            listBox = std::static_pointer_cast<ListBox>(widget);
        else
            listBox = std::make_shared<ListBox>();

        loadWidget(node, listBox);

        if (node->propertyValuePairs["Items"])
        {
            if (!node->propertyValuePairs["Items"]->listNode)
                throw Exception{"Failed to parse 'Items' property, expected a list as value"};

            if (node->propertyValuePairs["ItemIds"])
            {
                if (!node->propertyValuePairs["ItemIds"]->listNode)
                    throw Exception{"Failed to parse 'ItemIds' property, expected a list as value"};

                if (node->propertyValuePairs["Items"]->valueList.size() != node->propertyValuePairs["ItemIds"]->valueList.size())
                    throw Exception{"Amounts of values for 'Items' differs from the amount in 'ItemIds'"};

                for (unsigned int i = 0; i < node->propertyValuePairs["Items"]->valueList.size(); ++i)
                {
                    listBox->addItem(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["Items"]->valueList[i]).getString(),
                                     Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["ItemIds"]->valueList[i]).getString());
                }
            }
            else // There are no item ids
            {
                for (auto& item : node->propertyValuePairs["Items"]->valueList)
                    listBox->addItem(item);
            }
        }
        else // If there are no items, there should be no item ids
        {
            if (node->propertyValuePairs["ItemIds"])
            {
                if (!node->propertyValuePairs["ItemIds"]->listNode)
                    throw Exception{"Failed to parse 'ItemIds' property, expected a list as value"};

                if (!node->propertyValuePairs["ItemIds"]->valueList.empty())
                    throw Exception{"Found 'ItemIds' property while there is no 'Items' property"};
            }
        }

        if (node->propertyValuePairs["ItemHeight"])
            listBox->setItemHeight(tgui::stoi(node->propertyValuePairs["ItemHeight"]->value));
        if (node->propertyValuePairs["MaximumItems"])
            listBox->setMaximumItems(tgui::stoi(node->propertyValuePairs["MaximumItems"]->value));

        for (auto& childNode : node->children)
        {
            if (toLower(childNode->name) == "scrollbar")
                listBox->setScrollbar(std::static_pointer_cast<Scrollbar>(WidgetLoader::getLoadFunction("scrollbar")(childNode)));
        }

        return listBox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadPanel(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        if (widget)
            return loadContainer(node, std::static_pointer_cast<Panel>(widget));
        else
            return loadContainer(node, std::make_shared<Panel>());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadPicture(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        Picture::Ptr picture;
        if (widget)
            picture = std::static_pointer_cast<Picture>(widget);
        else
            picture = std::make_shared<Picture>();

        loadWidget(node, picture);
        if (node->propertyValuePairs["Filename"])
            picture = std::make_shared<Picture>(DESERIALIZE_STRING("Filename"));
        if (node->propertyValuePairs["Smooth"])
            picture->setSmooth(parseBoolean(node->propertyValuePairs["Smooth"]->value));

        return picture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadProgressBar(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        ProgressBar::Ptr progressBar;
        if (widget)
            progressBar = std::static_pointer_cast<ProgressBar>(widget);
        else
            progressBar = std::make_shared<ProgressBar>();

        loadWidget(node, progressBar);
        if (node->propertyValuePairs["Minimum"])
            progressBar->setMinimum(tgui::stoi(node->propertyValuePairs["Minimum"]->value));
        if (node->propertyValuePairs["Maximum"])
            progressBar->setMaximum(tgui::stoi(node->propertyValuePairs["Maximum"]->value));
        if (node->propertyValuePairs["Value"])
            progressBar->setValue(tgui::stoi(node->propertyValuePairs["Value"]->value));
        if (node->propertyValuePairs["Text"])
            progressBar->setText(DESERIALIZE_STRING("Text"));
        if (node->propertyValuePairs["TextSize"])
            progressBar->setTextSize(tgui::stoi(node->propertyValuePairs["TextSize"]->value));

        if (node->propertyValuePairs["FillDirection"])
        {
            std::string requestedStyle = toLower(trim(node->propertyValuePairs["FillDirection"]->value));
            if (requestedStyle == toLower("LeftToRight"))
                progressBar->setFillDirection(tgui::ProgressBar::FillDirection::LeftToRight);
            else if (requestedStyle == toLower("RightToLeft"))
                progressBar->setFillDirection(tgui::ProgressBar::FillDirection::RightToLeft);
            else if (requestedStyle == toLower("TopToBottom"))
                progressBar->setFillDirection(tgui::ProgressBar::FillDirection::TopToBottom);
            else if (requestedStyle == toLower("TopToBottom"))
                progressBar->setFillDirection(tgui::ProgressBar::FillDirection::TopToBottom);
            else
                throw Exception{"Failed to parse FillDirection property, found unknown value."};
        }

        return progressBar;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadRadioButton(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        RadioButton::Ptr radioButton;
        if (widget)
            radioButton = std::static_pointer_cast<RadioButton>(widget);
        else
            radioButton = std::make_shared<RadioButton>();

        loadWidget(node, radioButton);
        if (node->propertyValuePairs["Text"])
            radioButton->setText(DESERIALIZE_STRING("Text"));
        if (node->propertyValuePairs["TextSize"])
            radioButton->setTextSize(tgui::stoi(node->propertyValuePairs["TextSize"]->value));
        if (node->propertyValuePairs["Checked"])
        {
            if (parseBoolean(node->propertyValuePairs["Checked"]->value))
                radioButton->check();
        }

        return radioButton;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadScrollbar(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        Scrollbar::Ptr scrollbar;
        if (widget)
            scrollbar = std::static_pointer_cast<Scrollbar>(widget);
        else
            scrollbar = std::make_shared<Scrollbar>();

        loadWidget(node, scrollbar);
        if (node->propertyValuePairs["LowValue"])
            scrollbar->setLowValue(tgui::stoi(node->propertyValuePairs["LowValue"]->value));
        if (node->propertyValuePairs["Maximum"])
            scrollbar->setMaximum(tgui::stoi(node->propertyValuePairs["Maximum"]->value));
        if (node->propertyValuePairs["Value"])
            scrollbar->setValue(tgui::stoi(node->propertyValuePairs["Value"]->value));
        if (node->propertyValuePairs["ArrowScrollAmount"])
            scrollbar->setArrowScrollAmount(tgui::stoi(node->propertyValuePairs["ArrowScrollAmount"]->value));
        if (node->propertyValuePairs["AutoHide"])
            scrollbar->setAutoHide(parseBoolean(node->propertyValuePairs["AutoHide"]->value));

        return scrollbar;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadSlider(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        Slider::Ptr slider;
        if (widget)
            slider = std::static_pointer_cast<Slider>(widget);
        else
            slider = std::make_shared<Slider>();

        loadWidget(node, slider);
        if (node->propertyValuePairs["Minimum"])
            slider->setMinimum(tgui::stoi(node->propertyValuePairs["Minimum"]->value));
        if (node->propertyValuePairs["Maximum"])
            slider->setMaximum(tgui::stoi(node->propertyValuePairs["Maximum"]->value));
        if (node->propertyValuePairs["Value"])
            slider->setValue(tgui::stoi(node->propertyValuePairs["Value"]->value));

        return slider;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadSpinButton(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        SpinButton::Ptr spinButton;
        if (widget)
            spinButton = std::static_pointer_cast<SpinButton>(widget);
        else
            spinButton = std::make_shared<SpinButton>();

        loadWidget(node, spinButton);
        if (node->propertyValuePairs["Minimum"])
            spinButton->setMinimum(tgui::stoi(node->propertyValuePairs["Minimum"]->value));
        if (node->propertyValuePairs["Maximum"])
            spinButton->setMaximum(tgui::stoi(node->propertyValuePairs["Maximum"]->value));
        if (node->propertyValuePairs["Value"])
            spinButton->setValue(tgui::stoi(node->propertyValuePairs["Value"]->value));
        if (node->propertyValuePairs["VerticalScroll"])
            spinButton->setVerticalScroll(parseBoolean(node->propertyValuePairs["VerticalScroll"]->value));

        return spinButton;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadTab(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        Tab::Ptr tab;
        if (widget)
            tab = std::static_pointer_cast<Tab>(widget);
        else
            tab = std::make_shared<Tab>();

        loadWidget(node, tab);

        if (node->propertyValuePairs["Tabs"])
        {
            if (!node->propertyValuePairs["Tabs"]->listNode)
                throw Exception{"Failed to parse 'Tabs' property, expected a list as value"};

            for (auto& tabText : node->propertyValuePairs["Tabs"]->valueList)
                tab->add(Deserializer::deserialize(ObjectConverter::Type::String, tabText).getString());
        }

        if (node->propertyValuePairs["MaximumTabWidth"])
            tab->setMaximumTabWidth(tgui::stoi(node->propertyValuePairs["MaximumTabWidth"]->value));
        if (node->propertyValuePairs["TextSize"])
            tab->setTextSize(tgui::stoi(node->propertyValuePairs["TextSize"]->value));
        if (node->propertyValuePairs["TabHeight"])
            tab->setTabHeight(tgui::stoi(node->propertyValuePairs["TabHeight"]->value));
        if (node->propertyValuePairs["Selected"])
            tab->select(tgui::stoi(node->propertyValuePairs["Selected"]->value));

        return tab;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadTextBox(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        TextBox::Ptr textBox;
        if (widget)
            textBox = std::static_pointer_cast<TextBox>(widget);
        else
            textBox = std::make_shared<TextBox>();

        loadWidget(node, textBox);

        if (node->propertyValuePairs["Text"])
            textBox->setText(DESERIALIZE_STRING("Text"));
        if (node->propertyValuePairs["TextSize"])
            textBox->setTextSize(tgui::stoi(node->propertyValuePairs["TextSize"]->value));
        if (node->propertyValuePairs["MaximumCharacters"])
            textBox->setMaximumCharacters(tgui::stoi(node->propertyValuePairs["MaximumCharacters"]->value));
        if (node->propertyValuePairs["ReadOnly"])
            textBox->setReadOnly(parseBoolean(node->propertyValuePairs["ReadOnly"]->value));

        for (auto& childNode : node->children)
        {
            if (toLower(childNode->name) == "scrollbar")
                textBox->setScrollbar(std::static_pointer_cast<Scrollbar>(WidgetLoader::getLoadFunction("scrollbar")(childNode)));
        }

        return textBox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadTooltip(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        Tooltip::Ptr tooltip;
        if (widget)
            tooltip = std::static_pointer_cast<Tooltip>(widget);
        else
            tooltip = std::make_shared<Tooltip>();

        loadLabel(node, tooltip);
        if (node->propertyValuePairs["TimeToDisplay"])
            tooltip->setTimeToDisplay(sf::seconds(tgui::stof(node->propertyValuePairs["TimeToDisplay"]->value)));

        return tooltip;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, WidgetLoader::LoadFunction> WidgetLoader::m_loadFunctions =
        {
            {toLower("Widget"), std::bind(loadWidget, std::placeholders::_1, nullptr)},
            {toLower("Container"), std::bind(loadContainer, std::placeholders::_1, nullptr)},
            {toLower("Button"), std::bind(loadButton, std::placeholders::_1, nullptr)},
            {toLower("Canvas"), std::bind(loadCanvas, std::placeholders::_1, nullptr)},
            {toLower("CheckBox"), std::bind(loadCheckbox, std::placeholders::_1, nullptr)},
            {toLower("ChildWindow"), std::bind(loadChildWindow, std::placeholders::_1, nullptr)},
            {toLower("ClickableWidget"), std::bind(loadClickableWidget, std::placeholders::_1, nullptr)},
            {toLower("ComboBox"), std::bind(loadComboBox, std::placeholders::_1, nullptr)},
            {toLower("EditBox"), std::bind(loadEditBox, std::placeholders::_1, nullptr)},
            {toLower("Knob"), std::bind(loadKnob, std::placeholders::_1, nullptr)},
            {toLower("ListBox"), std::bind(loadListBox, std::placeholders::_1, nullptr)},
            {toLower("Label"), std::bind(loadLabel, std::placeholders::_1, nullptr)},
            {toLower("Panel"), std::bind(loadPanel, std::placeholders::_1, nullptr)},
            {toLower("Picture"), std::bind(loadPicture, std::placeholders::_1, nullptr)},
            {toLower("ProgressBar"), std::bind(loadProgressBar, std::placeholders::_1, nullptr)},
            {toLower("RadioButton"), std::bind(loadRadioButton, std::placeholders::_1, nullptr)},
            {toLower("Scrollbar"), std::bind(loadScrollbar, std::placeholders::_1, nullptr)},
            {toLower("Slider"), std::bind(loadSlider, std::placeholders::_1, nullptr)},
            {toLower("SpinButton"), std::bind(loadSpinButton, std::placeholders::_1, nullptr)},
            {toLower("Tab"), std::bind(loadTab, std::placeholders::_1, nullptr)},
            {toLower("TextBox"), std::bind(loadTextBox, std::placeholders::_1, nullptr)},
            {toLower("ToolTip"), std::bind(loadTooltip, std::placeholders::_1, nullptr)}
        };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetLoader::load(Container::Ptr parent, std::stringstream& stream)
    {
        auto rootNode = DataIO::parse(stream);

        if (rootNode->propertyValuePairs.size() != 0)
            loadWidget(rootNode, parent);

        for (auto& node : rootNode->children)
        {
            auto nameSeparator = node->name.find('.');
            auto widgetType = node->name.substr(0, nameSeparator);
            auto& loadFunction = m_loadFunctions[toLower(widgetType)];
            if (loadFunction)
            {
                std::string className;
                if (nameSeparator != std::string::npos)
                {
                    if ((node->name.size() >= nameSeparator + 2) && (node->name[nameSeparator+1] == '"') && (node->name.back() == '"'))
                        className = Deserializer::deserialize(ObjectConverter::Type::String, node->name.substr(nameSeparator + 1)).getString();
                    else
                        className = node->name.substr(nameSeparator + 1);
                }

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
