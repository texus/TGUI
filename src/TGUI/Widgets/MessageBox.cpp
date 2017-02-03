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


#include <TGUI/Widgets/MessageBox.hpp>
#include <TGUI/Loading/Theme.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::MessageBox()
    {
        m_callback.widgetType = "MessageBox";

        addSignal<sf::String>("ButtonPressed");

        m_renderer = std::make_shared<MessageBoxRenderer>(this);
        reload();

        add(m_label, "#TGUI_INTERNAL$MessageBoxText#");
        m_label->setTextSize(m_textSize);

        setSize({400, 150});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::MessageBox(const MessageBox& messageBoxToCopy) :
        ChildWindow      {messageBoxToCopy},
        m_loadedThemeFile(messageBoxToCopy.m_loadedThemeFile), // Did not compile in VS2013 when using braces
        m_buttonClassName(messageBoxToCopy.m_buttonClassName), // Did not compile in VS2013 when using braces
        m_textSize       {messageBoxToCopy.m_textSize}
    {
        m_label = Label::copy(messageBoxToCopy.m_label);
        add(m_label, "#TGUI_INTERNAL$MessageBoxText#");

        for (auto it = messageBoxToCopy.m_buttons.begin(); it != messageBoxToCopy.m_buttons.end(); ++it)
        {
            Button::Ptr button = Button::copy(*it);
            button->disconnectAll();
            button->connect("Pressed", [=]() { m_callback.text = button->getText(); sendSignal("ButtonPressed", button->getText()); });

            m_buttons.push_back(button);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox& MessageBox::operator= (const MessageBox& right)
    {
        if (this != &right)
        {
            MessageBox temp(right);
            ChildWindow::operator=(right);

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

    void MessageBox::setFont(const Font& font)
    {
        ChildWindow::setFont(font);
        m_label->setFont(font);

        for (unsigned int i = 0; i < m_buttons.size(); ++i)
            m_buttons[i]->setFont(font);

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::setText(const sf::String& text)
    {
        m_label->setText(text);

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::setTextSize(unsigned int size)
    {
        m_textSize = size;

        m_label->setTextSize(size);

        for (unsigned int i = 0; i < m_buttons.size(); ++i)
            m_buttons[i]->setTextSize(m_textSize);

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::addButton(const sf::String& caption)
    {
        Button::Ptr button;
        if (!getTheme() || m_buttonClassName.empty())
            button = std::make_shared<Button>();
        else
            button = getTheme()->internalLoad(getPrimaryLoadingParameter(), m_buttonClassName);

        button->setTextSize(m_textSize);
        button->setText(caption);
        button->connect("Pressed", [=](){ m_callback.text = caption; sendSignal("ButtonPressed", caption); });

        add(button, "#TGUI_INTERNAL$MessageBoxButton$" + caption + "#");
        m_buttons.push_back(button);

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::rearrange()
    {
        float buttonWidth = 120;
        float buttonHeight = 24;

        // Calculate the button size
        if (getFont())
        {
            buttonWidth = 5.0f * getFont()->getLineSpacing(m_textSize);
            buttonHeight = getFont()->getLineSpacing(m_textSize) / 0.85f;

            for (unsigned int i = 0; i < m_buttons.size(); ++i)
            {
                float width = sf::Text(m_buttons[i]->getText(), *getFont(), m_textSize).getLocalBounds().width;
                if (buttonWidth < width * 10.0f / 9.0f)
                    buttonWidth = width * 10.0f / 9.0f;
            }
        }

        // Calculate the space needed for the buttons
        float distance = buttonHeight * 2.0f / 3.0f;
        float buttonsAreaWidth = distance;
        for (unsigned int i = 0; i < m_buttons.size(); ++i)
        {
            m_buttons[i]->setSize({buttonWidth, buttonHeight});
            buttonsAreaWidth += m_buttons[i]->getSize().x + distance;
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
        float topPosition = 2*distance + m_label->getSize().y;
        for (unsigned int i = 0; i < m_buttons.size(); ++i)
        {
            leftPosition += distance + ((size.x - buttonsAreaWidth) / (m_buttons.size()+1));
            m_buttons[i]->setPosition({leftPosition, topPosition});
            leftPosition += m_buttons[i]->getSize().x;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::reload(const std::string& primary, const std::string& secondary, bool force)
    {
        ChildWindow::reload(primary, secondary, force);

        if (!m_theme || primary == "")
            getRenderer()->setTextColor({0, 0, 0});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBoxRenderer::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "textcolor")
            setTextColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "button")
            m_messageBox->m_buttonClassName = Deserializer::deserialize(ObjectConverter::Type::String, value).getString();
        else if (property == "childwindow")
        {
            if (m_messageBox->getTheme() == nullptr)
                throw Exception{"Failed to load scrollbar, ChatBox has no connected theme to load the scrollbar with"};

            tgui::ChildWindow::Ptr childWindow = m_messageBox->getTheme()->internalLoad(
                                                        m_messageBox->m_primaryLoadingParameter,
                                                        Deserializer::deserialize(ObjectConverter::Type::String, value).getString()
                                                    );

            for (auto& pair : childWindow->getRenderer()->getPropertyValuePairs())
                setProperty(pair.first, std::move(pair.second));
        }
        else
            ChildWindowRenderer::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBoxRenderer::setProperty(std::string property, ObjectConverter&& value)
    {
        property = toLower(property);

        if (value.getType() == ObjectConverter::Type::Color)
        {
            if (property == "textcolor")
                setTextColor(value.getColor());
            else
                ChildWindowRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::String)
        {
            if (property == "button")
                m_messageBox->m_buttonClassName = value.getString();
            else if (property == "childwindow")
            {
                if (m_messageBox->getTheme() == nullptr)
                    throw Exception{"Failed to load scrollbar, ChatBox has no connected theme to load the scrollbar with"};

                tgui::ChildWindow::Ptr childWindow = m_messageBox->getTheme()->internalLoad(m_messageBox->m_primaryLoadingParameter, value.getString());

                for (auto& pair : childWindow->getRenderer()->getPropertyValuePairs())
                    setProperty(pair.first, std::move(pair.second));
            }
            else
                ChildWindowRenderer::setProperty(property, std::move(value));
        }
        else
            ChildWindowRenderer::setProperty(property, std::move(value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter MessageBoxRenderer::getProperty(std::string property) const
    {
        property = toLower(property);

        if (property == "textcolor")
            return m_messageBox->m_label->getTextColor();
        else
            return ChildWindowRenderer::getProperty(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, ObjectConverter> MessageBoxRenderer::getPropertyValuePairs() const
    {
        auto pairs = ChildWindowRenderer::getPropertyValuePairs();
        pairs["TextColor"] = m_messageBox->m_label->getTextColor();
        return pairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBoxRenderer::setTextColor(const Color& color)
    {
        m_messageBox->m_label->setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> MessageBoxRenderer::clone(Widget* widget)
    {
        auto renderer = std::make_shared<MessageBoxRenderer>(*this);
        renderer->m_messageBox = static_cast<MessageBox*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
