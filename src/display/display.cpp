#include "display.h"
#include "SDL.h"
#include <string>
#include <iostream>

void runDisplayTest(std::string testType) {
    SDL_Init(SDL_INIT_VIDEO);

    if (testType == "rectangle") {
        SDL_Window* window = SDL_CreateWindow(
            "Rectangle Test",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            512, 480, SDL_WINDOW_RESIZABLE
        );

        SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

        SDL_Rect r;
        r.w = 100;
        r.h = 50;

        SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 512, 480);

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
                SDL_SetRenderTarget(renderer, NULL);
                // Copy the texture to the rendering target
                SDL_RenderCopy(renderer, texture, NULL, NULL);
                // Update the screen
                SDL_RenderPresent(renderer);
            }

            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
        }
    }
}
