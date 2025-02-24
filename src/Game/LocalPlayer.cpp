
// LocalPlayer.cpp

#include "Game/LocalPlayer.hpp"
#include "Utilities/Utility.hpp"
#include "Rendering/RenderingContext.hpp"



void LocalPlayer::update(float deltaTime) {

    // reset acceleration
    this->acceleration.x = 0.0f;
    this->acceleration.y = 0.0f;

    // apply acceleration based on key states
    if(EventHandler::keyStates.s) this->acceleration.y += PLAYER_ACCELERATION;
    if(EventHandler::keyStates.w) this->acceleration.y -= PLAYER_ACCELERATION;
    if(EventHandler::keyStates.a) this->acceleration.x -= PLAYER_ACCELERATION;
    if(EventHandler::keyStates.d) this->acceleration.x += PLAYER_ACCELERATION;

    // update the player position
    this->velocity.x += this->acceleration.x * deltaTime;
    this->velocity.y += this->acceleration.y * deltaTime;

    Point newPosition = { this->position.x, this->position.y };

    newPosition.x += this->velocity.x * deltaTime;
    newPosition.y += this->velocity.y * deltaTime;

    // apply friction if no keys are pressed
    if(!EventHandler::keyStates.a && !EventHandler::keyStates.d) {
        if(this->velocity.x > 0) {
            this->velocity.x -= PLAYER_FRICTION * deltaTime;
            if(this->velocity.x < 0) this->velocity.x = 0;
        } else if(this->velocity.x < 0) {
            this->velocity.x += PLAYER_FRICTION * deltaTime;
            if(this->velocity.x > 0) this->velocity.x = 0;
        }
    }
    if(!EventHandler::keyStates.w && !EventHandler::keyStates.s) {
        if(this->velocity.y > 0) {
            this->velocity.y -= PLAYER_FRICTION * deltaTime;
            if(this->velocity.y < 0) this->velocity.y = 0;
        } else if(this->velocity.y < 0) {
            this->velocity.y += PLAYER_FRICTION * deltaTime;
            if(this->velocity.y > 0) this->velocity.y = 0;
        }
    }

    // determine speed limit (players are slower when concussed)
    float speed_cap = (this->posture > 0) ? PLAYER_MAX_SPEED : PLAYER_MAX_SPEED_SLOWED;

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

    // update position
    newPosition.x += this->velocity.x * deltaTime;
    newPosition.y += this->velocity.y * deltaTime;

    // check for collision
    if(MapData::CheckCollision(*this, newPosition)) {
        // collision detected; update position
        this->position = newPosition;
    } else {
        // no collision; update position
        this->position = newPosition;
    }

}

void LocalPlayer::render(SDL_Renderer* renderer) {
    // draw the player
    SDL_Color tmp_c;
    SDL_GetRenderDrawColor(renderer, &tmp_c.r, &tmp_c.g, &tmp_c.b, &tmp_c.a);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 255);
    
    // old
    //DrawFillCircleF(renderer, position.x, position.y, PLAYER_RADIUS);

    // Draw the player at the correct position (middle of the window)
    DrawFillCircleF(renderer, rc::windowCenter.x, rc::windowCenter.y, PLAYER_RADIUS);

    
    SDL_SetRenderDrawColor(renderer, tmp_c.r, tmp_c.g, tmp_c.b, tmp_c.a);
}

data_packets::PlayerKeyStates LocalPlayer::dumpKeyStates() {
    data_packets::PlayerKeyStates pks;

    // key states
    pks.keyStates = encodeKeyStates(EventHandler::keyStates);
    
    return pks;
}

void LocalPlayer::importData(const data_packets::PlayerData& data) {

    // update posture
    this->posture = data.posture;

    // direction
    this->direction = data.direction;

    if(data.keyStates != encodeKeyStates(EventHandler::keyStates)) {
        if(data.recv_ts < PacketHandler::lastSentPacketTime) {
            // the recent player update was probably not received by the server
            // resend the update
            PacketHandler::sendPlayerUpdate();
            std::cout << "Correction!\n";
        }
    }

    float errorX = std::abs(this->position.x - data.position.x);
    float errorY = std::abs(this->position.y - data.position.y);

    // If the position is off by too much, smoothly correct it
    if (errorX > POSITION_OFFSET_TOLERANCE || errorY > POSITION_OFFSET_TOLERANCE) {
        const float correctionFactor = 0.1f; // Adjust between 0.1 (slow correction) and 1.0 (instant)
        this->position.x = lerp(this->position.x, data.position.x, correctionFactor);
        this->position.y = lerp(this->position.y, data.position.y, correctionFactor);
    }

    this->lastUpdateTime = SDL_GetTicks();
}

float LocalPlayer::getDirection() const {
    return this->direction;
}

Point LocalPlayer::getPosition() const {
    return this->position;
}