
// Game.hpp

#pragma once

#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>
#include <cstdint>

#include "Containers.hpp"
#include "Utilities/Constants.hpp"


class Game {
    static IPaddress server_addr;
    static int server_channel;
    static ConnectionState connection_state;

    static uint8_t session_id;
    static uint16_t client_id;



public:
    static void Setup();
    static void Run();
    static void Cleanup();
    static void setServerIP(const char* ip, uint16_t port);

    static void processNewPackets();
    static void manageConnection();
};