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


#include <TGUI/Container.hpp>
#include <TGUI/LoadingBar.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LoadingBar::LoadingBar()
    {
        m_callback.widgetType = Type_LoadingBar;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    LoadingBar::Ptr LoadingBar::create(const std::string& configFileFilename)
    {
        auto loadingBar = std::make_shared<LoadingBar>();

        loadingBar->m_loadedConfigFile = getResourcePath() + configFileFilename;

        // Open the config file
        ConfigFile configFile{loadingBar->m_loadedConfigFile, "LoadingBar"};

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = loadingBar->m_loadedConfigFile.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = loadingBar->m_loadedConfigFile.substr(0, slashPos+1);

        // Handle the read properties
        for (auto it = configFile.getProperties().cbegin(); it != configFile.getProperties().cend(); ++it)
        {
            if (it->first == "backimage")
                configFile.readTexture(it, configFileFolder, loadingBar->m_textureBack);
            else if (it->first == "frontimage")
                configFile.readTexture(it, configFileFolder, loadingBar->m_textureFront);
            else if (it->first == "textcolor")
                loadingBar->setTextColor(extractColor(it->second));
            else if (it->first == "textsize")
                loadingBar->setTextSize(tgui::stoi(it->second));
            else
                throw Exception{"Unrecognized property '" + it->first + "' in section LoadingBar in " + loadingBar->m_loadedConfigFile + "."};
        }

        // Make sure the required textures were loaded
        if ((loadingBar->m_textureBack.getData() == nullptr) || (loadingBar->m_textureFront.getData() == nullptr))
            throw Exception{"Not all needed images were loaded for the loading bar. Is the LoadingBar section in " + loadingBar->m_loadedConfigFile + " complete?"};

        loadingBar->setSize(loadingBar->m_textureBack.getImageSize());

        // Calculate the size of the front image (the size of the part that will be drawn)
        loadingBar->recalculateSize();

        return loadingBar;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setPosition(const Layout& position)
    {
        Widget::setPosition(position);

        m_textureBack.setPosition(getPosition());
        m_textureFront.setPosition(getPosition());

        m_text.setPosition(getPosition().x + (getSize().x - m_text.getSize().x) / 2.0f,
                           getPosition().y + (getSize().y - m_text.getSize().y) / 2.0f);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setSize(const Layout& size)
    {
        Widget::setSize(size);

        m_textureBack.setSize(getSize());
        m_textureFront.setSize(getSize());

        // Recalculate the size of the front image
        recalculateSize();

        // Recalculate the text size
        setText(m_text.getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setMinimum(unsigned int minimum)
    {
        // Set the new minimum
        m_minimum = minimum;

        // The minimum can never be greater than the maximum
        if (m_minimum > m_maximum)
            m_maximum = m_minimum;

        // When the value is below the minimum then adjust it
        if (m_value < m_minimum)
            m_value = m_minimum;

        // Recalculate the size of the front image (the size of the part that will be drawn)
        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setMaximum(unsigned int maximum)
    {
        // Set the new maximum
        m_maximum = maximum;

        // The maximum can never be below the minimum
        if (m_maximum < m_minimum)
            m_minimum = m_maximum;

        // When the value is above the maximum then adjust it
        if (m_value > m_maximum)
            m_value = m_maximum;

        // Recalculate the size of the front image (the size of the part that will be drawn)
        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setValue(unsigned int value)
    {
        // Set the new value
        m_value = value;

        // When the value is below the minimum or above the maximum then adjust it
        if (m_value < m_minimum)
            m_value = m_minimum;
        else if (m_value > m_maximum)
            m_value = m_maximum;

        // Recalculate the size of the front image (the size of the part that will be drawn)
        recalculateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int LoadingBar::incrementValue()
    {
        // When the value is still below the maximum then adjust it
        if (m_value < m_maximum)
        {
            ++m_value;

            // Add the callback (if the user requested it)
            if (m_callbackFunctions[ValueChanged].empty() == false)
            {
                m_callback.trigger = ValueChanged;
                m_callback.value   = static_cast<int>(m_value);
                addCallback();
            }

            // Check if the loading bar is now full
            if (m_value == m_maximum)
            {
                // Add the callback (if the user requested it)
                if (m_callbackFunctions[LoadingBarFull].empty() == false)
                {
                    m_callback.trigger = LoadingBarFull;
                    m_callback.value   = static_cast<int>(m_value);
                    addCallback();
                }
            }

            // Recalculate the size of the front image (the size of the part that will be drawn)
            recalculateSize();
        }

        // return the new value
        return m_value;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setText(const sf::String& text)
    {
        // Set the new text
        m_text.setText(text);

        // Check if the text is auto sized
        if (m_textSize == 0)
        {
            // Calculate a possible text size
            float size = getSize().y * 0.75f;
            m_text.setTextSize(static_cast<unsigned int>(size));

            // Make the text smaller when it is too width
            if (m_text.getSize().x > (getSize().x * 0.8f))
                m_text.setTextSize(static_cast<unsigned int>(size / (m_text.getSize().x / (getSize().x * 0.8f))));
        }
        else // When the text has a fixed size
        {
            // Set the text size
            m_text.setTextSize(m_textSize);
        }

        // Reposition the text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setTextFont(const sf::Font& font)
    {
        m_text.setTextFont(font);
        setText(getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setTextSize(unsigned int size)
    {
        // Change the text size
        m_textSize = size;

        // Call setText to reposition the text
        setText(m_text.getText());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::setTransparency(unsigned char transparency)
    {
        ClickableWidget::setTransparency(transparency);

        m_textureBack.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textureFront.setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::recalculateSize()
    {
        m_textureFront.setTextureRect(sf::FloatRect(0, 0, m_textureFront.getSize().x * ((m_value - m_minimum) / static_cast<float>(m_maximum - m_minimum)), m_textureFront.getSize().y));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        if (!getTextFont() && m_parent->getGlobalFont())
            setTextFont(*m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void LoadingBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        target.draw(m_textureBack, states);
        target.draw(m_textureFront, states);

        if (m_text.getText().isEmpty() == false)
            target.draw(m_text, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
