/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
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


#ifndef TGUI_TESTS_HPP
#define TGUI_TESTS_HPP

#include <TGUI/Config.hpp>

#ifdef TGUI_SYSTEM_WINDOWS
    #define NOMB
    #define NOMINMAX
    #define VC_EXTRALEAN
    #define WIN32_LEAN_AND_MEAN
#endif

#include "catch.hpp"
#include <TGUI/GuiBase.hpp>
#include <TGUI/Container.hpp>
#include <TGUI/Widgets/ClickableWidget.hpp>
#include <TGUI/Widgets/Panel.hpp>
#include <TGUI/Loading/Theme.hpp>
#include <TGUI/Loading/Serializer.hpp>

#if TGUI_HAS_BACKEND_SFML
    #include <SFML/Graphics/RenderTexture.hpp>
    #include <TGUI/Backends/SFML/GuiSFML.hpp>

    #define TEST_DRAW_INIT(width, height, widget) \
                sf::RenderTexture target; \
                target.create(width, height); \
                tgui::GuiSFML gui{target}; \
                gui.add(widget);

    #ifdef TGUI_ENABLE_DRAW_TESTS
        #define TEST_DRAW(filename) \
                    target.clear({25, 130, 10}); \
                    gui.draw(); \
                    target.display(); \
                    target.getTexture().copyToImage().saveToFile(filename); \
                    compareImageFiles(filename, "expected/" filename);
    #else
        #define TEST_DRAW(filename) \
                    target.clear({25, 130, 10}); \
                    gui.draw(); \
                    target.display(); \
                    target.getTexture().copyToImage().saveToFile(filename);
    #endif
#else
    // Drawing tests are currently unsupported in other backends
    #define TEST_DRAW_INIT(width, height, widget) \
                GuiNull gui; \
                gui.add(widget);

    #define TEST_DRAW(filename)
#endif

static const std::chrono::milliseconds DOUBLE_CLICK_TIMEOUT = std::chrono::milliseconds(500);

bool compareFiles(const tgui::String& leftFileName, const tgui::String& rightFileName);
void compareImageFiles(const tgui::String& filename1, const tgui::String& filename2);

tgui::String getClipboardContents();

void mouseCallback(unsigned int& count, tgui::Vector2f pos);
void genericCallback(unsigned int& count);

void testWidgetSignals(tgui::Widget::Ptr widget);
void testClickableWidgetSignals(tgui::ClickableWidget::Ptr widget);
void testClickableWidgetSignals(tgui::Panel::Ptr widget);

void testWidgetRenderer(tgui::WidgetRenderer* renderer);

template <typename WidgetType>
void testSavingWidget(tgui::String name, std::shared_ptr<WidgetType> widget, bool loadFromTheme = true)
{
    if (loadFromTheme)
    {
        tgui::Theme theme{"resources/Black.txt"};
        widget->setRenderer(theme.getRenderer(name));
        widget->getRenderer()->setFont("resources/DejaVuSans.ttf");
    }

    auto parent = std::make_shared<tgui::RootContainer>();
    parent->add(widget);

    REQUIRE_NOTHROW(parent->saveWidgetsToFile(name + "WidgetFile1.txt"));

    SECTION("Copying widget before saving")
    {
        // Copy constructor
        WidgetType temp1(*widget);

        // Assignment operator
        WidgetType temp2;
        temp2 = temp1;

#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wself-assign-overloaded"
#endif
        temp2 = temp2;
#if defined(__clang__)
    #pragma clang diagnostic pop
#endif

        // Move constructor
        WidgetType temp3 = std::move(temp2);

        // Move assignment operator
        WidgetType temp4;
        temp4 = std::move(temp3);

        // copy function
        std::shared_ptr<WidgetType> temp5 = std::make_shared<WidgetType>(temp4);
        tgui::Widget::Ptr temp6 = WidgetType::copy(temp5);
        REQUIRE(temp6 != nullptr);
        REQUIRE(WidgetType::copy(nullptr) == nullptr);

        // clone function
        tgui::Widget::Ptr temp7 = temp6->clone();

        parent = std::make_shared<tgui::RootContainer>();
        parent->add(temp7);

        REQUIRE_NOTHROW(parent->saveWidgetsToFile(name + "WidgetFile2.txt"));
        REQUIRE(compareFiles(name + "WidgetFile1.txt", name + "WidgetFile2.txt"));
    }

    SECTION("Saving again after loading")
    {
        parent = std::make_shared<tgui::RootContainer>();
        REQUIRE_NOTHROW(parent->loadWidgetsFromFile(name + "WidgetFile1.txt"));

        REQUIRE_NOTHROW(parent->saveWidgetsToFile(name + "WidgetFile3.txt"));
        REQUIRE(compareFiles(name + "WidgetFile1.txt", name + "WidgetFile3.txt"));
    }
}

class GuiNull : public tgui::GuiBase
{
public:
    GuiNull()
    {
        init();
        setAbsoluteViewport({0, 0, 200, 200});
        setAbsoluteView({0, 0, 200, 200});
    }

    void draw() {}
    void mainLoop() {}
};

#endif // TGUI_TESTS_HPP
