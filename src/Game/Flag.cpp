
// Flag.cpp

#include "Game/Flag.hpp"
#include "Rendering/RenderingContext.hpp"


void Flag::update(float deltaTime) {
    // Update flag position, apply lerp
}

void Flag::render(SDL_Renderer* renderer) {
    // draw the flag
    SDL_Color tmp_c;
    SDL_GetRenderDrawColor(renderer, &tmp_c.r, &tmp_c.g, &tmp_c.b, &tmp_c.a);
    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 255);

    // Draw the player at the correct position
    SDL_Rect rect;
    rect.x = rc::windowCenter.x + (this->position.x - rc::localPlayerPos.x);
    rect.y = rc::windowCenter.y + (this->position.y - rc::localPlayerPos.y);
    rect.w = this->size.x;
    rect.h = this->size.y;

    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, tmp_c.r, tmp_c.g, tmp_c.b, tmp_c.a);

}