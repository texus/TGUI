/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <TGUI/Config.hpp>

#if TGUI_HAS_WINDOW_BACKEND_SFML
    #include <SFML/Window.hpp>
#endif
#if TGUI_HAS_WINDOW_BACKEND_SDL
    #include <TGUI/extlibs/IncludeSDL.hpp>
    #if SDL_MAJOR_VERSION < 3
        #include <SDL_touch.h>
    #endif
#endif
#if TGUI_HAS_WINDOW_BACKEND_GLFW
    #define GLFW_INCLUDE_NONE
    #include <GLFW/glfw3.h>
#endif
#if TGUI_HAS_WINDOW_BACKEND_RAYLIB
    #include <raylib.h>
#endif

#include "Tests.hpp"

#if TGUI_BUILD_AS_CXX_MODULE
    #if TGUI_HAS_WINDOW_BACKEND_SFML
        import tgui.backend.window.sfml;
    #endif
    #if TGUI_HAS_WINDOW_BACKEND_SDL
        import tgui.backend.window.sdl;
    #endif
    #if TGUI_HAS_WINDOW_BACKEND_GLFW
        import tgui.backend.window.glfw;
    #endif
    #if TGUI_HAS_WINDOW_BACKEND_RAYLIB
        import tgui.backend.window.raylib;
    #endif
#else
    #if TGUI_HAS_WINDOW_BACKEND_SFML
        #include <TGUI/Backend/Window/SFML/BackendGuiSFML.hpp>
    #endif
    #if TGUI_HAS_WINDOW_BACKEND_SDL
        #include <TGUI/Backend/Window/SDL/BackendGuiSDL.hpp>
    #endif
    #if TGUI_HAS_WINDOW_BACKEND_GLFW
        #include <TGUI/Backend/Window/GLFW/BackendGuiGLFW.hpp>
    #endif
    #if TGUI_HAS_WINDOW_BACKEND_RAYLIB
        #include <TGUI/Backend/raylib.hpp>
    #endif
#endif

