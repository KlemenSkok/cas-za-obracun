
// Site.cpp

#include "Game/Site.hpp"
#include "Rendering/RenderingContext.hpp"



void Site::setPosition(const PointF& p) {
    this->position = p;
}

void Site::setSize(const Point& s) {
    this->size = s;
}

Point Site::getSize() const {
    return this->size;
}

void Site::setTeam(uint8_t t) {
    this->teamNumber = t;
}

uint8_t Site::getTeam() const {
    return this->teamNumber;
}

bool Site::checkFlagCollision(const PointF& f_pos, const Point& f_size) {
    
    // the flag should be entirely inside the site to count as captured
    return  f_pos.x >= this->position.x && 
            f_pos.y >= this->position.y && 
            (f_pos.x + f_size.x) <= (this->position.x + this->size.x) && 
            (f_pos.y + f_size.y) <= (this->position.y + this->size.y);
        
}


void Site::update(float deltaTime) { }

void Site::render(SDL_Renderer* renderer) {
    
    SDL_Rect dest = {static_cast<int>(rc::windowCenter.x + (this->position.x - rc::localPlayerPos.x)), 
                     static_cast<int>(rc::windowCenter.y + (this->position.y - rc::localPlayerPos.y)), 
                     this->size.x, this->size.y};
    SDL_RenderCopy(renderer, this->texture, nullptr, &dest);

}