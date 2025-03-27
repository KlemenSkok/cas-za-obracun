
// RemotePlayer.cpp

#include "Game/RemotePlayer.hpp"
#include "Utilities/Utility.hpp"
#include "Utilities/Constants.hpp"
#include "Communication/PacketTypes.hpp"
#include "Rendering/RenderingContext.hpp"

#include <algorithm>


void RemotePlayer::update(float deltaTime) {

    PointF newPosition = this->position;
    
    auto _checkCollision = [&]() {
        // check for collision
        if(MapData::CheckCollision(*this, newPosition)) {
            // collision detected; update position
            this->position = newPosition;
        } else {
            // no collision; update position
            this->position = newPosition;
        }
    };
    
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
            newPosition.x = lerp(newPosition.x, data.position.x, correctionFactor);
            newPosition.y = lerp(newPosition.y, data.position.y, correctionFactor);

            // interpolate direction (smooth rotation)
            this->direction = lerpAngle(this->direction, data.direction, correctionFactor);

            this->velocity.x = data.velocity.x;
            this->velocity.y = data.velocity.y;
    
            this->lastData = data;
            this->lastUpdateTime = SDL_GetTicks();
            
            // update position
            newPosition.x += this->velocity.x * deltaTime;
            newPosition.y += this->velocity.y * deltaTime;

            // check for collision
            _checkCollision();

            return;
        }
    
        // Interpolation
        float alpha = (estimatedServerTime - this->lastData.timestamp) / float(data.timestamp - this->lastData.timestamp);
        alpha = std::clamp(alpha, 0.0f, 1.0f);

        newPosition.x = lerp(this->lastData.position.x, data.position.x, alpha);
        newPosition.y = lerp(this->lastData.position.y, data.position.y, alpha);

        // interpolate direction (smooth rotation)
        this->direction = lerpAngle(this->direction, data.direction, alpha);
    

        // update position
        newPosition.x += this->velocity.x * deltaTime;
        newPosition.y += this->velocity.y * deltaTime;

        // check for collision
        _checkCollision();

        return;
    }
    
    // EXTRAPOLATION

    // reset acceleration
    this->acceleration.x = 0.0f;
    this->acceleration.y = 0.0f;

    // apply acceleration based on key states
    if(this->keyStates.w) this->acceleration.y -= PLAYER_ACCELERATION * nextAcceleration_k;
    if(this->keyStates.s) this->acceleration.y += PLAYER_ACCELERATION * nextAcceleration_k;
    if(this->keyStates.a) this->acceleration.x -= PLAYER_ACCELERATION * nextAcceleration_k;
    if(this->keyStates.d) this->acceleration.x += PLAYER_ACCELERATION * nextAcceleration_k;

    float current_friction = PLAYER_FRICTION * this->nextFriction_k;

    // update the player position
    this->velocity.x += this->acceleration.x * deltaTime;
    this->velocity.y += this->acceleration.y * deltaTime;

    // apply friction if no keys are pressed
    if(!this->keyStates.a && !this->keyStates.d) {
        if(this->velocity.x > 0) {
            this->velocity.x -= current_friction * deltaTime;
            if(this->velocity.x < 0) this->velocity.x = 0;
        } else if(this->velocity.x < 0) {
            this->velocity.x += current_friction * deltaTime;
            if(this->velocity.x > 0) this->velocity.x = 0;
        }
    }
    if(!this->keyStates.w && !this->keyStates.s) {
        if(this->velocity.y > 0) {
            this->velocity.y -= current_friction * deltaTime;
            if(this->velocity.y < 0) this->velocity.y = 0;
        } else if(this->velocity.y < 0) {
            this->velocity.y += current_friction * deltaTime;
            if(this->velocity.y > 0) this->velocity.y = 0;
        }
    }

    // determine speed limit (players are slower when concussed)
    float speed_cap = (this->posture > 0) ? PLAYER_MAX_SPEED : PLAYER_MAX_SPEED_SLOWED;
    // override the speed cap if needed (because of traps)
    if(speed_cap > nextSpeedCap) {
        speed_cap = nextSpeedCap;
    }

    // clamp velocity
    if(this->velocity.x > speed_cap) this->velocity.x = speed_cap;
    if(this->velocity.x < -speed_cap) this->velocity.x = -speed_cap;
    if(this->velocity.y > speed_cap) this->velocity.y = speed_cap;
    if(this->velocity.y < -speed_cap) this->velocity.y = -speed_cap;

    // normalize diagonal velocity
    if(this->velocity.x == speed_cap && this->velocity.y == speed_cap) {
        this->velocity.x = speed_cap / 1.4142f;
        this->velocity.y = speed_cap / 1.4142f;
    }
    if(this->velocity.x == -speed_cap && this->velocity.y == speed_cap) {
        this->velocity.x = -speed_cap / 1.4142f;
        this->velocity.y = speed_cap / 1.4142f;
    }
    if(this->velocity.x == speed_cap && this->velocity.y == -speed_cap) {
        this->velocity.x = speed_cap / 1.4142f;
        this->velocity.y = -speed_cap / 1.4142f;
    }
    if(this->velocity.x == -speed_cap && this->velocity.y == -speed_cap) {
        this->velocity.x = -speed_cap / 1.4142f;
        this->velocity.y = -speed_cap / 1.4142f;
    }

    // reset movement settings
    this->nextAcceleration_k = 1.0f;
    this->nextFriction_k = 1.0f;
    this->nextSpeedCap = PLAYER_MAX_SPEED;


    // update position
    newPosition.x += this->velocity.x * deltaTime;
    newPosition.y += this->velocity.y * deltaTime;

    // check for collision
    _checkCollision();

}

