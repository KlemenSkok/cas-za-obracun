
// EventHandler.hpp

#pragma once

#include <SDL2/SDL.h>
#include "Containers.hpp"

class EventHandler {
public:
    static bool LockKeyboard;
    static KeyStates keyStates;
    static void HandleEvents();

};
