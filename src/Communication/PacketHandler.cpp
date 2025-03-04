
// PacketHandler.cpp


#include "Communication/PacketHandler.hpp"
#include "Communication/PacketTypes.hpp"
#include "communication/SocketHandler.hpp"
#include "Game/Projectile.hpp"
#include "UI/UIManager.hpp"
#include "Utilities/AssetManager.hpp"

uint32_t PacketHandler::lastRecvPacketID = 0;
uint32_t PacketHandler::lastSentPacketID = 0;
std::vector<std::unique_ptr<UDPmessage>> PacketHandler::localMsgQueue;
Uint32 PacketHandler::lastSentPacketTime = SDL_GetTicks();



void PacketHandler::sendPendingPackets() {
    // clear the message buffer
    addMessagesToQueue(PacketHandler::localMsgQueue);
}

/**
 * @brief Schedule a packet for sending by adding it to a local message queue. Packets are sent at the end of each frame
 * 
 * @param data PacketData with actual data
 * @param ch channel to send to (server)
 */
void PacketHandler::appendToLocalQueue(PacketData& data, int ch) {
    auto msg = std::make_unique<UDPmessage>();
    msg->channel = ch;
    msg->len = data.size();
    msg->data = data.getRawData();
    PacketHandler::localMsgQueue.push_back(std::move(msg));
}

/**
 * @brief Classify the packet based on it's type and process it accordingly.
 * 
 * @param d The packet data to process.
 */
