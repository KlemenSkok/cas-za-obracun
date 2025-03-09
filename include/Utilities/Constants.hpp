
// Constants.hpp

#pragma once




#define DEBUG


// global constants

#define GAME_TITLE "Čas za obracun" // očitno šimniki ne delajo problemov
#define INITIAL_WINDOW_WIDTH 800
#define INITIAL_WINDOW_HEIGHT 600

#define TICK_RATE 60 // [Hz]
#define GAME_LOOP_DELAY 1000 / TICK_RATE // [ms]

#define SERVER_TICK_RATE 30 // [Hz]
#define SERVER_TICK_DELAY 1000 / SERVER_TICK_RATE // [ms]

#define MAX_PACKET_SIZE 512 // [B]
#define SOCKET_LOOP_DELAY 10 // [ms]


// CONNECTION CONSTANTS

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


//
//  GAMEPLAY CONSTANTS
//

#define MAX_PLAYERS 4

// -- PLAYERS -- //
// size
#define PLAYER_RADIUS 30 // px
// movement
#define PLAYER_MAX_SPEED 100.0f // [px/s]
#define PLAYER_MAX_SPEED_SLOWED 37.5f // [px/s]
#define PLAYER_MAX_SPEED_CARRYING 50.0f // [px/s]
#define PLAYER_ACCELERATION 1200.0f // [px/s^2]
#define PLAYER_FRICTION 1500.0f // [px/s^2]
// interpolation
#define POSITION_OFFSET_TOLERANCE 5.0f // [px]

// -- PROJECTILES -- //
#define PROJECTILE_RADIUS 15 // [px]
#define PROJECTILE_THROW_COOLDOWN 1000 // [ms]
#define PROJECTILE_VELOCITY 500.0f
#define PROJECTILE_RANGE 1770*1770 // [px^2] 2500 px in any direction
#define PROJECTILE_POSTURE_DAMAGE 22
#define PROJECTILE_ANGULAR_VELOCITY 720.0f // [deg/s]

// -- FLAGS -- //
#define GAME_FLAG_SIZE {100, 60} // [px] w, h
#define GAME_FLAG_PICKUP_RANGE 100 // [px] from center of the flag
#define GAME_FLAG_HOME_POS_X 400 // [px] x, y
#define GAME_FLAG_HOME_POS_Y 300 // [px] x, y
#define GAME_FLAG_HOME_POS {GAME_FLAG_HOME_POS_X, GAME_FLAG_HOME_POS_Y} // [px] x, y


//
// RENDERING CONSTANTS
// 

// -- TEXTURES -- //

#ifdef DEBUG
#define ASSETS_PATH "../../assets/"
#else
#define ASSETS_PATH "assets/"
#endif


//
// TEXTURE IDS
//

#define TEXTURE_WALL            1
#define TEXTURE_WALL_PILLAR     2
#define TEXTURE_PILLAR          3
#define TEXTURE_SOFA            4
#define TEXTURE_CRATE           5

#define TEXTURE_ICE_TRAP        10
#define TEXTURE_SAND_TRAP       11

#define TEXTURE_PLAYER_1        20
#define TEXTURE_PLAYER_2        21
#define TEXTURE_PROJECTILE_1    22
#define TEXTURE_PROJECTILE_2    23
#define TEXTURE_FLAG            24
#define TEXTURE_SITE_1          25
#define TEXTURE_SITE_2          26
