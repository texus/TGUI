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


#ifndef TGUI_TABLE_ITEM_HPP
#define TGUI_TABLE_ITEM_HPP

#include <TGUI/Widgets/Panel.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    class TGUI_API TableItem: public tgui::Panel
    {
    public:
        enum HorizontalAlign
        {
            Left,
            Center,
            Right,
            None /// internal
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    public:
        typedef std::shared_ptr<TableItem> Ptr; ///< Shared widget pointer
        typedef std::shared_ptr<const TableItem> ConstPtr; ///< Shared constant widget pointer

        TableItem();

        virtual Widget::Ptr clone() const override
        {
            return std::make_shared<TableItem>(*this);
        }

        void setItem(const Widget::Ptr& widgetPtr, HorizontalAlign align = Left);
        Widget::Ptr getItem() { return m_widget; }

        void setHorizontalAlign(HorizontalAlign align);

        void setFont(const Font& font) override;

        virtual void setSize(const Layout2d& size) override;
        using Transformable::setSize;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        void updateItem();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    private:
        Widget::Ptr m_widget;

        HorizontalAlign m_align = Left;
    };
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_TABLE_ITEM_HPP