void PacketHandler::processPacket(PacketData& d) {  

    uint32_t packet_id;
    uint8_t packet_type;
    try {
        d.getByOffset(packet_id, sizeof(uint32_t), OFFSET_PACKET_ID);
        d.getByOffset(packet_type, sizeof(uint8_t), OFFSET_PACKET_TYPE);
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

    switch((PacketType)packet_type) {
        case PacketType::PLAYERS_IN_RANGE:
            // process players in range
            processPlayerUpdates(d);
            break;
        case PacketType::PROJECTILES_IN_RANGE:
            // process projectiles in range
            processProjectileUpdates(d);
            break;
        case PacketType::GAME_STATE:
            // process game state
            processGameStateUpdates(d);
            break;
        case PacketType::FLAG_STATE:
            processFlagUpdates(d);
            // process flag state
            break;
        default:
            Logger::warn("Unknown packet type.");
    }

}

/**
 * @brief Process the packet marked as `PLAYERS_IN_RANGE`.
 * 
 * @param d The packet data to process.
 */
void PacketHandler::processPlayerUpdates(PacketData& d) {
    using namespace data_packets;

    size_t offset = OFFSET_DATA;
    std::unordered_map<uint16_t, PlayerData> players;

    bool force = (Game::current_state == GameState::WAITING_NEXT_ROUND);

    // decode players' data and store it in a map
    while(offset < d.size()) {
        PlayerData p;
        p.deserialize(d, offset);
        p.recv_ts = SDL_GetTicks();
        players[p.id] = p;
        offset += PlayerData::size();
    }

    // remove players that are no longer in range
    for(auto it = Game::remote_players.begin(); it != Game::remote_players.end();) {
        if(players.find(it->first) == players.end()) {
            it = Game::remote_players.erase(it);
        } else {
            it++;
        }
    }

    // update the players
    for(auto& p : players) {
        if(p.first != Game::client_id) {
            if(Game::remote_players.find(p.first) == Game::remote_players.end()) {
                // add a new player
                Game::remote_players[p.first] = std::make_shared<RemotePlayer>(p.second);
                if(p.second.teamNumber == 1) {
                    Game::remote_players[p.first]->setTexture(AssetManager::GetTexture(TEXTURE_PLAYER_1));
                }
                else {
                    Game::remote_players[p.first]->setTexture(AssetManager::GetTexture(TEXTURE_PLAYER_2));
                }
            }
            else {
                if(force) {
                    Game::remote_players[p.first]->forceImportData(p.second);
                }
                else {
                    Game::remote_players[p.first]->importData(p.second);
                }
            }
        }
    }

    if(force) {
        Game::player->forceImportData(players[Game::client_id]);
    }
    else {
        Game::player->importData(players[Game::client_id]);
    }

    if(!Game::player->hasTexture()) {
        if(Game::player->getTeamNumber() == 1)
            Game::player->setTexture(AssetManager::GetTexture(TEXTURE_PLAYER_1));
        else
            Game::player->setTexture(AssetManager::GetTexture(TEXTURE_PLAYER_2));
    }


}

void PacketHandler::sendPlayerUpdate() {
    if(Game::server_info.connection_state != ConnectionState::CONNECTED) {
        return;
    }

    PacketData d(true);
    d.flags() |= (1 << FLAG_DATA);                  // 1B
    d.append(Game::session_id);                     // 1B
    d.append(Game::client_id);                      // 2B
    d.append(++PacketHandler::lastSentPacketID);    // notice: increment the packet id
                                                    // 4B
    d.append((uint8_t)PacketType::PLAYER_UPDATES);  // 1B

    auto data = Game::player->dumpKeyStates();
    data.serialize(d);                              // 1B
    d.append(Game::player->getDirection());         // 4B

    Game::sendPacket(d);

    PacketHandler::lastSentPacketTime = SDL_GetTicks();
    // expected packet size: 14B
}

/**
 * @brief Process the packet marked as `PROJECTILE_UPDATES`.
 * 
 * @param data The packet data to process.
 */
void PacketHandler::processProjectileUpdates(PacketData& d) {
    using namespace data_packets;

    size_t offset = OFFSET_DATA;
    std::unordered_map<uint16_t, ProjectileData> projectiles;

    // decode projectiles' data and store it in a map
    while(offset < d.size()) {
        ProjectileData pr;
        pr.deserialize(d, offset);
        pr.recv_ts = SDL_GetTicks();
        projectiles[pr.id] = pr;
        offset += ProjectileData::size();
    }

    // remove projectiles that are no longer in range
    for(auto it = Game::projectiles.begin(); it != Game::projectiles.end();) {
        if(projectiles.find(it->first) == projectiles.end()) {
            it = Game::projectiles.erase(it);
        } else {
            it++;
        }
    }

    // update the projectiles
    for(auto& pr : projectiles) {
        if(pr.first != Game::client_id) {
            if(Game::projectiles.find(pr.first) == Game::projectiles.end()) {
                Game::projectiles[pr.first] = std::make_shared<Projectile>(pr.second);
            } else {
                Game::projectiles[pr.first]->importData(pr.second);
            }
        }
    }

}

/**
 * @brief Process the packet marked as `FLAG_STATE`.
 * 
 * @param d The packet data to process.
 */
void PacketHandler::processFlagUpdates(PacketData& d) {
    using namespace data_packets;

    bool force = (Game::current_state == GameState::WAITING_NEXT_ROUND);

    size_t offset = OFFSET_DATA;
    FlagData f;

    f.deserialize(d, offset);
    f.recv_ts = SDL_GetTicks();

    // import data
    if(force) {
        Game::flag->forceImportData(f);
    }
    else {
        Game::flag->importData(f);
    }

}

/**
 * @brief Process the packet marked as `GAME_STATE`.
 * 
 * @param d The packet data to process.
 */
void PacketHandler::processGameStateUpdates(PacketData& d) {
    using namespace data_packets;

    size_t offset = OFFSET_DATA;
    GameStateData gs;

    gs.deserialize(d, offset);
    gs.clientTime = SDL_GetTicks();

    // take case of the game state
    if(Game::current_state != gs.gameState) {
        // the game state has changed
        Game::current_state = gs.gameState;
        Game::last_state_change = gs.clientTime;

        switch(Game::current_state) {
            case GameState::WAITING_FOR_PLAYERS:
                // connection established, waiting for players
                EventHandler::UnlockKeyboard();
                //std::cout << "Waiting for players." << std::endl;
                break;
            case GameState::ROUND_ENDING:
                // show the round ending screen
                //std::cout << "Round ending." << std::endl;
                {
                    uint8_t sc1 = 0, sc2 = 0;
                    sc1 = (gs.teamScores & 0xF0) >> 4;
                    sc2 = gs.teamScores & 0x0F;
                    if(Game::scores[0] != sc1) {
                        Game::last_winner = 1;
                    }
                    else if(Game::scores[1] != sc2) {
                        Game::last_winner = 2;
                    }
                    Game::scores[0] = sc1;
                    Game::scores[1] = sc2;
                }
                break;
            case GameState::WAITING_NEXT_ROUND:
                // show the waiting screen
                EventHandler::LockKeyboard();
                //std::cout << "Waiting for the next round." << std::endl;
                break;
            case GameState::ROUND_RUNNING:
                // start the round
                EventHandler::UnlockKeyboard();
                //std::cout << "Round started." << std::endl;
                break;
            case GameState::GAME_FINISHED:
                // show the game finished screen
                EventHandler::LockKeyboard();
                {
                    uint8_t sc1 = 0, sc2 = 0;
                    sc1 = (gs.teamScores & 0xF0) >> 4;
                    sc2 = gs.teamScores & 0x0F;
                    if(Game::scores[0] != sc1) {
                        Game::last_winner = 1;
                    }
                    else if(Game::scores[1] != sc2) {
                        Game::last_winner = 2;
                    }
                    Game::scores[0] = sc1;
                    Game::scores[1] = sc2;

                    // the client can already start disconnecting
                    Game::resetConnection();
                }
                //std::cout << "Game finished." << std::endl;
                break;
            default:
                break;
        }

        gui::updateState(Game::current_state, Game::server_info.connection_state);

    }

}