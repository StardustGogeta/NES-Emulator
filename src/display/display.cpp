#include "display.h"
#include "SDL.h"
#include <string>
#include <iostream>

const int NES_DISPLAY_WIDTH = 256, NES_DISPLAY_HEIGHT = 240;

void rectangleTest() {
    SDL_Window* window = SDL_CreateWindow(
        "Rectangle Test",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        NES_DISPLAY_WIDTH * 2, NES_DISPLAY_HEIGHT * 2, SDL_WINDOW_RESIZABLE
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Rect r;
    r.w = 100;
    r.h = 50;

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET, NES_DISPLAY_WIDTH * 2, NES_DISPLAY_HEIGHT * 2);

    if (window) {
        std::cout << "SDL window creation worked!" << std::endl;
        SDL_Event event;

        while (1) {
            SDL_PollEvent(&event);

            if (event.type == SDL_QUIT) {
                break;
            }

            // Set random rectangle position
            r.x = rand() % 500;
            r.y = rand() % 500;

            // Prepare to render to texture
            SDL_SetRenderTarget(renderer, texture);
            // Set color to black
            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
            // Fill screen with black
            SDL_RenderClear(renderer);
            // Set color to red
            SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x00);
            // Fill a red rectangle
            SDL_RenderFillRect(renderer, &r);
            // Render to the window
            SDL_SetRenderTarget(renderer, nullptr);
            // Copy the texture to the rendering target
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
            // Update the screen
            SDL_RenderPresent(renderer);
        }

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    }
}

void noiseTest() {
    SDL_Window* window = SDL_CreateWindow(
        "Noise Test",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        NES_DISPLAY_WIDTH * 2, NES_DISPLAY_HEIGHT * 2, SDL_WINDOW_RESIZABLE
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, NES_DISPLAY_WIDTH, NES_DISPLAY_HEIGHT);
    SDL_SetRenderTarget(renderer, nullptr);// Set color to black
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    // Fill screen with black
    SDL_RenderClear(renderer);

    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING, NES_DISPLAY_WIDTH, NES_DISPLAY_HEIGHT);
    // SDL_PIXELFORMAT_RGBA8888
    uint32_t format;
    SDL_QueryTexture(texture, &format, nullptr, nullptr, nullptr); // 373694468
    SDL_PixelFormat* pixelFormat = SDL_AllocFormat(format);

    SDL_Event event;

    // Prepare a pixel data buffer
    uint32_t* pixelData = nullptr;

    uint64_t a, b, delta;
    a = SDL_GetTicks64();
    while (1) {
        // Calculate the current framerate using ticks since the last frame
        b = SDL_GetTicks64();
        delta = b - a;
        a = b;
        std::cout << "FPS: " << 1000. / delta << std::endl;

        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            break;
        }

        int pitch; // Should be NES_DISPLAY_WIDTH * sizeof(uint32_t)
        // Prepare the texture with random color data
        SDL_LockTexture(texture, NULL, (void**)&pixelData, &pitch);
        for (int i = 0; i < NES_DISPLAY_WIDTH * NES_DISPLAY_HEIGHT; i++) {
            double red =    (double)rand() / RAND_MAX,
                   blue =   (double)rand() / RAND_MAX,
                   green =  (double)rand() / RAND_MAX;

            uint32_t color = SDL_MapRGB(pixelFormat, (uint8_t)(red * 256), (uint8_t)(green * 256), (uint8_t)(blue * 256));
            pixelData[i] = color;
            //printf("%d\n", counter);
        }
        SDL_UnlockTexture(texture);

        // Copy the texture to the rendering target
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        // Update the screen
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void runDisplayTest(std::string testType) {
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        exit(1);
    }
    
    if (testType == "rectangle") {
        rectangleTest();
    } else if (testType == "noise") {
        noiseTest();
    }

    SDL_Quit();
}
