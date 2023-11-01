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

#ifndef TGUI_TEXTURE_HPP
#define TGUI_TEXTURE_HPP

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/TextureData.hpp>
#include <TGUI/Vector2.hpp>
#include <TGUI/String.hpp>
#include <TGUI/Global.hpp>
#include <TGUI/Color.hpp>
#include <TGUI/Rect.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <functional>
#endif

#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS && !TGUI_BUILD_AS_CXX_MODULE
    namespace sf
    {
        class Texture;
        class Shader;
    }
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Texture wrapper that internally reuses resources when multiple Texture objects are loaded from the same file
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API Texture
    {
    public:

        using CallbackFunc = std::function<void(std::shared_ptr<TextureData>)>;
        using BackendTextureLoaderFunc = std::function<bool(BackendTexture&, const String&, bool smooth)>;
        using TextureLoaderFunc = std::function<std::shared_ptr<TextureData>(Texture&, const String&, bool smooth)>;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default constructor
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Texture() = default;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor that created the texture
        ///
        /// @param id         Id for the the image to load (for the default loader, the id is the filename)
        /// @param partRect   Load only part of the image. Pass an empty rectangle if you want to load the full image
        /// @param middlePart Choose the middle part of the image for 9-slice scaling (relative to the part defined by partRect)
        /// @param smooth     Enable smoothing on the texture
        ///
        /// @throw Exception when loading failed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Texture(const char* id,
                const UIntRect& partRect = UIntRect(0, 0, 0, 0),
                const UIntRect& middlePart = UIntRect(0, 0, 0, 0),
                bool smooth = m_defaultSmooth)
            : Texture(String{id}, partRect, middlePart, smooth)
        {
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor that created the texture
        ///
        /// @param id         Id for the the image to load (for the default loader, the id is the filename)
        /// @param partRect   Load only part of the image. Pass an empty rectangle if you want to load the full image
        /// @param middlePart Choose the middle part of the image for 9-slice scaling (relative to the part defined by partRect)
        /// @param smooth     Enable smoothing on the texture
        ///
        /// This constructor just calls the corresponding load function.
        ///
        /// @throw Exception when loading failed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Texture(const String& id,
                const UIntRect& partRect = UIntRect(0, 0, 0, 0),
                const UIntRect& middlePart = UIntRect(0, 0, 0, 0),
                bool smooth = m_defaultSmooth);

#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS && !defined(TGUI_REMOVE_DEPRECATED_CODE)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor that created the texture from an existing sf::Texture
        ///
        /// @param texture    Existing texture to copy
        /// @param partRect   Load only part of the image. Pass an empty rectangle if you want to load the full image
        /// @param middlePart Choose the middle part of the image for 9-slice scaling (relative to the part defined by partRect)
        ///
        /// The texture will be copied, you do not have to keep the sf::Texture alive after calling this function.
        ///
        /// This constructor just calls the corresponding load function.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_DEPRECATED("Use Texture() and loadFromPixelData(texture.getSize(), texture.copyToImage().getPixelsPtr()) instead")
        Texture(const sf::Texture& texture,
                const UIntRect& partRect = UIntRect(0, 0, 0, 0),
                const UIntRect& middlePart = UIntRect(0, 0, 0, 0));
#endif
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Copy constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Texture(const Texture&);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Move constructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Texture(Texture&&) noexcept;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Destructor
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ~Texture();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Overload of copy assignment operator
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Texture& operator=(const Texture&);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Move assignment
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Texture& operator=(Texture&&) noexcept;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates the texture
        ///
        /// @param id         Id for the the image to load (for the default loader, the id is the filename)
        /// @param partRect   Load only part of the image. Don't pass this parameter if you want to load the full image
        /// @param middleRect Choose the middle part of the image for 9-slice scaling (relative to the part defined by partRect)
        /// @param smooth     Enable smoothing on the texture
        ///
        /// @throw Exception when loading failed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void load(const String& id,
                  const UIntRect& partRect = {},
                  const UIntRect& middleRect = {},
                  bool smooth = m_defaultSmooth);

