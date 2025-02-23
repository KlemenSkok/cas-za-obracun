
// Renderer.cpp

#include "Rendering/RenderWindow.hpp"
#include "Utilities/Utility.hpp"
#include "Containers.hpp"
#include "Rendering/RenderingContext.hpp"



/**
 * @brief Render game state relative to the player's position.
 * Thus, the remote players' windows position equals (player's position - remote player's position) + window center
 */
void RenderWindow::renderGameState() {

    // get player position for reference
    rc::localPlayerPos = Game::player->getPosition();
    rc::windowCenter = {Window::Width() / 2.0f, Window::Height() / 2.0f};

    // clear the screen
    Window::Clear();
    
    // render the map
    // todo

    // render the players
    Game::player->render(Window::renderer);
    for(auto& [i, player] : Game::remote_players) {
        player->render(Window::renderer);
    }

    // render the projectiles
    for(auto& [i, projectile] : Game::projectiles) {
        projectile->render(Window::renderer);
    }

    // present changes
    Window::Present();

}