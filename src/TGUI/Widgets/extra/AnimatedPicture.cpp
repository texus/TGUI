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


#include <TGUI/Widgets/extra/AnimatedPicture.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
namespace ext
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    AnimatedPicture::AnimatedPicture()
    {
        m_callback.widgetType = "AnimatedPicture";
        m_animatedWidget = true;

        addSignal("AnimationFinished");
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    AnimatedPicture::Ptr AnimatedPicture::copy(AnimatedPicture::ConstPtr picture)
    {
        if (picture)
            return std::make_shared<AnimatedPicture>(*picture);
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::setPosition(const Layout2d& position)
    {
        Widget::setPosition(position);

        for (auto& texture : m_textures)
            texture.setPosition(getPosition());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        for (auto& texture : m_textures)
            texture.setSize(getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::addFrame(const std::string& filename, sf::Time frameDuration)
    {
        // Try to load the texture from the file
        Texture tempTexture;
        tempTexture.load(getResourcePath() + filename);

        // If this is the first frame then set it as the current displayed frame
        if (m_textures.empty())
        {
            m_currentFrame = 0;

            // Remember the size of this image
            setSize(tempTexture.getSize());
        }

        // Add the texture and store its duration
        tempTexture.setColor({255, 255, 255, m_opacity});
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
        m_animationTimeElapsed = {};
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

    bool AnimatedPicture::setFrame(std::size_t frame)
    {
        // Check if there are no frames
        if (m_textures.empty())
        {
            m_currentFrame = -1;
            return false;
        }

        // Make sure the number isn't too high
        if (frame >= m_textures.size())
        {
            // Display the last frame
            m_currentFrame = static_cast<int>(m_textures.size()-1);
            return false;
        }

        // The frame number isn't too high
        m_currentFrame = static_cast<int>(frame);
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool AnimatedPicture::removeFrame(std::size_t frame)
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
            setSize({0, 0});

        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::removeAllFrames()
    {
        setSize({0, 0});

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
                        m_animationTimeElapsed = {};
                    }

                    sendSignal("AnimationFinished");
                }
            }
            else // The frame has to remain visible
                m_animationTimeElapsed = {};
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        if (!m_textures.empty())
            target.draw(m_textures[m_currentFrame], states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} // ext
} // tgui

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
