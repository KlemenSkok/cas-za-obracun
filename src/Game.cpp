
// Game.cpp

#include "Game.hpp"
#include "Utilities/Utility.hpp"
#include "Utilities/Constants.hpp"
#include "Communication/SocketListener.hpp"
#include "Communication/SocketSpeaker.hpp"
#include "Logging/Logger.hpp"
#include "Containers.hpp"

#include <iostream>
#include <stdexcept>



// static members
IPaddress Game::server_addr;
int Game::server_channel = 0;

void Game::Setup() {
    try {
        // pass 0 for dynamic port assignment
        SocketListener::Start();
    }
    catch (std::runtime_error &e) {
        throw std::runtime_error(std::string("Failed to start listening thread: ") + e.what());
    }

    try {
        // pass 0 for dynamic port assignment
        SocketSpeaker::Start();
    }
    catch (std::runtime_error &e) {
        throw std::runtime_error(std::string("Failed to start speaking thread: ") + e.what());
    }
}

void Game::Cleanup() {
    Logger::info("Cleaning up...");

    SocketListener::Stop();
    SocketSpeaker::Stop();

    SDLNet_UDP_Close(SocketSpeaker::getSocket());
    SDLNet_UDP_Close(SocketListener::getSocket());
}

void Game::setServerIP(const char* ip, uint16_t port) {
    if(SDLNet_ResolveHost(&Game::server_addr, ip, port) == -1) {
        throw std::runtime_error("Failed to resolve host.");
    }
    Logger::info(("Server IP resolved: " + formatIP(Game::server_addr.host) + ":" + std::to_string(Game::server_addr.port)).c_str());

    SDLNet_UDP_Bind(SocketSpeaker::getSocket(), server_channel, &server_addr);

}

void Game::Run() {
    Window::Open();
    bool quit = false;

    PacketData m(true);
    m.flags() |= (1 << FLAG_SYN);
    addMessageToQueue(m, server_channel);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

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

    m.reset();
    m.flags() |= (1 << FLAG_FIN);
    addMessageToQueue(m, server_channel);
}

void Game::processNewPackets() {

    // omejimo stevilo paketov, ki jih obdelamo naenkrat,
    // da ne blokiramo glavne niti
    const uint8_t max_packets = 100;
    uint8_t num_packets = 0;

    // check for recieved messages
    PacketData recv_msg;
    while(getMessageFromQueue(recv_msg) && num_packets < max_packets) {
        
        // handle the message
        // --------------------------------------

        std::cout << "[" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() << "] " << recv_msg.getRawData().get() << '\n';

        // --------------------------------------

        num_packets++;
    }

}