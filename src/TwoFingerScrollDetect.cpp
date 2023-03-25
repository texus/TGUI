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


#include <TGUI/TwoFingerScrollDetect.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TwoFingerScrollDetect::reportFingerDown(std::intptr_t fingerId, float x, float y)
    {
        m_fingerPositions[fingerId] = {x, y};

        if (m_fingerPositions.size() > 2)
            m_trackingActive = false;

        if (!m_trackingActive)
            return;

        if (m_fingerPositions.size() != 2)
            return;

        m_initialCentroidPosition = calculateFingerCentroid();
        m_lastCentroidPosition = m_initialCentroidPosition;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TwoFingerScrollDetect::reportFingerUp(std::intptr_t fingerId)
    {
        m_fingerPositions.erase(fingerId);

        // When a touch ends, we won't look for a new gesture until all fingers stopped touching
        m_trackingActive = m_fingerPositions.empty();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void TwoFingerScrollDetect::reportFingerMotion(std::intptr_t fingerId, float x, float y)
    {
        m_fingerPositions[fingerId] = {x, y};
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool TwoFingerScrollDetect::isScrolling() const
    {
        return m_trackingActive && (m_fingerPositions.size() == 2);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f TwoFingerScrollDetect::getTouchPosition() const
    {
        TGUI_ASSERT(isScrolling(), "TwoFingerScrollDetect::getTouchPosition should not be called when isScrolling() returns false");

        return m_initialCentroidPosition;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    float TwoFingerScrollDetect::getDelta(float scale)
    {
        TGUI_ASSERT(isScrolling(), "TwoFingerScrollDetect::getDelta should not be called when isScrolling() returns false");

        // We use an integer instead of a float because code related to scrollbars aren't designed to handle values less than 1
        const Vector2f centroid = calculateFingerCentroid();
        const int delta = static_cast<int>((centroid.y - m_lastCentroidPosition.y) * scale);
        if (delta != 0)
            m_lastCentroidPosition = centroid;

        return static_cast<float>(delta);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Vector2f TwoFingerScrollDetect::calculateFingerCentroid()
    {
        TGUI_ASSERT(!m_fingerPositions.empty(), "TwoFingerScrollDetect::calculateFingerCentroid should not be called when no fingers are down");

        Vector2f centroid;
        for (const auto& pair : m_fingerPositions)
            centroid += pair.second; // Add finger position

        centroid /= static_cast<float>(m_fingerPositions.size());
        return centroid;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
