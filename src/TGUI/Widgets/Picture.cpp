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


#include <TGUI/Widgets/Picture.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::Picture()
    {
        m_type = "Picture";

        m_renderer = aurora::makeCopied<PictureRenderer>();
        setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::Picture(const Texture& texture, bool ignoreTransparentParts) :
        Picture{}
    {
        getRenderer()->setTexture(texture);
        if (ignoreTransparentParts)
            getRenderer()->setIgnoreTransparentParts(ignoreTransparentParts);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::Ptr Picture::create(const Texture& texture, bool fullyClickable)
    {
        return std::make_shared<Picture>(texture, fullyClickable);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::Ptr Picture::copy(Picture::ConstPtr picture)
    {
        if (picture)
            return std::static_pointer_cast<Picture>(picture->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    PictureRenderer* Picture::getSharedRenderer()
    {
        return aurora::downcast<PictureRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const PictureRenderer* Picture::getSharedRenderer() const
    {
        return aurora::downcast<const PictureRenderer*>(Widget::getSharedRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    PictureRenderer* Picture::getRenderer()
    {
        return aurora::downcast<PictureRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const PictureRenderer* Picture::getRenderer() const
    {
        return aurora::downcast<const PictureRenderer*>(Widget::getRenderer());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        m_sprite.setSize(getSize());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::ignoreMouseEvents(bool ignore)
    {
        m_ignoringMouseEvents = ignore;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Picture::isIgnoringMouseEvents() const
    {
        return m_ignoringMouseEvents;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Picture::mouseOnWidget(sf::Vector2f pos) const
    {
        pos -= getPosition();

        // Check if the mouse is on top of the picture
        if (!m_ignoringMouseEvents && (sf::FloatRect{0, 0, getSize().x, getSize().y}.contains(pos)))
        {
            // We sometimes want clicks to go through transparent parts of the picture
            if (!m_ignoreTransparentParts && m_sprite.isTransparentPixel(pos))
                return false;
            else
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::leftMouseReleased(sf::Vector2f pos)
    {
        const bool mouseDown = m_mouseDown;

        ClickableWidget::leftMouseReleased(pos);

        if (mouseDown)
        {
            // Check if you double-clicked
            if (m_possibleDoubleClick)
            {
                m_possibleDoubleClick = false;
                onDoubleClick.emit(this, pos - getPosition());
            }
            else // This is the first click
            {
                m_animationTimeElapsed = {};
                m_possibleDoubleClick = true;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Signal& Picture::getSignal(std::string signalName)
    {
        if (signalName == toLower(onDoubleClick.getName()))
            return onDoubleClick;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::rendererChanged(const std::string& property)
    {
        if (property == "texture")
        {
            const auto& texture = getSharedRenderer()->getTexture();

            if (!m_sprite.isSet() && (getSize() == sf::Vector2f{0,0}))
                setSize(texture.getImageSize());

            m_sprite.setTexture(texture);
        }
        else if (property == "ignoretransparentparts")
        {
            m_ignoreTransparentParts = getSharedRenderer()->getIgnoreTransparentParts();
        }
        else if (property == "opacity")
        {
            Widget::rendererChanged(property);
            m_sprite.setOpacity(m_opacityCached);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::update(sf::Time elapsedTime)
    {
        Widget::update(elapsedTime);

        // When double-clicking, the second click has to come within 500 milliseconds
        if (m_animationTimeElapsed >= sf::milliseconds(500))
        {
            m_animationTimeElapsed = {};
            m_possibleDoubleClick = false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform.translate(getPosition());
        m_sprite.draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
