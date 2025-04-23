
// AssetManager.cpp

#include "Utilities/AssetManager.hpp"
#include "Utilities/Constants.hpp"
#include "Utilities/Utility.hpp"
#include "Logging/Logger.hpp"


#include <SDL2/SDL_image.h>


std::unordered_map<int, SDL_Texture*> AssetManager::textures;



SDL_Texture* AssetManager::LoadTexture(const char* fileName) {

    std::string path = std::string(ASSETS_PATH) + fileName;

    SDL_Texture* texture = IMG_LoadTexture(Window::renderer, path.c_str());

    if(texture == nullptr) {
        Logger::warn(("Failed to load texture: " + path).c_str());
    }
    else {
        //Logger::info(("Texture loaded: " + path).c_str());
    }

    return texture;
    
}


SDL_Texture* AssetManager::GetTexture(int id) {
    return textures[id];
}


void AssetManager::Clear() {
    for(auto& pair : AssetManager::textures) {
        if(pair.second != nullptr) {
            SDL_DestroyTexture(pair.second);
            pair.second = nullptr;
        }
    }
    AssetManager::textures.clear();
}


void AssetManager::Initialize() {
    // load textures
    textures[TEXTURE_PLAYER_1] =            LoadTexture("images/player1.png");
    textures[TEXTURE_PLAYER_2] =            LoadTexture("images/player2.png");
    textures[TEXTURE_FLAG] =                LoadTexture("images/flag.png");
    textures[TEXTURE_PROJECTILE_1] =        LoadTexture("images/projectile1.png");
    textures[TEXTURE_PROJECTILE_2] =        LoadTexture("images/projectile2.png");

    // load map components
    textures[TEXTURE_SITE_1] =              LoadTexture("images/map/site_team_1.png");
    textures[TEXTURE_SITE_2] =              LoadTexture("images/map/site_team_2.png");

    textures[TEXTURE_SOFA] =                LoadTexture("images/map/sofa.png");
    textures[TEXTURE_CRATE] =               LoadTexture("images/map/crate.png");
    textures[TEXTURE_DOUBLE_CRATE] =        LoadTexture("images/map/double_crate.png");
    textures[TEXTURE_FLOOR] =               LoadTexture("images/map/floor_tile.png");
    textures[TEXTURE_FLOOR_BORDER] =        LoadTexture("images/map/floor_border.png");

    textures[TEXTURE_WATER_TRAP] =          LoadTexture("images/map/water_trap.png");
    textures[TEXTURE_SAND_TRAP] =           LoadTexture("images/map/sand_trap.png");

    // load GUI textures    
    textures[TEXTURE_ARROW_NEUTRAL] =       LoadTexture("images/GUI/components/arrow_neutral.png");
    textures[TEXTURE_ARROW_BLUE] =          LoadTexture("images/GUI/components/arrow_blue.png");
    textures[TEXTURE_ARROW_RED] =           LoadTexture("images/GUI/components/arrow_red.png");
    textures[TEXTURE_ROUND_WON] =           LoadTexture("images/GUI/components/round_won.png");
    textures[TEXTURE_ROUND_LOST] =          LoadTexture("images/GUI/components/round_lost.png");
    textures[TEXTURE_WAITING_FOR_PLAYERS] = LoadTexture("images/GUI/components/waiting_for_players.png");

}