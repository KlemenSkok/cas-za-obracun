
// Game.cpp

#include "Game.hpp"
#include "Utilities/Utility.hpp"
#include "Communication/SocketHandler.hpp"
#include "Logging/Logger.hpp"
#include "Containers.hpp"
#include "UI/UIManager.hpp"
#include "Utilities/AssetManager.hpp"

#include <iostream>
#include <stdexcept>



// static members

bool Game::_running = false;

Game::ConnectionInfo Game::server_info;

GameState Game::current_state = GameState::NONE;
Uint32 Game::last_state_change = 0;
std::vector<uint8_t> Game::scores(2, 0);
uint8_t Game::last_winner = 0;

uint8_t Game::session_id = 0;
uint16_t Game::client_id = 0;

std::shared_ptr<LocalPlayer> Game::player = nullptr;
std::shared_ptr<Flag> Game::flag = nullptr;
std::unordered_map<uint16_t, std::shared_ptr<RemotePlayer>> Game::remote_players;
std::unordered_map<uint16_t, std::shared_ptr<Projectile>> Game::projectiles;



void Game::Setup() {
    // attempt to start the communication thread
    try {
        SocketHandler::Start();
    }
    catch (std::runtime_error &e) {
        throw std::runtime_error(std::string("Failed to start communication thread: ") + e.what());
    }

    // slight desync
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    // open the game window
    Window::Open();
    // open fonts
    Fonts::primaryFont = Fonts::LoadFont(ASSETS_PATH"fonts/CourierPrime-Regular.ttf", 100);
    // initialize the asset manager
    AssetManager::Initialize();
    // load all textures
    RenderWindow::loadScreens();

    // Attempt to load the map data
    try {
        MapData::LoadMap(ASSETS_PATH"data/map_data.xml");
    }
    catch (std::exception &e) {
        throw std::runtime_error(std::string("Failed to load map data: ") + e.what());
    }
}

void Game::Cleanup() {
    Logger::info("Cleaning up...");

    Fonts::Clear();

    AssetManager::Clear();
    SocketHandler::Stop();

}

void Game::setServerIP(int argc, char *argv[]) {

    // no arguments passed, use default values
    if(argc == 1) {
        if(SDLNet_ResolveHost(&Game::server_info.addr, DEFAULT_SERVER_IP, DEFAULT_SERVER_PORT) == -1) {
            throw std::runtime_error("Failed to resolve host.");
        }
    }
    else if(argc == 3) {
        int port = 0;
        try {
            port = std::stoi(argv[2]);
        }
        catch(std::invalid_argument &e) {
            throw std::runtime_error("Invalid port number.");
        }
        catch(std::out_of_range &e) {
            throw std::runtime_error("Port number out of range.");
        }

        if(SDLNet_ResolveHost(&Game::server_info.addr, argv[1], port) == -1) {
            throw std::runtime_error("Failed to resolve host.");
        }
    }
    else {
        throw std::runtime_error("Invalid number of arguments.");
    }

    Logger::info(("Server IP resolved: " + formatIP(Game::server_info.addr.host) + ":" + std::to_string(SDLNet_Read16(&Game::server_info.addr.port))).c_str());

    SDLNet_UDP_Bind(SocketHandler::getSocket(), Game::server_info.channel, &Game::server_info.addr);

}

/**
 * @brief Initialize the game state.
 * 
 */
void Game::Initialize() {

    // set all starting values
    Game::player = std::make_shared<LocalPlayer>(100, 100, 0);
    Game::flag = std::make_shared<Flag>(GAME_FLAG_HOME_POS_X, GAME_FLAG_HOME_POS_Y);
    flag->setTexture(AssetManager::GetTexture(TEXTURE_FLAG));

    Game::_running = true;
    // start connecting to the server
    Game::server_info.connection_state = ConnectionState::DISCONNECTED;

}

/**
 * @brief Run the game loop.
 * 
 */
