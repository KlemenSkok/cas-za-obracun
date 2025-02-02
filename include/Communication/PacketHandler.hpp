
// PacketHandler.hpp

#pragma once

#include "Game.hpp"
#include "Containers.hpp"

class PacketHandler {

public:
    static uint32_t lastRecvPacketID,
                    lastSentPacketID;

    static void processPacket(PacketData&);
    static void processPlayerUpdates(PacketData&);
    
    static void sendPlayerUpdate();

};