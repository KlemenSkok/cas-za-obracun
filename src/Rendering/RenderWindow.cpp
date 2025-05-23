
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
        constexpr int tileSize = 150;
        static SDL_Texture* floor_texture = AssetManager::GetTexture(TEXTURE_FLOOR);
        int tex_w, tex_h;
        SDL_QueryTexture(floor_texture, NULL, NULL, &tex_w, &tex_h);

        for (auto& rect : MapData::floorTiles) {
            int screen_x = static_cast<int>(rect.x - rc::localPlayerPos.x + rc::windowCenter.x);
            int screen_y = static_cast<int>(rect.y - rc::localPlayerPos.y + rc::windowCenter.y);

            SDL_Rect dest;
            dest.x = screen_x;
            dest.y = screen_y;
            dest.w = rect.w;
            dest.h = rect.h;

            if (rect.w == tileSize && rect.h == tileSize) {
                // full tile
                SDL_RenderCopy(Window::renderer, floor_texture, NULL, &dest);
            } else {
                // partial tile
                SDL_Rect src;
                src.x = 0;
                src.y = 0;
                src.w = tex_w * rect.w / tileSize;
                src.h = tex_h * rect.h / tileSize;

                SDL_RenderCopy(Window::renderer, floor_texture, &src, &dest);
            }
        }

        // floor borders
        static SDL_Rect floor_borders[4] = {
            {415, 515, 36, 335},
            {1085, 515, 36, 335},
            {1935, 515, 36, 335},
            {2605, 515, 36, 335},
        };
        static SDL_Texture* floor_border_texture = AssetManager::GetTexture(TEXTURE_FLOOR_BORDER);

        for (auto& rect : floor_borders) {
            int screen_x = static_cast<int>(rect.x - rc::localPlayerPos.x + rc::windowCenter.x);
            int screen_y = static_cast<int>(rect.y - rc::localPlayerPos.y + rc::windowCenter.y);

            SDL_Rect dest;
            dest.x = screen_x;
            dest.y = screen_y;
            dest.w = rect.w;
            dest.h = rect.h;

            SDL_RenderCopy(Window::renderer, floor_border_texture, NULL, &dest);
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

    static SDL_Texture* tex_roundWon = AssetManager::GetTexture(TEXTURE_ROUND_WON);
    static SDL_Texture* tex_roundLost = AssetManager::GetTexture(TEXTURE_ROUND_LOST);
    static SDL_Texture* tex_WaitingPlayers = AssetManager::GetTexture(TEXTURE_WAITING_FOR_PLAYERS);

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


    //
    // RENDER STATUS MESSAGES
    //
    if(Game::current_state == GameState::ROUND_ENDING) {
        if(Game::last_winner == Game::player->getTeamNumber()) {
            SDL_Rect dest = {Window::Width() / 2 - 200, Window::Height() / 2 - 60, 400, 120};
            SDL_RenderCopy(Window::renderer, tex_roundWon, NULL, &dest);
        }
        else {
            SDL_Rect dest = {Window::Width() / 2 - 200, Window::Height() / 2 - 60, 400, 120};
            SDL_RenderCopy(Window::renderer, tex_roundLost, NULL, &dest);
        }
    }
    else if(Game::current_state == GameState::WAITING_NEXT_ROUND) {
        int secondsLeft = int(GameState::WAITING_NEXT_ROUND) - std::round((SDL_GetTicks() - Game::last_state_change) / 1000.0f);

        SDL_Rect dest = {Window::Width() / 2 - 35, SCOREBOX_HEIGHT + 10, 70, 120};
        SDL_Color white = {255, 255, 255, 255};
        SDL_Texture* timerTexture = Fonts::createDigitTexture(Window::renderer, Fonts::primaryFont, secondsLeft, white);
        SDL_RenderCopy(Window::renderer, timerTexture, NULL, &dest);
        SDL_DestroyTexture(timerTexture);
    }
    else if(Game::current_state == GameState::WAITING_FOR_PLAYERS) {
        SDL_Rect dest = {Window::Width() / 2 - 200, Window::Height() / 2 - 60, 0, 50};
        SDL_Rect x;
        SDL_QueryTexture(tex_WaitingPlayers, NULL, NULL, &x.w, &x.h);
        dest.w = x.w * dest.h / x.h;
        // middle of the screen
        dest.x = rc::windowCenter.x - dest.w / 2;
        dest.y = (rc::windowCenter.y - dest.h / 2) / 2;
        
        
        SDL_RenderCopy(Window::renderer, tex_WaitingPlayers, NULL, &dest);
    }

}

void renderScore(int s1, int s2, int myTeam) {

    // render the score box and numbers
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

    // render score numbers
    SDL_Color red = {255, 0, 0, 255};
    SDL_Color blue = {0, 0, 255, 255};
    SDL_Color white = {255, 255, 255, 255};

    SDL_Color leftColor = (myTeam == 1) ? blue : red;
    SDL_Color rightColor = (myTeam == 1) ? red : blue;

    SDL_Texture* leftTexture = Fonts::createDigitTexture(Window::renderer, Fonts::primaryFont, (myTeam == 1) ? s1 : s2, leftColor);
    SDL_Texture* rightTexture = Fonts::createDigitTexture(Window::renderer, Fonts::primaryFont, (myTeam == 1) ? s2 : s1, rightColor);
    SDL_Texture* middleSeparator = Fonts::createCharTexture(Window::renderer, Fonts::primaryFont, ':', white);


    SDL_Rect middleRect = {rc::windowCenter.x - 15, SCOREBOX_HEIGHT / 2 - 25, 30, 50};
    SDL_Rect leftRect = {middleRect.x - 30, middleRect.y, 30, 50};
    SDL_Rect rightRect = {middleRect.x + middleRect.w, middleRect.y, 30, 50};

    SDL_RenderCopy(Window::renderer, leftTexture, NULL, &leftRect);
    SDL_RenderCopy(Window::renderer, middleSeparator, NULL, &middleRect);
    SDL_RenderCopy(Window::renderer, rightTexture, NULL, &rightRect);
    
    SDL_DestroyTexture(leftTexture);
    SDL_DestroyTexture(rightTexture);
    SDL_DestroyTexture(middleSeparator);
    
}