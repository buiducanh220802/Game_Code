#include "Bomb.h"
#include <iostream>
#include <SDL_image.h>
#include <string>

const int TILE_SIZE = 32;
// constructor demo
Bomb::Bomb(SDL_Renderer* renderer, Map* map, Player* player, std::vector<std::unique_ptr<Enemy>>& enemies)
    : map(map), player(player), enemies(enemies), exploded(false), active(false), timer(0) {

    if (!renderer) {
        std::cerr << "Invalid renderer passed to Bomb constructor" << std::endl;
        return;
    }

    if (!loadTextures(renderer)) {
        std::cerr << "Failed to load textures for Bomb!" << std::endl;
    }

    placeBombSound = Mix_LoadWAV("D:/Project_1/x64/Debug/res/sounds/BOM_SET.wav");
    if (!placeBombSound) {
        std::cerr << "Failed to load place bomb sound: " << Mix_GetError() << std::endl;
    }

    explosionSound = Mix_LoadWAV("C:/res/sounds/BOM_11_M.wav");
    if (!explosionSound) {
        std::cerr << "Failed to load explosion sound: " << Mix_GetError() << std::endl;
    }
}

// Destructor
Bomb::~Bomb() {
    cleanUp();
}

// Tải tất cả texture cho bom và vụ nổ
bool Bomb::loadTextures(SDL_Renderer* renderer) {
    bombTexture = IMG_LoadTexture(renderer, "D:/Project_1/x64/Debug/res/sprites/bomb.png");
    if (!bombTexture) {
        std::cerr << "Failed to load bomb texture: " << IMG_GetError() << std::endl;
        return false;
    }

    // Load các texture cho hiệu ứng nổ
    for (int i = 1; i <= 3; ++i) {
        std::string index = std::to_string(i);
        SDL_Texture* centerTexture = IMG_LoadTexture(renderer, ("D:/Project_1/x64/Debug/res/sprites/bomb_exploded" + index + ".png").c_str());
        SDL_Texture* upTexture = IMG_LoadTexture(renderer, ("D:/Project_1/x64/Debug/res/sprites/explosion_vertical_top_last" + index + ".png").c_str());
        SDL_Texture* downTexture = IMG_LoadTexture(renderer, ("D:/Project_1/x64/Debug/res/sprites/explosion_vertical_down_last" + index + ".png").c_str());
        SDL_Texture* leftTexture = IMG_LoadTexture(renderer, ("D:/Project_1/x64/Debug/res/sprites/explosion_horizontal_left_last" + index + ".png").c_str());
        SDL_Texture* rightTexture = IMG_LoadTexture(renderer, ("D:/Project_1/x64/Debug/res/sprites/explosion_horizontal_right_last" + index + ".png").c_str());

        if (!centerTexture || !upTexture || !downTexture || !leftTexture || !rightTexture) {
            std::cerr << "Failed to load explosion texture: " << IMG_GetError() << std::endl;
            continue;
        }
		if (!centerTexture) std::cerr << "Failed to load center texture: " << IMG_GetError() << std::endl;
		if (!upTexture) std::cerr << "Failed to load up texture: " << IMG_GetError() << std::endl;
		if (!downTexture) std::cerr << "Failed to load down texture: " << IMG_GetError() << std::endl;
		if (!leftTexture) std::cerr << "Failed to load left texture: " << IMG_GetError() << std::endl;
		if (!rightTexture) std::cerr << "Failed to load right texture: " << IMG_GetError() << std::endl;
		// Thêm các texture vào Animation
        explosionCenter.addFrame(CENTER, centerTexture);
        explosionUp.addFrame(UP, upTexture);
        explosionDown.addFrame(DOWN, downTexture);
        explosionLeft.addFrame(LEFT, leftTexture);
        explosionRight.addFrame(RIGHT, rightTexture);
    }

    return true;
}

// Dọn dẹp tài nguyên
void Bomb::cleanUp() {
    if (bombTexture) {
        SDL_DestroyTexture(bombTexture);
        bombTexture = nullptr;
    }

	if (placeBombSound) {
		placeBombSound = nullptr;
	}
    if (explosionSound) {
        explosionSound = nullptr;
    }
}


// Vẽ bom và hiệu ứng nổ
void Bomb::render(SDL_Renderer* renderer) {
    if (!renderer) {
        std::cerr << "Error: Renderer is NULL" << std::endl;
        return;
    }
    if (!active) {
		/*std::cerr << "Error: Bomb is not active" << std::endl;*/
		return;
    }

    SDL_Rect bombRect = { x, y, 32, 32 };

    if (!exploded) {
        if (bombTexture) {
            SDL_RenderCopy(renderer, bombTexture, nullptr, &bombRect);
        }
        else {
			std::cerr << "Error: Bomb texture is NULL" << std::endl;
        }
    }
    else {
		explosionCenter.setDirection(CENTER);
        explosionCenter.render(renderer, x, y);
        renderExplosion(renderer);
    }
}

