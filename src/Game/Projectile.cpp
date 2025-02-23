
// Projectile.cpp

#include "Game/Projectile.hpp"
#include "Utilities/Utility.hpp"
#include "Rendering/RenderingContext.hpp"

#include <algorithm>


void Projectile::update(float deltaTime) {
    //position.x += velocity.x * deltaTime;
    //position.y += velocity.y * deltaTime;

    // INTERPOLATION HERE

    if (!this->dataBuffer.empty()) {
        auto data = this->dataBuffer.front();
        float estimatedServerTime = data.timestamp + (SDL_GetTicks() - data.recv_ts);
    
        // Apply new packet if it's time
        if (estimatedServerTime >= data.timestamp) {
            this->dataBuffer.pop();
    
            // Smooth correction instead of snapping
            const float correctionFactor = 0.3f;
            this->position.x = lerp(this->position.x, data.position.x, correctionFactor);
            this->position.y = lerp(this->position.y, data.position.y, correctionFactor);
    
            this->velocity.x = data.velocity.x;
            this->velocity.y = data.velocity.y;
    
            this->lastData = data;
            this->lastUpdateTime = SDL_GetTicks();
            
            return;
        }
    
        // Interpolation
        float alpha = (estimatedServerTime - this->lastData.timestamp) / float(data.timestamp - this->lastData.timestamp);
        alpha = std::clamp(alpha, 0.0f, 1.0f);

        this->position.x = lerp(this->lastData.position.x, data.position.x, alpha);
        this->position.y = lerp(this->lastData.position.y, data.position.y, alpha);

        return;
    }

    // EXTRAPOLATION

    this->position.x += this->velocity.x * deltaTime;
    this->position.y += this->velocity.y * deltaTime;

}

void Projectile::render(SDL_Renderer* renderer) {
    // draw the projectile
    SDL_Color tmp_c;
    SDL_GetRenderDrawColor(renderer, &tmp_c.r, &tmp_c.g, &tmp_c.b, &tmp_c.a);
    SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0xFF, 255);

    // old
    //DrawFillCircleF(renderer, position.x, position.y, this->radius);

    Point pos = {rc::windowCenter.x + (this->position.x - rc::localPlayerPos.x), rc::windowCenter.y + (this->position.y - rc::localPlayerPos.y)};
    DrawFillCircleF(renderer, pos.x, pos.y, this->radius);

    SDL_SetRenderDrawColor(renderer, tmp_c.r, tmp_c.g, tmp_c.b, tmp_c.a);
}

void Projectile::importData(const data_packets::ProjectileData& data) {
    
    // add data to the buffer
    this->dataBuffer.push(data);

}