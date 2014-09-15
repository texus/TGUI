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


#include <TGUI/ChildWindow.hpp>

#include <SFML/OpenGL.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::ChildWindow()
    {
        m_widgetType = WidgetType::ChildWindow;

        addSignal<SignalVector2f>("MousePressed");
        addSignal<SignalChildWindowPtr>("Closed");

        m_renderer = std::make_shared<ChildWindowRenderer>(this);

        getRenderer()->setBorders({1, 1, 1, 1});

        setSize(400, 300);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::Ptr ChildWindow::create(const std::string& themeFileFilename, const std::string& section)
    {
        auto childWindow = std::make_shared<ChildWindow>();

        childWindow->createChildWindow(themeFileFilename, section);

        return childWindow;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChildWindow::Ptr ChildWindow::copy(ChildWindow::ConstPtr childWindow)
    {
        if (childWindow)
            return std::make_shared<ChildWindow>(*childWindow);
        else
            return nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setPosition(const Layout& position)
    {
        float x = position.getValue().x;
        float y = position.getValue().y;

        if (m_keepInParent)
        {
            if (y < 0)
                y = 0;
            else if (y > m_parent->getSize().y - getRenderer()->m_titleBarHeight)
                y = m_parent->getSize().y - getRenderer()->m_titleBarHeight;

            if (x < 0)
                x = 0;
            else if (x > m_parent->getSize().x - getSize().x)
                x = m_parent->getSize().x - getSize().x;
        }

        Widget::setPosition({x, y});

        getRenderer()->m_textureTitleBar.setPosition({x, y});
        m_iconTexture.setPosition(x + getRenderer()->m_distanceToSide, y + ((getRenderer()->m_titleBarHeight - m_iconTexture.getSize().y) / 2.0f));

        if (m_titleAlignment == TitleAlignmentLeft)
        {
            if (m_iconTexture.getData())
                m_titleText.setPosition(x + 2*getRenderer()->m_distanceToSide + m_iconTexture.getSize().x,
                                        y + ((getRenderer()->m_titleBarHeight - m_titleText.getSize().y) / 2.0f));
            else
                m_titleText.setPosition(x + getRenderer()->m_distanceToSide, y + ((getRenderer()->m_titleBarHeight - m_titleText.getSize().y) / 2.0f));

        }
        else if (m_titleAlignment == TitleAlignmentCentered)
        {
            if (m_iconTexture.getData())
                m_titleText.setPosition(x + (2 * getRenderer()->m_distanceToSide) + m_iconTexture.getSize().x
                                        + (((getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right) - (4 * getRenderer()->m_distanceToSide) - m_iconTexture.getSize().x - m_closeButton.getSize().x - m_titleText.getSize().x) / 2.0f),
                                        y + ((getRenderer()->m_titleBarHeight - m_titleText.getSize().y) / 2.0f));
            else
                m_titleText.setPosition(x + getRenderer()->m_distanceToSide + (((getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right) - (3 * getRenderer()->m_distanceToSide) - m_closeButton.getSize().x - m_titleText.getSize().x) / 2.0f),
                                        y + ((getRenderer()->m_titleBarHeight - m_titleText.getSize().y) / 2.0f));
        }
        else // if (m_titleAlignment == TitleAlignmentRight)
        {
            if (m_iconTexture.getData())
                m_titleText.setPosition(x + (getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right) - (2 * getRenderer()->m_distanceToSide) - m_closeButton.getSize().x - m_titleText.getSize().x,
                                        y + ((getRenderer()->m_titleBarHeight - m_titleText.getSize().y) / 2.0f));
            else
                m_titleText.setPosition(x + (getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right) - (2 * getRenderer()->m_distanceToSide) - m_closeButton.getSize().x - m_titleText.getSize().x,
                                        y + ((getRenderer()->m_titleBarHeight - m_titleText.getSize().y) / 2.0f));
        }

        m_closeButton.setPosition({x + (getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right) - getRenderer()->m_distanceToSide - m_closeButton.getSize().x,
                                   y + ((getRenderer()->m_titleBarHeight - m_closeButton.getSize().y) / 2.0f)});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setSize(const Layout& size)
    {
        Widget::setSize(size);

        if (getRenderer()->m_textureTitleBar.getData())
        {
            getRenderer()->m_textureTitleBar.setSize({getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right, getRenderer()->m_titleBarHeight});

            // If there is an icon then give it the correct size
            if (m_iconTexture.getData())
            {
                m_iconTexture.setSize({getRenderer()->m_titleBarHeight / getRenderer()->m_textureTitleBar.getImageSize().y * m_iconTexture.getImageSize().x,
                                       getRenderer()->m_titleBarHeight / getRenderer()->m_textureTitleBar.getImageSize().y * m_iconTexture.getImageSize().y});
            }
        }
        else
        {
            // If there is an icon then give it the correct size
            if (m_iconTexture.getData())
                m_iconTexture.setSize({((getRenderer()->m_titleBarHeight * 0.8f) / m_iconTexture.getImageSize().y) * m_iconTexture.getImageSize().x, getRenderer()->m_titleBarHeight * 0.8f});
        }

        // Reposition the images and text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChildWindow::getFullSize() const
    {
        return {getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right,
                getSize().y + getRenderer()->m_borders.top + getRenderer()->m_borders.bottom + getRenderer()->m_titleBarHeight};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setGlobalFont(const std::string& filename)
    {
        Container::setGlobalFont(filename);

        m_closeButton.getRenderer()->setTextFont(getGlobalFont());
        m_titleText.setTextFont(getGlobalFont());
        setTitle(getTitle());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setGlobalFont(std::shared_ptr<sf::Font> font)
    {
        Container::setGlobalFont(font);

        m_closeButton.getRenderer()->setTextFont(getGlobalFont());
        m_titleText.setTextFont(getGlobalFont());
        setTitle(getTitle());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTransparency(unsigned char transparency)
    {
        Container::setTransparency(transparency);

        m_iconTexture.setColor(sf::Color(255, 255, 255, m_opacity));
        getRenderer()->m_textureTitleBar.setColor(sf::Color(255, 255, 255, m_opacity));

        m_closeButton.setTransparency(m_opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitle(const sf::String& title)
    {
        m_titleText.setText(title);

        // Reposition the images and text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setTitleAlignment(TitleAlignment alignment)
    {
        m_titleAlignment = alignment;

        // Reposition the images and text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::setIcon(const std::string& filename)
    {
        m_iconTexture.load(filename);

        updateSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::removeIcon()
    {
        m_iconTexture = {};

        // Reposition the images and text
        updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::destroy()
    {
        m_parent->remove(shared_from_this());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::keepInParent(bool enabled)
    {
        m_keepInParent = enabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::isKeptInParent() const
    {
        return m_keepInParent;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChildWindow::getWidgetsOffset() const
    {
        return {getRenderer()->m_borders.left, getRenderer()->m_borders.top + getRenderer()->m_titleBarHeight};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChildWindow::mouseOnWidget(float x, float y)
    {
        // Check if the mouse is on top of the title bar
        if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right, getRenderer()->m_titleBarHeight}.contains(x, y))
        {
            for (unsigned int i = 0; i < m_widgets.size(); ++i)
                m_widgets[i]->mouseNotOnWidget();

            return true;
        }
        else
        {
            // Check if the mouse is inside the child window
            if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right,
                              getSize().y + getRenderer()->m_borders.top + getRenderer()->m_borders.bottom}.contains(x, y - getRenderer()->m_titleBarHeight))
            {
                return true;
            }
            else
            {
                if (m_mouseHover)
                {
                    mouseLeftWidget();

                    // Tell the widgets inside the child window that the mouse is no longer on top of them
                    for (unsigned int i = 0; i < m_widgets.size(); ++i)
                        m_widgets[i]->mouseNotOnWidget();

                    m_closeButton.mouseNotOnWidget();
                }

                return false;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMousePressed(float x, float y)
    {
        m_mouseDown = true;

        // Move the child window to the front
        m_parent->moveWidgetToFront(this);
        sendSignal("MousePressed", sf::Vector2f{x - getPosition().x, y - getPosition().y});

        // Check if the mouse is on top of the title bar
        if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x + getRenderer()->getBorders().left + getRenderer()->getBorders().right, getRenderer()->m_titleBarHeight}.contains(x, y))
        {
            // Send the mouse press event to the close button
            if (m_closeButton.mouseOnWidget(x, y))
                m_closeButton.leftMousePressed(x, y);
            else
            {
                // The mouse went down on the title bar
                m_mouseDownOnTitleBar = true;

                // Remember where we are dragging the title bar
                m_draggingPosition.x = x - getPosition().x;
                m_draggingPosition.y = y - getPosition().y;
            }

            return;
        }
        else // The mouse is not on top of the title bar
        {
            // When the mouse is not on the title bar, the mouse can't be on the close button
            if (m_closeButton.m_mouseHover)
                m_closeButton.mouseNotOnWidget();

            // Check if the mouse is on top of the borders
            if ((sf::FloatRect{getPosition().x, getPosition().y, getSize().x + getRenderer()->getBorders().left + getRenderer()->getBorders().right,
                               getSize().y + getRenderer()->getBorders().top + getRenderer()->getBorders().bottom + getRenderer()->m_titleBarHeight}.contains(x, y))
             && (sf::FloatRect{getPosition().x + getRenderer()->getBorders().left, getPosition().y + getRenderer()->m_titleBarHeight + getRenderer()->getBorders().top,
                               getSize().x, getSize().y}.contains(x, y) == false))
            {
                // Don't send the event to the widgets
                return;
            }
        }

        Container::leftMousePressed(x - getRenderer()->getBorders().left, y - (getRenderer()->m_titleBarHeight + getRenderer()->getBorders().top));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::leftMouseReleased(float x , float y)
    {
        m_mouseDown = false;
        m_mouseDownOnTitleBar = false;

        // Check if the mouse is on top of the title bar
        if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x + getRenderer()->getBorders().left + getRenderer()->getBorders().right, getRenderer()->m_titleBarHeight}.contains(x, y))
        {
            // Check if the close button was clicked
            if (m_closeButton.m_mouseDown == true)
            {
                m_closeButton.m_mouseDown = false;

                // Check if the mouse is still on the close button
                if (m_closeButton.mouseOnWidget(x, y))
                {
                    if (isSignalBound("closed"))
                        sendSignal("closed", std::static_pointer_cast<ChildWindow>(shared_from_this()));
                    else
                    {
                        destroy();
                        return;
                    }
                }
            }

            // Tell the widgets that the mouse is no longer down
            for (unsigned int i = 0; i < m_widgets.size(); ++i)
                m_widgets[i]->mouseNoLongerDown();

            return;
        }
        else // The mouse is not on top of the title bar
        {
            // When the mouse is not on the title bar, the mouse can't be on the close button
            if (m_closeButton.m_mouseHover)
                m_closeButton.mouseNotOnWidget();

            m_closeButton.mouseNoLongerDown();

            // Check if the mouse is on top of the borders
            if ((sf::FloatRect{getPosition().x, getPosition().y, getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right,
                               getSize().y + getRenderer()->m_borders.top + getRenderer()->m_borders.bottom + getRenderer()->m_titleBarHeight}.contains(x, y))
             && (sf::FloatRect{getPosition().x + getRenderer()->m_borders.left, getPosition().y + getRenderer()->m_titleBarHeight + getRenderer()->m_borders.top,
                               getSize().x, getSize().y}.contains(x, y) == false))
            {
                // Tell the widgets about that the mouse was released
                for (unsigned int i = 0; i < m_widgets.size(); ++i)
                    m_widgets[i]->mouseNoLongerDown();

                // Don't send the event to the widgets
                return;
            }
        }

        Container::leftMouseReleased(x - getRenderer()->m_borders.left, y - (getRenderer()->m_titleBarHeight + getRenderer()->m_borders.top));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseMoved(float x, float y)
    {
        m_mouseHover = true;

        // Check if you are dragging the child window
        if (m_mouseDown && m_mouseDownOnTitleBar)
        {
            // Move the child window
            setPosition(sf::Vector2f{x, y} - m_draggingPosition);
        }

        // Check if the mouse is on top of the title bar
        if (sf::FloatRect{getPosition().x, getPosition().y, getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right, getRenderer()->m_titleBarHeight}.contains(x, y))
        {
            // Send the hover event to the close button
            if (m_closeButton.mouseOnWidget(x, y))
                m_closeButton.mouseMoved(x, y);

            return;
        }
        else // The mouse is not on top of the title bar
        {
            // When the mouse is not on the title bar, the mouse can't be on the close button
            if (m_closeButton.m_mouseHover)
                m_closeButton.mouseNotOnWidget();

            // Check if the mouse is on top of the borders
            if ((sf::FloatRect{getPosition().x, getPosition().y, getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right,
                               getSize().y + getRenderer()->m_borders.top + getRenderer()->m_borders.bottom + getRenderer()->m_titleBarHeight}.contains(x, y))
             && (sf::FloatRect{getPosition().x + getRenderer()->m_borders.left, getPosition().y + getRenderer()->m_titleBarHeight + getRenderer()->m_borders.top,
                               getSize().x, getSize().y}.contains(x, y) == false))
            {
                // Don't send the event to the widgets
                return;
            }
        }

        Container::mouseMoved(x - getRenderer()->m_borders.left, y - getRenderer()->m_titleBarHeight - getRenderer()->m_borders.top);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseWheelMoved(int delta, int x, int y)
    {
        Container::mouseWheelMoved(delta, x - getRenderer()->m_borders.left, y - (getRenderer()->m_titleBarHeight + getRenderer()->m_borders.top));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::mouseNoLongerDown()
    {
        Container::mouseNoLongerDown();
        m_closeButton.mouseNoLongerDown();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::initialize(Container *const parent)
    {
        Widget::initialize(parent);

        m_closeButton.initialize(this);

        if (!m_font && m_parent->getGlobalFont())
            setGlobalFont(m_parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::createChildWindow(const std::string& themeFileFilename, const std::string& section)
    {
        if (themeFileFilename != "")
        {
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
                    if (it->first == "closebutton")
                    {
                        if ((it->second.length() < 3) || (it->second[0] != '"') || (it->second[it->second.length()-1] != '"'))
                            throw Exception{"Failed to parse value for 'CloseButton' property."};

                        m_closeButton = *Button::create(themeFileFilename, it->second.substr(1, it->second.length()-2));
                    }
                    else
                        getRenderer()->ChildWindowRenderer::setProperty(it->first, it->second, themeFileFolder);
                }
                catch (const Exception& e)
                {
                    throw Exception{std::string(e.what()) + " In section '" + section + "' in " + loadedThemeFile + "."};
                }
            }

            if (getRenderer()->m_textureTitleBar.getData())
                getRenderer()->m_titleBarHeight = getRenderer()->m_textureTitleBar.getImageSize().y;
        }

        if (!m_closeButton.getRenderer()->m_textureNormal.getData())
        {
            m_closeButton.setSize({getRenderer()->m_titleBarHeight * 0.8f, getRenderer()->m_titleBarHeight * 0.8f});
            m_closeButton.getRenderer()->setBorders({1, 1, 1, 1});
            m_closeButton.setText("X");
        }

        // Set the size of the title text
        m_titleText.setTextSize(getRenderer()->m_titleBarHeight * 0.8f);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindow::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        getRenderer()->draw(target, states);

        // Draw a window icon if one was set
        if (m_iconTexture.getData())
            target.draw(m_iconTexture, states);

        // Calculate the scale factor of the view
        const sf::View& view = target.getView();
        float scaleViewX = target.getSize().x / view.getSize().x;
        float scaleViewY = target.getSize().y / view.getSize().y;

        // Get the global position
        sf::Vector2f topLeftPanelPosition = {((getAbsolutePosition().x + getRenderer()->m_borders.left - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                             ((getAbsolutePosition().y + getRenderer()->m_titleBarHeight + getRenderer()->m_borders.top - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};
        sf::Vector2f bottomRightPanelPosition = {(getAbsolutePosition().x + getSize().x + getRenderer()->m_borders.left - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                                 (getAbsolutePosition().y + getRenderer()->m_titleBarHeight + getSize().y + getRenderer()->m_borders.top - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top)};

        sf::Vector2f topLeftTitleBarPosition;
        sf::Vector2f bottomRightTitleBarPosition;

        if (m_iconTexture.getData())
        {
            topLeftTitleBarPosition = {((getAbsolutePosition().x + 2*getRenderer()->m_distanceToSide + m_iconTexture.getSize().x - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                       ((getAbsolutePosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};
        }
        else
        {
            topLeftTitleBarPosition = {((getAbsolutePosition().x + getRenderer()->m_distanceToSide - view.getCenter().x + (view.getSize().x / 2.f)) * view.getViewport().width) + (view.getSize().x * view.getViewport().left),
                                       ((getAbsolutePosition().y - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height) + (view.getSize().y * view.getViewport().top)};
        }

        bottomRightTitleBarPosition = {(getAbsolutePosition().x + getSize().x + getRenderer()->m_borders.left + getRenderer()->m_borders.right - (2*getRenderer()->m_distanceToSide) - m_closeButton.getSize().x - view.getCenter().x + (view.getSize().x / 2.f))
                                       * view.getViewport().width + (view.getSize().x * view.getViewport().left),
                                       (getAbsolutePosition().y + getRenderer()->m_titleBarHeight - view.getCenter().y + (view.getSize().y / 2.f)) * view.getViewport().height + (view.getSize().y * view.getViewport().top)};

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Check if there is a title
        if (m_titleText.getText().isEmpty() == false)
        {
            // Calculate the clipping area
            GLint scissorLeft = std::max(static_cast<GLint>(topLeftTitleBarPosition.x * scaleViewX), scissor[0]);
            GLint scissorTop = std::max(static_cast<GLint>(topLeftTitleBarPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
            GLint scissorRight = std::min(static_cast<GLint>(bottomRightTitleBarPosition.x * scaleViewX), scissor[0] + scissor[2]);
            GLint scissorBottom = std::min(static_cast<GLint>(bottomRightTitleBarPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

            if (scissorRight < scissorLeft)
                scissorRight = scissorLeft;
            else if (scissorBottom < scissorTop)
                scissorTop = scissorBottom;

            // Set the clipping area
            glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

            // Draw the text in the title bar
            target.draw(m_titleText, states);

            // Reset the old clipping area
            glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
        }


        states.transform.translate(getPosition().x + getRenderer()->m_borders.left, getPosition().y + getRenderer()->m_titleBarHeight + getRenderer()->m_borders.top);

        // Draw the background
        if (getRenderer()->m_backgroundColor != sf::Color::Transparent)
        {
            sf::RectangleShape background(getSize());
            background.setFillColor(getRenderer()->m_backgroundColor);
            target.draw(background, states);
        }

        // Calculate the clipping area
        GLint scissorLeft = std::max(static_cast<GLint>(topLeftPanelPosition.x * scaleViewX), scissor[0]);
        GLint scissorTop = std::max(static_cast<GLint>(topLeftPanelPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
        GLint scissorRight = std::min(static_cast<GLint>(bottomRightPanelPosition.x * scaleViewX), scissor[0] + scissor[2]);
        GLint scissorBottom = std::min(static_cast<GLint>(bottomRightPanelPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

        if (scissorRight < scissorLeft)
            scissorRight = scissorLeft;
        else if (scissorBottom < scissorTop)
            scissorTop = scissorBottom;

        // Set the clipping area
        glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

        // Draw the widgets in the child window
        drawWidgetContainer(&target, states);

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setProperty(std::string property, const std::string& value, const std::string& rootPath)
    {
        if (property == "titlebarimage")
            extractTextureFromString(property, value, rootPath, m_textureTitleBar);
        else if (property == "titlebarcolor")
            setTitleBarColor(extractColorFromString(property, value));
        else if (property == "backgroundcolor")
            setBackgroundColor(extractColorFromString(property, value));
        else if (property == "titlecolor")
            setTitleColor(extractColorFromString(property, value));
        else if (property == "bordercolor")
            setBorderColor(extractColorFromString(property, value));
        else if (property == "borders")
            setBorders(extractBordersFromString(property, value));
        else if (property == "distancetoside")
            setDistanceToSide(tgui::stoul(value));
        else if (property == "titlebarheight")
            setTitleBarHeight(tgui::stoul(value));
        else
            throw Exception{"Unrecognized property '" + property + "'."};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setTitleBarImage(const std::string& filename, const sf::IntRect& partRect, const sf::IntRect& middlePart, bool repeated)
    {
        if (filename != "")
            m_textureTitleBar.load(getResourcePath() + filename, partRect, middlePart, repeated);
        else
            m_textureTitleBar = {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setTitleBarColor(const sf::Color& color)
    {
        m_titleBarColor = color;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setTitleBarHeight(float height)
    {
        m_titleBarHeight = height;

        // Set the size of the close button
        m_childWindow->m_closeButton.setSize({m_childWindow->m_closeButton.getSize().x * (height / m_titleBarHeight),
                                              m_childWindow->m_closeButton.getSize().y * (height / m_titleBarHeight)});

        // Set the size of the text in the title bar
        m_childWindow->m_titleText.setTextSize(static_cast<unsigned int>(m_titleBarHeight * 0.8f));

        m_textureTitleBar.setSize({m_childWindow->getSize().x + m_borders.left + m_borders.right, m_titleBarHeight});

        // Reposition the images and text
        m_childWindow->updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setTitleColor(const sf::Color& color)
    {
        m_childWindow->m_titleText.setTextColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setBorderColor(const sf::Color& borderColor)
    {
        m_borderColor = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setBorders(const Borders& borders)
    {
        m_borders = borders;

        m_textureTitleBar.setSize({m_childWindow->getSize().x + borders.left + borders.right, m_titleBarHeight});

        // Reposition the images and text
        m_childWindow->updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setDistanceToSide(unsigned int distanceToSide)
    {
        m_distanceToSide = distanceToSide;

        // Reposition the images and text
        m_childWindow->updatePosition();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_backgroundColor = backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<ButtonRenderer> ChildWindowRenderer::getCloseButton() const
    {
        return m_childWindow->m_closeButton.getRenderer();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChildWindowRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Draw the title bar
        if (m_textureTitleBar.getData())
            target.draw(m_textureTitleBar, states);
        else
        {
            sf::RectangleShape titleBar{{m_childWindow->getSize().x + m_borders.left + m_borders.right, m_titleBarHeight}};
            titleBar.setPosition({m_childWindow->getPosition().x, m_childWindow->getPosition().y});
            titleBar.setFillColor(m_titleBarColor);
            target.draw(titleBar, states);
        }

        // Draw the close button
        target.draw(m_childWindow->m_closeButton, states);

        // Draw the borders
        if (m_borders != Borders{0, 0, 0, 0})
        {
            sf::Vector2f size = m_childWindow->getSize();
            sf::Vector2f position = {m_childWindow->getPosition().x, m_childWindow->getPosition().y + m_titleBarHeight};

            // Draw left border
            sf::RectangleShape border({m_borders.left, size.y + m_borders.top});
            border.setPosition(position.x, position.y);
            border.setFillColor(m_borderColor);
            target.draw(border, states);

            // Draw top border
            border.setSize({size.x + m_borders.right, m_borders.top});
            border.setPosition(position.x + m_borders.left, position.y);
            target.draw(border, states);

            // Draw right border
            border.setSize({m_borders.right, size.y + m_borders.bottom});
            border.setPosition(position.x + size.x + m_borders.left, position.y + m_borders.top);
            target.draw(border, states);

            // Draw bottom border
            border.setSize({size.x + m_borders.left, m_borders.bottom});
            border.setPosition(position.x, position.y + size.y + m_borders.top);
            target.draw(border, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<WidgetRenderer> ChildWindowRenderer::clone(Widget* widget)
    {
        auto renderer = std::shared_ptr<ChildWindowRenderer>(new ChildWindowRenderer{*this});
        renderer->m_childWindow = static_cast<ChildWindow*>(widget);
        return renderer;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
