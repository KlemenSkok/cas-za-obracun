
// Trap.cpp

#include "Game/Map/Trap.hpp"
#include "Utilities/Constants.hpp"
#include "Rendering/RenderingContext.hpp"

#include <cstring>


Trap::Trap() : 
    MapObject(MapObjType::TRAP)
{
    this->friction_k = 1.0f;
    this->acceleration_k = 1.0f;
    this->max_speed = PLAYER_MAX_SPEED;

    this->texture = nullptr;
}

void Trap::render(SDL_Renderer* renderer) {
    if(texture == nullptr) {
        SDL_Color tmp_c;
        SDL_GetRenderDrawColor(renderer, &tmp_c.r, &tmp_c.g, &tmp_c.b, &tmp_c.a);
        SDL_SetRenderDrawColor(renderer, 0x80, 0x00, 0xFF, 255);
    
        SDL_Rect rect;
        rect.x = static_cast<int>(position.x - rc::localPlayerPos.x + rc::windowCenter.x);
        rect.y = static_cast<int>(position.y - rc::localPlayerPos.y + rc::windowCenter.y);
        rect.w = size.x;
        rect.h = size.y;
        
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, tmp_c.r, tmp_c.g, tmp_c.b, tmp_c.a);
    }
    else {
        SDL_Rect rect = {static_cast<int>(position.x - rc::localPlayerPos.x + rc::windowCenter.x), 
                         static_cast<int>(position.y - rc::localPlayerPos.y + rc::windowCenter.y), 
                         size.x, size.y};
    }
}


void Trap::setTrapType(char* t) {

    //std::cout << "Setting trap type: " << t << ".\n";
    if(!strcmp(t, "ice")) {
        this->friction_k = 0.1f;
        this->acceleration_k = 0.1f;
        this->max_speed = PLAYER_MAX_SPEED;
    }
    else if(!strcmp(t, "sand")) {
        this->friction_k = 10.0f;
        this->acceleration_k = 0.5f;
        this->max_speed = PLAYER_MAX_SPEED / 4;
    }

}

void Trap::setFriction(float kf) {
    this->friction_k = kf;
}
void Trap::setAccel(float ka) {
    this->acceleration_k = ka;
}
void Trap::setSpeed(float s) {
    this->max_speed = s;
}

float Trap::getFrictionCoefficient() const {
    return this->friction_k;
}
float Trap::getAccelerationCoefficient() const {
    return this->acceleration_k;
}
float Trap::getMaxSpeed() const {
    return this->max_speed;
}