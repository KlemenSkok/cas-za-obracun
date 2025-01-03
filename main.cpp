
// main.cpp

#include "Game.hpp"
#include "Logging/Logger.hpp"
#include "Utility.hpp"

#include <chrono>
#include <iostream>

#define SERVER_IP "localhost"
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

    auto start = std::chrono::high_resolution_clock::now();
    Game::Run();
    auto end = std::chrono::high_resolution_clock::now();
    
    Game::Cleanup();
    SDLUtils::cleanup();

    std::chrono::duration<double, std::milli> elapsed = end - start;
    std::cout << "Duration: " << elapsed.count() << " milliseconds" << std::endl;


    return 0;
}