#include "display.h"
#include "SDL.h"
#include <string>
#include <print>
#include <cmath>

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
        std::println("SDL window creation worked!");
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
        // std::println here does not flush stdout
        std::println("FPS: {:>7.3f}", 1000. / delta);
        std::fflush(stdout);

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

void rainbowTest() {
    SDL_Window* window = SDL_CreateWindow(
        "Rainbow Test",
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

    uint64_t a, b, delta, offset = 0;
    a = SDL_GetTicks64();
    while (1) {
        offset++;

        // Calculate the current framerate using ticks since the last frame
        b = SDL_GetTicks64();
        delta = b - a;
        a = b;
        std::println("FPS: {:>7.3f}", 1000. / delta);
        std::fflush(stdout);

        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            break;
        }

        int pitch; // Should be NES_DISPLAY_WIDTH * sizeof(uint32_t)
        // Prepare the texture with random color data
        SDL_LockTexture(texture, NULL, (void**)&pixelData, &pitch);

        for (int i = 0; i < NES_DISPLAY_WIDTH * NES_DISPLAY_HEIGHT; i++) {
            int r_x = (int)(i + offset) % NES_DISPLAY_WIDTH;
            int b_x = (int)(i + offset * 2.7183) % NES_DISPLAY_WIDTH;
            int g_x = (int)(i + offset * 2.3026) % NES_DISPLAY_WIDTH;
            double mult = 2 * M_PI / NES_DISPLAY_WIDTH;
            uint8_t red = static_cast<uint8_t>((sin(mult * r_x) / 2 + .5) * 235 + 20),
                    blue = static_cast<uint8_t>((sin(mult * b_x) / 2 + .5) * 235 + 20),
                    green = static_cast<uint8_t>((sin(mult * g_x) / 2 + .5) * 235 + 20);

            uint32_t color = SDL_MapRGB(pixelFormat, red, green, blue);
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
    }else if (testType == "rainbow") {
        rainbowTest();
    }

    SDL_Quit();
}
