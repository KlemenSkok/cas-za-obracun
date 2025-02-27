
// Renderer.cpp

#include "Rendering/RenderWindow.hpp"
#include "Utilities/Utility.hpp"
#include "Containers.hpp"
#include "Rendering/RenderingContext.hpp"
#include "Game/Map/MapData.hpp"



/**
 * @brief Render game state relative to the player's position.
 * Thus, the remote players' windows position equals (player's position - remote player's position) + window center
 */
void RenderWindow::renderGameState() {

    // get player position for reference
    rc::localPlayerPos = Game::player->getPosition();
    rc::windowCenter = {static_cast<int>(Window::Width() / 2.0f), static_cast<int>(Window::Height() / 2.0f)};

    // clear the screen
    Window::Clear();
    
    // render the map
    for(auto& [x, y_map] : MapData::grid) {
        for(auto& [y, barriers] : y_map) {
            for(auto& barrier : barriers) {
                barrier.render(Window::renderer);
            }
        }
    }
    for(auto& [i, site] : MapData::sites) {
        site->render(Window::renderer);
    }

    // render the players
    Game::player->render(Window::renderer);
    for(auto& [i, player] : Game::remote_players) {
        player->render(Window::renderer);
    }

    // render the projectiles
    for(auto& [i, projectile] : Game::projectiles) {
        projectile->render(Window::renderer);
    }

    // render flag
    Game::flag->render(Window::renderer);

    // present changes
    Window::Present();

}