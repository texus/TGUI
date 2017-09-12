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
#include <TGUI/Loading/WidgetSaver.hpp>
#include <TGUI/Loading/Serializer.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/ChatBox.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/ComboBox.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/Grid.hpp>
#include <TGUI/Widgets/HorizontalLayout.hpp>
#include <TGUI/Widgets/Knob.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/ListBox.hpp>
#include <TGUI/Widgets/MenuBar.hpp>
#include <TGUI/Widgets/MessageBox.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <TGUI/Widgets/ProgressBar.hpp>
#include <TGUI/Widgets/RadioButton.hpp>
#include <TGUI/Widgets/RangeSlider.hpp>
#include <TGUI/Widgets/ScrollablePanel.hpp>
#include <TGUI/Widgets/Scrollbar.hpp>
#include <TGUI/Widgets/Slider.hpp>
#include <TGUI/Widgets/SpinButton.hpp>
#include <TGUI/Widgets/Tabs.hpp>
#include <TGUI/Widgets/TextBox.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>
#include <TGUI/to_string.hpp>

#ifdef SFML_SYSTEM_WINDOWS
    #include <direct.h> // _getcwd
#else
    #include <unistd.h> // getcwd
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    namespace
    {
        #define SET_PROPERTY(property, value) node->propertyValuePairs[property] = make_unique<DataIO::ValueNode>(value)

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::string workingDirectory;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        sf::String tryRemoveAbsolutePath(const sf::String& value)
        {
            if (!value.isEmpty() && (value != "null") && (value != "nullptr"))
            {
                if (value[0] != '"')
                {
                #ifdef SFML_SYSTEM_WINDOWS
                    if ((value[0] == '/') || (value[0] == '\\') || ((value.getSize() > 1) && (value[1] == ':')))
                #else
                    if (value[0] == '/')
                #endif
                    {
                        if ((value.getSize() > workingDirectory.size()) && (value.substring(0, workingDirectory.size()) == workingDirectory))
                        {
                            if ((value[workingDirectory.size()] != '/') && (value[workingDirectory.size()] != '\\'))
                                return value.substring(workingDirectory.size());
                            else
                                return value.substring(workingDirectory.size() + 1);
                        }
                    }
                }
                else // The filename is between quotes
                {
                    if (value.getSize() <= 1)
                        return value;

                #ifdef SFML_SYSTEM_WINDOWS
                    if ((value[1] == '/') || (value[1] == '\\') || ((value.getSize() > 2) && (value[2] == ':')))
                #else
                    if (value[1] == '/')
                #endif
                    {
                        if ((value.getSize() + 1 > workingDirectory.size()) && (value.substring(1, workingDirectory.size()) == workingDirectory))
                            return '"' + value.substring(workingDirectory.size() + 1);
                    }
                }
            }

            return value;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void recursiveTryRemoveAbsolutePath(std::unique_ptr<DataIO::Node>& node)
        {
            for (auto& pair : node->propertyValuePairs)
            {
                if (((pair.first.size() >= 7) && (toLower(pair.first.substr(0, 7)) == "texture")) || (pair.first == "font"))
                    pair.second->value = tryRemoveAbsolutePath(pair.second->value);
            }

            for (auto& child : node->children)
                recursiveTryRemoveAbsolutePath(child);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveWidget(Widget::Ptr widget)
        {
            sf::String widgetName;
            if (widget->getParent())
                widgetName = widget->getParent()->getWidgetName(widget);

            auto node = make_unique<DataIO::Node>();
            if (widgetName.isEmpty())
                node->name = widget->getWidgetType();
            else
                node->name = widget->getWidgetType() + "." + Serializer::serialize(widgetName);

            if (!widget->isVisible())
                SET_PROPERTY("Visible", "false");
            if (!widget->isEnabled())
                SET_PROPERTY("Enabled", "false");
            if (widget->getPosition() != sf::Vector2f{})
                SET_PROPERTY("Position", widget->getPositionLayout().toString());
            if (widget->getSize() != sf::Vector2f{})
                SET_PROPERTY("Size", widget->getSizeLayout().toString());

            if (widget->getToolTip() != nullptr)
            {
                auto toolTipWidgetNode = WidgetSaver::getSaveFunction("widget")(widget->getToolTip());

                auto toolTipNode = make_unique<DataIO::Node>();
                toolTipNode->name = "ToolTip";
                toolTipNode->children.emplace_back(std::move(toolTipWidgetNode));

                toolTipNode->propertyValuePairs["TimeToDisplay"] = make_unique<DataIO::ValueNode>(to_string(ToolTip::getTimeToDisplay().asSeconds()));
                toolTipNode->propertyValuePairs["DistanceToMouse"] = make_unique<DataIO::ValueNode>("(" + to_string(ToolTip::getDistanceToMouse().x) + "," + to_string(ToolTip::getDistanceToMouse().y) + ")");

                node->children.emplace_back(std::move(toolTipNode));
            }

            /// TODO: Separate renderer section?
            if (!widget->getRenderer()->getPropertyValuePairs().empty())
            {
                node->children.emplace_back(make_unique<DataIO::Node>());
                node->children.back()->name = "Renderer";
                for (const auto& pair : widget->getRenderer()->getPropertyValuePairs())
                {
                    // Skip "font = null"
                    if (pair.first == "font" && ObjectConverter{pair.second}.getString() == "null")
                        continue;

                    sf::String value = ObjectConverter{pair.second}.getString();

                    // Turn absolute paths (which were likely caused by loading from a theme) into relative paths if the first part of the path matches the current working directory
                    if (!workingDirectory.empty())
                    {
                        if ((pair.second.getType() == ObjectConverter::Type::Font) || (pair.second.getType() == ObjectConverter::Type::Texture))
                            value = tryRemoveAbsolutePath(value);
                    }

                    if (pair.second.getType() == ObjectConverter::Type::RendererData)
                    {
                        std::stringstream ss{value};
                        auto rendererRootNode = DataIO::parse(ss);
                        if (!rendererRootNode->children.empty())
                            rendererRootNode = std::move(rendererRootNode->children[0]);

                        recursiveTryRemoveAbsolutePath(rendererRootNode);

                        rendererRootNode->name = pair.first;
                        node->children.back()->children.push_back(std::move(rendererRootNode));
                    }
                    else
                        node->children.back()->propertyValuePairs[pair.first] = make_unique<DataIO::ValueNode>(value);
                }
            }

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveContainer(Widget::Ptr widget)
        {
            auto container = std::static_pointer_cast<Container>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(container);

            for (const auto& child : container->getWidgets())
            {
                auto& saveFunction = WidgetSaver::getSaveFunction(toLower(child->getWidgetType()));
                if (saveFunction)
                    node->children.emplace_back(saveFunction(child));
                else
                    throw Exception{"No save function exists for widget type '" + child->getWidgetType() + "'."};
            }

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveButton(Widget::Ptr widget)
        {
            auto button = std::static_pointer_cast<Button>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(button);

            if (!button->getText().isEmpty())
                SET_PROPERTY("Text", Serializer::serialize(button->getText()));

            SET_PROPERTY("TextSize", to_string(button->getTextSize()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveBoxLayoutRatios(Widget::Ptr widget)
        {
            auto layout = std::static_pointer_cast<BoxLayoutRatios>(widget);
            auto node = WidgetSaver::getSaveFunction("container")(layout);

            if (layout->getWidgets().size() > 0)
            {
                std::string ratioList = "[" + Serializer::serialize(layout->getRatio(0));
                for (std::size_t i = 1; i < layout->getWidgets().size(); ++i)
                    ratioList += ", " + Serializer::serialize(layout->getRatio(i));

                ratioList += "]";
                SET_PROPERTY("Ratios", ratioList);
            }

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveChatBox(Widget::Ptr widget)
        {
            auto chatBox = std::static_pointer_cast<ChatBox>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(chatBox);

            SET_PROPERTY("TextSize", to_string(chatBox->getTextSize()));
            SET_PROPERTY("TextColor", Serializer::serialize(chatBox->getTextColor()));

            if (chatBox->getLineLimit())
                SET_PROPERTY("LineLimit", to_string(chatBox->getLineLimit()));

            if (chatBox->getLinesStartFromTop())
                SET_PROPERTY("LinesStartFromTop", "true");
            else
                SET_PROPERTY("LinesStartFromTop", "false");

            if (chatBox->getNewLinesBelowOthers())
                SET_PROPERTY("NewLinesBelowOthers", "true");
            else
                SET_PROPERTY("NewLinesBelowOthers", "false");

            for (std::size_t i = 0; i < chatBox->getLineAmount(); ++i)
            {
                unsigned int lineTextSize = chatBox->getLineTextSize(i);
                sf::Color lineTextColor = chatBox->getLineColor(i);

                auto lineNode = make_unique<DataIO::Node>();
                lineNode->parent = node.get();
                lineNode->name = "Line";

                lineNode->propertyValuePairs["Text"] = make_unique<DataIO::ValueNode>(Serializer::serialize(chatBox->getLine(i)));
                if (lineTextSize != chatBox->getTextSize())
                    lineNode->propertyValuePairs["TextSize"] = make_unique<DataIO::ValueNode>(to_string(lineTextSize));
                if (lineTextColor != chatBox->getTextColor())
                    lineNode->propertyValuePairs["Color"] = make_unique<DataIO::ValueNode>(Serializer::serialize(lineTextColor));

                node->children.push_back(std::move(lineNode));
            }

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveChildWindow(Widget::Ptr widget)
        {
            auto childWindow = std::static_pointer_cast<ChildWindow>(widget);
            auto node = WidgetSaver::getSaveFunction("container")(childWindow);

            if (childWindow->getTitleAlignment() == ChildWindow::TitleAlignment::Left)
                SET_PROPERTY("TitleAlignment", "Left");
            else if (childWindow->getTitleAlignment() == ChildWindow::TitleAlignment::Center)
                SET_PROPERTY("TitleAlignment", "Center");
            else if (childWindow->getTitleAlignment() == ChildWindow::TitleAlignment::Right)
                SET_PROPERTY("TitleAlignment", "Right");

            if (childWindow->getTitle().getSize() > 0)
                SET_PROPERTY("Title", Serializer::serialize(childWindow->getTitle()));

            if (childWindow->isKeptInParent())
                SET_PROPERTY("KeepInParent", "true");

            if (childWindow->isResizable())
                SET_PROPERTY("Resizable", "true");

            if (childWindow->getMinimumSize() != sf::Vector2f{})
                SET_PROPERTY("MinimumSize", "(" + to_string(childWindow->getMinimumSize().x) + ", " + to_string(childWindow->getMinimumSize().y) + ")");
            if (childWindow->getMaximumSize() != sf::Vector2f{std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()})
                SET_PROPERTY("MaximumSize", "(" + to_string(childWindow->getMaximumSize().x) + ", " + to_string(childWindow->getMaximumSize().y) + ")");

            std::string serializedTitleButtons;
            if (childWindow->getTitleButtons() & ChildWindow::TitleButton::Minimize)
                serializedTitleButtons += " | Minimize";
            if (childWindow->getTitleButtons() & ChildWindow::TitleButton::Maximize)
                serializedTitleButtons += " | Maximize";
            if (childWindow->getTitleButtons() & ChildWindow::TitleButton::Close)
                serializedTitleButtons += " | Close";

            if (!serializedTitleButtons.empty())
                serializedTitleButtons.erase(0, 3);
            else
                serializedTitleButtons = "None";

            SET_PROPERTY("TitleButtons", serializedTitleButtons);

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveComboBox(Widget::Ptr widget)
        {
            auto comboBox = std::static_pointer_cast<ComboBox>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(comboBox);

            if (comboBox->getItemCount() > 0)
            {
                auto items = comboBox->getItems();
                auto& ids = comboBox->getItemIds();

                std::string itemList = "[" + Serializer::serialize(items[0]);
                std::string itemIdList = "[" + Serializer::serialize(ids[0]);
                for (std::size_t i = 1; i < items.size(); ++i)
                {
                    itemList += ", " + Serializer::serialize(items[i]);
                    itemIdList += ", " + Serializer::serialize(ids[i]);
                }
                itemList += "]";
                itemIdList += "]";

                SET_PROPERTY("Items", itemList);
                SET_PROPERTY("ItemIds", itemIdList);
            }

            SET_PROPERTY("ItemsToDisplay", to_string(comboBox->getItemsToDisplay()));
            SET_PROPERTY("TextSize", to_string(comboBox->getTextSize()));
            SET_PROPERTY("MaximumItems", to_string(comboBox->getMaximumItems()));

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveEditBox(Widget::Ptr widget)
        {
            auto editBox = std::static_pointer_cast<EditBox>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(editBox);

            if (editBox->getAlignment() != EditBox::Alignment::Left)
            {
                if (editBox->getAlignment() == EditBox::Alignment::Center)
                    SET_PROPERTY("Alignment", "Center");
                else
                    SET_PROPERTY("Alignment", "Right");
            }

            if (editBox->getInputValidator() != ".*")
            {
                if (editBox->getInputValidator() == EditBox::Validator::Int)
                    SET_PROPERTY("InputValidator", "Int");
                else if (editBox->getInputValidator() == EditBox::Validator::UInt)
                    SET_PROPERTY("InputValidator", "UInt");
                else if (editBox->getInputValidator() == EditBox::Validator::Float)
                    SET_PROPERTY("InputValidator", "Float");
                else
                    SET_PROPERTY("InputValidator", Serializer::serialize(sf::String{editBox->getInputValidator()}));
            }

            if (!editBox->getText().isEmpty())
                SET_PROPERTY("Text", Serializer::serialize(editBox->getText()));
            if (!editBox->getDefaultText().isEmpty())
                SET_PROPERTY("DefaultText", Serializer::serialize(editBox->getDefaultText()));
            if (editBox->getPasswordCharacter() != '\0')
                SET_PROPERTY("PasswordCharacter", Serializer::serialize(sf::String(editBox->getPasswordCharacter())));
            if (editBox->getMaximumCharacters() != 0)
                SET_PROPERTY("MaximumCharacters", to_string(editBox->getMaximumCharacters()));
            if (editBox->isTextWidthLimited())
                SET_PROPERTY("TextWidthLimited", "true");

            SET_PROPERTY("TextSize", to_string(editBox->getTextSize()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveGrid(Widget::Ptr widget)
        {
            auto grid = std::static_pointer_cast<Grid>(widget);
            auto node = WidgetSaver::getSaveFunction("container")(grid);

            const auto& children = grid->getWidgets();
            auto widgetsMap = grid->getWidgetLocations();
            if (!widgetsMap.empty())
            {
                auto alignmentToString = [](Grid::Alignment alignment) -> std::string {
                    switch (alignment)
                    {
                    case Grid::Alignment::Center:
                        return "Center";
                    case Grid::Alignment::UpperLeft:
                        return "UpperLeft";
                    case Grid::Alignment::Up:
                        return "Up";
                    case Grid::Alignment::UpperRight:
                        return "UpperRight";
                    case Grid::Alignment::Right:
                        return "Right";
                    case Grid::Alignment::BottomRight:
                        return "BottomRight";
                    case Grid::Alignment::Bottom:
                        return "Bottom";
                    case Grid::Alignment::BottomLeft:
                        return "BottomLeft";
                    case Grid::Alignment::Left:
                        return "Left";
                    default:
                        throw Exception{"Invalid grid alignment encountered."};
                    }
                };

                auto getWidgetsInGridString = [&](const tgui::Widget::Ptr& w) -> std::string {
                    auto it = widgetsMap.find(w);
                    if (it != widgetsMap.end())
                    {
                        const auto row = it->second.first;
                        const auto col = it->second.second;
                        return "\"(" + to_string(row)
                             + ", " + to_string(col)
                             + ", " + grid->getWidgetBorders(row, col).toString()
                             + ", " + alignmentToString(grid->getWidgetAlignment(row, col))
                             + ")\"";
                    }
                    else
                        return "\"()\"";
                };

                std::string str = "[" + getWidgetsInGridString(children[0]);

                for (std::size_t i = 1; i < children.size(); ++i)
                    str += ", " + getWidgetsInGridString(children[i]);

                str += "]";
                SET_PROPERTY("GridWidgets", str);
            }

            if (grid->getAutoSize())
                node->propertyValuePairs.erase("Size");

            SET_PROPERTY("AutoSize", to_string(grid->getAutoSize()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveKnob(Widget::Ptr widget)
        {
            auto knob = std::static_pointer_cast<Knob>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(knob);

            if (knob->getClockwiseTurning())
                SET_PROPERTY("ClockwiseTurning", "true");
            else
                SET_PROPERTY("ClockwiseTurning", "false");

            SET_PROPERTY("StartRotation", to_string(knob->getStartRotation()));
            SET_PROPERTY("EndRotation", to_string(knob->getEndRotation()));
            SET_PROPERTY("Minimum", to_string(knob->getMinimum()));
            SET_PROPERTY("Maximum", to_string(knob->getMaximum()));
            SET_PROPERTY("Value", to_string(knob->getValue()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveLabel(Widget::Ptr widget)
        {
            auto label = std::static_pointer_cast<Label>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(label);

            if (label->getHorizontalAlignment() == Label::HorizontalAlignment::Center)
                SET_PROPERTY("HorizontalAlignment", "Center");
            else if (label->getHorizontalAlignment() == Label::HorizontalAlignment::Right)
                SET_PROPERTY("HorizontalAlignment", "Right");

            if (label->getVerticalAlignment() == Label::VerticalAlignment::Center)
                SET_PROPERTY("VerticalAlignment", "Center");
            else if (label->getVerticalAlignment() == Label::VerticalAlignment::Bottom)
                SET_PROPERTY("VerticalAlignment", "Bottom");

            if (!label->getText().isEmpty())
                SET_PROPERTY("Text", Serializer::serialize(label->getText()));
            if (label->getMaximumTextWidth() > 0)
                SET_PROPERTY("MaximumTextWidth", to_string(label->getMaximumTextWidth()));
            if (label->getAutoSize())
                SET_PROPERTY("AutoSize", "true");

            if (label->isIgnoringMouseEvents())
                SET_PROPERTY("IgnoreMouseEvents", Serializer::serialize(label->isIgnoringMouseEvents()));

            SET_PROPERTY("TextSize", to_string(label->getTextSize()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveListBox(Widget::Ptr widget)
        {
            auto listBox = std::static_pointer_cast<ListBox>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(listBox);

            if (listBox->getItemCount() > 0)
            {
                auto items = listBox->getItems();
                auto& ids = listBox->getItemIds();

                std::string itemList = "[" + Serializer::serialize(items[0]);
                std::string itemIdList = "[" + Serializer::serialize(ids[0]);
                for (std::size_t i = 1; i < items.size(); ++i)
                {
                    itemList += ", " + Serializer::serialize(items[i]);
                    itemIdList += ", " + Serializer::serialize(ids[i]);
                }
                itemList += "]";
                itemIdList += "]";

                SET_PROPERTY("Items", itemList);
                SET_PROPERTY("ItemIds", itemIdList);
            }

            if (!listBox->getAutoScroll())
                SET_PROPERTY("AutoScroll", "false");

            SET_PROPERTY("TextSize", to_string(listBox->getTextSize()));
            SET_PROPERTY("ItemHeight", to_string(listBox->getItemHeight()));
            SET_PROPERTY("MaximumItems", to_string(listBox->getMaximumItems()));

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveMenuBar(Widget::Ptr widget)
        {
            auto menuBar = std::static_pointer_cast<MenuBar>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(menuBar);

            std::map<sf::String, std::vector<sf::String>> menus = menuBar->getMenus();
            for (const auto& menu : menus)
            {
                auto menuNode = make_unique<DataIO::Node>();
                menuNode->parent = node.get();
                menuNode->name = "Menu";

                menuNode->propertyValuePairs["Name"] = make_unique<DataIO::ValueNode>(Serializer::serialize(menu.first));

                const auto& items = menu.second;
                if (!items.empty())
                {
                    std::string itemList = "[" + Serializer::serialize(items[0]);
                    for (std::size_t i = 1; i < items.size(); ++i)
                        itemList += ", " + Serializer::serialize(items[i]);
                    itemList += "]";

                    menuNode->propertyValuePairs["Items"] = make_unique<DataIO::ValueNode>(itemList);
                }

                node->children.push_back(std::move(menuNode));
            }

            SET_PROPERTY("TextSize", to_string(menuBar->getTextSize()));
            SET_PROPERTY("MinimumSubMenuWidth", to_string(menuBar->getMinimumSubMenuWidth()));

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveMessageBox(Widget::Ptr widget)
        {
            auto messageBox = std::static_pointer_cast<MessageBox>(widget);
            auto node = WidgetSaver::getSaveFunction("childwindow")(messageBox);

            SET_PROPERTY("TextSize", to_string(messageBox->getTextSize()));

            // Label and buttons are saved indirectly by saving the child window

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> savePicture(Widget::Ptr widget)
        {
            auto picture = std::static_pointer_cast<Picture>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(picture);

            if (picture->isIgnoringMouseEvents())
                SET_PROPERTY("IgnoreMouseEvents", Serializer::serialize(picture->isIgnoringMouseEvents()));

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveProgressBar(Widget::Ptr widget)
        {
            auto progressBar = std::static_pointer_cast<ProgressBar>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(progressBar);

            if (!progressBar->getText().isEmpty())
                SET_PROPERTY("Text", Serializer::serialize(progressBar->getText()));

            if (progressBar->getFillDirection() != ProgressBar::FillDirection::LeftToRight)
            {
                if (progressBar->getFillDirection() == ProgressBar::FillDirection::RightToLeft)
                    SET_PROPERTY("FillDirection", "RightToLeft");
                else if (progressBar->getFillDirection() == ProgressBar::FillDirection::TopToBottom)
                    SET_PROPERTY("FillDirection", "TopToBottom");
                else if (progressBar->getFillDirection() == ProgressBar::FillDirection::BottomToTop)
                    SET_PROPERTY("FillDirection", "BottomToTop");
            }

            SET_PROPERTY("Minimum", to_string(progressBar->getMinimum()));
            SET_PROPERTY("Maximum", to_string(progressBar->getMaximum()));
            SET_PROPERTY("Value", to_string(progressBar->getValue()));
            SET_PROPERTY("TextSize", to_string(progressBar->getTextSize()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveRadioButton(Widget::Ptr widget)
        {
            auto radioButton = std::static_pointer_cast<RadioButton>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(radioButton);

            if (!radioButton->getText().isEmpty())
                SET_PROPERTY("Text", Serializer::serialize(radioButton->getText()));
            if (radioButton->isChecked())
                SET_PROPERTY("Checked", "true");
            if (!radioButton->isTextClickable())
                SET_PROPERTY("TextClickable", "false");

            SET_PROPERTY("TextSize", to_string(radioButton->getTextSize()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveRangeSlider(Widget::Ptr widget)
        {
            auto slider = std::static_pointer_cast<RangeSlider>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(slider);

            SET_PROPERTY("Minimum", to_string(slider->getMinimum()));
            SET_PROPERTY("Maximum", to_string(slider->getMaximum()));
            SET_PROPERTY("SelectionStart", to_string(slider->getSelectionStart()));
            SET_PROPERTY("SelectionEnd", to_string(slider->getSelectionEnd()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveScrollablePanel(Widget::Ptr widget)
        {
            auto panel = std::static_pointer_cast<ScrollablePanel>(widget);
            auto node = WidgetSaver::getSaveFunction("panel")(panel);

            SET_PROPERTY("CententSize", "(" + to_string(panel->getContentSize().x) + ", " + to_string(panel->getContentSize().y) + ")");

            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveScrollbar(Widget::Ptr widget)
        {
            auto scrollbar = std::static_pointer_cast<Scrollbar>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(scrollbar);

            if (scrollbar->getAutoHide())
                SET_PROPERTY("AutoHide", "true");
            else
                SET_PROPERTY("AutoHide", "false");

            SET_PROPERTY("LowValue", to_string(scrollbar->getLowValue()));
            SET_PROPERTY("Maximum", to_string(scrollbar->getMaximum()));
            SET_PROPERTY("Value", to_string(scrollbar->getValue()));
            SET_PROPERTY("ScrollAmount", to_string(scrollbar->getScrollAmount()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveSlider(Widget::Ptr widget)
        {
            auto slider = std::static_pointer_cast<Slider>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(slider);

            SET_PROPERTY("Minimum", to_string(slider->getMinimum()));
            SET_PROPERTY("Maximum", to_string(slider->getMaximum()));
            SET_PROPERTY("Value", to_string(slider->getValue()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveSpinButton(Widget::Ptr widget)
        {
            auto spinButton = std::static_pointer_cast<SpinButton>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(spinButton);

            if (spinButton->getVerticalScroll())
                SET_PROPERTY("VerticalScroll", "true");
            else
                SET_PROPERTY("VerticalScroll", "false");

            SET_PROPERTY("Minimum", to_string(spinButton->getMinimum()));
            SET_PROPERTY("Maximum", to_string(spinButton->getMaximum()));
            SET_PROPERTY("Value", to_string(spinButton->getValue()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveTabs(Widget::Ptr widget)
        {
            auto tabs = std::static_pointer_cast<Tabs>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(tabs);

            if (tabs->getTabsCount() > 0)
            {
                std::string tabList = "[" + Serializer::serialize(tabs->getText(0));
                for (std::size_t i = 1; i < tabs->getTabsCount(); ++i)
                    tabList += ", " + Serializer::serialize(tabs->getText(i));

                tabList += "]";
                SET_PROPERTY("Tabs", tabList);
            }

            if (tabs->getSelectedIndex() >= 0)
                SET_PROPERTY("Selected", to_string(tabs->getSelectedIndex()));

            if (tabs->getMaximumTabWidth() > 0)
                SET_PROPERTY("MaximumTabWidth", to_string(tabs->getMaximumTabWidth()));

            if (tabs->getAutoSize())
            {
                node->propertyValuePairs.erase("Size");
                SET_PROPERTY("TabHeight", to_string(tabs->getSize().y));
            }

            SET_PROPERTY("TextSize", to_string(tabs->getTextSize()));
            SET_PROPERTY("AutoSize", to_string(tabs->getAutoSize()));
            return node;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::unique_ptr<DataIO::Node> saveTextBox(Widget::Ptr widget)
        {
            auto textBox = std::static_pointer_cast<TextBox>(widget);
            auto node = WidgetSaver::getSaveFunction("widget")(textBox);

            SET_PROPERTY("Text", Serializer::serialize(textBox->getText()));
            SET_PROPERTY("TextSize", to_string(textBox->getTextSize()));
            SET_PROPERTY("MaximumCharacters", to_string(textBox->getMaximumCharacters()));

            if (textBox->isReadOnly())
                SET_PROPERTY("ReadOnly", "true");

            if (!textBox->isVerticalScrollbarPresent())
                SET_PROPERTY("VerticalScrollbarPresent", "false");

            return node;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, WidgetSaver::SaveFunction> WidgetSaver::m_saveFunctions =
        {
            {"widget", saveWidget},
            {"container", saveContainer},
            {"button", saveButton},
            {"canvas", saveWidget},
            {"chatbox", saveChatBox},
            {"checkbox", saveRadioButton},
            {"childwindow", saveChildWindow},
            {"clickablewidget", saveWidget},
            {"combobox", saveComboBox},
            {"editbox", saveEditBox},
            {"grid", saveGrid},
            {"group", saveContainer},
            {"horizontallayout", saveBoxLayoutRatios},
            {"horizontalwrap", saveContainer},
            {"knob", saveKnob},
            {"label", saveLabel},
            {"listbox", saveListBox},
            {"menubar", saveMenuBar},
            {"messagebox", saveMessageBox},
            {"panel", saveContainer},
            {"picture", savePicture},
            {"progressbar", saveProgressBar},
            {"radiobutton", saveRadioButton},
            {"radiobuttongroup", saveContainer},
            {"rangeslider", saveRangeSlider},
            {"scrollablepanel", saveScrollablePanel},
            {"scrollbar", saveScrollbar},
            {"slider", saveSlider},
            {"spinbutton", saveSpinButton},
            {"tabs", saveTabs},
            {"textbox", saveTextBox},
            {"verticallayout", saveBoxLayoutRatios}
        };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetSaver::save(Container::ConstPtr widget, std::stringstream& stream)
    {
        std::string resourcePath = getResourcePath();
    #ifdef SFML_SYSTEM_WINDOWS
        if ((resourcePath[0] == '/') || (resourcePath[0] == '\\') || ((resourcePath.size() > 1) && (resourcePath[1] == ':')))
    #else
        if (resourcePath[0] == '/')
    #endif
        {
            // The resource path is already an absolute path, we don't even need to find out the current working directory
            workingDirectory = resourcePath;
        }
        else
        {
            // Get the current working directory (used for turning absolute into relative paths in saveWidget)
        #ifdef SFML_SYSTEM_WINDOWS
            char* buffer = _getcwd(nullptr, 0);
        #else
            char* buffer = getcwd(nullptr, 0);
        #endif
            if (buffer)
            {
                workingDirectory = buffer;
                free(buffer);

                if (!workingDirectory.empty() && (workingDirectory[workingDirectory.size() - 1] != '/') && (workingDirectory[workingDirectory.size() - 1] != '\\'))
                    workingDirectory.push_back('/');

                workingDirectory += getResourcePath();
            }
        }

        auto node = make_unique<DataIO::Node>();
        for (const auto& child : widget->getWidgets())
        {
            auto& saveFunction = WidgetSaver::getSaveFunction(toLower(child->getWidgetType()));
            if (saveFunction)
                node->children.emplace_back(saveFunction(child));
            else
                throw Exception{"No save function exists for widget type '" + child->getWidgetType() + "'."};
        }

        DataIO::emit(node, stream);
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
