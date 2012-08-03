/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (VDV_B@hotmail.com)
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


#ifndef _TGUI_TEXTURE_MANAGER_INCLUDED_
#define _TGUI_TEXTURE_MANAGER_INCLUDED_

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <list>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    struct TGUI_API TextureManager : public sf::NonCopyable
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief This will load a texture from a file an return it.
        ///
        /// If the texture was already loaded then it won't be loaded again and this function just returns the same texture.
        ///
        /// \remarks You MUST call removeTexture when you don"t need the texture anymore (ONLY when function returned true).
        ///
        /// \return
        ///         - true when the image was loaded successfully
        ///         - false when the image couldn't be loaded (probalby file not found)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool getTexture(const std::string filename, sf::Texture*& textureToLoad);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Tell the event manager that the texture is now also used somewhere else and may thus not be deleted if only one of the places is done with it.
        ///
        /// Regardless of what the function returns, NewTexture will be the same as TextureToCopy.
        ///
        /// \remarks You MUST call removeTexture when you don"t need the texture anymore (ONLY when function returned true).
        ///
        /// \return
        ///         - true when the texture was loaded before by TextureManager
        ///         - false when the texture was never loaded (or already removed) by TextureManager
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool copyTexture(sf::Texture* textureToCopy, sf::Texture*& newTexture);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief When the texture is no longer needed then this function is called.
        ///
        /// When the same texture is still in use somewhere else then the texture will not be deleted.
        ///
        /// \remarks For EVERY call to getTexture or copyTexture (that returned true) this function MUST be called.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void removeTexture(sf::Texture*& textureToRemove);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        std::vector<std::string>   m_Filenames;  // The filenames of the texture
        std::vector<sf::Image>     m_Images;     // The SFML images
        std::list<sf::Texture>     m_Textures;   // The SFML textures
        std::vector<unsigned int>  m_Users;      // The number of times this texture is used
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //_TGUI_TEXTURE_MANAGER_INCLUDED_
