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

#ifndef TGUI_TRANSFORMABLE_HPP
#define TGUI_TRANSFORMABLE_HPP

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class TGUI_API Transformable
    {
      public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Default constructor
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Transformable();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Destructor
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~Transformable();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Set the position of the object
        ///
        /// This function completely overwrites the previous position.
        /// See the move function to apply an offset based on the previous position instead.
        /// The default position of a transformable object is (0, 0).
        ///
        /// \param x X coordinate of the new position
        /// \param y Y coordinate of the new position
        ///
        /// \see move, getPosition
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setPosition(float x, float y);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief set the position of the object
        ///
        /// This function completely overwrites the previous position.
        /// See the move function to apply an offset based on the previous position instead.
        /// The default position of a transformable object is (0, 0).
        ///
        /// \param position New position
        ///
        /// \see move, getPosition
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void setPosition(const Vector2f& position);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief get the position of the object
        ///
        /// \return Current position
        ///
        /// \see setPosition
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        const Vector2f& getPosition() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Move the object by a given offset
        ///
        /// This function adds to the current position of the object, unlike setPosition which overwrites it.
        /// Thus, it is equivalent to the following code:
        /// \code
        /// sf::Vector2f pos = object.getPosition();
        /// object.setPosition(pos.x + offsetX, pos.y + offsetY);
        /// \endcode
        ///
        /// \param offsetX X offset
        /// \param offsetY Y offset
        ///
        /// \see setPosition
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void move(float offsetX, float offsetY);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Move the object by a given offset
        ///
        /// This function adds to the current position of the object, unlike setPosition which overwrites it.
        /// Thus, it is equivalent to the following code:
        /// \code
        /// object.setPosition(object.getPosition() + offset);
        /// \endcode
        ///
        /// \param offset Offset
        ///
        /// \see setPosition
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void move(const Vector2f& offset);


        // This function is temporarily kept for compability reasons.
        // It will be removed when the function is no longer needed by tgui objects.
        // This function will always return (1, 1).
        Vector2f getScale() const;


        // This function is temporarily kept for compability reasons.
        // It will be removed when the function is no longer needed by tgui objects.
        const sf::Transform& getTransform() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      private:

        Vector2f m_Position;

        mutable bool          m_TransformNeedUpdate;
        mutable sf::Transform m_Transform;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_TRANSFORMABLE_HPP
