
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
    SDL_Color tmp_c;
    SDL_GetRenderDrawColor(renderer, &tmp_c.r, &tmp_c.g, &tmp_c.b, &tmp_c.a);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 255);
    DrawFillCircleF(renderer, position.x, position.y, this->radius);
    SDL_SetRenderDrawColor(renderer, tmp_c.r, tmp_c.g, tmp_c.b, tmp_c.a);
}