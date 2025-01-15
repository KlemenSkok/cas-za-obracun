
// Constants.hpp

#pragma once


// global constants

#define GAME_TITLE "Čas za obracun" // očitno šimniki ne delajo problemov
#define INITIAL_WINDOW_WIDTH 800
#define INITIAL_WINDOW_HEIGHT 600




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
#define OFFSET_DATA 4