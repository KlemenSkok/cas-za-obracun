
// MapData.hpp

#include "XML/tinyxml2.h"
#include "Game/Map/MapData.hpp"
#include "Utilities/AssetManager.hpp"
#include "Game/Map/Trap.hpp"

#include <exception>
#include <cmath>



std::unordered_map<uint16_t, std::unordered_map<uint16_t, std::vector<std::shared_ptr<MapObject>>>> MapData::grid;
std::unordered_map<uint8_t, std::shared_ptr<Site>> MapData::sites;
std::vector<SDL_Rect> MapData::floorTiles;



/* 

wall: 001
wall_pillar: 002
pillar: 003
crate: 004
sofa: 005

flag home pos: { 1478, 140}
flag size: {100, 60}

player start positions:
team 1: 
    {1084, 670}, {1084, 775}
team 2:
    {1940, 670}, {1940, 775}


*/




void MapData::InitializeGrid() {
    grid.clear();
    sites.clear();
}

void MapData::AddBarrier(Barrier& b) {
    auto pos = b.getPosition();
    
    int start_x = getGridKey(pos.x);
    int start_y = getGridKey(pos.y);
    int end_x = getGridKey(pos.x + b.getWidth());
    int end_y = getGridKey(pos.y + b.getHeight());

    // edge case: when barrier border is on the edge of a cell, expand the object territory
    if(int(pos.x + b.getWidth()) % GRID_CELL_SIZE == 0) end_x++;
    if(int(pos.y + b.getHeight()) % GRID_CELL_SIZE == 0) end_y++;

    // set the texture
    b.setTexture(AssetManager::GetTexture(b.getTextureId()));

    for(int x = start_x; x <= end_x; x++) {
        for(int y = start_y; y <= end_y; y++) {
            // add to grid
            grid[x][y].push_back(std::make_shared<Barrier>(b));
        }
    }
}

void MapData::AddTrap(Trap& t) {
    auto pos = t.getPosition();
    
    int start_x = getGridKey(pos.x);
    int start_y = getGridKey(pos.y);
    int end_x = getGridKey(pos.x + t.getWidth());
    int end_y = getGridKey(pos.y + t.getHeight());

    // edge case: when barrier border is on the edge of a cell, expand the object territory
    if(int(pos.x + t.getWidth()) % GRID_CELL_SIZE == 0) end_x++;
    if(int(pos.y + t.getHeight()) % GRID_CELL_SIZE == 0) end_y++;

    // set the texture
    t.setTexture(AssetManager::GetTexture(t.getTextureId()));

    for(int x = start_x; x <= end_x; x++) {
        for(int y = start_y; y <= end_y; y++) {
            // add to grid
            grid[x][y].push_back(std::make_shared<Trap>(t));
        }
    }
}

/**
 * @brief Read a single Barrier node from the XML file
 * 
 * @param node The node to read from 
 * @param b The Barrier object to write to
 * @return 0 on success, 1 on failure
 */
