
// RemotePlayer.cpp

#include "Game/RemotePlayer.hpp"
#include "Utilities/Utility.hpp"
#include "Utilities/Constants.hpp"
#include "Communication/PacketTypes.hpp"


void RemotePlayer::update(float deltaTime) {

    // INTERPOLATION HERE

    if(!this->dataBuffer.empty()) {
        
        auto data = this->dataBuffer.front();
        //std::cout << "update time: " << data.timestamp << '\n';
        // poglej ce je cas za menjavo paketov
        if(SDL_GetTicks() - this->lastUpdateTime > this->dataBuffer.front().timestamp - lastData.timestamp) {
            this->dataBuffer.pop();
            this->position.x = data.position.x;
            this->position.y = data.position.y;
            this->velocity.x = data.velocity.x;
            this->velocity.y = data.velocity.y;

            //std::cout << "New pos: " << data.position.x << ", " << data.position.y << '\n';

            this->lastData = data;
            this->lastUpdateTime = SDL_GetTicks();
            //std::cout << "C\n";
            return;
        }
        // else interpolate between states
        else {
            decodeKeyStates(data.keyStates, this->keyStates);
            float alpha = (float(SDL_GetTicks() - this->lastUpdateTime)) / (float(data.timestamp - this->lastData.timestamp));
            this->position.x = lerp(this->lastData.position.x, data.position.x, alpha);
            this->position.y = lerp(this->lastData.position.y, data.position.y, alpha);
            //std::cout << " I\n";
            return;
        }
        std::cout << "   N\n";
    }

    // ------------------
    std::cout << "   E\n";

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

    std::cout << "Recieved pos: " << data.position.x << ", " << data.position.y << '\n';
    
    /* this->position.x = data.position.x;
    this->position.y = data.position.y;
    this->velocity.x = data.velocity.x;
    this->velocity.y = data.velocity.y;
    decodeKeyStates(data.keyStates, this->keyStates);

    // reset acceleration
    this->acceleration.x = 0.0f;
    this->acceleration.y = 0.0f;

    // apply acceleration based on key states
    if(this->keyStates.w) this->acceleration.y -= PLAYER_ACCELERATION;
    if(this->keyStates.s) this->acceleration.y += PLAYER_ACCELERATION;
    if(this->keyStates.a) this->acceleration.x -= PLAYER_ACCELERATION;
    if(this->keyStates.d) this->acceleration.x += PLAYER_ACCELERATION; */

    //std::cout << "Remote player updated: " << this->position.x << ", " << this->position.y << std::endl;

}