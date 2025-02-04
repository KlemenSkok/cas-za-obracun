
// LocalPlayer.cpp

#include "Game/LocalPlayer.hpp"
#include "Utilities/Utility.hpp"



void LocalPlayer::update(float deltaTime) {

    // update the player position
    this->velocity.x += this->acceleration.x * deltaTime;
    this->velocity.y += this->acceleration.y * deltaTime;

    this->position.x += this->velocity.x * deltaTime;
    this->position.y += this->velocity.y * deltaTime;

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

void LocalPlayer::render(SDL_Renderer* renderer) {
    // draw the player
    SDL_Color tmp_c;
    SDL_GetRenderDrawColor(renderer, &tmp_c.r, &tmp_c.g, &tmp_c.b, &tmp_c.a);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 255);
    DrawFillCircleF(renderer, position.x, position.y, this->radius);
    SDL_SetRenderDrawColor(renderer, tmp_c.r, tmp_c.g, tmp_c.b, tmp_c.a);
}

data_packets::PlayerKeyStates LocalPlayer::dumpKeyStates() {
    data_packets::PlayerKeyStates pks;

    // key states
    pks.keyStates = encodeKeyStates(EventHandler::keyStates);
    
    return pks;
}

void LocalPlayer::importData(const data_packets::PlayerData& data) {
    if(data.keyStates != encodeKeyStates(EventHandler::keyStates)) {
        // the recent player update was probably not received by the server
        // resend the update
        PacketHandler::sendPlayerUpdate();
    }

    /* this->position.x = data.position.x;
    this->position.y = data.position.y;
    this->velocity.x = data.velocity.x;
    this->velocity.y = data.velocity.y;
    this->direction = data.direction; */

}