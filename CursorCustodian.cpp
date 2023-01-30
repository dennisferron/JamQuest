#include "CursorCustodian.hpp"

#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>
#include <stdlib.h>

CursorCustodian::CursorCustodian()
{
    playerTex.push_back(render_window->loadTexture("res/textures/player/player_0.png"));
    playerTex.push_back(render_window->loadTexture("res/textures/player/player_1.png"));
    playerTex.push_back(render_window->loadTexture("res/textures/player/player_2.png"));
    playerTex.push_back(render_window->loadTexture("res/textures/player/player_3.png"));
    playerTex.push_back(render_window->loadTexture("res/textures/player/player_4.png"));

    player = new Player(0, 0, playerTex);

    groundTex[0] = render_window->loadTexture("res/textures/ground/left.png");
    groundTex[1] = render_window->loadTexture("res/textures/ground/center.png");
    groundTex[2] = render_window->loadTexture("res/textures/ground/right.png");
    groundTex[3] = render_window->loadTexture("res/textures/ground/hole.png");

    ground = new Ground(groundTex[0], groundTex[1], groundTex[2], groundTex[3]);

    arrow = render_window->loadTexture("res/textures/arrow.png");
    highscoreBox = render_window->loadTexture("res/textures/highscore_box.png");
    deathOverlay = render_window->loadTexture("res/textures/death_overlay.png");
    logo = render_window->loadTexture("res/textures/logo.png");

    font32 = TTF_OpenFont("res/fonts/cocogoose.ttf", 32);
    font32_outline = TTF_OpenFont("res/fonts/cocogoose.ttf", 32);
    font24 = TTF_OpenFont("res/fonts/cocogoose.ttf", 24);
    font16 = TTF_OpenFont("res/fonts/cocogoose.ttf", 16);
    TTF_SetFontOutline(font32_outline, 3);

    jumpSfx = Mix_LoadWAV("res/sounds/jump.wav");
    fallSfx = Mix_LoadWAV("res/sounds/fall.wav");
    hitSfx = Mix_LoadWAV("res/sounds/hit.wav");
    clickSfx = Mix_LoadWAV("res/sounds/click.wav");
    Mix_PlayChannel(-1, jumpSfx, 0);
}

CursorCustodian::~CursorCustodian()
{
    delete player;
    delete ground;

    TTF_CloseFont(font32);
    TTF_CloseFont(font32_outline);
    TTF_CloseFont(font24);
    TTF_CloseFont(font16);
}

void CursorCustodian::reset()
{
    player->reset();
    ground->reset();
}
void CursorCustodian::gameLoop()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            case SDL_QUIT:
            {
                gameRunning = false;
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                if (mainMenu)
                {
                    if (event.button.button == SDL_BUTTON_LEFT && SDL_GetTicks() > 2500)
                    {
                        mainMenu = false;
                        Mix_PlayChannel(-1, clickSfx, 0);
                    }
                }
                else
                {
                    if (event.button.button == SDL_BUTTON_LEFT && player->isDead() == ALIVE)
                    {
                        if (player->jump())
                        {
                            Mix_PlayChannel(-1, jumpSfx, 0);
                        }
                    }
                    else if (player->isDead() != ALIVE)
                    {
                        Mix_PlayChannel(-1, clickSfx, 0);
                        reset();
                        playedDeathSFX = false;
                    }
                }
                break;
            }
        }
    }
    if (mainMenu)
    {
        if (SDL_GetTicks() < 2500)
        {
            render_window->clear();
            render_window->renderCenter(0, sin(SDL_GetTicks() / 100) * 2 - 4, "POLYMARS", font24, white);
            render_window->display();
        }
        else
        {
            render_window->clear();

            render_window->render(SCREEN_WIDTH / 2 - 234, SCREEN_HEIGHT / 2 - 94 - 30, logo);
            render_window->renderCenter(0, 90 + sin(SDL_GetTicks() / 100) * 2, "Click to start", font24, white);

            for (int i = 0; i < ground->getLength(); i++)
            {
                render_window->render(ground->getTile(i));
            }
            render_window->display();
        }
    }
    else
    {
        if (player->isDead() != CURSOR_DEATH)
        {
            ground->update(player->getScoreInt());
        }

        if (player->isDead() == ALIVE)
        {
            player->update(*ground);
        }
        else if (!playedDeathSFX) {
            if (player->isDead() == CURSOR_DEATH)
            {
                Mix_PlayChannel(-1, hitSfx, 0);
            }
            else if (player->isDead() == HOLE_DEATH)
            {
                Mix_PlayChannel(-1, fallSfx, 0);
            }
            playedDeathSFX = true;
        }

        render_window->clear();

        render_window->render(*player);
        for (int i = 0; i < ground->getLength(); i++)
        {
            render_window->render(ground->getTile(i));
        }
        render_window->render(25, 30, arrow);
        render_window->render(62, 20, player->getScore(), font32_outline, black);
        render_window->render(65, 23, player->getScore(), font32, white);
        render_window->render(0, 65, highscoreBox);
        render_window->render(65, 64, player->getHighscore(), font16, white);

        if (player->isDead() != ALIVE)
        {
            render_window->render(deathOverlay);
            if (player->isDead() == CURSOR_DEATH)
            {
                render_window->renderCenter(0, -24, "The cursor is deadly to the player->..", font24, white);
            }
            else if (player->isDead() == HOLE_DEATH)
            {
                render_window->renderCenter(0, -24, "The hole had no bottom...", font24, white);
            }
            render_window->renderCenter(0, 12, "Click to retry.", font16, white);
        }
        render_window->display();
    }
}
