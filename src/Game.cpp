
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
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if(!recievedQueue.empty()) {
            std::lock_guard<std::mutex> lock(recvq_mutex);
            do {
                std::unique_ptr<UDPmessage> msg = std::move(recievedQueue.front());
                //std::cout << formatIP(msg->ip->host) << '\n';
                std::cout << "Prejeto sporocilo: " << msg->data.get() << '\n';
                recievedQueue.pop();

            } while(!recievedQueue.empty());
        }

        // send a message to the server
        PacketData msg((Uint8*)"hello from client!", 19);
        addMessageToQueue(msg);
        
        cnt++;

    }
}

void Game::Cleanup() {

    SocketListener::Stop();
    SocketSpeaker::Stop();    
}

void Game::setServerIP(const char* ip, uint16_t port) {
    if(SDLNet_ResolveHost(&Game::server_addr, ip, port) == -1) {
        throw std::runtime_error("Failed to resolve host.");
    }
    Logger::info("Server IP resolved.");
    Logger::info((formatIP(Game::server_addr.host) + ":" + std::to_string(Game::server_addr.port) + '\n').c_str());

    SDLNet_UDP_Bind(SocketSpeaker::getSocket(), server_channel, &server_addr);

}

void Game::addMessageToQueue(PacketData& data) {
    std::unique_ptr<UDPmessage> msg = std::make_unique<UDPmessage>();
    //msg->ip = std::make_unique<IPaddress>(server_addr);
    msg->channel = server_channel;
    msg->len = data.size();
    msg->data = data.getRawData();

    {
        std::lock_guard<std::mutex> lock(sendq_mutex);
        sendQueue.push(std::move(msg));
    }
}