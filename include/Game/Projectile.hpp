
// Projectile.hpp

#pragma once

#include "GameObject.hpp"
#include "Communication/PacketTypes.hpp"

#include <queue>


class Projectile : public GameObject {

    PointF velocity;
    PointF acceleration;
    float direction;

    uint8_t radius = 10;

    // interpolation variables
    Uint32 lastUpdateTime; // local timestamp
    data_packets::ProjectileData lastData; // last processed data
    std::queue<data_packets::ProjectileData> dataBuffer; // data to be processed

public:
    Projectile(float px, float py, float vx, float vy) : 
        GameObject(px, py),
        direction(0.0f)
    {
        velocity.x = vx;
        velocity.y = vy;
        lastUpdateTime = SDL_GetTicks();
    }
    Projectile(const data_packets::ProjectileData& data) : 
        GameObject(data.position.x, data.position.y),
        direction(0.0f)
    {
        velocity.x = data.velocity.x;
        velocity.y = data.velocity.y;
        lastUpdateTime = SDL_GetTicks();
        lastData = data;
    }
    ~Projectile() = default;

    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;

    void importData(const data_packets::ProjectileData& data);
};