TEST_CASE("[Backend events]")
{
#if TGUI_HAS_WINDOW_BACKEND_SFML
    auto backendGuiSFML = dynamic_cast<tgui::BackendGuiSFML*>(globalGui);
    if (backendGuiSFML)
    {
        SECTION("SFML")
        {
            SECTION("KeyPressed")
            {
#if SFML_VERSION_MAJOR >= 3
                sf::Event::KeyPressed eventKeyPressed;
                eventKeyPressed.alt = false;
                eventKeyPressed.control = false;
                eventKeyPressed.shift = false;
                eventKeyPressed.system = false;
    #if 0
                eventKeyPressed.numLock = true;
    #endif

#else
                sf::Event eventSFML;
                eventSFML.type = sf::Event::KeyPressed;
                eventSFML.key.alt = false;
                eventSFML.key.control = false;
                eventSFML.key.shift = false;
                eventSFML.key.system = false;
#endif
                SECTION("All key codes")
                {
                    std::array<std::pair<sf::Keyboard::Key, tgui::Event::KeyboardKey>, 100> keys = {{
                        {sf::Keyboard::Key::A,          tgui::Event::KeyboardKey::A},
                        {sf::Keyboard::Key::B,          tgui::Event::KeyboardKey::B},
                        {sf::Keyboard::Key::C,          tgui::Event::KeyboardKey::C},
                        {sf::Keyboard::Key::D,          tgui::Event::KeyboardKey::D},
                        {sf::Keyboard::Key::E,          tgui::Event::KeyboardKey::E},
                        {sf::Keyboard::Key::F,          tgui::Event::KeyboardKey::F},
                        {sf::Keyboard::Key::G,          tgui::Event::KeyboardKey::G},
                        {sf::Keyboard::Key::H,          tgui::Event::KeyboardKey::H},
                        {sf::Keyboard::Key::I,          tgui::Event::KeyboardKey::I},
                        {sf::Keyboard::Key::J,          tgui::Event::KeyboardKey::J},
                        {sf::Keyboard::Key::K,          tgui::Event::KeyboardKey::K},
                        {sf::Keyboard::Key::L,          tgui::Event::KeyboardKey::L},
                        {sf::Keyboard::Key::M,          tgui::Event::KeyboardKey::M},
                        {sf::Keyboard::Key::N,          tgui::Event::KeyboardKey::N},
                        {sf::Keyboard::Key::O,          tgui::Event::KeyboardKey::O},
                        {sf::Keyboard::Key::P,          tgui::Event::KeyboardKey::P},
                        {sf::Keyboard::Key::Q,          tgui::Event::KeyboardKey::Q},
                        {sf::Keyboard::Key::R,          tgui::Event::KeyboardKey::R},
                        {sf::Keyboard::Key::S,          tgui::Event::KeyboardKey::S},
                        {sf::Keyboard::Key::T,          tgui::Event::KeyboardKey::T},
                        {sf::Keyboard::Key::U,          tgui::Event::KeyboardKey::U},
                        {sf::Keyboard::Key::V,          tgui::Event::KeyboardKey::V},
                        {sf::Keyboard::Key::W,          tgui::Event::KeyboardKey::W},
                        {sf::Keyboard::Key::X,          tgui::Event::KeyboardKey::X},
                        {sf::Keyboard::Key::Y,          tgui::Event::KeyboardKey::Y},
                        {sf::Keyboard::Key::Z,          tgui::Event::KeyboardKey::Z},
                        {sf::Keyboard::Key::Num0,       tgui::Event::KeyboardKey::Num0},
                        {sf::Keyboard::Key::Num1,       tgui::Event::KeyboardKey::Num1},
                        {sf::Keyboard::Key::Num2,       tgui::Event::KeyboardKey::Num2},
                        {sf::Keyboard::Key::Num3,       tgui::Event::KeyboardKey::Num3},
                        {sf::Keyboard::Key::Num4,       tgui::Event::KeyboardKey::Num4},
                        {sf::Keyboard::Key::Num5,       tgui::Event::KeyboardKey::Num5},
                        {sf::Keyboard::Key::Num6,       tgui::Event::KeyboardKey::Num6},
                        {sf::Keyboard::Key::Num7,       tgui::Event::KeyboardKey::Num7},
                        {sf::Keyboard::Key::Num8,       tgui::Event::KeyboardKey::Num8},
                        {sf::Keyboard::Key::Num9,       tgui::Event::KeyboardKey::Num9},
                        {sf::Keyboard::Key::Escape,     tgui::Event::KeyboardKey::Escape},
                        {sf::Keyboard::Key::LControl,   tgui::Event::KeyboardKey::LControl},
                        {sf::Keyboard::Key::LShift,     tgui::Event::KeyboardKey::LShift},
                        {sf::Keyboard::Key::LAlt,       tgui::Event::KeyboardKey::LAlt},
                        {sf::Keyboard::Key::LSystem,    tgui::Event::KeyboardKey::LSystem},
                        {sf::Keyboard::Key::RControl,   tgui::Event::KeyboardKey::RControl},
                        {sf::Keyboard::Key::RShift,     tgui::Event::KeyboardKey::RShift},
                        {sf::Keyboard::Key::RAlt,       tgui::Event::KeyboardKey::RAlt},
                        {sf::Keyboard::Key::RSystem,    tgui::Event::KeyboardKey::RSystem},
                        {sf::Keyboard::Key::Menu,       tgui::Event::KeyboardKey::Menu},
                        {sf::Keyboard::Key::LBracket,   tgui::Event::KeyboardKey::LBracket},
                        {sf::Keyboard::Key::RBracket,   tgui::Event::KeyboardKey::RBracket},
                        {sf::Keyboard::Key::Semicolon,  tgui::Event::KeyboardKey::Semicolon},
                        {sf::Keyboard::Key::Comma,      tgui::Event::KeyboardKey::Comma},
                        {sf::Keyboard::Key::Period,     tgui::Event::KeyboardKey::Period},
#if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR < 6
                        {sf::Keyboard::Key::Quote,      tgui::Event::KeyboardKey::Quote},
#else
                        {sf::Keyboard::Key::Apostrophe, tgui::Event::KeyboardKey::Quote},
#endif
                        {sf::Keyboard::Key::Slash,      tgui::Event::KeyboardKey::Slash},
                        {sf::Keyboard::Key::Backslash,  tgui::Event::KeyboardKey::Backslash},
                        {sf::Keyboard::Key::Equal,      tgui::Event::KeyboardKey::Equal},
                        {sf::Keyboard::Key::Hyphen,     tgui::Event::KeyboardKey::Minus},
                        {sf::Keyboard::Key::Space,      tgui::Event::KeyboardKey::Space},
                        {sf::Keyboard::Key::Enter,      tgui::Event::KeyboardKey::Enter},
                        {sf::Keyboard::Key::Backspace,  tgui::Event::KeyboardKey::Backspace},
                        {sf::Keyboard::Key::Tab,        tgui::Event::KeyboardKey::Tab},
                        {sf::Keyboard::Key::PageUp,     tgui::Event::KeyboardKey::PageUp},
                        {sf::Keyboard::Key::PageDown,   tgui::Event::KeyboardKey::PageDown},
                        {sf::Keyboard::Key::End,        tgui::Event::KeyboardKey::End},
                        {sf::Keyboard::Key::Home,       tgui::Event::KeyboardKey::Home},
                        {sf::Keyboard::Key::Insert,     tgui::Event::KeyboardKey::Insert},
                        {sf::Keyboard::Key::Delete,     tgui::Event::KeyboardKey::Delete},
                        {sf::Keyboard::Key::Add,        tgui::Event::KeyboardKey::Add},
                        {sf::Keyboard::Key::Subtract,   tgui::Event::KeyboardKey::Subtract},
                        {sf::Keyboard::Key::Multiply,   tgui::Event::KeyboardKey::Multiply},
                        {sf::Keyboard::Key::Divide,     tgui::Event::KeyboardKey::Divide},
                        {sf::Keyboard::Key::Left,       tgui::Event::KeyboardKey::Left},
                        {sf::Keyboard::Key::Right,      tgui::Event::KeyboardKey::Right},
                        {sf::Keyboard::Key::Up,         tgui::Event::KeyboardKey::Up},
                        {sf::Keyboard::Key::Down,       tgui::Event::KeyboardKey::Down},
                        {sf::Keyboard::Key::Numpad0,    tgui::Event::KeyboardKey::Numpad0},
                        {sf::Keyboard::Key::Numpad1,    tgui::Event::KeyboardKey::Numpad1},
                        {sf::Keyboard::Key::Numpad2,    tgui::Event::KeyboardKey::Numpad2},
                        {sf::Keyboard::Key::Numpad3,    tgui::Event::KeyboardKey::Numpad3},
                        {sf::Keyboard::Key::Numpad4,    tgui::Event::KeyboardKey::Numpad4},
                        {sf::Keyboard::Key::Numpad5,    tgui::Event::KeyboardKey::Numpad5},
                        {sf::Keyboard::Key::Numpad6,    tgui::Event::KeyboardKey::Numpad6},
                        {sf::Keyboard::Key::Numpad7,    tgui::Event::KeyboardKey::Numpad7},
                        {sf::Keyboard::Key::Numpad8,    tgui::Event::KeyboardKey::Numpad8},
                        {sf::Keyboard::Key::Numpad9,    tgui::Event::KeyboardKey::Numpad9},
                        {sf::Keyboard::Key::F1,         tgui::Event::KeyboardKey::F1},
                        {sf::Keyboard::Key::F2,         tgui::Event::KeyboardKey::F2},
                        {sf::Keyboard::Key::F3,         tgui::Event::KeyboardKey::F3},
                        {sf::Keyboard::Key::F4,         tgui::Event::KeyboardKey::F4},
                        {sf::Keyboard::Key::F5,         tgui::Event::KeyboardKey::F5},
                        {sf::Keyboard::Key::F6,         tgui::Event::KeyboardKey::F6},
                        {sf::Keyboard::Key::F7,         tgui::Event::KeyboardKey::F7},
                        {sf::Keyboard::Key::F8,         tgui::Event::KeyboardKey::F8},
                        {sf::Keyboard::Key::F9,         tgui::Event::KeyboardKey::F9},
                        {sf::Keyboard::Key::F10,        tgui::Event::KeyboardKey::F10},
                        {sf::Keyboard::Key::F11,        tgui::Event::KeyboardKey::F11},
                        {sf::Keyboard::Key::F12,        tgui::Event::KeyboardKey::F12},
                        {sf::Keyboard::Key::F13,        tgui::Event::KeyboardKey::F13},
                        {sf::Keyboard::Key::F14,        tgui::Event::KeyboardKey::F14},
                        {sf::Keyboard::Key::F15,        tgui::Event::KeyboardKey::F15},
                        {sf::Keyboard::Key::Pause,      tgui::Event::KeyboardKey::Pause}
                    }};
                    for (auto pair : keys)
                    {
#if SFML_VERSION_MAJOR >= 3
                        eventKeyPressed.code = pair.first;
                        sf::Event eventSFML(eventKeyPressed);
#else
                        eventSFML.key.code = pair.first;
#endif
                        tgui::Event eventTGUI;
                        REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                        REQUIRE(eventTGUI.key.code == pair.second);
                    }
                }

#if SFML_VERSION_MAJOR >= 3
                SECTION("Invalid key code")
                {
                    eventKeyPressed.code = sf::Keyboard::Key::Unknown;
                    sf::Event eventSFML(eventKeyPressed);

                    tgui::Event eventTGUI;
                    REQUIRE(!backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                }

                SECTION("Modifiers")
                {
                    eventKeyPressed.code = sf::Keyboard::Key::Space;
                    eventKeyPressed.alt = false;
                    eventKeyPressed.control = true;
                    eventKeyPressed.shift = true;
                    eventKeyPressed.system = false;
                    sf::Event eventSFML(eventKeyPressed);

                    tgui::Event eventTGUI;
                    REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                    REQUIRE(eventTGUI.type == tgui::Event::Type::KeyPressed);
                    REQUIRE(eventTGUI.key.code == tgui::Event::KeyboardKey::Space);
                    REQUIRE(!eventTGUI.key.alt);
                    REQUIRE(eventTGUI.key.control);
                    REQUIRE(eventTGUI.key.shift);
                    REQUIRE(!eventTGUI.key.system);
                }

                SECTION("Key release")
                {
                    sf::Event::KeyReleased eventKeyReleased;
                    eventKeyReleased.code = sf::Keyboard::Key::Space;
                    sf::Event eventSFML(eventKeyReleased);

                    tgui::Event eventTGUI;
                    REQUIRE(!backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                }

    #if 0 // When enabling this, don't forget the eventKeyPressed.numLock line earlier in this file
                SECTION("Numpad keys with NumLock off")
                {
                    tgui::Event eventTGUI;
                    eventKeyPressed.numLock = true;
                    eventKeyPressed.code = sf::Keyboard::Key::Numpad0;
                    REQUIRE((backendGuiSFML->convertEvent(eventKeyPressed, eventTGUI) && eventTGUI.key.code == tgui::Event::KeyboardKey::Numpad0));

                    eventKeyPressed.numLock = false;
                    eventKeyPressed.code = sf::Keyboard::Key::Numpad0;
                    REQUIRE((backendGuiSFML->convertEvent(eventKeyPressed, eventTGUI) && eventTGUI.key.code == tgui::Event::KeyboardKey::Insert));
                    eventKeyPressed.code = sf::Keyboard::Key::Numpad1;
                    REQUIRE((backendGuiSFML->convertEvent(eventKeyPressed, eventTGUI) && eventTGUI.key.code == tgui::Event::KeyboardKey::End));
                    eventKeyPressed.code = sf::Keyboard::Key::Numpad2;
                    REQUIRE((backendGuiSFML->convertEvent(eventKeyPressed, eventTGUI) && eventTGUI.key.code == tgui::Event::KeyboardKey::Down));
                    eventKeyPressed.code = sf::Keyboard::Key::Numpad3;
                    REQUIRE((backendGuiSFML->convertEvent(eventKeyPressed, eventTGUI) && eventTGUI.key.code == tgui::Event::KeyboardKey::PageDown));
                    eventKeyPressed.code = sf::Keyboard::Key::Numpad4;
                    REQUIRE((backendGuiSFML->convertEvent(eventKeyPressed, eventTGUI) && eventTGUI.key.code == tgui::Event::KeyboardKey::Left));
                    eventKeyPressed.code = sf::Keyboard::Key::Numpad5;
                    REQUIRE(!backendGuiSFML->convertEvent(eventKeyPressed, eventTGUI));
                    eventKeyPressed.code = sf::Keyboard::Key::Numpad6;
                    REQUIRE((backendGuiSFML->convertEvent(eventKeyPressed, eventTGUI) && eventTGUI.key.code == tgui::Event::KeyboardKey::Right));
                    eventKeyPressed.code = sf::Keyboard::Key::Numpad7;
                    REQUIRE((backendGuiSFML->convertEvent(eventKeyPressed, eventTGUI) && eventTGUI.key.code == tgui::Event::KeyboardKey::Home));
                    eventKeyPressed.code = sf::Keyboard::Key::Numpad8;
                    REQUIRE((backendGuiSFML->convertEvent(eventKeyPressed, eventTGUI) && eventTGUI.key.code == tgui::Event::KeyboardKey::Up));
                    eventKeyPressed.code = sf::Keyboard::Key::Numpad9;
                    REQUIRE((backendGuiSFML->convertEvent(eventKeyPressed, eventTGUI) && eventTGUI.key.code == tgui::Event::KeyboardKey::PageUp));
                }
    #endif
#else
                SECTION("Invalid key code")
                {
                    eventSFML.key.code = sf::Keyboard::Key::Unknown;

                    tgui::Event eventTGUI;
                    REQUIRE(!backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                }

                SECTION("Modifiers")
                {
                    eventSFML.key.code = sf::Keyboard::Key::Space;
                    eventSFML.key.alt = false;
                    eventSFML.key.control = true;
                    eventSFML.key.shift = true;
                    eventSFML.key.system = false;

                    tgui::Event eventTGUI;
                    REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                    REQUIRE(eventTGUI.type == tgui::Event::Type::KeyPressed);
                    REQUIRE(eventTGUI.key.code == tgui::Event::KeyboardKey::Space);
                    REQUIRE(!eventTGUI.key.alt);
                    REQUIRE(eventTGUI.key.control);
                    REQUIRE(eventTGUI.key.shift);
                    REQUIRE(!eventTGUI.key.system);
                }

                SECTION("Key release")
                {
                    eventSFML.type = sf::Event::KeyReleased;
                    eventSFML.key.code = sf::Keyboard::Key::Space;

                    tgui::Event eventTGUI;
                    REQUIRE(!backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                }
#endif
            }

#if SFML_VERSION_MAJOR >= 3
            SECTION("GainedFocus")
            {
                sf::Event::FocusGained eventFocusGained;
                sf::Event eventSFML{eventFocusGained};

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::GainedFocus);
            }

            SECTION("LostFocus")
            {
                sf::Event::FocusLost eventFocusLost;
                sf::Event eventSFML{eventFocusLost};

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::LostFocus);
            }

            SECTION("Closed")
            {
                sf::Event::Closed eventClose;
                sf::Event eventSFML{eventClose};

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::Closed);
            }

            SECTION("Resized")
            {
                sf::Event::Resized eventResized;
                eventResized.size.x = 400;
                eventResized.size.y = 300;
                sf::Event eventSFML{eventResized};

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::Resized);
                REQUIRE(eventTGUI.size.width == 400);
                REQUIRE(eventTGUI.size.height == 300);
            }

            SECTION("TextEntered")
            {
                sf::Event::TextEntered eventTextEntered;
                eventTextEntered.unicode = 0x2705;
                sf::Event eventSFML{eventTextEntered};

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::TextEntered);
                REQUIRE(eventTGUI.text.unicode == 0x2705);
            }

            SECTION("MouseWheelScrolled")
            {
                sf::Event::MouseWheelScrolled eventMouseWheel;
                eventMouseWheel.wheel = sf::Mouse::Wheel::Vertical;
                eventMouseWheel.delta = 2;
                eventMouseWheel.position.x = 200;
                eventMouseWheel.position.y = 150;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent({eventMouseWheel}, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseWheelScrolled);
                REQUIRE(eventTGUI.mouseWheel.delta == 2);
                REQUIRE(eventTGUI.mouseWheel.x == 200);
                REQUIRE(eventTGUI.mouseWheel.y == 150);

                // We only handle vertical scrolling
                eventMouseWheel.wheel = sf::Mouse::Wheel::Horizontal;
                REQUIRE(!backendGuiSFML->convertEvent({eventMouseWheel}, eventTGUI));
            }

            SECTION("MouseButtonPressed")
            {
                sf::Event::MouseButtonPressed mousePressed;
                mousePressed.button = sf::Mouse::Button::Left;
                mousePressed.position.x = 200;
                mousePressed.position.y = 150;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent({mousePressed}, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseButtonPressed);
                REQUIRE(eventTGUI.mouseButton.button == tgui::Event::MouseButton::Left);
                REQUIRE(eventTGUI.mouseButton.x == 200);
                REQUIRE(eventTGUI.mouseButton.y == 150);

                mousePressed.button = sf::Mouse::Button::Right;
                REQUIRE(backendGuiSFML->convertEvent({mousePressed}, eventTGUI));
                REQUIRE(eventTGUI.mouseButton.button == tgui::Event::MouseButton::Right);

                mousePressed.button = sf::Mouse::Button::Middle;
                REQUIRE(backendGuiSFML->convertEvent({mousePressed}, eventTGUI));
                REQUIRE(eventTGUI.mouseButton.button == tgui::Event::MouseButton::Middle);

                // Only left, middle and right mouse buttons are handled
                mousePressed.button = sf::Mouse::Button::Extra1;
                REQUIRE(!backendGuiSFML->convertEvent({mousePressed}, eventTGUI));
            }

            SECTION("MouseButtonReleased")
            {
                sf::Event::MouseButtonReleased mouseReleased;
                mouseReleased.button = sf::Mouse::Button::Left;
                mouseReleased.position.x = 200;
                mouseReleased.position.y = 150;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent({mouseReleased}, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseButtonReleased);
                REQUIRE(eventTGUI.mouseButton.button == tgui::Event::MouseButton::Left);
                REQUIRE(eventTGUI.mouseButton.x == 200);
                REQUIRE(eventTGUI.mouseButton.y == 150);

                mouseReleased.button = sf::Mouse::Button::Right;
                REQUIRE(backendGuiSFML->convertEvent({mouseReleased}, eventTGUI));
                REQUIRE(eventTGUI.mouseButton.button == tgui::Event::MouseButton::Right);

                mouseReleased.button = sf::Mouse::Button::Middle;
                REQUIRE(backendGuiSFML->convertEvent({mouseReleased}, eventTGUI));
                REQUIRE(eventTGUI.mouseButton.button == tgui::Event::MouseButton::Middle);

                // Only left, middle and right mouse buttons are handled
                mouseReleased.button = sf::Mouse::Button::Extra1;
                REQUIRE(!backendGuiSFML->convertEvent({mouseReleased}, eventTGUI));
            }

            SECTION("MouseMoved")
            {
                sf::Event::MouseMoved eventMouseMoved;
                eventMouseMoved.position.x = 200;
                eventMouseMoved.position.y = 150;
                sf::Event eventSFML{eventMouseMoved};

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseMoved);
                REQUIRE(eventTGUI.mouseMove.x == 200);
                REQUIRE(eventTGUI.mouseMove.y == 150);
            }

            SECTION("Touch")
            {
                sf::Event::TouchBegan eventTouchBegan;
                eventTouchBegan.finger = 0;
                eventTouchBegan.position.x = 200;
                eventTouchBegan.position.y = 150;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent({eventTouchBegan}, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseButtonPressed);
                REQUIRE(eventTGUI.mouseButton.x == 200);
                REQUIRE(eventTGUI.mouseButton.y == 150);

                sf::Event::TouchMoved eventTouchMoved;
                eventTouchMoved.finger = 0;
                eventTouchMoved.position.x = 210;
                eventTouchMoved.position.y = 155;
                REQUIRE(backendGuiSFML->convertEvent({eventTouchMoved}, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseMoved);
                REQUIRE(eventTGUI.mouseMove.x == 210);
                REQUIRE(eventTGUI.mouseMove.y == 155);

                sf::Event::TouchEnded eventTouchEnded;
                eventTouchEnded.finger = 0;
                eventTouchEnded.position.x = 220;
                eventTouchEnded.position.y = 160;
                REQUIRE(backendGuiSFML->convertEvent({eventTouchEnded}, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseButtonReleased);
                REQUIRE(eventTGUI.mouseButton.x == 220);
                REQUIRE(eventTGUI.mouseButton.y == 160);
            }

            SECTION("MouseEntered")
            {
                sf::Event::MouseEntered eventMouseEntered;
                sf::Event eventSFML{eventMouseEntered};

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseEntered);
            }

            SECTION("MouseLeft")
            {
                sf::Event::MouseLeft eventMouseLeft;
                sf::Event eventSFML{eventMouseLeft};

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseLeft);
            }

            SECTION("Handling events")
            {
                auto childWindow = tgui::ChildWindow::create();
                childWindow->setResizable(true);
                childWindow->setPosition({20, 10});
                childWindow->setSize({300, 250});
                childWindow->getRenderer()->setTitleBarHeight(20);
                childWindow->getRenderer()->setBorders({1});
                globalGui->add(childWindow);

                auto slider = tgui::Slider::create();
                slider->setChangeValueOnScroll(true);
                slider->setPosition({230, 20});
                slider->setSize({20, 150});
                slider->setMaximum(10);
                slider->setValue(3);
                childWindow->add(slider);

                auto editBox = tgui::EditBox::create();
                editBox->setPosition({10, 10});
                editBox->setSize({150, 24});
                childWindow->add(editBox);

                editBox->setFocused(true);

                // Type 3 characters in the edit box
                sf::Event::TextEntered eventTextEntered;
                eventTextEntered.unicode = 'A';
                backendGuiSFML->handleEvent({eventTextEntered});
                eventTextEntered.unicode = 'B';
                backendGuiSFML->handleEvent({eventTextEntered});
                eventTextEntered.unicode = 'C';
                backendGuiSFML->handleEvent({eventTextEntered});

                // Erase the second character from the edit box
                sf::Event::KeyPressed eventKeyPressed;
                eventKeyPressed.alt = false;
                eventKeyPressed.control = false;
                eventKeyPressed.shift = false;
                eventKeyPressed.system = false;
                eventKeyPressed.code = sf::Keyboard::Key::Left;
                backendGuiSFML->handleEvent({eventKeyPressed});
                eventKeyPressed.code = sf::Keyboard::Key::Backspace;
                backendGuiSFML->handleEvent({eventKeyPressed});

                // Verify that the events were correctly processed by the edit box
                REQUIRE(editBox->getText() == "AC");

                // Scroll the mouse wheel on top of the slider and verify that its value changes
                sf::Event::MouseWheelScrolled eventMouseWheelScrolled;
                eventMouseWheelScrolled.wheel = sf::Mouse::Wheel::Vertical;
                eventMouseWheelScrolled.delta = 4;
                eventMouseWheelScrolled.position.x = 260;
                eventMouseWheelScrolled.position.y = 80;
                backendGuiSFML->handleEvent({eventMouseWheelScrolled});
                eventMouseWheelScrolled.delta = -1;
                backendGuiSFML->handleEvent({eventMouseWheelScrolled});
                backendGuiSFML->handleEvent({eventMouseWheelScrolled});

                REQUIRE(slider->getValue() == 5);

                // The mouse leaving the window will remove the hover state of widgets
                unsigned int mouseLeftCount = 0;
                sf::Event::MouseMoved eventMouseMoved;
                eventMouseMoved.position.x = 260;
                eventMouseMoved.position.y = 80;
                backendGuiSFML->handleEvent({eventMouseMoved});
                slider->onMouseLeave([&]{ genericCallback(mouseLeftCount); });
                sf::Event::MouseLeft eventMouseLeft;
                backendGuiSFML->handleEvent({eventMouseLeft});
                REQUIRE(mouseLeftCount == 1);

                // Resize the child window using mouse events (decrease width with 20px)
                // Note that the resizing ignores the position of the mouse released event
                sf::Event::MouseButtonPressed eventMousePressed;
                eventMousePressed.button = sf::Mouse::Button::Left;
                eventMousePressed.position.x = 320;
                eventMousePressed.position.y = 100;
                backendGuiSFML->handleEvent({eventMousePressed});
                eventMouseMoved.position.x = 300;
                eventMouseMoved.position.y = 105;
                backendGuiSFML->handleEvent({eventMouseMoved});
                sf::Event::MouseButtonReleased eventMouseReleased;
                eventMouseReleased.button = sf::Mouse::Button::Left;
                eventMouseReleased.position.x = 290;
                eventMouseReleased.position.y = 110;
                backendGuiSFML->handleEvent({eventMouseReleased});

                // Resize the child window using touch events  (decrease height with 10px)
                // Note that the resizing ignores the position of the touch ended event
                sf::Event::TouchBegan eventTouchBegan;
                eventTouchBegan.finger = 0;
                eventTouchBegan.position.x = 100;
                eventTouchBegan.position.y = 260;
                backendGuiSFML->handleEvent({eventTouchBegan});
                sf::Event::TouchMoved eventTouchMoved;
                eventTouchMoved.finger = 0;
                eventTouchMoved.position.x = 110;
                eventTouchMoved.position.y = 270;
                backendGuiSFML->handleEvent({eventTouchMoved});
                sf::Event::TouchEnded eventTouchEnded;
                eventTouchEnded.finger = 0;
                eventTouchEnded.position.x = 105;
                eventTouchEnded.position.y = 275;
                backendGuiSFML->handleEvent({eventTouchEnded});

                REQUIRE(childWindow->getSize() == tgui::Vector2f(280, 260));

                globalGui->removeAllWidgets();
            }
#else
            SECTION("GainedFocus")
            {
                sf::Event eventSFML;
                eventSFML.type = sf::Event::GainedFocus;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::GainedFocus);
            }

            SECTION("LostFocus")
            {
                sf::Event eventSFML;
                eventSFML.type = sf::Event::LostFocus;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::LostFocus);
            }

            SECTION("Closed")
            {
                sf::Event eventSFML;
                eventSFML.type = sf::Event::Closed;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::Closed);
            }

            SECTION("Resized")
            {
                sf::Event eventSFML;
                eventSFML.type = sf::Event::Resized;
                eventSFML.size.width = 400;
                eventSFML.size.height = 300;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::Resized);
                REQUIRE(eventTGUI.size.width == 400);
                REQUIRE(eventTGUI.size.height == 300);
            }

            SECTION("TextEntered")
            {
                sf::Event eventSFML;
                eventSFML.type = sf::Event::TextEntered;
                eventSFML.text.unicode = 0x2705;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::TextEntered);
                REQUIRE(eventTGUI.text.unicode == 0x2705);
            }

            SECTION("MouseWheelScrolled")
            {
                sf::Event eventSFML;
                eventSFML.type = sf::Event::MouseWheelScrolled;
                eventSFML.mouseWheelScroll.wheel = sf::Mouse::Wheel::VerticalWheel;
                eventSFML.mouseWheelScroll.delta = 2;
                eventSFML.mouseWheelScroll.x = 200;
                eventSFML.mouseWheelScroll.y = 150;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseWheelScrolled);
                REQUIRE(eventTGUI.mouseWheel.delta == 2);
                REQUIRE(eventTGUI.mouseWheel.x == 200);
                REQUIRE(eventTGUI.mouseWheel.y == 150);

                // We only handle vertical scrolling
                eventSFML.mouseWheelScroll.wheel = sf::Mouse::Wheel::HorizontalWheel;
                REQUIRE(!backendGuiSFML->convertEvent(eventSFML, eventTGUI));
            }

            SECTION("MouseButtonPressed")
            {
                sf::Event eventSFML;
                eventSFML.type = sf::Event::MouseButtonPressed;
                eventSFML.mouseButton.button = sf::Mouse::Button::Left;
                eventSFML.mouseButton.x = 200;
                eventSFML.mouseButton.y = 150;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseButtonPressed);
                REQUIRE(eventTGUI.mouseButton.button == tgui::Event::MouseButton::Left);
                REQUIRE(eventTGUI.mouseButton.x == 200);
                REQUIRE(eventTGUI.mouseButton.y == 150);

                eventSFML.mouseButton.button = sf::Mouse::Button::Right;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.mouseButton.button == tgui::Event::MouseButton::Right);

                eventSFML.mouseButton.button = sf::Mouse::Button::Middle;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.mouseButton.button == tgui::Event::MouseButton::Middle);

                // Only left, middle and right mouse buttons are handled
                eventSFML.mouseButton.button = sf::Mouse::Button::XButton1;
                REQUIRE(!backendGuiSFML->convertEvent(eventSFML, eventTGUI));
            }

            SECTION("MouseButtonReleased")
            {
                sf::Event eventSFML;
                eventSFML.type = sf::Event::MouseButtonReleased;
                eventSFML.mouseButton.button = sf::Mouse::Button::Left;
                eventSFML.mouseButton.x = 200;
                eventSFML.mouseButton.y = 150;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseButtonReleased);
                REQUIRE(eventTGUI.mouseButton.button == tgui::Event::MouseButton::Left);
                REQUIRE(eventTGUI.mouseButton.x == 200);
                REQUIRE(eventTGUI.mouseButton.y == 150);

                eventSFML.mouseButton.button = sf::Mouse::Button::Right;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.mouseButton.button == tgui::Event::MouseButton::Right);

                eventSFML.mouseButton.button = sf::Mouse::Button::Middle;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.mouseButton.button == tgui::Event::MouseButton::Middle);

                // Only left, middle and right mouse buttons are handled
                eventSFML.mouseButton.button = sf::Mouse::Button::XButton1;
                REQUIRE(!backendGuiSFML->convertEvent(eventSFML, eventTGUI));
            }

            SECTION("MouseMoved")
            {
                sf::Event eventSFML;
                eventSFML.type = sf::Event::MouseMoved;
                eventSFML.mouseMove.x = 200;
                eventSFML.mouseMove.y = 150;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseMoved);
                REQUIRE(eventTGUI.mouseMove.x == 200);
                REQUIRE(eventTGUI.mouseMove.y == 150);
            }

            SECTION("Touch")
            {
                sf::Event eventSFML;
                eventSFML.type = sf::Event::TouchBegan;
                eventSFML.touch.finger = 0;
                eventSFML.touch.x = 200;
                eventSFML.touch.y = 150;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseButtonPressed);
                REQUIRE(eventTGUI.mouseButton.x == 200);
                REQUIRE(eventTGUI.mouseButton.y == 150);

                eventSFML.type = sf::Event::TouchMoved;
                eventSFML.touch.finger = 0;
                eventSFML.touch.x = 210;
                eventSFML.touch.y = 155;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseMoved);
                REQUIRE(eventTGUI.mouseMove.x == 210);
                REQUIRE(eventTGUI.mouseMove.y == 155);

                eventSFML.type = sf::Event::TouchEnded;
                eventSFML.touch.x = 220;
                eventSFML.touch.y = 160;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseButtonReleased);
                REQUIRE(eventTGUI.mouseButton.x == 220);
                REQUIRE(eventTGUI.mouseButton.y == 160);
            }

            SECTION("MouseEntered")
            {
                sf::Event eventSFML;
                eventSFML.type = sf::Event::MouseEntered;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseEntered);
            }

            SECTION("MouseLeft")
            {
                sf::Event eventSFML;
                eventSFML.type = sf::Event::MouseLeft;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSFML->convertEvent(eventSFML, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseLeft);
            }

            SECTION("Handling events")
            {
                auto childWindow = tgui::ChildWindow::create();
                childWindow->setResizable(true);
                childWindow->setPosition({20, 10});
                childWindow->setSize({300, 250});
                childWindow->getRenderer()->setTitleBarHeight(20);
                childWindow->getRenderer()->setBorders({1});
                globalGui->add(childWindow);

                auto slider = tgui::Slider::create();
                slider->setChangeValueOnScroll(true);
                slider->setPosition({230, 20});
                slider->setSize({20, 150});
                slider->setMaximum(10);
                slider->setValue(3);
                childWindow->add(slider);

                auto editBox = tgui::EditBox::create();
                editBox->setPosition({10, 10});
                editBox->setSize({150, 24});
                childWindow->add(editBox);

                editBox->setFocused(true);

                // Type 3 characters in the edit box
                sf::Event eventSFML;
                eventSFML.type = sf::Event::TextEntered;
                eventSFML.text.unicode = 'A';
                backendGuiSFML->handleEvent(eventSFML);
                eventSFML.text.unicode = 'B';
                backendGuiSFML->handleEvent(eventSFML);
                eventSFML.text.unicode = 'C';
                backendGuiSFML->handleEvent(eventSFML);

                // Erase the second character from the edit box
                eventSFML.type = sf::Event::KeyPressed;
                eventSFML.key.alt = false;
                eventSFML.key.control = false;
                eventSFML.key.shift = false;
                eventSFML.key.system = false;
                eventSFML.key.code = sf::Keyboard::Key::Left;
                backendGuiSFML->handleEvent(eventSFML);
                eventSFML.key.code = sf::Keyboard::Key::Backspace;
                backendGuiSFML->handleEvent(eventSFML);

                // Verify that the events were correctly processed by the edit box
                REQUIRE(editBox->getText() == "AC");

                // Scroll the mouse wheel on top of the slider and verify that its value changes
                eventSFML.type = sf::Event::MouseWheelScrolled;
                eventSFML.mouseWheelScroll.wheel = sf::Mouse::Wheel::VerticalWheel;
                eventSFML.mouseWheelScroll.delta = 4;
                eventSFML.mouseWheelScroll.x = 260;
                eventSFML.mouseWheelScroll.y = 80;
                backendGuiSFML->handleEvent(eventSFML);
                eventSFML.mouseWheelScroll.delta = -1;
                backendGuiSFML->handleEvent(eventSFML);
                backendGuiSFML->handleEvent(eventSFML);

                REQUIRE(slider->getValue() == 5);

                // The mouse leaving the window will remove the hover state of widgets
                unsigned int mouseLeftCount = 0;
                eventSFML.type = sf::Event::MouseMoved;
                eventSFML.mouseMove.x = 260;
                eventSFML.mouseMove.y = 80;
                backendGuiSFML->handleEvent(eventSFML);
                slider->onMouseLeave([&]{ genericCallback(mouseLeftCount); });
                eventSFML.type = sf::Event::MouseLeft;
                backendGuiSFML->handleEvent(eventSFML);
                REQUIRE(mouseLeftCount == 1);

                // Resize the child window using mouse events (decrease width with 20px)
                // Note that the resizing ignores the position of the mouse released event
                eventSFML.type = sf::Event::MouseButtonPressed;
                eventSFML.mouseButton.button = sf::Mouse::Button::Left;
                eventSFML.mouseButton.x = 320;
                eventSFML.mouseButton.y = 100;
                backendGuiSFML->handleEvent(eventSFML);
                eventSFML.type = sf::Event::MouseMoved;
                eventSFML.mouseMove.x = 300;
                eventSFML.mouseMove.y = 105;
                backendGuiSFML->handleEvent(eventSFML);
                eventSFML.type = sf::Event::MouseButtonReleased;
                eventSFML.mouseButton.button = sf::Mouse::Button::Left;
                eventSFML.mouseButton.x = 290;
                eventSFML.mouseButton.y = 110;
                backendGuiSFML->handleEvent(eventSFML);

                // Resize the child window using touch events  (decrease height with 10px)
                // Note that the resizing ignores the position of the touch ended event
                eventSFML.type = sf::Event::TouchBegan;
                eventSFML.touch.finger = 0;
                eventSFML.touch.x = 100;
                eventSFML.touch.y = 260;
                backendGuiSFML->handleEvent(eventSFML);
                eventSFML.type = sf::Event::TouchMoved;
                eventSFML.touch.finger = 0;
                eventSFML.touch.x = 110;
                eventSFML.touch.y = 270;
                backendGuiSFML->handleEvent(eventSFML);
                eventSFML.type = sf::Event::TouchEnded;
                eventSFML.touch.x = 105;
                eventSFML.touch.y = 275;
                backendGuiSFML->handleEvent(eventSFML);

                REQUIRE(childWindow->getSize() == tgui::Vector2f(280, 260));

                globalGui->removeAllWidgets();
            }
