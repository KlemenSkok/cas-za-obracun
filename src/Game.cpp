
// Game.cpp

#include "Game.hpp"
#include "Utility.hpp"
#include "Communication/SocketListener.hpp"
#include "Communication/SocketSpeaker.hpp"
#include "Logging/Logger.hpp"

#include <iostream>
#include <stdexcept>



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

    IPaddress server_ip;
    if(SDLNet_ResolveHost(&server_ip, "127.0.0.1", 42069) == -1) {
        Logger::error((std::string("Failed to resolve host: ") + SDLNet_GetError()).c_str());
        return;
    }
    Logger::info("Server IP resolved.");
    Logger::info((formatIP(server_ip.host) + ":" + std::to_string(server_ip.port) + '\n').c_str());


    bool quit = false;

    while(!quit) {
        
        std::this_thread::sleep_for(std::chrono::seconds(1));

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
        std::unique_ptr<UDPmessage> msg = std::make_unique<UDPmessage>();
        msg->ip = std::make_unique<IPaddress>(server_ip);
        msg->channel = -1;
        msg->len = 19;
        msg->data = std::make_unique<Uint8[]>(msg->len);
        std::memcpy(msg->data.get(), "hello from client!", msg->len);
        
        {
            std::lock_guard<std::mutex> lock(sendq_mutex);
            sendQueue.push(std::move(msg));
        }
        
    }
}

void Game::Cleanup() {

    SocketListener::Stop();
    //SocketSpeaker::Stop();    
}