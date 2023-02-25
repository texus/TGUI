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


#ifndef TGUI_DATA_IO_HPP
#define TGUI_DATA_IO_HPP

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <TGUI/String.hpp>

#if !TGUI_EXPERIMENTAL_USE_STD_MODULE
    #include <sstream>
    #include <memory>
    #include <vector>
    #include <string>
    #include <map>
    #include <cstdio>
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TGUI_MODULE_EXPORT namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @brief Parser and emitter for widget files
    /// @internal
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    class TGUI_API DataIO
    {
    public:
        struct ValueNode;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Widget file node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct Node
        {
            Node() = default;
            Node(Node&&) = default;
            Node& operator=(const Node&) = delete; // Not implemented because current code has no use for it
            Node& operator=(Node&&) = default;

            Node(const Node& other) :
                parent{other.parent},
                children{},
                propertyValuePairs{},
                name{other.name}
            {
                for (const auto& child : other.children)
                    children.push_back(std::make_unique<Node>(*child));
                for (const auto& pair : other.propertyValuePairs)
                    propertyValuePairs[pair.first] = std::make_unique<ValueNode>(*pair.second);
            }

            Node* parent = nullptr;
            std::vector<std::unique_ptr<Node>> children;
            std::map<String, std::unique_ptr<ValueNode>> propertyValuePairs;
            String name;
        };


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Widget file value node
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        struct ValueNode
        {
            ValueNode(String v = U"") : value(std::move(v)) {}

            String value;
            bool listNode = false;
            std::vector<String> valueList;
        };


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Parses a widget file
        ///
        /// @param stream  Stream containing the widget file
        ///
        /// @return Root node of the tree of nodes
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        TGUI_NODISCARD static std::unique_ptr<Node> parse(std::stringstream& stream);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @brief Emits the widget file
        ///
        /// @param rootNode Root node of the tree of nodes that is to be converted to a string stream
        /// @param stream   Stream to which the widget file will be added
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static void emit(const std::unique_ptr<Node>& rootNode, std::stringstream& stream);
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_DATA_IO_HPP
