
// RemotePlayer.cpp

#include "Game/RemotePlayer.hpp"
#include "Utilities/Utility.hpp"
#include "Utilities/Constants.hpp"
#include "Communication/PacketTypes.hpp"

#include <algorithm>


void RemotePlayer::update(float deltaTime) {

    // INTERPOLATION HERE

    if (!this->dataBuffer.empty()) {
        auto data = this->dataBuffer.front();
        decodeKeyStates(data.keyStates, this->keyStates);
        
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

    // reset acceleration
    this->acceleration.x = 0.0f;
    this->acceleration.y = 0.0f;

    // apply acceleration based on key states
    if(this->keyStates.w) this->acceleration.y -= PLAYER_ACCELERATION;
    if(this->keyStates.s) this->acceleration.y += PLAYER_ACCELERATION;
    if(this->keyStates.a) this->acceleration.x -= PLAYER_ACCELERATION;
    if(this->keyStates.d) this->acceleration.x += PLAYER_ACCELERATION;

    // update the player position
    this->velocity.x += this->acceleration.x * deltaTime;
    this->velocity.y += this->acceleration.y * deltaTime;

    // apply friction if no keys are pressed
    if(!this->keyStates.a && !this->keyStates.d) {
        if(this->velocity.x > 0) {
            this->velocity.x -= PLAYER_FRICTION * deltaTime;
            if(this->velocity.x < 0) this->velocity.x = 0;
        } else if(this->velocity.x < 0) {
            this->velocity.x += PLAYER_FRICTION * deltaTime;
            if(this->velocity.x > 0) this->velocity.x = 0;
        }
    }
    if(!this->keyStates.w && !this->keyStates.s) {
        if(this->velocity.y > 0) {
            this->velocity.y -= PLAYER_FRICTION * deltaTime;
            if(this->velocity.y < 0) this->velocity.y = 0;
        } else if(this->velocity.y < 0) {
            this->velocity.y += PLAYER_FRICTION * deltaTime;
            if(this->velocity.y > 0) this->velocity.y = 0;
        }
    }

    // clamp velocity
    if(this->velocity.x > PLAYER_MAX_SPEED) this->velocity.x = PLAYER_MAX_SPEED;
    if(this->velocity.x < -PLAYER_MAX_SPEED) this->velocity.x = -PLAYER_MAX_SPEED;
    if(this->velocity.y > PLAYER_MAX_SPEED) this->velocity.y = PLAYER_MAX_SPEED;
    if(this->velocity.y < -PLAYER_MAX_SPEED) this->velocity.y = -PLAYER_MAX_SPEED;

    // normalize diagonal velocity
    if(this->velocity.x == PLAYER_MAX_SPEED && this->velocity.y == PLAYER_MAX_SPEED) {
        this->velocity.x = PLAYER_MAX_SPEED / 1.4142f;
        this->velocity.y = PLAYER_MAX_SPEED / 1.4142f;
    }
    if(this->velocity.x == -PLAYER_MAX_SPEED && this->velocity.y == PLAYER_MAX_SPEED) {
        this->velocity.x = -PLAYER_MAX_SPEED / 1.4142f;
        this->velocity.y = PLAYER_MAX_SPEED / 1.4142f;
    }
    if(this->velocity.x == PLAYER_MAX_SPEED && this->velocity.y == -PLAYER_MAX_SPEED) {
        this->velocity.x = PLAYER_MAX_SPEED / 1.4142f;
        this->velocity.y = -PLAYER_MAX_SPEED / 1.4142f;
    }
    if(this->velocity.x == -PLAYER_MAX_SPEED && this->velocity.y == -PLAYER_MAX_SPEED) {
        this->velocity.x = -PLAYER_MAX_SPEED / 1.4142f;
        this->velocity.y = -PLAYER_MAX_SPEED / 1.4142f;
    }

    // update position
    this->position.x += this->velocity.x * deltaTime;
    this->position.y += this->velocity.y * deltaTime;

}

void RemotePlayer::render(SDL_Renderer* renderer) {
    // draw the player
    SDL_Color tmp_c;
    SDL_GetRenderDrawColor(renderer, &tmp_c.r, &tmp_c.g, &tmp_c.b, &tmp_c.a);
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 255);
    DrawFillCircleF(renderer, position.x, position.y, this->radius);
    SDL_SetRenderDrawColor(renderer, tmp_c.r, tmp_c.g, tmp_c.b, tmp_c.a);
}


void RemotePlayer::importData(const data_packets::PlayerData& data) {
    
    // push the data to the buffer
    this->dataBuffer.push(data);

}