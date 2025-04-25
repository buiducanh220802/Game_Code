#include "LevelSelect.h"
#include <iostream>
#include <SDL_image.h>

LevelSelect::LevelSelect(SDL_Renderer* renderer, const std::string& resPath)
    : renderer(renderer), resPath(resPath), backgroundTexture(nullptr), bgMusic(nullptr), selectedLevel(0) {
    loadAssets();
}

LevelSelect::~LevelSelect() {
    SDL_DestroyTexture(backgroundTexture);
    for (int i = 0; i < 5; ++i) {
        SDL_DestroyTexture(levelTextures[i]);
    }
    Mix_HaltMusic();
    Mix_FreeMusic(bgMusic);
}

SDL_Texture* LevelSelect::loadTexture(const std::string& path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (!texture) {
        std::cerr << "Failed to load texture: " << path << " | Error: " << IMG_GetError() << std::endl;
    }
    return texture;
}

bool LevelSelect::loadAssets() {
    backgroundTexture = loadTexture(resPath + "sprites/level_select_bg.png");

    for (int i = 0; i < 5; ++i) {
        levelTextures[i] = loadTexture(resPath + "sprites/level_" + std::to_string(i + 1) + ".png");
    }

    bgMusic = Mix_LoadMUS((resPath + "sounds/menu_music.mp3").c_str());
    if (!bgMusic) {
        std::cerr << "Failed to load background music: " << Mix_GetError() << std::endl;
        return false;
    }
    Mix_PlayMusic(bgMusic, -1); // Play music looped

    // Set button positions for each level
    int buttonWidth = 80; // chiều rộng của mỗi nút
    int buttonHeight = 60; // chiều cao của mỗi nút
    int spacing = 20;
    int startX = 20;
    int startY = 200;

    for (int i = 0; i < 5; ++i) {
        levelRects[i] = { startX + (buttonWidth + spacing) * i, startY, buttonWidth, buttonHeight };
    }

    return backgroundTexture && bgMusic;
}

void LevelSelect::render() {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

    for (int i = 0; i < 5; ++i) {
        SDL_RenderCopy(renderer, levelTextures[i], nullptr, &levelRects[i]);
    }

    SDL_RenderPresent(renderer);
}

bool LevelSelect::handleEvents(bool& running, int& selectedLevel) {
    SDL_Event e;
    SDL_Point mousePoint;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
            selectedLevel = 0;
        }
        else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            mousePoint.x = e.button.x;
            mousePoint.y = e.button.y;
            for (int i = 0; i < 5; ++i) {
                if (SDL_PointInRect(&mousePoint, &levelRects[i])) {
                    selectedLevel = i + 1;
                    return true;
                }
            }
        }
    }
    return false;
}

bool LevelSelect::show() {
    bool running = true;

    while (running) {
        render();
        if (handleEvents(running, selectedLevel) && selectedLevel > 0) {
            break;
        }
        SDL_Delay(16); // ~60 FPS
    }

    Mix_HaltMusic(); // Stop the background music when level is selected
    return selectedLevel > 0;
}
int LevelSelect::getSelectedLevel() const {
    return selectedLevel; // Trả về cấp độ đã chọn
}