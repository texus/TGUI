#define CATCH_CONFIG_RUNNER
#include "Tests.hpp"

#include <TGUI/TextureManager.hpp>
#include <TGUI/Backend.hpp>

#if TGUI_HAS_BACKEND_SFML
    #include <TGUI/Backends/SFML/BackendSFML.hpp>
#elif TGUI_HAS_BACKEND_SDL
    #include <TGUI/Backends/SDL/BackendSDL.hpp>
    #include <TGUI/DefaultBackendWindow.hpp>
#endif

int main(int argc, char * argv[])
{
    // We create the backend manually so that there is always a backend present even while the tests create empty gui objects
#if TGUI_HAS_BACKEND_SFML
    tgui::setBackend(std::make_shared<tgui::BackendSFML>());
#elif TGUI_HAS_BACKEND_SDL
    // SDL/OpenGL backend requires a GL context, so we need to create a window
    auto window = tgui::DefaultBackendWindow::create(250, 100, "TGUI Tests");
#else
    static_assert(false, "Tests require a backend");
#endif

    const int retVal = Catch::Session().run(argc, argv);

#if TGUI_HAS_BACKEND_SFML
    tgui::setBackend(nullptr);
#endif

    return retVal;
}

TEST_CASE("[Memory leak test]")
{
    REQUIRE(tgui::TextureManager::getCachedImagesCount() == 0);
}
