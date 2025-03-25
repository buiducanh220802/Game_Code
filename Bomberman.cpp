#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include "Map.h"
#include "Player.h"
#include "Enemy.h"
#include "Bomb.h"
#include "Items.h"

const int TILE_SIZE = 32;
const int MAP_SIZE = 17;
const int HUD_HEIGHT = 32;
const int SCREEN_WIDTH = MAP_SIZE * TILE_SIZE;
const int SCREEN_HEIGHT = (MAP_SIZE * TILE_SIZE) + HUD_HEIGHT;
const int MAX_LEVELS = 5;
const std::string RES_PATH = "D:/Project_1/x64/Debug/res/";

Mix_Music* backgroundMusic = nullptr;
Mix_Chunk* explosionSound = nullptr;
TTF_Font* font = nullptr;
// tải hình ảnh từ file vào texture
SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& path) {
    SDL_Texture* newTexture = IMG_LoadTexture(renderer, path.c_str());
    if (!newTexture) {
        std::cerr << "Failed to load texture: " << path << " | Error: " << IMG_GetError() << std::endl;
    }
    return newTexture;
}
// khởi tạo SDL và các thành phần khác
bool initSDL(SDL_Window*& window, SDL_Renderer*& renderer) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
        return false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! Mix_Error: " << Mix_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Bomberman", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    backgroundMusic = Mix_LoadMUS((RES_PATH + "sounds/soundtrack.wav").c_str());
    explosionSound = Mix_LoadWAV((RES_PATH + "sounds/BOM_11_M.wav").c_str());
    font = TTF_OpenFont((RES_PATH + "fonts/Arial.ttf").c_str(), 20);

    if (!font) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }
    if (backgroundMusic) Mix_PlayMusic(backgroundMusic, -1);
    return true;
}
// hàm vẽ text lên màn hình
void renderText(SDL_Renderer* renderer, const std::string& text, int x, int y) {
    SDL_Color white = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), white);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect destRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
// hàm vẽ HUD
void renderHUD(SDL_Renderer* renderer, int level, int timeLeft) {
    SDL_Rect hudRect = { 0, 0, SCREEN_WIDTH, HUD_HEIGHT };
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &hudRect);

    renderText(renderer, "Level: " + std::to_string(level), 20, 5);
    renderText(renderer, "Time: " + std::to_string(timeLeft / 60) + ":" + (timeLeft % 60 < 10 ? "0" : "") + std::to_string(timeLeft % 60), SCREEN_WIDTH - 120, 5);
}

void cleanup(SDL_Window* window, SDL_Renderer* renderer) {
    Mix_FreeMusic(backgroundMusic);
    Mix_FreeChunk(explosionSound);
    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    Mix_CloseAudio();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

std::vector<Enemy> enemies;
int main(int argc, char* argv[]) {
    // khởi tạo cửa sổ
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (!initSDL(window, renderer)) return -1;

    srand(static_cast<unsigned int>(time(0)));
    int level = 1, timeLeft = 150;

    // tải bản đồ xuống và phân tích file bản đồ
    Map map;
    map.loadFromFile(RES_PATH + "levels/Level" + std::to_string(level) + ".txt");
    map.loadTextures(renderer);
    // Duyệt map để tìm Enemy
    for (int row = 0; row < map.getHeight(); ++row) {
        for (int col = 0; col < map.getWidth(); ++col) {
            if (map.getTile(col, row) == ONEAL) {
                enemies.emplace_back(true); // Enemy thông minh
                enemies.back().init(renderer, col, row);
            }
            else if (map.getTile(col, row) == KONDORIA) {
                enemies.emplace_back(false); // Enemy ngẫu nhiên
                enemies.back().init(renderer, col, row);
            }
        }
    }
    
    Player player;
    player.init(renderer);
    Map gameMap;
    Bomb bomb(renderer, &gameMap, &player, enemies);

  
    SDL_Event e;
    bool running = true;
    Uint32 lastTime = SDL_GetTicks();

    while (running) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastTime >= 1000) {
            timeLeft = std::max(0, timeLeft - 1);
            lastTime = currentTime;
        }

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
        }

        if (player.reachedPortal(map,enemies)) {
            level++;
            if (level > MAX_LEVELS) {
                std::cout << "Congratulations! You won!" << std::endl;
                break;
            }
            timeLeft = 150;
            map.loadFromFile(RES_PATH + "levels/Level" + std::to_string(level) + ".txt");
            enemies.clear();
            for (int row = 0; row < map.getHeight(); ++row) {
                for (int col = 0; col < map.getWidth(); ++col) {
                    if (map.getTile(col, row) == ONEAL) {
                        enemies.emplace_back(true);
                        enemies.back().init(renderer, col, row);
                    }
                    else if (map.getTile(col, row) == KONDORIA) {
                        enemies.emplace_back(false);
                        enemies.back().init(renderer, col, row);
                    }
                }
            }
            player.resetPosition();
        }

        SDL_RenderClear(renderer);
        renderHUD(renderer, level, timeLeft);
        map.render(renderer, 0, HUD_HEIGHT);
        player.update(map);
        for (Enemy& enemy : enemies) {
            enemy.update(map, player);
        }
        player.render(renderer);
        for (Enemy& enemy : enemies) {
            enemy.render(renderer);
        }
        SDL_RenderPresent(renderer);
        SDL_GetTicks();
    }

    cleanup(window, renderer);
    return 0;
}
