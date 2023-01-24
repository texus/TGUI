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


#include <TGUI/Transform.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <algorithm>
    #include <cmath>

    #if defined(__cpp_lib_math_constants) && (__cpp_lib_math_constants >= 201907L)
        #include <numbers>
    #endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Transform::Transform() :
         Transform(1, 0, 0, 0, 1, 0, 0, 0, 1)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Transform::Transform(float a00, float a01, float a02,
              float a10, float a11, float a12,
              float a20, float a21, float a22)
    {
        m_matrix[0] = a00; m_matrix[4] = a01; m_matrix[8]  = 0.f; m_matrix[12] = a02;
        m_matrix[1] = a10; m_matrix[5] = a11; m_matrix[9]  = 0.f; m_matrix[13] = a12;
        m_matrix[2] = 0.f; m_matrix[6] = 0.f; m_matrix[10] = 1.f; m_matrix[14] = 0.f;
        m_matrix[3] = a20; m_matrix[7] = a21; m_matrix[11] = 0.f; m_matrix[15] = a22;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Transform::Transform(const std::array<float, 16>& matrix) :
        m_matrix(matrix)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::array<float, 16>& Transform::getMatrix() const
    {
        return m_matrix;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Transform Transform::getInverse() const
    {
        // Compute the determinant
        const float det = m_matrix[0] * (m_matrix[15] * m_matrix[5] - m_matrix[7] * m_matrix[13])
                        - m_matrix[1] * (m_matrix[15] * m_matrix[4] - m_matrix[7] * m_matrix[12])
                        + m_matrix[3] * (m_matrix[13] * m_matrix[4] - m_matrix[5] * m_matrix[12]);

        // Compute the inverse if the determinant is not zero (don't use an epsilon because the determinant may *really* be tiny)
        if (det != 0.f)
        {
            return { (m_matrix[15] * m_matrix[5] - m_matrix[7] * m_matrix[13]) / det,
                    -(m_matrix[15] * m_matrix[4] - m_matrix[7] * m_matrix[12]) / det,
                     (m_matrix[13] * m_matrix[4] - m_matrix[5] * m_matrix[12]) / det,
                    -(m_matrix[15] * m_matrix[1] - m_matrix[3] * m_matrix[13]) / det,
                     (m_matrix[15] * m_matrix[0] - m_matrix[3] * m_matrix[12]) / det,
                    -(m_matrix[13] * m_matrix[0] - m_matrix[1] * m_matrix[12]) / det,
                     (m_matrix[7]  * m_matrix[1] - m_matrix[3] * m_matrix[5])  / det,
                    -(m_matrix[7]  * m_matrix[0] - m_matrix[3] * m_matrix[4])  / det,
                     (m_matrix[5]  * m_matrix[0] - m_matrix[1] * m_matrix[4])  / det};
        }
        else
            return {};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Transform::transformPoint(const Vector2f& point) const
    {
        return {m_matrix[0] * point.x + m_matrix[4] * point.y + m_matrix[12],
                m_matrix[1] * point.x + m_matrix[5] * point.y + m_matrix[13]};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    FloatRect Transform::transformRect(const FloatRect& rectangle) const
    {
        // Transform the 4 corners of the rectangle
        const Vector2f points[] =
        {
            transformPoint({rectangle.left, rectangle.top}),
            transformPoint({rectangle.left, rectangle.top + rectangle.height}),
            transformPoint({rectangle.left + rectangle.width, rectangle.top}),
            transformPoint({rectangle.left + rectangle.width, rectangle.top + rectangle.height})
        };

        // Compute the bounding rectangle of the transformed points
        const float left = std::min({points[0].x, points[1].x, points[2].x, points[3].x});
        const float right = std::max({points[0].x, points[1].x, points[2].x, points[3].x});
        const float top = std::min({points[0].y, points[1].y, points[2].y, points[3].y});
        const float bottom = std::max({points[0].y, points[1].y, points[2].y, points[3].y});

        return {left, top, right - left, bottom - top};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Transform& Transform::combine(const Transform& other)
    {
        const auto& a = m_matrix;
        const auto& b = other.m_matrix;

        *this = Transform(a[0] * b[0]  + a[4] * b[1]  + a[12] * b[3],
                          a[0] * b[4]  + a[4] * b[5]  + a[12] * b[7],
                          a[0] * b[12] + a[4] * b[13] + a[12] * b[15],
                          a[1] * b[0]  + a[5] * b[1]  + a[13] * b[3],
                          a[1] * b[4]  + a[5] * b[5]  + a[13] * b[7],
                          a[1] * b[12] + a[5] * b[13] + a[13] * b[15],
                          a[3] * b[0]  + a[7] * b[1]  + a[15] * b[3],
                          a[3] * b[4]  + a[7] * b[5]  + a[15] * b[7],
                          a[3] * b[12] + a[7] * b[13] + a[15] * b[15]);
        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Transform& Transform::translate(const Vector2f& offset)
    {
        return combine({1, 0, offset.x,
                        0, 1, offset.y,
                        0, 0, 1});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Transform& Transform::rotate(float angle, const Vector2f& center)
    {
#if defined(__cpp_lib_math_constants) && (__cpp_lib_math_constants >= 201907L)
    const float pi = std::numbers::pi_v<float>;
#else
    const float pi = 3.14159265359f;
#endif
        const float rad = angle * pi / 180.f;
        const float cos = std::cos(rad);
        const float sin = std::sin(rad);
        return combine({cos, -sin, center.x * (1 - cos) + center.y * sin,
                        sin,  cos, center.y * (1 - cos) - center.x * sin,
                        0,    0,   1});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Transform& Transform::scale(const Vector2f& factors, const Vector2f& center)
    {
        return combine({factors.x, 0,      center.x * (1 - factors.x),
                        0,      factors.y, center.y * (1 - factors.y),
                        0,      0,      1});
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Transform Transform::operator *(const Transform& right) const
    {
        return Transform(*this).combine(right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Transform& Transform::operator *=(const Transform& right)
    {
        return combine(right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f Transform::operator *(const Vector2f& right) const
    {
        return transformPoint(right);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Transform::roundPosition(float pixelScaleX, float pixelScaleY)
    {
        m_matrix[12] = std::round(m_matrix[12] * pixelScaleX) / pixelScaleX;
        m_matrix[13] = std::round(m_matrix[13] * pixelScaleY) / pixelScaleY;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
