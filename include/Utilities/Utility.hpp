
// Utility.hpp

#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <iostream>


// ID generator for all server objects
class IDGenerator {
public:
    static int GetNextID() {
        static int current_id = 1;
        return current_id++;
    }
};


// namespace for dealing with SDL initialization and quitting
namespace SDLUtils {
    // tuki samo deklaracije
    bool initialize();
    void cleanup();
}

// namespace for dealing with SDL window and renderer
namespace Window {
    extern SDL_Window *window;
    extern SDL_Renderer *renderer;

    void Open();
    void Close();
    void Clear();
    void Present();

    void Maximize();
    void Fullscreen();

    int Width();
    int Height();
}

// game info constants

enum class GameState: uint8_t {
    WAITING_FOR_PLAYERS = 0,    // as long as needed
    ROUND_ENDING = 3,           // 3s
    WAITING_NEXT_ROUND = 10,    // 10s
    ROUND_RUNNING,              // as long as needed
    GAME_FINISHED = 30,         // 30s
    NONE
};

enum class RenderState: uint8_t {
    MAIN_MENU = 0,
    CONNECTING,
    GAME,
    GAME_OVER
};

enum class ConnectionState: uint8_t {
    DISCONNECTED = 0,
    CONNECTING,
    CONNECTED,
    DISCONNECTING
};


std::string formatIP(Uint32 ip);
void dumpPacket(UDPpacket *packet);

// Draw a filled circle with center at (x, y) and radius r
void DrawFillCircleF(SDL_Renderer* renderer, float x, float y, int r);

// Linear interpolation function
float lerp(float a, float b, float t);
// Linear interpolation for direction (values within [-180, 180])
float lerpAngle(float a, float b, float t);

