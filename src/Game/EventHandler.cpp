
// EventHandler.cpp

#include "Game/EventHandler.hpp"
#include "Game.hpp"
#include "Rendering/RenderingContext.hpp"
#include "UI/UIManager.hpp"

#include <cmath>


bool EventHandler::keyboardLocked = true;
KeyStates EventHandler::keyStates = {0, 0, 0, 0, 0, 0};


void EventHandler::LockKeyboard() {
    EventHandler::keyboardLocked = true;
    keyStates = KeyStates();
}

void EventHandler::UnlockKeyboard() {
    EventHandler::keyboardLocked = false;
    keyStates = KeyStates();
}


void EventHandler::HandleEvents() {
    // handle all events in the queue
    // apply the changes to the local game state
    // send the changes to the server

    bool sendUpdates = false;

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
        else if(e.type == SDL_KEYDOWN && !EventHandler::keyboardLocked) {
            // handle key presses
            switch(e.key.keysym.sym) {
                case SDLK_w:
                    // move the player up
                    if(!keyStates.w) 
                        sendUpdates = true;
                    keyStates.w = 1;
                    break;
                case SDLK_s:
                    // move the player down
                    if(!keyStates.s) 
                        sendUpdates = true;
                    keyStates.s = 1;
                    break;
                case SDLK_a:
                    // move the player left
                    if(!keyStates.a) 
                        sendUpdates = true;
                    keyStates.a = 1;
                    break;
                case SDLK_d:
                    // move the player right
                    if(!keyStates.d) 
                        sendUpdates = true;
                    keyStates.d = 1;
                    break;
                    case SDLK_e:
                    // interact with the flag
                    if(!keyStates.use_button) 
                        sendUpdates = true;
                    keyStates.use_button = 1;
                    break;
                default:
                    // do nothing
                    break;
            }
        }
        else if(e.type == SDL_KEYUP && !EventHandler::keyboardLocked) {
            // handle key releases
            switch(e.key.keysym.sym) {
                case SDLK_w:
                    // move the player up
                    if(keyStates.w) 
                        sendUpdates = true;
                    keyStates.w = 0;
                    break;
                case SDLK_s:
                    // move the player down
                    if(keyStates.s) 
                        sendUpdates = true;
                    keyStates.s = 0;
                    break;
                case SDLK_a:
                    // move the player left
                    if(keyStates.a) 
                        sendUpdates = true;
                    keyStates.a = 0;
                    break;
                case SDLK_d:
                    // move the player right
                    if(keyStates.d) 
                        sendUpdates = true;
                    keyStates.d = 0;
                    break;
                case SDLK_e:
                    // interact with the flag
                    if(keyStates.use_button) 
                        sendUpdates = true;
                    keyStates.use_button = 0;
                    break;
                default:
                    // do nothing
                    break;
            }
        }
        else if(e.type == SDL_MOUSEMOTION) {
            // handle mouse movement
            // rotate the player

            int posx, posy;
            SDL_GetMouseState(&posx, &posy);
            
            // to dela, ko je igralec v srediscu
            //* ok
            float dx = rc::windowCenter.x - posx;
            float dy = rc::windowCenter.y - posy;
            constexpr float multiplier = 180 / M_PI;
            Game::player->direction = SDL_atan2f(dy, dx) * multiplier; // angle in degrees
            
            sendUpdates = true;
        }
        else if(e.type == SDL_MOUSEBUTTONDOWN) {
            // handle mouse button presses
            // throw a projectile

            int posx, posy;
            SDL_GetMouseState(&posx, &posy);
            EventHandler::keyStates.left_click = 1;
            sendUpdates = true;

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
            sendUpdates = true;
        }
        // ignore the keyboard lock for these events (GUI interactions)
        if(gui::expectsInput && e.type == SDL_KEYDOWN) {
            switch(e.key.keysym.sym) {
                case SDLK_1:
                    // select the first option
                    gui::processInput(1);
                    break;
            }
        }
    }

    // send the pending packets
    if(sendUpdates) {
        PacketHandler::sendPlayerUpdate();
    }

}
