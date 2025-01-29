
// EventHandler.cpp

#include "Game/EventHandler.hpp"
#include "Game.hpp"


bool EventHandler::LockKeyboard = false;
KeyStates EventHandler::keyStates = {0, 0, 0, 0};

void EventHandler::HandleEvents() {
    // handle all events in the queue
    // apply the changes to the local game state
    // send the changes to the server

    SDL_Event e;
    while(SDL_PollEvent(&e) != 0) {
        if(e.type == SDL_QUIT) {
            // quit the game
            Game::_running = false;
            if(Game::connection_state == ConnectionState::CONNECTED)
                Game::connection_state = ConnectionState::DISCONNECTING;
            // dont wait for the server response if the connection hadn't been established
            else Game::connection_state = ConnectionState::DISCONNECTED;
        }
        else if(e.type == SDL_KEYDOWN && !EventHandler::LockKeyboard) {
            // handle key presses
            switch(e.key.keysym.sym) {
                case SDLK_w:
                    // move the player up
                    keyStates.w = 1;
                    Game::player->acceleration.y = -PLAYER_ACCELERATION;
                    break;
                case SDLK_s:
                    // move the player down
                    keyStates.s = 1;
                    Game::player->acceleration.y = PLAYER_ACCELERATION;
                    break;
                case SDLK_a:
                    // move the player left
                    keyStates.a = 1;
                    Game::player->acceleration.x = -PLAYER_ACCELERATION;
                    break;
                case SDLK_d:
                    // move the player right
                    keyStates.d = 1;
                    Game::player->acceleration.x = PLAYER_ACCELERATION;
                    break;
                default:
                    // do nothing
                    break;
            }
        }
        else if(e.type == SDL_KEYUP && !EventHandler::LockKeyboard) {
            // handle key releases
            switch(e.key.keysym.sym) {
                case SDLK_w:
                    // move the player up
                    keyStates.w = 0;
                    Game::player->acceleration.y = (keyStates.s) ? PLAYER_ACCELERATION : 0.0f;
                    break;
                case SDLK_s:
                    // move the player down
                    keyStates.s = 0;
                    Game::player->acceleration.y = (keyStates.w) ? -PLAYER_ACCELERATION : 0.0f;
                    break;
                case SDLK_a:
                    // move the player left
                    keyStates.a = 0;
                    Game::player->acceleration.x = (keyStates.d) ? PLAYER_ACCELERATION : 0.0f;
                    break;
                case SDLK_d:
                    // move the player right
                    keyStates.d = 0;
                    Game::player->acceleration.x = (keyStates.a) ? -PLAYER_ACCELERATION : 0.0f;
                    break;
                default:
                    // do nothing
                    break;
            }
        }
        else if(e.type == SDL_MOUSEMOTION) {
            // handle mouse movement
            // rotate the player
            // todo
        }
        else if(e.type == SDL_MOUSEBUTTONDOWN) {
            // handle mouse button presses
            // throw a projectile
            // todo
        }
        else if(e.type == SDL_MOUSEBUTTONUP) {
            // handle mouse button releases
            // stop throwing the projectile
            // start cooldown
            // todo
        }
    }

    // normalize the velocity vector for diagonal movement
    /* if(keyStates.w && keyStates.a) {
        Game::player->velocity.x = -PLAYER_SPEED / 1.4142f;
        Game::player->velocity.y = -PLAYER_SPEED / 1.4142f;
    }
    if(keyStates.w && keyStates.d) {
        Game::player->velocity.x = PLAYER_SPEED / 1.4142f;
        Game::player->velocity.y = -PLAYER_SPEED / 1.4142f;
    }
    if(keyStates.s && keyStates.a) {
        Game::player->velocity.x = -PLAYER_SPEED / 1.4142f;
        Game::player->velocity.y = PLAYER_SPEED / 1.4142f;
    }
    if(keyStates.s && keyStates.d) {
        Game::player->velocity.x = PLAYER_SPEED / 1.4142f;
        Game::player->velocity.y = PLAYER_SPEED / 1.4142f;
    } */

}
