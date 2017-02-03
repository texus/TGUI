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


#include <TGUI/Loading/Serializer.hpp>
#include <TGUI/Loading/WidgetSaver.hpp>
#include <TGUI/Widgets/Button.hpp>
#include <TGUI/Widgets/ChatBox.hpp>
#include <TGUI/Widgets/ChildWindow.hpp>
#include <TGUI/Widgets/ComboBox.hpp>
#include <TGUI/Widgets/EditBox.hpp>
#include <TGUI/Widgets/Knob.hpp>
#include <TGUI/Widgets/ListBox.hpp>
#include <TGUI/Widgets/Picture.hpp>
#include <TGUI/Widgets/ProgressBar.hpp>
#include <TGUI/Widgets/RadioButton.hpp>
#include <TGUI/Widgets/Scrollbar.hpp>
#include <TGUI/Widgets/Slider.hpp>
#include <TGUI/Widgets/SpinButton.hpp>
#include <TGUI/Widgets/Tab.hpp>
#include <TGUI/Widgets/TextBox.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
    std::string emitLayout(tgui::Layout2d layout)
    {
        std::string str;
        str += "(";

        if (layout.x.getImpl()->operation == tgui::LayoutImpl::Operation::String)
            str += "\"" + layout.x.getImpl()->stringExpression + "\"";
        else
            str += tgui::to_string(layout.x.getValue());

        str += ", ";

        if (layout.y.getImpl()->operation == tgui::LayoutImpl::Operation::String)
            str += "\"" + layout.y.getImpl()->stringExpression + "\"";
        else
            str += tgui::to_string(layout.y.getValue());

        str += ")";
        return str;
    }
}

