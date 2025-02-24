
// Barrier.hpp

#pragma once

#include "Containers.hpp"

#include <SDL2/SDL.h>


class Barrier {

    Point position;
    int width, height;

    int texture_id;

public:

    Barrier() = default;
    void render(SDL_Renderer* renderer);

    void setPosition(float x, float y);
    void setDimensions(int w, int h);
    void setTexture(int id);
    Point getPosition() const { return position; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    //void loadTexture(SDL_Renderer* renderer);
    
};