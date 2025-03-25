﻿#include "Bomb.h"
#include <iostream>
#include <string>

Bomb::Bomb(SDL_Renderer* renderer, Map* gameMap, Player* player, std::vector<Enemy>& enemies) : map(gameMap), player(player), enemies(enemies) {
    bombTexture = IMG_LoadTexture(renderer, "D:/Project_1/x64/Debug/res/sprites/bomb.png");
    explosionSound = Mix_LoadWAV("D:/Project_1/x64/Debug/res/sounds/BOM_11_M.wav");
    // tải texture cho bom 
	if (!bombTexture) {
		std::cerr << "Failed to load bomb texture\n";
	}
	// tải các frame cho animation của bom nổ
    for (int i = 0; i < 3; i++) {
        std::string index = std::to_string(i + 1);
        explosionCenter.addFrame(CENTER, IMG_LoadTexture(renderer, ("D:/Project_1/x64/Debug/res/sprites/bomb_exploded" + index + ".png").c_str()));
        explosionUp.addFrame(UP, IMG_LoadTexture(renderer, ("D:/Project_1/x64/Debug/res/sprites/explosion_vertical_top_last" + index + ".png").c_str()));
        explosionDown.addFrame(DOWN, IMG_LoadTexture(renderer, ("D:/Project_1/x64/Debug/res/sprites/explosion_vertical_down_last" + index + ".png").c_str()));
        explosionLeft.addFrame(LEFT, IMG_LoadTexture(renderer, ("D:/Project_1/x64/Debug/res/sprites/explosion_horizontal_left_last" + index + ".png").c_str()));
        explosionRight.addFrame(RIGHT, IMG_LoadTexture(renderer, ("D:/Project_1/x64/Debug/res/sprites/explosion_horizontal_right_last" + index + ".png").c_str()));
    }
	SDL_Texture* tex = IMG_LoadTexture(renderer, "D:/Project_1/x64/Debug/res/sprites/bomb_exploded1.png");
	if (!tex) {
		std::cerr << "Failed to load explosion texture\n";
	}
    exploded = false;
}

Bomb::~Bomb() { // hủy các texture khi hủy đối tượng
    SDL_DestroyTexture(bombTexture);
	explosionCenter.~Animation();
	explosionDown.~Animation();
	explosionLeft.~Animation();
	explosionRight.~Animation();
	explosionUp.~Animation();
    Mix_FreeChunk(explosionSound);
    // tránh rò rì bộ nhớ
}
// hàm đặt boom
void Bomb::place(int px, int py) {
    gridX = px;// lưu tọa độ trên bản đồ
    gridY = py;
    x = gridX * 32; // chuyển từ tọa độ lưới sang pixel
    y = gridY * 32;
	timer = 120; // bom phát nổ sau 120frame
    active = true;
    exploded = false;
}
// cập nhật trạng thái của boom
void Bomb::update() {
    if (!active) return;

    if (!exploded) {
        timer--;
        if (timer == 0) { // nếu chưa nổ giảm dần thời gian để boom nổ
            explode();
            // thêm log để debug xem sao
            std::cout << "Bomb exploded at (" << gridX << ", " << gridY << ")\n";
        }
    }
    else { // nếu đã nổ cập nhật ngay animation của vụ nổ
        explosionCenter.update();
        explosionUp.update();
        explosionDown.update();
        explosionLeft.update();
        explosionRight.update();
    }

}
// vẽ boom và hiệu ứng nổ 
void Bomb::render(SDL_Renderer* renderer) {
    if (!active) return;

    SDL_Rect bombRect = { x, y, 32, 32 };

	if (!exploded) { // bom chưa nổ thì vẽ texture của bom
        SDL_RenderCopy(renderer, bombTexture, nullptr, &bombRect);
    }
	else { // đã nổ thì thể hiện vụ nổ theo các frame
        explosionCenter.render(renderer, x, y);
        if (!map->isWall(gridX, gridY - 1)) explosionUp.render(renderer, x, y - 32);
        if (!map->isWall(gridX, gridY + 1)) explosionDown.render(renderer, x, y + 32);
        if (!map->isWall(gridX - 1, gridY)) explosionLeft.render(renderer, x - 32, y);
        if (!map->isWall(gridX + 1, gridY)) explosionRight.render(renderer, x + 32, y);
    }
}
// bom ảnh hướng đến enemy và người chơi
void Bomb::checkExplosionHit() {
    for (Enemy& enemy : enemies) {  // Duyệt qua vector bằng tham chiếu
        if (enemy.getX() / 32 == gridX && enemy.getY() / 32 == gridY) {
            enemy.die();
        }
    }

    if (player->getX() / 32 == gridX && player->getY() / 32 == gridY) {
        player->die();
    }
}

// kích hoạt vụ nổ của bom
void Bomb::explode() {
    exploded = true;
    Mix_PlayChannel(-1, explosionSound, 0);
    std::cout << "Bomb exploded at (" << gridX << ", " << gridY << ")\n";
}

