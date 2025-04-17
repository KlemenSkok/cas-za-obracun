
// UIManager.cpp

#include "UI/UIManager.hpp"



namespace gui {
    
    bool expectsInput = true;
    RenderState currentScreen = RenderState::MAIN_MENU;

    void updateState(GameState game_state, ConnectionState connection_state) {
        // decide what to render based on the game state
    switch(game_state) {
            case GameState::NONE:
                // render the main menu
                currentScreen = RenderState::MAIN_MENU;
                expectsInput = true;
                break;
            case GameState::WAITING_FOR_PLAYERS:
            case GameState::ROUND_ENDING:
            case GameState::ROUND_RUNNING:
            case GameState::WAITING_NEXT_ROUND:
                // render the game state
                currentScreen = RenderState::GAME;
                expectsInput = false;
                break;
            case GameState::GAME_FINISHED:
                // render the game over menu
                currentScreen = RenderState::GAME_OVER;
                expectsInput = true;
                break;
        }

        if(connection_state == ConnectionState::CONNECTING) {
            currentScreen = RenderState::CONNECTING;
        }

    }

    void processInput(int key) {
        // process the input
        switch(currentScreen) {
            case RenderState::MAIN_MENU:
                // process the main menu input
                if(key == 1) {
                    // start connecting to the server
                    currentScreen = RenderState::CONNECTING;
                    expectsInput = false;
                }
                break;
            case RenderState::CONNECTING:
                // do nothing
                break;
            case RenderState::GAME:
                // process the game input
                break;
            case RenderState::GAME_OVER:
                // process the game over menu input
                if(key == 1) {
                    // return to the main menu and exit
                    SDL_Event e;
                    e.type = SDL_QUIT;
                    e.quit.timestamp = SDL_GetTicks();
                    SDL_PushEvent(&e);
                    currentScreen = RenderState::MAIN_MENU;
                }
                break;
        }
    }

}