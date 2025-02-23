
// MapData.hpp

#include "XML/tinyxml2.h"
#include "Game/Map/MapData.hpp"

#include <exception>



std::unordered_map<uint16_t, std::unordered_map<uint16_t, std::vector<Barrier>>> MapData::grid;


void MapData::InitializeGrid() {
    grid.clear();
}

void MapData::AddBarrier(Barrier& b) {
    auto pos = b.getPosition();
    
    int start_x = getGridKey(pos.x);
    int start_y = getGridKey(pos.y);
    int end_x = getGridKey(pos.x + b.getWidth());
    int end_y = getGridKey(pos.y + b.getHeight());

    for(int x = start_x; x < end_x; x++) {
        for(int y = start_y; y < end_y; y++) {
            grid[x][y].push_back(b);
        }
    }
}


int parseBarrierNode(tinyxml2::XMLNode* node, Barrier& b) {
    using namespace tinyxml2;
    
    Point pos;
    int w, h, texture_id;
    int err = 0;
    
    // parse barrier data
    // position
    XMLElement* n = node->FirstChildElement("position");
    if(n == nullptr) {
        Logger::warn((std::string("Missing barrier *position* data (line ") + std::to_string(node->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    err = n->QueryFloatAttribute("x", &pos.x);
    err += n->QueryFloatAttribute("y", &pos.y);
    if(err != 0) {
        Logger::warn((std::string("Failed to parse barrier *position* (line ") + std::to_string(n->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    
    // dimensions
    n = node->FirstChildElement("size");
    if(n == nullptr) {
        Logger::warn((std::string("Missing barrier *size* data (line ") + std::to_string(node->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    err = n->QueryIntAttribute("w", &w);
    err += n->QueryIntAttribute("h", &h);
    if(err != 0) {
        Logger::warn((std::string("Failed to parse barrier *size* (line ") + std::to_string(n->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    
    // texture
    n = node->FirstChildElement("texture");
    if(n == nullptr) {
        Logger::warn((std::string("Missing barrier *texture* data (line ") + std::to_string(node->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }
    err = n->QueryIntAttribute("id", &texture_id);
    if(err != 0) {
        Logger::warn((std::string("Failed to parse barrier *texture* (line ") + std::to_string(n->GetLineNum()) + ").").c_str());
        return EXIT_FAILURE;
    }

    b.setPosition(pos.x, pos.y);
    b.setDimensions(w, h);
    b.setTexture(texture_id);

    return  EXIT_SUCCESS;
}

/**
 * @brief Attempt to load map data from an XML file.
 * 
 * @param filename Name of the XML file to load.
 * @return int Returns 0 on success, -1 in failure.
 */
int MapData::LoadMap(const char* filename) {
    std::cout << "Loading map: " << filename << std::endl;
    InitializeGrid();

    // use tinyxml2 for parsing
    using namespace tinyxml2;


    XMLDocument doc;
    if(doc.LoadFile(filename) != 0) {
        throw std::runtime_error("Failed to load XML file.");
    }

    XMLNode* root = doc.FirstChildElement("map");
    if(root == nullptr) {
        throw std::runtime_error("Failed to find *map* root element.");
    }
    
    for(XMLNode* node = root->FirstChildElement("barrier"); node != nullptr; node = node->NextSiblingElement("barrier")) {
        
        Barrier b;
        if(parseBarrierNode(node, b) == 0) {
            AddBarrier(b);
            //std::cout << "Barrier added: " << b.getPosition().x << ", " << b.getPosition().y << std::endl;
        }

    }

    return EXIT_SUCCESS;
}