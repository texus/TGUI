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


#ifndef TGUI_GUI_BUILDER_LIST_VIEW_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_LIST_VIEW_PROPERTIES_HPP

#include "WidgetProperties.hpp"

struct ListViewProperties : WidgetProperties
{
    // TODO: Items
    // TODO: Selected items
    // TODO: Item data
    // TODO: Item icons
    // TODO: Scrollbar renderer

    void updateProperty(const tgui::Widget::Ptr& widget, const tgui::String& property, const tgui::String& value) const override
    {
        auto listView = widget->cast<tgui::ListView>();
        if (property == "Columns")
        {
            listView->removeAllColumns();
            auto serializedColumns = deserializeList(value);
            for (const auto& serializedColumn : serializedColumns)
            {
                tgui::String text;
                float width;
                tgui::ListView::ColumnAlignment alignment;
                bool autoResize;
                bool expanded;
                if (deserializeColumn(serializedColumn, text, width, alignment, autoResize, expanded))
                {
                    const auto columnIndex = listView->addColumn(text, width, alignment);
                    listView->setColumnAutoResize(columnIndex, autoResize);
                    listView->setColumnExpanded(columnIndex, expanded);
                }
            }
        }
        else if (property == "HeaderHeight")
            listView->setHeaderHeight(value.toFloat());
        else if (property == "HeaderVisible")
            listView->setHeaderVisible(parseBoolean(value, true));
        else if (property == "MultiSelect")
            listView->setMultiSelect(parseBoolean(value, false));
        else if (property == "ItemHeight")
            listView->setItemHeight(value.toUInt());
        else if (property == "TextSize")
            listView->setTextSize(value.toUInt());
        else if (property == "HeaderTextSize")
            listView->setHeaderTextSize(value.toUInt());
        else if (property == "SeparatorWidth")
            listView->setSeparatorWidth(value.toUInt());
        else if (property == "HeaderSeparatorHeight")
            listView->setHeaderSeparatorHeight(value.toUInt());
        else if (property == "GridLinesWidth")
            listView->setGridLinesWidth(value.toUInt());
        else if (property == "AutoScroll")
            listView->setAutoScroll(parseBoolean(value, true));
        else if (property == "ShowVerticalGridLines")
            listView->setShowVerticalGridLines(parseBoolean(value, true));
        else if (property == "ShowHorizontalGridLines")
            listView->setShowHorizontalGridLines(parseBoolean(value, false));
        else if (property == "ExpandLastColumn")
        {
            TGUI_IGNORE_DEPRECATED_WARNINGS_START
            listView->setExpandLastColumn(parseBoolean(value, false));
            TGUI_IGNORE_DEPRECATED_WARNINGS_END
        }
        else if (property == "ResizableColumns")
            listView->setResizableColumns(parseBoolean(value, false));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    TGUI_NODISCARD PropertyValueMapPair initProperties(const tgui::Widget::Ptr& widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto listView = widget->cast<tgui::ListView>();
        pair.first["Columns"] = {"ListViewColumns", serializeColumns(listView)};
        pair.first["HeaderHeight"] = {"Float", tgui::String::fromNumber(listView->getHeaderHeight())};
        pair.first["HeaderVisible"] = {"Bool", tgui::Serializer::serialize(listView->getHeaderVisible())};
        pair.first["MultiSelect"] = {"Bool", tgui::Serializer::serialize(listView->getMultiSelect())};
        pair.first["ItemHeight"] = {"UInt", tgui::String::fromNumber(listView->getItemHeight())};
        pair.first["TextSize"] = {"UInt", tgui::String::fromNumber(listView->getTextSize())};
        pair.first["HeaderTextSize"] = {"UInt", tgui::String::fromNumber(listView->getHeaderTextSize())};
        pair.first["SeparatorWidth"] = {"UInt", tgui::String::fromNumber(listView->getSeparatorWidth())};
        pair.first["HeaderSeparatorHeight"] = {"UInt", tgui::String::fromNumber(listView->getHeaderSeparatorHeight())};
        pair.first["GridLinesWidth"] = {"UInt", tgui::String::fromNumber(listView->getGridLinesWidth())};
        pair.first["AutoScroll"] = {"Bool", tgui::Serializer::serialize(listView->getAutoScroll())};
        pair.first["ShowVerticalGridLines"] = {"Bool", tgui::Serializer::serialize(listView->getShowVerticalGridLines())};
        pair.first["ShowHorizontalGridLines"] = {"Bool", tgui::Serializer::serialize(listView->getShowHorizontalGridLines())};
        pair.first["ResizableColumns"] = {"Bool", tgui::Serializer::serialize(listView->getResizableColumns())};

        TGUI_IGNORE_DEPRECATED_WARNINGS_START
        pair.first["ExpandLastColumn"] = {"Bool", tgui::Serializer::serialize(listView->getExpandLastColumn())};
        TGUI_IGNORE_DEPRECATED_WARNINGS_END

        const auto renderer = listView->getSharedRenderer();
        pair.second["Borders"] = {"Outline", renderer->getBorders().toString()};
        pair.second["Padding"] = {"Outline", renderer->getPadding().toString()};
        pair.second["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pair.second["BackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColorHover())};
        pair.second["SelectedBackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedBackgroundColor())};
        pair.second["SelectedBackgroundColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedBackgroundColorHover())};
        pair.second["TextColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextColor())};
        pair.second["TextColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getTextColorHover())};
        pair.second["SelectedTextColor"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedTextColor())};
        pair.second["SelectedTextColorHover"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedTextColorHover())};
        pair.second["HeaderBackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getHeaderBackgroundColor())};
        pair.second["HeaderTextColor"] = {"Color", tgui::Serializer::serialize(renderer->getHeaderTextColor())};
        pair.second["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pair.second["SeparatorColor"] = {"Color", tgui::Serializer::serialize(renderer->getSeparatorColor())};
        pair.second["GridLinesColor"] = {"Color", tgui::Serializer::serialize(renderer->getGridLinesColor())};
        pair.second["ScrollbarWidth"] = {"Float", tgui::String::fromNumber(renderer->getScrollbarWidth())};
        pair.second["TextureHeaderBackground"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureHeaderBackground())};
        pair.second["TextureBackground"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureBackground())};
        return pair;
    }

    TGUI_NODISCARD static tgui::String serializeColumnAlignment(tgui::ListView::ColumnAlignment alignment)
    {
        switch (alignment)
        {
            case tgui::ListView::ColumnAlignment::Center:
                return "Center";
            case tgui::ListView::ColumnAlignment::Right:
                return "Right";
            default: // tgui::ListView::ColumnAlignment::Left
                return "Left";
        }
    }

    TGUI_NODISCARD static tgui::String serializeColumns(const tgui::ListView::Ptr& listView)
    {
        std::vector<tgui::String> serializedColumns;
        for (std::size_t i = 0; i < listView->getColumnCount(); ++i)
        {
            const tgui::String caption = listView->getColumnText(i);
            const float width = listView->getColumnDesignWidth(i);
            const tgui::ListView::ColumnAlignment alignment = listView->getColumnAlignment(i);
            const bool autoResize = listView->getColumnAutoResize(i);
            const bool expanded = listView->getColumnExpanded(i);
            serializedColumns.emplace_back('(' + tgui::Serializer::serialize(caption) + ',' + tgui::Serializer::serialize(width)
                + ',' + serializeColumnAlignment(alignment) + ',' + tgui::Serializer::serialize(autoResize)
                + ',' + tgui::Serializer::serialize(expanded) + ')');
        }

        return serializeList(serializedColumns);
    }

    TGUI_NODISCARD static bool deserializeColumn(
        tgui::String serializedColumn,
        tgui::String& text,
        float& width,
        tgui::ListView::ColumnAlignment& alignment,
        bool& autoResize,
        bool& expanded)
    {
        if ((serializedColumn.length() < 2) || (serializedColumn.front() != '(') || (serializedColumn.back() != ')'))
        {
            std::cout << "Failed to deserialize column '" + serializedColumn + "'. Expected brackets around value." << std::endl;
            return false;
        }

        serializedColumn = serializedColumn.substr(1, serializedColumn.length() - 2); // Remove brackets at front and back of string

        try
        {
            std::stringstream ss{(U"l=[" + serializedColumn + U"];").toStdString()};
            auto node = tgui::DataIO::parse(ss);
            if (node->propertyValuePairs["l"])
            {
                const auto& values = node->propertyValuePairs["l"]->valueList;
                if (values.size() != 5)
                {
                    std::cout << "Failed to deserialize column '" + serializedColumn + "'. Expected 5 values between brackets." << std::endl;
                    return false;
                }

                const tgui::String& alignmentStr = values[2];
                if (alignmentStr == "Left")
                    alignment = tgui::ListView::ColumnAlignment::Left;
                else if (alignmentStr == "Center")
                    alignment = tgui::ListView::ColumnAlignment::Center;
                else if (alignmentStr == "Right")
                    alignment = tgui::ListView::ColumnAlignment::Right;
                else
                {
                    std::cout << "Failed to deserialize column '" + serializedColumn + "'. Alignment has to be either 'Left', 'Center' or 'Right'." << std::endl;
                    return false;
                }

                text = tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::String, values[0]).getString();
                width = tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Number, values[1]).getNumber();
                autoResize = tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Bool, values[3]).getBool();
                expanded = tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::Bool, values[4]).getBool();
                return true;
            }
        }
        catch (const tgui::Exception&)
        {
        }

        std::cout << "Failed to deserialize column '" + serializedColumn + "'" << std::endl;
        return false;
    }
};

#endif // TGUI_GUI_BUILDER_LIST_BOX_PROPERTIES_HPP
