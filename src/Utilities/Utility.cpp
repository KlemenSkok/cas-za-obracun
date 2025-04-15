
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

        // init SDL_ttf
        if(TTF_Init() != 0) {
            Logger::error((std::string("TTF_Init Error: ") + TTF_GetError()).c_str());
            return false;
        }

        return true;
    }

    void cleanup() {
        SDLNet_Quit();
        TTF_Quit();
        SDL_Quit();
    }
}

namespace Window {

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    void Open() {
        window = SDL_CreateWindow(GAME_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 0xFF); // dark grey
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

namespace Fonts {

    TTF_Font *primaryFont;

    TTF_Font* LoadFont(const char* fileName, int size) {
        TTF_Font* font = TTF_OpenFont(fileName, size);
        if(font == nullptr) {
            Logger::error((std::string("TTF_OpenFont Error: ") + TTF_GetError()).c_str());
            return nullptr;
        }
        return font;
    }

    void Clear() {
        if(primaryFont != nullptr) {
            TTF_CloseFont(primaryFont);
            primaryFont = nullptr;
        }
    }
    
    // Load a digit texture (0–9)
    SDL_Texture* createDigitTexture(SDL_Renderer* renderer, TTF_Font* font, int digit, SDL_Color color) {
        if (digit < 0 || digit > 9) {
            std::cerr << "Invalid digit: " << digit << std::endl;
            return nullptr;
        }
    
        char digitChar = '0' + digit;
        return createCharTexture(renderer, font, digitChar, color);
    }

    // Generic character texture creator
    SDL_Texture* createCharTexture(SDL_Renderer* renderer, TTF_Font* font, char character, SDL_Color color) {
        std::string text(1, character);  // single character string

        SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
        if (!surface) {
            std::cerr << "TTF_RenderText_Blended Error: " << TTF_GetError() << std::endl;
            return nullptr;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        if (!texture) {
            std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        }

        return texture;
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

float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float lerpAngle(float a, float b, float t) {
    float diff = b - a;
    if(diff > 180) {
        b -= 360;
    } else if(diff < -180) {
        b += 360;
    }
    return lerp(a, b, t);
}