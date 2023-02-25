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


#ifndef TGUI_TRANSFORM_HPP
#define TGUI_TRANSFORM_HPP

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/Config.hpp>
#include <TGUI/Rect.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <array>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Defines a transform matrix
    // Based on sf::Transform from SFML
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API Transform
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default constructor
        ///
        /// Creates an identity transform (a transform that does nothing).
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Transform();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Construct a transform from a 3x3 matrix
        ///
        /// @param a00  Element (0, 0) of the 3x3 matrix
        /// @param a01  Element (0, 1) of the 3x3 matrix
        /// @param a02  Element (0, 2) of the 3x3 matrix
        /// @param a10  Element (1, 0) of the 3x3 matrix
        /// @param a11  Element (1, 1) of the 3x3 matrix
        /// @param a12  Element (1, 2) of the 3x3 matrix
        /// @param a20  Element (2, 0) of the 3x3 matrix
        /// @param a21  Element (2, 1) of the 3x3 matrix
        /// @param a22  Element (2, 2) of the 3x3 matrix
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Transform(float a00, float a01, float a02,
                  float a10, float a11, float a12,
                  float a20, float a21, float a22);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructs a transform from a 4x4 matrix
        ///
        /// @param matrix  4x4 transform matrix, similar to what getMatrix returns
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Transform(const std::array<float, 16>& matrix);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Return the transform as a 4x4 matrix
        ///
        /// This function returns an array of 16 floats containing the transform elements as a 4x4 matrix, which is
        /// directly compatible with OpenGL functions.
        ///
        /// @code
        /// glLoadMatrixf(transform.getMatrix().data());
        /// @endcode
        ///
        /// @return 4x4 transform matrix
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD const std::array<float, 16>& getMatrix() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Return the inverse of the transform
        ///
        /// @return A new transform which is the inverse of this transform
        ///
        /// If the inverse cannot be computed, an identity transform is returned.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Transform getInverse() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Transform a 2D point
        ///
        /// @param point  Point to transform
        ///
        /// @return Transformed point
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Vector2f transformPoint(const Vector2f& point) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Transform a rectangle
        ///
        /// If the transform contains a rotation, the bounding rectangle of the transformed rectangle is returned.
        ///
        /// @param rectangle Rectangle to transform
        ///
        /// @return Transformed rectangle
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD FloatRect transformRect(const FloatRect& rectangle) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Combine the current transform with another one
        ///
        /// The result is a transform that is equivalent to applying the other transform followed by this transform.
        /// Mathematically, it is equivalent to a matrix multiplication (*this) * other.
        ///
        /// @param other  Transform to combine with this transform
        ///
        /// @return Reference to *this
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Transform& combine(const Transform& other);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Combine the current transform with a translation
        ///
        /// @param offset  Translation offset to apply
        ///
        /// @return Reference to *this
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Transform& translate(const Vector2f& offset);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Combine the current transform with a rotation
        ///
        /// The center of rotation is provided for convenience as a second argument, so that you can build rotations around
        /// arbitrary points more easily (and efficiently) than the usual translate(-center).rotate(angle).translate(center).
        ///
        /// @param angle  Rotation angle, in degrees
        /// @param center Center of rotation
        ///
        /// @return Reference to *this
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Transform& rotate(float angle, const Vector2f& center = {0, 0});


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Combine the current transform with a scaling
        ///
        /// The center of scaling is provided for convenience as a second argument, so that you can build scaling around
        /// arbitrary points more easily (and efficiently) than the usual translate(-center).scale(factors).translate(center).
        ///
        /// @param factors Scaling factors
        /// @param center Center of scaling
        ///
        /// @return Reference to *this
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Transform& scale(const Vector2f& factors, const Vector2f& center = {0, 0});


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Overload of binary operator * to combine two transforms
        ///
        /// @param right  Transform to combine with a copy of this transform
        ///
        /// @see combine
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD Transform operator *(const Transform& right) const;
        Transform& operator *=(const Transform& right);
        TGUI_NODISCARD Vector2f operator *(const Vector2f& right) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Rounds the position stored in the transform to the nearest pixel
        ///
        /// @param pixelScaleX  Pixels per point horizontally
        /// @param pixelScaleY  Pixels per point vertically
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void roundPosition(float pixelScaleX, float pixelScaleY);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        std::array<float, 16> m_matrix; //!< 4x4 matrix defining the transformation
    };
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_TRANSFORM_HPP
