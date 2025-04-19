#include "Menu.h"
#include <iostream>
#include <SDL_image.h>
const std::string resPath = "D:/Project_1/x64/Debug/res/";

Menu::Menu(SDL_Renderer* renderer,const std::string& resPath)
    : renderer(renderer), resPath(resPath),
    backgroundTexture(nullptr), startButtonTexture(nullptr),
    exitButtonTexture(nullptr), menuMusic(nullptr)
{
    loadAssets();
}

Menu::~Menu() {
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(startButtonTexture);
    SDL_DestroyTexture(exitButtonTexture);
    Mix_FreeMusic(menuMusic);
}

SDL_Texture* Menu::loadTexture(const std::string& path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (!texture) {
        std::cerr << "Failed to load texture: " << path << " | Error: " << IMG_GetError() << std::endl;
    }
    return texture;
}

bool Menu::loadAssets() {
    backgroundTexture = loadTexture(resPath + "sprites/menu_background.png");
    startButtonTexture = loadTexture(resPath + "sprites/start_button.png");
    exitButtonTexture = loadTexture(resPath + "sprites/exit_button.png");

    startButtonRect = { 60, 250, 180, 80 }; // điều chỉnh vị trí/kích thước phù hợp
    exitButtonRect = { 60, 350, 180, 80 };

    originalStartButtonRect = startButtonRect;
    originalExitButtonRect = exitButtonRect;

    menuMusic = Mix_LoadMUS((resPath + "sounds/menu_music.mp3").c_str());
    if (!menuMusic) {
        std::cerr << "Failed to load menu music! Mix_Error: " << Mix_GetError() << std::endl;
        return false;
    }
    Mix_PlayMusic(menuMusic, -1); // Phát lặp vô hạn

    return backgroundTexture && startButtonTexture && exitButtonTexture && menuMusic;
}

void Menu::render() {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
    SDL_RenderCopy(renderer, startButtonTexture, nullptr, &startButtonRect);
    SDL_RenderCopy(renderer, exitButtonTexture, nullptr, &exitButtonRect);
    SDL_RenderPresent(renderer);
}

bool Menu::handleEvents(bool& running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
            return false;
        }
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int x = e.button.x;
            int y = e.button.y;

            if (x >= startButtonRect.x && x <= startButtonRect.x + startButtonRect.w &&
                y >= startButtonRect.y && y <= startButtonRect.y + startButtonRect.h) {
                return true; // Start game
            }

            if (x >= exitButtonRect.x && x <= exitButtonRect.x + exitButtonRect.w &&
                y >= exitButtonRect.y && y <= exitButtonRect.y + exitButtonRect.h) {
                running = false;
                return false; // Exit game
            }
        }

        if (e.type == SDL_MOUSEMOTION) {
            int x = e.motion.x;
            int y = e.motion.y;

            if (x >= originalStartButtonRect.x && x <= originalStartButtonRect.x + originalStartButtonRect.w &&
                y >= originalStartButtonRect.y && y <= originalStartButtonRect.y + originalStartButtonRect.h) {
                startButtonRect = { originalStartButtonRect.x - 10, originalStartButtonRect.y - 20,
                originalStartButtonRect.w + 20, originalStartButtonRect.h + 20
                };
            }
            else {
                startButtonRect = originalStartButtonRect;
            }

            if (x >= originalExitButtonRect.x && x <= originalExitButtonRect.x + originalExitButtonRect.w &&
                y >= originalExitButtonRect.y && y <= originalExitButtonRect.y + originalExitButtonRect.h) {
                exitButtonRect = { originalExitButtonRect.x - 10, originalExitButtonRect.y - 10, 
                    originalExitButtonRect.w + 20, originalExitButtonRect.h + 20
                };
            }
            else {
                exitButtonRect = originalExitButtonRect;
            }
        }
    }
    return false;
}

bool Menu::show() {
    bool running = true;
    bool startGame = false;

    while (running && !startGame) {
        render();
        startGame = handleEvents(running);
        SDL_Delay(16); // ~60 FPS
    }

    Mix_HaltMusic(); // Dừng nhạc menu khi vào game
    return startGame;
}
