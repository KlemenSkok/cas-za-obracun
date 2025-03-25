
// main.cpp

#include "Game.hpp"
#include "Logging/Logger.hpp"
#include "Utilities/Utility.hpp"

#include <iostream>




int main(int argc, char *argv[]) {

    if (!SDLUtils::initialize()) {
        return 1;
    }

    try {
        Game::Setup();
        Game::setServerIP(argc, argv);
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