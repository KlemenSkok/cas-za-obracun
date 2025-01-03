
// Game.cpp

#include "Game.hpp"
#include "Utility.hpp"
#include "Communication/SocketListener.hpp"
#include "Communication/SocketSpeaker.hpp"
#include "Logging/Logger.hpp"
#include "Containers.hpp"

#include <iostream>
#include <stdexcept>


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

void Game::Run() {
    bool quit = false;
    int cnt = 0;

    while(!quit) {
        
        std::this_thread::sleep_for(std::chrono::microseconds(1));

        processNewPackets();

        // send a message to the server
        PacketData send_msg((Uint8*)"Hello from client!", 20);
        addMessageToQueue(send_msg, server_channel);
        
        cnt++;
    }
}

void Game::Cleanup() {
    SocketListener::Stop();
    SocketSpeaker::Stop();

    SDLNet_UDP_Unbind(SocketSpeaker::getSocket(), server_channel);
    SDLNet_UDP_Close(SocketSpeaker::getSocket());

}

void Game::setServerIP(const char* ip, uint16_t port) {
    if(SDLNet_ResolveHost(&Game::server_addr, ip, port) == -1) {
        throw std::runtime_error("Failed to resolve host.");
    }
    Logger::info("Server IP resolved.");
    Logger::info((formatIP(Game::server_addr.host) + ":" + std::to_string(Game::server_addr.port) + '\n').c_str());

    SDLNet_UDP_Bind(SocketSpeaker::getSocket(), server_channel, &server_addr);

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