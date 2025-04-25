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
#include <memory>
#include "Map.h"
#include "Player.h"
#include "Enemy.h"
#include "Bomb.h"
#include "Items.h"
#include "EnemyTextureManager.h"
#include "Menu.h"
#include "LevelSelect.h"

const int TILE_SIZE = 32;
const int MAP_SIZE = 17;
const int HUD_HEIGHT = 32;
const int SCREEN_WIDTH = MAP_SIZE * TILE_SIZE;
const int SCREEN_HEIGHT = MAP_SIZE * TILE_SIZE;
const int MAX_LEVELS = 5;
const std::string RES_PATH = "D:/Project_1/x64/Debug/res/";

Mix_Music* backgroundMusic = nullptr;
Mix_Chunk* explosionSound = nullptr;
TTF_Font* font = nullptr;
SDL_Texture* gameOverBackground = nullptr;

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
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) { // khởi tạo hình ảnh và âm thanh
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    if (TTF_Init() == -1) { // khởi tạo thư viện font chữ
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) { // khởi tạo thư viện ảnh
        std::cerr << "SDL_image could not initialize! IMG_Error: " << IMG_GetError() << std::endl;
        return false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) { // khởi tạo thư viện âm thanh
        std::cerr << "SDL_mixer could not initialize! Mix_Error: " << Mix_GetError() << std::endl;
        return false;
    }
    // tạo cửa sổ game với kích thước xác định
    window = SDL_CreateWindow("Bomberman", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); // tạo renderer để vẽ lên cửa sổ
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    } 
    // load các thành phần liên quan
    backgroundMusic = Mix_LoadMUS((RES_PATH + "sounds/soundtrack.wav").c_str());

    explosionSound = Mix_LoadWAV((RES_PATH + "sounds/BOM_11_M.wav").c_str());

    font = TTF_OpenFont((RES_PATH + "fonts/Arial.ttf").c_str(), 20);

	gameOverBackground = loadTexture(renderer, RES_PATH + "sprites/gameover.png");

    if (!font) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }
    /*if (backgroundMusic) Mix_PlayMusic(backgroundMusic, -1);*/
    if (!gameOverBackground) {
		std::cerr << "Failed to load game over background! SDL_Error: " << IMG_GetError() << std::endl;
    }

    return true;
}
// dọn dẹp tài nguyên
void cleanup(SDL_Window* window, SDL_Renderer* renderer) {
    Mix_FreeMusic(backgroundMusic);
    Mix_FreeChunk(explosionSound);
    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
	SDL_DestroyTexture(gameOverBackground);
	EnemyTextureManager::cleanUp(); // Giải phóng tất cả các texture của enemy
    Mix_CloseAudio();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

}
const int FRAME_DELAY = 1000 / 60; // Giới hạn 60 FPS

// Biến toàn cục
std::vector<std::unique_ptr<Enemy>> enemies;
std::vector<std::unique_ptr<Bomb>> bombs;
bool running = true;
bool startGame = false;
bool isGameOver = false;
bool showGameOverScreen = false;
Menu* menu = nullptr;
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
int level = 1, timeLeft = 150;
Uint32 lastTime = SDL_GetTicks();
Uint32 gameOverStartTime = 0;
Map map;
Player player(&map);

// Hàm khởi tạo game
bool initGame() {
    if (!initSDL(window, renderer)) return false;

    menu = new Menu(renderer, RES_PATH);
    // vòng lặp xử lý menu
    if (!menu->show()) {
        running = false;
        return false;
    }
    else {
        LevelSelect levelselect(renderer, RES_PATH);
        int selectedLevel = 0; // khởi tạo biến lưu
        if (levelselect.show()) {
            selectedLevel = levelselect.getSelectedLevel(); // lấy cấp độ đã chọn
        }
        
        if (selectedLevel == 0) {
            running  = false;
            return false;
        }
        level = selectedLevel;
        startGame = true;
    }

    srand(static_cast<unsigned int>(time(0)));
    map.loadFromFile(RES_PATH + "levels/Level" + std::to_string(level) + ".txt");
    map.loadTextures(renderer);

    // Duyệt map để tìm Enemy
    enemies.clear();
    for (int row = 0; row < map.getHeight(); ++row) {
        for (int col = 0; col < map.getWidth(); ++col) {
            if (map.getTile(col, row) == ONEAL) {
                enemies.emplace_back(std::make_unique<Enemy>(col,row));  
				enemies.back()->init(renderer,"oneal");
                //std::cout << "Spawned ONEAL at (" << col << ", " << row << ")" << std::endl;
            }
            else if (map.getTile(col, row) == KONDORIA) {
                enemies.emplace_back(std::make_unique<Enemy>(col, row)); 
                enemies.back()->init(renderer,"kondoria");
                //std::cout << "Spawned KONDORIA at (" << col << ", " << row << ")" << std::endl;
            }
        }
    }
    //std::cout << "Total enemies: " << enemies.size() << std::endl;
    player.init(renderer);
    return true;
}