#endif // SFML_VERSION_MAJOR
        }
    }
#endif // TGUI_HAS_WINDOW_BACKEND_SFML

#if TGUI_HAS_WINDOW_BACKEND_SDL
    auto backendGuiSDL = dynamic_cast<tgui::BackendGuiSDL*>(globalGui);
    if (backendGuiSDL)
    {
        SECTION("SDL")
        {
            SECTION("KeyPressed")
            {
                SDL_Event eventSDL;
                eventSDL.type = SDL_EVENT_KEY_DOWN;
                eventSDL.key.windowID = 0;
                eventSDL.key.repeat = 0;
#if SDL_MAJOR_VERSION >= 3
                eventSDL.key.down = true;
                eventSDL.key.scancode = SDL_SCANCODE_UNKNOWN;
                auto& eventSdlKey = eventSDL.key.key;
                auto& eventSdlMod = eventSDL.key.mod;
#else
                eventSDL.key.state = SDL_PRESSED;
                eventSDL.key.keysym.scancode = SDL_SCANCODE_UNKNOWN;
                auto& eventSdlKey = eventSDL.key.keysym.sym;
                auto& eventSdlMod = eventSDL.key.keysym.mod;
#endif
                eventSdlKey = SDLK_UNKNOWN;
                eventSdlMod = SDL_KMOD_NUM;

                SECTION("All key codes")
                {
#if SDL_MAJOR_VERSION >= 3
                    std::array<std::pair<uint32_t, tgui::Event::KeyboardKey>, 101> keys = {{
                        {SDLK_A,            tgui::Event::KeyboardKey::A},
                        {SDLK_B,            tgui::Event::KeyboardKey::B},
                        {SDLK_C,            tgui::Event::KeyboardKey::C},
                        {SDLK_D,            tgui::Event::KeyboardKey::D},
                        {SDLK_E,            tgui::Event::KeyboardKey::E},
                        {SDLK_F,            tgui::Event::KeyboardKey::F},
                        {SDLK_G,            tgui::Event::KeyboardKey::G},
                        {SDLK_H,            tgui::Event::KeyboardKey::H},
                        {SDLK_I,            tgui::Event::KeyboardKey::I},
                        {SDLK_J,            tgui::Event::KeyboardKey::J},
                        {SDLK_K,            tgui::Event::KeyboardKey::K},
                        {SDLK_L,            tgui::Event::KeyboardKey::L},
                        {SDLK_M,            tgui::Event::KeyboardKey::M},
                        {SDLK_N,            tgui::Event::KeyboardKey::N},
                        {SDLK_O,            tgui::Event::KeyboardKey::O},
                        {SDLK_P,            tgui::Event::KeyboardKey::P},
                        {SDLK_Q,            tgui::Event::KeyboardKey::Q},
                        {SDLK_R,            tgui::Event::KeyboardKey::R},
                        {SDLK_S,            tgui::Event::KeyboardKey::S},
                        {SDLK_T,            tgui::Event::KeyboardKey::T},
                        {SDLK_U,            tgui::Event::KeyboardKey::U},
                        {SDLK_V,            tgui::Event::KeyboardKey::V},
                        {SDLK_W,            tgui::Event::KeyboardKey::W},
                        {SDLK_X,            tgui::Event::KeyboardKey::X},
                        {SDLK_Y,            tgui::Event::KeyboardKey::Y},
                        {SDLK_Z,            tgui::Event::KeyboardKey::Z},
#else
                    std::array<std::pair<int32_t, tgui::Event::KeyboardKey>, 101> keys = {{
                        {SDLK_a,            tgui::Event::KeyboardKey::A},
                        {SDLK_b,            tgui::Event::KeyboardKey::B},
                        {SDLK_c,            tgui::Event::KeyboardKey::C},
                        {SDLK_d,            tgui::Event::KeyboardKey::D},
                        {SDLK_e,            tgui::Event::KeyboardKey::E},
                        {SDLK_f,            tgui::Event::KeyboardKey::F},
                        {SDLK_g,            tgui::Event::KeyboardKey::G},
                        {SDLK_h,            tgui::Event::KeyboardKey::H},
                        {SDLK_i,            tgui::Event::KeyboardKey::I},
                        {SDLK_j,            tgui::Event::KeyboardKey::J},
                        {SDLK_k,            tgui::Event::KeyboardKey::K},
                        {SDLK_l,            tgui::Event::KeyboardKey::L},
                        {SDLK_m,            tgui::Event::KeyboardKey::M},
                        {SDLK_n,            tgui::Event::KeyboardKey::N},
                        {SDLK_o,            tgui::Event::KeyboardKey::O},
                        {SDLK_p,            tgui::Event::KeyboardKey::P},
                        {SDLK_q,            tgui::Event::KeyboardKey::Q},
                        {SDLK_r,            tgui::Event::KeyboardKey::R},
                        {SDLK_s,            tgui::Event::KeyboardKey::S},
                        {SDLK_t,            tgui::Event::KeyboardKey::T},
                        {SDLK_u,            tgui::Event::KeyboardKey::U},
                        {SDLK_v,            tgui::Event::KeyboardKey::V},
                        {SDLK_w,            tgui::Event::KeyboardKey::W},
                        {SDLK_x,            tgui::Event::KeyboardKey::X},
                        {SDLK_y,            tgui::Event::KeyboardKey::Y},
                        {SDLK_z,            tgui::Event::KeyboardKey::Z},
#endif
                        {SDLK_0,            tgui::Event::KeyboardKey::Num0},
                        {SDLK_1,            tgui::Event::KeyboardKey::Num1},
                        {SDLK_2,            tgui::Event::KeyboardKey::Num2},
                        {SDLK_3,            tgui::Event::KeyboardKey::Num3},
                        {SDLK_4,            tgui::Event::KeyboardKey::Num4},
                        {SDLK_5,            tgui::Event::KeyboardKey::Num5},
                        {SDLK_6,            tgui::Event::KeyboardKey::Num6},
                        {SDLK_7,            tgui::Event::KeyboardKey::Num7},
                        {SDLK_8,            tgui::Event::KeyboardKey::Num8},
                        {SDLK_9,            tgui::Event::KeyboardKey::Num9},
                        {SDLK_ESCAPE,       tgui::Event::KeyboardKey::Escape},
                        {SDLK_LCTRL,        tgui::Event::KeyboardKey::LControl},
                        {SDLK_LSHIFT,       tgui::Event::KeyboardKey::LShift},
                        {SDLK_LALT,         tgui::Event::KeyboardKey::LAlt},
                        {SDLK_LGUI,         tgui::Event::KeyboardKey::LSystem},
                        {SDLK_RCTRL,        tgui::Event::KeyboardKey::RControl},
                        {SDLK_RSHIFT,       tgui::Event::KeyboardKey::RShift},
                        {SDLK_RALT,         tgui::Event::KeyboardKey::RAlt},
                        {SDLK_RGUI,         tgui::Event::KeyboardKey::RSystem},
                        {SDLK_MENU,         tgui::Event::KeyboardKey::Menu},
                        {SDLK_LEFTBRACKET,  tgui::Event::KeyboardKey::LBracket},
                        {SDLK_RIGHTBRACKET, tgui::Event::KeyboardKey::RBracket},
                        {SDLK_SEMICOLON,    tgui::Event::KeyboardKey::Semicolon},
                        {SDLK_COMMA,        tgui::Event::KeyboardKey::Comma},
                        {SDLK_PERIOD,       tgui::Event::KeyboardKey::Period},
#if SDL_MAJOR_VERSION >= 3
                        {SDLK_APOSTROPHE,   tgui::Event::KeyboardKey::Quote},
#else
                        {SDLK_QUOTE,        tgui::Event::KeyboardKey::Quote},
#endif
                        {SDLK_SLASH,        tgui::Event::KeyboardKey::Slash},
                        {SDLK_BACKSLASH,    tgui::Event::KeyboardKey::Backslash},
                        {SDLK_EQUALS,       tgui::Event::KeyboardKey::Equal},
                        {SDLK_MINUS,        tgui::Event::KeyboardKey::Minus},
                        {SDLK_SPACE,        tgui::Event::KeyboardKey::Space},
                        {SDLK_RETURN,       tgui::Event::KeyboardKey::Enter},
                        {SDLK_KP_ENTER,     tgui::Event::KeyboardKey::Enter},
                        {SDLK_BACKSPACE,    tgui::Event::KeyboardKey::Backspace},
                        {SDLK_TAB,          tgui::Event::KeyboardKey::Tab},
                        {SDLK_PAGEUP,       tgui::Event::KeyboardKey::PageUp},
                        {SDLK_PAGEDOWN,     tgui::Event::KeyboardKey::PageDown},
                        {SDLK_END,          tgui::Event::KeyboardKey::End},
                        {SDLK_HOME,         tgui::Event::KeyboardKey::Home},
                        {SDLK_INSERT,       tgui::Event::KeyboardKey::Insert},
                        {SDLK_DELETE,       tgui::Event::KeyboardKey::Delete},
                        {SDLK_KP_PLUS,      tgui::Event::KeyboardKey::Add},
                        {SDLK_KP_MINUS,     tgui::Event::KeyboardKey::Subtract},
                        {SDLK_KP_MULTIPLY,  tgui::Event::KeyboardKey::Multiply},
                        {SDLK_KP_DIVIDE,    tgui::Event::KeyboardKey::Divide},
                        {SDLK_LEFT,         tgui::Event::KeyboardKey::Left},
                        {SDLK_RIGHT,        tgui::Event::KeyboardKey::Right},
                        {SDLK_UP,           tgui::Event::KeyboardKey::Up},
                        {SDLK_DOWN,         tgui::Event::KeyboardKey::Down},
                        {SDLK_KP_0,         tgui::Event::KeyboardKey::Numpad0},
                        {SDLK_KP_1,         tgui::Event::KeyboardKey::Numpad1},
                        {SDLK_KP_2,         tgui::Event::KeyboardKey::Numpad2},
                        {SDLK_KP_3,         tgui::Event::KeyboardKey::Numpad3},
                        {SDLK_KP_4,         tgui::Event::KeyboardKey::Numpad4},
                        {SDLK_KP_5,         tgui::Event::KeyboardKey::Numpad5},
                        {SDLK_KP_6,         tgui::Event::KeyboardKey::Numpad6},
                        {SDLK_KP_7,         tgui::Event::KeyboardKey::Numpad7},
                        {SDLK_KP_8,         tgui::Event::KeyboardKey::Numpad8},
                        {SDLK_KP_9,         tgui::Event::KeyboardKey::Numpad9},
                        {SDLK_F1,           tgui::Event::KeyboardKey::F1},
                        {SDLK_F2,           tgui::Event::KeyboardKey::F2},
                        {SDLK_F3,           tgui::Event::KeyboardKey::F3},
                        {SDLK_F4,           tgui::Event::KeyboardKey::F4},
                        {SDLK_F5,           tgui::Event::KeyboardKey::F5},
                        {SDLK_F6,           tgui::Event::KeyboardKey::F6},
                        {SDLK_F7,           tgui::Event::KeyboardKey::F7},
                        {SDLK_F8,           tgui::Event::KeyboardKey::F8},
                        {SDLK_F9,           tgui::Event::KeyboardKey::F9},
                        {SDLK_F10,          tgui::Event::KeyboardKey::F10},
                        {SDLK_F11,          tgui::Event::KeyboardKey::F11},
                        {SDLK_F12,          tgui::Event::KeyboardKey::F12},
                        {SDLK_F13,          tgui::Event::KeyboardKey::F13},
                        {SDLK_F14,          tgui::Event::KeyboardKey::F14},
                        {SDLK_F15,          tgui::Event::KeyboardKey::F15},
                        {SDLK_PAUSE,        tgui::Event::KeyboardKey::Pause}
                    }};
                    for (auto pair : keys)
                    {
                        eventSdlKey = pair.first;

                        tgui::Event eventTGUI;
                        REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                        REQUIRE(eventTGUI.key.code == pair.second);
                    }
                }

                SECTION("Invalid key code")
                {
                    eventSdlKey = SDLK_UNKNOWN;

                    tgui::Event eventTGUI;
                    REQUIRE(!backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                }

                SECTION("Modifiers")
                {
                    eventSdlKey = SDLK_SPACE;
                    eventSdlMod = SDL_KMOD_LCTRL | SDL_KMOD_RSHIFT;

                    tgui::Event eventTGUI;
                    REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                    REQUIRE(eventTGUI.type == tgui::Event::Type::KeyPressed);
                    REQUIRE(eventTGUI.key.code == tgui::Event::KeyboardKey::Space);
                    REQUIRE(!eventTGUI.key.alt);
                    REQUIRE(eventTGUI.key.control);
                    REQUIRE(eventTGUI.key.shift);
                    REQUIRE(!eventTGUI.key.system);
                }

                SECTION("Key release")
                {
                    eventSDL.type = SDL_EVENT_KEY_UP;
                    eventSdlKey = SDLK_SPACE;
#if SDL_MAJOR_VERSION >= 3
                    eventSDL.key.down = false;
#else
                    eventSDL.key.state = SDL_RELEASED;
#endif
                    tgui::Event eventTGUI;
                    REQUIRE(!backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                }

#if (SDL_MAJOR_VERSION > 2) || ((SDL_MAJOR_VERSION == 2) && (SDL_MINOR_VERSION > 0)) || ((SDL_MAJOR_VERSION == 2) && (SDL_MINOR_VERSION == 0) && (SDL_PATCHLEVEL >= 22))
                SECTION("Numpad keys with NumLock off")
                {
                    tgui::Event eventTGUI;
                    eventSdlMod = SDL_KMOD_NUM;
                    eventSdlKey = SDLK_KP_0;
                    REQUIRE((backendGuiSDL->convertEvent(eventSDL, eventTGUI) && eventTGUI.key.code == tgui::Event::KeyboardKey::Numpad0));

                    eventSdlMod = SDL_KMOD_NONE;
                    eventSdlKey = SDLK_KP_0;
                    REQUIRE((backendGuiSDL->convertEvent(eventSDL, eventTGUI) && eventTGUI.key.code == tgui::Event::KeyboardKey::Insert));
                    eventSdlKey = SDLK_KP_1;
                    REQUIRE((backendGuiSDL->convertEvent(eventSDL, eventTGUI) && eventTGUI.key.code == tgui::Event::KeyboardKey::End));
                    eventSdlKey = SDLK_KP_2;
                    REQUIRE((backendGuiSDL->convertEvent(eventSDL, eventTGUI) && eventTGUI.key.code == tgui::Event::KeyboardKey::Down));
                    eventSdlKey = SDLK_KP_3;
                    REQUIRE((backendGuiSDL->convertEvent(eventSDL, eventTGUI) && eventTGUI.key.code == tgui::Event::KeyboardKey::PageDown));
                    eventSdlKey = SDLK_KP_4;
                    REQUIRE((backendGuiSDL->convertEvent(eventSDL, eventTGUI) && eventTGUI.key.code == tgui::Event::KeyboardKey::Left));
                    eventSdlKey = SDLK_KP_5;
                    REQUIRE(!backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                    eventSdlKey = SDLK_KP_6;
                    REQUIRE((backendGuiSDL->convertEvent(eventSDL, eventTGUI) && eventTGUI.key.code == tgui::Event::KeyboardKey::Right));
                    eventSdlKey = SDLK_KP_7;
                    REQUIRE((backendGuiSDL->convertEvent(eventSDL, eventTGUI) && eventTGUI.key.code == tgui::Event::KeyboardKey::Home));
                    eventSdlKey = SDLK_KP_8;
                    REQUIRE((backendGuiSDL->convertEvent(eventSDL, eventTGUI) && eventTGUI.key.code == tgui::Event::KeyboardKey::Up));
                    eventSdlKey = SDLK_KP_9;
                    REQUIRE((backendGuiSDL->convertEvent(eventSDL, eventTGUI) && eventTGUI.key.code == tgui::Event::KeyboardKey::PageUp));
                }
#endif
            }

            SECTION("GainedFocus")
            {
                SDL_Event eventSDL;
#if SDL_MAJOR_VERSION >= 3
                eventSDL.type = SDL_EVENT_WINDOW_FOCUS_GAINED;
#else
                eventSDL.type = SDL_WINDOWEVENT;
                eventSDL.window.event = SDL_WINDOWEVENT_FOCUS_GAINED;
#endif

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::GainedFocus);
            }

            SECTION("LostFocus")
            {
                SDL_Event eventSDL;
#if SDL_MAJOR_VERSION >= 3
                eventSDL.type = SDL_EVENT_WINDOW_FOCUS_LOST;
#else
                eventSDL.type = SDL_WINDOWEVENT;
                eventSDL.window.event = SDL_WINDOWEVENT_FOCUS_LOST;
#endif

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::LostFocus);
            }

            SECTION("Closed")
            {
                SDL_Event eventSDL;
                eventSDL.type = SDL_EVENT_QUIT;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::Closed);
            }

            SECTION("Resized")
            {
                SDL_Event eventSDL;
#if SDL_MAJOR_VERSION >= 3
                eventSDL.type = SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED;
#else
                eventSDL.type = SDL_WINDOWEVENT;
                eventSDL.window.event = SDL_WINDOWEVENT_SIZE_CHANGED;
#endif
                eventSDL.window.data1 = 400;
                eventSDL.window.data2 = 300;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::Resized);
                REQUIRE(eventTGUI.size.width == 400);
                REQUIRE(eventTGUI.size.height == 300);
            }

            SECTION("TextEntered")
            {
                SDL_Event eventSDL;
                eventSDL.type = SDL_EVENT_TEXT_INPUT;
#if SDL_MAJOR_VERSION >= 3
                char textInput[4] = {
                    static_cast<char>(static_cast<unsigned char>(0xE2)),
                    static_cast<char>(static_cast<unsigned char>(0x9C)),
                    static_cast<char>(static_cast<unsigned char>(0x85)),
                    '\0'
                };
                eventSDL.text.text = textInput;
#else
                eventSDL.text.text[0] = static_cast<char>(static_cast<unsigned char>(0xE2));
                eventSDL.text.text[1] = static_cast<char>(static_cast<unsigned char>(0x9C));
                eventSDL.text.text[2] = static_cast<char>(static_cast<unsigned char>(0x85));
                eventSDL.text.text[3] = '\0';
#endif
                tgui::Event eventTGUI;
                REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::TextEntered);
                REQUIRE(eventTGUI.text.unicode == 0x2705);
            }

            SECTION("MouseWheelScrolled")
            {
                SDL_Event eventSDL;

#if SDL_MAJOR_VERSION >= 3
                eventSDL.wheel.mouse_x = 200;
                eventSDL.wheel.mouse_y = 150;
#elif ((SDL_MAJOR_VERSION == 2) && (SDL_MINOR_VERSION >= 26))
                eventSDL.wheel.mouseX = 200;
                eventSDL.wheel.mouseY = 150;
#else
                // SDL < 2.26 had no mouse position in its mouse scroll event, so first simulate a mouse move
                eventSDL.type = SDL_MOUSEMOTION;
                eventSDL.motion.which = 1;
                eventSDL.motion.x = 200;
                eventSDL.motion.y = 150;
                backendGuiSDL->handleEvent(eventSDL);
#endif

                eventSDL.type = SDL_EVENT_MOUSE_WHEEL;
                eventSDL.wheel.x = 0;
                eventSDL.wheel.y = 2;
                eventSDL.wheel.direction = SDL_MOUSEWHEEL_NORMAL;
#if (SDL_MAJOR_VERSION == 2) && ((SDL_MINOR_VERSION > 0) || (SDL_PATCHLEVEL >= 18))
                eventSDL.wheel.preciseX = static_cast<float>(eventSDL.wheel.x);
                eventSDL.wheel.preciseY = static_cast<float>(eventSDL.wheel.y);
#endif

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseWheelScrolled);
                REQUIRE(eventTGUI.mouseWheel.delta == 2);
                REQUIRE(eventTGUI.mouseWheel.x == 200);
                REQUIRE(eventTGUI.mouseWheel.y == 150);

                // Value can be inverted
                eventSDL.wheel.direction = SDL_MOUSEWHEEL_FLIPPED;
                REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                REQUIRE(eventTGUI.mouseWheel.delta == -2);

                // We only handle vertical scrolling
                eventSDL.wheel.x = 2;
                eventSDL.wheel.y = 0;
#if (SDL_MAJOR_VERSION == 2) && ((SDL_MINOR_VERSION > 0) || (SDL_PATCHLEVEL >= 18))
                eventSDL.wheel.preciseX = static_cast<float>(eventSDL.wheel.x);
                eventSDL.wheel.preciseY = static_cast<float>(eventSDL.wheel.y);
#endif
                REQUIRE(!backendGuiSDL->convertEvent(eventSDL, eventTGUI));
            }

            SECTION("MouseButtonPressed")
            {
                SDL_Event eventSDL;
                eventSDL.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
                eventSDL.button.which = 1;
                eventSDL.button.button = SDL_BUTTON_LEFT;
                eventSDL.button.x = 200;
                eventSDL.button.y = 150;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseButtonPressed);
                REQUIRE(eventTGUI.mouseButton.button == tgui::Event::MouseButton::Left);
                REQUIRE(eventTGUI.mouseButton.x == 200);
                REQUIRE(eventTGUI.mouseButton.y == 150);

                eventSDL.button.button = SDL_BUTTON_RIGHT;
                REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                REQUIRE(eventTGUI.mouseButton.button == tgui::Event::MouseButton::Right);

                eventSDL.button.button = SDL_BUTTON_MIDDLE;
                REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                REQUIRE(eventTGUI.mouseButton.button == tgui::Event::MouseButton::Middle);

                // Only left, middle and right mouse buttons are handled
                eventSDL.button.button = SDL_BUTTON_X1;
                REQUIRE(!backendGuiSDL->convertEvent(eventSDL, eventTGUI));
            }

            SECTION("MouseButtonReleased")
            {
                SDL_Event eventSDL;
                eventSDL.type = SDL_EVENT_MOUSE_BUTTON_UP;
                eventSDL.button.which = 1;
                eventSDL.button.button = SDL_BUTTON_LEFT;
                eventSDL.button.x = 200;
                eventSDL.button.y = 150;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseButtonReleased);
                REQUIRE(eventTGUI.mouseButton.button == tgui::Event::MouseButton::Left);
                REQUIRE(eventTGUI.mouseButton.x == 200);
                REQUIRE(eventTGUI.mouseButton.y == 150);

                eventSDL.button.button = SDL_BUTTON_RIGHT;
                REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                REQUIRE(eventTGUI.mouseButton.button == tgui::Event::MouseButton::Right);

                eventSDL.button.button = SDL_BUTTON_MIDDLE;
                REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                REQUIRE(eventTGUI.mouseButton.button == tgui::Event::MouseButton::Middle);

                // Only left, middle and right mouse buttons are handled
                eventSDL.button.button = SDL_BUTTON_X1;
                REQUIRE(!backendGuiSDL->convertEvent(eventSDL, eventTGUI));
            }

            SECTION("MouseMoved")
            {
                SDL_Event eventSDL;
                eventSDL.type = SDL_EVENT_MOUSE_MOTION;
                eventSDL.motion.which = 1;
                eventSDL.motion.x = 200;
                eventSDL.motion.y = 150;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseMoved);
                REQUIRE(eventTGUI.mouseMove.x == 200);
                REQUIRE(eventTGUI.mouseMove.y == 150);
            }

            SECTION("Touch")
            {
                // Touch events have relative positions, so we need the window size.
                // Since we can't actually access the window size here, we instead take the viewport size which
                // is the same as the viewport isn't manually changed.
                const tgui::Vector2f windowSize = backendGuiSDL->getViewport().getSize();

                SDL_Event eventSDL;
                eventSDL.type = SDL_EVENT_FINGER_DOWN;
#if SDL_MAJOR_VERSION >= 3
                eventSDL.tfinger.touchID = 1;
                eventSDL.tfinger.fingerID = 1;
#else
                eventSDL.tfinger.touchId = 1;
                eventSDL.tfinger.fingerId = 1;
#endif
                eventSDL.tfinger.x = 200.f / windowSize.x;
                eventSDL.tfinger.y = 150.f / windowSize.y;

                tgui::Event eventTGUI;
                REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseButtonPressed);
                REQUIRE(eventTGUI.mouseButton.x == 200);
                REQUIRE(eventTGUI.mouseButton.y == 150);

                eventSDL.type = SDL_EVENT_FINGER_MOTION;
                eventSDL.tfinger.x = 210.f / windowSize.x;
                eventSDL.tfinger.y = 155.f / windowSize.y;
                REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseMoved);
                REQUIRE(eventTGUI.mouseMove.x == 210);
                REQUIRE(eventTGUI.mouseMove.y == 155);

                eventSDL.type = SDL_EVENT_FINGER_UP;
                eventSDL.tfinger.x = 220.f / windowSize.x;
                eventSDL.tfinger.y = 160.f / windowSize.y;
                REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseButtonReleased);
                REQUIRE(eventTGUI.mouseButton.x == 220);
                REQUIRE(eventTGUI.mouseButton.y == 160);
            }

            SECTION("MouseEntered")
            {
                SDL_Event eventSDL;
#if SDL_MAJOR_VERSION >= 3
                eventSDL.type = SDL_EVENT_WINDOW_MOUSE_ENTER;
#else
                eventSDL.type = SDL_WINDOWEVENT;
                eventSDL.window.event = SDL_WINDOWEVENT_ENTER;
#endif
                tgui::Event eventTGUI;
                REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseEntered);
            }

            SECTION("MouseLeft")
            {
                SDL_Event eventSDL;
#if SDL_MAJOR_VERSION >= 3
                eventSDL.type = SDL_EVENT_WINDOW_MOUSE_LEAVE;
#else
                eventSDL.type = SDL_WINDOWEVENT;
                eventSDL.window.event = SDL_WINDOWEVENT_LEAVE;
#endif
                tgui::Event eventTGUI;
                REQUIRE(backendGuiSDL->convertEvent(eventSDL, eventTGUI));
                REQUIRE(eventTGUI.type == tgui::Event::Type::MouseLeft);
            }

            SECTION("Handling events")
            {
                auto childWindow = tgui::ChildWindow::create();
                childWindow->setResizable(true);
                childWindow->setPosition({20, 10});
                childWindow->setSize({300, 250});
                childWindow->getRenderer()->setTitleBarHeight(20);
                childWindow->getRenderer()->setBorders({1});
                globalGui->add(childWindow);

                auto slider = tgui::Slider::create();
                slider->setChangeValueOnScroll(true);
                slider->setPosition({230, 20});
                slider->setSize({20, 150});
                slider->setMaximum(10);
                slider->setValue(3);
                childWindow->add(slider);

                auto editBox = tgui::EditBox::create();
                editBox->setPosition({10, 10});
                editBox->setSize({150, 24});
                childWindow->add(editBox);

                editBox->setFocused(true);

                // Type 3 characters in the edit box
                SDL_Event eventSDL;
                eventSDL.type = SDL_EVENT_TEXT_INPUT;
#if SDL_MAJOR_VERSION >= 3
                char textInput[2] = "A";
                eventSDL.text.text = textInput;
                backendGuiSDL->handleEvent(eventSDL);
                textInput[0] = 'B';
                backendGuiSDL->handleEvent(eventSDL);
                textInput[0] = 'C';
                backendGuiSDL->handleEvent(eventSDL);
#else
                eventSDL.text.text[0] = 'A';
                eventSDL.text.text[1] = 0;
                backendGuiSDL->handleEvent(eventSDL);
                eventSDL.text.text[0] = 'B';
                backendGuiSDL->handleEvent(eventSDL);
                eventSDL.text.text[0] = 'C';
                backendGuiSDL->handleEvent(eventSDL);
#endif
                // Erase the second character from the edit box
                eventSDL.type = SDL_EVENT_KEY_DOWN;
                eventSDL.key.windowID = 0;
                eventSDL.key.repeat = 0;
#if SDL_MAJOR_VERSION >= 3
                eventSDL.key.down = true;
                eventSDL.key.scancode = SDL_SCANCODE_UNKNOWN;
                eventSDL.key.mod = SDL_KMOD_NONE;
                eventSDL.key.key = SDLK_LEFT;
                backendGuiSDL->handleEvent(eventSDL);
                eventSDL.key.key = SDLK_BACKSPACE;
                backendGuiSDL->handleEvent(eventSDL);
#else
                eventSDL.key.state = SDL_PRESSED;
                eventSDL.key.keysym.scancode = SDL_SCANCODE_UNKNOWN;
                eventSDL.key.keysym.mod = SDL_KMOD_NONE;
                eventSDL.key.keysym.sym = SDLK_LEFT;
                backendGuiSDL->handleEvent(eventSDL);
                eventSDL.key.keysym.sym = SDLK_BACKSPACE;
                backendGuiSDL->handleEvent(eventSDL);
#endif

                // Verify that the events were correctly processed by the edit box
                REQUIRE(editBox->getText() == "AC");

                // Scroll the mouse wheel on top of the slider and verify that its value changes
#if SDL_MAJOR_VERSION >= 3
                eventSDL.wheel.mouse_x = 260;
                eventSDL.wheel.mouse_y = 80;
#elif ((SDL_MAJOR_VERSION == 2) && (SDL_MINOR_VERSION >= 26))
                eventSDL.wheel.mouseX = 260;
                eventSDL.wheel.mouseY = 80;
#else
                eventSDL.type = SDL_MOUSEMOTION;
                eventSDL.motion.which = 1;
                eventSDL.motion.x = 260;
                eventSDL.motion.y = 80;
                backendGuiSDL->handleEvent(eventSDL);
#endif
                eventSDL.type = SDL_EVENT_MOUSE_WHEEL;
                eventSDL.wheel.direction = SDL_MOUSEWHEEL_NORMAL;
                eventSDL.wheel.x = 0;
                eventSDL.wheel.y = 4;
#if (SDL_MAJOR_VERSION == 2) && ((SDL_MINOR_VERSION > 0) || (SDL_PATCHLEVEL >= 18))
                eventSDL.wheel.preciseX = static_cast<float>(eventSDL.wheel.x);
                eventSDL.wheel.preciseY = static_cast<float>(eventSDL.wheel.y);
#endif
                backendGuiSDL->handleEvent(eventSDL);
                eventSDL.wheel.y = -1;
#if (SDL_MAJOR_VERSION == 2) && ((SDL_MINOR_VERSION > 0) || (SDL_PATCHLEVEL >= 18))
                eventSDL.wheel.preciseY = static_cast<float>(eventSDL.wheel.y);
#endif
                backendGuiSDL->handleEvent(eventSDL);
                backendGuiSDL->handleEvent(eventSDL);

                REQUIRE(slider->getValue() == 5);

                // The mouse leaving the window will remove the hover state of widgets
                unsigned int mouseLeftCount = 0;
                eventSDL.type = SDL_EVENT_MOUSE_MOTION;
                eventSDL.motion.which = 1;
                eventSDL.motion.x = 260;
                eventSDL.motion.y = 80;
                backendGuiSDL->handleEvent(eventSDL);
                slider->onMouseLeave([&]{ genericCallback(mouseLeftCount); });
#if SDL_MAJOR_VERSION >= 3
                eventSDL.type = SDL_EVENT_WINDOW_MOUSE_LEAVE;
#else
                eventSDL.type = SDL_WINDOWEVENT;
                eventSDL.window.event = SDL_WINDOWEVENT_LEAVE;
#endif
                backendGuiSDL->handleEvent(eventSDL);
                REQUIRE(mouseLeftCount == 1);

                // Resize the child window using mouse events (decrease width with 20px)
                // Note that the resizing ignores the position of the mouse released event
                eventSDL.type = SDL_EVENT_MOUSE_BUTTON_DOWN;
                eventSDL.button.which = 1;
                eventSDL.button.button = SDL_BUTTON_LEFT;
                eventSDL.button.x = 320;
                eventSDL.button.y = 100;
                backendGuiSDL->handleEvent(eventSDL);
                eventSDL.type = SDL_EVENT_MOUSE_MOTION;
                eventSDL.motion.which = 1;
                eventSDL.motion.x = 300;
                eventSDL.motion.y = 105;
                backendGuiSDL->handleEvent(eventSDL);
                eventSDL.type = SDL_EVENT_MOUSE_BUTTON_UP;
                eventSDL.button.which = 1;
                eventSDL.button.button = SDL_BUTTON_LEFT;
                eventSDL.button.x = 290;
                eventSDL.button.y = 110;
                backendGuiSDL->handleEvent(eventSDL);

                // Resize the child window using touch events  (decrease height with 10px)
                // Note that the resizing ignores the position of the touch ended event
                const tgui::Vector2f windowSize = backendGuiSDL->getViewport().getSize();
                eventSDL.type = SDL_EVENT_FINGER_DOWN;
#if SDL_MAJOR_VERSION >= 3
                eventSDL.tfinger.touchID = 1;
#else
                eventSDL.tfinger.touchId = 1;
#endif
                eventSDL.tfinger.x = 100.f / windowSize.x;
                eventSDL.tfinger.y = 260.f / windowSize.y;
                backendGuiSDL->handleEvent(eventSDL);
                eventSDL.type = SDL_EVENT_FINGER_MOTION;
                eventSDL.tfinger.x = 110.f / windowSize.x;
                eventSDL.tfinger.y = 270.f / windowSize.y;
                backendGuiSDL->handleEvent(eventSDL);
                eventSDL.type = SDL_EVENT_FINGER_UP;
                eventSDL.tfinger.x = 105.f / windowSize.x;
                eventSDL.tfinger.y = 275.f / windowSize.y;
                backendGuiSDL->handleEvent(eventSDL);

                REQUIRE(childWindow->getSize() == tgui::Vector2f(280, 260));

                globalGui->removeAllWidgets();
            }
        }
    }
