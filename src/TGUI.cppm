/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2025 Bruno Van de Velde (vdv_b@tgui.eu)
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

module;

#include <TGUI/TGUI.hpp>
#include <TGUI/Base64.hpp>
#include <TGUI/TextureManager.hpp>
#include <TGUI/CustomWidgetForBindings.hpp>
#include <TGUI/SignalManager.hpp>
#include <TGUI/Loading/ImageLoader.hpp>
#include <TGUI/Widgets/CanvasBase.hpp>

export module tgui;

export namespace tgui
{
    using tgui::AbsoluteOrRelativeValue;
    using tgui::RelativeValue;
    using tgui::ShowEffectType;
    using tgui::AnimationType;
    using tgui::Any;
    using tgui::AnyCast;
    using tgui::base64Encode;
    using tgui::base64Decode;
    using tgui::Color;
    using tgui::Container;
    using tgui::RootContainer;
    using tgui::CopiedPtr;
    using tgui::makeCopied;
    using tgui::CopiedSharedPtr;
    using tgui::Cursor;
    using tgui::CustomWidgetForBindings;
    using tgui::Duration;
    using tgui::Event;
    using tgui::Exception;
    using tgui::FileDialogIconLoader;
    using tgui::Filesystem;
    using tgui::FontGlyph;
    using tgui::Font;
    using tgui::MakeUniqueForOverwrite;
    using tgui::clamp;
    using tgui::setGlobalTextSize;
    using tgui::getGlobalTextSize;
    using tgui::setDoubleClickTime;
    using tgui::getDoubleClickTime;
    using tgui::setResourcePath;
    using tgui::getResourcePath;
    using tgui::setEditCursorBlinkRate;
    using tgui::getEditCursorBlinkRate;
    using tgui::readFileToMemory;
    using tgui::writeFile;
    using tgui::downcast;
    using tgui::Orientation;
    using tgui::HorizontalAlignment;
    using tgui::VerticalAlignment;
    using tgui::AutoLayout;
    using tgui::Layout;
    using tgui::Layout2d;
    using tgui::ObjectConverter;
    using tgui::Optional;
    using tgui::Outline;
    using tgui::Borders;
    using tgui::Padding;
    using tgui::Rect;
    using tgui::FloatRect;
    using tgui::IntRect;
    using tgui::UIntRect;
    using tgui::RelFloatRect;
    using tgui::RenderStates;
    using tgui::Signal;
    using tgui::SignalTyped;
    using tgui::SignalTyped2;
    using tgui::SignalInt;
    using tgui::SignalUInt;
    using tgui::SignalBool;
    using tgui::SignalFloat;
    using tgui::SignalColor;
    using tgui::SignalString;
    using tgui::SignalVector2f;
    using tgui::SignalFloatRect;
    using tgui::SignalRange;
    using tgui::SignalChildWindow;
    using tgui::SignalItem;
    using tgui::SignalPanelListBoxItem;
    using tgui::SignalFileDialogPaths;
    using tgui::SignalShowEffect;
    using tgui::SignalAnimationType;
    using tgui::SignalItemHierarchy;
    using tgui::SignalManager;
    using tgui::Sprite;
    using tgui::isWhitespace;
    using tgui::isAlpha;
    using tgui::isDigit;
    using tgui::String;
    using tgui::StringView;
    using tgui::CharStringView;
    using tgui::viewEqualIgnoreCase;
    using tgui::viewStartsWith;
    using tgui::viewEndsWith;
    using tgui::SubwidgetContainer;
    using tgui::SvgImage;
    using tgui::AutoTextSize;
    using tgui::Text;
    using tgui::TextStyle;
    using tgui::TextStyles;
    using tgui::Texture;
    using tgui::TextureData;
    using tgui::TextureDataHolder;
    using tgui::TextureManager;
    using tgui::Timer;
    using tgui::ToolTip;
    using tgui::Transform;
    using tgui::TwoFingerScrollDetect;
    using tgui::Variant;
    using tgui::Vector2;
    using tgui::Vector2f;
    using tgui::Vector2u;
    using tgui::Vector2i;
    using tgui::Vertex;
    using tgui::Widget;
    using tgui::DataIO;
    using tgui::Deserializer;
    using tgui::ImageLoader;
    using tgui::Serializer;
    using tgui::Theme;
    using tgui::BaseThemeLoader;
    using tgui::WidgetFactory;

