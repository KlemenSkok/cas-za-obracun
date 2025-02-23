
// Barrier.cpp

#include "Game/Map/Barrier.hpp"
#include "Rendering/RenderingContext.hpp"



void Barrier::render(SDL_Renderer* renderer) {
    SDL_Color tmp_c;
    SDL_GetRenderDrawColor(renderer, &tmp_c.r, &tmp_c.g, &tmp_c.b, &tmp_c.a);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0x00, 255);

    SDL_Rect rect;
    rect.x = static_cast<int>(position.x - rc::localPlayerPos.x + rc::windowCenter.x);
    rect.y = static_cast<int>(position.y - rc::localPlayerPos.y + rc::windowCenter.y);
    rect.w = width;
    rect.h = height;
    
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, tmp_c.r, tmp_c.g, tmp_c.b, tmp_c.a);
}

void Barrier::setPosition(float x, float y) {
    this->position.x = x;
    this->position.y = y;
}

void Barrier::setDimensions(int w, int h) {
    this->width = w;
    this->height = h;
}

void Barrier::setTexture(int id) {
    this->texture_id = id;
}
