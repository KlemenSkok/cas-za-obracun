
// RemotePlayer.hpp

#pragma once

#include "Game/GameObject.hpp"
#include "Game/EventHandler.hpp"
#include "Game.hpp" 


class RemotePlayer : public GameObject {

    float direction;
    int radius = 20;
    KeyStates keyStates;

    friend class Game;
    friend class EventHandler;

public:
    RemotePlayer(float x, float y, float direction) : GameObject(x, y), direction(direction) {}
    RemotePlayer(const data_packets::PlayerData& data) : GameObject(data.position.x, data.position.y), direction(data.direction) {
        decodeKeyStates(data.keyStates, this->keyStates);
    }

    void update(float deltaTime) override;
    void render(SDL_Renderer*) override;

    void importData(const data_packets::PlayerData& data);
};