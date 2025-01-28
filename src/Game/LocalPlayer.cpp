
// LocalPlayer.cpp

#include "Game/LocalPlayer.hpp"
#include "Utilities/Utility.hpp"



void LocalPlayer::update(float deltaTime) {
    // update player position
    position.x += velocity.x * deltaTime;
    position.y += velocity.y * deltaTime;
}

void LocalPlayer::render(SDL_Renderer* renderer) {
    // draw the player
    DrawFillCircleF(renderer, position.x, position.y, this->radius);
}