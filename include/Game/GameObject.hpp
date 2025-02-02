
// GameObject.hpp

#pragma once

#include <SDL2/SDL.h>
#include <vector>


class GameObject {

protected:

    // position ni ravno vektor, ampak vseen
    struct { float x, y; } position;
    struct { float x, y; } velocity;
    struct { float x, y; } acceleration;

public:
    GameObject(float x, float y) : position({x, y}), velocity({0, 0}) {}
    virtual void update(float deltaTime) = 0;
    virtual void render(SDL_Renderer*) = 0;
    virtual ~GameObject() = default;
};