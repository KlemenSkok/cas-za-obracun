
// SocketSpeaker.hpp

#pragma once

#include <cstdint>
#include <memory>
#include <thread>
#include <queue>
#include <mutex>
#include <SDL2/SDL_net.h>

// za UDPmessage
#include "Containers.hpp"


// -------------------------------------------------//
//                                                  //
//                  RECIEVED QUEUE                  //
//                                                  //
// -------------------------------------------------//

// Queue with recieved messages and its mutex
extern std::queue<std::unique_ptr<UDPmessage>> recievedQueue;
extern std::mutex recvq_mutex;

// function that retrieves a message from the recieved queue
bool getMessageFromQueue(PacketData& data);


// -------------------------------------------------//
//                                                  //
//                  SOCKET LISTENER                 //
//                                                  //
// -------------------------------------------------//

// Recieving UDP packets
// runs on a separate thread and listens for incoming UDP packets
class SocketListener {

    // pointer to the listener thread
    static std::unique_ptr<std::thread> worker;

public:
    static bool _running;
    static void Start();
    static void Listen(UDPsocket) noexcept;
    static void Stop() noexcept;
};