/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (VDV_B@hotmail.com)
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


#include <TGUI/TGUI.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    AnimatedPicture::AnimatedPicture() :
    loop             (false),
    m_Textures       (),
    m_Sprites        (),
    m_LoadedFilenames(),
    m_FrameDuration  (),
    m_CurrentFrame   (0),
    m_Playing        (false)
    {
        m_ObjectType = animatedPicture;
        m_AnimatedObject = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    AnimatedPicture::AnimatedPicture(const AnimatedPicture& copy) :
    OBJECT           (copy),
    OBJECT_ANIMATION (copy),
    loop             (copy.loop),
    m_LoadedFilenames(copy.m_LoadedFilenames),
    m_FrameDuration  (copy.m_FrameDuration),
    m_CurrentFrame   (copy.m_CurrentFrame),
    m_Playing        (copy.m_Playing)
    {
        // Copy the textures
        for (unsigned int i=0; i< copy.m_Textures.size(); ++i)
        {
            if (TGUI_TextureManager.copyTexture(copy.m_Textures[i], m_Textures[i]))
                m_Sprites[i].setTexture(*m_Textures[i]);
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
        m_Sprites.clear();
        m_LoadedFilenames.clear();
        m_FrameDuration.clear();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    AnimatedPicture& AnimatedPicture::operator= (const AnimatedPicture& right)
    {
        // Make sure it is not the same object
        if (this != &right)
        {
            AnimatedPicture temp(right);
            this->OBJECT::operator=(right);
            this->OBJECT_ANIMATION::operator=(right);

            // If there already were frames then remove them now
            for (unsigned int i=0; i< m_Textures.size(); ++i)
                TGUI_TextureManager.removeTexture(m_Textures[i]);

            std::swap(loop,              temp.loop);
            std::swap(m_Textures,        temp.m_Textures);
            std::swap(m_Sprites,         temp.m_Sprites);
            std::swap(m_LoadedFilenames, temp.m_LoadedFilenames);
            std::swap(m_FrameDuration,   temp.m_FrameDuration);
            std::swap(m_CurrentFrame,    temp.m_CurrentFrame);
            std::swap(m_Playing,         temp.m_Playing);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    AnimatedPicture* AnimatedPicture::clone()
    {
        return new AnimatedPicture(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int AnimatedPicture::addFrame(const std::string filename, const sf::Time frameDurarion)
    {
        // Check if the filename is empty
        if (filename.empty() == true)
            return false;

        sf::Texture* tempTexture;

        // Try to load the texture from the file
        if (TGUI_TextureManager.getTexture(filename, tempTexture))
        {
            // If this is the first frame then set it as the current displayed frame
            if (m_Textures.empty())
                m_CurrentFrame = 1;

            // Add the texture
            m_Textures.push_back(tempTexture);

            // Add the sprite
            m_Sprites.push_back(sf::Sprite(*tempTexture));

            // Store the filename
            m_LoadedFilenames.push_back(filename);

            // Store the frame duration
            m_FrameDuration.push_back(frameDurarion);

            // Return true to indicate that nothing went wrong
            m_Loaded = true;
            return true;
        }
        else // The texture was not loaded
            return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::setSize(float width, float height)
    {
        // Make sure that at least one frame was already loaded
        if (m_Textures.empty())
            return;

        // Set the new scale
        setScale(width / m_Textures[0]->getSize().x, height / m_Textures[0]->getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2u AnimatedPicture::getSize() const
    {
        if (m_Textures.empty() == false)
            return Vector2u(m_Textures[0]->getSize().x, m_Textures[0]->getSize().y);
        else
            return Vector2u(0, 0);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f AnimatedPicture::getScaledSize() const
    {
        if (m_Textures.empty() == false)
            return Vector2f(m_Textures[0]->getSize().x * getScale().x, m_Textures[0]->getSize().y * getScale().y);
        else
            return Vector2f(0, 0);
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

        if (m_Textures.size())
            m_CurrentFrame = 1;
        else
            m_CurrentFrame = 0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::setFrame(unsigned int frame)
    {
        // Make sure the number isn't too high
        if (frame > m_Textures.size())
        {
            // Display the last frame
            m_CurrentFrame = m_Textures.size();
        }
        else // The frame number isn't too high
        {
            // Check if there are no frames
            if (m_Textures.empty() == true)
                m_CurrentFrame = 0;
            else
            {
                // Select the requested frame
                if (frame == 0)
                    m_CurrentFrame = 1;
                else
                    m_CurrentFrame = frame;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int AnimatedPicture::getCurrentFrame()
    {
        return m_CurrentFrame;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::setFrameDuration(unsigned int frame, const sf::Time frameDuration)
    {
        // Only continue when there are frames
        if (m_Textures.empty())
            return;

        // Make sure the number isn't too high
        if (frame > m_Textures.size())
        {
            // Change the duration of the last frame
            m_FrameDuration[m_Textures.size()-1] = frameDuration;
        }
        else // The frame number isn't too high
        {
            // Change the duration of the requested frame
            if (frame == 0)
                m_FrameDuration[0] = frameDuration;
            else
                m_FrameDuration[frame-1] = frameDuration;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Time AnimatedPicture::getCurrentFrameDuration()
    {
        if (m_CurrentFrame > 0)
            return m_FrameDuration[m_CurrentFrame-1];
        else
            return sf::Time();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int AnimatedPicture::getFrames()
    {
        return m_Textures.size();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::string> AnimatedPicture::getLoadedFilenames()
    {
        return m_LoadedFilenames;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::removeFrame(unsigned int frame)
    {
        // If there are no frames then there isn't much to do
        if (m_Textures.empty())
            return;

        // When there is only one frame then keep things simple
        if (m_Textures.size() == 1)
        {
            removeAllFrames();
            return;
        }

        // Make sure the number isn't too high
        if (frame > m_Textures.size())
        {
            // Remove the last frame
            m_Textures.pop_back();
            m_Sprites.pop_back();
            m_LoadedFilenames.pop_back();
            m_FrameDuration.pop_back();

            // You can't display a frame that was removed
            if (m_CurrentFrame == m_Textures.size())
                --m_CurrentFrame;
        }
        else // The frame number isn't too high
        {
            // Check if you tried to remove the first frame (in a wrong way)
            if (frame == 0)
            {
                // Remove the first frame
                m_Textures.erase(m_Textures.begin(), m_Textures.begin()+1);
                m_Sprites.erase(m_Sprites.begin(), m_Sprites.begin()+1);
                m_LoadedFilenames.erase(m_LoadedFilenames.begin(), m_LoadedFilenames.begin()+1);
                m_FrameDuration.erase(m_FrameDuration.begin(), m_FrameDuration.begin()+1);

                // Unless the removed frame was displayed, the displayed frame is now on another position in the vectors
                if (m_CurrentFrame > 1)
                    --m_CurrentFrame;
            }
            else
            {
                // Remove the requested frame
                m_Textures.erase(m_Textures.begin()+frame, m_Textures.begin()+frame+1);
                m_Sprites.erase(m_Sprites.begin()+frame, m_Sprites.begin()+frame+1);
                m_LoadedFilenames.erase(m_LoadedFilenames.begin()+frame, m_LoadedFilenames.begin()+frame+1);
                m_FrameDuration.erase(m_FrameDuration.begin()+frame, m_FrameDuration.begin()+frame+1);

                // If the displayed frame was behind the deleted one, then it should be shifted
                if (m_CurrentFrame > frame)
                    --m_CurrentFrame;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::removeAllFrames()
    {
        // Remove the textures (if we are the only one using it)
        for (unsigned int i=0; i< m_Textures.size(); ++i)
            TGUI_TextureManager.removeTexture(m_Textures[i]);

        // Clear the vectors
        m_Textures.clear();
        m_Sprites.clear();
        m_LoadedFilenames.clear();
        m_FrameDuration.clear();

        // Reset the animation
        m_CurrentFrame = 0;
        m_Playing = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void AnimatedPicture::update()
    {
        // Only continue when you are playing
        if (m_Playing == false)
            return;

        // Check if the next frame should be displayed
        while (m_AnimationTimeElapsed > m_FrameDuration[m_CurrentFrame-1])
        {
            // Make sure the frame duration isn't 0
            if (m_FrameDuration[m_CurrentFrame-1].asMicroseconds() > 0)
            {
                // Decrease the elapsed time
                m_AnimationTimeElapsed -= m_FrameDuration[m_CurrentFrame-1];

                // Make the next frame visible
                if (m_CurrentFrame < m_Textures.size())
                    ++m_CurrentFrame;
                else
                {
                    // If looping is enabled then stat over
                    if (loop == true)
                        m_CurrentFrame = 1;
                    else
                    {
                        // Looping is disabled so stop the animation
                        m_Playing = false;
                        m_AnimationTimeElapsed = sf::Time();
                    }

                    // The animation has finished, send a callback if needed
                    if (callbackID > 0)
                    {
                        Callback callback;
                        callback.callbackID = callbackID;
                        callback.trigger    = Callback::animationFinished;
                        m_Parent->addCallback(callback);
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
        // Don't continue when there are no frames
        if (m_CurrentFrame == 0)
            return;

        // Apply the transformation
        states.transform *= getTransform();

        // Draw the frame
        target.draw(m_Sprites[m_CurrentFrame-1], states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

