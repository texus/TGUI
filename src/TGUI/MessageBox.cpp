/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2014 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/MessageBox.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::MessageBox()
    {
        m_callback.widgetType = WidgetType::MessageBox;

        addSignal<SignalString>("ButtonPressed");

        m_renderer = std::make_shared<MessageBoxRenderer>(this);

        getRenderer()->setBorders({1, 1, 1, 1});

        add(m_label, "#TGUI_INTERNAL$MessageBoxText#");
        m_label->setTextSize(m_textSize);

        setSize({400, 150});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::MessageBox(const MessageBox& messageBoxToCopy) :
        ChildWindow        {messageBoxToCopy},
        m_loadedThemeFile  (messageBoxToCopy.m_loadedThemeFile), // Did not compile in VS2013 when using braces
        m_buttonSectionName(messageBoxToCopy.m_buttonSectionName), // Did not compile in VS2013 when using braces
        m_textSize         {messageBoxToCopy.m_textSize}
    {
        m_label = Label::copy(messageBoxToCopy.m_label);
        add(m_label, "#TGUI_INTERNAL$MessageBoxText#");

        for (auto it = messageBoxToCopy.m_buttons.begin(); it != messageBoxToCopy.m_buttons.end(); ++it)
        {
            Button::Ptr button = Button::copy(*it);
            button->disconnectAll();
            button->connect("Pressed", [=]() { m_callback.text = button->getText(); sendSignal("ButtonClicked", button->getText()); });

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

            std::swap(m_loadedThemeFile,   temp.m_loadedThemeFile);
            std::swap(m_buttonSectionName, temp.m_buttonSectionName);
            std::swap(m_buttons,           temp.m_buttons);
            std::swap(m_label,             temp.m_label);
            std::swap(m_textSize,          temp.m_textSize);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::Ptr MessageBox::create(const std::string& themeFileFilename, const std::string& section)
    {
        auto messageBox = std::make_shared<MessageBox>();

        if (themeFileFilename != "")
        {
            messageBox->m_loadedThemeFile = themeFileFilename;
            std::string loadedThemeFile = getResourcePath() + themeFileFilename;

            // Open the theme file
            ThemeFileParser themeFile{loadedThemeFile, section};

            // Find the folder that contains the theme file
            std::string themeFileFolder = "";
            std::string::size_type slashPos = loadedThemeFile.find_last_of("/\\");
            if (slashPos != std::string::npos)
                themeFileFolder = loadedThemeFile.substr(0, slashPos+1);

            // Handle the read properties
            for (auto it = themeFile.getProperties().cbegin(); it != themeFile.getProperties().cend(); ++it)
            {
                try
                {
                    if (it->first == "childwindow")
                    {
                        if ((it->second.length() < 3) || (it->second[0] != '"') || (it->second[it->second.length()-1] != '"'))
                            throw Exception{"Failed to parse value for ChildWindow property in section '" + section + "' in " + loadedThemeFile + "."};

                        try
                        {
                            messageBox->createChildWindow(messageBox->m_loadedThemeFile, it->second.substr(1, it->second.length()-2));
                        }
                        catch (const Exception& e)
                        {
                            throw Exception{"Failed to load the internal ChildWindow for MessageBox. " + std::string{e.what()}};
                        }
                    }
                    else
                        messageBox->getRenderer()->setProperty(it->first, it->second, themeFileFolder);
                }
                catch (const Exception& e)
                {
                    throw Exception{std::string(e.what()) + " In section '" + section + "' in " + loadedThemeFile + "."};
                }
            }

            if (messageBox->getRenderer()->m_textureTitleBar.getData())
                messageBox->getRenderer()->m_titleBarHeight = messageBox->getRenderer()->m_textureTitleBar.getImageSize().y;
        }

        if (!messageBox->m_closeButton.getRenderer()->m_textureNormal.getData())
        {
            messageBox->m_closeButton.setSize({messageBox->getRenderer()->m_titleBarHeight * 0.8f, messageBox->getRenderer()->m_titleBarHeight * 0.8f});
            messageBox->m_closeButton.getRenderer()->setBorders({1, 1, 1, 1});
            messageBox->m_closeButton.setText("X");
        }

        // Set the size of the title text
        messageBox->m_titleText.setTextSize(static_cast<unsigned int>(messageBox->getRenderer()->m_titleBarHeight * 0.8f));

        return messageBox;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    MessageBox::Ptr MessageBox::copy(MessageBox::ConstPtr messageBox)
    {
        if (messageBox)
            return std::make_shared<MessageBox>(*messageBox);
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
        auto button = Button::create(m_loadedThemeFile, m_buttonSectionName);
        button->setTextSize(m_textSize);
        button->setText(caption);
        button->connect("Pressed", [=](){ m_callback.text = caption; sendSignal("ButtonClicked", caption); });

        add(button, "#TGUI_INTERNAL$MessageBoxButton$" + caption + "#");
        m_buttons.push_back(button);

        rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::initialize(Container *const parent)
    {
        ChildWindow::initialize(parent);

        if (!m_font && m_parent->getGlobalFont())
            getRenderer()->setTextFont(m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBox::rearrange()
    {
        // Calculate the button size
        float buttonWidth = 5.0f * m_textSize;
        float buttonHeight = m_textSize * 10.0f / 8.0f;
        for (unsigned int i = 0; i < m_buttons.size(); ++i)
        {
            float width = sf::Text(m_buttons[i]->getText(), *getGlobalFont(), m_textSize).getLocalBounds().width;
            if (buttonWidth < width * 10.0f / 9.0f)
                buttonWidth = width * 10.0f / 9.0f;
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
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBoxRenderer::setProperty(std::string property, const std::string& value, const std::string&)
    {
        if (property == "button")
        {
            if ((value.length() < 3) || (value[0] != '"') || (value[value.length()-1] != '"'))
                throw Exception{"Failed to parse value for Button  property."};

            m_messageBox->m_buttonSectionName = value.substr(1, value.length()-2);
        }
        else if (property == "textcolor")
            setTextColor(extractColorFromString(property, value));
        else
            throw Exception{"Unrecognized property '" + property + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBoxRenderer::setTextFont(std::shared_ptr<sf::Font> font)
    {
        m_messageBox->m_font = font;
        m_messageBox->m_label->setTextFont(font);

        for (unsigned int i = 0; i < m_messageBox->m_buttons.size(); ++i)
            m_messageBox->m_buttons[i]->getRenderer()->setTextFont(font);

        m_messageBox->rearrange();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void MessageBoxRenderer::setTextColor(const sf::Color& color)
    {
        m_messageBox->m_label->setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> MessageBoxRenderer::clone(Widget* widget)
    {
        auto renderer = std::shared_ptr<MessageBoxRenderer>(new MessageBoxRenderer{*this});
        renderer->m_messageBox = static_cast<MessageBox*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
