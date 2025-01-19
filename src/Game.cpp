
// Game.cpp

#include "Game.hpp"
#include "Utilities/Utility.hpp"
#include "Utilities/Constants.hpp"
#include "Communication/SocketHandler.hpp"
#include "Logging/Logger.hpp"
#include "Containers.hpp"

#include <iostream>
#include <stdexcept>



// static members
IPaddress Game::server_addr;
int Game::server_channel = 0;

uint8_t Game::session_id = 0;
uint16_t Game::client_id = 0;

void Game::Setup() {
    try {
        // pass 0 for dynamic port assignment
        SocketHandler::Start();
    }
    catch (std::runtime_error &e) {
        throw std::runtime_error(std::string("Failed to start communication thread: ") + e.what());
    }

}

void Game::Cleanup() {
    Logger::info("Cleaning up...");

    SocketHandler::Stop();
    SDLNet_UDP_Close(SocketHandler::getSocket());
}

void Game::setServerIP(const char* ip, uint16_t port) {
    if(SDLNet_ResolveHost(&Game::server_addr, ip, port) == -1) {
        throw std::runtime_error("Failed to resolve host.");
    }
    Logger::info(("Server IP resolved: " + formatIP(Game::server_addr.host) + ":" + std::to_string(SDLNet_Read16(&Game::server_addr.port))).c_str());

    SDLNet_UDP_Bind(SocketHandler::getSocket(), server_channel, &server_addr);

}

void Game::Run() {
    Window::Open();
    bool quit = false;

    // request connection
    PacketData m(true);
    m.flags() |= (1 << FLAG_SYN);
    //m.append((uint8_t)1);
    //m.append((uint16_t)14);

/*     uint16_t client_id = 0;
    m.getByOffset(client_id, sizeof(client_id), OFFSET_CLIENT_ID);
    std::cout << "Client ID: " << client_id << '\n'; */

    addMessageToQueue(m, server_channel);



    while(!quit) {

        processNewPackets();

        // LOOP IDEA:
        // process server updates
        // process user input
        // send updates to the server
        // show the game state
        // repeat

        

        // send a message to the server
        //PacketData send_msg((Uint8*)("Hello from client! [" + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) + "]").c_str(), 40);
        //addMessageToQueue(send_msg, server_channel);

        SDL_SetRenderDrawColor(Window::renderer, 0x00, 0x00, 0x00, 255);
        SDL_RenderClear(Window::renderer);
        SDL_SetRenderDrawColor(Window::renderer, 0xFF, 0xFF, 0xFF, 255);
        SDL_RenderDrawLine(Window::renderer, 0, 0, Window::Width(), Window::Height());
        SDL_RenderDrawLine(Window::renderer, Window::Width(), 0, 0, Window::Height());
        SDL_RenderPresent(Window::renderer);
        

        SDL_Event e;
        while(SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_QUIT) {
                quit = true;
            }
        }
         
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    Window::Close();

    // send the FIN message
    m.reset();
    m.flags() |= (1 << FLAG_FIN);
    m.append(Game::session_id);
    m.append(Game::client_id);
    addMessageToQueue(m, server_channel);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void Game::processNewPackets() {

    // omejimo stevilo paketov, ki jih obdelamo naenkrat,
    // da ne blokiramo glavne niti
    const uint8_t max_packets = 100;
    uint8_t num_packets = 0;

    // check for recieved messages
    PacketData data;
    while(getMessageFromQueue(data) && num_packets < max_packets) {
        
        // handle the message
        // --------------------------------------

        //std::cout << "[" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << "] " << data.getRawData().get() << '\n';

        for(int i = NUM_FLAGS - 1; i >= 0; i--) {
            if(data.flags() & (1 << i)) {
                switch(i) {
                        case FLAG_ACK:
                            // look for any awaiting confirmations
                            break;
                        case FLAG_SYN:
                            // start the connection (get the client id)
                            uint16_t c_id;
                            uint8_t s_id;
                            try {
                                data.getByOffset(c_id, sizeof(c_id), OFFSET_CLIENT_ID);
                                data.getByOffset(s_id, sizeof(s_id), OFFSET_SESSION_ID);
                            }
                            catch (std::out_of_range &e) {
                                Logger::error("Failed to retrieve client id or session id from the packet.");
                                break;
                            }
                            Logger::info(("Client ID: " + std::to_string(c_id) + ", Session ID: " + std::to_string(s_id)).c_str());
                            Game::client_id = c_id;
                            Game::session_id = s_id;
                            break;
                        case FLAG_FIN:
                            // close the connection
                            break;
                        case FLAG_KEEPALIVE:
                            // just update timestamp of the last keepalive message
                            break;
                        case FLAG_DATA:
                            // process the data and update local game state
                            break;
                        case FLAG_PUSH:
                            // send back data to the server
                            break;
                        default:
                            Logger::info(("A packet was ignored: " + std::string((char*)data.getRawData().get())).c_str());
                            // ignore the packet
                    }
            }
        }
        


        // --------------------------------------

        num_packets++;
    }

}