void RemotePlayer::render(SDL_Renderer* renderer) {
    if(this->texture == nullptr) {
        // draw the player
        SDL_Color tmp_c;
        SDL_GetRenderDrawColor(renderer, &tmp_c.r, &tmp_c.g, &tmp_c.b, &tmp_c.a);
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 255);
        
        // Draw the player at the correct position
        PointF pos = {rc::windowCenter.x + (this->position.x - rc::localPlayerPos.x), rc::windowCenter.y + (this->position.y - rc::localPlayerPos.y)};
        DrawFillCircleF(renderer, pos.x, pos.y, PLAYER_RADIUS);
        
        SDL_SetRenderDrawColor(renderer, tmp_c.r, tmp_c.g, tmp_c.b, tmp_c.a);
    }
    else {
        SDL_Rect dest = {static_cast<int>(rc::windowCenter.x + (this->position.x - rc::localPlayerPos.x) - PLAYER_RADIUS), 
                         static_cast<int>(rc::windowCenter.y + (this->position.y - rc::localPlayerPos.y) - PLAYER_RADIUS), 
                         2 * PLAYER_RADIUS, 2 * PLAYER_RADIUS};
        SDL_RenderCopyEx(renderer, this->texture, nullptr, &dest, static_cast<double>(this->direction - 90.0f), nullptr, SDL_FLIP_NONE);
    }
}


void RemotePlayer::importData(const data_packets::PlayerData& data) {
    
    // push the data to the buffer
    this->dataBuffer.push(data);

    // update the posture immediately
    this->posture = data.posture;
    this->teamNumber = data.teamNumber;

}

void RemotePlayer::forceImportData(const data_packets::PlayerData& data) {

    // clear the buffer
    while(!this->dataBuffer.empty()) this->dataBuffer.pop();

    // update the player data
    this->position = data.position;
    this->velocity = data.velocity;
    this->posture = data.posture;
    this->teamNumber = data.teamNumber;
    decodeKeyStates(data.keyStates, this->keyStates);

    // update the last data
    this->lastData = data;
    this->lastUpdateTime = SDL_GetTicks();

    // direction is still interpolated in the next RemotePlayer::update call

}

void RemotePlayer::setNextFriction(float fk) {
    this->nextFriction_k = fk;
}

void RemotePlayer::setNextAcceleration(float ak) {
    this->nextAcceleration_k = ak;
}

void RemotePlayer::setNextSpeedCap(float s) {
    this->nextSpeedCap = s;
}

uint8_t RemotePlayer::getTeamNumber() const {
    return this->teamNumber;
}