
// UIManager.hpp

#pragma once

#include "Utilities/Utility.hpp"


namespace gui {
    extern bool expectsInput;
    extern RenderState currentScreen;
    void updateState(GameState, ConnectionState);
    void processInput(int);

}