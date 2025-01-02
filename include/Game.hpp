
// Game.hpp

#pragma once

#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>
#include <cstdint>

#include "Containers.hpp"


class Game {
    static IPaddress server_addr;
    static int server_channel;

public:
    static void Setup();
    static void Run();
    static void Cleanup();
    static void setServerIP(const char* ip, uint16_t port);

    static void addMessageToQueue(PacketData& data);
    static PacketData getMessageFromQueue();

};