    using tgui::BitmapButton;
    using tgui::BoxLayout;
    using tgui::BoxLayoutRatios;
    using tgui::ButtonBase;
    using tgui::Button;
    using tgui::CanvasBase;
    using tgui::ChatBox;
    using tgui::CheckBox;
    using tgui::ChildWindow;
    using tgui::ClickableWidget;
    using tgui::ColorPicker;
    using tgui::ComboBox;
    using tgui::ContextMenu;
    using tgui::EditBox;
    using tgui::EditBoxSlider;
    using tgui::FileDialog;
    using tgui::Grid;
    using tgui::Group;
    using tgui::GrowHorizontalLayout;
    using tgui::GrowVerticalLayout;
    using tgui::HorizontalLayout;
    using tgui::HorizontalWrap;
    using tgui::Knob;
    using tgui::Label;
    using tgui::ListBox;
    using tgui::ListView;
    using tgui::MenuBar;
    using tgui::MenuWidgetBase;
    using tgui::MessageBox;
    using tgui::Panel;
    using tgui::PanelListBox;
    using tgui::Picture;
    using tgui::ProgressBar;
    using tgui::RadioButtonGroup;
    using tgui::RadioButton;
    using tgui::RangeSlider;
    using tgui::RichTextLabel;
    using tgui::ScrollablePanel;
    using tgui::Scrollbar;
    using tgui::SeparatorLine;
    using tgui::Slider;
    using tgui::SpinButton;
    using tgui::SpinControl;
    using tgui::SplitContainer;
    using tgui::TabContainer;
    using tgui::Tabs;
    using tgui::TextArea;
    using tgui::ToggleButton;
    using tgui::TreeView;
    using tgui::VerticalLayout;

    using tgui::BoxLayoutRenderer;
    using tgui::ButtonRenderer;
    using tgui::ChatBoxRenderer;
    using tgui::CheckBoxRenderer;
    using tgui::ChildWindowRenderer;
    using tgui::ColorPickerRenderer;
    using tgui::ComboBoxRenderer;
    using tgui::ContextMenuRenderer;
    using tgui::EditBoxRenderer;
    using tgui::FileDialogRenderer;
    using tgui::GroupRenderer;
    using tgui::KnobRenderer;
    using tgui::LabelRenderer;
    using tgui::ListBoxRenderer;
    using tgui::ListViewRenderer;
    using tgui::MenuBarRenderer;
    using tgui::MenuWidgetBaseRenderer;
    using tgui::MessageBoxRenderer;
    using tgui::PanelListBoxRenderer;
    using tgui::PanelRenderer;
    using tgui::PictureRenderer;
    using tgui::ProgressBarRenderer;
    using tgui::RadioButtonRenderer;
    using tgui::RangeSliderRenderer;
    using tgui::ScrollablePanelRenderer;
    using tgui::ScrollbarRenderer;
    using tgui::SeparatorLineRenderer;
    using tgui::SliderRenderer;
    using tgui::SpinButtonRenderer;
    using tgui::SplitContainerRenderer;
    using tgui::TabsRenderer;
    using tgui::TextAreaRenderer;
    using tgui::TreeViewRenderer;
    using tgui::WidgetRenderer;

    using tgui::BackendFont;
    using tgui::BackendFontFactory;
    using tgui::BackendFontFactoryImpl;
    using tgui::BackendRenderer;
    using tgui::BackendRenderTarget;
    using tgui::BackendText;
    using tgui::BackendTexture;
    using tgui::Backend;
    using tgui::BackendGui;
    using tgui::isBackendSet;
    using tgui::setBackend;
    using tgui::getBackend;

    using tgui::operator==;
    using tgui::operator!=;
    using tgui::operator>;
    using tgui::operator>=;
    using tgui::operator<;
    using tgui::operator<=;
    using tgui::operator+;
    using tgui::operator-;
    using tgui::operator*;
    using tgui::operator/;
    using tgui::operator%;
    using tgui::operator+=;
    using tgui::operator-=;
    using tgui::operator*=;
    using tgui::operator/=;
    using tgui::operator%=;
    using tgui::operator<<;
    using tgui::operator>>;

    inline namespace literals
    {
        inline namespace percent
        {
            using tgui::literals::percent::operator""_percent;
        }
    }

    inline namespace bind_functions
    {
        using tgui::bind_functions::bindPosX;
        using tgui::bind_functions::bindPosY;
        using tgui::bind_functions::bindLeft;
        using tgui::bind_functions::bindTop;
        using tgui::bind_functions::bindWidth;
        using tgui::bind_functions::bindHeight;
        using tgui::bind_functions::bindInnerWidth;
        using tgui::bind_functions::bindInnerHeight;
        using tgui::bind_functions::bindRight;
        using tgui::bind_functions::bindBottom;
        using tgui::bind_functions::bindPosition;
        using tgui::bind_functions::bindSize;
        using tgui::bind_functions::bindInnerSize;
        using tgui::bind_functions::bindWidth;
        using tgui::bind_functions::bindHeight;
        using tgui::bind_functions::bindSize;
        using tgui::bind_functions::bindMin;
        using tgui::bind_functions::bindMax;
    }
}
