
// Barrier.cpp

#include "Game/Map/Barrier.hpp"
#include "Rendering/RenderingContext.hpp"



void Barrier::render(SDL_Renderer* renderer) {
    if(texture == nullptr) {
        SDL_Color tmp_c;
        SDL_GetRenderDrawColor(renderer, &tmp_c.r, &tmp_c.g, &tmp_c.b, &tmp_c.a);
        SDL_SetRenderDrawColor(renderer, 0x87, 0x7d, 0x61, 255);
        
        SDL_Rect rect;
        rect.x = static_cast<int>(position.x - rc::localPlayerPos.x + rc::windowCenter.x);
        rect.y = static_cast<int>(position.y - rc::localPlayerPos.y + rc::windowCenter.y);
        rect.w = size.x;
        rect.h = size.y;
        
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255);
        SDL_RenderDrawRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, tmp_c.r, tmp_c.g, tmp_c.b, tmp_c.a);
    }
    else {
        SDL_Rect dest = {static_cast<int>(rc::windowCenter.x + (position.x - rc::localPlayerPos.x)), 
                         static_cast<int>(rc::windowCenter.y + (position.y - rc::localPlayerPos.y)), 
                         size.x, size.y};
        SDL_RenderCopy(renderer, texture, nullptr, &dest);
    }
}
