
// Flag.hpp

#include "GameObject.hpp"
#include "Utilities/Constants.hpp"


class Flag : public GameObject {

    Point size;

    PointF homePosition;
    uint16_t carrierID;
    bool _isHome;
    bool _isCaptured;

public:

    Flag(float x, float y) : GameObject(x, y), homePosition({x, y}), size({50, 20}), carrierID(0), _isHome(true), _isCaptured(false) {}
    ~Flag() = default;

    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;

    void capture(uint16_t playerID);
    void release();
    void reset();

    bool isCaptured() const;
    bool isHome() const;
    uint16_t getCarrierID() const;

};