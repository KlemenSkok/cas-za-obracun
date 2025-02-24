
// GameObject.hpp

#pragma once

#include <SDL2/SDL.h>
#include <vector>

#include "Containers.hpp"


class GameObject {

protected:

    Point position;
    Point velocity;
    Point acceleration;

public:
    GameObject(float x, float y) : position({x, y}), velocity({0, 0}) {}
    virtual ~GameObject() = default;
    virtual void update(float deltaTime) = 0;
    virtual void render(SDL_Renderer*) = 0;
};