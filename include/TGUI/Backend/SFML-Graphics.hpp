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

#ifndef TGUI_BACKEND_SFML_GRAPHICS_INCLUDE_HPP
#define TGUI_BACKEND_SFML_GRAPHICS_INCLUDE_HPP

#include <TGUI/Config.hpp>
#if !TGUI_HAS_BACKEND_SFML_GRAPHICS
    #error "TGUI wasn't build with the SFML_GRAPHICS backend"
#endif

#if TGUI_BUILD_AS_CXX_MODULE
    import tgui.backend.window.sfml;
    import tgui.backend.renderer.sfml_graphics;
    import tgui.backend.font.sfml_graphics;
#else
    #include <TGUI/Backend/Window/SFML/BackendSFML.hpp>
    #include <TGUI/Backend/Renderer/SFML-Graphics/BackendRendererSFML.hpp>
    #include <TGUI/Backend/Font/SFML-Graphics/BackendFontSFML.hpp>
#endif

#include <SFML/Graphics.hpp>

#ifdef TGUI_SYSTEM_IOS
  #include <SFML/Main.hpp>
#endif

TGUI_MODULE_EXPORT namespace tgui
{
    inline namespace SFML_GRAPHICS
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Gui class for SFML (using sfml-graphics module for rendering and font)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        class TGUI_API Gui : public BackendGuiSFML
        {
        public:

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Default constructor
            ///
            /// @warning You must still call setWindow (or setTarget) on the Gui before using any TGUI functions
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            Gui() = default;


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Constructor that immediately sets the window on which the gui should be drawn
            ///
            /// @param window  The SFML window that will be used by the gui
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            Gui(sf::RenderWindow& window)
            {
                setWindow(window);
            }


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Constructor that immediately sets the render target on which the gui should be drawn
            ///
            /// @param target  The SFML render target that will be used by the gui
            ///
            /// @warning If the target isn't an sf::RenderWindow (which it isn't if this constructor is called) then window-related
            ///          functionality such as changing the mouse pointer won't work. You can still call setWindow later with a
            ///          regular sf::Window (not an sf::RenderWindow) to enable this functionality while still rendering to the
            ///          target that is passed here.
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            Gui(sf::RenderTarget& target)
            {
                setTarget(target);
            }


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the window on which the gui should be drawn
            ///
            /// @param window  The SFML window that will be used by the gui
            ///
            /// Calling setWindow or setTarget with an sf::RenderWindow does the exact same thing.
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void setWindow(sf::RenderWindow& window);


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the window which the gui should use
            ///
            /// @param window  The SFML window that will be used by the gui
            ///
            /// @warning If the window isn't an sf::RenderWindow (which it isn't if this function overload is called) then the window
            ///          can't be used to render on and is only used for enabling window-specific functionality such as changing the
            ///          mouse cursor. You still have to call setTarget to tell the gui what to render on.
            /// @warning This function overload shouldn't be used when passing a sf::RenderWindow to setTarget.
            ///
            /// @see setWindow(sf::RenderWindow)
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void setWindow(sf::Window& window);


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the window on which the gui should be drawn
            ///
            /// @param window  The SFML window that will be used by the gui
            ///
            /// Calling setWindow or setTarget with an sf::RenderWindow does the exact same thing.
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void setTarget(sf::RenderWindow& window);


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Sets the target on which the gui should be drawn
            ///
            /// @param target  The SFML render target that will be used by the gui
            ///
            /// @warning If the target isn't an sf::RenderWindow (which it isn't if this function overload is called) then
            ///          window-related functionality such as changing the mouse pointer won't work. You can still call setWindow
            ///          later with a regular sf::Window (not an sf::RenderWindow) to enable this functionality while still
            ///          rendering to the target that is passed here.
            ///
            /// @see setTarget(sf::RenderWindow)
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void setTarget(sf::RenderTarget& target);


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            /// @brief Returns the render target on which the gui is being drawn
            ///
            /// @return The sfml render target that is used by the gui
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            TGUI_NODISCARD sf::RenderTarget* getTarget() const;


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        protected:

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Updates the view and changes the size of the root container when needed
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            void updateContainerSize() override;


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        private:

            sf::RenderTarget* m_target = nullptr;
        };
    }
}

#endif // TGUI_BACKEND_SFML_GRAPHICS_INCLUDE_HPP
