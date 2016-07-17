#include "Tests.hpp"
#include <TGUI/Widgets/Panel.hpp>

void mouseCallback(unsigned int& count, sf::Vector2f pos)
{
    count++;
    REQUIRE(pos == sf::Vector2f(75, 50));
}

void genericCallback(unsigned int& count)
{
    count++;
}

void testWidgetSignals(tgui::Widget::Ptr widget)
{
    // TODO: Test other signals than MouseEntered and MouseLeft

    SECTION("mouse move")
    {
        unsigned int mouseEnteredCount = 0;
        unsigned int mouseLeftCount = 0;

        widget->connect("MouseEntered", genericCallback, std::ref(mouseEnteredCount));
        widget->connect("MouseLeft", genericCallback, std::ref(mouseLeftCount));

        auto parent = std::make_shared<tgui::Panel>(300, 200);
        parent->add(widget);

        widget->setPosition(40, 30);
        widget->setSize(150, 100);

        parent->mouseMoved(10, 15);
        REQUIRE(mouseEnteredCount == 0);
        REQUIRE(mouseLeftCount == 0);

        parent->mouseMoved(40, 30);
        REQUIRE(mouseEnteredCount == 1);
        REQUIRE(mouseLeftCount == 0);

        parent->mouseMoved(189, 129);
        REQUIRE(mouseEnteredCount == 1);
        REQUIRE(mouseLeftCount == 0);

        parent->mouseMoved(190, 130);
        REQUIRE(mouseEnteredCount == 1);
        REQUIRE(mouseLeftCount == 1);
    }
}

void testClickableWidgetSignals(tgui::ClickableWidget::Ptr widget)
{
    testWidgetSignals(widget);

    unsigned int mousePressedCount = 0;
    unsigned int mouseReleasedCount = 0;
    unsigned int clickedCount = 0;

    widget->setPosition(40, 30);
    widget->setSize(150, 100);

    widget->connect("MousePressed", mouseCallback, std::ref(mousePressedCount));
    widget->connect("MouseReleased", mouseCallback, std::ref(mouseReleasedCount));
    widget->connect("Clicked", mouseCallback, std::ref(clickedCount));

    SECTION("mouseOnWidget")
    {
        REQUIRE(!widget->mouseOnWidget(10, 15));
        REQUIRE(widget->mouseOnWidget(40, 30));
        REQUIRE(widget->mouseOnWidget(115, 80));
        REQUIRE(widget->mouseOnWidget(189, 129));
        REQUIRE(!widget->mouseOnWidget(190, 130));

        REQUIRE(mousePressedCount == 0);
        REQUIRE(mouseReleasedCount == 0);
        REQUIRE(clickedCount == 0);
    }

    SECTION("mouse click")
    {
        widget->leftMouseReleased(115, 80);

        REQUIRE(mouseReleasedCount == 1);
        REQUIRE(clickedCount == 0);

        SECTION("mouse press")
        {
            widget->leftMousePressed(115, 80);

            REQUIRE(mousePressedCount == 1);
            REQUIRE(mouseReleasedCount == 1);
            REQUIRE(clickedCount == 0);
        }

        widget->leftMouseReleased(115, 80);

        REQUIRE(mousePressedCount == 1);
        REQUIRE(mouseReleasedCount == 2);
        REQUIRE(clickedCount == 1);
    }
}

void testWidgetRenderer(tgui::WidgetRenderer* renderer)
{
    SECTION("WidgetRenderer")
    {
        SECTION("set serialized property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("Opacity", "0.8"));
            REQUIRE_NOTHROW(renderer->setProperty("Font", "resources/DroidSansArmenian.ttf"));
        }

        SECTION("set object property")
        {
            REQUIRE_NOTHROW(renderer->setProperty("Opacity", 0.8f));
            REQUIRE_NOTHROW(renderer->setProperty("Font", tgui::Font{"resources/DroidSansArmenian.ttf"}));
        }

        SECTION("functions")
        {
            renderer->setOpacity(0.8f);
            renderer->setFont({"resources/DroidSansArmenian.ttf"});
        }

        REQUIRE(renderer->getProperty("Opacity").getNumber() == 0.8f);
        REQUIRE(renderer->getProperty("Font").getFont() != nullptr);

        REQUIRE(renderer->getOpacity() == 0.8f);
        REQUIRE(renderer->getFont().getId() == "resources/DroidSansArmenian.ttf");

        REQUIRE_THROWS_AS(renderer->setProperty("NonexistentProperty", ""), tgui::Exception);
    }
}
