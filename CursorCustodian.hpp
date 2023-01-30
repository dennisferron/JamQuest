#pragma once

#include "SdlApplication.hpp"

#include "renderwindow.h"
#include "player.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

class CursorCustodian : public SdlApplication
{
private:
    const int ALIVE = 0;
    const int CURSOR_DEATH = 1;
    const int HOLE_DEATH = 2;

    std::vector<SDL_Texture*> playerTex;
    SDL_Texture* groundTex[4];
    SDL_Texture* arrow;
    SDL_Texture* highscoreBox;
    SDL_Texture* deathOverlay;
    SDL_Texture* logo;

    TTF_Font* font32;
    TTF_Font* font32_outline;
    TTF_Font* font24;
    TTF_Font* font16;

    SDL_Color white = { 255, 255, 255 };
    SDL_Color black = { 0, 0, 0 };

    Mix_Chunk* jumpSfx;
    Mix_Chunk* fallSfx;
    Mix_Chunk* hitSfx;
    Mix_Chunk* clickSfx;

    bool playedDeathSFX = false;
    bool mainMenu = true;

    Player* player;
    Ground* ground;

    void reset();

public:
    CursorCustodian();
    ~CursorCustodian();

    void gameLoop() override;
};
