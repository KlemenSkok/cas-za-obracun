
// Utility.cpp

#include "Utilities/Utility.hpp"
#include "Utilities/Constants.hpp"
#include "Logging/Logger.hpp"
#include <iostream>

namespace SDLUtils {
    bool initialize() {
        // init SDL
        if(SDL_Init(SDL_INIT_EVENTS) != 0) {
            Logger::error((std::string("SDL_Init Error: ") + SDL_GetError()).c_str());
            return false;    
        }

        // init SDL_net
        if(SDLNet_Init() != 0) {
            Logger::error((std::string("SDLNet_Init Error: ") + SDLNet_GetError()).c_str());
            return false;
        }

        return true;
    }

    void cleanup() {
        SDLNet_Quit();
        SDL_Quit();
    }
}

namespace Window {

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    void Open() {
        window = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF); // black
        SDL_RenderClear(renderer);
        SDL_RenderPresent(Window::renderer);
    }

    void Close() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    }

    void Clear() {
        SDL_RenderClear(renderer);
    }

    void Present() {
        SDL_RenderPresent(renderer);
    }

    void Maximize() {
        SDL_MaximizeWindow(window);
    }

    void Fullscreen() {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }

    int Width() {
        int w;
        SDL_GetWindowSize(window, &w, nullptr);
        return w;
    }

    int Height() {
        int h;
        SDL_GetWindowSize(window, nullptr, &h);
        return h;
    }

}

std::string formatIP(Uint32 ip) {
    std::string out;
    for(int i = 0; i < 4; i++) {
        out += std::to_string(((ip & (0xff << i*8)) >> i*8)) + ((i != 3) ? "." : "");
    }
    return out;
}

void dumpPacket(UDPpacket *packet) {
    // dump packet data
    std::cout << "Received packet of size " << packet->len << " bytes.\n";
    std::cout << "Packet content: " << packet->data << "\n";
    std::cout << "Other packet data: \n" << 
        "\tSource address: " << formatIP(packet->address.host) << ":" << packet->address.port << "\n" <<
        "\tChannel: " << packet->channel << "\n" <<
        "\tStatus: " << packet->status << "\n" <<
        "\tMaxLen: " << packet->maxlen << "\n";
    std::cout << "\n";
}

void DrawFillCircleF(SDL_Renderer* renderer, float x, float y, int r) {
    for(int w = 0; w <= r; w++) {
        for(int h = 0; h <= r; h++) {
            int dx = r - w;
            int dy = r - h;
            if((dx * dx + dy * dy) <= (r * r)) {
                SDL_RenderDrawPointF(renderer, x + dx, y + dy);
                SDL_RenderDrawPointF(renderer, x - dx, y + dy);
                SDL_RenderDrawPointF(renderer, x + dx, y - dy);
                SDL_RenderDrawPointF(renderer, x - dx, y - dy);
            }
        }
    }
}