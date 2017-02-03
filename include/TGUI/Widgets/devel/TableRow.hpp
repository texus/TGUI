/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2017 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_TABLE_ROW_HPP
#define TGUI_TABLE_ROW_HPP

#include <TGUI/HorizontalLayout.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/devel/TableItem.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class TGUI_API TableRow: public HorizontalLayout
    {
    public:
        typedef std::shared_ptr<TableRow> Ptr; ///< Shared widget pointer
        typedef std::shared_ptr<const TableRow> ConstPtr; ///< Shared constant widget pointer

        TableRow();

        virtual Widget::Ptr clone() const override
        {
            return std::make_shared<TableRow>(*this);
        }

        /// TODO: Maybe return bool and to check if the row has the table columns number
        void setItem(unsigned int column, const std::string& name, TableItem::HorizontalAlign align = TableItem::None);
        void setItem(unsigned int column, const std::string& name, const sf::Color& color, TableItem::HorizontalAlign align = TableItem::None);

        void addItem(const std::string& name);
        void addItem(const std::string& name, const sf::Color& color);
        void addItems(const std::vector<std::string>& columns, const sf::Color& color, TableItem::HorizontalAlign align = TableItem::None);

        bool insert(std::size_t index, const tgui::Widget::Ptr& widget, TableItem::HorizontalAlign align, const sf::String& widgetName = "");
        virtual void add(const tgui::Widget::Ptr& widget, const sf::String& widgetName = "") override;
        void add(const tgui::Widget::Ptr& widget, TableItem::HorizontalAlign align, const sf::String& widgetName = "");
        void add(const tgui::Widget::Ptr& widget, bool fixedHeight, TableItem::HorizontalAlign align = TableItem::None, const sf::String& widgetName = "");

        void setHoverBackgroundColor(const sf::Color& color);
        void setNormalBackgroundColor(const sf::Color& color);

        // void setHoverTextColor(const sf::Color& color);
        void setNormalTextColor(const sf::Color& color);

        void setItemsHorizontalAlign(TableItem::HorizontalAlign align);

        void mouseEnteredWidget() override;
        void mouseLeftWidget() override;

        float getCustomHeight() // internal
        {
            return m_customHeight;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        TableItem::Ptr createItem(const std::string& name, TableItem::HorizontalAlign align, const sf::Color& color = sf::Color::Black);
        TableItem::Ptr createItem(const tgui::Widget::Ptr& widget, TableItem::HorizontalAlign align, bool fixedHeight = false);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        sf::Color m_normalBackgroundColor;
        sf::Color m_hoverBackgroundColor = sf::Color::Transparent;

        sf::Color m_normalTextColor;

        TableItem::HorizontalAlign m_align = TableItem::Left;

        float m_customHeight = 0.f;
    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_TABLE_ROW_HPP