void Game::Run() {
    
    Game::Initialize();
    Uint32 lastUpdate = SDL_GetTicks();

    while(Game::_running) {
        Uint32 now = SDL_GetTicks();
        if(now - lastUpdate < GAME_LOOP_DELAY) {
            continue;
        }
        int deltaTime = now - lastUpdate;
        lastUpdate = now;

        // update the local game state
        // send the updates to the server
        // show the game state

        Game::processNewPackets();
        EventHandler::HandleEvents();
        Game::manageConnection();
        PacketHandler::sendPendingPackets();

        if(Game::current_state != GameState::NONE) {
            Game::Update(deltaTime);
        }

        Game::Render();


        // LOOP IDEA:
        // process server updates
        // process user input
        // send updates to the server
        // show the game state
        // repeat


    }
    Window::Close();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

/**
 * @brief Update the game state based on the time passed since the last update.
 * 
 * @param deltaTime Time passed since the last update in milliseconds.
 */
void Game::Update(int deltaTime) {

    float t = deltaTime / 1000.0f;

    // update the local player's position   
    Game::player->update(t);

    // update remote players
    for(auto& p : Game::remote_players) {
        p.second->update(t);
    }
    for(auto& pr : Game::projectiles) {
        pr.second->update(t);
    }

    // update the flag
    Game::flag->update(t);

    if(Game::flag->getCarrierID() == Game::client_id && (Game::client_id != 0)) {
        // update the flag's position
        // override the flag's position with the player's position to cancel out the interpolation
        Game::flag->updatePosition(Game::player->position);
        Game::flag->setCarrierTeam(Game::player->getTeamNumber());
        Game::player->captureFlag();
    }
    else {
        Game::player->dropFlag();
    }

}

/**
 * @brief Render the game state.
 */
void Game::Render() {

    // render the game state
    RenderWindow::renderWindow();

}


/**
 * @brief Classify incoming packets based on flags and process them accordingly.
 */
void Game::processNewPackets() {

    // Omejimo stevilo paketov, ki jih obdelamo naenkrat,
    // da ne blokiramo glavne niti
    constexpr const uint8_t max_packets = 100;
    uint8_t num_packets = 0;

    // check for recieved messages
    PacketData data;
    while(getMessageFromQueue(data) && num_packets < max_packets) {
        
        // handle the message
        // --------------------------------------

        for(int i = NUM_FLAGS - 1; i >= 0; i--) {
            if(data.flags() & (1 << i)) {
                switch(i) {
                    case FLAG_ACK:
                        // look for any awaiting confirmations
                        break;
                    case FLAG_SYN:
                        // start the connection (get the client id)
                        uint16_t c_id;
                        uint8_t s_id;
                        try {
                            data.getByOffset(c_id, sizeof(c_id), OFFSET_CLIENT_ID);
                            data.getByOffset(s_id, sizeof(s_id), OFFSET_SESSION_ID);
                        }
                        catch (std::out_of_range &e) {
                            Logger::error("Failed to retrieve client id or session id from the packet.");
                            break;
                        }
                        Logger::info(("Client ID: " + std::to_string(c_id) + ", Session ID: " + std::to_string(s_id)).c_str());
                        Game::client_id = c_id;
                        Game::session_id = s_id;

                        Game::server_info.connection_state = ConnectionState::CONNECTED;
                        gui::currentScreen = RenderState::GAME;
                        scores = {0, 0};
                        break;
                    case FLAG_FIN:
                        // close the connection
                        Game::server_info.connection_state = ConnectionState::DISCONNECTED;
                        Game::client_id = 0;
                        Game::session_id = 0;
                        break;
                    case FLAG_DATA:
                        // process the data and update local game state
                        PacketHandler::processPacket(data);
                        break;
                    default:
                        Logger::info(("A packet was ignored: " + std::string((char*)data.getRawData().get())).c_str());
                        // ignore the packet
                }
            }
        }



        // --------------------------------------

        num_packets++;
    }

}

void Game::manageConnection() {
    static std::chrono::steady_clock::time_point lastPacketTime = std::chrono::steady_clock::now();


    switch(Game::server_info.connection_state) {
        case ConnectionState::DISCONNECTED:
            // do nothing
            if(gui::currentScreen == RenderState::CONNECTING) {
                Game::server_info.connection_state = ConnectionState::CONNECTING;
            }
            else break;
        case ConnectionState::CONNECTING:
            // check if the connection was established
            // if not, try again
            if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastPacketTime).count() > CONNECTION_REQUEST_INTERVAL) {
                // send a connection request
                PacketData m(true);
                m.flags() |= (1 << FLAG_SYN);
                PacketHandler::appendToLocalQueue(m, Game::server_info.channel);
                //addMessageToQueue(m, Game::server_info.channel);
                lastPacketTime = std::chrono::steady_clock::now();
                
                Logger::info("Connection request sent.");
            }
            break;
            case ConnectionState::CONNECTED:
            // send a keepalive message every 5 seconds
            if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastPacketTime).count() > KEEPALIVE_INTERVAL) {
                // send a keepalive message
                PacketData m(true);
                m.flags() |= (1 << FLAG_KEEPALIVE);
                m.append(Game::session_id);
                m.append(Game::client_id);
                PacketHandler::appendToLocalQueue(m, Game::server_info.channel);
                //addMessageToQueue(m, Game::server_info.channel);
                lastPacketTime = std::chrono::steady_clock::now();
                
                //Logger::info("Keepalive message sent.");
                if(gui::currentScreen == RenderState::MAIN_MENU && current_state == GameState::GAME_FINISHED) {
                    // exit
                    //Game::_running = false;
                    //Game::server_info.connection_state = ConnectionState::DISCONNECTED;
                    // do tle more pridt
                    std::cout << "Game finished." << std::endl;
                }
            }
            break;
            case ConnectionState::DISCONNECTING:
            // check if the disconnection was successful
            // if not, try again
            if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastPacketTime).count() > DISCONNECTION_REQUEST_INTERVAL) {
                // send a disconnection request
                PacketData m(true);
                m.flags() |= (1 << FLAG_FIN);
                m.append(Game::session_id);
                m.append(Game::client_id);
                PacketHandler::appendToLocalQueue(m, Game::server_info.channel);
                //addMessageToQueue(m, Game::server_info.channel);
                lastPacketTime = std::chrono::steady_clock::now();

                Logger::info("Disconnection request sent.");
            }
            break;
    }
}

void Game::sendPacket(PacketData& d) {
    addMessageToQueue(d, Game::server_info.channel);
}

// not very reliable
void Game::resetConnection() {
    Game::server_info.connection_state = ConnectionState::DISCONNECTING;
    Game::last_winner = 0;

    Game::projectiles.clear();
    Game::remote_players.clear();
    Game::flag->reset();
}
