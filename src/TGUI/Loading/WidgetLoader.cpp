/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
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


#include <TGUI/ToolTip.hpp>
#include <TGUI/Loading/Deserializer.hpp>
#include <TGUI/Loading/WidgetLoader.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/Canvas.hpp>
#include <TGUI/Widgets/ChatBox.hpp>
#include <TGUI/Widgets/CheckBox.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/ComboBox.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/Group.hpp>
#include <TGUI/Widgets/Knob.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/ListBox.hpp>
#include <TGUI/Widgets/MenuBar.hpp>
#include <TGUI/Widgets/MessageBox.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <TGUI/Widgets/ProgressBar.hpp>
#include <TGUI/Widgets/RadioButton.hpp>
#include <TGUI/Widgets/RadioButtonGroup.hpp>
#include <TGUI/Widgets/Scrollbar.hpp>
#include <TGUI/Widgets/Slider.hpp>
#include <TGUI/Widgets/SpinButton.hpp>
#include <TGUI/Widgets/Tabs.hpp>
#include <TGUI/Widgets/TextBox.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    namespace
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        #define DESERIALIZE_STRING(property) Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs[property]->value).getString()

        #define REMOVE_CHILD(childName) node->children.erase(std::remove_if(node->children.begin(), node->children.end(), \
                                            [](std::shared_ptr<DataIO::Node> child){ return toLower(child->name) == childName; }), node->children.end());

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadWidget(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
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

            for (const auto& childNode : node->children)
            {
                if (toLower(childNode->name) == "tooltip")
                {
                    for (const auto& pair : childNode->propertyValuePairs)
                    {
                        if (pair.first == "timetodisplay")
                            ToolTip::setTimeToDisplay(sf::seconds(tgui::stof(pair.second->value)));
                        else if (pair.first == "distancetomouse")
                            ToolTip::setDistanceToMouse(parseLayout(pair.second->value).getValue());
                    }

                    if (!childNode->children.empty())
                    {
                        // There can only be one child in the tool tip section
                        if (childNode->children.size() > 1)
                            throw Exception{"ToolTip section contained multiple children."};

                        auto toolTipWidgetNode = childNode->children[0];
                        auto& loadFunction = WidgetLoader::getLoadFunction(toolTipWidgetNode->name);
                        if (loadFunction)
                            widget->setToolTip(loadFunction(toolTipWidgetNode, nullptr));
                        else
                            throw Exception{"No load function exists for widget type '" + toolTipWidgetNode->name + "'."};
                    }
                }

                /// TODO: Signals?

                /// TODO: Separate renderer section?
                else if (toLower(childNode->name) == "renderer")
                {
                    auto rendererData = RendererData::create();

                    for (const auto& pair : childNode->propertyValuePairs)
                        rendererData->propertyValuePairs[pair.first] = ObjectConverter(pair.second->value); // Did not compile in VS2013 when just assigning "{pair.second->value}"

                    for (const auto& nestedProperty : childNode->children)
                    {
                        std::stringstream ss;
                        DataIO::emit(nestedProperty, ss);
                        rendererData->propertyValuePairs[toLower(nestedProperty->name)] = {sf::String{"{\n" + ss.str() + "}"}};
                    }

                    widget->setRenderer(rendererData);
                }
            }
            REMOVE_CHILD("tooltip");
            REMOVE_CHILD("renderer");

            return widget;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadContainer(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            assert(widget != nullptr);
            Container::Ptr container = std::static_pointer_cast<Container>(widget);
            loadWidget(node, container);

            for (const auto& childNode : node->children)
            {
                auto nameSeparator = childNode->name.find('.');
                auto widgetType = childNode->name.substr(0, nameSeparator);
                auto& loadFunction = WidgetLoader::getLoadFunction(widgetType);
                if (loadFunction)
                {
                    std::string className;
                    if (nameSeparator != std::string::npos)
                        className = Deserializer::deserialize(ObjectConverter::Type::String, childNode->name.substr(nameSeparator + 1)).getString();

                    tgui::Widget::Ptr childWidget = loadFunction(childNode, nullptr);
                    container->add(childWidget, className);
                }
                else
                    throw Exception{"No load function exists for widget type '" + widgetType + "'."};
            }

            return container;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadButton(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            Button::Ptr button;
            if (widget)
                button = std::static_pointer_cast<Button>(widget);
            else
                button = Button::create();

            loadWidget(node, button);
            if (node->propertyValuePairs["text"])
                button->setText(DESERIALIZE_STRING("text"));
            if (node->propertyValuePairs["textsize"])
                button->setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));

            return button;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadCanvas(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            if (widget)
                return loadWidget(node, widget);
            else
                return loadWidget(node, std::make_shared<Canvas>());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadChatBox(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            ChatBox::Ptr chatBox;
            if (widget)
                chatBox = std::static_pointer_cast<ChatBox>(widget);
            else
                chatBox = ChatBox::create();

            loadWidget(node, chatBox);

            if (node->propertyValuePairs["textsize"])
                chatBox->setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
            if (node->propertyValuePairs["textcolor"])
                chatBox->setTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, node->propertyValuePairs["textcolor"]->value).getColor());
            if (node->propertyValuePairs["linelimit"])
                chatBox->setLineLimit(tgui::stoi(node->propertyValuePairs["linelimit"]->value));

            for (const auto& childNode : node->children)
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

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadCheckBox(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            CheckBox::Ptr checkbox;
            if (widget)
                checkbox = std::static_pointer_cast<CheckBox>(widget);
            else
                checkbox = CheckBox::create();

            loadWidget(node, checkbox);
            if (node->propertyValuePairs["text"])
                checkbox->setText(DESERIALIZE_STRING("text"));
            if (node->propertyValuePairs["textsize"])
                checkbox->setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
            if (node->propertyValuePairs["textclickable"])
                checkbox->setTextClickable(parseBoolean(node->propertyValuePairs["textclickable"]->value));
            if (node->propertyValuePairs["checked"])
            {
                if (parseBoolean(node->propertyValuePairs["checked"]->value))
                    checkbox->check();
            }

            return checkbox;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadChildWindow(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            ChildWindow::Ptr childWindow;
            if (widget)
                childWindow = std::static_pointer_cast<ChildWindow>(widget);
            else
                childWindow = ChildWindow::create();

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

            if (node->propertyValuePairs["titlebuttons"])
            {
                int decodedTitleButtons = ChildWindow::TitleButton::None;
                std::vector<std::string> titleButtons = Deserializer::split(node->propertyValuePairs["titlebuttons"]->value, '|');
                for (const auto& elem : titleButtons)
                {
                    std::string requestedTitleButton = toLower(trim(elem));
                    if (requestedTitleButton == "close")
                        decodedTitleButtons |= sf::Text::Bold;
                    else if (requestedTitleButton == "maximize")
                        decodedTitleButtons |= sf::Text::Italic;
                    else if (requestedTitleButton == "minimize")
                        decodedTitleButtons |= sf::Text::Underlined;
                }

                childWindow->setTitleButtons(decodedTitleButtons);
            }

            if (node->propertyValuePairs["title"])
                childWindow->setTitle(DESERIALIZE_STRING("title"));

            if (node->propertyValuePairs["keepinparent"])
                childWindow->keepInParent(parseBoolean(node->propertyValuePairs["keepinparent"]->value));

            if (node->propertyValuePairs["resizable"])
                childWindow->setResizable(parseBoolean(node->propertyValuePairs["resizable"]->value));

            loadContainer(node, childWindow);

            return childWindow;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadClickableWidget(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            if (widget)
                return loadWidget(node, widget);
            else
                return loadWidget(node, std::make_shared<ClickableWidget>());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadComboBox(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            ComboBox::Ptr comboBox;
            if (widget)
                comboBox = std::static_pointer_cast<ComboBox>(widget);
            else
                comboBox = ComboBox::create();

            loadWidget(node, comboBox);

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
                        comboBox->addItem(Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["items"]->valueList[i]).getString(),
                                          Deserializer::deserialize(ObjectConverter::Type::String, node->propertyValuePairs["itemids"]->valueList[i]).getString());
                    }
                }
                else // There are no item ids
                {
                    for (const auto& item : node->propertyValuePairs["items"]->valueList)
                        comboBox->addItem(item);
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

            if (node->propertyValuePairs["itemstodisplay"])
                comboBox->setItemsToDisplay(tgui::stoi(node->propertyValuePairs["itemstodisplay"]->value));
            if (node->propertyValuePairs["textsize"])
                comboBox->setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
            if (node->propertyValuePairs["maximumitems"])
                comboBox->setMaximumItems(tgui::stoi(node->propertyValuePairs["maximumitems"]->value));

            return comboBox;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadEditBox(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            EditBox::Ptr editBox;
            if (widget)
                editBox = std::static_pointer_cast<EditBox>(widget);
            else
                editBox = EditBox::create();

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

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadGroup(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            if (widget)
                return loadContainer(node, std::static_pointer_cast<Group>(widget));
            else
                return loadContainer(node, std::make_shared<Group>());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadKnob(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            Knob::Ptr knob;
            if (widget)
                knob = std::static_pointer_cast<Knob>(widget);
            else
                knob = Knob::create();

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

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadLabel(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            Label::Ptr label;
            if (widget)
                label = std::static_pointer_cast<Label>(widget);
            else
                label = Label::create();

            loadWidget(node, label);

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

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadListBox(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            ListBox::Ptr listBox;
            if (widget)
                listBox = std::static_pointer_cast<ListBox>(widget);
            else
                listBox = ListBox::create();

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
                    for (const auto& item : node->propertyValuePairs["items"]->valueList)
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

            if (node->propertyValuePairs["autoscroll"])
                listBox->setAutoScroll(parseBoolean(node->propertyValuePairs["autoscroll"]->value));
            if (node->propertyValuePairs["textsize"])
                listBox->setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
            if (node->propertyValuePairs["itemheight"])
                listBox->setItemHeight(tgui::stoi(node->propertyValuePairs["itemheight"]->value));
            if (node->propertyValuePairs["maximumitems"])
                listBox->setMaximumItems(tgui::stoi(node->propertyValuePairs["maximumitems"]->value));

            return listBox;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadMenuBar(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            MenuBar::Ptr menuBar;
            if (widget)
                menuBar = std::static_pointer_cast<MenuBar>(widget);
            else
                menuBar = MenuBar::create();

            loadWidget(node, menuBar);

            if (node->propertyValuePairs["textsize"])
                menuBar->setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
            if (node->propertyValuePairs["minimumsubmenuwidth"])
                menuBar->setMinimumSubMenuWidth(tgui::stof(node->propertyValuePairs["minimumsubmenuwidth"]->value));

            for (const auto& childNode : node->children)
            {
                if (toLower(childNode->name) == "menu")
                {
                    if (childNode->propertyValuePairs["name"])
                    {
                        menuBar->addMenu(Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs["name"]->value).getString());

                        if (childNode->propertyValuePairs["items"])
                        {
                            if (!childNode->propertyValuePairs["items"]->listNode)
                                throw Exception{"Failed to parse 'Items' property inside 'Menu' property, expected a list as value"};

                            for (std::size_t i = 0; i < childNode->propertyValuePairs["items"]->valueList.size(); ++i)
                                menuBar->addMenuItem(Deserializer::deserialize(ObjectConverter::Type::String, childNode->propertyValuePairs["items"]->valueList[i]).getString());
                        }
                    }
                }
            }
            REMOVE_CHILD("menu");

            return menuBar;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadMessageBox(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            MessageBox::Ptr messageBox;
            if (widget)
                messageBox = std::static_pointer_cast<MessageBox>(widget);
            else
                messageBox = MessageBox::create();

            // Remove the label that the MessageBox constructor creates because it will be created when loading the child window
            messageBox->removeAllWidgets();

            loadChildWindow(node, messageBox);

            if (node->propertyValuePairs["textsize"])
                messageBox->setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));

            // Make a copy because the label and buttons are identified when doing so
            return tgui::MessageBox::copy(messageBox);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadPanel(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            if (widget)
                return loadContainer(node, std::static_pointer_cast<Panel>(widget));
            else
                return loadContainer(node, std::make_shared<Panel>());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadPicture(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            Picture::Ptr picture;
            if (widget)
                picture = std::static_pointer_cast<Picture>(widget);
            else
                picture = Picture::create();

            if (node->propertyValuePairs["filename"])
                picture = std::make_shared<Picture>(DESERIALIZE_STRING("filename"));

            loadWidget(node, picture);

            if (node->propertyValuePairs["smooth"])
                picture->setSmooth(parseBoolean(node->propertyValuePairs["smooth"]->value));

            return picture;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadProgressBar(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            ProgressBar::Ptr progressBar;
            if (widget)
                progressBar = std::static_pointer_cast<ProgressBar>(widget);
            else
                progressBar = ProgressBar::create();

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

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadRadioButton(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            RadioButton::Ptr radioButton;
            if (widget)
                radioButton = std::static_pointer_cast<RadioButton>(widget);
            else
                radioButton = RadioButton::create();

            loadWidget(node, radioButton);
            if (node->propertyValuePairs["text"])
                radioButton->setText(DESERIALIZE_STRING("text"));
            if (node->propertyValuePairs["textsize"])
                radioButton->setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
            if (node->propertyValuePairs["textclickable"])
                radioButton->setTextClickable(parseBoolean(node->propertyValuePairs["textclickable"]->value));
            if (node->propertyValuePairs["checked"])
            {
                if (parseBoolean(node->propertyValuePairs["checked"]->value))
                    radioButton->check();
            }

            return radioButton;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadRadioButtonGroup(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            if (widget)
                return loadContainer(node, std::static_pointer_cast<RadioButtonGroup>(widget));
            else
                return loadContainer(node, std::make_shared<RadioButtonGroup>());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadScrollbar(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            Scrollbar::Ptr scrollbar;
            if (widget)
                scrollbar = std::static_pointer_cast<Scrollbar>(widget);
            else
                scrollbar = Scrollbar::create();

            loadWidget(node, scrollbar);
            if (node->propertyValuePairs["lowvalue"])
                scrollbar->setLowValue(tgui::stoi(node->propertyValuePairs["lowvalue"]->value));
            if (node->propertyValuePairs["maximum"])
                scrollbar->setMaximum(tgui::stoi(node->propertyValuePairs["maximum"]->value));
            if (node->propertyValuePairs["value"])
                scrollbar->setValue(tgui::stoi(node->propertyValuePairs["value"]->value));
            if (node->propertyValuePairs["scrollamount"])
                scrollbar->setScrollAmount(tgui::stoi(node->propertyValuePairs["scrollamount"]->value));
            if (node->propertyValuePairs["autohide"])
                scrollbar->setAutoHide(parseBoolean(node->propertyValuePairs["autohide"]->value));

            return scrollbar;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadSlider(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            Slider::Ptr slider;
            if (widget)
                slider = std::static_pointer_cast<Slider>(widget);
            else
                slider = Slider::create();

            loadWidget(node, slider);
            if (node->propertyValuePairs["minimum"])
                slider->setMinimum(tgui::stoi(node->propertyValuePairs["minimum"]->value));
            if (node->propertyValuePairs["maximum"])
                slider->setMaximum(tgui::stoi(node->propertyValuePairs["maximum"]->value));
            if (node->propertyValuePairs["value"])
                slider->setValue(tgui::stoi(node->propertyValuePairs["value"]->value));

            return slider;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadSpinButton(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            SpinButton::Ptr spinButton;
            if (widget)
                spinButton = std::static_pointer_cast<SpinButton>(widget);
            else
                spinButton = SpinButton::create();

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

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadTabs(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            Tabs::Ptr tabs;
            if (widget)
                tabs = std::static_pointer_cast<Tabs>(widget);
            else
                tabs = Tabs::create();

            loadWidget(node, tabs);

            if (node->propertyValuePairs["tabs"])
            {
                if (!node->propertyValuePairs["tabs"]->listNode)
                    throw Exception{"Failed to parse 'Tabs' property, expected a list as value"};

                for (const auto& tabText : node->propertyValuePairs["tabs"]->valueList)
                    tabs->add(Deserializer::deserialize(ObjectConverter::Type::String, tabText).getString());
            }

            if (node->propertyValuePairs["maximumtabwidth"])
                tabs->setMaximumTabWidth(tgui::stof(node->propertyValuePairs["maximumtabwidth"]->value));
            if (node->propertyValuePairs["textsize"])
                tabs->setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
            if (node->propertyValuePairs["tabheight"])
                tabs->setTabHeight(tgui::stof(node->propertyValuePairs["tabheight"]->value));
            if (node->propertyValuePairs["selected"])
                tabs->select(tgui::stoi(node->propertyValuePairs["selected"]->value));

            return tabs;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Widget::Ptr loadTextBox(std::shared_ptr<DataIO::Node> node, Widget::Ptr widget)
        {
            TextBox::Ptr textBox;
            if (widget)
                textBox = std::static_pointer_cast<TextBox>(widget);
            else
                textBox = TextBox::create();

            loadWidget(node, textBox);

            if (node->propertyValuePairs["text"])
                textBox->setText(DESERIALIZE_STRING("text"));
            if (node->propertyValuePairs["textsize"])
                textBox->setTextSize(tgui::stoi(node->propertyValuePairs["textsize"]->value));
            if (node->propertyValuePairs["maximumcharacters"])
                textBox->setMaximumCharacters(tgui::stoi(node->propertyValuePairs["maximumcharacters"]->value));
            if (node->propertyValuePairs["readonly"])
                textBox->setReadOnly(parseBoolean(node->propertyValuePairs["readonly"]->value));
            if (node->propertyValuePairs["verticalscrollbarpresent"])
                textBox->setVerticalScrollbarPresent(parseBoolean(node->propertyValuePairs["verticalscrollbarpresent"]->value));

            return textBox;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, WidgetLoader::LoadFunction> WidgetLoader::m_loadFunctions =
        {
            {"widget", loadWidget},
            {"container", loadContainer},
            {"button", loadButton},
            {"canvas", loadCanvas},
            {"chatbox", loadChatBox},
            {"checkbox", loadCheckBox},
            {"childwindow", loadChildWindow},
            {"clickablewidget", loadClickableWidget},
            {"combobox", loadComboBox},
            {"editbox", loadEditBox},
            {"group", loadGroup},
            {"knob", loadKnob},
            {"label", loadLabel},
            {"listbox", loadListBox},
            {"menubar", loadMenuBar},
            {"messagebox", loadMessageBox},
            {"panel", loadPanel},
            {"picture", loadPicture},
            {"progressbar", loadProgressBar},
            {"radiobutton", loadRadioButton},
            {"radiobuttongroup", loadRadioButtonGroup},
            {"scrollbar", loadScrollbar},
            {"slider", loadSlider},
            {"spinbutton", loadSpinButton},
            {"tabs", loadTabs},
            {"textbox", loadTextBox}
        };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetLoader::load(Container::Ptr parent, std::stringstream& stream)
    {
        auto rootNode = DataIO::parse(stream);

        if (rootNode->propertyValuePairs.size() != 0)
            loadWidget(rootNode, parent);

        for (const auto& node : rootNode->children)
        {
            auto nameSeparator = node->name.find('.');
            auto widgetType = node->name.substr(0, nameSeparator);
            auto& loadFunction = m_loadFunctions[toLower(widgetType)];
            if (loadFunction)
            {
                std::string className;
                if (nameSeparator != std::string::npos)
                    className = Deserializer::deserialize(ObjectConverter::Type::String, node->name.substr(nameSeparator + 1)).getString();

                tgui::Widget::Ptr widget = loadFunction(node, nullptr);
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
