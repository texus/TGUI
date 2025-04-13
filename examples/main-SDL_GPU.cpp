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

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SDL-GPU.hpp>
#include <SDL3/SDL_main.h>

bool runExample(tgui::BackendGui& gui);

// We don't put this code in main() to make sure that all TGUI resources are destroyed before destroying SDL
void run_application(SDL_Window* window, SDL_GPUDevice* device)
{
    tgui::Gui gui(window, device);
    if (!runExample(gui))
        return;

    gui.mainLoop(); // To use your own main loop, see https://tgui.eu/tutorials/latest-stable/backend-sdl-gpu/#main-loop
}

// Note that no error checking is performed on SDL initialization in this example code
int main(int, char **)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GPUDevice* device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL, false, nullptr);
    SDL_Window* window = SDL_CreateWindow("TGUI example (SDL-GPU)", 800, 600, SDL_WINDOW_RESIZABLE);
    SDL_ClaimWindowForGPUDevice(device, window);

    // SDL_ttf needs to be initialized before using TGUI
    TTF_Init();

    run_application(window, device);

    // Note that all TGUI resources must be destroyed before SDL_ttf is cleaned up
    TTF_Quit();

    SDL_ReleaseWindowFromGPUDevice(device, window);
	SDL_DestroyWindow(window);
	SDL_DestroyGPUDevice(device);
    SDL_Quit();
    return 0;
}
