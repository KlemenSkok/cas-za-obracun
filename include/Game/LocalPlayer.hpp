
// LocalPlayer.hpp

#pragma once

#include "Game/GameObject.hpp"
#include "Communication/PacketTypes.hpp"
#include "Game/EventHandler.hpp"
#include "Game.hpp"


class LocalPlayer : public GameObject {
    float direction;
    int8_t posture;

    friend class Game;
    friend class EventHandler;

    // interpolation variables
    Uint32 lastUpdateTime; // local timestamp

public:
    LocalPlayer(float x, float y, float direction) : GameObject(x, y), direction(direction), lastUpdateTime(SDL_GetTicks()) {}

    void update(float deltaTime) override;
    void render(SDL_Renderer*) override;

    data_packets::PlayerKeyStates dumpKeyStates();
    void importData(const data_packets::PlayerData& data);
    float getDirection() const;
    Point getPosition() const;

};