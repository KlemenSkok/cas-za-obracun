
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


extern std::queue<std::unique_ptr<UDPmessage>> sendQueue;
extern std::mutex sendq_mutex;


// -------------------------------------------------//
//                                                  //
//                  SOCKET SPEAKER                  //
//                                                  //
// -------------------------------------------------//

// Sending UDP packets
// runs on a separate thread and sends pending UDP packets
class SocketSpeaker {

    // pointer to the speaker thread
    static std::unique_ptr<std::thread> worker;
    static UDPsocket socket;

public:
    static bool _running;
    static void Start();
    static void Speak(UDPsocket) noexcept;
    static void Stop() noexcept;
    static UDPsocket getSocket() noexcept;
};