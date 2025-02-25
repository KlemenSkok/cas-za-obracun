
// GameObject.hpp

#pragma once

#include <SDL2/SDL.h>
#include <vector>

#include "Containers.hpp"


class GameObject {

protected:

    PointF position;

public:
    GameObject(float x, float y) : position({x, y}) {}
    virtual ~GameObject() = default;
    virtual void update(float deltaTime) = 0;
    virtual void render(SDL_Renderer*) = 0;
    virtual PointF getPosition() const { return position; }
};