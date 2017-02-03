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


#include <TGUI/Container.hpp>
#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Widgets/SpinButton.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton::SpinButton()
    {
        m_callback.widgetType = "SpinButton";

        addSignal<int>("ValueChanged");

        m_renderer = std::make_shared<SpinButtonRenderer>(this);
        reload();

        setSize(20, 42);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton::Ptr SpinButton::create(int minimum, int maximum)
    {
        auto spinButton = std::make_shared<SpinButton>();

        spinButton->setMinimum(minimum);
        spinButton->setMaximum(maximum);

        return spinButton;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SpinButton::Ptr SpinButton::copy(SpinButton::ConstPtr spinButton)
    {
        if (spinButton)
            return std::static_pointer_cast<SpinButton>(spinButton->clone());
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setPosition(const Layout2d& position)
    {
        Widget::setPosition(position);

        if (m_verticalScroll)
        {
            getRenderer()->m_textureArrowUpNormal.setPosition(getPosition());
            getRenderer()->m_textureArrowUpHover.setPosition(getPosition());
            getRenderer()->m_textureArrowDownNormal.setPosition(getPosition().x, getPosition().y + getRenderer()->m_textureArrowUpNormal.getSize().y + getRenderer()->m_spaceBetweenArrows);
            getRenderer()->m_textureArrowDownHover.setPosition(getPosition().x, getPosition().y + getRenderer()->m_textureArrowUpHover.getSize().y + getRenderer()->m_spaceBetweenArrows);
        }
        else // Horizontal orientation
        {
            getRenderer()->m_textureArrowUpNormal.setPosition(getPosition());
            getRenderer()->m_textureArrowUpHover.setPosition(getPosition());
            getRenderer()->m_textureArrowDownNormal.setPosition(getPosition().x + getRenderer()->m_textureArrowUpNormal.getSize().y + getRenderer()->m_spaceBetweenArrows, getPosition().y);
            getRenderer()->m_textureArrowDownHover.setPosition(getPosition().x + getRenderer()->m_textureArrowUpHover.getSize().y + getRenderer()->m_spaceBetweenArrows, getPosition().y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setSize(const Layout2d& size)
    {
        Widget::setSize(size);

        if (m_verticalScroll)
            getRenderer()->m_textureArrowUpNormal.setSize({getSize().x, (getSize().y - getRenderer()->m_spaceBetweenArrows) / 2.0f});
        else
            getRenderer()->m_textureArrowUpNormal.setSize({getSize().y, (getSize().x - getRenderer()->m_spaceBetweenArrows) / 2.0f});

        getRenderer()->m_textureArrowUpHover.setSize(getRenderer()->m_textureArrowUpNormal.getSize());
        getRenderer()->m_textureArrowDownNormal.setSize(getRenderer()->m_textureArrowUpNormal.getSize());
        getRenderer()->m_textureArrowDownHover.setSize(getRenderer()->m_textureArrowUpNormal.getSize());

        // Recalculate the position of the images
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f SpinButton::getFullSize() const
    {
        return {getSize().x + getRenderer()->getBorders().left + getRenderer()->getBorders().right,
                getSize().y + getRenderer()->getBorders().top + getRenderer()->getBorders().bottom};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setMinimum(int minimum)
    {
        // Set the new minimum
        m_minimum = minimum;

        // The minimum can never be greater than the maximum
        if (m_minimum > m_maximum)
            setMaximum(m_minimum);

        // When the value is below the minimum then adjust it
        if (m_value < m_minimum)
            setValue(m_minimum);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setMaximum(int maximum)
    {
        m_maximum = maximum;

        // The maximum can never be below the minimum
        if (m_maximum < m_minimum)
            setMinimum(m_maximum);

        // When the value is above the maximum then adjust it
        if (m_value > m_maximum)
            setValue(m_maximum);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setValue(int value)
    {
        // When the value is below the minimum or above the maximum then adjust it
        if (value < m_minimum)
            value = m_minimum;
        else if (value > m_maximum)
            value = m_maximum;

        if (m_value != value)
        {
            m_value = value;

            m_callback.value = m_value;
            sendSignal("ValueChanged", value);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setVerticalScroll(bool verticalScroll)
    {
        m_verticalScroll = verticalScroll;

        if (verticalScroll)
        {
            getRenderer()->m_textureArrowUpNormal.setRotation(0);
            getRenderer()->m_textureArrowUpHover.setRotation(0);
            getRenderer()->m_textureArrowDownNormal.setRotation(0);
            getRenderer()->m_textureArrowDownHover.setRotation(0);
        }
        else
        {
            getRenderer()->m_textureArrowUpNormal.setRotation(-90);
            getRenderer()->m_textureArrowUpHover.setRotation(-90);
            getRenderer()->m_textureArrowDownNormal.setRotation(-90);
            getRenderer()->m_textureArrowDownHover.setRotation(-90);
        }

        updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::setOpacity(float opacity)
    {
        ClickableWidget::setOpacity(opacity);

        getRenderer()->m_textureArrowUpNormal.setColor({getRenderer()->m_textureArrowUpNormal.getColor().r, getRenderer()->m_textureArrowUpNormal.getColor().g, getRenderer()->m_textureArrowUpNormal.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureArrowUpHover.setColor({getRenderer()->m_textureArrowUpHover.getColor().r, getRenderer()->m_textureArrowUpHover.getColor().g, getRenderer()->m_textureArrowUpHover.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureArrowDownNormal.setColor({getRenderer()->m_textureArrowDownNormal.getColor().r, getRenderer()->m_textureArrowDownNormal.getColor().g, getRenderer()->m_textureArrowDownNormal.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
        getRenderer()->m_textureArrowDownHover.setColor({getRenderer()->m_textureArrowDownHover.getColor().r, getRenderer()->m_textureArrowDownHover.getColor().g, getRenderer()->m_textureArrowDownHover.getColor().b, static_cast<sf::Uint8>(m_opacity * 255)});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f SpinButton::getWidgetOffset() const
    {
        return {getRenderer()->getBorders().left, getRenderer()->getBorders().top};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::leftMousePressed(float x, float y)
    {
        ClickableWidget::leftMousePressed(x, y);

        // Check if the mouse is on top of the upper/right arrow
        if (m_verticalScroll)
        {
            if (sf::FloatRect{getPosition().x - getRenderer()->getBorders().left, getPosition().y - getRenderer()->getBorders().top, getFullSize().x, getFullSize().y / 2.0f}.contains(x, y))
                m_mouseDownOnTopArrow = true;
            else
                m_mouseDownOnTopArrow = false;
        }
        else
        {
            if (sf::FloatRect{getPosition().x - getRenderer()->getBorders().left, getPosition().y - getRenderer()->getBorders().top, getFullSize().x / 2.0f, getFullSize().y}.contains(x, y))
                m_mouseDownOnTopArrow = false;
            else
                m_mouseDownOnTopArrow = true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::leftMouseReleased(float x, float y)
    {
        ClickableWidget::leftMouseReleased(x, y);

        // Check if the mouse went down on the spin button
        if (m_mouseDown)
        {
            // Check if the arrow went down on the top/right arrow
            if (m_mouseDownOnTopArrow)
            {
                // Check if the mouse went up on the same arrow
                if ((m_verticalScroll && (sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y / 2.f}.contains(x, y)))
                 || (!m_verticalScroll && (!sf::FloatRect{getPosition().x, getPosition().y, getSize().x / 2.f, getSize().y}.contains(x, y))))
                {
                    // Increment the value
                    if (m_value < m_maximum)
                        setValue(m_value + 1);
                    else
                        return;
                }
                else
                    return;
            }
            else // The mouse went down on the bottom/left arrow
            {
                // Check if the mouse went up on the same arrow
                if ((m_verticalScroll && (!sf::FloatRect{getPosition().x, getPosition().y, getSize().x, getSize().y / 2.f}.contains(x, y)))
                 || (!m_verticalScroll && (sf::FloatRect{getPosition().x, getPosition().y, getSize().x / 2.f, getSize().y}.contains(x, y))))
                {
                    // Decrement the value
                    if (m_value > m_minimum)
                        setValue(m_value - 1);
                    else
                        return;
                }
                else
                    return;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::mouseMoved(float x, float y)
    {
        // Check if the mouse is on top of the upper/right arrow
        if (m_verticalScroll)
        {
            if (sf::FloatRect{getPosition().x - getRenderer()->getBorders().left, getPosition().y - getRenderer()->getBorders().top, getFullSize().x, getFullSize().y / 2.0f}.contains(x, y))
                m_mouseHoverOnTopArrow = true;
            else
                m_mouseHoverOnTopArrow = false;
        }
        else
        {
            if (sf::FloatRect{getPosition().x - getRenderer()->getBorders().left, getPosition().y - getRenderer()->getBorders().top, getFullSize().x / 2.0f, getFullSize().y}.contains(x, y))
                m_mouseHoverOnTopArrow = true;
            else
                m_mouseHoverOnTopArrow = false;
        }

        if (!m_mouseHover)
            mouseEnteredWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::widgetFocused()
    {
        // A spin button can't be focused
        unfocus();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::reload(const std::string& primary, const std::string& secondary, bool force)
    {
        getRenderer()->setBorders({2, 2, 2, 2});
        getRenderer()->setBackgroundColorNormal({245, 245, 245});
        getRenderer()->setBackgroundColorHover({255, 255, 255});
        getRenderer()->setArrowColorNormal({60, 60, 60});
        getRenderer()->setArrowColorHover({0, 0, 0});
        getRenderer()->setBorderColor({0, 0, 0});
        getRenderer()->setSpaceBetweenArrows(2);
        getRenderer()->setArrowUpTexture({});
        getRenderer()->setArrowDownTexture({});
        getRenderer()->setArrowUpHoverTexture({});
        getRenderer()->setArrowDownHoverTexture({});

        if (m_theme && primary != "")
        {
            getRenderer()->setSpaceBetweenArrows(0);
            getRenderer()->setBorders({0, 0, 0, 0});
            Widget::reload(primary, secondary, force);

            if (force)
            {
                // Use the size of the images when images were loaded
                if (getRenderer()->m_textureArrowUpNormal.isLoaded() && getRenderer()->m_textureArrowDownNormal.isLoaded())
                {
                    setSize({getRenderer()->m_textureArrowUpNormal.getSize().x,
                             getRenderer()->m_textureArrowUpNormal.getSize().y + getRenderer()->m_textureArrowDownNormal.getSize().y});
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        getRenderer()->draw(target, states);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButtonRenderer::setProperty(std::string property, const std::string& value)
    {
        property = toLower(property);

        if (property == "borders")
            setBorders(Deserializer::deserialize(ObjectConverter::Type::Borders, value).getBorders());
        else if (property == "backgroundcolor")
            setBackgroundColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "backgroundcolornormal")
            setBackgroundColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "backgroundcolorhover")
            setBackgroundColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "arrowcolor")
            setArrowColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "arrowcolornormal")
            setArrowColorNormal(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "arrowcolorhover")
            setArrowColorHover(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "bordercolor")
            setBorderColor(Deserializer::deserialize(ObjectConverter::Type::Color, value).getColor());
        else if (property == "arrowupimage")
            setArrowUpTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "arrowdownimage")
            setArrowDownTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "arrowuphoverimage")
            setArrowUpHoverTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "arrowdownhoverimage")
            setArrowDownHoverTexture(Deserializer::deserialize(ObjectConverter::Type::Texture, value).getTexture());
        else if (property == "spacebetweenarrows")
            setSpaceBetweenArrows(tgui::stof(value));
        else
            WidgetRenderer::setProperty(property, value);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButtonRenderer::setProperty(std::string property, ObjectConverter&& value)
    {
        property = toLower(property);

        if (value.getType() == ObjectConverter::Type::Borders)
        {
            if (property == "borders")
                setBorders(value.getBorders());
            else
                return WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Color)
        {
            if (property == "backgroundcolor")
                setBackgroundColor(value.getColor());
            else if (property == "backgroundcolornormal")
                setBackgroundColorNormal(value.getColor());
            else if (property == "backgroundcolorhover")
                setBackgroundColorHover(value.getColor());
            else if (property == "arrowcolor")
                setArrowColor(value.getColor());
            else if (property == "arrowcolornormal")
                setArrowColorNormal(value.getColor());
            else if (property == "arrowcolorhover")
                setArrowColorHover(value.getColor());
            else if (property == "bordercolor")
                setBorderColor(value.getColor());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Texture)
        {
            if (property == "arrowupimage")
                setArrowUpTexture(value.getTexture());
            else if (property == "arrowdownimage")
                setArrowDownTexture(value.getTexture());
            else if (property == "arrowuphoverimage")
                setArrowUpHoverTexture(value.getTexture());
            else if (property == "arrowdownhoverimage")
                setArrowDownHoverTexture(value.getTexture());
            else
                WidgetRenderer::setProperty(property, std::move(value));
        }
        else if (value.getType() == ObjectConverter::Type::Number)
        {
            if (property == "spacebetweenarrows")
                setSpaceBetweenArrows(value.getNumber());
        }
        else
            WidgetRenderer::setProperty(property, std::move(value));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ObjectConverter SpinButtonRenderer::getProperty(std::string property) const
    {
        property = toLower(property);

        if (property == "borders")
            return m_borders;
        else if (property == "backgroundcolor")
            return m_backgroundColorNormal;
        else if (property == "backgroundcolornormal")
            return m_backgroundColorNormal;
        else if (property == "backgroundcolorhover")
            return m_backgroundColorHover;
        else if (property == "arrowcolor")
            return m_arrowColorNormal;
        else if (property == "arrowcolornormal")
            return m_arrowColorNormal;
        else if (property == "arrowcolorhover")
            return m_arrowColorHover;
        else if (property == "bordercolor")
            return m_borderColor;
        else if (property == "arrowupimage")
            return m_textureArrowUpNormal;
        else if (property == "arrowdownimage")
            return m_textureArrowDownNormal;
        else if (property == "arrowuphoverimage")
            return m_textureArrowUpHover;
        else if (property == "arrowdownhoverimage")
            return m_textureArrowDownHover;
        if (property == "spacebetweenarrows")
            return m_spaceBetweenArrows;
        else
            return WidgetRenderer::getProperty(property);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::map<std::string, ObjectConverter> SpinButtonRenderer::getPropertyValuePairs() const
    {
        auto pairs = WidgetRenderer::getPropertyValuePairs();

        if (m_textureArrowUpNormal.isLoaded() && m_textureArrowDownNormal.isLoaded())
        {
            pairs["ArrowUpImage"] = m_textureArrowUpNormal;
            pairs["ArrowDownImage"] = m_textureArrowDownNormal;
            if (m_textureArrowUpHover.isLoaded())
                pairs["ArrowUpHoverImage"] = m_textureArrowUpHover;
            if (m_textureArrowDownHover.isLoaded())
                pairs["ArrowDownHoverImage"] = m_textureArrowDownHover;
        }
        else
        {
            pairs["BackgroundColorNormal"] = m_backgroundColorNormal;
            pairs["BackgroundColorHover"] = m_backgroundColorHover;
            pairs["ArrowColorNormal"] = m_arrowColorNormal;
            pairs["ArrowColorHover"] = m_arrowColorHover;
        }

        pairs["BorderColor"] = m_borderColor;
        pairs["Borders"] = m_borders;
        pairs["SpaceBetweenArrows"] = m_spaceBetweenArrows;
        return pairs;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButtonRenderer::setBackgroundColor(const Color& color)
    {
        setBackgroundColorNormal(color);
        setBackgroundColorHover(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButtonRenderer::setBackgroundColorNormal(const Color& color)
    {
        m_backgroundColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButtonRenderer::setBackgroundColorHover(const Color& color)
    {
        m_backgroundColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButtonRenderer::setArrowColor(const Color& color)
    {
        setArrowColorNormal(color);
        setArrowColorHover(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButtonRenderer::setArrowColorNormal(const Color& color)
    {
        m_arrowColorNormal = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButtonRenderer::setArrowColorHover(const Color& color)
    {
        m_arrowColorHover = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButtonRenderer::setBorderColor(const Color& color)
    {
        m_borderColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButtonRenderer::setArrowUpTexture(const Texture& texture)
    {
        m_textureArrowUpNormal = texture;
        if (m_textureArrowUpNormal.isLoaded())
        {
            m_textureArrowUpNormal.setColor({m_textureArrowUpNormal.getColor().r, m_textureArrowUpNormal.getColor().g, m_textureArrowUpNormal.getColor().b, static_cast<sf::Uint8>(m_spinButton->getOpacity() * 255)});

            if (m_textureArrowUpNormal.isLoaded() && m_textureArrowDownNormal.isLoaded())
                m_spinButton->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButtonRenderer::setArrowDownTexture(const Texture& texture)
    {
        m_textureArrowDownNormal = texture;
        if (m_textureArrowDownNormal.isLoaded())
        {
            m_textureArrowDownNormal.setColor({m_textureArrowDownNormal.getColor().r, m_textureArrowDownNormal.getColor().g, m_textureArrowDownNormal.getColor().b, static_cast<sf::Uint8>(m_spinButton->getOpacity() * 255)});

            if (m_textureArrowUpNormal.isLoaded() && m_textureArrowDownNormal.isLoaded())
                m_spinButton->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButtonRenderer::setArrowUpHoverTexture(const Texture& texture)
    {
        m_textureArrowUpHover = texture;
        if (m_textureArrowUpHover.isLoaded())
        {
            m_textureArrowUpHover.setColor({m_textureArrowUpHover.getColor().r, m_textureArrowUpHover.getColor().g, m_textureArrowUpHover.getColor().b, static_cast<sf::Uint8>(m_spinButton->getOpacity() * 255)});

            if (m_textureArrowUpNormal.isLoaded() && m_textureArrowDownNormal.isLoaded())
                m_spinButton->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButtonRenderer::setArrowDownHoverTexture(const Texture& texture)
    {
        m_textureArrowDownHover = texture;
        if (m_textureArrowDownHover.isLoaded())
        {
            m_textureArrowDownHover.setColor({m_textureArrowDownHover.getColor().r, m_textureArrowDownHover.getColor().g, m_textureArrowDownHover.getColor().b, static_cast<sf::Uint8>(m_spinButton->getOpacity() * 255)});

            if (m_textureArrowUpNormal.isLoaded() && m_textureArrowDownNormal.isLoaded())
                m_spinButton->updateSize();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButtonRenderer::setSpaceBetweenArrows(float space)
    {
        m_spaceBetweenArrows = space;

        m_spinButton->updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void SpinButtonRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        sf::Vector2f position = m_spinButton->getPosition();
        sf::Vector2f size = m_spinButton->getSize();

        // Draw the arrows
        if (m_textureArrowUpNormal.isLoaded() && m_textureArrowDownNormal.isLoaded())
        {
            if (m_spinButton->m_mouseHover && m_spinButton->m_mouseHoverOnTopArrow && m_textureArrowUpHover.isLoaded())
                target.draw(m_textureArrowUpHover, states);
            else
                target.draw(m_textureArrowUpNormal, states);

            if (m_spinButton->m_mouseHover && !m_spinButton->m_mouseHoverOnTopArrow && m_textureArrowDownHover.isLoaded())
                target.draw(m_textureArrowDownHover, states);
            else
                target.draw(m_textureArrowDownNormal, states);
        }
        else // There are no images
        {
            sf::ConvexShape arrow{3};
            sf::RectangleShape arrowBack;
            arrowBack.setPosition(position);

            if (m_spinButton->m_mouseHover && m_spinButton->m_mouseHoverOnTopArrow)
            {
                arrowBack.setFillColor(calcColorOpacity(m_backgroundColorHover, m_spinButton->getOpacity()));
                arrow.setFillColor(calcColorOpacity(m_arrowColorHover, m_spinButton->getOpacity()));
            }
            else
            {
                arrowBack.setFillColor(calcColorOpacity(m_backgroundColorNormal, m_spinButton->getOpacity()));
                arrow.setFillColor(calcColorOpacity(m_arrowColorNormal, m_spinButton->getOpacity()));
            }

            if (m_spinButton->m_verticalScroll)
            {
                arrowBack.setSize({size.x, (size.y - m_spaceBetweenArrows) / 2.0f});

                arrow.setPoint(0, {arrowBack.getPosition().x + (arrowBack.getSize().x / 5), arrowBack.getPosition().y + (arrowBack.getSize().y * 4/5)});
                arrow.setPoint(1, {arrowBack.getPosition().x + (arrowBack.getSize().x / 2), arrowBack.getPosition().y + (arrowBack.getSize().y / 5)});
                arrow.setPoint(2, {arrowBack.getPosition().x + (arrowBack.getSize().x * 4/5), arrowBack.getPosition().y + (arrowBack.getSize().y * 4/5)});
            }
            else // Spin button lies horizontal
            {
                arrowBack.setSize({(size.x - m_spaceBetweenArrows) / 2.0f, size.y});

                arrow.setPoint(0, {arrowBack.getPosition().x + (arrowBack.getSize().x * 4/5), arrowBack.getPosition().y + (arrowBack.getSize().y / 5)});
                arrow.setPoint(1, {arrowBack.getPosition().x + (arrowBack.getSize().x / 5), arrowBack.getPosition().y + (arrowBack.getSize().y / 2)});
                arrow.setPoint(2, {arrowBack.getPosition().x + (arrowBack.getSize().x * 4/5), arrowBack.getPosition().y + (arrowBack.getSize().y * 4/5)});
            }

            target.draw(arrowBack, states);
            target.draw(arrow, states);

            if (m_spinButton->m_mouseHover && !m_spinButton->m_mouseHoverOnTopArrow)
            {
                arrowBack.setFillColor(calcColorOpacity(m_backgroundColorHover, m_spinButton->getOpacity()));
                arrow.setFillColor(calcColorOpacity(m_arrowColorHover, m_spinButton->getOpacity()));
            }
            else
            {
                arrowBack.setFillColor(calcColorOpacity(m_backgroundColorNormal, m_spinButton->getOpacity()));
                arrow.setFillColor(calcColorOpacity(m_arrowColorNormal, m_spinButton->getOpacity()));
            }

            if (m_spinButton->m_verticalScroll)
            {
                arrowBack.move({0, arrowBack.getSize().y + m_spaceBetweenArrows});

                arrow.setPoint(0, {arrowBack.getPosition().x + (arrowBack.getSize().x / 5), arrowBack.getPosition().y + (arrowBack.getSize().y / 5)});
                arrow.setPoint(1, {arrowBack.getPosition().x + (arrowBack.getSize().x / 2), arrowBack.getPosition().y + (arrowBack.getSize().y * 4/5)});
                arrow.setPoint(2, {arrowBack.getPosition().x + (arrowBack.getSize().x * 4/5), arrowBack.getPosition().y + (arrowBack.getSize().y / 5)});
            }
            else // Spin button lies horizontal
            {
                arrowBack.move({arrowBack.getSize().x + m_spaceBetweenArrows, 0});

                arrow.setPoint(0, {arrowBack.getPosition().x + (arrowBack.getSize().x / 5), arrowBack.getPosition().y + (arrowBack.getSize().y / 5)});
                arrow.setPoint(1, {arrowBack.getPosition().x + (arrowBack.getSize().x * 4/5), arrowBack.getPosition().y + (arrowBack.getSize().y / 2)});
                arrow.setPoint(2, {arrowBack.getPosition().x + (arrowBack.getSize().x / 5), arrowBack.getPosition().y + (arrowBack.getSize().y * 4/5)});
            }

            target.draw(arrowBack, states);
            target.draw(arrow, states);
        }

        // Draw the space between the arrows if needed
        if (m_spaceBetweenArrows > 0)
        {
            sf::RectangleShape space;
            if (m_spinButton->m_verticalScroll)
            {
                space.setSize({size.x, m_spaceBetweenArrows});
                space.setPosition(position.x, position.y + (size.y - m_spaceBetweenArrows) / 2.0f);
            }
            else // The spin button lies horizontal
            {
                space.setSize({m_spaceBetweenArrows, size.y});
                space.setPosition(position.x + (size.x - m_spaceBetweenArrows) / 2.0f, position.y);
            }

            space.setFillColor(calcColorOpacity(m_borderColor, m_spinButton->getOpacity()));
            target.draw(space, states);
        }

        // Draw the borders around the spin button
        if (m_borders != Borders{0, 0, 0, 0})
        {
            // Draw left border
            sf::RectangleShape border({m_borders.left, size.y + m_borders.top});
            border.setPosition(position.x - m_borders.left, position.y - m_borders.top);
            border.setFillColor(calcColorOpacity(m_borderColor, m_spinButton->getOpacity()));
            target.draw(border, states);

            // Draw top border
            border.setSize({size.x + m_borders.right, m_borders.top});
            border.setPosition(position.x, position.y - m_borders.top);
            target.draw(border, states);

            // Draw right border
            border.setSize({m_borders.right, size.y + m_borders.bottom});
            border.setPosition(position.x + size.x, position.y);
            target.draw(border, states);

            // Draw bottom border
            border.setSize({size.x + m_borders.left, m_borders.bottom});
            border.setPosition(position.x - m_borders.left, position.y + size.y);
            target.draw(border, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> SpinButtonRenderer::clone(Widget* widget)
    {
        auto renderer = std::make_shared<SpinButtonRenderer>(*this);
        renderer->m_spinButton = static_cast<SpinButton*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
