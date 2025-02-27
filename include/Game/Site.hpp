
// Site.hpp

#pragma once

#include "Containers.hpp"
#include "Game/GameObject.hpp"


class Site : public GameObject {

    PointF position;
    Point size;
    uint8_t teamNumber;
    
public:

    Site() : 
        GameObject(0, 0) {}

    void update(float deltaTime) override;
    void render(SDL_Renderer*) override;

    void setPosition(const PointF&);
    void setSize(const Point&);
    Point getSize() const;
    void setTeam(uint8_t);
    uint8_t getTeam() const;

    bool checkFlagCollision(const PointF&, const Point&);

};