#endif

#if TGUI_HAS_WINDOW_BACKEND_GLFW
    auto backendGuiGLFW = dynamic_cast<tgui::BackendGuiGLFW*>(globalGui);
    if (backendGuiGLFW)
    {
        SECTION("GLFW")
        {
            SECTION("KeyPressed")
            {
                SECTION("All key codes")
                {
                    std::array<std::pair<int32_t, tgui::Event::KeyboardKey>, 102> keys = {{
                        {GLFW_KEY_A,             tgui::Event::KeyboardKey::A},
                        {GLFW_KEY_B,             tgui::Event::KeyboardKey::B},
                        {GLFW_KEY_C,             tgui::Event::KeyboardKey::C},
                        {GLFW_KEY_D,             tgui::Event::KeyboardKey::D},
                        {GLFW_KEY_E,             tgui::Event::KeyboardKey::E},
                        {GLFW_KEY_F,             tgui::Event::KeyboardKey::F},
                        {GLFW_KEY_G,             tgui::Event::KeyboardKey::G},
                        {GLFW_KEY_H,             tgui::Event::KeyboardKey::H},
                        {GLFW_KEY_I,             tgui::Event::KeyboardKey::I},
                        {GLFW_KEY_J,             tgui::Event::KeyboardKey::J},
                        {GLFW_KEY_K,             tgui::Event::KeyboardKey::K},
                        {GLFW_KEY_L,             tgui::Event::KeyboardKey::L},
                        {GLFW_KEY_M,             tgui::Event::KeyboardKey::M},
                        {GLFW_KEY_N,             tgui::Event::KeyboardKey::N},
                        {GLFW_KEY_O,             tgui::Event::KeyboardKey::O},
                        {GLFW_KEY_P,             tgui::Event::KeyboardKey::P},
                        {GLFW_KEY_Q,             tgui::Event::KeyboardKey::Q},
                        {GLFW_KEY_R,             tgui::Event::KeyboardKey::R},
                        {GLFW_KEY_S,             tgui::Event::KeyboardKey::S},
                        {GLFW_KEY_T,             tgui::Event::KeyboardKey::T},
                        {GLFW_KEY_U,             tgui::Event::KeyboardKey::U},
                        {GLFW_KEY_V,             tgui::Event::KeyboardKey::V},
                        {GLFW_KEY_W,             tgui::Event::KeyboardKey::W},
                        {GLFW_KEY_X,             tgui::Event::KeyboardKey::X},
                        {GLFW_KEY_Y,             tgui::Event::KeyboardKey::Y},
                        {GLFW_KEY_Z,             tgui::Event::KeyboardKey::Z},
                        {GLFW_KEY_0,             tgui::Event::KeyboardKey::Num0},
                        {GLFW_KEY_1,             tgui::Event::KeyboardKey::Num1},
                        {GLFW_KEY_2,             tgui::Event::KeyboardKey::Num2},
                        {GLFW_KEY_3,             tgui::Event::KeyboardKey::Num3},
                        {GLFW_KEY_4,             tgui::Event::KeyboardKey::Num4},
                        {GLFW_KEY_5,             tgui::Event::KeyboardKey::Num5},
                        {GLFW_KEY_6,             tgui::Event::KeyboardKey::Num6},
                        {GLFW_KEY_7,             tgui::Event::KeyboardKey::Num7},
                        {GLFW_KEY_8,             tgui::Event::KeyboardKey::Num8},
                        {GLFW_KEY_9,             tgui::Event::KeyboardKey::Num9},
                        {GLFW_KEY_ESCAPE,        tgui::Event::KeyboardKey::Escape},
                        {GLFW_KEY_LEFT_CONTROL,  tgui::Event::KeyboardKey::LControl},
                        {GLFW_KEY_LEFT_SHIFT,    tgui::Event::KeyboardKey::LShift},
                        {GLFW_KEY_LEFT_ALT,      tgui::Event::KeyboardKey::LAlt},
                        {GLFW_KEY_LEFT_SUPER,    tgui::Event::KeyboardKey::LSystem},
                        {GLFW_KEY_RIGHT_CONTROL, tgui::Event::KeyboardKey::RControl},
                        {GLFW_KEY_RIGHT_SHIFT,   tgui::Event::KeyboardKey::RShift},
                        {GLFW_KEY_RIGHT_ALT,     tgui::Event::KeyboardKey::RAlt},
                        {GLFW_KEY_RIGHT_SUPER,   tgui::Event::KeyboardKey::RSystem},
                        {GLFW_KEY_MENU,          tgui::Event::KeyboardKey::Menu},
                        {GLFW_KEY_LEFT_BRACKET,  tgui::Event::KeyboardKey::LBracket},
                        {GLFW_KEY_RIGHT_BRACKET, tgui::Event::KeyboardKey::RBracket},
                        {GLFW_KEY_SEMICOLON,     tgui::Event::KeyboardKey::Semicolon},
                        {GLFW_KEY_COMMA,         tgui::Event::KeyboardKey::Comma},
                        {GLFW_KEY_PERIOD,        tgui::Event::KeyboardKey::Period},
                        {GLFW_KEY_APOSTROPHE,    tgui::Event::KeyboardKey::Quote},
                        {GLFW_KEY_SLASH,         tgui::Event::KeyboardKey::Slash},
                        {GLFW_KEY_BACKSLASH,     tgui::Event::KeyboardKey::Backslash},
                        {GLFW_KEY_EQUAL,         tgui::Event::KeyboardKey::Equal},
                        {GLFW_KEY_MINUS,         tgui::Event::KeyboardKey::Minus},
                        {GLFW_KEY_SPACE,         tgui::Event::KeyboardKey::Space},
                        {GLFW_KEY_ENTER,         tgui::Event::KeyboardKey::Enter},
                        {GLFW_KEY_BACKSPACE,     tgui::Event::KeyboardKey::Backspace},
                        {GLFW_KEY_TAB,           tgui::Event::KeyboardKey::Tab},
                        {GLFW_KEY_PAGE_UP,       tgui::Event::KeyboardKey::PageUp},
                        {GLFW_KEY_PAGE_DOWN,     tgui::Event::KeyboardKey::PageDown},
                        {GLFW_KEY_END,           tgui::Event::KeyboardKey::End},
                        {GLFW_KEY_HOME,          tgui::Event::KeyboardKey::Home},
                        {GLFW_KEY_INSERT,        tgui::Event::KeyboardKey::Insert},
                        {GLFW_KEY_DELETE,        tgui::Event::KeyboardKey::Delete},
                        {GLFW_KEY_KP_ADD,        tgui::Event::KeyboardKey::Add},
                        {GLFW_KEY_KP_SUBTRACT,   tgui::Event::KeyboardKey::Subtract},
                        {GLFW_KEY_KP_MULTIPLY,   tgui::Event::KeyboardKey::Multiply},
                        {GLFW_KEY_KP_DIVIDE,     tgui::Event::KeyboardKey::Divide},
                        {GLFW_KEY_KP_DECIMAL,    tgui::Event::KeyboardKey::Period},
                        {GLFW_KEY_KP_EQUAL,      tgui::Event::KeyboardKey::Equal},
                        {GLFW_KEY_LEFT,          tgui::Event::KeyboardKey::Left},
                        {GLFW_KEY_RIGHT,         tgui::Event::KeyboardKey::Right},
                        {GLFW_KEY_UP,            tgui::Event::KeyboardKey::Up},
                        {GLFW_KEY_DOWN,          tgui::Event::KeyboardKey::Down},
                        {GLFW_KEY_KP_0,          tgui::Event::KeyboardKey::Numpad0},
                        {GLFW_KEY_KP_1,          tgui::Event::KeyboardKey::Numpad1},
                        {GLFW_KEY_KP_2,          tgui::Event::KeyboardKey::Numpad2},
                        {GLFW_KEY_KP_3,          tgui::Event::KeyboardKey::Numpad3},
                        {GLFW_KEY_KP_4,          tgui::Event::KeyboardKey::Numpad4},
                        {GLFW_KEY_KP_5,          tgui::Event::KeyboardKey::Numpad5},
                        {GLFW_KEY_KP_6,          tgui::Event::KeyboardKey::Numpad6},
                        {GLFW_KEY_KP_7,          tgui::Event::KeyboardKey::Numpad7},
                        {GLFW_KEY_KP_8,          tgui::Event::KeyboardKey::Numpad8},
                        {GLFW_KEY_KP_9,          tgui::Event::KeyboardKey::Numpad9},
                        {GLFW_KEY_F1,            tgui::Event::KeyboardKey::F1},
                        {GLFW_KEY_F2,            tgui::Event::KeyboardKey::F2},
                        {GLFW_KEY_F3,            tgui::Event::KeyboardKey::F3},
                        {GLFW_KEY_F4,            tgui::Event::KeyboardKey::F4},
                        {GLFW_KEY_F5,            tgui::Event::KeyboardKey::F5},
                        {GLFW_KEY_F6,            tgui::Event::KeyboardKey::F6},
                        {GLFW_KEY_F7,            tgui::Event::KeyboardKey::F7},
                        {GLFW_KEY_F8,            tgui::Event::KeyboardKey::F8},
                        {GLFW_KEY_F9,            tgui::Event::KeyboardKey::F9},
                        {GLFW_KEY_F10,           tgui::Event::KeyboardKey::F10},
                        {GLFW_KEY_F11,           tgui::Event::KeyboardKey::F11},
                        {GLFW_KEY_F12,           tgui::Event::KeyboardKey::F12},
                        {GLFW_KEY_F13,           tgui::Event::KeyboardKey::F13},
                        {GLFW_KEY_F14,           tgui::Event::KeyboardKey::F14},
                        {GLFW_KEY_F15,           tgui::Event::KeyboardKey::F15},
                        {GLFW_KEY_PAUSE,         tgui::Event::KeyboardKey::Pause}
                    }};
                    for (auto pair : keys)
                    {
                        auto eventTGUI = backendGuiGLFW->convertKeyEvent(pair.first, 0, GLFW_PRESS, GLFW_MOD_NUM_LOCK);
                        REQUIRE(eventTGUI);
                        REQUIRE(eventTGUI->key.code == pair.second);
                    }
                }

                SECTION("Invalid key code")
                {
                    REQUIRE(!backendGuiGLFW->convertKeyEvent(GLFW_KEY_UNKNOWN, 0, GLFW_PRESS, 0));
                }

                SECTION("Modifiers")
                {
                    auto eventTGUI = backendGuiGLFW->convertKeyEvent(GLFW_KEY_SPACE, 0, GLFW_PRESS, GLFW_MOD_CONTROL | GLFW_MOD_SHIFT);
                    REQUIRE(eventTGUI);
                    REQUIRE(eventTGUI->type == tgui::Event::Type::KeyPressed);
                    REQUIRE(eventTGUI->key.code == tgui::Event::KeyboardKey::Space);
                    REQUIRE(!eventTGUI->key.alt);
                    REQUIRE(eventTGUI->key.control);
                    REQUIRE(eventTGUI->key.shift);
                    REQUIRE(!eventTGUI->key.system);
                }

                SECTION("Key release")
                {
                    REQUIRE(!backendGuiGLFW->convertKeyEvent(GLFW_KEY_SPACE, 0, GLFW_RELEASE, 0));
                }

#if GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 3)
                SECTION("Numpad keys with NumLock off")
                {
                    REQUIRE(backendGuiGLFW->convertKeyEvent(GLFW_KEY_KP_0, 0, GLFW_PRESS, GLFW_MOD_NUM_LOCK)->key.code == tgui::Event::KeyboardKey::Numpad0);

                    REQUIRE(backendGuiGLFW->convertKeyEvent(GLFW_KEY_KP_0, 0, GLFW_PRESS, 0)->key.code == tgui::Event::KeyboardKey::Insert);
                    REQUIRE(backendGuiGLFW->convertKeyEvent(GLFW_KEY_KP_1, 0, GLFW_PRESS, 0)->key.code == tgui::Event::KeyboardKey::End);
                    REQUIRE(backendGuiGLFW->convertKeyEvent(GLFW_KEY_KP_2, 0, GLFW_PRESS, 0)->key.code == tgui::Event::KeyboardKey::Down);
                    REQUIRE(backendGuiGLFW->convertKeyEvent(GLFW_KEY_KP_3, 0, GLFW_PRESS, 0)->key.code == tgui::Event::KeyboardKey::PageDown);
                    REQUIRE(backendGuiGLFW->convertKeyEvent(GLFW_KEY_KP_4, 0, GLFW_PRESS, 0)->key.code == tgui::Event::KeyboardKey::Left);
                    REQUIRE(!backendGuiGLFW->convertKeyEvent(GLFW_KEY_KP_5, 0, GLFW_PRESS, 0));
                    REQUIRE(backendGuiGLFW->convertKeyEvent(GLFW_KEY_KP_6, 0, GLFW_PRESS, 0)->key.code == tgui::Event::KeyboardKey::Right);
                    REQUIRE(backendGuiGLFW->convertKeyEvent(GLFW_KEY_KP_7, 0, GLFW_PRESS, 0)->key.code == tgui::Event::KeyboardKey::Home);
                    REQUIRE(backendGuiGLFW->convertKeyEvent(GLFW_KEY_KP_8, 0, GLFW_PRESS, 0)->key.code == tgui::Event::KeyboardKey::Up);
                    REQUIRE(backendGuiGLFW->convertKeyEvent(GLFW_KEY_KP_9, 0, GLFW_PRESS, 0)->key.code == tgui::Event::KeyboardKey::PageUp);
                }
#endif
            }

            SECTION("GainedFocus")
            {
                auto eventTGUI = backendGuiGLFW->convertWindowFocusEvent(GLFW_TRUE);
                REQUIRE(eventTGUI);
                REQUIRE(eventTGUI->type == tgui::Event::Type::GainedFocus);
            }

            SECTION("LostFocus")
            {
                auto eventTGUI = backendGuiGLFW->convertWindowFocusEvent(GLFW_FALSE);
                REQUIRE(eventTGUI);
                REQUIRE(eventTGUI->type == tgui::Event::Type::LostFocus);
            }

            SECTION("Resized")
            {
                auto eventTGUI = backendGuiGLFW->convertSizeEvent(400, 300);
                REQUIRE(eventTGUI);
                REQUIRE(eventTGUI->type == tgui::Event::Type::Resized);
                REQUIRE(eventTGUI->size.width == 400);
                REQUIRE(eventTGUI->size.height == 300);
            }

            SECTION("TextEntered")
            {
                auto eventTGUI = backendGuiGLFW->convertCharEvent(0x2705);
                REQUIRE(eventTGUI);
                REQUIRE(eventTGUI->type == tgui::Event::Type::TextEntered);
                REQUIRE(eventTGUI->text.unicode == 0x2705);
            }

            SECTION("MouseWheelScrolled")
            {
                // GLFW has no mouse position in its mouse scroll event, so first simulate a mouse move
                backendGuiGLFW->cursorPosCallback(200, 150);

                auto eventTGUI = backendGuiGLFW->convertScrollEvent(0, 2);
                REQUIRE(eventTGUI);
                REQUIRE(eventTGUI->type == tgui::Event::Type::MouseWheelScrolled);
                REQUIRE(eventTGUI->mouseWheel.delta == 2);
                REQUIRE(eventTGUI->mouseWheel.x == 200);
                REQUIRE(eventTGUI->mouseWheel.y == 150);

                // We only handle vertical scrolling
                REQUIRE(!backendGuiGLFW->convertScrollEvent(2, 0));
            }

            SECTION("MouseButtonPressed")
            {
                // GLFW has no mouse position in its mouse press event, so first simulate a mouse move
                backendGuiGLFW->cursorPosCallback(200, 150);

                auto eventTGUI = backendGuiGLFW->convertMouseButtonEvent(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0);
                REQUIRE(eventTGUI);
                REQUIRE(eventTGUI->type == tgui::Event::Type::MouseButtonPressed);
                REQUIRE(eventTGUI->mouseButton.button == tgui::Event::MouseButton::Left);
                REQUIRE(eventTGUI->mouseButton.x == 200);
                REQUIRE(eventTGUI->mouseButton.y == 150);

                eventTGUI = backendGuiGLFW->convertMouseButtonEvent(GLFW_MOUSE_BUTTON_RIGHT, GLFW_PRESS, 0);
                REQUIRE(eventTGUI);
                REQUIRE(eventTGUI->mouseButton.button == tgui::Event::MouseButton::Right);

                eventTGUI = backendGuiGLFW->convertMouseButtonEvent(GLFW_MOUSE_BUTTON_MIDDLE, GLFW_PRESS, 0);
                REQUIRE(eventTGUI);
                REQUIRE(eventTGUI->mouseButton.button == tgui::Event::MouseButton::Middle);

                // Only left, middle and right mouse buttons are handled
                REQUIRE(!backendGuiGLFW->convertMouseButtonEvent(GLFW_MOUSE_BUTTON_4, GLFW_PRESS, 0));
            }

            SECTION("MouseButtonReleased")
            {
                // GLFW has no mouse position in its mouse press event, so first simulate a mouse move
                backendGuiGLFW->cursorPosCallback(200, 150);

                auto eventTGUI = backendGuiGLFW->convertMouseButtonEvent(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE, 0);
                REQUIRE(eventTGUI);
                REQUIRE(eventTGUI->type == tgui::Event::Type::MouseButtonReleased);
                REQUIRE(eventTGUI->mouseButton.button == tgui::Event::MouseButton::Left);
                REQUIRE(eventTGUI->mouseButton.x == 200);
                REQUIRE(eventTGUI->mouseButton.y == 150);

                eventTGUI = backendGuiGLFW->convertMouseButtonEvent(GLFW_MOUSE_BUTTON_RIGHT, GLFW_RELEASE, 0);
                REQUIRE(eventTGUI);
                REQUIRE(eventTGUI->mouseButton.button == tgui::Event::MouseButton::Right);

                eventTGUI = backendGuiGLFW->convertMouseButtonEvent(GLFW_MOUSE_BUTTON_MIDDLE, GLFW_RELEASE, 0);
                REQUIRE(eventTGUI);
                REQUIRE(eventTGUI->mouseButton.button == tgui::Event::MouseButton::Middle);

                // Only left, middle and right mouse buttons are handled
                REQUIRE(!backendGuiGLFW->convertMouseButtonEvent(GLFW_MOUSE_BUTTON_4, GLFW_RELEASE, 0));
            }

            SECTION("MouseMoved")
            {
                auto eventTGUI = backendGuiGLFW->convertCursorPosEvent(200, 150);
                REQUIRE(eventTGUI);
                REQUIRE(eventTGUI->type == tgui::Event::Type::MouseMoved);
                REQUIRE(eventTGUI->mouseMove.x == 200);
                REQUIRE(eventTGUI->mouseMove.y == 150);
            }

            SECTION("MouseEntered")
            {
                auto eventTGUI = backendGuiGLFW->convertCursorEnterEvent(true);
                REQUIRE(eventTGUI);
                REQUIRE(eventTGUI->type == tgui::Event::Type::MouseEntered);
            }

            SECTION("MouseLeft")
            {
                auto eventTGUI = backendGuiGLFW->convertCursorEnterEvent(false);
                REQUIRE(eventTGUI);
                REQUIRE(eventTGUI->type == tgui::Event::Type::MouseLeft);
            }

            SECTION("Handling events")
            {
                auto childWindow = tgui::ChildWindow::create();
                childWindow->setResizable(true);
                childWindow->setPosition({20, 10});
                childWindow->setSize({300, 250});
                childWindow->getRenderer()->setTitleBarHeight(20);
                childWindow->getRenderer()->setBorders({1});
                globalGui->add(childWindow);

                auto slider = tgui::Slider::create();
                slider->setChangeValueOnScroll(true);
                slider->setPosition({230, 20});
                slider->setSize({20, 150});
                slider->setMaximum(10);
                slider->setValue(3);
                childWindow->add(slider);

                auto editBox = tgui::EditBox::create();
                editBox->setPosition({10, 10});
                editBox->setSize({150, 24});
                childWindow->add(editBox);

                editBox->setFocused(true);

                // Type 3 characters in the edit box and erase the second one
                backendGuiGLFW->charCallback('A');
                backendGuiGLFW->charCallback('B');
                backendGuiGLFW->charCallback('C');
                backendGuiGLFW->keyCallback(GLFW_KEY_LEFT, 0, GLFW_PRESS, 0);
                backendGuiGLFW->keyCallback(GLFW_KEY_BACKSPACE, 0, GLFW_PRESS, 0);

                // Verify that the events were correctly processed by the edit box
                REQUIRE(editBox->getText() == "AC");

                // Scroll the mouse wheel on top of the slider and verify that its value changes
                backendGuiGLFW->cursorPosCallback(260, 80);
                backendGuiGLFW->scrollCallback(0, 4);
                backendGuiGLFW->scrollCallback(0, -1);
                backendGuiGLFW->scrollCallback(0, -1);

                REQUIRE(slider->getValue() == 5);

                // The mouse leaving the window will remove the hover state of widgets
                unsigned int mouseLeftCount = 0;
                backendGuiGLFW->cursorPosCallback(260, 80);
                slider->onMouseLeave([&]{ genericCallback(mouseLeftCount); });
                backendGuiGLFW->cursorEnterCallback(false);
                REQUIRE(mouseLeftCount == 1);

                // Resize the child window using mouse events (decrease width with 20px)
                backendGuiGLFW->cursorPosCallback(320, 100);
                backendGuiGLFW->mouseButtonCallback(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0);
                backendGuiGLFW->cursorPosCallback(300, 105);
                backendGuiGLFW->mouseButtonCallback(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE, 0);

                // GLFW currently doesn't support touch events, so we can't test resizing by touch
                backendGuiGLFW->cursorPosCallback(100, 260);
                backendGuiGLFW->mouseButtonCallback(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0);
                backendGuiGLFW->cursorPosCallback(110, 270);
                backendGuiGLFW->mouseButtonCallback(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE, 0);

                REQUIRE(childWindow->getSize() == tgui::Vector2f(280, 260));

                globalGui->removeAllWidgets();
            }
        }
    }
