
// Game.hpp

#pragma once

#include <memory>
#include <cstdint>
#include <SDL2/SDL.h>
#include <unordered_map>
#include <SDL2/SDL_net.h>

#include "Communication/PacketHandler.hpp"
#include "Rendering/RenderWindow.hpp"
#include "Utilities/Constants.hpp"
#include "Game/EventHandler.hpp"
#include "Game/RemotePlayer.hpp"
#include "Game/Map/MapData.hpp"
#include "Game/LocalPlayer.hpp"
#include "Game/Projectile.hpp"
#include "Containers.hpp"
#include "Game/Flag.hpp"

// forward declaration because of circular dependency
class LocalPlayer;
class RemotePlayer;
class EventHandler;
class PacketHandler;
class RenderWindow;

class Game {
    struct ConnectionInfo {
        IPaddress addr;
        int channel;
        ConnectionState connection_state;
        ConnectionInfo() : addr(), channel(0), connection_state(ConnectionState::DISCONNECTED) {}
    };

    // game loop control
    static bool _running;

    // connection info
    static ConnectionInfo server_info;

    // session and client id
    static uint8_t session_id;
    static uint16_t client_id;

    // game objects
    static std::shared_ptr<Flag> flag;
    static std::shared_ptr<LocalPlayer> player;
    static std::unordered_map<uint16_t, std::shared_ptr<RemotePlayer>> remote_players;
    static std::unordered_map<uint16_t, std::shared_ptr<Projectile>> projectiles;

public:
    static void Setup();
    static void Run();
    static void Update(int deltaTime);
    static void Render();
    static void Cleanup();
    static void setServerIP(const char* ip, uint16_t port);

    static void processNewPackets();
    static void manageConnection();
    static void sendPacket(PacketData& d);

    friend class EventHandler; // handle all events
    friend class PacketHandler; // handle all packets    
    friend class RenderWindow; // render all game objects
};