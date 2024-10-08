
// main.cpp

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>
#include <iostream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize SDL_ttf
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create the window
    SDL_Window* window = SDL_CreateWindow("Hello World Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Create a renderer for the window
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Set the background color (white)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Load the font (make sure you have a .ttf font file in the directory)
    // ! the font path goes from the executable directory
    TTF_Font* font = TTF_OpenFont("../../assets/fonts/CourierPrime-Regular.ttf", 28);
    if (font == NULL) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Set the color of the text (black)
    SDL_Color textColor = {0, 0, 0};

    // Render the text "Hello World"
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Hello World", textColor);
    if (textSurface == NULL) {
        std::cerr << "Unable to render text surface! TTF_Error: " << TTF_GetError() << std::endl;
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Create a texture from the surface
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture == NULL) {
        std::cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    // Get the width and height of the text
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;

    // Free the text surface
    SDL_FreeSurface(textSurface);

    // Create a rectangle to position the text in the center of the window
    SDL_Rect renderQuad = { (SCREEN_WIDTH - textWidth) / 2, (SCREEN_HEIGHT - textHeight) / 2, textWidth, textHeight };

    // Render the texture in the rectangle
    SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);

    // Update the screen
    SDL_RenderPresent(renderer);

    // Pause for 10 seconds before closing
    SDL_Delay(10000);

    // Clean up
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
