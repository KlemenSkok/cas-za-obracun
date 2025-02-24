
// MapData.hpp

#pragma once

#include "Barrier.hpp"
#include "Game/GameObject.hpp"
#include "Game/LocalPlayer.hpp"

#include <vector>
#include <unordered_map>

#define GRID_CELL_SIZE 100 // [px]

class LocalPlayer;

class MapData {

public:

    static std::unordered_map<uint16_t, std::unordered_map<uint16_t, std::vector<Barrier>>> grid;

    static void InitializeGrid();
    static void AddBarrier(Barrier&);
    static int LoadMap(const char*);
    static bool CheckCollision(const LocalPlayer&, Point&);

};

inline int getGridKey(float val) {
    return static_cast<int>(val) / GRID_CELL_SIZE;
}