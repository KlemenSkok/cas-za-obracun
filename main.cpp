
// main.cpp

#include "Game.hpp"
#include "Logging/Logger.hpp"
#include "Utilities/Utility.hpp"

#include <iostream>


#define SERVER_IP "192.168.64.112"
//#define SERVER_IP "192.168.64.69"
#define SERVER_PORT 42069


int main(int argc, char *argv[]) {

    if (!SDLUtils::initialize()) {
        return 1;
    }

    try {
        Game::Setup();
        Game::setServerIP(SERVER_IP, SERVER_PORT);
    }
    catch(std::runtime_error &e) {
        Logger::error(e.what());
        SDLUtils::cleanup();
        Game::Cleanup();

        return 1;
    }

    Game::Run();
    
    Game::Cleanup();
    SDLUtils::cleanup();


    return 0;
}