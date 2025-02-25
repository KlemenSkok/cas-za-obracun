
// EventHandler.cpp

#include "Game/EventHandler.hpp"
#include "Game.hpp"
#include "Rendering/RenderingContext.hpp"

#include <cmath>


bool EventHandler::LockKeyboard = false;
KeyStates EventHandler::keyStates = {0, 0, 0, 0, 0, 0};

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
                    break;
                case SDLK_s:
                    // move the player down
                    if(!keyStates.s) 
                        pendingPackets.push_back(PacketType::PLAYER_UPDATES);
                    keyStates.s = 1;
                    break;
                case SDLK_a:
                    // move the player left
                    if(!keyStates.a) 
                        pendingPackets.push_back(PacketType::PLAYER_UPDATES);
                    keyStates.a = 1;
                    break;
                case SDLK_d:
                    // move the player right
                    if(!keyStates.d) 
                        pendingPackets.push_back(PacketType::PLAYER_UPDATES);
                        keyStates.d = 1;
                    break;
                    case SDLK_e:
                    // interact with the flag
                    keyStates.use_button = 1;
                    pendingPackets.push_back(PacketType::PLAYER_UPDATES);
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
                    break;
                case SDLK_s:
                    // move the player down
                    if(keyStates.s) 
                        pendingPackets.push_back(PacketType::PLAYER_UPDATES);
                    keyStates.s = 0;
                    break;
                case SDLK_a:
                    // move the player left
                    if(keyStates.a) 
                        pendingPackets.push_back(PacketType::PLAYER_UPDATES);
                    keyStates.a = 0;
                    break;
                case SDLK_d:
                    // move the player right
                    if(keyStates.d) 
                        pendingPackets.push_back(PacketType::PLAYER_UPDATES);
                        keyStates.d = 0;
                        break;
                        case SDLK_e:
                        // interact with the flag
                        keyStates.use_button = 0;
                        pendingPackets.push_back(PacketType::PLAYER_UPDATES);
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
            int posx, posy;
            SDL_GetMouseState(&posx, &posy);
            EventHandler::keyStates.left_click = 1;
            pendingPackets.push_back(PacketType::PLAYER_UPDATES);

            // calculate the angle
            // tole bo treba prilagodit glede na postavitev kamere
            // to dela za primer, ko igralcec ni v srediscu (early dev)
/*             float dy = Game::player->position.y - posy;
            float dx = Game::player->position.x - posx;
            float angle = SDL_atan2f(dy, dx);
            constexpr float multiplier = 180 / M_PI;
            Game::player->direction = angle * multiplier; */

            // to dela, ko je igralec v srediscu
            //* ok
            float dx = rc::windowCenter.x - posx;
            float dy = rc::windowCenter.y - posy;
            constexpr float multiplier = 180 / M_PI;
            Game::player->direction = SDL_atan2f(dy, dx) * multiplier; // angle in degrees


        }
        else if(e.type == SDL_MOUSEBUTTONUP) {
            // handle mouse button releases
            // stop throwing the projectile
            // start cooldown
            // todo
            int posx, posy;
            SDL_GetMouseState(&posx, &posy);
            EventHandler::keyStates.left_click = 0;
            pendingPackets.push_back(PacketType::PLAYER_UPDATES);
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
