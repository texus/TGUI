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


#include <TGUI/Widgets/BitmapButton.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cmath>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
#if TGUI_COMPILED_WITH_CPP_VER < 17
    constexpr const char BitmapButton::StaticWidgetType[];
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BitmapButton::BitmapButton(const char* typeName, bool initRenderer) :
        Button{typeName, initRenderer},
        m_imageComponent{std::make_shared<priv::dev::ImageComponent>(&icon)}
    {
        initComponentsBitmapButton();

        m_imageComponent->setVisible(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BitmapButton::BitmapButton(const BitmapButton& other) :
        Button               (other),
        icon                 (other.icon),
        m_imageComponent     (std::make_shared<priv::dev::ImageComponent>(*other.m_imageComponent, &icon)),
        m_relativeGlyphHeight(other.m_relativeGlyphHeight)
    {
        initComponentsBitmapButton();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BitmapButton::BitmapButton(BitmapButton&& other) noexcept :
        Button               (std::move(other)),
        icon                 (std::move(other.icon)),
        m_imageComponent     (std::make_shared<priv::dev::ImageComponent>(*other.m_imageComponent, &icon)),
        m_relativeGlyphHeight(std::move(other.m_relativeGlyphHeight))
    {
        initComponentsBitmapButton();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BitmapButton& BitmapButton::operator=(const BitmapButton& other)
    {
        if (&other != this)
        {
            Button::operator=(other);
            icon = other.icon;
            m_imageComponent = std::make_shared<priv::dev::ImageComponent>(*other.m_imageComponent, &icon);
            m_relativeGlyphHeight = other.m_relativeGlyphHeight;

            initComponentsBitmapButton();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BitmapButton& BitmapButton::operator=(BitmapButton&& other) noexcept
    {
        if (&other != this)
        {
            icon = std::move(other.icon);
            m_imageComponent = std::make_shared<priv::dev::ImageComponent>(*other.m_imageComponent, &icon);
            m_relativeGlyphHeight = std::move(other.m_relativeGlyphHeight);
            Button::operator=(std::move(other));

            initComponentsBitmapButton();
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BitmapButton::Ptr BitmapButton::create(const String& text)
    {
        auto button = std::make_shared<BitmapButton>();

        if (!text.empty())
            button->setText(text);

        return button;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    BitmapButton::Ptr BitmapButton::copy(const BitmapButton::ConstPtr& button)
    {
        if (button)
            return std::static_pointer_cast<BitmapButton>(button->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BitmapButton::setSize(const Layout2d& size)
    {
        Button::setSize(size);

        recalculateGlyphSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BitmapButton::setImage(const Texture& image)
    {
        priv::dev::setOptionalPropertyValue(icon, image, priv::dev::ComponentState::Normal);

        if (image.getData())
        {
            m_imageComponent->setSize(Vector2f{image.getImageSize()});
            m_imageComponent->setVisible(true);
        }
        else
            m_imageComponent->setVisible(false);

        updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const Texture& BitmapButton::getImage() const
    {
        return icon.getValue();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BitmapButton::setImageScaling(float relativeHeight)
    {
        m_relativeGlyphHeight = relativeHeight;
        updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float BitmapButton::getImageScaling() const
    {
        return m_relativeGlyphHeight;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BitmapButton::rendererChanged(const String& property)
    {
        if ((property == U"Opacity") || (property == U"OpacityDisabled"))
        {
            Button::rendererChanged(property);
            m_imageComponent->setOpacity(m_opacityCached);
        }
        else
            Button::rendererChanged(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::unique_ptr<DataIO::Node> BitmapButton::save(SavingRenderersMap& renderers) const
    {
        auto node = Button::save(renderers);
        node->propertyValuePairs[U"Image"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(icon.getValue()));
        node->propertyValuePairs[U"ImageScaling"] = std::make_unique<DataIO::ValueNode>(Serializer::serialize(m_relativeGlyphHeight));
        return node;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BitmapButton::load(const std::unique_ptr<DataIO::Node>& node, const LoadingRenderersMap& renderers)
    {
        Button::load(node, renderers);

        if (node->propertyValuePairs[U"Image"])
            setImage(Deserializer::deserialize(ObjectConverter::Type::Texture, node->propertyValuePairs[U"Image"]->value).getTexture());
        if (node->propertyValuePairs[U"ImageScaling"])
            setImageScaling(Deserializer::deserialize(ObjectConverter::Type::Number, node->propertyValuePairs[U"ImageScaling"]->value).getNumber());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BitmapButton::updateSize()
    {
        if (!m_imageComponent->isVisible())
        {
            Button::updateSize();
            return;
        }

        if (m_autoSize)
        {
            m_autoLayout = AutoLayout::Manual;

            const Outline& borders = m_backgroundComponent->getBorders();

            // NOLINTNEXTLINE(bugprone-parent-virtual-call)
            Widget::setSize({getSize().x, std::round(m_textComponent->getLineHeight() * 1.25f) + borders.getTop() + borders.getBottom()});

            recalculateGlyphSize();

            if (m_string.empty())
            {
                const Vector2f innerSize = m_backgroundComponent->getClientSize();

                // NOLINTNEXTLINE(bugprone-parent-virtual-call)
                Widget::setSize({m_imageComponent->getSize().x + (innerSize.y - m_imageComponent->getSize().y) + borders.getLeft() + borders.getRight(),
                                 getSize().y});
            }
            else
            {
                const float distanceBetweenTextAndImage = std::round(m_textComponent->getLineHeight() / 5.f);
                const float spaceAroundImageAndText = m_textComponent->getLineHeight();

                // NOLINTNEXTLINE(bugprone-parent-virtual-call)
                Widget::setSize({m_imageComponent->getSize().x + distanceBetweenTextAndImage + m_textComponent->getSize().x
                                + spaceAroundImageAndText + borders.getLeft() + borders.getRight(), getSize().y});
            }

            m_backgroundComponent->setSize(getSize());
        }
        else
        {
            m_backgroundComponent->setSize(getSize());
            recalculateGlyphSize();
        }

        updateTextPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BitmapButton::recalculateGlyphSize()
    {
        if (!m_imageComponent->isVisible())
            return;

        if (m_relativeGlyphHeight == 0)
            m_imageComponent->setSize(Vector2f{icon.getValue().getImageSize()});
        else
        {
            const Vector2f innerSize = m_backgroundComponent->getClientSize();
            m_imageComponent->setSize({(m_relativeGlyphHeight * innerSize.y) * icon.getValue().getImageSize().x / icon.getValue().getImageSize().y,
                                       (m_relativeGlyphHeight * innerSize.y)});
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BitmapButton::initComponentsBitmapButton()
    {
        m_stylePropertiesNames.emplace(m_type + U".Icon", &icon);
        m_namedComponents.emplace("Icon", m_imageComponent);

        m_backgroundComponent->addComponent(m_imageComponent);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BitmapButton::updateTextPosition()
    {
        if (!m_imageComponent->isVisible())
            return Button::updateTextPosition();

        const Vector2f innerSize = m_backgroundComponent->getClientSize();
        const float distanceBetweenTextAndImage = m_textComponent->getLineHeight() / 5.f;
        Vector2f contentSize = m_imageComponent->getSize();
        if (!m_string.empty())
        {
            contentSize.x += distanceBetweenTextAndImage + m_textComponent->getSize().x;
            contentSize.y = std::max(contentSize.y, m_textComponent->getSize().y);
        }

        m_textPosition.x.updateParentSize(innerSize.x);
        m_textPosition.y.updateParentSize(innerSize.y);

        m_imageComponent->setPosition({m_textPosition.x.getValue() - m_textOrigin.x * contentSize.x,
                                       m_textPosition.y.getValue() - m_textOrigin.y * contentSize.y + (contentSize.y - m_imageComponent->getSize().y) / 2.f});
        m_textComponent->setPosition({m_imageComponent->getPosition().x + m_imageComponent->getSize().x + distanceBetweenTextAndImage,
                                      m_textPosition.y.getValue() - m_textOrigin.y * contentSize.y + (contentSize.y - m_textComponent->getSize().y) / 2.f});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr BitmapButton::clone() const
    {
        return std::make_shared<BitmapButton>(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
