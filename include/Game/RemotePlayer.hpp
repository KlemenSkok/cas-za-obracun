
// RemotePlayer.hpp

#pragma once

#include "Communication/PacketTypes.hpp"
#include "Game/EventHandler.hpp"
#include "Game/GameObject.hpp"
#include "Game.hpp"

#include <queue>

class RemotePlayer : public GameObject {

    PointF velocity;
    PointF acceleration;

    float direction;
    int8_t posture;
    KeyStates keyStates;

    uint8_t teamNumber;

    friend class Game;
    friend class EventHandler;

    // interpolation variables
    Uint32 lastUpdateTime; // local timestamp
    data_packets::PlayerData lastData; // last processed data
    std::queue<data_packets::PlayerData> dataBuffer; // data to be processed

    // dynamic movement settings
    float nextFriction_k;
    float nextAcceleration_k;
    float nextSpeedCap;

public:

    RemotePlayer(const data_packets::PlayerData& data) : 
        GameObject(data.position.x, data.position.y), 
        velocity({ 0.0f, 0.0f }),
        acceleration({ 0.0f, 0.0f }),
        direction(data.direction), 
        teamNumber(data.teamNumber), 
        lastUpdateTime(SDL_GetTicks()),
        nextFriction_k(1.0f),
        nextAcceleration_k(1.0f),
        nextSpeedCap(PLAYER_MAX_SPEED)
    {
        decodeKeyStates(data.keyStates, this->keyStates);
        this->lastData = data;
    }

    void update(float deltaTime) override;
    void render(SDL_Renderer*) override;

    void importData(const data_packets::PlayerData& data);
    void forceImportData(const data_packets::PlayerData& data);

    void setNextFriction(float);
    void setNextAcceleration(float);
    void setNextSpeedCap(float);

    uint8_t getTeamNumber() const;

};