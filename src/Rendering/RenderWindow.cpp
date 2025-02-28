
// Renderer.cpp

#include "Rendering/RenderingContext.hpp"
#include "Rendering/RenderWindow.hpp"
#include "Utilities/AssetManager.hpp"
#include "Game/Map/MapData.hpp"
#include "Containers.hpp"
#include "UI/UIManager.hpp"


std::unordered_map<uint8_t, SDL_Texture*> RenderWindow::screens;


void RenderWindow::loadScreens() {

    // load all textures
    SDL_Texture* t;

    // main menu
    t = AssetManager::LoadTexture("images/GUI/screens/main_menu.png");
    screens[static_cast<uint8_t>(RenderState::MAIN_MENU)] = t;

    // loading screen
    t = AssetManager::LoadTexture("images/GUI/screens/connecting_menu.png");
    screens[static_cast<uint8_t>(RenderState::CONNECTING)] = t;

    // game state doesn't need a texture; it shows dinamically

    // game over menu
    t = AssetManager::LoadTexture("images/GUI/screens/game_over.png");
    screens[static_cast<uint8_t>(RenderState::GAME_OVER)] = t;

}


/**
 * @brief Render the game window based on the current game state.
 * 
 */
void RenderWindow::renderWindow() {

    switch(gui::currentScreen) {
        case RenderState::MAIN_MENU:
        case RenderState::CONNECTING:
        case RenderState::GAME_OVER:
            // render the current screen
            renderCurrentScreen();
            break;

        case RenderState::GAME:
            // render the game state
            renderGameState();
            break;
    }

}


/**
 @brief Render game state relative to the player's position.
 * Thus, the remote players' position equals (player's position - remote player's position) + window center
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


void RenderWindow::renderCurrentScreen() {
    // clear the screen
    Window::Clear();

    // double check the game state
    if(!(gui::currentScreen == RenderState::GAME)) {
        // render the loading screen
        SDL_RenderCopy(Window::renderer, screens[static_cast<uint8_t>(gui::currentScreen)], NULL, NULL);
    }

    // present changes
    Window::Present();

}