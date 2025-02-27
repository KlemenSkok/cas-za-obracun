
// EventHandler.hpp

#pragma once

#include <SDL2/SDL.h>
#include "Containers.hpp"

class EventHandler {
    
    static bool keyboardLocked;
    
public:

    static void LockKeyboard();
    static void UnlockKeyboard();

    static KeyStates keyStates;
    static void HandleEvents();

};
