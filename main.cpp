
// main.cpp

#include "Game.hpp"
#include "Logging/Logger.hpp"
#include "Utility.hpp"


int main(int argc, char *argv[]) {

    if (!SDLUtils::initialize()) {
        return 1;
    }

    try {
        Game::Setup();
    }
    catch(std::runtime_error &e) {
        Logger::error(e.what());
        SDLUtils::cleanup();

        return 1;
    }


    Game::Run();
    Game::Cleanup();


    
    return 0;
}