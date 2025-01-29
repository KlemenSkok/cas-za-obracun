
// Game.hpp

#pragma once

#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>
#include <cstdint>
#include <memory>

#include "Containers.hpp"
#include "Utilities/Constants.hpp"
#include "Game/LocalPlayer.hpp"
#include "Game/EventHandler.hpp"

// forward declaration, because LocalPlayer.hpp includes Game.hpp
class LocalPlayer;
class EventHandler;

class Game {

    // game loop control
    static bool _running;

    // server connection
    static IPaddress server_addr;
    static int server_channel;
    static ConnectionState connection_state;

    // session and client id
    static uint8_t session_id;
    static uint16_t client_id;

    // the local player
    static std::shared_ptr<LocalPlayer> player;


public:
    static void Setup();
    static void Run();
    static void Cleanup();
    static void setServerIP(const char* ip, uint16_t port);

    static void processNewPackets();
    static void manageConnection();
    friend class EventHandler; // handle all events
};