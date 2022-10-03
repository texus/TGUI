/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Widgets/MessageBox.hpp>
#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::MessageBox(const char* typeName, bool initRenderer) :
        ChildWindow{typeName, false}
    {
        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<MessageBoxRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }

        setTitleButtons(ChildWindow::TitleButton::None);
        setTextSize(getGlobalTextSize());

        add(m_label, "#TGUI_INTERNAL$MessageBoxText#");
        m_label->setTextSize(m_textSizeCached);

        setClientSize({400, 150});
        m_autoSize = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::MessageBox(const MessageBox& other) :
        ChildWindow      {other},
        onButtonPress    {other.onButtonPress},
        m_loadedThemeFile{other.m_loadedThemeFile},
        m_buttonClassName{other.m_buttonClassName},
        m_autoSize       {other.m_autoSize},
        m_labelAlignment {other.m_labelAlignment},
        m_buttonAlignment{other.m_buttonAlignment}
    {
        identifyLabelAndButtons();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::MessageBox(MessageBox&& other) noexcept :
        ChildWindow      {std::move(other)},
        onButtonPress    {std::move(other.onButtonPress)},
        m_loadedThemeFile{std::move(other.m_loadedThemeFile)},
        m_buttonClassName{std::move(other.m_buttonClassName)},
        m_autoSize       {std::move(other.m_autoSize)},
        m_labelAlignment {std::move(other.m_labelAlignment)},
        m_buttonAlignment{std::move(other.m_buttonAlignment)},
        m_buttons        {std::move(other.m_buttons)},
        m_label          {std::move(other.m_label)}
    {
        for (std::size_t i = 0; i < m_buttons.size(); ++i)
            connectButtonPressSignal(i);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox& MessageBox::operator= (const MessageBox& other)
    {
        if (this != &other)
        {
            ChildWindow::operator=(other);
            onButtonPress     = other.onButtonPress;
            m_loadedThemeFile = other.m_loadedThemeFile;
            m_buttonClassName = other.m_buttonClassName;
            m_autoSize        = other.m_autoSize;
            m_labelAlignment  = other.m_labelAlignment;
            m_buttonAlignment = other.m_buttonAlignment;

            identifyLabelAndButtons();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox& MessageBox::operator= (MessageBox&& other) noexcept
    {
        if (this != &other)
        {
            ChildWindow::operator=(std::move(other));

            onButtonPress     = std::move(other.onButtonPress);
            m_loadedThemeFile = std::move(other.m_loadedThemeFile);
            m_buttonClassName = std::move(other.m_buttonClassName);
            m_autoSize        = std::move(other.m_autoSize);
            m_labelAlignment  = std::move(other.m_labelAlignment);
            m_buttonAlignment = std::move(other.m_buttonAlignment);
            m_buttons         = std::move(other.m_buttons);
            m_label           = std::move(other.m_label);

            for (std::size_t i = 0; i < m_buttons.size(); ++i)
                connectButtonPressSignal(i);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::Ptr MessageBox::create(String title, String text, std::vector<String> buttons)
    {
        auto messageBox = std::make_shared<MessageBox>();
        messageBox->setTitle(title);
        messageBox->setText(text);
        for (auto& buttonText : buttons)
            messageBox->addButton(std::move(buttonText));

        return messageBox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::Ptr MessageBox::copy(MessageBox::ConstPtr messageBox)
    {
        if (messageBox)
            return std::static_pointer_cast<MessageBox>(messageBox->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBoxRenderer* MessageBox::getSharedRenderer()
    {
        return aurora::downcast<MessageBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const MessageBoxRenderer* MessageBox::getSharedRenderer() const
    {
        return aurora::downcast<const MessageBoxRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBoxRenderer* MessageBox::getRenderer()
    {
        return aurora::downcast<MessageBoxRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::setSize(const Layout2d& size)
    {
        // Provide a way to re-enable auto-size after a manual size was previously provided
        if ((size.x.isConstant() && size.x.getValue() == 0) && (size.y.isConstant() && size.y.getValue() == 0))
        {
            m_autoSize = true;
            rearrange();
            return;
        }

        ChildWindow::setSize(size);

        m_autoSize = false;
        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::setClientSize(const Layout2d& size)
    {
        // Provide a way to re-enable auto-size after a manual size was previously provided
        if ((size.x.isConstant() && size.x.getValue() == 0) && (size.y.isConstant() && size.y.getValue() == 0))
        {
            m_autoSize = true;
            rearrange();
            return;
        }

        ChildWindow::setClientSize(size);

        m_autoSize = false;
        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::setText(const String& text)
    {
        m_label->setText(text);

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const String& MessageBox::getText() const
    {
        return m_label->getText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::updateTextSize()
    {
        m_label->setTextSize(m_textSizeCached);

        for (auto& button : m_buttons)
            button->setTextSize(m_textSizeCached);

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::addButton(const String& caption)
    {
        auto button = Button::create(caption);
        button->setRenderer(getSharedRenderer()->getButton());
        button->setTextSize(m_textSizeCached);

        add(button, "#TGUI_INTERNAL$MessageBoxButton:" + caption + "#");
        m_buttons.push_back(button);

        connectButtonPressSignal(m_buttons.size() - 1);
        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<String> MessageBox::getButtons() const
    {
        std::vector<String> buttonTexts;
        for (auto& button : m_buttons)
            buttonTexts.emplace_back(button->getText());

        return buttonTexts;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::setLabelAlignment(Alignment labelAlignment)
    {
        m_labelAlignment = labelAlignment;
        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::Alignment MessageBox::getLabelAlignment() const
    {
        return m_labelAlignment;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::setButtonAlignment(Alignment buttonAlignment)
    {
        m_buttonAlignment = buttonAlignment;
        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::Alignment MessageBox::getButtonAlignment() const
    {
        return m_buttonAlignment;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::rearrange()
    {
        float buttonWidth = 120;
        float buttonHeight = 24;

        // Calculate the button size
        if (m_fontCached)
        {
            buttonWidth = 4.0f * Text::getLineHeight(m_fontCached, m_textSizeCached);
            buttonHeight = Text::getLineHeight(m_fontCached, m_textSizeCached) * 1.25f;

            for (const auto& button : m_buttons)
            {
                /// TODO: Implement a way to calculate text size without creating a text object?
                Text tempText;
                tempText.setFont(m_fontCached);
                tempText.setCharacterSize(m_textSizeCached);
                tempText.setString(button->getText());
                const float width = tempText.getSize().x;
                if (buttonWidth < width * 10.0f / 9.0f)
                    buttonWidth = width * 10.0f / 9.0f;
            }
        }

        // Calculate the space needed for the buttons
        const float distance = std::round(buttonHeight * 2.0f / 3.0f);
        float buttonsAreaWidth = distance;
        for (auto& button : m_buttons)
        {
            button->setSize({buttonWidth, buttonHeight});
            buttonsAreaWidth += button->getSize().x + distance;
        }

        // Calculate the suggested size of the window
        Vector2f size = {2*distance + m_label->getSize().x, 3*distance + m_label->getSize().y + buttonHeight};

        // Make sure the buttons fit inside the message box
        if (buttonsAreaWidth > size.x)
            size.x = buttonsAreaWidth;

        // Set the size of the window
        if (m_autoSize)
        {
            setClientSize(size);
            m_autoSize = true;
        }

        // Set the text on the correct position
        if (m_labelAlignment == Alignment::Left)
        {
            m_label->setPosition({distance, distance});
        }
        else if (m_labelAlignment == Alignment::Right)
        {
            m_label->setPosition({getClientSize().x - distance - m_label->getSize().x, distance});
        }
        else // if (m_labelAlignment == Alignment::Center)
        {
            m_label->setPosition({(getClientSize().x - m_label->getSize().x) / 2.f, distance});
        }

        // Set the buttons on the correct position
        const float topPosition = getClientSize().y - distance - buttonHeight;
        if (m_buttonAlignment == Alignment::Left)
        {
            float leftPosition = distance;
            for (auto& button : m_buttons)
            {
                button->setPosition({leftPosition, topPosition});
                leftPosition += button->getSize().x + distance;
            }
        }
        else if (m_buttonAlignment == Alignment::Right)
        {
            float leftPosition = getClientSize().x;
            for (auto& button : m_buttons)
            {
                leftPosition -= distance + button->getSize().x;
                button->setPosition({leftPosition, topPosition});
            }
        }
        else // if (m_buttonAlignment == Alignment::Center)
        {
            float leftPosition = 0;
            for (auto& button : m_buttons)
            {
                leftPosition += distance + ((getClientSize().x - buttonsAreaWidth) / (m_buttons.size()+1));
                button->setPosition({leftPosition, topPosition});
                leftPosition += button->getSize().x;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& MessageBox::getSignal(String signalName)
    {
        if (signalName == onButtonPress.getName())
            return onButtonPress;
        else
            return ChildWindow::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::rendererChanged(const String& property)
    {
        if (property == "TextColor")
        {
            m_label->getRenderer()->setTextColor(getSharedRenderer()->getTextColor());
        }
        else if (property == "Button")
        {
            const auto& renderer = getSharedRenderer()->getButton();
            for (auto& button : m_buttons)
                button->setRenderer(renderer);
        }
        else if (property == "Font")
        {
            ChildWindow::rendererChanged(property);

            m_label->setInheritedFont(m_fontCached);

            for (auto& button : m_buttons)
                button->setInheritedFont(m_fontCached);

            rearrange();
        }
        else
            ChildWindow::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> MessageBox::save(SavingRenderersMap& renderers) const
    {
        auto node = Container::save(renderers);

        node->propertyValuePairs["AutoSize"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_autoSize));

        if (m_labelAlignment == Alignment::Left)
            node->propertyValuePairs["LabelAlignment"] = std::make_unique<DataIO::ValueNode>("Left");
        else if (m_labelAlignment == Alignment::Center)
            node->propertyValuePairs["LabelAlignment"] = std::make_unique<DataIO::ValueNode>("Center");
        else if (m_labelAlignment == Alignment::Right)
            node->propertyValuePairs["LabelAlignment"] = std::make_unique<DataIO::ValueNode>("Right");

        if (m_buttonAlignment == Alignment::Left)
            node->propertyValuePairs["ButtonAlignment"] = std::make_unique<DataIO::ValueNode>("Left");
        else if (m_buttonAlignment == Alignment::Center)
            node->propertyValuePairs["ButtonAlignment"] = std::make_unique<DataIO::ValueNode>("Center");
        else if (m_buttonAlignment == Alignment::Right)
            node->propertyValuePairs["ButtonAlignment"] = std::make_unique<DataIO::ValueNode>("Right");

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        // Remove the label that the MessageBox constructor creates because it will be created when loading the child window
        removeAllWidgets();

        ChildWindow::load(node, renderers);

        if (node->propertyValuePairs["AutoSize"])
            m_autoSize = Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs["AutoSize"]->value).getBool();
        else
            m_autoSize = true;

        if (node->propertyValuePairs["LabelAlignment"])
        {
            if (node->propertyValuePairs["LabelAlignment"]->value == "Left")
                setLabelAlignment(Alignment::Left);
            else if (node->propertyValuePairs["LabelAlignment"]->value == "Center")
                setLabelAlignment(Alignment::Center);
            else if (node->propertyValuePairs["LabelAlignment"]->value == "Right")
                setLabelAlignment(Alignment::Right);
            else
                throw Exception{"Failed to parse LabelAlignment property. Only the values Left, Center and Right are correct."};
        }

        if (node->propertyValuePairs["ButtonAlignment"])
        {
            if (node->propertyValuePairs["ButtonAlignment"]->value == "Left")
                setButtonAlignment(Alignment::Left);
            else if (node->propertyValuePairs["ButtonAlignment"]->value == "Center")
                setButtonAlignment(Alignment::Center);
            else if (node->propertyValuePairs["ButtonAlignment"]->value == "Right")
                setButtonAlignment(Alignment::Right);
            else
                throw Exception{"Failed to parse ButtonAlignment property. Only the values Left, Center and Right are correct."};
        }

        identifyLabelAndButtons();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::identifyLabelAndButtons()
    {
        m_label = get<Label>("#TGUI_INTERNAL$MessageBoxText#");

        for (unsigned int i = 0; i < m_widgets.size(); ++i)
        {
            if ((m_widgets[i]->getWidgetName().length() >= 32) && (m_widgets[i]->getWidgetName().substr(0, 32) == "#TGUI_INTERNAL$MessageBoxButton:"))
            {
                auto button = std::dynamic_pointer_cast<Button>(m_widgets[i]);
                m_buttons.push_back(button);
                connectButtonPressSignal(m_buttons.size() - 1);
            }
        }

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::connectButtonPressSignal(std::size_t buttonIndex)
    {
        TGUI_ASSERT(buttonIndex < m_buttons.size(), "Index shouldn't be out-of-range");
        m_buttons[buttonIndex]->onPress.disconnectAll();
        m_buttons[buttonIndex]->onPress(TGUI_LAMBDA_CAPTURE_EQ_THIS{
            // We can't copy button into this lambda because it would cause a memory leak.
            // We can however copy the index and access the button from m_buttons via the copied this pointer.
            onButtonPress.emit(this, m_buttons[buttonIndex]->getText());
        });
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr MessageBox::clone() const
    {
        return std::make_shared<MessageBox>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
