
// PacketHandler.cpp


#include "Communication/PacketHandler.hpp"
#include "Communication/PacketTypes.hpp"

uint32_t PacketHandler::lastRecvPacketID = 0;
uint32_t PacketHandler::lastSentPacketID = 0;

/**
 * @brief Classify the packet based on it's type and process it accordingly.
 * 
 * @param d The packet data to process.
 */
void PacketHandler::processPacket(PacketData& d) {  

    uint32_t packet_id;
    uint8_t packetType;
    try {
        d.getByOffset(packet_id, sizeof(uint32_t), OFFSET_PACKET_ID);
        d.getByOffset(packetType, sizeof(uint8_t), OFFSET_PACKET_TYPE);
    }
    catch(std::exception& e) {
        Logger::warn(e.what());
        return;
    }

    // ensure duplicate packet handling
    if(packet_id <= PacketHandler::lastRecvPacketID) {
        // this is a duplicate packet, ignore it
        return;
    }

    // now we can process the packet
    PacketHandler::lastRecvPacketID = packet_id;

    switch((PacketType)packetType) {
        case PacketType::PLAYERS_IN_RANGE:
            // process players in range
            processPlayerUpdates(d);
            break;
        case PacketType::GAME_STATE:
            // process game state
            break;
        default:
            Logger::warn("Unknown packet type.");
    }



    // todo
    
}

/**
 * @brief Process the packet marked as `PLAYERS_IN_RANGE`.
 * 
 * @param d The packet data to process.
 */
void PacketHandler::processPlayerUpdates(PacketData& d) {
    using namespace data_packets;

    size_t offset = OFFSET_DATA;
    std::vector<PlayerData> players;

    while(offset < d.size()) {
        PlayerData p;
        p.deserialize(d, offset);
        players.push_back(p);
        offset += PlayerData::size();
    }

/*     std::cout << "dumping data[0]: \n";
    std::cout << "id: " << players[0].id << std::endl;
    std::cout << "position: " << players[0].position.x << ", " << players[0].position.y << std::endl;
    std::cout << "velocity: " << players[0].velocity.x << ", " << players[0].velocity.y << std::endl;
    std::cout << "keyStates: " << (int)players[0].keyStates << std::endl;
    std::cout << "direction: " << players[0].direction << std::endl;
    std::cout << "--------------------------------\n\n"; */


}

void PacketHandler::sendPlayerUpdate() {
    PacketData d(true);
    d.flags() |= (1 << FLAG_DATA);                  // 1B
    d.append(Game::session_id);                     // 1B
    d.append(Game::client_id);                      // 2B
    d.append(++PacketHandler::lastSentPacketID); // notice: increment the packet id
                                                    // 4B
    d.append((uint8_t)PacketType::PLAYER_UPDATES);  // 1B

    auto data = Game::player->dumpMovement();
    data.id = Game::client_id;
    data.serialize(d);                              // 23B

    Game::sendPacket(d);

    // expected packet size: 32B
}