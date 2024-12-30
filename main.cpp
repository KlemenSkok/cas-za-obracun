
// main.cpp

#include <iostream>
#include <cstring>
#include <windows.h>
#include <conio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>

#include "include/Logger.hpp"


#define MAX_BUFFER_SIZE 512

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;



void setNonBlockingMode(bool enable) {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);

    if (enable) {
        mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT); // Disable line input and echo
    } else {
        mode |= (ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT); // Enable line input and echo
    }

    SetConsoleMode(hStdin, mode);
}


int main(int argc, char* argv[]) {

    setNonBlockingMode(true); // enable non-blocking input for terminal

    // Initialize SDL and SDL_Net
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        std::cerr << "SDL_Init: " << SDL_GetError() << std::endl;
        return -1;
    }
    if (SDLNet_Init() == -1) {
        std::cerr << "SDLNet_Init: " << SDLNet_GetError() << std::endl;
        return -1;
    }

    // Resolve the host (server IP address) and port
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, "192.168.64.110", 12345) == -1) {
        std::cerr << "SDLNet_ResolveHost: " << SDLNet_GetError() << std::endl;
        SDLNet_Quit();
        SDL_Quit();
        return -1;
    }

    // Open a TCP connection to the server
    TCPsocket client = SDLNet_TCP_Open(&ip);
    if (!client) {
        std::cerr << "SDLNet_TCP_Open: " << SDLNet_GetError() << std::endl;
        SDLNet_Quit();
        SDL_Quit();
        return -1;
    }

    std::cout << "Connected to the server!" << std::endl;

    // Message to send to the server
/*     const char *message = "Hello, Server!";
    if (SDLNet_TCP_Send(client, message, strlen(message) + 1) < (int)strlen(message) + 1) {
        std::cerr << "SDLNet_TCP_Send: " << SDLNet_GetError() << std::endl;
    } */

    char buffer[MAX_BUFFER_SIZE];
    int i = 0;
    buffer[i] = '\0';
    bool exit = false;

    while(!exit) {        
        if(_kbhit()) { // check if a key was pressed
            char ch = _getch(); // get the pressed key           
            switch (ch) {
                case '\b': // delete a character
                    if (i > 0) {
                        i--;
                        buffer[i] = '\0';
                    }
                    std::cout << "\b \b";
                    break;
                case '\r': // send the message
                    if(i == 0) {
                        break;
                    }
                    if(SDLNet_TCP_Send(client, buffer, strlen(buffer) + 1) < (int)strlen(buffer) + 1) {
                        std::cerr << "SDLNet_TCP_Send: " << SDLNet_GetError() << std::endl;
                    }
                    std::cout << std::endl;
                    if(strcmp(buffer, "exit") == 0) {
                        exit = true;
                    }
                    i = 0;
                    buffer[i] = '\0';
                    break;
                default: // add a character
                    if(i < MAX_BUFFER_SIZE - 2) {
                        buffer[i] = ch;
                        i++;
                        buffer[i] = '\0';
                    }
                    std::cout << buffer[i - 1];
            }
        }

        SDL_Delay(100);
    }

    // Close the TCP connection
    SDLNet_TCP_Close(client);

    // Clean up
    SDLNet_Quit();
    SDL_Quit();

    setNonBlockingMode(false); // restore default console settings

    return 0;
}
