/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2024 Bruno Van de Velde (vdv_b@tgui.eu)
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

#include <memory>
#include <chrono>
#include <cstring>
#include <map>

#if TGUI_HAS_BACKEND_SFML_GRAPHICS
    #include <SFML/Graphics/RenderTexture.hpp>
    #if TGUI_BUILD_AS_CXX_MODULE
        import tgui.backend.sfml_graphics;
    #else
        #include <TGUI/Backend/SFML-Graphics.hpp>
    #endif

    #if SFML_VERSION_MAJOR >= 3
        #define TEST_DRAW_INIT(width, height, widget) \
                    tgui::BackendGui* guiPtr = globalGui; \
                    std::unique_ptr<tgui::BackendGui> guiUniquePtr; \
                    std::unique_ptr<sf::RenderTexture> target; \
                    if (std::dynamic_pointer_cast<tgui::BackendRendererSFML>(tgui::getBackend()->getRenderer())) \
                    { \
                        target = std::make_unique<sf::RenderTexture>(); \
                        (void)target->resize({width, height}); \
                        guiUniquePtr = std::make_unique<tgui::SFML_GRAPHICS::Gui>(*target); \
                        guiPtr = guiUniquePtr.get(); \
                    } \
                    tgui::BackendGui& gui{*guiPtr}; \
                    gui.removeAllWidgets(); \
                    gui.add(widget);
    #else
        #define TEST_DRAW_INIT(width, height, widget) \
                    tgui::BackendGui* guiPtr = globalGui; \
                    std::unique_ptr<tgui::BackendGui> guiUniquePtr; \
                    std::unique_ptr<sf::RenderTexture> target; \
                    if (std::dynamic_pointer_cast<tgui::BackendRendererSFML>(tgui::getBackend()->getRenderer())) \
                    { \
                        target = std::make_unique<sf::RenderTexture>(); \
                        (void)target->create(width, height); \
                        guiUniquePtr = std::make_unique<tgui::SFML_GRAPHICS::Gui>(*target); \
                        guiPtr = guiUniquePtr.get(); \
                    } \
                    tgui::BackendGui& gui{*guiPtr}; \
                    gui.removeAllWidgets(); \
                    gui.add(widget);
    #endif

    #ifdef TGUI_ENABLE_DRAW_TESTS
        #define TEST_DRAW(filename) \
                    if (std::dynamic_pointer_cast<tgui::BackendRendererSFML>(tgui::getBackend()->getRenderer())) \
                    { \
                        target->clear({25, 130, 10}); \
                        gui.draw(); \
                        target->display(); \
                        (void)target->getTexture().copyToImage().saveToFile(filename); \
                        compareImageFiles(filename, "expected/" filename); \
                    } \
                    else \
                        gui.draw();
    #else
        #define TEST_DRAW(filename) \
                    if (std::dynamic_pointer_cast<tgui::BackendRendererSFML>(tgui::getBackend()->getRenderer())) \
                    { \
                        target->clear({25, 130, 10}); \
                        gui.draw(); \
                        target->display(); \
                        (void)target->getTexture().copyToImage().saveToFile(filename); \
                    } \
                    else \
                        gui.draw();
    #endif

#else // Drawing tests are currently unsupported in other backends
    // Note that the code here has to be equivalent to the case where TGUI_HAS_BACKEND_SFML_GRAPHICS is
    // set but the BackendRendererSFML isn't being used at runtime.
    #define TEST_DRAW_INIT(width, height, widget) \
                tgui::BackendGui& gui{*globalGui}; \
                gui.removeAllWidgets(); \
                gui.add(widget);

    // We draw to the window, without clearing or presenting it
    #define TEST_DRAW(filename) \
                gui.draw();
#endif

#if TGUI_BUILD_AS_CXX_MODULE
    import tgui;
#else
    #include <TGUI/TGUI.hpp>
#endif

static const std::chrono::milliseconds DOUBLE_CLICK_TIMEOUT = std::chrono::milliseconds(500);

bool compareFiles(const tgui::String& leftFileName, const tgui::String& rightFileName);
void compareImageFiles(const tgui::String& filename1, const tgui::String& filename2);

tgui::String getClipboardContents();

void mouseCallback(unsigned int& count, tgui::Vector2f pos);
void genericCallback(unsigned int& count);

void testWidgetSignals(const tgui::Widget::Ptr& widget);
void testClickableWidgetSignals(const tgui::ClickableWidget::Ptr& widget);
void testClickableWidgetSignals(const tgui::Panel::Ptr& widget);

void testScrollbarAccess(tgui::ScrollbarAccessor *scrollbar);
void testWidgetRenderer(tgui::WidgetRenderer* renderer);

template <typename WidgetType>
void testSavingWidget(const tgui::String& name, std::shared_ptr<WidgetType> widget, bool loadFromTheme = true)
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
        temp2 = temp2; // NOLINT(misc-redundant-expression)
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

    SECTION("Loading empty widget")
    {
        try
        {
            // Loading an empty section may throw an error for some widgets, but it should never crash.
            // After an exception is thrown, attempting to use the widget may however result in a crash,
            // so we only test that attempting to load it works.
            parent->loadWidgetsFromStream(std::stringstream((widget->getWidgetType() + " {}").toStdString()), true);
        }
        catch (const tgui::Exception&)
        {
        }
    }
}

class GuiNull : public tgui::BackendGui
{
public:
    GuiNull()
    {
        setAbsoluteViewport({0, 0, 200, 200});
        setAbsoluteView({0, 0, 200, 200});
    }

    void draw() override {}
    void mainLoop(tgui::Color = {240, 240, 240}) override {}

    void handleTwoFingerScroll(bool wasAlreadyScrolling) { BackendGui::handleTwoFingerScroll(wasAlreadyScrolling); }

    tgui::TwoFingerScrollDetect& twoFingerScroll = m_twoFingerScroll;
};

extern tgui::BackendGui* globalGui;

#endif // TGUI_TESTS_HPP