#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS && !defined(TGUI_REMOVE_DEPRECATED_CODE)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Creates the texture from an existing sf::Texture
        ///
        /// @param texture    Existing texture to copy
        /// @param partRect   Load only part of the texture. Don't pass this parameter if you want to load the full image
        /// @param middleRect Choose the middle part of the image for 9-slice scaling (relative to the part defined by partRect)
        ///
        /// The texture will be copied, you do not have to keep the sf::Texture alive after calling this function.
        ///
        /// @throw Exception when loading failed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_DEPRECATED("Use loadFromPixelData(texture.getSize(), texture.copyToImage().getPixelsPtr()) instead")
        void load(const sf::Texture& texture,
                  const UIntRect& partRect = {},
                  const UIntRect& middleRect = {});
#endif
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Loads the texture from memory (data in memory should contain the entire file, not just the pixels)
        ///
        /// @param data       Pointer to the file in memory
        /// @param dataSize   Amount of bytes of the file in memory
        /// @param partRect   Load only part of the texture. Don't pass this parameter if you want to load the full image
        /// @param middleRect Choose the middle part of the image for 9-slice scaling (relative to the part defined by partRect)
        /// @param smooth     Enable smoothing on the texture
        ///
        /// @throw Exception when loading failed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void loadFromMemory(const std::uint8_t* data, std::size_t dataSize, const UIntRect& partRect = {}, const UIntRect& middleRect = {}, bool smooth = m_defaultSmooth);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Loads the texture from an array of 32-bits RGBA pixels
        ///
        /// @param size       Width and height of the image to create
        /// @param pixels     Pointer to array of size.x*size.y*4 bytes with RGBA pixels
        /// @param partRect   Load only part of the texture. Don't pass this parameter if you want to load the full image
        /// @param middleRect Choose the middle part of the image for 9-slice scaling (relative to the part defined by partRect)
        /// @param smooth     Enable smoothing on the texture
        ///
        /// The pixels are copied by this function, you can free the buffer after calling this function.
        ///
        /// @throw Exception when loading failed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void loadFromPixelData(Vector2u size, const std::uint8_t* pixels, const UIntRect& partRect = {}, const UIntRect& middleRect = {}, bool smooth = m_defaultSmooth);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Loads the texture from a base64 string
        ///
        /// @param imageAsBase64  Character array with the base64-encoded image (the entire file, not just the pixel data)
        /// @param partRect       Load only part of the texture. Don't pass this parameter if you want to load the full image
        /// @param middleRect     Choose the middle part of the image for 9-slice scaling (relative to the part defined by partRect)
        /// @param smooth         Enable smoothing on the texture
        ///
        /// Note that imageAsBase64 is just the base64 data (of the entire file), it should NOT start with a prefix like "data:".
        ///
        /// @throw Exception when loading failed
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void loadFromBase64(CharStringView imageAsBase64, const UIntRect& partRect = {}, const UIntRect& middleRect = {}, bool smooth = m_defaultSmooth);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the id that was used to load the texture (for the default loader, the id is the filename)
        ///
        /// @return Id of the texture
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD const String& getId() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the texture data
        ///
        /// @return Data of the texture
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD std::shared_ptr<TextureData> getData() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the size that the loaded image, or the size of the part if only a part of the image is loaded
        ///
        /// @return Size of the image like it was when loaded (no scaling applied)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Vector2u getImageSize() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns which part of the image was loaded
        ///
        /// @return Part of the image that was loaded
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD UIntRect getPartRect() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Tells whether the smooth filter is enabled or not
        ///
        /// @return True if smoothing is enabled, false if it is disabled
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD bool isSmooth() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the global color of the texture
        ///
        /// This color is modulated (multiplied) with the pixel color. It can be used to colorize the texture,
        /// or change its global opacity. Note that the alpha component is multiplied with the opacity of the widget.
        ///
        /// By default, the texture's color is opaque white.
        ///
        /// @param color  New color of the texture
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setColor(const Color& color);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the global color of the texture
        ///
        /// This color is modulated (multiplied) with the pixel color. It can be used to colorize the texture,
        /// or change its global opacity. Note that the alpha component is multiplied with the opacity of the widget.
        ///
        /// By default, the texture's color is opaque white.
        ///
        /// @return Current color of the texture
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD const Color& getColor() const;

