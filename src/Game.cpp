
// Game.cpp

#include "Game.hpp"
#include "Utility.hpp"
#include "Communication/SocketListener.hpp"
#include "Communication/SocketSpeaker.hpp"

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

    std::string str;
    bool quit = false;

    while(!quit) {
        //std::cin >> str;
        if(str == "exit")
            quit = true;

        std::this_thread::sleep_for(std::chrono::microseconds(1));

        if(!recievedQueue.empty()) {
            std::lock_guard<std::mutex> lock(recvq_mutex);
            do {
                std::unique_ptr<UDPmessage> msg = std::move(recievedQueue.front());
                std::cout << formatIP(msg->ip->host) << '\n';
                std::cout << msg->data.get() << '\n';
                recievedQueue.pop();

            } while(!recievedQueue.empty());
        }
    }
}

void Game::Cleanup() {

    SocketListener::Stop();
    //SocketSpeaker::Stop();    
}