
// Barrier.hpp

#pragma once

#include "MapObject.hpp"

#include <SDL2/SDL.h>


class Barrier : public MapObject {

public:

    Barrier() : MapObject(MapObjType::BARRIER) {}
    
    void render(SDL_Renderer* renderer) override;
    
};