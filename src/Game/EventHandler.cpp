
// EventHandler.cpp

#include "Game/EventHandler.hpp"
#include "Game.hpp"


bool EventHandler::LockKeyboard = false;
KeyStates EventHandler::keyStates = {0, 0, 0, 0};

void EventHandler::HandleEvents() {
    // handle all events in the queue
    // apply the changes to the local game state
    // send the changes to the server

    std::vector<PacketType> pendingPackets;

    SDL_Event e;
    while(SDL_PollEvent(&e) != 0) {
        if(e.type == SDL_QUIT) {
            // quit the game
            Game::_running = false;
            if(Game::server_info.connection_state == ConnectionState::CONNECTED)
                Game::server_info.connection_state = ConnectionState::DISCONNECTING;
            // dont wait for the server response if the connection hadn't been established
            else Game::server_info.connection_state = ConnectionState::DISCONNECTED;
        }
        else if(e.type == SDL_KEYDOWN && !EventHandler::LockKeyboard) {
            // handle key presses
            switch(e.key.keysym.sym) {
                case SDLK_w:
                    // move the player up
                    if(!keyStates.w) 
                        pendingPackets.push_back(PacketType::PLAYER_UPDATES);
                    keyStates.w = 1;
                    Game::player->acceleration.y = -PLAYER_ACCELERATION;
                    break;
                case SDLK_s:
                    // move the player down
                    if(!keyStates.s) 
                        pendingPackets.push_back(PacketType::PLAYER_UPDATES);
                    keyStates.s = 1;
                    Game::player->acceleration.y = PLAYER_ACCELERATION;
                    break;
                case SDLK_a:
                    // move the player left
                    if(!keyStates.a) 
                        pendingPackets.push_back(PacketType::PLAYER_UPDATES);
                    keyStates.a = 1;
                    Game::player->acceleration.x = -PLAYER_ACCELERATION;
                    break;
                case SDLK_d:
                    // move the player right
                    if(!keyStates.d) 
                        pendingPackets.push_back(PacketType::PLAYER_UPDATES);
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
                    if(keyStates.w) 
                        pendingPackets.push_back(PacketType::PLAYER_UPDATES);
                    keyStates.w = 0;
                    Game::player->acceleration.y = (keyStates.s) ? PLAYER_ACCELERATION : 0.0f;
                    break;
                case SDLK_s:
                    // move the player down
                    if(keyStates.s) 
                        pendingPackets.push_back(PacketType::PLAYER_UPDATES);
                    keyStates.s = 0;
                    Game::player->acceleration.y = (keyStates.w) ? -PLAYER_ACCELERATION : 0.0f;
                    break;
                case SDLK_a:
                    // move the player left
                    if(keyStates.a) 
                        pendingPackets.push_back(PacketType::PLAYER_UPDATES);
                    keyStates.a = 0;
                    Game::player->acceleration.x = (keyStates.d) ? PLAYER_ACCELERATION : 0.0f;
                    break;
                case SDLK_d:
                    // move the player right
                    if(keyStates.d) 
                        pendingPackets.push_back(PacketType::PLAYER_UPDATES);
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

    // send the pending packets
    for(auto& p : pendingPackets) {
        switch(p) {
            case PacketType::PLAYER_UPDATES:
                PacketHandler::sendPlayerUpdate();
                break;
            default:
                // do nothing
                break;
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
