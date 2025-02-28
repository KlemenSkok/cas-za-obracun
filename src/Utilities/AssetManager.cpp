
// AssetManager.cpp

#include "Utilities/AssetManager.hpp"
#include "Utilities/Constants.hpp"
#include "Utilities/Utility.hpp"
#include "Logging/Logger.hpp"


#include <SDL2/SDL_image.h>


SDL_Texture* AssetManager::LoadTexture(const char* fileName) {

    std::string path = std::string(ASSETS_PATH) + fileName;

    SDL_Texture* texture = IMG_LoadTexture(Window::renderer, path.c_str());

    if(texture == nullptr) {
        Logger::warn(("Failed to load texture: " + path).c_str());
    }

    return texture;
    
}