// Hidden functions
namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    #define SET_PROPERTY(property, value) node->propertyValuePairs[property] = std::make_shared<DataIO::ValueNode>(node.get(), value)

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::shared_ptr<DataIO::Node> saveWidget(Widget::Ptr widget)
    {
        sf::String widgetName;
        if (widget->getParent())
            widgetName = widget->getParent()->getWidgetName(widget);

        auto node = std::make_shared<DataIO::Node>();
        if (widgetName.isEmpty())
            node->name = widget->getWidgetType();
        else
            node->name = widget->getWidgetType() + "." + Serializer::serialize(widgetName);

        if (!widget->isVisible())
            SET_PROPERTY("Visible", "false");
        if (!widget->isEnabled())
            SET_PROPERTY("Enabled", "false");
        if (widget->getPosition() != sf::Vector2f{})
            SET_PROPERTY("Position", emitLayout(widget->getPositionLayout()));
        if (widget->getSize() != sf::Vector2f{})
        {
            /// TODO: Fix Grid and Tab to no longer override the getSize function
            if (widget->getSize() != widget->getSizeLayout().getValue())
                SET_PROPERTY("Size", emitLayout({widget->getSize()}));
            else
                SET_PROPERTY("Size", emitLayout(widget->getSizeLayout()));
        }
        if (widget->getOpacity() != 1)
            SET_PROPERTY("Opacity", tgui::to_string(widget->getOpacity()));

        /// TODO: Font and ToolTip

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

    TGUI_API std::shared_ptr<DataIO::Node> saveButton(Button::Ptr button)
    {
        auto node = saveWidget(button);

        if (!button->getText().isEmpty())
            SET_PROPERTY("Text", Serializer::serialize(button->getText()));

        SET_PROPERTY("TextSize", tgui::to_string(button->getTextSize()));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::shared_ptr<DataIO::Node> saveChatBox(ChatBox::Ptr chatBox)
    {
        auto node = saveWidget(chatBox);

        SET_PROPERTY("TextSize", tgui::to_string(chatBox->getTextSize()));
        SET_PROPERTY("TextColor", Serializer::serialize(chatBox->getTextColor()));

        if (chatBox->getLineLimit())
            SET_PROPERTY("LineLimit", tgui::to_string(chatBox->getLineLimit()));

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

            auto lineNode = std::make_shared<DataIO::Node>();
            lineNode->parent = node.get();
            lineNode->name = "Line";

            lineNode->propertyValuePairs["Text"] = std::make_shared<DataIO::ValueNode>(lineNode.get(), Serializer::serialize(chatBox->getLine(i)));
            if (lineTextSize != chatBox->getTextSize())
                lineNode->propertyValuePairs["TextSize"] = std::make_shared<DataIO::ValueNode>(lineNode.get(), tgui::to_string(lineTextSize));
            if (lineTextColor != chatBox->getTextColor())
                lineNode->propertyValuePairs["Color"] = std::make_shared<DataIO::ValueNode>(lineNode.get(), Serializer::serialize(lineTextColor));

            node->children.push_back(lineNode);
        }

        if (chatBox->getScrollbar())
        {
            node->children.push_back(WidgetSaver::getSaveFunction("scrollbar")(tgui::WidgetConverter{chatBox->getScrollbar()}));
            node->children.back()->parent = node.get();
            node->children.back()->name = "Scrollbar";
        }

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::shared_ptr<DataIO::Node> saveChildWindow(ChildWindow::Ptr childWindow)
    {
        auto node = saveContainer(childWindow);

        if (childWindow->getTitleAlignment() == ChildWindow::TitleAlignment::Left)
            SET_PROPERTY("TitleAlignment", "Left");
        else if (childWindow->getTitleAlignment() == ChildWindow::TitleAlignment::Center)
            SET_PROPERTY("TitleAlignment", "Center");
        else if (childWindow->getTitleAlignment() == ChildWindow::TitleAlignment::Right)
            SET_PROPERTY("TitleAlignment", "Right");

        if (childWindow->getTitle().getSize() > 0)
            SET_PROPERTY("Title", Serializer::serialize(childWindow->getTitle()));

        if (childWindow->getIcon().isLoaded())
            SET_PROPERTY("Icon", Serializer::serialize(childWindow->getIcon()));

        if (childWindow->isKeptInParent())
            SET_PROPERTY("KeepInParent", "true");

        node->children.push_back(WidgetSaver::getSaveFunction("button")(tgui::WidgetConverter{childWindow->getCloseButton()}));
        node->children.back()->parent = node.get();
        node->children.back()->name = "CloseButton";

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::shared_ptr<DataIO::Node> saveComboBox(ComboBox::Ptr comboBox)
    {
        auto node = saveWidget(comboBox);

        SET_PROPERTY("ItemsToDisplay", tgui::to_string(comboBox->getItemsToDisplay()));

        node->children.push_back(WidgetSaver::getSaveFunction("listbox")(tgui::WidgetConverter{comboBox->getListBox()}));
        node->children.back()->parent = node.get();
        node->children.back()->name = "ListBox";

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::shared_ptr<DataIO::Node> saveEditBox(EditBox::Ptr editBox)
    {
        auto node = saveWidget(editBox);

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
            SET_PROPERTY("MaximumCharacters", tgui::to_string(editBox->getMaximumCharacters()));
        if (editBox->isTextWidthLimited())
            SET_PROPERTY("TextWidthLimited", "true");

        SET_PROPERTY("CaretWidth", tgui::to_string((int)editBox->getCaretWidth()));
        SET_PROPERTY("TextSize", tgui::to_string(editBox->getTextSize()));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::shared_ptr<DataIO::Node> saveKnob(Knob::Ptr knob)
    {
        auto node = saveWidget(knob);

        if (knob->getClockwiseTurning())
            SET_PROPERTY("ClockwiseTurning", "true");
        else
            SET_PROPERTY("ClockwiseTurning", "false");

        SET_PROPERTY("StartRotation", tgui::to_string(knob->getStartRotation()));
        SET_PROPERTY("EndRotation", tgui::to_string(knob->getEndRotation()));
        SET_PROPERTY("Minimum", tgui::to_string(knob->getMinimum()));
        SET_PROPERTY("Maximum", tgui::to_string(knob->getMaximum()));
        SET_PROPERTY("Value", tgui::to_string(knob->getValue()));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::shared_ptr<DataIO::Node> saveLabel(Label::Ptr label)
    {
        auto node = saveWidget(label);

        if (label->getTextStyle() != sf::Text::Regular)
        {
            std::string style;
            if (label->getTextStyle() & sf::Text::Bold)
                style += " | Bold";
            if (label->getTextStyle() & sf::Text::Italic)
                style += " | Italic";
            if (label->getTextStyle() & sf::Text::Underlined)
                style += " | Underlined";
            if (label->getTextStyle() & sf::Text::StrikeThrough)
                style += " | StrikeThrough";

            if (!style.empty())
                SET_PROPERTY("TextStyle", style.substr(3));
        }

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
            SET_PROPERTY("MaximumTextWidth", tgui::to_string(label->getMaximumTextWidth()));
        if (label->getAutoSize())
            SET_PROPERTY("AutoSize", "true");

        SET_PROPERTY("TextSize", tgui::to_string(label->getTextSize()));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::shared_ptr<DataIO::Node> saveListBox(ListBox::Ptr listBox)
    {
        auto node = saveWidget(listBox);

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

        SET_PROPERTY("ItemHeight", tgui::to_string(listBox->getItemHeight()));
        SET_PROPERTY("MaximumItems", tgui::to_string(listBox->getMaximumItems()));

        if (listBox->getScrollbar() != nullptr)
        {
            node->children.push_back(WidgetSaver::getSaveFunction("scrollbar")(tgui::WidgetConverter{listBox->getScrollbar()}));
            node->children.back()->parent = node.get();
            node->children.back()->name = "Scrollbar";
        }

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::shared_ptr<DataIO::Node> savePicture(Picture::Ptr picture)
    {
        auto node = saveWidget(picture);

        if (!picture->getLoadedFilename().isEmpty())
            SET_PROPERTY("Filename", Serializer::serialize(sf::String{picture->getLoadedFilename()}));
        if (picture->isSmooth())
            SET_PROPERTY("Smooth", "true");

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::shared_ptr<DataIO::Node> saveProgressBar(ProgressBar::Ptr progressBar)
    {
        auto node = saveWidget(progressBar);

        if (!progressBar->getText().isEmpty())
            SET_PROPERTY("Text", Serializer::serialize(progressBar->getText()));

        if (progressBar->getFillDirection() != tgui::ProgressBar::FillDirection::LeftToRight)
        {
            if (progressBar->getFillDirection() == tgui::ProgressBar::FillDirection::RightToLeft)
                SET_PROPERTY("FillDirection", "RightToLeft");
            else if (progressBar->getFillDirection() == tgui::ProgressBar::FillDirection::TopToBottom)
                SET_PROPERTY("FillDirection", "TopToBottom");
            else if (progressBar->getFillDirection() == tgui::ProgressBar::FillDirection::BottomToTop)
                SET_PROPERTY("FillDirection", "BottomToTop");
        }

        SET_PROPERTY("Minimum", tgui::to_string(progressBar->getMinimum()));
        SET_PROPERTY("Maximum", tgui::to_string(progressBar->getMaximum()));
        SET_PROPERTY("Value", tgui::to_string(progressBar->getValue()));
        SET_PROPERTY("TextSize", tgui::to_string(progressBar->getTextSize()));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::shared_ptr<DataIO::Node> saveRadioButton(RadioButton::Ptr radioButton)
    {
        auto node = saveWidget(radioButton);

        if (!radioButton->getText().isEmpty())
            SET_PROPERTY("Text", Serializer::serialize(radioButton->getText()));
        if (radioButton->isChecked())
            SET_PROPERTY("Checked", "true");

        SET_PROPERTY("TextSize", tgui::to_string(radioButton->getTextSize()));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::shared_ptr<DataIO::Node> saveScrollbar(Scrollbar::Ptr scrollbar)
    {
        auto node = saveWidget(scrollbar);

        if (scrollbar->getAutoHide())
            SET_PROPERTY("AutoHide", "true");
        else
            SET_PROPERTY("AutoHide", "false");

        SET_PROPERTY("LowValue", tgui::to_string(scrollbar->getLowValue()));
        SET_PROPERTY("Maximum", tgui::to_string(scrollbar->getMaximum()));
        SET_PROPERTY("Value", tgui::to_string(scrollbar->getValue()));
        SET_PROPERTY("ArrowScrollAmount", tgui::to_string(scrollbar->getArrowScrollAmount()));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::shared_ptr<DataIO::Node> saveSlider(Slider::Ptr slider)
    {
        auto node = saveWidget(slider);
        SET_PROPERTY("Minimum", tgui::to_string(slider->getMinimum()));
        SET_PROPERTY("Maximum", tgui::to_string(slider->getMaximum()));
        SET_PROPERTY("Value", tgui::to_string(slider->getValue()));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::shared_ptr<DataIO::Node> saveSpinButton(SpinButton::Ptr spinButton)
    {
        auto node = saveWidget(spinButton);

        if (spinButton->getVerticalScroll())
            SET_PROPERTY("VerticalScroll", "true");
        else
            SET_PROPERTY("VerticalScroll", "false");

        SET_PROPERTY("Minimum", tgui::to_string(spinButton->getMinimum()));
        SET_PROPERTY("Maximum", tgui::to_string(spinButton->getMaximum()));
        SET_PROPERTY("Value", tgui::to_string(spinButton->getValue()));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::shared_ptr<DataIO::Node> saveTab(Tab::Ptr tab)
    {
        auto node = saveWidget(tab);

        if (tab->getTabsCount() > 0)
        {
            std::string tabList = "[" + Serializer::serialize(tab->getText(0));
            for (std::size_t i = 1; i < tab->getTabsCount(); ++i)
                tabList += ", " + Serializer::serialize(tab->getText(i));

            tabList += "]";
            SET_PROPERTY("Tabs", tabList);
        }

        if (tab->getSelectedIndex() >= 0)
            SET_PROPERTY("Selected", tgui::to_string(tab->getSelectedIndex()));

        if (tab->getMaximumTabWidth() > 0)
            SET_PROPERTY("MaximumTabWidth", tgui::to_string(tab->getMaximumTabWidth()));

        SET_PROPERTY("TextSize", tgui::to_string(tab->getTextSize()));
        SET_PROPERTY("TabHeight", tgui::to_string(tab->getTabHeight()));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TGUI_API std::shared_ptr<DataIO::Node> saveTextBox(TextBox::Ptr textBox)
    {
        auto node = saveWidget(textBox);

        SET_PROPERTY("Text", Serializer::serialize(textBox->getText()));
        SET_PROPERTY("TextSize", tgui::to_string(textBox->getTextSize()));
        SET_PROPERTY("MaximumCharacters", tgui::to_string(textBox->getMaximumCharacters()));

        if (textBox->isReadOnly())
            SET_PROPERTY("ReadOnly", "true");

        if (textBox->getScrollbar() != nullptr)
        {
            node->children.push_back(WidgetSaver::getSaveFunction("scrollbar")(tgui::WidgetConverter{textBox->getScrollbar()}));
            node->children.back()->parent = node.get();
            node->children.back()->name = "Scrollbar";
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
            {"container", saveContainer},
            {"button", saveButton},
            {"canvas", saveWidget},
            {"chatbox", saveChatBox},
            {"checkbox", saveRadioButton},
            {"clickablewidget", saveWidget},
            {"childwindow", saveChildWindow},
            {"combobox", saveComboBox},
            {"editbox", saveEditBox},
            {"knob", saveKnob},
            {"label", saveLabel},
            {"listbox", saveListBox},
            {"panel", saveContainer},
            {"picture", savePicture},
            {"progressbar", saveProgressBar},
            {"radiobutton", saveRadioButton},
            {"scrollbar", saveScrollbar},
            {"slider", saveSlider},
            {"spinbutton", saveSpinButton},
            {"tab", saveTab},
            {"textbox", saveTextBox}
        };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void WidgetSaver::save(Container::Ptr widget, std::stringstream& stream)
    {
        auto node = std::make_shared<DataIO::Node>();
        for (auto& child : widget->getWidgets())
        {
            auto& saveFunction = WidgetSaver::getSaveFunction(toLower(child->getWidgetType()));
            if (saveFunction)
                node->children.emplace_back(saveFunction(WidgetConverter{child}));
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
