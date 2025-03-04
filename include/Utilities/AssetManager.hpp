
#pragma once

#include <SDL2/SDL.h>
#include <unordered_map>

class AssetManager {

    static std::unordered_map<int, SDL_Texture*> textures;

public:

    static SDL_Texture* LoadTexture(const char* fileName);
    static SDL_Texture* GetTexture(int id);
    static void Clear();
    static void Initialize();

};