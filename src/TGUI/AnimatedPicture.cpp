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


#include <TGUI/AnimatedPicture.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    AnimatedPicture::AnimatedPicture()
    {
        m_callback.widgetType = Type_AnimatedPicture;
        m_animatedWidget = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::addFrame(const std::string& filename, sf::Time frameDuration)
    {
        // Try to load the texture from the file
        Texture tempTexture;
        TGUI_TextureManager.getTexture(tempTexture, getResourcePath() + filename);

        // If this is the first frame then set it as the current displayed frame
        if (m_textures.empty())
        {
            m_currentFrame = 0;

            // Remember the size of this image
            m_size = sf::Vector2f(tempTexture.getSize());
        }

        // Add the texture and store its duration
        tempTexture.setColor(sf::Color(255, 255, 255, m_opacity));
        m_textures.push_back(tempTexture);
        m_frameDuration.push_back(frameDuration);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::play()
    {
        // You can't start playing when no frames were loaded
        if (m_textures.empty())
            return;

        // Start playing
        m_playing = true;

        // Reset the elapsed time
        m_animationTimeElapsed = sf::Time();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::pause()
    {
        m_playing = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::stop()
    {
        m_playing = false;

        if (m_textures.empty())
            m_currentFrame = -1;
        else
            m_currentFrame = 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool AnimatedPicture::setFrame(unsigned int frame)
    {
        // Check if there are no frames
        if (m_textures.empty() == true)
        {
            m_currentFrame = -1;
            return false;
        }

        // Make sure the number isn't too high
        if (frame >= m_textures.size())
        {
            // Display the last frame
            m_currentFrame = m_textures.size()-1;
            return false;
        }

        // The frame number isn't too high
        m_currentFrame = frame;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool AnimatedPicture::removeFrame(unsigned int frame)
    {
        // Make sure the number isn't too high
        if (frame >= m_textures.size())
            return false;

        // Remove the requested frame
        m_textures.erase(m_textures.begin() + frame);
        m_frameDuration.erase(m_frameDuration.begin() + frame);

        // If the displayed frame was behind the deleted one, then it should be shifted
        if (m_currentFrame >= static_cast<int>(frame))
            --m_currentFrame;

        if (m_textures.empty())
            m_size = {0, 0};

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::removeAllFrames()
    {
        m_size = {0, 0};

        // Clear the vectors
        m_textures.clear();
        m_frameDuration.clear();

        // Reset the animation
        stop();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::setTransparency(unsigned char transparency)
    {
        ClickableWidget::setTransparency(transparency);

        for (unsigned int i = 0; i < m_textures.size(); ++i)
            m_textures[i].setColor(sf::Color(255, 255, 255, m_opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "playing")
        {
            if ((value == "true") || (value == "True"))
                m_playing = true;
            else if ((value == "false") || (value == "False"))
                m_playing = false;
            else
                throw Exception("Failed to parse 'Playing' property.");
        }
        else if (property == "looping")
        {
            if ((value == "true") || (value == "True"))
                m_looping = true;
            else if ((value == "false") || (value == "False"))
                m_looping = false;
            else
                throw Exception("Failed to parse 'Looping' property.");
        }
        else if (property == "callback")
        {
            ClickableWidget::setProperty(property, value);

            std::vector<sf::String> callbacks;
            decodeList(value, callbacks);

            for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
            {
                if ((*it == "AnimationFinished") || (*it == "animationfinished"))
                    bindCallback(AnimationFinished);
            }
        }
        else // The property didn't match
            ClickableWidget::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "playing")
            value = m_playing ? "true" : "false";
        else if (property == "looping")
            value = m_looping ? "true" : "false";
        else if (property == "callback")
        {
            std::string tempValue;
            ClickableWidget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_callbackFunctions.find(AnimationFinished) != m_callbackFunctions.end()) && (m_callbackFunctions.at(AnimationFinished).size() == 1) && (m_callbackFunctions.at(AnimationFinished).front() == nullptr))
                callbacks.push_back("AnimationFinished");

            encodeList(callbacks, value);

            if (value.empty())
                value = tempValue;
            else if (!tempValue.empty())
                value += "," + tempValue;
        }
        else // The property didn't match
            ClickableWidget::getProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::list< std::pair<std::string, std::string> > AnimatedPicture::getPropertyList() const
    {
        auto list = ClickableWidget::getPropertyList();
        list.push_back(std::pair<std::string, std::string>("Playing", "bool"));
        list.push_back(std::pair<std::string, std::string>("Looping", "bool"));
        return list;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::update()
    {
        // Only continue when you are playing
        if (m_playing == false)
            return;

        // Check if the next frame should be displayed
        while (m_animationTimeElapsed > m_frameDuration[m_currentFrame])
        {
            // Make sure the frame duration isn't 0
            if (m_frameDuration[m_currentFrame].asMicroseconds() > 0)
            {
                // Decrease the elapsed time
                m_animationTimeElapsed -= m_frameDuration[m_currentFrame];

                // Make the next frame visible
                if (static_cast<unsigned int>(m_currentFrame + 1) < m_textures.size())
                    ++m_currentFrame;
                else
                {
                    // If looping is enabled then stat over
                    if (m_looping == true)
                        m_currentFrame = 0;
                    else
                    {
                        // Looping is disabled so stop the animation
                        m_playing = false;
                        m_animationTimeElapsed = sf::Time();
                    }

                    // The animation has finished, send a callback if needed
                    if (m_callbackFunctions[AnimationFinished].empty() == false)
                    {
                        m_callback.trigger = AnimationFinished;
                        addCallback();
                    }

                }
            }
            else // The frame has to remain visible
                m_animationTimeElapsed = sf::Time();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (!m_textures.empty())
        {
            states.transform *= getTransform();
            states.transform.scale(m_size.x / m_textures[m_currentFrame].getSize().x, m_size.y / m_textures[m_currentFrame].getSize().y);
            target.draw(m_textures[m_currentFrame], states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
