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


#include <TGUI/Widgets/MessageBox.hpp>
#include <TGUI/Loading/Theme.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    static std::map<std::string, ObjectConverter> defaultRendererValues =
            {
                {"borders", Borders{1}},
                {"bordercolor", sf::Color::Black},
                {"titlecolor", sf::Color::Black},
                {"titlebarcolor", sf::Color::White},
                {"backgroundcolor", Color{230, 230, 230}},
                {"distancetoside", 3.f},
                {"paddingbetweenbuttons", 1.f},
                {"textcolor", sf::Color::Black}
            };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::MessageBox()
    {
        m_type = "MessageBox";

        m_renderer = aurora::makeCopied<MessageBoxRenderer>();
        setRenderer(RendererData::create(defaultRendererValues));

        getRenderer()->getCloseButton()->propertyValuePairs["borders"] = {Borders{1}};
        getRenderer()->getMaximizeButton()->propertyValuePairs["borders"] = {Borders{1}};
        getRenderer()->getMinimizeButton()->propertyValuePairs["borders"] = {Borders{1}};

        add(m_label, "#TGUI_INTERNAL$MessageBoxText#");
        m_label->setTextSize(m_textSize);

        setTitleButtons(ChildWindow::TitleButton::None);
        setSize({400, 150});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::MessageBox(const MessageBox& other) :
        ChildWindow      {other},
        onButtonPress    {other.onButtonPress},
        m_loadedThemeFile(other.m_loadedThemeFile), // Did not compile in VS2013 when using braces
        m_buttonClassName(other.m_buttonClassName), // Did not compile in VS2013 when using braces
        m_textSize       {other.m_textSize}
    {
        m_label = get<tgui::Label>("#TGUI_INTERNAL$MessageBoxText#");

        for (unsigned int i = 0; i < m_widgets.size(); ++i)
        {
            if ((m_widgetNames[i].getSize() > 32) && (m_widgetNames[i].substring(0, 32) == "#TGUI_INTERNAL$MessageBoxButton:"))
            {
                auto button = std::dynamic_pointer_cast<Button>(m_widgets[i]);

                button->onPress->disconnectAll();
                button->onPress->connect([=]() { onButtonPress->emit(this, button->getText()); });
                m_buttons.push_back(button);
            }
        }

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox& MessageBox::operator= (const MessageBox& other)
    {
        if (this != &other)
        {
            MessageBox temp(other);
            ChildWindow::operator=(temp);

            std::swap(onButtonPress,     temp.onButtonPress);
            std::swap(m_loadedThemeFile, temp.m_loadedThemeFile);
            std::swap(m_buttonClassName, temp.m_buttonClassName);
            std::swap(m_buttons,         temp.m_buttons);
            std::swap(m_label,           temp.m_label);
            std::swap(m_textSize,        temp.m_textSize);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::Ptr MessageBox::create()
    {
        return std::make_shared<MessageBox>();
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

    void MessageBox::setText(const sf::String& text)
    {
        m_label->setText(text);

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::String& MessageBox::getText() const
    {
        return m_label->getText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::setTextSize(unsigned int size)
    {
        m_textSize = size;

        m_label->setTextSize(size);

        for (auto& button : m_buttons)
            button->setTextSize(m_textSize);

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int MessageBox::getTextSize() const
    {
        return m_textSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::addButton(const sf::String& caption)
    {
        auto button = Button::create(caption);
        button->setRenderer(getRenderer()->getButton());
        button->setTextSize(m_textSize);
        button->onPress->connect([=]() { onButtonPress->emit(this, caption); });

        add(button, "#TGUI_INTERNAL$MessageBoxButton:" + caption + "#");
        m_buttons.push_back(button);

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<sf::String> MessageBox::getButtons() const
    {
        std::vector<sf::String> buttonTexts;
        for (auto& button : m_buttons)
            buttonTexts.emplace_back(button->getText());

        return buttonTexts;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::rearrange()
    {
        float buttonWidth = 120;
        float buttonHeight = 24;

        // Calculate the button size
        if (m_fontCached)
        {
            buttonWidth = 5.0f * m_fontCached.getLineSpacing(m_textSize);
            buttonHeight = m_fontCached.getLineSpacing(m_textSize) / 0.85f;

            for (const auto& button : m_buttons)
            {
                const float width = sf::Text(button->getText(), *m_fontCached.getFont(), m_textSize).getLocalBounds().width;
                if (buttonWidth < width * 10.0f / 9.0f)
                    buttonWidth = width * 10.0f / 9.0f;
            }
        }

        // Calculate the space needed for the buttons
        const float distance = buttonHeight * 2.0f / 3.0f;
        float buttonsAreaWidth = distance;
        for (auto& button : m_buttons)
        {
            button->setSize({buttonWidth, buttonHeight});
            buttonsAreaWidth += button->getSize().x + distance;
        }

        // Calculate the suggested size of the window
        sf::Vector2f size = {2*distance + m_label->getSize().x, 3*distance + m_label->getSize().y + buttonHeight};

        // Make sure the buttons fit inside the message box
        if (buttonsAreaWidth > size.x)
            size.x = buttonsAreaWidth;

        // Set the size of the window
        setSize(size);

        // Set the text on the correct position
        m_label->setPosition({distance, distance});

        // Set the buttons on the correct position
        float leftPosition = 0;
        const float topPosition = 2*distance + m_label->getSize().y;
        for (auto& button : m_buttons)
        {
            leftPosition += distance + ((size.x - buttonsAreaWidth) / (m_buttons.size()+1));
            button->setPosition({leftPosition, topPosition});
            leftPosition += button->getSize().x;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& MessageBox::getSignal(std::string&& signalName)
    {
        if (signalName == toLower(onButtonPress->getName()))
            return *onButtonPress;
        else
            return ChildWindow::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::rendererChanged(const std::string& property)
    {
        if (property == "textcolor")
        {
            m_label->getRenderer()->setTextColor(getRenderer()->getTextColor());
        }
        else if (property == "button")
        {
            const auto& renderer = getRenderer()->getButton();
            for (auto& button : m_buttons)
                button->setRenderer(renderer);
        }
        else if (property == "font")
        {
            ChildWindow::rendererChanged(property);

            m_label->getRenderer()->setFont(m_fontCached);

            for (auto& button : m_buttons)
                button->getRenderer()->setFont(m_fontCached);

            rearrange();
        }
        else
            ChildWindow::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
