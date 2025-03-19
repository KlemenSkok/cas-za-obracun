
// Renderer.hpp

#pragma once

#include "Game.hpp"
#include "Containers.hpp"
#include "Utilities/Utility.hpp"

#include <unordered_map>
#include <SDL2/SDL.h>


/**
 * @brief Class to handle game state rendering
 * 
 */
class RenderWindow {

    static std::unordered_map<uint8_t, SDL_Texture*> screens;
    static SDL_Texture* screen_victory;
    static SDL_Texture* screen_defeat;
    
    static RenderState prev_state;

public:

    static void loadScreens(); // Load all textures
    static void renderWindow(); // Main rendering function

    static void renderCurrentScreen();
    static void renderGameState();
    static void renderGameUI();

};