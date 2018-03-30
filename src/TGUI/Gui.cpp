/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/Gui.hpp>
#include <TGUI/Clipboard.hpp>
#include <TGUI/ToolTip.hpp>
#include <TGUI/Clipping.hpp>

#include <SFML/Graphics/RenderTexture.hpp>

#include <cassert>

#ifdef SFML_SYSTEM_WINDOWS
    #define NOMB
    #define NOMINMAX
    #define VC_EXTRALEAN
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Gui::Gui() :
    #if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 5)
        m_target(nullptr)
    #else
        m_target        (nullptr),
        m_accessToWindow(false)
    #endif
    {
        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5
    Gui::Gui(sf::RenderWindow& window) :
        m_target        (&window),
        m_accessToWindow(true)
    {
        Clipboard::setWindowHandle(window.getSystemHandle());

        setView(window.getDefaultView());

        init();
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Gui::Gui(sf::RenderTarget& target) :
    #if SFML_VERSION_MAJOR > 2 || (SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR >= 5)
        m_target(&target)
    #else
        m_target        (&target),
        m_accessToWindow(false)
    #endif
    {
        setView(target.getDefaultView());

        init();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5
    void Gui::setTarget(sf::RenderWindow& window)
    {
        m_target = &window;

        m_accessToWindow = true;
        Clipboard::setWindowHandle(window.getSystemHandle());

        setView(window.getDefaultView());
    }
#endif

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::setTarget(sf::RenderTarget& target)
    {
    #if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5
        m_accessToWindow = false;
    #endif

        m_target = &target;

        setView(target.getDefaultView());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::RenderTarget* Gui::getTarget() const
    {
        return m_target;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::setView(const sf::View& view)
    {
        if ((m_view.getCenter() != view.getCenter()) || (m_view.getSize() != view.getSize()))
        {
            m_view = view;

            m_container->m_size = view.getSize();
            m_container->onSizeChange.emit(m_container.get(), m_container->getSize());

            for (auto& layout : m_container->m_boundSizeLayouts)
                layout->recalculateValue();
        }
        else // Set it anyway in case something changed that we didn't care to check
            m_view = view;

        Clipping::setGuiView(m_view);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::View& Gui::getView() const
    {
        return m_view;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::handleEvent(sf::Event event)
    {
        assert(m_target != nullptr);

        // Check if the event has something to do with the mouse
        if ((event.type == sf::Event::MouseMoved) || (event.type == sf::Event::TouchMoved)
         || (event.type == sf::Event::MouseButtonPressed) || (event.type == sf::Event::TouchBegan)
         || (event.type == sf::Event::MouseButtonReleased) || (event.type == sf::Event::TouchEnded)
         || (event.type == sf::Event::MouseWheelScrolled))
        {
            Vector2f mouseCoords;

            switch (event.type)
            {
                case sf::Event::MouseMoved:
                {
                    mouseCoords = m_target->mapPixelToCoords({event.mouseMove.x, event.mouseMove.y}, m_view);
                    event.mouseMove.x = static_cast<int>(mouseCoords.x + 0.5f);
                    event.mouseMove.y = static_cast<int>(mouseCoords.y + 0.5f);
                    break;
                }

                case sf::Event::MouseButtonPressed:
                case sf::Event::MouseButtonReleased:
                {
                    mouseCoords = m_target->mapPixelToCoords({event.mouseButton.x, event.mouseButton.y}, m_view);
                    event.mouseButton.x = static_cast<int>(mouseCoords.x + 0.5f);
                    event.mouseButton.y = static_cast<int>(mouseCoords.y + 0.5f);
                    break;
                }

                case sf::Event::MouseWheelScrolled:
                {
                    mouseCoords = m_target->mapPixelToCoords({event.mouseWheelScroll.x, event.mouseWheelScroll.y}, m_view);
                    event.mouseWheelScroll.x = static_cast<int>(mouseCoords.x + 0.5f);
                    event.mouseWheelScroll.y = static_cast<int>(mouseCoords.y + 0.5f);
                    break;
                }

                case sf::Event::TouchMoved:
                case sf::Event::TouchBegan:
                case sf::Event::TouchEnded:
                {
                    mouseCoords = m_target->mapPixelToCoords({event.touch.x, event.touch.y}, m_view);
                    event.touch.x = static_cast<int>(mouseCoords.x + 0.5f);
                    event.touch.y = static_cast<int>(mouseCoords.y + 0.5f);
                    break;
                }

                default:
                    break;
            }

            // If a tooltip is visible then hide it now
            if (m_visibleToolTip != nullptr)
            {
                // Correct the position of the tool tip so that it is relative again
                m_visibleToolTip->setPosition(m_visibleToolTip->getPosition() - ToolTip::getDistanceToMouse() - m_lastMousePos);

                remove(m_visibleToolTip);
                m_visibleToolTip = nullptr;
            }

            // Reset the data for the tooltip since the mouse has moved
            m_tooltipTime = {};
            m_tooltipPossible = true;
            m_lastMousePos = mouseCoords;
        }

        // Handle tab key presses
        else if (event.type == sf::Event::KeyPressed)
        {
            if (isTabKeyUsageEnabled() && (event.key.code == sf::Keyboard::Tab))
            {
                if (event.key.shift)
                    focusPreviousWidget();
                else
                    focusNextWidget();

                return true;
            }
        }

        // Keep track of whether the window is focused or not
        else if (event.type == sf::Event::LostFocus)
        {
            m_windowFocused = false;
        }
        else if (event.type == sf::Event::GainedFocus)
        {
            m_windowFocused = true;
        #if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5
            if (m_accessToWindow)
                Clipboard::setWindowHandle(static_cast<sf::RenderWindow*>(m_target)->getSystemHandle());
        #endif
        }

        return m_container->handleEvent(event);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::enableTabKeyUsage()
    {
        m_TabKeyUsageEnabled = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::disableTabKeyUsage()
    {
        m_TabKeyUsageEnabled = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::isTabKeyUsageEnabled() const
    {
        return m_TabKeyUsageEnabled;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::draw()
    {
        assert(m_target != nullptr);

        // Update the time
        if (m_windowFocused)
            updateTime(m_clock.restart());
        else
            m_clock.restart();

        // Change the view
        const sf::View oldView = m_target->getView();
        m_target->setView(m_view);

        // Draw the widgets
        m_container->drawWidgetContainer(m_target, sf::RenderStates::Default);

        // Restore the old view
        m_target->setView(oldView);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    GuiContainer::Ptr Gui::getContainer() const
    {
        return m_container;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::setFont(const Font& font)
    {
        m_container->setInheritedFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::shared_ptr<sf::Font> Gui::getFont() const
    {
        return m_container->getInheritedFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<Widget::Ptr>& Gui::getWidgets() const
    {
        return m_container->getWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<sf::String>& Gui::getWidgetNames() const
    {
        return m_container->getWidgetNames();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::add(const Widget::Ptr& widgetPtr, const sf::String& widgetName)
    {
        m_container->add(widgetPtr, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Gui::get(const sf::String& widgetName) const
    {
        return m_container->get(widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::remove(const Widget::Ptr& widget)
    {
        return m_container->remove(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::removeAllWidgets()
    {
        m_container->removeAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::setWidgetName(const Widget::Ptr& widget, const std::string& name)
    {
        return m_container->setWidgetName(widget, name);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Gui::getWidgetName(const Widget::Ptr& widget) const
    {
        return m_container->getWidgetName(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::focusNextWidget()
    {
        m_container->focusNextWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::focusPreviousWidget()
    {
        m_container->focusPreviousWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::unfocusAllWidgets()
    {
        m_container->setFocused(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::uncheckRadioButtons()
    {
        m_container->uncheckRadioButtons();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::setOpacity(float opacity)
    {
        m_container->setInheritedOpacity(opacity);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float Gui::getOpacity() const
    {
        return m_container->getInheritedOpacity();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::loadWidgetsFromFile(const std::string& filename)
    {
        m_container->loadWidgetsFromFile(filename);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::saveWidgetsToFile(const std::string& filename)
    {
        m_container->saveWidgetsToFile(filename);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::loadWidgetsFromStream(std::stringstream& stream)
    {
        m_container->loadWidgetsFromStream(stream);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::loadWidgetsFromStream(std::stringstream&& stream)
    {
        loadWidgetsFromStream(stream);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::saveWidgetsToStream(std::stringstream& stream) const
    {
        m_container->saveWidgetsToStream(stream);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::updateTime(const sf::Time& elapsedTime)
    {
        m_container->m_animationTimeElapsed = elapsedTime;
        m_container->update(elapsedTime);

        if (m_tooltipPossible)
        {
            m_tooltipTime += elapsedTime;
            if (m_tooltipTime >= ToolTip::getTimeToDisplay())
            {
                Widget::Ptr tooltip = m_container->askToolTip(m_lastMousePos);
                if (tooltip)
                {
                    m_visibleToolTip = tooltip;
                    add(tooltip, "#TGUI_INTERNAL$ToolTip#");

                    // Change the relative tool tip position in an absolute one
                    tooltip->setPosition(m_lastMousePos + ToolTip::getDistanceToMouse() + tooltip->getPosition());
                }

                m_tooltipPossible = false;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::init()
    {
    #ifdef SFML_SYSTEM_WINDOWS
        unsigned int doubleClickTime = GetDoubleClickTime();
        if (doubleClickTime > 0)
            setDoubleClickTime(doubleClickTime);
    #endif
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
