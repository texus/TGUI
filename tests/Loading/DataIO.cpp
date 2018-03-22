/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2018 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include "Tests.hpp"
#include <TGUI/Loading/DataIO.hpp>
#include <TGUI/Loading/Deserializer.hpp>

TEST_CASE("[DataIO]")
{
    SECTION("parse")
    {
        SECTION("Empty input")
        {
            std::stringstream input("");
            REQUIRE_NOTHROW(tgui::DataIO::parse(input));
        }

        SECTION("Global properties")
        {
            std::stringstream input("Property = Value;");
            REQUIRE_NOTHROW(tgui::DataIO::parse(input));
        }

        SECTION("Nested childs")
        {
            std::stringstream input("Child { Child2 { Property = Value; } }");
            REQUIRE_NOTHROW(tgui::DataIO::parse(input));
        }

        SECTION("Nested child as value")
        {
            std::stringstream input("Child { Child2 = { Property = Value; }; }");
            REQUIRE_NOTHROW(tgui::DataIO::parse(input));
        }

        SECTION("Extra semicolons")
        {
            std::stringstream input("Child { Child2 { Property = Value; }; };");
            REQUIRE_NOTHROW(tgui::DataIO::parse(input));
        }

        SECTION("List property")
        {
            std::stringstream input("Property = [a,b,c,d];");
            REQUIRE_NOTHROW(tgui::DataIO::parse(input));
        }

        SECTION("Section without name")
        {
            std::stringstream input("{ Property//\n= [\"a\", \"\\\"b\\\"\", \"\\\\c\\\\\"]; }");
            REQUIRE_NOTHROW(tgui::DataIO::parse(input));
        }

        SECTION("Empty section")
        {
            std::stringstream input("name{}");
            REQUIRE_NOTHROW(tgui::DataIO::parse(input));
        }

        SECTION("Name with special characters")
        {
            std::stringstream input("\"SpecialChars.{}=:;/*#//\\t\\\"\\\\\" { Property\r\n//txt\n = \"\\\\\\\"Value\\\"\\\\\"; }");

            std::unique_ptr<tgui::DataIO::Node> rootNode;
            REQUIRE_NOTHROW(rootNode = tgui::DataIO::parse(input));

            REQUIRE(rootNode->children.size() == 1);
            REQUIRE(rootNode->children[0]->name == "\"SpecialChars.{}=:;/*#//\\t\\\"\\\\\"");
            REQUIRE(rootNode->children[0]->propertyValuePairs.size() == 1);
            REQUIRE(rootNode->children[0]->propertyValuePairs["property"]->value == "\"\\\\\\\"Value\\\"\\\\\"");

            REQUIRE(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::String,
                                                    rootNode->children[0]->name).getString()
                    == "SpecialChars.{}=:;/*#//\t\"\\");

            REQUIRE(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::String,
                                                    rootNode->children[0]->propertyValuePairs["property"]->value).getString()
                    == "\\\"Value\"\\");
        }

        SECTION("Unexpected '/' found")
        {
            std::stringstream input("/ {}");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input), tgui::Exception);

            std::stringstream input2("{ / }");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input2), tgui::Exception);

            // Slash is allowed in name when serialized
            std::stringstream input3("\"/\" {}");
            REQUIRE_NOTHROW(tgui::DataIO::parse(input3));
        }

        SECTION("Unexpected EOF while parsing")
        {
            std::stringstream input("name");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input), tgui::Exception);

            std::stringstream input2("name ");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input2), tgui::Exception);

            std::stringstream input3("{");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input3), tgui::Exception);

            std::stringstream input4("{{");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input4), tgui::Exception);

            std::stringstream input5("{ name{");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input5), tgui::Exception);

            std::stringstream input6("{ /**/ ");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input6), tgui::Exception);
        }

        SECTION("Expected section name, found ';' instead")
        {
            std::stringstream input(";");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input), tgui::Exception);
        }

        SECTION("Expected '{' or '=', found X instead")
        {
            std::stringstream input("name;");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input), tgui::Exception);

            std::stringstream input2("}");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input2), tgui::Exception);

            std::stringstream input3("{ Property;");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input3), tgui::Exception);

            std::stringstream input4("{ Property }");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input4), tgui::Exception);
        }

        SECTION("Found EOF while trying to read property or nested section name")
        {
            std::stringstream input("{ Property");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input), tgui::Exception);

            std::stringstream input2("{ Property ");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input2), tgui::Exception);
        }

        SECTION("Expected property or nested section name, found ';' instead")
        {
            std::stringstream input("{;");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input), tgui::Exception);
        }

        SECTION("Found EOF while trying to read a value")
        {
            std::stringstream input("{ Property = Value");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input), tgui::Exception);

            std::stringstream input2("{ { Property = Value /");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input2), tgui::Exception);

            std::stringstream input3("{ Property= \"");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input3), tgui::Exception);
        }

        SECTION("Found '=' while trying to read a value")
        {
            std::stringstream input("{ Property = =");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input), tgui::Exception);
        }

        SECTION("Found '{' while trying to read a value")
        {
            std::stringstream input("{ Property = {");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input), tgui::Exception);
        }

        SECTION("Found empty value")
        {
            std::stringstream input("{ Property = ; ");
            REQUIRE_THROWS_AS(tgui::DataIO::parse(input), tgui::Exception);
        }
    }

    SECTION("correct input")
    {
        std::stringstream stream;

        SECTION("emit")
        {
            auto root = std::make_unique<tgui::DataIO::Node>();
            root->name = "Root";
            root->propertyValuePairs["GlobalProperty1"] = std::make_unique<tgui::DataIO::ValueNode>("GlobalValue1");
            root->propertyValuePairs["GlobalProperty2"] = std::make_unique<tgui::DataIO::ValueNode>("GlobalValue2");

            auto child1 = std::make_unique<tgui::DataIO::Node>();
            child1->name = "Child1";
            child1->propertyValuePairs["Property"] = std::make_unique<tgui::DataIO::ValueNode>("Value");
            child1->parent = root.get();

            auto child2 = std::make_unique<tgui::DataIO::Node>();
            child2->parent = root.get();

            auto nestedChild = std::make_unique<tgui::DataIO::Node>();
            nestedChild->name = "NestedChild";
            nestedChild->propertyValuePairs["PropertyA"] = std::make_unique<tgui::DataIO::ValueNode>("ValueA");
            nestedChild->propertyValuePairs["PropertyB"] = std::make_unique<tgui::DataIO::ValueNode>("[]");
            nestedChild->propertyValuePairs["PropertyC"] = std::make_unique<tgui::DataIO::ValueNode>("[X, Y, Z]");
            nestedChild->parent = root.get();
            child1->children.push_back(std::move(nestedChild));

            root->children.push_back(std::move(child1));
            root->children.push_back(std::move(child2));

            tgui::DataIO::emit(root, stream);
        }

        SECTION("direct stream")
        {
            stream << "GlobalProperty1 = GlobalValue1;" << std::endl;
            stream << "GlobalProperty2 = GlobalValue2;" << std::endl;
            stream << "Child1" << std::endl;
            stream << "{" << std::endl;
            stream << "Property = Value;" << std::endl;
            stream << "NestedChild" << std::endl;
            stream << "{" << std::endl;
            stream << "PropertyA = ValueA;" << std::endl;
            stream << "PropertyB = [];" << std::endl;
            stream << "PropertyC = [X, Y, Z];" << std::endl;
            stream << "}" << std::endl;
            stream << "}" << std::endl;
            stream << "{" << std::endl;
            stream << "}" << std::endl;
        }

        // Check that emit function works correctly be parsing the result and check if it equals the input
        auto parsedRoot = tgui::DataIO::parse(stream);
        REQUIRE(parsedRoot->name == ""); // Root node name is ignored when saving
        REQUIRE(parsedRoot->parent == nullptr);
        REQUIRE(parsedRoot->children.size() == 2);
        REQUIRE(parsedRoot->propertyValuePairs.size() == 2);
        REQUIRE(!parsedRoot->propertyValuePairs["globalproperty1"]->listNode);
        REQUIRE(parsedRoot->propertyValuePairs["globalproperty1"]->value == "GlobalValue1");
        REQUIRE(!parsedRoot->propertyValuePairs["globalproperty2"]->listNode);
        REQUIRE(parsedRoot->propertyValuePairs["globalproperty2"]->value == "GlobalValue2");
        REQUIRE(parsedRoot->children[0]->name == "Child1");
        REQUIRE(parsedRoot->children[0]->parent == parsedRoot.get());
        REQUIRE(parsedRoot->children[0]->children.size() == 1);
        REQUIRE(parsedRoot->children[0]->propertyValuePairs.size() == 1);
        REQUIRE(!parsedRoot->children[0]->propertyValuePairs["property"]->listNode);
        REQUIRE(parsedRoot->children[0]->propertyValuePairs["property"]->value == "Value");
        REQUIRE(parsedRoot->children[0]->children[0]->name == "NestedChild");
        REQUIRE(parsedRoot->children[0]->children[0]->parent == parsedRoot->children[0].get());
        REQUIRE(parsedRoot->children[0]->children[0]->children.empty());
        REQUIRE(parsedRoot->children[0]->children[0]->propertyValuePairs.size() == 3);
        REQUIRE(!parsedRoot->children[0]->children[0]->propertyValuePairs["propertya"]->listNode);
        REQUIRE(parsedRoot->children[0]->children[0]->propertyValuePairs["propertya"]->value == "ValueA");
        REQUIRE(parsedRoot->children[0]->children[0]->propertyValuePairs["propertyb"]->listNode);
        REQUIRE(parsedRoot->children[0]->children[0]->propertyValuePairs["propertyb"]->value == "[]");
        REQUIRE(parsedRoot->children[0]->children[0]->propertyValuePairs["propertyb"]->valueList.empty());
        REQUIRE(parsedRoot->children[0]->children[0]->propertyValuePairs["propertyc"]->listNode);
        REQUIRE(parsedRoot->children[0]->children[0]->propertyValuePairs["propertyc"]->value == "[X, Y, Z]");
        REQUIRE(parsedRoot->children[0]->children[0]->propertyValuePairs["propertyc"]->valueList.size() == 3);
        REQUIRE(parsedRoot->children[0]->children[0]->propertyValuePairs["propertyc"]->valueList[0] == "X");
        REQUIRE(parsedRoot->children[0]->children[0]->propertyValuePairs["propertyc"]->valueList[1] == "Y");
        REQUIRE(parsedRoot->children[0]->children[0]->propertyValuePairs["propertyc"]->valueList[2] == "Z");
        REQUIRE(parsedRoot->children[1]->name == "");
        REQUIRE(parsedRoot->children[1]->parent == parsedRoot.get());
        REQUIRE(parsedRoot->children[1]->children.empty());
        REQUIRE(parsedRoot->children[1]->propertyValuePairs.empty());
    }
}
