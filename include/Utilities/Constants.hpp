
// Constants.hpp

#pragma once


// global constants

#define GAME_TITLE "Čas za obracun" // očitno šimniki ne delajo problemov
#define INITIAL_WINDOW_WIDTH 800
#define INITIAL_WINDOW_HEIGHT 600

#define TICK_RATE 60 // [Hz]
#define GAME_LOOP_DELAY 1000 / TICK_RATE // [ms]

#define MAX_PACKET_SIZE 512 // [B]
#define SOCKET_LOOP_DELAY 10 // [ms]


// CONNECTION CONSTANTS
enum class ConnectionState: uint8_t {
    DISCONNECTED = 0,
    CONNECTING,
    CONNECTED,
    DISCONNECTING
};

#define CONNECTION_REQUEST_INTERVAL 1000 // [ms]
#define DISCONNECTION_REQUEST_INTERVAL 1000 // [ms]
#define KEEPALIVE_INTERVAL 5000 // [ms]

// PACKET HEADER CONSTANTS

// header flags
#define NUM_FLAGS 8

#define FLAG_ACK 7
#define FLAG_FIN 6
#define FLAG_SYN 5
#define FLAG_KEEPALIVE 4
#define FLAG_DATA 3
#define FLAG_PULL 2
#define FLAG_PUSH 1

// data offsets in packets [B]
#define OFFSET_FLAGS 0
#define OFFSET_SESSION_ID 1
#define OFFSET_CLIENT_ID 2
#define OFFSET_PACKET_ID 4
#define OFFSET_PACKET_TYPE 8
#define OFFSET_DATA 9

// game info constants

enum class GameState: uint8_t {
    WAITING_FOR_PLAYERS = 0,
    ROUND_PAUSED,
    ROUND_RUNNING,
    GAME_FINISHED
};

#define MAX_PLAYERS 4

#define PLAYER_MAX_SPEED 200.0f // [px/s]
#define PLAYER_ACCELERATION 1200.0f // [px/s^2]
#define PLAYER_FRICTION 1500.0f // [px/s^2]