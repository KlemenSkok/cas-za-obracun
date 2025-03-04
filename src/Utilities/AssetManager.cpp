
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

    return texture;
    
}


SDL_Texture* AssetManager::GetTexture(int id) {
    return textures[id];
}

void AssetManager::Clear() {
    for(auto& pair : AssetManager::textures) {
        SDL_DestroyTexture(pair.second);
    }
    AssetManager::textures.clear();
}



void AssetManager::Initialize() {
    // load textures
    textures[TEXTURE_PLAYER_1] = LoadTexture("images/player1.png");
    textures[TEXTURE_PLAYER_2] = LoadTexture("images/player2.png");
    
}