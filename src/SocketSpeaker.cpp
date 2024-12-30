
// SocketSpeaker.cpp

#include "Communication/SocketSpeaker.hpp"
#include "Utility.hpp"
#include "Logging/Logger.hpp"

#include <cstring>
#include <utility>
#include <chrono>


#define MAX_PACKET_SIZE 512

// -------------------------------------------------//
//                                                  //
//                  SOCKET SPEAKER                  //
//                                                  //
// -------------------------------------------------//

bool SocketSpeaker::_running = false;

std::unique_ptr<std::thread> SocketSpeaker::worker = nullptr;

std::queue<std::unique_ptr<UDPmessage>> sendQueue;
std::mutex sendq_mutex;

// open socket and start thread
void SocketSpeaker::Start() {

    // open the passed port (0 for dynamic port assignment)
    UDPsocket socket = SDLNet_UDP_Open(0);
    if (!socket) {
        throw std::runtime_error(SDLNet_GetError());
    }

    SocketSpeaker::worker = std::make_unique<std::thread>(&SocketSpeaker::Speak, socket);

}

// stop and close thread
void SocketSpeaker::Stop() noexcept {
        SocketSpeaker::_running = false;

    if(SocketSpeaker::worker && SocketSpeaker::worker->joinable()) {
        SocketSpeaker::worker->join();
    }
    Logger::info("Socket speaker closed.");
}


void SocketSpeaker::Speak(UDPsocket socket) noexcept {

    // allocate a packet
    UDPpacket* packet = SDLNet_AllocPacket(MAX_PACKET_SIZE);
    if (!packet) {
        Logger::error((std::string("Failed to allocate packet: ") + SDLNet_GetError()).c_str());
        SocketSpeaker::_running = false;
        return;
    }

    Logger::info("Socket speaker running.");

    // start after successful initialization
    SocketSpeaker::_running = true;
    while(SocketSpeaker::_running) {
        
        // check for new packets
        if (!sendQueue.empty()) {
            // wait for the mutex to unlock
            std::lock_guard<std::mutex> lock(sendq_mutex); // ta mutex blocka, ampak ni treba bufferja

            do {
                auto msg = std::move(sendQueue.front());
                sendQueue.pop();

                // copy the message to the packet
                packet->channel = msg.get()->channel; // server uporablja channele, zato ga je treba poslat zraven
                packet->len = msg.get()->len;
                packet->data = msg.get()->data.get();
                packet->address.host = msg.get()->ip->host; // IP address
                packet->address.port = msg.get()->ip->port; // port

                // send the packet
                if(SDLNet_UDP_Send(socket, packet->channel, packet) == 0) {
                    Logger::error((std::string("SDLNet_UDP_Send error: ") + SDLNet_GetError()).c_str());
                }

            } while(!sendQueue.empty());
        }
    }

    // sleep to reduce CPU usage (1ms)
    std::this_thread::sleep_for(std::chrono::microseconds(10));

    // cleannup
    SDLNet_FreePacket(packet);
    SDLNet_UDP_Close(socket);
}
