/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_TEXTURE_MANAGER_HPP
#define TGUI_TEXTURE_MANAGER_HPP

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <list>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API TextureManager : public sf::NonCopyable
    {
      public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief This will load a texture from a file an return it.
        ///
        /// \param filename       The filename of the image to load
        /// \param textureToLoad  When the function returns true, this pointer will point to the texture.
        ///                       If the texture was already loaded then the pointer will point to the earlier loaded texture.
        ///
        /// \return
        ///         - true when the image was loaded successfully (or already loaded before)
        ///         - false when the image couldn't be loaded (probalby file not found)
        ///
        /// \remarks
        ///         - You MUST call removeTexture when you don"t need the texture anymore (ONLY when function returned true).
        ///         - You may NEVER delete the pointer directly.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool getTexture(const std::string& filename, sf::Texture*& textureToLoad);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Tell the event manager that the texture is now also used somewhere else and may thus not be deleted if only one of the places is done with it.
        ///
        /// \param textureToCopy  Pointer to an earlier loaded texture that should now also be accessed somewhere else
        /// \param newTexture     When \a textureToCopy was loaded by TextureManager (and not yet removed), then \a newTexture will point to the same texture as \a textureToCopy.
        ///                       Otherwise, when the texture doesn't exist inside TextureManager, \a newTexture will be set to NULL.
        ///
        /// \return
        ///         - true when the texture was loaded before by TextureManager
        ///         - false when the texture was never loaded (or already removed) by TextureManager
        ///
        /// \remarks
        ///         - You MUST call removeTexture when you don"t need the texture anymore (ONLY when function returned true).
        ///         - You may NEVER delete the pointer directly.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool copyTexture(const sf::Texture* const textureToCopy, sf::Texture*& newTexture);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief When the texture is no longer needed then this function is called.
        ///
        /// \param textureToRemove  Pointer to the texture that should be removed.
        ///                         When the same texture is still in use somewhere else then the texture will not be deleted.
        ///
        /// \remarks For EVERY call to getTexture or copyTexture (that returned true) this function MUST be called.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void removeTexture(sf::Texture*& textureToRemove);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Checks if the color on the given position is transparent.
        ///
        /// \param texture  Pointer to an earlier loaded texture of which you want to check the pixel data
        /// \param x        The x position of the pixel on the image
        /// \param y        The y position of the pixel on the image
        ///
        /// \return
        ///        - true when the pixel was transparent (alpha component in color is 0)
        ///        - false when the pixel wasn't transparent
        ///        - false when the texture was not in the list (not loaded by the texture manager or already removed)
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool isTransparentPixel(const sf::Texture* const texture, unsigned int x, unsigned int y);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      protected:

        struct TextureData
        {
            std::string   filename;   // The filenames of the texture
            sf::Image     image;      // The SFML images
            sf::Texture   texture;    // The SFML textures
            unsigned int  users;      // The number of times this texture is used
        };

        std::list<TextureData> m_Data;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_TEXTURE_MANAGER_HPP
