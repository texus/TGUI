/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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
#include <TGUI/Event.hpp>

#include <SFML/Graphics/RenderTexture.hpp>

#include <cassert>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    static Event::KeyboardKey convertKeyCode(sf::Keyboard::Key key)
    {
        switch (key)
        {
        case sf::Keyboard::Key::A:          return Event::KeyboardKey::A;
        case sf::Keyboard::Key::B:          return Event::KeyboardKey::B;
        case sf::Keyboard::Key::C:          return Event::KeyboardKey::C;
        case sf::Keyboard::Key::D:          return Event::KeyboardKey::D;
        case sf::Keyboard::Key::E:          return Event::KeyboardKey::E;
        case sf::Keyboard::Key::F:          return Event::KeyboardKey::F;
        case sf::Keyboard::Key::G:          return Event::KeyboardKey::G;
        case sf::Keyboard::Key::H:          return Event::KeyboardKey::H;
        case sf::Keyboard::Key::I:          return Event::KeyboardKey::I;
        case sf::Keyboard::Key::J:          return Event::KeyboardKey::J;
        case sf::Keyboard::Key::K:          return Event::KeyboardKey::K;
        case sf::Keyboard::Key::L:          return Event::KeyboardKey::L;
        case sf::Keyboard::Key::M:          return Event::KeyboardKey::M;
        case sf::Keyboard::Key::N:          return Event::KeyboardKey::N;
        case sf::Keyboard::Key::O:          return Event::KeyboardKey::O;
        case sf::Keyboard::Key::P:          return Event::KeyboardKey::P;
        case sf::Keyboard::Key::Q:          return Event::KeyboardKey::Q;
        case sf::Keyboard::Key::R:          return Event::KeyboardKey::R;
        case sf::Keyboard::Key::S:          return Event::KeyboardKey::S;
        case sf::Keyboard::Key::T:          return Event::KeyboardKey::T;
        case sf::Keyboard::Key::U:          return Event::KeyboardKey::U;
        case sf::Keyboard::Key::V:          return Event::KeyboardKey::V;
        case sf::Keyboard::Key::W:          return Event::KeyboardKey::W;
        case sf::Keyboard::Key::X:          return Event::KeyboardKey::X;
        case sf::Keyboard::Key::Y:          return Event::KeyboardKey::Y;
        case sf::Keyboard::Key::Z:          return Event::KeyboardKey::Z;
        case sf::Keyboard::Key::Num0:       return Event::KeyboardKey::Num0;
        case sf::Keyboard::Key::Num1:       return Event::KeyboardKey::Num1;
        case sf::Keyboard::Key::Num2:       return Event::KeyboardKey::Num2;
        case sf::Keyboard::Key::Num3:       return Event::KeyboardKey::Num3;
        case sf::Keyboard::Key::Num4:       return Event::KeyboardKey::Num4;
        case sf::Keyboard::Key::Num5:       return Event::KeyboardKey::Num5;
        case sf::Keyboard::Key::Num6:       return Event::KeyboardKey::Num6;
        case sf::Keyboard::Key::Num7:       return Event::KeyboardKey::Num7;
        case sf::Keyboard::Key::Num8:       return Event::KeyboardKey::Num8;
        case sf::Keyboard::Key::Num9:       return Event::KeyboardKey::Num9;
        case sf::Keyboard::Key::Escape:     return Event::KeyboardKey::Escape;
        case sf::Keyboard::Key::LControl:   return Event::KeyboardKey::LControl;
        case sf::Keyboard::Key::LShift:     return Event::KeyboardKey::LShift;
        case sf::Keyboard::Key::LAlt:       return Event::KeyboardKey::LAlt;
        case sf::Keyboard::Key::LSystem:    return Event::KeyboardKey::LSystem;
        case sf::Keyboard::Key::RControl:   return Event::KeyboardKey::RControl;
        case sf::Keyboard::Key::RShift:     return Event::KeyboardKey::RShift;
        case sf::Keyboard::Key::RAlt:       return Event::KeyboardKey::RAlt;
        case sf::Keyboard::Key::RSystem:    return Event::KeyboardKey::RSystem;
        case sf::Keyboard::Key::Menu:       return Event::KeyboardKey::Menu;
        case sf::Keyboard::Key::LBracket:   return Event::KeyboardKey::LBracket;
        case sf::Keyboard::Key::RBracket:   return Event::KeyboardKey::RBracket;
#if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5
        case sf::Keyboard::Key::SemiColon:  return Event::KeyboardKey::Semicolon;
#else
        case sf::Keyboard::Key::Semicolon:  return Event::KeyboardKey::Semicolon;
#endif
        case sf::Keyboard::Key::Comma:      return Event::KeyboardKey::Comma;
        case sf::Keyboard::Key::Period:     return Event::KeyboardKey::Period;
        case sf::Keyboard::Key::Quote:      return Event::KeyboardKey::Quote;
        case sf::Keyboard::Key::Slash:      return Event::KeyboardKey::Slash;
#if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5
        case sf::Keyboard::Key::BackSlash:  return Event::KeyboardKey::Backslash;
#else
        case sf::Keyboard::Key::Backslash:  return Event::KeyboardKey::Backslash;
#endif
        case sf::Keyboard::Key::Tilde:      return Event::KeyboardKey::Tilde;
        case sf::Keyboard::Key::Equal:      return Event::KeyboardKey::Equal;
#if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5
        case sf::Keyboard::Key::Dash:       return Event::KeyboardKey::Hyphen;
#else
        case sf::Keyboard::Key::Hyphen:     return Event::KeyboardKey::Hyphen;
#endif
        case sf::Keyboard::Key::Space:      return Event::KeyboardKey::Space;
#if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5
        case sf::Keyboard::Key::Return:     return Event::KeyboardKey::Enter;
        case sf::Keyboard::Key::BackSpace:  return Event::KeyboardKey::Backspace;
#else
        case sf::Keyboard::Key::Enter:      return Event::KeyboardKey::Enter;
        case sf::Keyboard::Key::Backspace:  return Event::KeyboardKey::Backspace;
#endif
        case sf::Keyboard::Key::Tab:        return Event::KeyboardKey::Tab;
        case sf::Keyboard::Key::PageUp:     return Event::KeyboardKey::PageUp;
        case sf::Keyboard::Key::PageDown:   return Event::KeyboardKey::PageDown;
        case sf::Keyboard::Key::End:        return Event::KeyboardKey::End;
        case sf::Keyboard::Key::Home:       return Event::KeyboardKey::Home;
        case sf::Keyboard::Key::Insert:     return Event::KeyboardKey::Insert;
#if defined(TGUI_SYSTEM_ANDROID) && SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 5
        case sf::Keyboard::Key::Delete:     return Event::KeyboardKey::Backspace; // SFML versions prior to 2.5 sent Delete instead of BackSpace on Android
#else
        case sf::Keyboard::Key::Delete:     return Event::KeyboardKey::Delete;
#endif
        case sf::Keyboard::Key::Add:        return Event::KeyboardKey::Add;
        case sf::Keyboard::Key::Subtract:   return Event::KeyboardKey::Subtract;
        case sf::Keyboard::Key::Multiply:   return Event::KeyboardKey::Multiply;
        case sf::Keyboard::Key::Divide:     return Event::KeyboardKey::Divide;
        case sf::Keyboard::Key::Left:       return Event::KeyboardKey::Left;
        case sf::Keyboard::Key::Right:      return Event::KeyboardKey::Right;
        case sf::Keyboard::Key::Up:         return Event::KeyboardKey::Up;
        case sf::Keyboard::Key::Down:       return Event::KeyboardKey::Down;
        case sf::Keyboard::Key::Numpad0:    return Event::KeyboardKey::Numpad0;
        case sf::Keyboard::Key::Numpad1:    return Event::KeyboardKey::Numpad1;
        case sf::Keyboard::Key::Numpad2:    return Event::KeyboardKey::Numpad2;
        case sf::Keyboard::Key::Numpad3:    return Event::KeyboardKey::Numpad3;
        case sf::Keyboard::Key::Numpad4:    return Event::KeyboardKey::Numpad4;
        case sf::Keyboard::Key::Numpad5:    return Event::KeyboardKey::Numpad5;
        case sf::Keyboard::Key::Numpad6:    return Event::KeyboardKey::Numpad6;
        case sf::Keyboard::Key::Numpad7:    return Event::KeyboardKey::Numpad7;
        case sf::Keyboard::Key::Numpad8:    return Event::KeyboardKey::Numpad8;
        case sf::Keyboard::Key::Numpad9:    return Event::KeyboardKey::Numpad9;
        case sf::Keyboard::Key::F1:         return Event::KeyboardKey::F1;
        case sf::Keyboard::Key::F2:         return Event::KeyboardKey::F2;
        case sf::Keyboard::Key::F3:         return Event::KeyboardKey::F3;
        case sf::Keyboard::Key::F4:         return Event::KeyboardKey::F4;
        case sf::Keyboard::Key::F5:         return Event::KeyboardKey::F5;
        case sf::Keyboard::Key::F6:         return Event::KeyboardKey::F6;
        case sf::Keyboard::Key::F7:         return Event::KeyboardKey::F7;
        case sf::Keyboard::Key::F8:         return Event::KeyboardKey::F8;
        case sf::Keyboard::Key::F9:         return Event::KeyboardKey::F9;
        case sf::Keyboard::Key::F10:        return Event::KeyboardKey::F10;
        case sf::Keyboard::Key::F11:        return Event::KeyboardKey::F11;
        case sf::Keyboard::Key::F12:        return Event::KeyboardKey::F12;
        case sf::Keyboard::Key::F13:        return Event::KeyboardKey::F13;
        case sf::Keyboard::Key::F14:        return Event::KeyboardKey::F14;
        case sf::Keyboard::Key::F15:        return Event::KeyboardKey::F15;
        case sf::Keyboard::Key::Pause:      return Event::KeyboardKey::Pause;
        default: // We don't process the other keys
            return Event::KeyboardKey::Unknown;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    static bool convertEventSFML(const sf::Event& eventSFML, Event& eventTGUI)
    {
        switch (eventSFML.type)
        {
            case sf::Event::LostFocus:
            {
                eventTGUI.type = Event::Type::LostFocus;
                return true;
            }
            case sf::Event::GainedFocus:
            {
                eventTGUI.type = Event::Type::LostFocus;
                return true;
            }
            case sf::Event::Resized:
            {
                eventTGUI.type = Event::Type::Resized;
                eventTGUI.size.width = eventSFML.size.width;
                eventTGUI.size.height = eventSFML.size.height;
                return true;
            }
            case sf::Event::TextEntered:
            {
                eventTGUI.type = Event::Type::TextEntered;
                eventTGUI.text.unicode = eventSFML.text.unicode;
                return true;
            }
            case sf::Event::KeyPressed:
            {
                const Event::KeyboardKey code = convertKeyCode(eventSFML.key.code);
                if (code == Event::KeyboardKey::Unknown)
                    return false; // This key isn't handled by TGUI

                eventTGUI.type = Event::Type::KeyPressed;
                eventTGUI.key.code = code;
                eventTGUI.key.alt = eventSFML.key.alt;
                eventTGUI.key.control = eventSFML.key.control;
                eventTGUI.key.shift = eventSFML.key.shift;
                eventTGUI.key.system = eventSFML.key.system;
                return true;
            }
            case sf::Event::MouseWheelScrolled:
            {
                if (eventSFML.mouseWheelScroll.wheel != sf::Mouse::Wheel::VerticalWheel)
                    return false; // TGUI only handles the vertical mouse wheel

                eventTGUI.type = Event::Type::MouseWheelScrolled;
                eventTGUI.mouseWheel.delta = eventSFML.mouseWheelScroll.delta;
                eventTGUI.mouseWheel.x = eventSFML.mouseWheelScroll.x;
                eventTGUI.mouseWheel.y = eventSFML.mouseWheelScroll.y;
                return true;
            }
            case sf::Event::MouseButtonPressed:
            case sf::Event::MouseButtonReleased:
            {
                switch (eventSFML.mouseButton.button)
                {
                case sf::Mouse::Button::Left:
                    eventTGUI.mouseButton.button = Event::MouseButton::Left;
                    break;
                case sf::Mouse::Button::Middle:
                    eventTGUI.mouseButton.button = Event::MouseButton::Middle;
                    break;
                case sf::Mouse::Button::Right:
                    eventTGUI.mouseButton.button = Event::MouseButton::Right;
                    break;
                default: // This mouse button isn't handled by TGUI
                    return false;
                }

                if (eventSFML.type == sf::Event::MouseButtonPressed)
                    eventTGUI.type = Event::Type::MouseButtonPressed;
                else
                    eventTGUI.type = Event::Type::MouseButtonReleased;

                eventTGUI.mouseButton.x = eventSFML.mouseButton.x;
                eventTGUI.mouseButton.y = eventSFML.mouseButton.y;
                return true;
            }
            case sf::Event::MouseMoved:
            {
                eventTGUI.type = Event::Type::MouseMoved;
                eventTGUI.mouseMove.x = eventSFML.mouseMove.x;
                eventTGUI.mouseMove.y = eventSFML.mouseMove.y;
                return true;
            }
            case sf::Event::TouchMoved:
            {
                if (eventSFML.touch.finger != 0)
                    return false; // Only the first finger is handled

                // Simulate a MouseMoved event
                eventTGUI.type = Event::Type::MouseMoved;
                eventTGUI.mouseMove.x = eventSFML.touch.x;
                eventTGUI.mouseMove.y = eventSFML.touch.y;
                return true;
            }
            case sf::Event::TouchBegan:
            case sf::Event::TouchEnded:
            {
                if (eventSFML.touch.finger != 0)
                    return false; // Only the first finger is handled

                // Simulate a MouseButtonPressed or MouseButtonReleased event
                if (eventSFML.type == sf::Event::TouchBegan)
                    eventTGUI.type = Event::Type::MouseButtonPressed;
                else
                    eventTGUI.type = Event::Type::MouseButtonReleased;

                eventTGUI.mouseButton.button = Event::MouseButton::Left;
                eventTGUI.mouseButton.x = eventSFML.touch.x;
                eventTGUI.mouseButton.y = eventSFML.touch.y;
                return true;
            }
            default: // This event is not handled by TGUI
                return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Gui::Gui() :
        m_target(nullptr)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Gui::Gui(sf::RenderTarget& target) :
        m_target(&target)
    {
        setView(target.getDefaultView());
        m_customViewSet = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::setTarget(sf::RenderTarget& target)
    {
        m_target = &target;

        setView(target.getDefaultView());
        m_customViewSet = false;
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

            m_container->m_size = Vector2f{view.getSize()};
            m_container->onSizeChange.emit(m_container.get(), m_container->getSize());

            for (auto& layout : m_container->m_boundSizeLayouts)
                layout->recalculateValue();
        }
        else // Set it anyway in case something changed that we didn't care to check
            m_view = view;

        m_customViewSet = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::View& Gui::getView() const
    {
        return m_view;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::handleEvent(sf::Event sfmlEvent)
    {
        assert(m_target != nullptr);

        Event event;
        if (!convertEventSFML(sfmlEvent, event))
            return false; // We don't process this type of event

        switch (event.type)
        {
            case Event::Type::MouseMoved:
            case Event::Type::MouseButtonPressed:
            case Event::Type::MouseButtonReleased:
            case Event::Type::MouseWheelScrolled:
            {
                Vector2f mouseCoords;
                if (event.type == Event::Type::MouseMoved)
                    mouseCoords = Vector2f{m_target->mapPixelToCoords({sfmlEvent.mouseMove.x, sfmlEvent.mouseMove.y}, m_view)};
                else if (event.type == Event::Type::MouseWheelScrolled)
                    mouseCoords = Vector2f{m_target->mapPixelToCoords({sfmlEvent.mouseWheelScroll.x, sfmlEvent.mouseWheelScroll.y}, m_view)};
                else // if ((event.type == Event::Type::MouseButtonPressed) || (event.type == Event::Type::MouseButtonReleased))
                    mouseCoords = Vector2f{m_target->mapPixelToCoords({sfmlEvent.mouseButton.x, sfmlEvent.mouseButton.y}, m_view)};

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

                if (event.type == Event::Type::MouseMoved)
                    return m_container->processMouseMoveEvent(mouseCoords);
                else if (event.type == Event::Type::MouseWheelScrolled)
                    return m_container->processMouseWheelScrollEvent(event.mouseWheel.delta, mouseCoords);
                else if (event.type == Event::Type::MouseButtonPressed)
                    return m_container->processMousePressEvent(Event::MouseButton::Left, mouseCoords);
                else // if (event.type == Event::Type::MouseButtonReleased)
                    return m_container->processMouseReleaseEvent(Event::MouseButton::Left, mouseCoords);
            }
            case Event::Type::KeyPressed:
            {
                if (isTabKeyUsageEnabled() && (event.key.code == Event::KeyboardKey::Tab))
                {
                    if (event.key.shift)
                        focusPreviousWidget();
                    else
                        focusNextWidget();

                    return true;
                }
                else
                    return m_container->processKeyPressEvent(event.key);
            }
            case Event::Type::TextEntered:
            {
                return m_container->processTextEnteredEvent(event.text.unicode);
            }
            case Event::Type::LostFocus:
            {
                m_windowFocused = false;
                m_lastUpdateTime = decltype(m_lastUpdateTime){};
                break;
            }
            case Event::Type::GainedFocus:
            {
                m_windowFocused = true;
                break;
            }
            case Event::Type::Resized:
            {
                if (!m_customViewSet && m_target)
                {
                    setView(sf::View({0, 0, static_cast<float>(event.size.width), static_cast<float>(event.size.height)}));
                    m_customViewSet = false;
                }
                break;
            }
        }

        // The event wasn't absorbed by the gui
        return false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::setTabKeyUsageEnabled(bool enabled)
    {
        m_TabKeyUsageEnabled = enabled;
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
        {
            const auto timePointNow = std::chrono::steady_clock::now();

            if (m_lastUpdateTime > decltype(m_lastUpdateTime){})
                updateTime(timePointNow - m_lastUpdateTime);

            m_lastUpdateTime = timePointNow;
        }

        // Change the view
        const sf::View oldView = m_target->getView();
        m_target->setView(m_view);
        Clipping::setGuiView(m_view);

        // Draw the widgets
        m_container->draw(*m_target, sf::RenderStates::Default);

        // Restore the old view
        m_target->setView(oldView);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    RootContainer::Ptr Gui::getContainer() const
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
       if (m_container->getInheritedFont())
          return m_container->getInheritedFont();
        else
            return getGlobalFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::vector<Widget::Ptr>& Gui::getWidgets() const
    {
        return m_container->getWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::add(const Widget::Ptr& widgetPtr, const String& widgetName)
    {
        m_container->add(widgetPtr, widgetName);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Widget::Ptr Gui::get(const String& widgetName) const
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

    bool Gui::focusNextWidget()
    {
        return m_container->focusNextWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Gui::focusPreviousWidget()
    {
        return m_container->focusPreviousWidget();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::unfocusAllWidgets()
    {
        m_container->setFocused(false);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::moveWidgetToFront(const Widget::Ptr& widget)
    {
        m_container->moveWidgetToFront(widget);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::moveWidgetToBack(const Widget::Ptr& widget)
    {
        m_container->moveWidgetToBack(widget);
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

    void Gui::setTextSize(unsigned int size)
    {
        m_container->setTextSize(size);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Gui::getTextSize() const
    {
        return m_container->getTextSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::loadWidgetsFromFile(const String& filename, bool replaceExisting)
    {
        m_container->loadWidgetsFromFile(filename, replaceExisting);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::saveWidgetsToFile(const String& filename)
    {
        m_container->saveWidgetsToFile(filename);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::loadWidgetsFromStream(std::stringstream& stream, bool replaceExisting)
    {
        m_container->loadWidgetsFromStream(stream, replaceExisting);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::loadWidgetsFromStream(std::stringstream&& stream, bool replaceExisting)
    {
        loadWidgetsFromStream(stream, replaceExisting);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::saveWidgetsToStream(std::stringstream& stream) const
    {
        m_container->saveWidgetsToStream(stream);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Gui::updateTime(Duration elapsedTime)
    {
        m_container->m_animationTimeElapsed = elapsedTime;
        m_container->updateTime(elapsedTime);

        if (m_tooltipPossible)
        {
            m_tooltipTime += elapsedTime;
            if (m_tooltipTime >= ToolTip::getInitialDelay())
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
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
