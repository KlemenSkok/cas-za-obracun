
// RemotePlayer.hpp

#pragma once

#include "Game/GameObject.hpp"
#include "Game/EventHandler.hpp"
#include "Game.hpp"

#include <queue>

class RemotePlayer : public GameObject {

    PointF velocity;
    PointF acceleration;

    float direction;
    int8_t posture;
    KeyStates keyStates;

    friend class Game;
    friend class EventHandler;

    // interpolation variables
    Uint32 lastUpdateTime; // local timestamp
    data_packets::PlayerData lastData; // last processed data
    std::queue<data_packets::PlayerData> dataBuffer; // data to be processed

public:
    RemotePlayer(float x, float y, float direction) : GameObject(x, y), direction(direction), lastUpdateTime(SDL_GetTicks()) {}
    RemotePlayer(const data_packets::PlayerData& data) : GameObject(data.position.x, data.position.y), direction(data.direction), lastUpdateTime(SDL_GetTicks()) {
        decodeKeyStates(data.keyStates, this->keyStates);
        this->lastData = data;
    }

    void update(float deltaTime) override;
    void render(SDL_Renderer*) override;

    void importData(const data_packets::PlayerData& data);
};