#define CATCH_CONFIG_RUNNER
#include "Tests.hpp"

#include <TGUI/TextureManager.hpp>
#include <TGUI/DefaultBackendWindow.hpp>

int main(int argc, char * argv[])
{
    // Although the tests need a window, we create one for the backends that require it (e.g. SDL),
    // and to not bother with the different backends in this code.
    auto window = tgui::DefaultBackendWindow::create(250, 100, "TGUI Tests");

    return Catch::Session().run(argc, argv);
}

TEST_CASE("[Memory leak test]")
{
    REQUIRE(tgui::TextureManager::getCachedImagesCount() == 0);
}
