
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



std::string formatIP(Uint32 ip);
void dumpPacket(UDPpacket *packet);

// Draw a filled circle with center at (x, y) and radius r
void DrawFillCircleF(SDL_Renderer* renderer, float x, float y, int r);

// Linear interpolation function
float lerp(float a, float b, float t);