#endif

#if TGUI_HAS_WINDOW_BACKEND_RAYLIB
    auto backendGuiRaylib = dynamic_cast<tgui::BackendGuiRaylib*>(globalGui);
    if (backendGuiRaylib)
    {
        SECTION("Raylib")
        {
            struct RaylibTestWidget : public tgui::ClickableWidget
            {
            public:
                tgui::Event::KeyboardKey expectedKeyCode;
                char32_t expectedTextChar;
                unsigned int nrKeyPressedCallbacks = 0;
                unsigned int nrTextEnteredCallbacks = 0;

            private:
                void keyPressed(const tgui::Event::KeyEvent& event) override
                {
                    REQUIRE(event.code == expectedKeyCode);
                    ++nrKeyPressedCallbacks;
                }

                void textEntered(char32_t key) override
                {
                    REQUIRE(key == expectedTextChar);
                    ++nrTextEnteredCallbacks;
                }
            };

            auto testWidget = std::make_shared<RaylibTestWidget>();
            globalGui->add(testWidget);
            testWidget->setFocused(true);

            SECTION("KeyPressed")
            {
                SECTION("All key codes")
                {
                    std::array<std::pair<int32_t, tgui::Event::KeyboardKey>, 98> keys = {{
                        {KEY_A,             tgui::Event::KeyboardKey::A},
                        {KEY_B,             tgui::Event::KeyboardKey::B},
                        {KEY_C,             tgui::Event::KeyboardKey::C},
                        {KEY_D,             tgui::Event::KeyboardKey::D},
                        {KEY_E,             tgui::Event::KeyboardKey::E},
                        {KEY_F,             tgui::Event::KeyboardKey::F},
                        {KEY_G,             tgui::Event::KeyboardKey::G},
                        {KEY_H,             tgui::Event::KeyboardKey::H},
                        {KEY_I,             tgui::Event::KeyboardKey::I},
                        {KEY_J,             tgui::Event::KeyboardKey::J},
                        {KEY_K,             tgui::Event::KeyboardKey::K},
                        {KEY_L,             tgui::Event::KeyboardKey::L},
                        {KEY_M,             tgui::Event::KeyboardKey::M},
                        {KEY_N,             tgui::Event::KeyboardKey::N},
                        {KEY_O,             tgui::Event::KeyboardKey::O},
                        {KEY_P,             tgui::Event::KeyboardKey::P},
                        {KEY_Q,             tgui::Event::KeyboardKey::Q},
                        {KEY_R,             tgui::Event::KeyboardKey::R},
                        {KEY_S,             tgui::Event::KeyboardKey::S},
                        {KEY_T,             tgui::Event::KeyboardKey::T},
                        {KEY_U,             tgui::Event::KeyboardKey::U},
                        {KEY_V,             tgui::Event::KeyboardKey::V},
                        {KEY_W,             tgui::Event::KeyboardKey::W},
                        {KEY_X,             tgui::Event::KeyboardKey::X},
                        {KEY_Y,             tgui::Event::KeyboardKey::Y},
                        {KEY_Z,             tgui::Event::KeyboardKey::Z},
                        {KEY_ZERO,          tgui::Event::KeyboardKey::Num0},
                        {KEY_ONE,           tgui::Event::KeyboardKey::Num1},
                        {KEY_TWO,           tgui::Event::KeyboardKey::Num2},
                        {KEY_THREE,         tgui::Event::KeyboardKey::Num3},
                        {KEY_FOUR,          tgui::Event::KeyboardKey::Num4},
                        {KEY_FIVE,          tgui::Event::KeyboardKey::Num5},
                        {KEY_SIX,           tgui::Event::KeyboardKey::Num6},
                        {KEY_SEVEN,         tgui::Event::KeyboardKey::Num7},
                        {KEY_EIGHT,         tgui::Event::KeyboardKey::Num8},
                        {KEY_NINE,          tgui::Event::KeyboardKey::Num9},
                        {KEY_ESCAPE,        tgui::Event::KeyboardKey::Escape},
                        {KEY_LEFT_CONTROL,  tgui::Event::KeyboardKey::LControl},
                        {KEY_LEFT_SHIFT,    tgui::Event::KeyboardKey::LShift},
                        {KEY_LEFT_ALT,      tgui::Event::KeyboardKey::LAlt},
                        {KEY_LEFT_SUPER,    tgui::Event::KeyboardKey::LSystem},
                        {KEY_RIGHT_CONTROL, tgui::Event::KeyboardKey::RControl},
                        {KEY_RIGHT_SHIFT,   tgui::Event::KeyboardKey::RShift},
                        {KEY_RIGHT_ALT,     tgui::Event::KeyboardKey::RAlt},
                        {KEY_RIGHT_SUPER,   tgui::Event::KeyboardKey::RSystem},
                        {KEY_LEFT_BRACKET,  tgui::Event::KeyboardKey::LBracket},
                        {KEY_RIGHT_BRACKET, tgui::Event::KeyboardKey::RBracket},
                        {KEY_SEMICOLON,     tgui::Event::KeyboardKey::Semicolon},
                        {KEY_COMMA,         tgui::Event::KeyboardKey::Comma},
                        {KEY_PERIOD,        tgui::Event::KeyboardKey::Period},
                        {KEY_APOSTROPHE,    tgui::Event::KeyboardKey::Quote},
                        {KEY_SLASH,         tgui::Event::KeyboardKey::Slash},
                        {KEY_BACKSLASH,     tgui::Event::KeyboardKey::Backslash},
                        {KEY_EQUAL,         tgui::Event::KeyboardKey::Equal},
                        {KEY_MINUS,         tgui::Event::KeyboardKey::Minus},
                        {KEY_SPACE,         tgui::Event::KeyboardKey::Space},
                        {KEY_ENTER,         tgui::Event::KeyboardKey::Enter},
                        {KEY_BACKSPACE,     tgui::Event::KeyboardKey::Backspace},
                        {KEY_TAB,           tgui::Event::KeyboardKey::Tab},
                        {KEY_PAGE_UP,       tgui::Event::KeyboardKey::PageUp},
                        {KEY_PAGE_DOWN,     tgui::Event::KeyboardKey::PageDown},
                        {KEY_END,           tgui::Event::KeyboardKey::End},
                        {KEY_HOME,          tgui::Event::KeyboardKey::Home},
                        {KEY_INSERT,        tgui::Event::KeyboardKey::Insert},
                        {KEY_DELETE,        tgui::Event::KeyboardKey::Delete},
                        {KEY_KP_ADD,        tgui::Event::KeyboardKey::Add},
                        {KEY_KP_SUBTRACT,   tgui::Event::KeyboardKey::Subtract},
                        {KEY_KP_MULTIPLY,   tgui::Event::KeyboardKey::Multiply},
                        {KEY_KP_DIVIDE,     tgui::Event::KeyboardKey::Divide},
                        {KEY_KP_DECIMAL,    tgui::Event::KeyboardKey::Period},
                        {KEY_KP_EQUAL,      tgui::Event::KeyboardKey::Equal},
                        {KEY_LEFT,          tgui::Event::KeyboardKey::Left},
                        {KEY_RIGHT,         tgui::Event::KeyboardKey::Right},
                        {KEY_UP,            tgui::Event::KeyboardKey::Up},
                        {KEY_DOWN,          tgui::Event::KeyboardKey::Down},
                        {KEY_KP_0,          tgui::Event::KeyboardKey::Numpad0},
                        {KEY_KP_1,          tgui::Event::KeyboardKey::Numpad1},
                        {KEY_KP_2,          tgui::Event::KeyboardKey::Numpad2},
                        {KEY_KP_3,          tgui::Event::KeyboardKey::Numpad3},
                        {KEY_KP_4,          tgui::Event::KeyboardKey::Numpad4},
                        {KEY_KP_5,          tgui::Event::KeyboardKey::Numpad5},
                        {KEY_KP_6,          tgui::Event::KeyboardKey::Numpad6},
                        {KEY_KP_7,          tgui::Event::KeyboardKey::Numpad7},
                        {KEY_KP_8,          tgui::Event::KeyboardKey::Numpad8},
                        {KEY_KP_9,          tgui::Event::KeyboardKey::Numpad9},
                        {KEY_F1,            tgui::Event::KeyboardKey::F1},
                        {KEY_F2,            tgui::Event::KeyboardKey::F2},
                        {KEY_F3,            tgui::Event::KeyboardKey::F3},
                        {KEY_F4,            tgui::Event::KeyboardKey::F4},
                        {KEY_F5,            tgui::Event::KeyboardKey::F5},
                        {KEY_F6,            tgui::Event::KeyboardKey::F6},
                        {KEY_F7,            tgui::Event::KeyboardKey::F7},
                        {KEY_F8,            tgui::Event::KeyboardKey::F8},
                        {KEY_F9,            tgui::Event::KeyboardKey::F9},
                        {KEY_F10,           tgui::Event::KeyboardKey::F10},
                        {KEY_F11,           tgui::Event::KeyboardKey::F11},
                        {KEY_F12,           tgui::Event::KeyboardKey::F12},
                        {KEY_PAUSE,         tgui::Event::KeyboardKey::Pause}
                    }};

                    unsigned int nrKeysRequiredHandling = 0;
                    for (auto pair : keys)
                    {
                        bool requiresHandling = true;
                        if ((pair.first == KEY_LEFT_ALT) || (pair.first == KEY_RIGHT_ALT)
                         || (pair.first == KEY_LEFT_CONTROL) || (pair.first == KEY_RIGHT_CONTROL)
                         || (pair.first == KEY_LEFT_SHIFT) || (pair.first == KEY_RIGHT_SHIFT)
                         || (pair.first == KEY_LEFT_SUPER) || (pair.first == KEY_RIGHT_SUPER))
                        {
                            requiresHandling = false;
                        }

                        testWidget->expectedKeyCode = pair.second;
                        if (requiresHandling)
                        {
                            if (pair.first != KEY_TAB)
                                ++nrKeysRequiredHandling;

                            REQUIRE(backendGuiRaylib->handleKeyPressed(pair.first));
                        }
                        else
                        {
                            REQUIRE(!backendGuiRaylib->handleKeyPressed(pair.first));
                        }
                    }

                    REQUIRE(testWidget->nrKeyPressedCallbacks == nrKeysRequiredHandling);
                }

                SECTION("Invalid key code")
                {
                    REQUIRE(!backendGuiRaylib->handleKeyPressed(KEY_NULL));
                    REQUIRE(!backendGuiRaylib->handleKeyPressed(-1));
                    REQUIRE(!backendGuiRaylib->handleKeyPressed(5000));

                    REQUIRE(testWidget->nrKeyPressedCallbacks == 0);
                }
            }

            SECTION("TextEntered")
            {
                testWidget->expectedTextChar = 0x2705;
                REQUIRE(backendGuiRaylib->handleCharPressed(0x2705));
                REQUIRE(!backendGuiRaylib->handleCharPressed(-1));

                REQUIRE(testWidget->nrTextEnteredCallbacks == 1);
            }

            globalGui->remove(testWidget);
        }
    }
#endif
}
