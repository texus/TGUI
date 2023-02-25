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


#ifndef TGUI_OUTLINE_HPP
#define TGUI_OUTLINE_HPP

#include <TGUI/Vector2.hpp>
#include <TGUI/AbsoluteOrRelativeValue.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API Outline
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Default constructor that initializes the outline
        ///
        /// @param size  Width and height of the outline in all directions
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        constexpr Outline(AbsoluteOrRelativeValue size = 0) :
            m_left  {size},
            m_top   {size},
            m_right {size},
            m_bottom{size}
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor that initializes the outline
        ///
        /// @param size  Width and height of the outline in all directions
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        template <typename T, typename = typename std::enable_if_t<std::is_arithmetic<T>::value, T>>
        constexpr Outline(T size) :
            m_left  {size},
            m_top   {size},
            m_right {size},
            m_bottom{size}
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor that initializes the outline
        ///
        /// @param width   Width of the left and right outline
        /// @param height  Height of the top and bottom outline
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        constexpr Outline(AbsoluteOrRelativeValue width, AbsoluteOrRelativeValue height) :
            m_left  {width},
            m_top   {height},
            m_right {width},
            m_bottom{height}
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Constructor that initializes the outline
        ///
        /// @param leftBorderWidth    Width of the left outline
        /// @param topBorderHeight    Height of the top outline
        /// @param rightBorderWidth   Width of the right outline
        /// @param bottomBorderHeight Height of the bottom outline
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        constexpr Outline(AbsoluteOrRelativeValue leftBorderWidth, AbsoluteOrRelativeValue topBorderHeight, AbsoluteOrRelativeValue rightBorderWidth, AbsoluteOrRelativeValue bottomBorderHeight) :
            m_left  {leftBorderWidth},
            m_top   {topBorderHeight},
            m_right {rightBorderWidth},
            m_bottom{bottomBorderHeight}
        {
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returnes the width of the left border
        ///
        /// @return Left border width
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD constexpr float getLeft() const
        {
            return m_left.getValue();
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returnes the height of the top border
        ///
        /// @return Top border height
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD constexpr float getTop() const
        {
            return m_top.getValue();
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returnes the width of the right border
        ///
        /// @return Right border width
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD constexpr float getRight() const
        {
            return m_right.getValue();
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returnes the height of the bottom border
        ///
        /// @return Bottom border height
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD constexpr float getBottom() const
        {
            return m_bottom.getValue();
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returnes the width of the left and top borders
        /// @return Left and top borders
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD constexpr Vector2f getOffset() const
        {
            return {getLeft(), getTop()};
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Compares two outlines
        ///
        /// @param outline  The outline to compare with this instance
        ///
        /// @return Whether the outlines are equal or not
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD constexpr bool operator==(const Outline& outline) const
        {
            return (getLeft() == outline.getLeft()) && (getTop() == outline.getTop()) && (getRight() == outline.getRight()) && (getBottom() == outline.getBottom());
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Compares two outlines
        ///
        /// @param outline  The outline to compare with this instance
        ///
        /// @return Whether the outlines are equal or not
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD constexpr bool operator!=(const Outline& outline) const
        {
            return !(*this == outline);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Adds two outlines together (e.g. to add padding and borders)
        ///
        /// @param other  The outline to add together with this instance
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD constexpr Outline operator+(const Outline& other) const
        {
            return {getLeft() + other.getLeft(),
                    getTop() + other.getTop(),
                    getRight() + other.getRight(),
                    getBottom() + other.getBottom()};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Subtracts two outlines from each other
        ///
        /// @param other  The outline to subtract from this instance
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD constexpr Outline operator-(const Outline& other) const
        {
            return {getLeft() - other.getLeft(),
                    getTop() - other.getTop(),
                    getRight() - other.getRight(),
                    getBottom() - other.getBottom()};
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Adds an outline to this instance (e.g. to add padding and borders)
        ///
        /// @param other  The outline to add to this instance
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        constexpr Outline& operator+=(const Outline& other)
        {
            m_left = getLeft() + other.getLeft();
            m_top = getTop() + other.getTop();
            m_right = getRight() + other.getRight();
            m_bottom = getBottom() + other.getBottom();
            return *this;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Subtracts an outline from this instance
        ///
        /// @param other  The outline to subtract from this instance
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        constexpr Outline& operator-=(const Outline& other)
        {
            m_left = getLeft() - other.getLeft();
            m_top = getTop() - other.getTop();
            m_right = getRight() - other.getRight();
            m_bottom = getBottom() - other.getBottom();
            return *this;
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Update the size to which the outline depends on if its values are relative
        ///
        /// @param newParentSize  New size from which to take the relative value
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        constexpr void updateParentSize(Vector2f newParentSize)
        {
            m_left.updateParentSize(newParentSize.x);
            m_top.updateParentSize(newParentSize.y);
            m_right.updateParentSize(newParentSize.x);
            m_bottom.updateParentSize(newParentSize.y);
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @internal
        /// @brief Converts the outline to a string representation
        ///
        /// @return String representation of outline
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD String toString() const
        {
            return U"(" + m_left.toString() + U", " + m_top.toString() + U", " + m_right.toString() + U", " + m_bottom.toString() + U")";
        }


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        AbsoluteOrRelativeValue m_left = 0;      //!< Width of the left outline
        AbsoluteOrRelativeValue m_top = 0;       //!< Height of the top outline
        AbsoluteOrRelativeValue m_right = 0;     //!< Width of the right outline
        AbsoluteOrRelativeValue m_bottom = 0;    //!< Height of the bottom outline
    };


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    using Borders = Outline;
    using Padding = Outline;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_OUTLINE_HPP