// Xử lý sự kiện
void handleEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) running = false;
    }
}

// Kiểm tra nếu người chơi đến cổng portal
void checkPortal() {
    if (player.reachedPortal(map, enemies)) {
        std::cout << "Portal reached! Moving to next level...\n";  // Log khi người chơi vào cổng portal
        level++;
        if (level > MAX_LEVELS) {
            std::cout << "🎉 Congratulations! You won! 🎉" << std::endl;
            running = false;
            return;
        }

        timeLeft = 150;
        map.loadFromFile(RES_PATH + "levels/Level" + std::to_string(level) + ".txt");

        // Xóa enemy cũ và tạo enemy mới theo map
        //enemies.clear();
        //for (int row = 0; row < map.getHeight(); ++row) {
        //    for (int col = 0; col < map.getWidth(); ++col) {
        //        if (map.getTile(col, row) == ONEAL) {
        //            enemies.emplace_back(std::make_unique<Enemy>(col, row));
        //            enemies.back()->init(renderer,"oneal");
        //            //std::cout << "Spawned ONEAL at (" << col << ", " << row << ")" << std::endl;
        //        }
        //        else if (map.getTile(col, row) == KONDORIA) {
        //            enemies.emplace_back(std::make_unique<Enemy>(col, row));
        //            enemies.back()->init(renderer,"kondoria");                   
        //            //std::cout << "Spawned KONDORIA at (" << col << ", " << row << ")" << std::endl;
        //        }
        //    }
        //}
        //std::cout << "Total enemies: " << enemies.size() << std::endl;
        player.resetPosition();
    }
}

// Hàm chạy game loop
void gameLoop() {
    
    while (running) {

        if (startGame) {
            if (backgroundMusic) Mix_PlayMusic(backgroundMusic, -1);

            Uint32 frameStart, frameTime;
            while (running && startGame) {
                frameStart = SDL_GetTicks();
                // Cập nhật thời gian đếm ngược
                Uint32 currentTime = SDL_GetTicks();
                if (currentTime - lastTime >= 1000) {
                    timeLeft = std::max(0, timeLeft - 1);
                    lastTime = currentTime;
                }

                handleEvents();  // Xử lý sự kiện

                // Xử lý input trước khi update
                const Uint8* keys = SDL_GetKeyboardState(NULL);
                player.calculateMove(keys, map);
                player.handleBombInput(keys, bombs, map, renderer, enemies);
                checkPortal();   // Kiểm tra portal

                // Cập nhật logic game
                player.update(map);
                if (!isGameOver) {
                    player.checkCollisionWithEnemies(enemies);
                    if (!player.isAlive()) {
                        isGameOver = true;
                        gameOverStartTime = SDL_GetTicks(); // ghi lại thời điểm chết của player
                    }
                }
                else {
                    // sau khi animation chết xong ( khoảng tầm 2 giây )
                    if (SDL_GetTicks() - gameOverStartTime >= 2000 && !showGameOverScreen) {
                        showGameOverScreen = true;
                    }
                }
                for (auto& bomb : bombs) {
                    if (bomb->isActive()) {
                        bomb->update();
                    }
                }
                bombs.erase(std::remove_if(bombs.begin(), bombs.end(),
                    []( const std::unique_ptr<Bomb>& b) { return !b->isActive() && b->isFinished(); }),
                    bombs.end());
                for (auto& enemy : enemies) {
                    enemy->update(map);
                }
                enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                    [](const std::unique_ptr<Enemy>& e) { return !e->isAlive(); }),
                    enemies.end());

                // Render game
                SDL_RenderClear(renderer);

                if (showGameOverScreen && gameOverBackground) {
                    SDL_RenderCopy(renderer, gameOverBackground, NULL, NULL);
                }
                else {
                    map.render(renderer, 0, 0);
                    player.render(renderer);
                    for (auto& bomb : bombs) {
                        if (bomb->isActive()) {
                            bomb->render(renderer);
                        }
                    }
                    for (auto& enemy : enemies) {
                        enemy->render(renderer);
                    }
                }

                SDL_RenderPresent(renderer);

                // Giới hạn FPS
                frameTime = SDL_GetTicks() - frameStart;
                if (frameTime < FRAME_DELAY) {
                    SDL_Delay(FRAME_DELAY - frameTime);
                }
            }
            
        }
    }
}

// Hàm chính
int main(int argc, char* argv[]) {
    if (!initGame()) return -1;

    gameLoop();  // Chạy game loop

    cleanup(window, renderer); // Dọn dẹp bộ nhớ khi thoát
    return 0;
}
