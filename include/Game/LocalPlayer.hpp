
// LocalPlayer.hpp

#pragma once

#include "Game/GameObject.hpp"
#include "Game.hpp"


class LocalPlayer : public GameObject {
    float direction;
    int radius = 10;

    friend class Game;

public:
    LocalPlayer(float x, float y, float direction) : GameObject(x, y), direction(direction) {}

    void update(float deltaTime) override;
    void render(SDL_Renderer*) override;

};