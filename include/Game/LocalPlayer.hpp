
// LocalPlayer.hpp

#pragma once

#include "Game/GameObject.hpp"
#include "Communication/PacketTypes.hpp"
#include "Game/EventHandler.hpp"
#include "Game.hpp"


class LocalPlayer : public GameObject {

    PointF velocity;
    PointF acceleration;

    float direction;
    int8_t posture;

    bool hasFlag;
    uint8_t teamNumber;

    friend class Game;
    friend class EventHandler;

    // interpolation variables
    Uint32 lastUpdateTime; // local timestamp

    // dynamic movement settings
    float nextFriction_k;
    float nextAcceleration_k;
    float nextSpeedCap;

public:
    LocalPlayer(float x, float y, float direction) : 
        GameObject(x, y), 
        direction(direction), 
        lastUpdateTime(SDL_GetTicks()), 
        hasFlag(false), 
        nextAcceleration_k(1.0f),
        nextFriction_k(1.0f),
        nextSpeedCap(PLAYER_MAX_SPEED) {}

    void update(float deltaTime) override;
    void render(SDL_Renderer*) override;

    data_packets::PlayerKeyStates dumpKeyStates();
    void importData(const data_packets::PlayerData& data);
    void forceImportData(const data_packets::PlayerData& data);

    float getDirection() const;
    PointF getPosition() const;
    uint8_t getTeamNumber() const;
    uint8_t getPosture() const;

    void captureFlag();
    void dropFlag();

    void setNextFriction(float);
    void setNextAcceleration(float);
    void setNextSpeedCap(float);

};