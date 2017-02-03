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


#include <TGUI/Loading/Deserializer.hpp>
#include <TGUI/Loading/WidgetLoader.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Canvas.hpp>
#include <TGUI/Widgets/ChatBox.hpp>
#include <TGUI/Widgets/CheckBox.hpp>
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

    tgui::Layout2d parseLayout(std::string str)
    {
        if (str.empty())
            throw tgui::Exception{"Failed to parse layout. String was empty."};

        // Check if the layout is an (x, y) vector or a quoted string
        if ((str.front() == '(') && (str.back() == ')'))
        {
            str = str.substr(1, str.length() - 2);
            if (str.empty())
                return {0, 0};

            tgui::Layout x;
            tgui::Layout y;

            auto commaPos = str.find(',');
            if (commaPos == std::string::npos)
                throw tgui::Exception{"Failed to parse layout '" + str + "'. Expected numbers separated with a comma."};

            // Check if the first part is quoted
            auto openingQuotePos = str.find('"');
            if (commaPos > openingQuotePos)
            {
                auto closingQuotePos = str.find('"', openingQuotePos + 1);
                if (closingQuotePos == std::string::npos)
                    throw tgui::Exception{"Failed to parse layout '" + str + "'. Expected closing quote."};

                // Make sure we didn't select a quote inside the string
                if (commaPos < closingQuotePos)
                {
                    commaPos = str.find(',', closingQuotePos + 1);
                    if (commaPos == std::string::npos)
                        throw tgui::Exception{"Failed to parse layout '" + str + "'. Expected numbers separated with a comma."};
                }

                x = {str.substr(openingQuotePos + 1, closingQuotePos - openingQuotePos - 1)};
            }
            else // Normal value
                x = {tgui::stof(tgui::trim(str.substr(0, commaPos)))};

            // Check if the second part is quoted
            openingQuotePos = str.find('"', commaPos + 1);
            if (openingQuotePos != std::string::npos)
            {
                auto closingQuotePos = str.find('"', openingQuotePos + 1);
                if (closingQuotePos == std::string::npos)
                    throw tgui::Exception{"Failed to parse layout '" + str + "'. Expected closing quote."};

                y = {str.substr(openingQuotePos + 1, closingQuotePos - openingQuotePos - 1)};
            }
            else // Normal value
                y = {tgui::stof(tgui::trim(str.substr(commaPos + 1)))};

            return {x, y};
        }
        else if ((str.front() == '"') && (str.back() == '"'))
        {
            str = str.substr(1, str.length() - 2);
            return {str};
        }
        else
            throw tgui::Exception{"Failed to parse layout '" + str + "'. Expected (x,y) or a quoted layout string."};
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Hidden functions
namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    #define DESERIALIZE_STRING(property) Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[property]->value).getString()

    #define REMOVE_CHILD(childName) node->children.erase(std::remove_if(node->children.begin(), node->children.end(), \
                                        [](std::shared_ptr<DataIO::Node> child){ return toLower(child->name) == childName; }), node->children.end());

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadWidget(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
    {
        assert(widget != nullptr);

        if (node->propertyValuePairs["visible"])
        {
            bool visible = parseBoolean(node->propertyValuePairs["visible"]->value);
            if (visible)
                widget->show();
            else
                widget->hide();
        }
        if (node->propertyValuePairs["enabled"])
        {
            bool enabled = parseBoolean(node->propertyValuePairs["enabled"]->value);
            if (enabled)
                widget->enable();
            else
                widget->disable();
        }
        if (node->propertyValuePairs["position"])
            widget->setPosition(parseLayout(node->propertyValuePairs["position"]->value));
        if (node->propertyValuePairs["size"])
            widget->setSize(parseLayout(node->propertyValuePairs["size"]->value));
        if (node->propertyValuePairs["opacity"])
            widget->setOpacity(tgui::stof(node->propertyValuePairs["opacity"]->value));

        /// TODO: Font and ToolTip (and Theme?)

        for (auto& childNode : node->children)
        {
            if (toLower(childNode->name) == "renderer")
            {
                for (auto& pair : childNode->propertyValuePairs)
                    widget->getRenderer()->setProperty(pair.first, pair.second->value);
            }
        }
        REMOVE_CHILD("renderer");

        return widget;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadContainer(std::shared_ptr<DataIO::Node> node, Container::Ptr container)
    {
        assert(container != nullptr);
        loadWidget(node, container);

        for (auto& childNode : node->children)
        {
            auto nameSeparator = childNode->name.find('.');
            auto widgetType = childNode->name.substr(0, nameSeparator);
            auto& loadFunction = WidgetLoader::getLoadFunction(toLower(widgetType));
            if (loadFunction)
            {
                std::string className;
                if (nameSeparator != std::string::npos)
                    className = Deserializer::deserialize(ObjectConverter::Type::String, childNode->name.substr(nameSeparator + 1)).getString();

                tgui::Widget::Ptr childWidget = loadFunction(childNode);
                container->add(childWidget, className);
            }
            else
                throw Exception{"No load function exists for widget type '" + widgetType + "'."};
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
        if (node->propertyValuePairs["text"])
            button->setText(DESERIALIZE_STRING("text"));
        if (node->propertyValuePairs["textsize"])
            button->setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));

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

    TGUI_API Widget::Ptr loadChatBox(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        ChatBox::Ptr chatBox;
        if (widget)
            chatBox = std::static_pointer_cast<ChatBox>(widget);
        else
            chatBox = std::make_shared<ChatBox>();

        loadWidget(node, chatBox);

        if (node->propertyValuePairs["textsize"])
            chatBox->setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["textcolor"])
            chatBox->setTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, node->propertyValuePairs["textcolor"]->value).getColor());
        if (node->propertyValuePairs["linelimit"])
            chatBox->setLineLimit(tgui::stoi(node->propertyValuePairs["linelimit"]->value));

        for (auto& childNode : node->children)
        {
            if (toLower(childNode->name) == "scrollbar")
                chatBox->setScrollbar(std::static_pointer_cast<Scrollbar>(WidgetLoader::getLoadFunction("scrollbar")(childNode)));
        }
        REMOVE_CHILD("scrollbar");

        for (auto& childNode : node->children)
        {
            if (toLower(childNode->name) == "line")
            {
                unsigned int lineTextSize = chatBox->getTextSize();
                sf::Color lineTextColor = chatBox->getTextColor();

                if (childNode->propertyValuePairs["textsize"])
                    lineTextSize = tgui::stoi(childNode->propertyValuePairs["textsize"]->value);
                if (childNode->propertyValuePairs["color"])
                    lineTextColor = Deserializer::deserialize(ObjectConverter::Type::Color, childNode->propertyValuePairs["color"]->value).getColor();

                if (childNode->propertyValuePairs["text"])
                {
                    chatBox->addLine(Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs["text"]->value).getString(),
                                     lineTextColor,
                                     lineTextSize);
                }
            }
        }
        REMOVE_CHILD("line");

        if (node->propertyValuePairs["linesstartfromtop"])
            chatBox->setLinesStartFromTop(parseBoolean(node->propertyValuePairs["linesstartfromtop"]->value));

        // This has to be parsed after the lines have been added
        if (node->propertyValuePairs["newlinesbelowothers"])
            chatBox->setNewLinesBelowOthers(parseBoolean(node->propertyValuePairs["newlinesbelowothers"]->value));

        return chatBox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API Widget::Ptr loadCheckBox(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget = nullptr)
    {
        CheckBox::Ptr checkbox;
        if (widget)
            checkbox = std::static_pointer_cast<CheckBox>(widget);
        else
            checkbox = std::make_shared<CheckBox>();

        loadWidget(node, checkbox);
        if (node->propertyValuePairs["text"])
            checkbox->setText(DESERIALIZE_STRING("text"));
        if (node->propertyValuePairs["textsize"])
            checkbox->setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["checked"])
        {
            if (parseBoolean(node->propertyValuePairs["checked"]->value))
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

        if (node->propertyValuePairs["titlealignment"])
        {
            if (toLower(node->propertyValuePairs["titlealignment"]->value) == "left")
                childWindow->setTitleAlignment(ChildWindow::TitleAlignment::Left);
            else if (toLower(node->propertyValuePairs["titlealignment"]->value) == "center")
                childWindow->setTitleAlignment(ChildWindow::TitleAlignment::Center);
            else if (toLower(node->propertyValuePairs["titlealignment"]->value) == "right")
                childWindow->setTitleAlignment(ChildWindow::TitleAlignment::Right);
            else
                throw Exception{"Failed to parse TitleAlignment property. Only the values Left, Center and Right are correct."};
        }

        if (node->propertyValuePairs["title"])
            childWindow->setTitle(DESERIALIZE_STRING("title"));

        if (node->propertyValuePairs["icon"])
            childWindow->setIcon(Deserializer::deserialize(ObjectConverter::Type::Texture, node->propertyValuePairs["icon"]->value).getTexture());

        if (node->propertyValuePairs["keepinparent"])
            childWindow->keepInParent(parseBoolean(node->propertyValuePairs["keepinparent"]->value));

        for (auto& childNode : node->children)
        {
            if (toLower(childNode->name) == "closebutton")
                childWindow->setCloseButton(std::static_pointer_cast<Button>(WidgetLoader::getLoadFunction("button")(childNode)));
        }
        REMOVE_CHILD("closebutton");

        loadContainer(node, childWindow);

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
        REMOVE_CHILD("listbox");

        loadWidget(node, comboBox);

        if (node->propertyValuePairs["itemstodisplay"])
            comboBox->setItemsToDisplay(tgui::stoi(node->propertyValuePairs["itemstodisplay"]->value));

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

        if (node->propertyValuePairs["text"])
            editBox->setText(DESERIALIZE_STRING("text"));
        if (node->propertyValuePairs["defaulttext"])
            editBox->setDefaultText(DESERIALIZE_STRING("defaulttext"));
        if (node->propertyValuePairs["textsize"])
            editBox->setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["maximumcharacters"])
            editBox->setMaximumCharacters(tgui::stoi(node->propertyValuePairs["maximumcharacters"]->value));
        if (node->propertyValuePairs["textwidthlimited"])
            editBox->limitTextWidth(parseBoolean(node->propertyValuePairs["textwidthlimited"]->value));
        if (node->propertyValuePairs["caretwidth"])
            editBox->setCaretWidth(tgui::stof(node->propertyValuePairs["caretwidth"]->value));
        if (node->propertyValuePairs["passwordcharacter"])
        {
            std::string pass = DESERIALIZE_STRING("passwordcharacter");
            if (!pass.empty())
                editBox->setPasswordCharacter(pass[0]);
        }
        if (node->propertyValuePairs["alignment"])
        {
            if (toLower(node->propertyValuePairs["alignment"]->value) == "left")
                editBox->setAlignment(EditBox::Alignment::Left);
            else if (toLower(node->propertyValuePairs["alignment"]->value) == "center")
                editBox->setAlignment(EditBox::Alignment::Center);
            else if (toLower(node->propertyValuePairs["alignment"]->value) == "right")
                editBox->setAlignment(EditBox::Alignment::Right);
            else
                throw Exception{"Failed to parse Alignment property. Only the values Left, Center and Right are correct."};
        }
        if (node->propertyValuePairs["inputvalidator"])
        {
            if (toLower(node->propertyValuePairs["inputvalidator"]->value) == "int")
                editBox->setInputValidator(EditBox::Validator::Int);
            else if (toLower(node->propertyValuePairs["inputvalidator"]->value) == "uint")
                editBox->setInputValidator(EditBox::Validator::UInt);
            else if (toLower(node->propertyValuePairs["inputvalidator"]->value) == "float")
                editBox->setInputValidator(EditBox::Validator::Float);
            else
                editBox->setInputValidator(DESERIALIZE_STRING("inputvalidator"));
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
        if (node->propertyValuePairs["startrotation"])
            knob->setStartRotation(tgui::stof(node->propertyValuePairs["startrotation"]->value));
        if (node->propertyValuePairs["endrotation"])
            knob->setEndRotation(tgui::stof(node->propertyValuePairs["endrotation"]->value));
        if (node->propertyValuePairs["minimum"])
            knob->setMinimum(tgui::stoi(node->propertyValuePairs["minimum"]->value));
        if (node->propertyValuePairs["maximum"])
            knob->setMaximum(tgui::stoi(node->propertyValuePairs["maximum"]->value));
        if (node->propertyValuePairs["value"])
            knob->setValue(tgui::stoi(node->propertyValuePairs["value"]->value));
        if (node->propertyValuePairs["clockwiseturning"])
            knob->setClockwiseTurning(parseBoolean(node->propertyValuePairs["clockwiseturning"]->value));

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

        if (node->propertyValuePairs["textstyle"])
        {
            sf::Uint32 style = sf::Text::Regular;
            std::vector<std::string> styles = tgui::split(node->propertyValuePairs["textstyle"]->value, '|');
            for (auto& elem : styles)
            {
                std::string requestedStyle = toLower(trim(elem));
                if (requestedStyle == "bold")
                    style |= sf::Text::Bold;
                else if (requestedStyle == "italic")
                    style |= sf::Text::Italic;
                else if (requestedStyle == "underlined")
                    style |= sf::Text::Underlined;
                else if (requestedStyle == "strikethrough")
                    style |= sf::Text::StrikeThrough;
                else if (requestedStyle != "regular")
                    throw Exception{"Failed to parse TextStyle property, found unknown style."};
            }

            label->setTextStyle(style);
        }

        if (node->propertyValuePairs["horizontalalignment"])
        {
            std::string alignment = toLower(DESERIALIZE_STRING("horizontalalignment"));
            if (alignment == "right")
                label->setHorizontalAlignment(Label::HorizontalAlignment::Right);
            else if (alignment == "center")
                label->setHorizontalAlignment(Label::HorizontalAlignment::Center);
            else if (alignment != "left")
                throw Exception{"Failed to parse HorizontalAlignment property, found unknown value."};
        }

        if (node->propertyValuePairs["verticalalignment"])
        {
            std::string alignment = toLower(DESERIALIZE_STRING("verticalalignment"));
            if (alignment == "bottom")
                label->setVerticalAlignment(Label::VerticalAlignment::Bottom);
            else if (alignment == "center")
                label->setVerticalAlignment(Label::VerticalAlignment::Center);
            else if (alignment != "top")
                throw Exception{"Failed to parse VerticalAlignment property, found unknown value."};
        }

        if (node->propertyValuePairs["text"])
            label->setText(DESERIALIZE_STRING("text"));
        if (node->propertyValuePairs["textsize"])
            label->setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["maximumtextwidth"])
            label->setMaximumTextWidth(tgui::stof(node->propertyValuePairs["maximumtextwidth"]->value));
        if (node->propertyValuePairs["autosize"])
            label->setAutoSize(parseBoolean(node->propertyValuePairs["autosize"]->value));

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

        if (node->propertyValuePairs["items"])
        {
            if (!node->propertyValuePairs["items"]->listNode)
                throw Exception{"Failed to parse 'Items' property, expected a list as value"};

            if (node->propertyValuePairs["itemids"])
            {
                if (!node->propertyValuePairs["itemids"]->listNode)
                    throw Exception{"Failed to parse 'ItemIds' property, expected a list as value"};

                if (node->propertyValuePairs["items"]->valueList.size() != node->propertyValuePairs["itemids"]->valueList.size())
                    throw Exception{"Amounts of values for 'Items' differs from the amount in 'ItemIds'"};

                for (std::size_t i = 0; i < node->propertyValuePairs["items"]->valueList.size(); ++i)
                {
                    listBox->addItem(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["items"]->valueList[i]).getString(),
                                     Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["itemids"]->valueList[i]).getString());
                }
            }
            else // There are no item ids
            {
                for (auto& item : node->propertyValuePairs["items"]->valueList)
                    listBox->addItem(item);
            }
        }
        else // If there are no items, there should be no item ids
        {
            if (node->propertyValuePairs["itemids"])
            {
                if (!node->propertyValuePairs["itemids"]->listNode)
                    throw Exception{"Failed to parse 'ItemIds' property, expected a list as value"};

                if (!node->propertyValuePairs["itemids"]->valueList.empty())
                    throw Exception{"Found 'ItemIds' property while there is no 'Items' property"};
            }
        }

        if (node->propertyValuePairs["itemheight"])
            listBox->setItemHeight(tgui::stoi(node->propertyValuePairs["itemheight"]->value));
        if (node->propertyValuePairs["maximumitems"])
            listBox->setMaximumItems(tgui::stoi(node->propertyValuePairs["maximumitems"]->value));

        for (auto& childNode : node->children)
        {
            if (toLower(childNode->name) == "scrollbar")
                listBox->setScrollbar(std::static_pointer_cast<Scrollbar>(WidgetLoader::getLoadFunction("scrollbar")(childNode)));
        }
        REMOVE_CHILD("scrollbar");

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

        if (node->propertyValuePairs["filename"])
            picture = std::make_shared<Picture>(DESERIALIZE_STRING("filename"));

        loadWidget(node, picture);

        if (node->propertyValuePairs["smooth"])
            picture->setSmooth(parseBoolean(node->propertyValuePairs["smooth"]->value));

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
        if (node->propertyValuePairs["minimum"])
            progressBar->setMinimum(tgui::stoi(node->propertyValuePairs["minimum"]->value));
        if (node->propertyValuePairs["maximum"])
            progressBar->setMaximum(tgui::stoi(node->propertyValuePairs["maximum"]->value));
        if (node->propertyValuePairs["value"])
            progressBar->setValue(tgui::stoi(node->propertyValuePairs["value"]->value));
        if (node->propertyValuePairs["text"])
            progressBar->setText(DESERIALIZE_STRING("text"));
        if (node->propertyValuePairs["textsize"])
            progressBar->setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));

        if (node->propertyValuePairs["filldirection"])
        {
            std::string requestedStyle = toLower(trim(node->propertyValuePairs["filldirection"]->value));
            if (requestedStyle == "lefttoright")
                progressBar->setFillDirection(tgui::ProgressBar::FillDirection::LeftToRight);
            else if (requestedStyle == "righttoleft")
                progressBar->setFillDirection(tgui::ProgressBar::FillDirection::RightToLeft);
            else if (requestedStyle == "toptobottom")
                progressBar->setFillDirection(tgui::ProgressBar::FillDirection::TopToBottom);
            else if (requestedStyle == "toptobottom")
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
        if (node->propertyValuePairs["text"])
            radioButton->setText(DESERIALIZE_STRING("text"));
        if (node->propertyValuePairs["textsize"])
            radioButton->setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["checked"])
        {
            if (parseBoolean(node->propertyValuePairs["checked"]->value))
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
        if (node->propertyValuePairs["lowvalue"])
            scrollbar->setLowValue(tgui::stoi(node->propertyValuePairs["lowvalue"]->value));
        if (node->propertyValuePairs["maximum"])
            scrollbar->setMaximum(tgui::stoi(node->propertyValuePairs["maximum"]->value));
        if (node->propertyValuePairs["value"])
            scrollbar->setValue(tgui::stoi(node->propertyValuePairs["value"]->value));
        if (node->propertyValuePairs["arrowscrollamount"])
            scrollbar->setArrowScrollAmount(tgui::stoi(node->propertyValuePairs["arrowscrollamount"]->value));
        if (node->propertyValuePairs["autohide"])
            scrollbar->setAutoHide(parseBoolean(node->propertyValuePairs["autohide"]->value));

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
        if (node->propertyValuePairs["minimum"])
            slider->setMinimum(tgui::stoi(node->propertyValuePairs["minimum"]->value));
        if (node->propertyValuePairs["maximum"])
            slider->setMaximum(tgui::stoi(node->propertyValuePairs["maximum"]->value));
        if (node->propertyValuePairs["value"])
            slider->setValue(tgui::stoi(node->propertyValuePairs["value"]->value));

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
        if (node->propertyValuePairs["minimum"])
            spinButton->setMinimum(tgui::stoi(node->propertyValuePairs["minimum"]->value));
        if (node->propertyValuePairs["maximum"])
            spinButton->setMaximum(tgui::stoi(node->propertyValuePairs["maximum"]->value));
        if (node->propertyValuePairs["value"])
            spinButton->setValue(tgui::stoi(node->propertyValuePairs["value"]->value));
        if (node->propertyValuePairs["verticalscroll"])
            spinButton->setVerticalScroll(parseBoolean(node->propertyValuePairs["verticalscroll"]->value));

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

        if (node->propertyValuePairs["tabs"])
        {
            if (!node->propertyValuePairs["tabs"]->listNode)
                throw Exception{"Failed to parse 'Tabs' property, expected a list as value"};

            for (auto& tabText : node->propertyValuePairs["tabs"]->valueList)
                tab->add(Deserializer::deserialize(ObjectConverter::Type::String, tabText).getString());
        }

        if (node->propertyValuePairs["maximumtabwidth"])
            tab->setMaximumTabWidth(tgui::stof(node->propertyValuePairs["maximumtabwidth"]->value));
        if (node->propertyValuePairs["textsize"])
            tab->setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["tabheight"])
            tab->setTabHeight(tgui::stof(node->propertyValuePairs["tabheight"]->value));
        if (node->propertyValuePairs["selected"])
            tab->select(tgui::stoi(node->propertyValuePairs["selected"]->value));

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

        if (node->propertyValuePairs["text"])
            textBox->setText(DESERIALIZE_STRING("text"));
        if (node->propertyValuePairs["textsize"])
            textBox->setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
        if (node->propertyValuePairs["maximumcharacters"])
            textBox->setMaximumCharacters(tgui::stoi(node->propertyValuePairs["maximumcharacters"]->value));
        if (node->propertyValuePairs["readonly"])
            textBox->setReadOnly(parseBoolean(node->propertyValuePairs["readonly"]->value));

        for (auto& childNode : node->children)
        {
            if (toLower(childNode->name) == "scrollbar")
                textBox->setScrollbar(std::static_pointer_cast<Scrollbar>(WidgetLoader::getLoadFunction("scrollbar")(childNode)));
        }
        REMOVE_CHILD("scrollbar");

        return textBox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, WidgetLoader::LoadFunction> WidgetLoader::m_loadFunctions =
        {
            // Using "std::shared_ptr<Widget>{}" instead of "nullptr" to work around internal compiler error with MinGW 4.7.1 TDM
            {"widget", std::bind(loadWidget, std::placeholders::_1, std::shared_ptr<Widget>{})},
            {"container", std::bind(loadContainer, std::placeholders::_1, std::shared_ptr<Container>{})},
            {"button", std::bind(loadButton, std::placeholders::_1, std::shared_ptr<Button>{})},
            {"canvas", std::bind(loadCanvas, std::placeholders::_1, std::shared_ptr<Canvas>{})},
            {"chatbox", std::bind(loadChatBox, std::placeholders::_1, std::shared_ptr<ChatBox>{})},
            {"checkbox", std::bind(loadCheckBox, std::placeholders::_1, std::shared_ptr<CheckBox>{})},
            {"childwindow", std::bind(loadChildWindow, std::placeholders::_1, std::shared_ptr<ChildWindow>{})},
            {"clickablewidget", std::bind(loadClickableWidget, std::placeholders::_1, std::shared_ptr<ClickableWidget>{})},
            {"combobox", std::bind(loadComboBox, std::placeholders::_1, std::shared_ptr<ComboBox>{})},
            {"editbox", std::bind(loadEditBox, std::placeholders::_1, std::shared_ptr<EditBox>{})},
            {"knob", std::bind(loadKnob, std::placeholders::_1, std::shared_ptr<Knob>{})},
            {"listbox", std::bind(loadListBox, std::placeholders::_1, std::shared_ptr<ListBox>{})},
            {"label", std::bind(loadLabel, std::placeholders::_1, std::shared_ptr<Label>{})},
            {"panel", std::bind(loadPanel, std::placeholders::_1, std::shared_ptr<Panel>{})},
            {"picture", std::bind(loadPicture, std::placeholders::_1, std::shared_ptr<Picture>{})},
            {"progressbar", std::bind(loadProgressBar, std::placeholders::_1, std::shared_ptr<ProgressBar>{})},
            {"radiobutton", std::bind(loadRadioButton, std::placeholders::_1, std::shared_ptr<RadioButton>{})},
            {"scrollbar", std::bind(loadScrollbar, std::placeholders::_1, std::shared_ptr<Scrollbar>{})},
            {"slider", std::bind(loadSlider, std::placeholders::_1, std::shared_ptr<Slider>{})},
            {"spinbutton", std::bind(loadSpinButton, std::placeholders::_1, std::shared_ptr<SpinButton>{})},
            {"tab", std::bind(loadTab, std::placeholders::_1, std::shared_ptr<Tab>{})},
            {"textbox", std::bind(loadTextBox, std::placeholders::_1, std::shared_ptr<TextBox>{})}
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
                    className = Deserializer::deserialize(ObjectConverter::Type::String, node->name.substr(nameSeparator + 1)).getString();

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
