/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2015 Bruno Van de Velde (vdv_b@tgui.eu)
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

    AnimatedPicture::AnimatedPicture() :
    m_Textures       (),
    m_FrameDuration  (),
    m_CurrentFrame   (-1),
    m_Playing        (false),
    m_Looping        (false)
    {
        m_Callback.widgetType = Type_AnimatedPicture;
        m_AnimatedWidget = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    AnimatedPicture::AnimatedPicture(const AnimatedPicture& copy) :
    ClickableWidget(copy),
    m_FrameDuration(copy.m_FrameDuration),
    m_CurrentFrame (copy.m_CurrentFrame),
    m_Playing      (copy.m_Playing),
    m_Looping      (copy.m_Looping)
    {
        for (unsigned int i = 0; i < copy.m_Textures.size(); ++i)
        {
            m_Textures.push_back(Texture());
            TGUI_TextureManager.copyTexture(copy.m_Textures[i], m_Textures.back());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    AnimatedPicture::~AnimatedPicture()
    {
        // Remove the textures (if we are the only one using it)
        for (unsigned int i=0; i< m_Textures.size(); ++i)
            TGUI_TextureManager.removeTexture(m_Textures[i]);

        // Clear the vectors
        m_Textures.clear();
        m_FrameDuration.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    AnimatedPicture& AnimatedPicture::operator= (const AnimatedPicture& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            AnimatedPicture temp(right);
            this->ClickableWidget::operator=(right);

            // If there already were frames then remove them now
            for (unsigned int i=0; i< m_Textures.size(); ++i)
                TGUI_TextureManager.removeTexture(m_Textures[i]);

            std::swap(m_Textures,        temp.m_Textures);
            std::swap(m_FrameDuration,   temp.m_FrameDuration);
            std::swap(m_CurrentFrame,    temp.m_CurrentFrame);
            std::swap(m_Playing,         temp.m_Playing);
            std::swap(m_Looping,         temp.m_Looping);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    AnimatedPicture* AnimatedPicture::clone()
    {
        return new AnimatedPicture(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool AnimatedPicture::addFrame(const std::string& filename, sf::Time frameDuration)
    {
        // Check if the filename is empty
        if (filename.empty())
            return false;

        Texture tempTexture;

        // Try to load the texture from the file
        if (TGUI_TextureManager.getTexture(getResourcePath() + filename, tempTexture))
        {
            // If this is the first frame then set it as the current displayed frame
            if (m_Textures.empty())
            {
                m_CurrentFrame = 0;

                // Remember the size of this image
                m_Size = sf::Vector2f(tempTexture.getSize());
            }

            // Add the texture
            tempTexture.sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
            m_Textures.push_back(tempTexture);

            // Store the frame duration
            m_FrameDuration.push_back(frameDuration);

            // Return true to indicate that nothing went wrong
            return m_Loaded = true;
        }
        else // The texture was not loaded
            return m_Loaded = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::setSize(float width, float height)
    {
        m_Size.x = width;
        m_Size.y = height;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f AnimatedPicture::getSize() const
    {
        if (m_Textures.empty() == false)
            return sf::Vector2f(m_Size.x, m_Size.y);
        else
            return sf::Vector2f(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::play()
    {
        // You can't start playing when no frames were loaded
        if (m_Textures.empty())
            return;

        // Start playing
        m_Playing = true;

        // Reset the elapsed time
        m_AnimationTimeElapsed = sf::Time();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::pause()
    {
        m_Playing = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::stop()
    {
        m_Playing = false;

        if (m_Textures.empty())
            m_CurrentFrame = -1;
        else
            m_CurrentFrame = 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool AnimatedPicture::setFrame(unsigned int frame)
    {
        // Check if there are no frames
        if (m_Textures.empty() == true)
        {
            m_CurrentFrame = -1;
            return false;
        }

        // Make sure the number isn't too high
        if (frame >= m_Textures.size())
        {
            // Display the last frame
            m_CurrentFrame = m_Textures.size()-1;
            return false;
        }

        // The frame number isn't too high
        m_CurrentFrame = frame;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int AnimatedPicture::getCurrentFrame() const
    {
        return m_CurrentFrame;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Time AnimatedPicture::getCurrentFrameDuration() const
    {
        if (!m_FrameDuration.empty())
            return m_FrameDuration[m_CurrentFrame];
        else
        {
            TGUI_OUTPUT("TGUI warning: Can't get duration of current frame: no frames available.");
            return sf::Time();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int AnimatedPicture::getFrames() const
    {
        return m_Textures.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool AnimatedPicture::removeFrame(unsigned int frame)
    {
        // Make sure the number isn't too high
        if (frame >= m_Textures.size())
            return false;

        // Remove the requested frame
        TGUI_TextureManager.removeTexture(m_Textures[frame]);
        m_Textures.erase(m_Textures.begin() + frame);
        m_FrameDuration.erase(m_FrameDuration.begin() + frame);

        // If the displayed frame was behind the deleted one, then it should be shifted
        if (m_CurrentFrame >= static_cast<int>(frame))
            --m_CurrentFrame;

        // Check if you are removing the last frame
        if (m_Textures.size() == 1)
            m_Loaded = false;

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::removeAllFrames()
    {
        // Remove the textures (if we are the only one using it)
        for (unsigned int i=0; i< m_Textures.size(); ++i)
            TGUI_TextureManager.removeTexture(m_Textures[i]);

        // Clear the vectors
        m_Textures.clear();
        m_FrameDuration.clear();

        // Reset the animation
        stop();
        m_Loaded = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::setLooping(bool loop)
    {
        m_Looping = loop;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool AnimatedPicture::getLooping() const
    {
        return m_Looping;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool AnimatedPicture::isPlaying() const
    {
        return m_Playing;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::setTransparency(unsigned char transparency)
    {
        ClickableWidget::setTransparency(transparency);

        for (unsigned int i = 0; i < m_Textures.size(); ++i)
            m_Textures[i].sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool AnimatedPicture::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "playing")
        {
            if ((value == "true") || (value == "True"))
                m_Playing = true;
            else if ((value == "false") || (value == "False"))
                m_Playing = false;
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'Playing' property.");
        }
        else if (property == "looping")
        {
            if ((value == "true") || (value == "True"))
                m_Looping = true;
            else if ((value == "false") || (value == "False"))
                m_Looping = false;
            else
                TGUI_OUTPUT("TGUI error: Failed to parse 'Looping' property.");
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
            return ClickableWidget::setProperty(property, value);

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool AnimatedPicture::getProperty(std::string property, std::string& value) const
    {
        property = toLower(property);

        if (property == "playing")
            value = m_Playing ? "true" : "false";
        else if (property == "looping")
            value = m_Looping ? "true" : "false";
        else if (property == "callback")
        {
            std::string tempValue;
            ClickableWidget::getProperty(property, tempValue);

            std::vector<sf::String> callbacks;

            if ((m_CallbackFunctions.find(AnimationFinished) != m_CallbackFunctions.end()) && (m_CallbackFunctions.at(AnimationFinished).size() == 1) && (m_CallbackFunctions.at(AnimationFinished).front() == nullptr))
                callbacks.push_back("AnimationFinished");

            encodeList(callbacks, value);

            if (value.empty())
                value = tempValue;
            else if (!tempValue.empty())
                value += "," + tempValue;
        }
        else // The property didn't match
            return ClickableWidget::getProperty(property, value);

        // You pass here when one of the properties matched
        return true;
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
        if (m_Playing == false)
            return;

        // Check if the next frame should be displayed
        while (m_AnimationTimeElapsed > m_FrameDuration[m_CurrentFrame])
        {
            // Make sure the frame duration isn't 0
            if (m_FrameDuration[m_CurrentFrame].asMicroseconds() > 0)
            {
                // Decrease the elapsed time
                m_AnimationTimeElapsed -= m_FrameDuration[m_CurrentFrame];

                // Make the next frame visible
                if (static_cast<unsigned int>(m_CurrentFrame + 1) < m_Textures.size())
                    ++m_CurrentFrame;
                else
                {
                    // If looping is enabled then stat over
                    if (m_Looping == true)
                        m_CurrentFrame = 0;
                    else
                    {
                        // Looping is disabled so stop the animation
                        m_Playing = false;
                        m_AnimationTimeElapsed = sf::Time();
                    }

                    // The animation has finished, send a callback if needed
                    if (m_CallbackFunctions[AnimationFinished].empty() == false)
                    {
                        m_Callback.trigger = AnimationFinished;
                        addCallback();
                    }

                }
            }
            else // The frame has to remain visible
                m_AnimationTimeElapsed = sf::Time();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (m_Loaded)
        {
            states.transform *= getTransform();
            states.transform.scale(m_Size.x / m_Textures[m_CurrentFrame].getSize().x, m_Size.y / m_Textures[m_CurrentFrame].getSize().y);
            target.draw(m_Textures[m_CurrentFrame], states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
