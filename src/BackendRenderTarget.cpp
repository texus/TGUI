/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2020 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/BackendRenderTarget.hpp>
#include <array>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    void BackendRenderTargetBase::drawBorders(const RenderStates& states, const Borders& borders, Vector2f size, Color color)
    {
        //////////////////////
        // 0---1----------6 //
        // |              | //
        // |   8------7   | //
        // |   |      |   | //
        // |   |      |   | //
        // |   3------5   | //
        // |              | //
        // 2--------------4 //
        //////////////////////
        const auto vertexColor = Vertex::Color(color);
        drawTriangles(states,
            {
                {{0, 0}, vertexColor},
                {{borders.getLeft(), 0}, vertexColor},
                {{0, size.y}, vertexColor},
                {{borders.getLeft(), size.y - borders.getBottom()}, vertexColor},
                {{size.x, size.y}, vertexColor},
                {{size.x - borders.getRight(), size.y - borders.getBottom()}, vertexColor},
                {{size.x, 0}, vertexColor},
                {{size.x - borders.getRight(), borders.getTop()}, vertexColor},
                {{borders.getLeft(), borders.getTop()}, vertexColor}
            },
            {
                0, 2, 1,
                1, 2, 3,
                2, 4, 3,
                3, 4, 5,
                4, 6, 5,
                5, 6, 7,
                6, 1, 7,
                7, 1, 8
            }
        );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetBase::drawFilledRect(const RenderStates& states, Vector2f size, Color color)
    {
        const auto vertexColor = Vertex::Color(color);
        drawTriangles(states,
            {
                {{0, 0}, vertexColor},
                {{0, size.y}, vertexColor},
                {{size.x, 0}, vertexColor},
                {{size.x, size.y}, vertexColor}
            },
            {
                0, 1, 2,
                2, 1, 3
            }
        );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void BackendRenderTargetBase::drawTriangles(const RenderStates& states, std::initializer_list<Vertex> vertices, std::initializer_list<int> indices)
    {
        if (indices.size() == 0)
            drawTriangles(states, vertices.begin(), vertices.size());
        else
        {
            TGUI_ASSERT(indices.size() % 3 == 0, "BackendRenderTargetBase::drawTriangles requires that the number of indices is divisible by 3");
            drawTriangles(states, vertices.begin(), vertices.size(), indices.begin(), indices.size());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