#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the shader used to draw the texture
        /// @param shader  New shader to use
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setShader(sf::Shader* shader);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the shader used to draw the texture
        /// @return Shader currently being use to draw the texture
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD sf::Shader* getShader() const;
#endif
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets the middle part of the image for 9-slice scaling (relative to the part defined by partRect)
        ///
        /// @param middleRect  Middle rect of the texture
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setMiddleRect(const UIntRect& middleRect);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the middle rect of the texture which is used for 9-slice scaling
        ///
        /// @return Middle rect of the texture
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD UIntRect getMiddleRect() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Checks if a certain pixel is transparent
        ///
        /// @param pos  Coordinate of the pixel
        ///
        /// @return True when the pixel is transparent, false when it is not
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD bool isTransparentPixel(Vector2u pos) const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets a callback function for when this texture is copied
        ///
        /// @param func  Function that will be called when this texture is copied
        ///
        /// This function can be useful when implementing a resource manager.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setCopyCallback(const CallbackFunc& func);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets a callback function for when this texture is destroyed
        ///
        /// @param func  Function that will be called when this texture is destroyed
        ///
        /// This function can be useful when implementing a resource manager.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setDestructCallback(const CallbackFunc& func);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Compares the texture with another one
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD bool operator==(const Texture& right) const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Compares the texture with another one
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD bool operator!=(const Texture& right) const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes whether textures are smoothed by default or not
        ///
        /// @param smooth  Enable smoothing on the texture by default
        ///
        /// Smoothed textures are rendered with bilinear interpolation (default). Turning smoothing off will switch rendering to
        /// nearest neighbor interpolation.
        ///
        /// Changing the default only affects new textures where smoothing isn't explicitly set. No existing textures are modified.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void setDefaultSmooth(bool smooth);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns whether textures are smoothed by default or not
        ///
        /// @return True if smoothing is enabled by default, false if it is disabled
        ///
        /// Smoothed textures are rendered with bilinear interpolation (default). Turning smoothing off will switch rendering to
        /// nearest neighbor interpolation.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD static bool getDefaultSmooth();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets a different backend texture loader
        ///
        /// @param func  New backend texture loader function
        ///
        /// The backend texture loader will be called inside the texture loader to create the backend texture.
        ///
        /// The default loader will simply load the image from a file.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void setBackendTextureLoader(const BackendTextureLoaderFunc& func);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the used backend texture loader
        ///
        /// @return Backend texture loader that is currently being used
        ///
        /// @see setBackendTextureLoader
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD static const BackendTextureLoaderFunc& getBackendTextureLoader();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Sets a different texture loader
        ///
        /// @param func  New texture loader function
        ///
        /// The texture loader will initialize this Texture object.
        ///
        /// The default loader will use an internal texture manager to prevent the same thing from being loaded twice.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void setTextureLoader(const TextureLoaderFunc& func);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the used texture loader
        ///
        /// @return Texture loader that is currently being used
        ///
        /// @see setTextureLoader
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD static const TextureLoaderFunc& getTextureLoader();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Changes the texture data
        ///
        /// @param data       New texture data
        /// @param partRect   Load only part of the image
        /// @param middleRect Choose the middle part of the image part to determine scaling (e.g. 9-slice scaling)
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setTextureData(std::shared_ptr<TextureData> data, const UIntRect& partRect, const UIntRect& middleRect);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

#if TGUI_HAS_RENDERER_BACKEND_SFML_GRAPHICS
        sf::Shader* m_shader = nullptr;
#endif

        std::shared_ptr<TextureData> m_data = nullptr;
        Color m_color = Color::White;

        UIntRect m_partRect;
        UIntRect m_middleRect;
        String  m_id;

        CallbackFunc m_copyCallback;
        CallbackFunc m_destructCallback;

        static bool m_defaultSmooth;

        static TextureLoaderFunc m_textureLoader;
        static BackendTextureLoaderFunc m_backendTextureLoader;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_TEXTURE_HPP
