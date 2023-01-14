/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2023 Bruno Van de Velde (vdv_b@tgui.eu)
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
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char Picture::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::Picture(const char* typeName, bool initRenderer) :
        ClickableWidget{typeName, false}
    {
        if (initRenderer)
        {
            m_renderer = aurora::makeCopied<PictureRenderer>();
            setRenderer(Theme::getDefault()->getRendererNoThrow(m_type));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::Ptr Picture::create()
    {
        return std::make_shared<Picture>();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::Ptr Picture::create(const Texture& texture, bool transparentTexture)
    {
        auto picture = std::make_shared<Picture>();
        picture->getRenderer()->setTexture(texture);
        picture->getRenderer()->setTransparentTexture(transparentTexture);
        return picture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Picture::Ptr Picture::copy(const Picture::ConstPtr& picture)
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

    bool Picture::canGainFocus() const
    {
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Picture::isMouseOnWidget(Vector2f pos) const
    {
        pos -= getPosition();

        // Check if the mouse is on top of the picture
        if (!m_ignoringMouseEvents && (FloatRect{0, 0, getSize().x, getSize().y}.contains(pos)))
        {
            if (!m_transparentTextureCached || !m_sprite.isSet() || !m_sprite.isTransparentPixel(pos))
                return true;
        }

        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::leftMouseReleased(Vector2f pos)
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

    Signal& Picture::getSignal(String signalName)
    {
        if (signalName == onDoubleClick.getName())
            return onDoubleClick;
        else
            return ClickableWidget::getSignal(std::move(signalName));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::rendererChanged(const String& property)
    {
        if (property == U"Texture")
        {
            const auto& texture = getSharedRenderer()->getTexture();

            if (!m_sprite.isSet() && m_size.x.isConstant() && m_size.y.isConstant() && (getSize() == Vector2f{0,0}))
                setSize(Vector2f{texture.getImageSize()});

            m_sprite.setTexture(texture);
        }
        else if ((property == U"Opacity") || (property == U"OpacityDisabled"))
        {
            Widget::rendererChanged(property);
            m_sprite.setOpacity(m_opacityCached);
        }
        else
            Widget::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> Picture::save(SavingRenderersMap& renderers) const
    {
        auto node = Widget::save(renderers);

        if (m_ignoringMouseEvents)
            node->propertyValuePairs[U"IgnoreMouseEvents"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_ignoringMouseEvents));

        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Widget::load(node, renderers);

        if (node->propertyValuePairs[U"IgnoreMouseEvents"])
            ignoreMouseEvents(Deserializer::deserialize(ObjectConverter::Type::Bool, node->propertyValuePairs[U"IgnoreMouseEvents"]->value).getBool());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Picture::updateTime(Duration elapsedTime)
    {
        const bool screenRefreshRequired = Widget::updateTime(elapsedTime);

        if (m_animationTimeElapsed >= getDoubleClickTime())
        {
            m_animationTimeElapsed = {};
            m_possibleDoubleClick = false;
        }

        return screenRefreshRequired;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Picture::draw(BackendRenderTarget& target, RenderStates states) const
    {
        target.drawSprite(states, m_sprite);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Picture::clone() const
    {
        return std::make_shared<Picture>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
