
// Flag.hpp

#include "GameObject.hpp"
#include "Utilities/Constants.hpp"
#include "Communication/PacketTypes.hpp"

#include <queue>


class Flag : public GameObject {

    Point size;

    PointF homePosition;
    uint16_t carrierID;
    uint8_t carrierTeam;
    bool _isHome;
    bool _isCaptured;

    // interpolation variables
    Uint32 lastUpdateTime; // local timestamp
    data_packets::FlagData lastData; // last processed data
    std::queue<data_packets::FlagData> dataBuffer; // data to be processed

public:

    Flag(float x, float y) :
        GameObject(x, y), 
        homePosition(GAME_FLAG_HOME_POS), 
        size(GAME_FLAG_SIZE), 
        carrierID(0), 
        carrierTeam(0),
        _isHome(true), 
        _isCaptured(false),
        lastUpdateTime(SDL_GetTicks()) 
    {
        this->lastData.position = this->homePosition;
    }
    ~Flag() = default;

    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;

    void importData(const data_packets::FlagData& data);
    void forceImportData(const data_packets::FlagData& data);

    void capture(uint16_t playerID);
    void release();
    void reset();

    bool isCaptured() const;
    bool isHome() const;

    uint16_t getCarrierID() const;
    uint8_t getCarrierTeam() const;
    void setCarrierTeam(uint8_t team);

    void setPosition(const PointF&);
    void updatePosition(const PointF&);
    Point getSize() const;

};