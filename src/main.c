#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    // start SDL3's video system so we can create a window
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL initialization failed: %s", SDL_GetError());
        return 1; // Exit if SDL3 couldn't start
    }
    

    // create a window titled NESter that's 768 pixels wide and 720 tall
    SDL_Window *window = SDL_CreateWindow(
        "NESter",
        768,
        720,
        0
    );

    // make sure the window was created successfully
    if (window == NULL) {
        SDL_Log("Window creation failed: %s", SDL_GetError());
        SDL_Quit(); // Shut down SDL3 before exiting
        return 1;
    }

    // create a renderer so SDL3 can draw graphics inside the window
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

    if (renderer == NULL) {
        SDL_Log("Renderer creation failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // create a texture to hold the NES's 256x240 pixel image
    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        256,
        240
    );

    // check that the texture was created successfully
    if (texture == NULL) {
        SDL_Log("Texture creation failed: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // keep NES graphics sharp when scaling them up
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    // create an array to hold all 61,440 pixels of the NES screen
    uint32_t framebuffer[256 * 240];

    // fill the framebuffer with a simple four-color test pattern
    // this will be replaced with the NES's actual graphics in the future
    for (int y = 0; y < 240; y++) {
        for (int x = 0; x < 256; x++) {

            uint8_t r = 0;
            uint8_t g = 0;
            uint8_t b = 0;

            if (x < 128 && y < 120) {
                r = 255; // top-left: red
            }
            else if (x >= 128 && y < 120) {
                b = 255; // top-right: blue
            }
            else if (x < 128) {
                g = 255; // bottom-left: green
            }
            else {
                r = 255;
                g = 255; // bottom-right: yellow
            }

            // convert RGB values into the texture's RGBA8888 format
            framebuffer[y * 256 + x] =
                SDL_MapRGBA(SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA8888),
                            NULL, r, g, b, 255);
        }
    }

    bool running = true; // keeps track of whether the program should run
    SDL_Event event;     // holds information about events like key presses or closing the window

    // keep the program running until the user closes the window
    while (running) {

        // check and handle all events waiting in SDL3's event queue
        while (SDL_PollEvent(&event)) {

            // if the user closes the window, stop the program
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        // copy the framebuffer's pixels into the SDL3 texture
        SDL_UpdateTexture(texture, NULL, framebuffer, 256 * sizeof(uint32_t));

        // clear the previous image from the renderer
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // draw the NES texture across the entire window
        SDL_RenderTexture(renderer, texture, NULL, NULL);

        // display the completed image
        SDL_RenderPresent(renderer);

        // wait 16 milliseconds to keep the loop from using unnecessary CPU power
        SDL_Delay(16);
    }

    // release SDL3 resources in reverse order of creation
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0; // exit successfully
}