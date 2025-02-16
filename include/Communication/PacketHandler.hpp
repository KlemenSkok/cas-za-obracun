
// PacketHandler.hpp

#pragma once

#include "Game.hpp"
#include "Containers.hpp"

#include <vector>

class PacketHandler {

public:
    static uint32_t lastRecvPacketID,
                    lastSentPacketID;
    static Uint32 lastSentPacketTime;
    static std::vector<std::unique_ptr<UDPmessage>> localMsgQueue;

    static void appendToLocalQueue(PacketData&, int);
    static void sendPendingPackets();

    static void processPacket(PacketData&);
    static void processPlayerUpdates(PacketData&);
    static void processProjectileUpdates(PacketData&);
    
    static void sendPlayerUpdate();

};