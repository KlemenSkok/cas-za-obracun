
// Renderer.cpp

#include "Rendering/RenderingContext.hpp"
#include "Rendering/RenderWindow.hpp"
#include "Utilities/AssetManager.hpp"
#include "Game/Map/MapData.hpp"
#include "Containers.hpp"
#include "UI/UIManager.hpp"

#include <cmath>
#include <algorithm>


std::unordered_map<uint8_t, SDL_Texture*> RenderWindow::screens;
SDL_Texture* RenderWindow::screen_victory;
SDL_Texture* RenderWindow::screen_defeat;

RenderState RenderWindow::prev_state = RenderState::MAIN_MENU;


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

    // victory screen
    t = AssetManager::LoadTexture("images/GUI/screens/game_over_victory.png");
    screen_victory = t;

    // defeat screen
    t = AssetManager::LoadTexture("images/GUI/screens/game_over_defeat.png");
    screen_defeat = t;

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

    prev_state = gui::currentScreen;

}


/**
 @brief Render game state relative to the player's position.
 * Thus, the remote players' positions equal to (player's position - remote player's position) + window center
 */
void RenderWindow::renderGameState() {

    // get player position for reference
    rc::localPlayerPos = Game::player->getPosition();
    rc::windowCenter = {static_cast<int>(Window::Width() / 2.0f), static_cast<int>(Window::Height() / 2.0f)};

    // clear the screen
    Window::Clear();

    {
        // render the floor
        SDL_Rect src = {0, 0, 100, 100};
        int src_w, src_h;
        SDL_Texture* texture = AssetManager::GetTexture(TEXTURE_FLOOR);
        for(auto& rect : MapData::floorTiles) {
            SDL_Rect dest = {static_cast<int>(rect.x - rc::localPlayerPos.x + rc::windowCenter.x), static_cast<int>(rect.y - rc::localPlayerPos.y + rc::windowCenter.y), rect.w, rect.h};
            SDL_QueryTexture(texture, NULL, NULL, &src_w, &src_h);
            // scale the texture to the destination rectangle
            src.w = src_h * dest.w / dest.h;
            src.h = src_w * dest.h / dest.w;
            SDL_RenderCopy(Window::renderer, texture, &src, &dest);
        }
    }

    // render the map
    for(auto& [x, y_map] : MapData::grid) {
        for(auto& [y, objects] : y_map) {
            for(auto& object : objects) {
                object->render(Window::renderer);
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

    // render UI overlay
    RenderWindow::renderGameUI();
    

    // present changes
    Window::Present();

}


void RenderWindow::renderCurrentScreen() {
    // clear the screen
    Window::Clear();

    switch(gui::currentScreen) {
        case RenderState::MAIN_MENU:
        case RenderState::CONNECTING:
            // render the current screen
            SDL_RenderCopy(Window::renderer, screens[static_cast<uint8_t>(gui::currentScreen)], NULL, NULL);
            break;
        case RenderState::GAME_OVER:
            // render the game over screen
            if(Game::scores[0] > Game::scores[1] && Game::player->getTeamNumber() == 1) {
                SDL_RenderCopy(Window::renderer, screen_victory, NULL, NULL);
            }
            else {
                SDL_RenderCopy(Window::renderer, screen_defeat, NULL, NULL);
            }
            break;
    }

    // present changes
    Window::Present();

}


void renderScore(int s1, int s2, int myTeam);

void RenderWindow::renderGameUI() {

    //
    // RENDER THE ARROW TO THE FLAG
    //

    PointF flagPos = Game::flag->getPosition();
    Point flagSize = Game::flag->getSize();

    float dx = flagPos.x - rc::localPlayerPos.x + flagSize.x / 2;
    float dy = flagPos.y - rc::localPlayerPos.y + flagSize.y / 2;

    if((std::abs(dx) > rc::windowCenter.x) || 
        (std::abs(dy) > rc::windowCenter.y))
    {
        // only render the arrow if the flag is outside the window
        SDL_Texture* arrow = AssetManager::GetTexture(Game::flag->getCarrierTeam() == 1 ? TEXTURE_ARROW_BLUE : Game::flag->getCarrierTeam() == 2 ? TEXTURE_ARROW_RED : TEXTURE_ARROW_NEUTRAL);

        float angle = atan2(dy, dx) * 180 / M_PI;

        // get the arrow render position
        SDL_Rect dest = {0, 0, 50, 50};

        if(angle >= -45.0f && angle < 45.0f) {
            // Right edge
            dest.x = Window::Width() - dest.w;
            dest.y = rc::windowCenter.y + (dy + Game::flag->getSize().y / 2) / 2.0f - dest.h / 2.0f;
        }
        else if(angle > -135.0f && angle <= -45.0f) {
            // Top edge
            dest.y = 0;
            dest.x = rc::windowCenter.x + (dx + Game::flag->getSize().x / 2) / 2.0f - dest.w / 2.0f;
        }
        else if(angle >= 135.0f || angle < -135.0f) {
            // Left edge
            dest.x = 0;
            dest.y = rc::windowCenter.y + (dy + Game::flag->getSize().y / 2) / 2.0f - dest.h / 2.0f;
        }
        else {
            // Bottom edge
            dest.y = Window::Height() - dest.h;
            dest.x = rc::windowCenter.x + (dx + Game::flag->getSize().x / 2) / 2.0f - dest.w / 2.0f;
        }

        // Snap the arrow to the window edges
        dest.x = std::clamp((int)dest.x, 0, Window::Width() - dest.w);
        dest.y = std::clamp((int)dest.y, 0, Window::Height() - dest.h);


        // render the arrow
        SDL_RenderCopyEx(Window::renderer, arrow, NULL, &dest, angle + 90, NULL, SDL_FLIP_NONE);
    }

    //
    // RENDER HEALTHBAR
    //

    SDL_Rect hb_outline = {Window::Width() / 2 - HEALTHBAR_WIDTH / 2, Window::Height() - HEALTHBAR_HEIGHT * 2, HEALTHBAR_WIDTH, HEALTHBAR_HEIGHT};
    SDL_Rect hb_fill = {hb_outline.x + 3, hb_outline.y + 3, (HEALTHBAR_WIDTH - 6) * Game::player->getPosture() / 100, HEALTHBAR_HEIGHT - 6};

    SDL_Color tmp_c;
    SDL_GetRenderDrawColor(Window::renderer, &tmp_c.r, &tmp_c.g, &tmp_c.b, &tmp_c.a);
    SDL_SetRenderDrawColor(Window::renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(Window::renderer, &hb_outline);
    SDL_SetRenderDrawColor(Window::renderer, 255, 0, 0, 255);

    if(Game::player->getPosture() > 50) {
        SDL_SetRenderDrawColor(Window::renderer, 0, 255, 0, 255);
    }
    else if(Game::player->getPosture() > 25) {
        SDL_SetRenderDrawColor(Window::renderer, 255, 255, 0, 255);
    }
    SDL_RenderFillRect(Window::renderer, &hb_fill);
    
    SDL_SetRenderDrawColor(Window::renderer, tmp_c.r, tmp_c.g, tmp_c.b, tmp_c.a);


    //
    // RENDER SCORE
    //

    renderScore(Game::scores[0], Game::scores[1], Game::player->getTeamNumber());

}


void renderScore(int s1, int s2, int myTeam) {

    // render the score box and numbers
    static auto prevScore = std::make_pair(0, 0);
    
    // _______________
    // \             /
    //  \___________/ 

    const SDL_Vertex verticesLeft[] = {
        {rc::windowCenter.x - SCOREBOX_WIDTH / 2.0f - 30, 0.0f},
        {rc::windowCenter.x - SCOREBOX_WIDTH / 2.0f, SCOREBOX_HEIGHT},
        {rc::windowCenter.x + SCOREBOX_WIDTH / 2.0f + 30, 0.0f}
    };
    const SDL_Vertex verticesRight[] = {
        {rc::windowCenter.x + SCOREBOX_WIDTH / 2.0f + 30, 0.0f},
        {rc::windowCenter.x + SCOREBOX_WIDTH / 2.0f, SCOREBOX_HEIGHT},
        {rc::windowCenter.x - SCOREBOX_WIDTH / 2.0f, SCOREBOX_HEIGHT}
    };

    SDL_Color tmp_c;
    SDL_GetRenderDrawColor(Window::renderer, &tmp_c.r, &tmp_c.g, &tmp_c.b, &tmp_c.a);
    SDL_SetRenderDrawColor(Window::renderer, 0, 0, 0, 255);

    SDL_RenderGeometry(Window::renderer, nullptr, verticesLeft, 3, nullptr, 0);
    SDL_RenderGeometry(Window::renderer, nullptr, verticesRight, 3, nullptr, 0);

    SDL_SetRenderDrawColor(Window::renderer, tmp_c.r, tmp_c.g, tmp_c.b, tmp_c.a);

}