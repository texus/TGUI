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


#ifndef TGUI_TWO_FINGER_SCROLL_DETECT_HPP
#define TGUI_TWO_FINGER_SCROLL_DETECT_HPP

#include <TGUI/Vector2.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <cstdint>
    #include <unordered_map>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @internal
    /// Used by the gui to detect when the user is trying to scroll with two finger touches
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API TwoFingerScrollDetect
    {
    public:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Informs the detector that a finger began touching
        /// @param fingerId  Id for the finger that is unique for at least as long as the finger is held down
        /// @param x         X position of the finger on the window
        /// @param y         Y position of the finger on the window
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void reportFingerDown(std::intptr_t fingerId, float x, float y);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Informs the detector that a touching finger has moved
        /// @param fingerId  Id for the finger that was passed to reportFingerDown when the touch began
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void reportFingerUp(std::intptr_t fingerId);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Informs the detector that a finger stopped touching
        /// @param fingerId  Id for the finger that was passed to reportFingerDown when the touch began
        /// @param x         X position of the finger on the window
        /// @param y         Y position of the finger on the window
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void reportFingerMotion(std::intptr_t fingerId, float x, float y);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns whether two fingers are currently being held down and we have detected it as a scroll event
        /// @return Should the generateScrollEvent function be called?
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool isScrolling() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the original centroid of the finger positions when the fingers began touching
        /// @warning This function should only be called when isScrolling() returns true.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Vector2f getTouchPosition() const;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Returns the amount that the centroid of the finger positions have moved vertically
        /// @param scale  Factor to multiply with distance, for when the view size differs from the window size
        /// @return Distance to scroll. Positive value when the fingers went down (and we should scroll upwards).
        /// @warning This function should only be called when isScrolling() returns true.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        float getDelta(float scale);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Calculates the centroid position of all fingers
        /// @warning This function should only be called when isScrolling() returns true.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Vector2f calculateFingerCentroid();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:

        std::unordered_map<std::intptr_t, Vector2f> m_fingerPositions;
        Vector2f m_initialCentroidPosition;
        Vector2f m_lastCentroidPosition;
        bool m_trackingActive = true; // When three fingers touch, we stop trying to detect scrolling until all fingers are released again
    };
}

#endif // TGUI_TWO_FINGER_SCROLL_DETECT_HPP
