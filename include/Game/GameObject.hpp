
// GameObject.hpp

#pragma once

#include <SDL2/SDL.h>


class GameObject {

protected:
    typedef struct {
        float x, y;
    } Vector2D;

    // position ni ravno vektor, ampak vseen
    Vector2D position;
    Vector2D velocity;
    Vector2D acceleration;

public:
    GameObject(float x, float y) : position({x, y}), velocity({0, 0}) {}
    virtual void update(float deltaTime) = 0;
    virtual void render(SDL_Renderer*) = 0;
    virtual ~GameObject() = default;
};