// Vẽ các hướng của vụ nổ
void Bomb::renderExplosion(SDL_Renderer* renderer) {
	if (!renderer) {
		std::cerr << "Error: Renderer is NULL (không vẽ được vụ nổ)" << std::endl;
		return;
	}
    if (!map->isWall(gridX, gridY - 1)) {
        //std::cout << "Render UP\n";
		explosionUp.setDirection(UP);
        explosionUp.render(renderer, x, y - TILE_SIZE);
    }
    if (!map->isWall(gridX, gridY + 1)) {
		//std::cout << "Render DOWN\n";
		explosionDown.setDirection(DOWN);
        explosionDown.render(renderer, x, y + TILE_SIZE);
    }
    if (!map->isWall(gridX - 1, gridY)) {
		//std::cout << "Render LEFT\n";
		explosionLeft.setDirection(LEFT);
        explosionLeft.render(renderer, x - TILE_SIZE, y);
    }
    if (!map->isWall(gridX + 1, gridY)) {
		//std::cout << "Render RIGHT\n";
		explosionRight.setDirection(RIGHT);
        explosionRight.render(renderer, x + TILE_SIZE, y);
    }
}

// Đặt bom tại vị trí người chơi
void Bomb::place(int px, int py) {
    gridX = px;
    gridY = py;
    x = gridX * TILE_SIZE;
    y = gridY * TILE_SIZE;
    timer = 120; // bom phát nổ sau 120 frame
    active = true;
    exploded = false;
    if (placeBombSound) {
        Mix_PlayChannel(-1, placeBombSound, 0);
    }
    else {
		std::cerr << "Failed to play place bomb sound: " << Mix_GetError() << std::endl;
    }
}

// Cập nhật trạng thái bom
void Bomb::update() {
    if (!active) return;

    if (!exploded) {
        timer--;
        if (timer == 0) {
            explode();
            timer = -1;
        }
    }
    else {
        timer--; // tiếp tục giảm thử khi đang nổ
        explosionCenter.update();
        explosionUp.update();
        explosionDown.update();
        explosionLeft.update();
        explosionRight.update();

        if (timer <= -60) {  // Xóa hiệu ứng sau 60 frame (khoảng 1 giây)
            //std::cout << "Resetting bomb explosion state..." << std::endl;
            if (player) {
                player->increaseBombCount();
            }
            resetExplosion();
        }
    }
}

// Xử lý va chạm nổ bom với Enemy và Player
void Bomb::checkExplosionHit() {
    std::vector<std::pair<int, int>> explosionTiles = {
        {gridX, gridY}, {gridX - 1, gridY}, {gridX + 1, gridY},
        {gridX, gridY - 1}, {gridX, gridY + 1}
    };

    for (auto& enemy : enemies) {
        int enemyGridX = enemy->getX() / 32;
        int enemyGridY = enemy->getY() / 32;

        for (auto& tile : explosionTiles) {
            if (enemyGridX == tile.first && enemyGridY == tile.second) {
                enemy->die();
                break;
            }
        }
    }
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(), [](const std::unique_ptr<Enemy>& e) {
            return !e->isAlive(); // hoặc e.dead == true nếu bạn có biến trạng thái
            }),
        enemies.end()
    );
    
    // kiểm tra va chạm với player
    int playerGridX = player->getX() / 32;
    int playerGridY = player->getY() / 32;

    for (auto& tile : explosionTiles) {
        if (playerGridX == tile.first && playerGridY == tile.second) {
            if (player->getImmortal() == 0)
            player->die();
            break;
        }
    }
}

// Kích hoạt vụ nổ
void Bomb::explode() {
    exploded = true;
    Mix_PlayChannel(-1, explosionSound, 0);

    std::vector<std::pair<int, int>> explosionTiles = {
        {gridX, gridY}, {gridX - 1, gridY}, {gridX + 1, gridY},
        {gridX, gridY - 1}, {gridX, gridY + 1}
    };

    for (auto& tile : explosionTiles) {
        if (map->isBrick(tile.first, tile.second)) {
            map->destroyTile(tile.first, tile.second);
        }
    }
    checkExplosionHit();
}

bool Bomb::isFinished() const {
    return !active;
}

void Bomb::resetExplosion() {
    // Khôi phục lại trạng thái ban đầu
    active = false;
    exploded = false;
    timer = 0;

    explosionCenter.reset();
    explosionUp.reset();
    explosionDown.reset();
    explosionLeft.reset();
    explosionRight.reset();
}