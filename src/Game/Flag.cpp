
// Flag.cpp

#include "Game/Flag.hpp"
#include "Rendering/RenderingContext.hpp"
#include "Utilities/Utility.hpp"


void Flag::update(float deltaTime) {
    // interpolate the flag's position
    PointF newPosition = this->position;

    // INTERPOLATION HERE

    if(!this->dataBuffer.empty()) {
        auto data = this->dataBuffer.front();
        
        float estimatedServerTime = data.timestamp + (SDL_GetTicks() - data.recv_ts);

        // Apply new packet if it's time
        if(estimatedServerTime >= data.timestamp) {
            this->dataBuffer.pop();

            // Smooth correction instead of snapping
            const float correctionFactor = 0.3f;
            newPosition.x = lerp(newPosition.x, data.position.x, correctionFactor);
            newPosition.y = lerp(newPosition.y, data.position.y, correctionFactor);

            this->lastData = data;
            this->lastUpdateTime = SDL_GetTicks();
        }
        else if(newPosition.x != this->lastData.position.x || newPosition.y != this->lastData.position.y) {
            // interpolate
            const float correctionFactor = 0.3f;
            newPosition.x = lerp(newPosition.x, data.position.x, correctionFactor);
            newPosition.y = lerp(newPosition.y, data.position.y, correctionFactor);
        }

    }
    else if(newPosition.x != this->lastData.position.x || newPosition.y != this->lastData.position.y) {
        // interpolate
        const float correctionFactor = 0.3f;
        newPosition.x = lerp(newPosition.x, this->lastData.position.x, correctionFactor);
        newPosition.y = lerp(newPosition.y, this->lastData.position.y, correctionFactor);
    }

    // mislm da tle ni treba collisiona cekirat
    this->position = newPosition;

}

void Flag::render(SDL_Renderer* renderer) {
    if(this->texture == nullptr) {
        // draw the flag
        SDL_Color tmp_c;
        SDL_GetRenderDrawColor(renderer, &tmp_c.r, &tmp_c.g, &tmp_c.b, &tmp_c.a);
        SDL_SetRenderDrawColor(renderer, 0x00, 0x50, 0x00, 255);
        
        // Draw the player at the correct position
        SDL_Rect rect;
        rect.x = rc::windowCenter.x + (this->position.x - rc::localPlayerPos.x);
        rect.y = rc::windowCenter.y + (this->position.y - rc::localPlayerPos.y);
        rect.w = this->size.x;
        rect.h = this->size.y;
        
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, tmp_c.r, tmp_c.g, tmp_c.b, tmp_c.a);
    }
    else {
        SDL_Rect dest = {static_cast<int>(rc::windowCenter.x + (this->position.x - rc::localPlayerPos.x)), 
                         static_cast<int>(rc::windowCenter.y + (this->position.y - rc::localPlayerPos.y)), 
                         this->size.x, this->size.y};
        SDL_RenderCopy(renderer, this->texture, nullptr, &dest);
    }

}

void Flag::capture(uint16_t playerID) {
    this->_isCaptured = true;
    this->_isHome = false;
    this->carrierID = playerID;
}

void Flag::release() {
    this->_isCaptured = false;
    this->_isHome = false;
    this->carrierID = 0;
    this->carrierTeam = 0;
}

void Flag::reset() {
    this->_isCaptured = false;
    this->_isHome = true;
    this->carrierID = 0;
    this->position = this->homePosition;
}

bool Flag::isCaptured() const {
    return this->_isCaptured;
}

bool Flag::isHome() const {
    return this->_isHome;
}

uint16_t Flag::getCarrierID() const {
    return this->carrierID;
}

uint8_t Flag::getCarrierTeam() const {
    return this->carrierTeam;
}

void Flag::setCarrierTeam(uint8_t team) {
    this->carrierTeam = team;
}

void Flag::setPosition(const PointF& p) {
    this->position.x = p.x;
    this->position.y = p.y;
}

// set position relative to carrier's position (flag center == player center)
void Flag::updatePosition(const PointF& p) {
    this->position.x = p.x - this->size.x / 2.0f;
    this->position.y = p.y - this->size.y / 2.0f;
}

Point Flag::getSize() const {
    return this->size;
}

void Flag::importData(const data_packets::FlagData& data) {
    this->dataBuffer.push(data);

    // decode data
    if(this->carrierID != data.carrierID) {
        this->capture(data.carrierID);
    }

}

void Flag::forceImportData(const data_packets::FlagData& data) {

    // clear buffer
    while(!this->dataBuffer.empty()) {
        this->dataBuffer.pop();
    }

    // apply new data
    this->lastData = data;
    this->lastUpdateTime = SDL_GetTicks();
    this->position = data.position;
    
    // decode data
    if(this->carrierID != data.carrierID) {
        this->capture(data.carrierID);
    }


}