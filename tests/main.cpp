#define CATCH_CONFIG_RUNNER
#include "Tests.hpp"

#include <TGUI/TextureManager.hpp>

int main (int argc, char * argv[])
{
    // All tests are performed while a Gui object exists.
    // This will keep the global font alive instead of being destructed and reconstructed multiple times between the tests.
    tgui::GuiSFML gui;

    return Catch::Session().run(argc, argv);
}

TEST_CASE("[Memory leak test]")
{
    REQUIRE(tgui::TextureManager::getCachedImagesCount() == 0);
}
