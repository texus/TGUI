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

#include <TGUI/Backend/SFML-Graphics.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    inline namespace SFML_GRAPHICS
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Gui::setWindow(sf::RenderWindow& renderWindow)
        {
            setWindow(static_cast<sf::Window&>(renderWindow));
            setTarget(static_cast<sf::RenderTarget&>(renderWindow));
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Gui::setWindow(sf::Window& window)
        {
            // If we don't have a target to render to yet then just store the window for later
            if (!m_target)
            {
                m_window = &window;
                return;
            }

            setGuiWindow(window);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Gui::setTarget(sf::RenderWindow& window)
        {
            setWindow(window);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Gui::setTarget(sf::RenderTarget& target)
        {
            m_target = &target;

            if (!isBackendSet())
            {
                auto backend = std::make_shared<BackendSFML>();
                backend->setFontBackend(std::make_shared<BackendFontFactoryImpl<BackendFontSFML>>());
                backend->setRenderer(std::make_shared<BackendRendererSFML>());
                backend->setDestroyOnLastGuiDetatch(true);
                setBackend(backend);
            }

            m_backendRenderTarget = std::make_shared<BackendRenderTargetSFML>(target);

            if (m_window)
                setGuiWindow(*m_window);
            else
            {
                // We don't have a window, so do the same as BackendGuiSFML::setWindow would have done but without associating
                // the gui with a window.
                getBackend()->attachGui(this);
                updateContainerSize();
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        sf::RenderTarget* Gui::getTarget() const
        {
            return m_target;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Gui::updateContainerSize()
        {
            // We can't do anything yet if we don't have a target to render to
            if (!m_target)
                return;

            m_framebufferSize = {static_cast<int>(m_target->getSize().x), static_cast<int>(m_target->getSize().y)};

            // Call the base function directly on BackendGui and not BackendGuiSFML!
            // NOLINTNEXTLINE(bugprone-parent-virtual-call)
            BackendGui::updateContainerSize();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
