
// LocalPlayer.cpp

#include "Game/LocalPlayer.hpp"
#include "Utilities/Utility.hpp"
#include "Rendering/RenderingContext.hpp"



void LocalPlayer::update(float deltaTime) {

    // reset acceleration
    this->acceleration.x = 0.0f;
    this->acceleration.y = 0.0f;

    // apply acceleration based on key states
    if(EventHandler::keyStates.s) this->acceleration.y += PLAYER_ACCELERATION * this->nextAcceleration_k;
    if(EventHandler::keyStates.w) this->acceleration.y -= PLAYER_ACCELERATION * this->nextAcceleration_k;
    if(EventHandler::keyStates.a) this->acceleration.x -= PLAYER_ACCELERATION * this->nextAcceleration_k;
    if(EventHandler::keyStates.d) this->acceleration.x += PLAYER_ACCELERATION * this->nextAcceleration_k;

    // update the player position
    this->velocity.x += this->acceleration.x * deltaTime;
    this->velocity.y += this->acceleration.y * deltaTime;

    PointF newPosition = { this->position.x, this->position.y };
    float current_friction = PLAYER_FRICTION * this->nextFriction_k;

    newPosition.x += this->velocity.x * deltaTime;
    newPosition.y += this->velocity.y * deltaTime;

    // apply friction if no keys are pressed
    if(!EventHandler::keyStates.a && !EventHandler::keyStates.d) {
        if(this->velocity.x > 0) {
            this->velocity.x -= current_friction * deltaTime;
            if(this->velocity.x < 0) this->velocity.x = 0;
        } else if(this->velocity.x < 0) {
            this->velocity.x += current_friction * deltaTime;
            if(this->velocity.x > 0) this->velocity.x = 0;
        }
    }
    if(!EventHandler::keyStates.w && !EventHandler::keyStates.s) {
        if(this->velocity.y > 0) {
            this->velocity.y -= current_friction * deltaTime;
            if(this->velocity.y < 0) this->velocity.y = 0;
        } else if(this->velocity.y < 0) {
            this->velocity.y += current_friction * deltaTime;
            if(this->velocity.y > 0) this->velocity.y = 0;
        }
    }

    // determine speed limit (players are slower when concussed)
    float speed_cap = (this->posture == 0) ? PLAYER_MAX_SPEED_SLOWED : (hasFlag) ? PLAYER_MAX_SPEED_CARRYING : PLAYER_MAX_SPEED;
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
    if(MapData::CheckCollision(*this, newPosition)) {
        // collision detected; update position
        this->position = newPosition;
    } else {
        // no collision; update position
        this->position = newPosition;
    }

}

void LocalPlayer::render(SDL_Renderer* renderer) {
    if(this->texture == nullptr) {
        // draw the player as a red circle
        SDL_Color tmp_c;
        SDL_GetRenderDrawColor(renderer, &tmp_c.r, &tmp_c.g, &tmp_c.b, &tmp_c.a);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 255);
        
        // Draw the player at the correct position (middle of the window)
        DrawFillCircleF(renderer, rc::windowCenter.x, rc::windowCenter.y, PLAYER_RADIUS);
        
        SDL_SetRenderDrawColor(renderer, tmp_c.r, tmp_c.g, tmp_c.b, tmp_c.a);
    }
    else {
        SDL_Rect dest = {static_cast<int>(rc::windowCenter.x + (this->position.x - rc::localPlayerPos.x) - PLAYER_RADIUS), 
            static_cast<int>(rc::windowCenter.y + (this->position.y - rc::localPlayerPos.y) - PLAYER_RADIUS), 
            2 * PLAYER_RADIUS, 2 * PLAYER_RADIUS};
        SDL_RenderCopyEx(renderer, this->texture, nullptr, &dest, static_cast<double>(this->direction - 90.0f), nullptr, SDL_FLIP_NONE);
    }
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

    this->teamNumber = data.teamNumber;

    // interpolate direction
    this->direction = lerpAngle(this->direction, data.direction, 0.3f);

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

void LocalPlayer::forceImportData(const data_packets::PlayerData& data) {

    // update posture
    this->posture = data.posture;

    this->teamNumber = data.teamNumber;

    // update the player position without interpolation
    this->position = data.position;
    this->velocity = data.velocity;

    // stil interpolate direction
    this->direction = lerpAngle(this->direction, data.direction, 0.3f);

    this->lastUpdateTime = SDL_GetTicks();

}

float LocalPlayer::getDirection() const {
    return this->direction;
}

PointF LocalPlayer::getPosition() const {
    return this->position;
}

uint8_t LocalPlayer::getTeamNumber() const {
    return this->teamNumber;
}

void LocalPlayer::captureFlag() {
    this->hasFlag = true;
}

void LocalPlayer::dropFlag() {
    this->hasFlag = false;
}

void LocalPlayer::setNextFriction(float fk) {
    this->nextFriction_k = fk;
}

void LocalPlayer::setNextAcceleration(float ak) {
    this->nextAcceleration_k = ak;
}

void LocalPlayer::setNextSpeedCap(float s) {
    this->nextSpeedCap = s;
}