int parseBarrierNode(tinyxml2::XMLNode* node, Barrier& b) {
    using namespace tinyxml2;
    
    PointF pos;
    int w, h, texture_id;
    int err = 0;

    // parse barrier data
    // position
    XMLElement* n = node->FirstChildElement("position");
    if(n == nullptr) {
        Logger::warn((std::string("Missing barrier <position> data (line ") + std::to_string(node->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    err = n->QueryFloatAttribute("x", &pos.x);
    err += n->QueryFloatAttribute("y", &pos.y);
    if(err != 0) {
        Logger::warn((std::string("Failed to parse barrier <position> (line ") + std::to_string(n->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    
    // dimensions
    n = node->FirstChildElement("size");
    if(n == nullptr) {
        Logger::warn((std::string("Missing barrier <size> data (line ") + std::to_string(node->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    err = n->QueryIntAttribute("w", &w);
    err += n->QueryIntAttribute("h", &h);
    if(err != 0) {
        Logger::warn((std::string("Failed to parse barrier <size> (line ") + std::to_string(n->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    
    // texture
    n = node->FirstChildElement("texture");
    if(n == nullptr) {
        Logger::warn((std::string("Missing barrier <texture> data (line ") + std::to_string(node->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    err = n->QueryIntAttribute("id", &texture_id);
    if(err != 0) {
        Logger::warn((std::string("Failed to parse barrier <texture> (line ") + std::to_string(n->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }

    // boxes in the file have origin in the bottom left corner
    // we need to adjust the position to the top left corner

    pos.y -= h;

    b.setPosition(pos.x, pos.y);
    b.setDimensions(w, h);
    b.setTextureId(texture_id);

    return  EXIT_SUCCESS;
}

/**
 * @brief Read a single Site node from the XML file
 * 
 * @param node The node to read from 
 * @param b The Site object to write to
 * @return 0 on success, 1 on failure
 */
int parseSiteNode(tinyxml2::XMLNode* node, Site& s) {
    using namespace tinyxml2;
    
    PointF pos;
    Point size;
    int team;
    int err = 0;

    // parse barrier data
    // position
    XMLElement* n = node->FirstChildElement("position");
    if(n == nullptr) {
        Logger::warn((std::string("Missing site <position> data (line ") + std::to_string(node->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    err = n->QueryFloatAttribute("x", &pos.x);
    err += n->QueryFloatAttribute("y", &pos.y);
    if(err != 0) {
        Logger::warn((std::string("Failed to parse site <position> (line ") + std::to_string(n->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    
    // dimensions
    n = node->FirstChildElement("size");
    if(n == nullptr) {
        Logger::warn((std::string("Missing site <size> data (line ") + std::to_string(node->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    err = n->QueryIntAttribute("w", &size.x);
    err += n->QueryIntAttribute("h", &size.y);
    if(err != 0) {
        Logger::warn((std::string("Failed to parse site <size> (line ") + std::to_string(n->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    
    // texture
    n = node->FirstChildElement("team");
    if(n == nullptr) {
        Logger::warn((std::string("Missing site <team> data (line ") + std::to_string(node->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    err = n->QueryIntAttribute("id", &team);
    if(err != 0) {
        Logger::warn((std::string("Failed to parse site <team> (line ") + std::to_string(n->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }


    // boxes in the file have origin in the bottom left corner
    // we need to adjust the position to the top left corner

    pos.y -= size.y;

    s.setPosition(pos);
    s.setSize(size);
    s.setTeam(team);

    return  EXIT_SUCCESS;
}


/**
 * @brief Read a single Trap node from the XML file
 * 
 * @param node The node to read from 
 * @param b The Trap object to write to
 * @return 0 on success, 1 on failure
 */
int parseTrapNode(tinyxml2::XMLNode* node, Trap& s) {
    using namespace tinyxml2;
    
    PointF pos;
    Point size;
    int texture_id;
    char* type;
    int err = 0;

    // parse barrier data
    // position
    XMLElement* n = node->FirstChildElement("position");
    if(n == nullptr) {
        Logger::warn((std::string("Missing trap <position> data (line ") + std::to_string(node->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    err = n->QueryFloatAttribute("x", &pos.x);
    err += n->QueryFloatAttribute("y", &pos.y);
    if(err != 0) {
        Logger::warn((std::string("Failed to parse trap <position> (line ") + std::to_string(n->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    
    // dimensions
    n = node->FirstChildElement("size");
    if(n == nullptr) {
        Logger::warn((std::string("Missing trap <size> data (line ") + std::to_string(node->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    err = n->QueryIntAttribute("w", &size.x);
    err += n->QueryIntAttribute("h", &size.y);
    if(err != 0) {
        Logger::warn((std::string("Failed to parse trap <size> (line ") + std::to_string(n->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    
    // texture
    n = node->FirstChildElement("texture");
    if(n == nullptr) {
        Logger::warn((std::string("Missing trap <texture> data (line ") + std::to_string(node->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    err = n->QueryIntAttribute("id", &texture_id);
    if(err != 0) {
        Logger::warn((std::string("Failed to parse trap <texture> (line ") + std::to_string(n->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }

    // type
    n = node->FirstChildElement("type");
    if(n == nullptr) {
        Logger::warn((std::string("Missing trap <type> data (line ") + std::to_string(node->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    err = n->QueryStringAttribute("name", (const char**)&type);
    if(err != 0) {
        Logger::warn((std::string("Failed to parse trap <type> (line ") + std::to_string(n->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }



    // boxes in the file have origin in the bottom left corner
    // we need to adjust the position to the top left corner

    pos.y -= size.y;

    s.setPosition(pos.x, pos.y);
    s.setDimensions(size.x, size.y);
    s.setTextureId(texture_id);
    s.setTrapType(type);


    return  EXIT_SUCCESS;
}


/**
 * @brief Read a single Trap node from the XML file
 * 
 * @param node The node to read from 
 * @param b The Trap object to write to
 * @return 0 on success, 1 on failure
 */
int parseTileNode(tinyxml2::XMLNode* node, SDL_Rect& t) {
    using namespace tinyxml2;
    
    PointF pos;
    Point size;
    int err = 0;

    // parse barrier data
    // position
    XMLElement* n = node->FirstChildElement("position");
    if(n == nullptr) {
        Logger::warn((std::string("Missing trap <position> data (line ") + std::to_string(node->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    err = n->QueryFloatAttribute("x", &pos.x);
    err += n->QueryFloatAttribute("y", &pos.y);
    if(err != 0) {
        Logger::warn((std::string("Failed to parse trap <position> (line ") + std::to_string(n->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    
    // dimensions
    n = node->FirstChildElement("size");
    if(n == nullptr) {
        Logger::warn((std::string("Missing trap <size> data (line ") + std::to_string(node->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    err = n->QueryIntAttribute("w", &size.x);
    err += n->QueryIntAttribute("h", &size.y);
    if(err != 0) {
        Logger::warn((std::string("Failed to parse trap <size> (line ") + std::to_string(n->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }

    
    // boxes in the file have origin in the bottom left corner
    // we need to adjust the position to the top left corner
    //pos.y -= size.y;


    t.x = pos.x;
    t.y = pos.y;
    t.w = size.x;
    t.h = size.y;

    //std::cout << "Tile: " << t.x << ", " << t.y << ", " << t.w << ", " << t.h << std::endl;

    return  EXIT_SUCCESS;
}


/**
 * @brief Attempt to load map data from an XML file.
 * 
 * @param filename Name of the XML file to load.
 * @return 0 on success, 1 in failure.
 */
int MapData::LoadMap(const char* filename) {
    //std::cout << "Loading map: " << filename << std::endl;
    InitializeGrid();

    // use tinyxml2 for parsing
    using namespace tinyxml2;


    XMLDocument doc;
    if(doc.LoadFile(filename) != 0) {
        throw std::runtime_error("Failed to load XML file.");
    }

    XMLNode* root = doc.FirstChildElement("map");
    if(root == nullptr) {
        throw std::runtime_error("Failed to find <map> root element.");
    }

    // use XMLHandle with automatic nullptr checks
    XMLHandle rootHandle(root);
    
    XMLNode* barriers = rootHandle.FirstChildElement("barriers").FirstChildElement("barrier").ToNode();
    if(barriers == nullptr) {
        throw std::runtime_error("Failed to find barriers in the map data.");
    }
    else {
        // extract map barriers
        for(XMLNode* node = barriers; node != nullptr; node = node->NextSiblingElement("barrier")) {
            Barrier b;
            if(parseBarrierNode(node, b) == 0) {
                AddBarrier(b);
            }
        }
    }

    XMLNode* sites = rootHandle.FirstChildElement("sites").FirstChildElement("site").ToNode();
    if(sites == nullptr) {
        throw std::runtime_error("Failed to find sites in the map data.");
    }
    else {
        for(XMLNode* node = sites; node != nullptr; node = node->NextSiblingElement("site")) {
            Site s;
            if(parseSiteNode(node, s) == 0) {
                MapData::sites[s.getTeam()] = std::make_shared<Site>(s);

                if(s.getTeam() == 1) {
                    MapData::sites[s.getTeam()]->setTexture(AssetManager::GetTexture(TEXTURE_SITE_1));
                }
                else {
                    MapData::sites[s.getTeam()]->setTexture(AssetManager::GetTexture(TEXTURE_SITE_2));
                }
            }
        }
    }

    XMLNode* traps = rootHandle.FirstChildElement("traps").FirstChildElement("trap").ToNode();
    if(traps == nullptr) {
        throw std::runtime_error("Failed to find traps in the map data.");
    }
    else {
        // extract map traps
        for(XMLNode* node = traps; node != nullptr; node = node->NextSiblingElement("trap")) {
            Trap t;
            if(parseTrapNode(node, t) == 0) {
                AddTrap(t);
            }
        }
    }

    XMLNode* tiles = rootHandle.FirstChildElement("floor").FirstChildElement("tile").ToNode();
    if(tiles == nullptr) {
        throw std::runtime_error("Failed to find floor tiles in the map data.");
    }
    else {
        // extract floor tiles
        for(XMLNode* node = tiles; node != nullptr; node = node->NextSiblingElement("tile")) {
            SDL_Rect t;
            if(parseTileNode(node, t) == 0) {
                floorTiles.push_back(t);
            }
        }
    }
    

    return EXIT_SUCCESS;
}

/**
 * @brief Check for collision between the local player and map barriers.
 * 
 * @param player Player object to check collision for. Used to identify the overload for the LocalPlayer class.
 * @param correctedPos The position to check for collision. If a collision is detected, this will be updated to the corrected position.
 * @return `true` if collision was detected, `false` otherwise.
 */
bool MapData::CheckCollision(LocalPlayer& player, PointF& correctedPos) {
    // player grid position
    int p_grid_x = getGridKey(correctedPos.x);
    int p_grid_y = getGridKey(correctedPos.y);

    bool isColliding = false;
    for(int x = p_grid_x - 1; x <= p_grid_x + 1; x++) {
        for(int y = p_grid_y - 1; y <= p_grid_y + 1; y++) {

            if(grid.find(x) == grid.end() || grid[x].find(y) == grid[x].end()) {
                continue; // skip; nothing to check in this cell
            }

            for(const auto& object : grid[x][y]) {

                if(object->getType() == MapObjType::BARRIER) {

                    float closestX = std::max(object->getPosition().x, 
                        std::min(correctedPos.x, object->getPosition().x + object->getWidth()));
                    float closestY = std::max(object->getPosition().y, 
                        std::min(correctedPos.y, object->getPosition().y + object->getHeight()));
                    
                    float distanceX = correctedPos.x - closestX;
                    float distanceY = correctedPos.y - closestY;
                    float distanceSQ = (distanceX * distanceX) + (distanceY * distanceY);
                    constexpr int playerRad = PLAYER_RADIUS * PLAYER_RADIUS;
                    
                    if(distanceSQ < playerRad) {
                            // collision detected
                            float distance = std::sqrt(distanceSQ);
                            float overlap = PLAYER_RADIUS - distance;
                            
                        if(distance > 0) {
                            // push player away from the barrier
                            correctedPos.x += (distanceX / distance) * overlap;
                            correctedPos.y += (distanceY / distance) * overlap;
                        }
                        else {
                            // edge case: player is exactly inside the barrier
                            correctedPos.x += PLAYER_RADIUS;
                        }
                        
                        isColliding = true;
                    }
                }
                else if(object->getType() == MapObjType::TRAP) {
                    if(
                        correctedPos.x > object->getPosition().x &&
                        correctedPos.x < object->getPosition().x + object->getWidth() &&
                        correctedPos.y > object->getPosition().y &&
                        correctedPos.y < object->getPosition().y + object->getHeight()
                    ) 
                    {
                        auto trap = dynamic_cast<Trap*>(object.get());

                        player.setNextAcceleration(trap->getAccelerationCoefficient());
                        player.setNextFriction(trap->getFrictionCoefficient());
                        player.setNextSpeedCap(trap->getMaxSpeed());
                    }
                }
            }
        }
    }
    return isColliding;
}

/**
 * @brief Check for collision between the remote player and the map barriers.
 * 
 * @param player Player object to check collision for. Used to identify the overload for the RemotePlayer class.
 * @param correctedPos The position to check for collision. If a collision is detected, this will be updated to the corrected position.
 * @return `true` if collision was detected, `false` otherwise.
 */
bool MapData::CheckCollision(RemotePlayer& player, PointF& correctedPos) {
        // player grid position
        int p_grid_x = getGridKey(correctedPos.x);
        int p_grid_y = getGridKey(correctedPos.y);
    
        for(int x = p_grid_x - 1; x <= p_grid_x + 1; x++) {
            for(int y = p_grid_y - 1; y <= p_grid_y + 1; y++) {
    
                if(grid.find(x) == grid.end() || grid[x].find(y) == grid[x].end()) {
                    continue; // skip; nothing to check in this cell
                }
    
                for(const auto& object : grid[x][y]) {

                    if(object->getType() == MapObjType::BARRIER) {
                        
                        float closestX = std::max(object->getPosition().x, 
                            std::min(correctedPos.x, object->getPosition().x + object->getWidth()));
                        float closestY = std::max(object->getPosition().y, 
                            std::min(correctedPos.y, object->getPosition().y + object->getHeight()));
                        
                        float distanceX = correctedPos.x - closestX;
                        float distanceY = correctedPos.y - closestY;
                        float distanceSQ = (distanceX * distanceX) + (distanceY * distanceY);
                        constexpr int playerRad = PLAYER_RADIUS * PLAYER_RADIUS;
                        
                        if(distanceSQ < playerRad) {
                            // collision detected
                            float distance = std::sqrt(distanceSQ);
                            float overlap = PLAYER_RADIUS - distance;
                            
                            if(distance > 0) {
                                // push player away from the barrier
                                correctedPos.x += (distanceX / distance) * overlap;
                                correctedPos.y += (distanceY / distance) * overlap;
                            }
                            else {
                                // edge case: player is exactly inside the barrier
                                correctedPos.x += PLAYER_RADIUS;
                            }
                            
                            return true;
                        }
                    }
                    else if(object->getType() == MapObjType::TRAP) {
                        if(
                            correctedPos.x > object->getPosition().x &&
                            correctedPos.x < object->getPosition().x + object->getWidth() &&
                            correctedPos.y > object->getPosition().y &&
                            correctedPos.y < object->getPosition().y + object->getHeight()
                        ) 
                        {
                            auto trap = dynamic_cast<Trap*>(object.get());
    
                            player.setNextAcceleration(trap->getAccelerationCoefficient());
                            player.setNextFriction(trap->getFrictionCoefficient());
                            player.setNextSpeedCap(trap->getMaxSpeed());
                        }
                    }
                }
            }
        }
        return false;
}