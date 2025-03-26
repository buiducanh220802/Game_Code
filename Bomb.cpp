#include "Bomb.h"
#include <iostream>
#include <string>
// constructor 
Bomb::Bomb(SDL_Renderer* renderer, Map* gameMap, Player* player, std::vector<Enemy>& enemies) : map(gameMap), player(player), enemies(enemies) {
    bombTexture = IMG_LoadTexture(renderer, "D:/Project_1/x64/Debug/res/sprites/bomb.png"); // load Texxture
    explosionSound = Mix_LoadWAV("D:/Project_1/x64/Debug/res/sounds/BOM_11_M.wav");
    // tải texture cho bom 
	if (!bombTexture) {
		std::cerr << "Failed to load bomb texture\n"; // nếu có lỗi thông báo lại ra output để dễ debug
	}
	// tải các frame cho animation của bom nổ
    for (int i = 0; i < 3; i++) {
        std::string index = std::to_string(i + 1);
        explosionCenter.addFrame(CENTER, IMG_LoadTexture(renderer, ("D:/Project_1/x64/Debug/res/sprites/bomb_exploded" + index + ".png").c_str()));
        explosionUp.addFrame(UP, IMG_LoadTexture(renderer, ("D:/Project_1/x64/Debug/res/sprites/explosion_vertical_top_last" + index + ".png").c_str()));
        explosionDown.addFrame(DOWN, IMG_LoadTexture(renderer, ("D:/Project_1/x64/Debug/res/sprites/explosion_vertical_down_last" + index + ".png").c_str()));
        explosionLeft.addFrame(LEFT, IMG_LoadTexture(renderer, ("D:/Project_1/x64/Debug/res/sprites/explosion_horizontal_left_last" + index + ".png").c_str()));
        explosionRight.addFrame(RIGHT, IMG_LoadTexture(renderer, ("D:/Project_1/x64/Debug/res/sprites/explosion_horizontal_right_last" + index + ".png").c_str()));
    } // các frame thể hiện animation cho bomb nổ
	SDL_Texture* tex = IMG_LoadTexture(renderer, "D:/Project_1/x64/Debug/res/sprites/bomb_exploded1.png"); // thử log xem có nổ không để dễ debug
	if (!tex) {
		std::cerr << "Failed to load explosion texture\n";
	}
    exploded = false;
}

Bomb::~Bomb() { // hủy các texture khi hủy đối tượng descontructor khi sử dụng class
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
    // Xác định phạm vi nổ theo 4 hướng
    std::vector<std::pair<int, int>> explosionTiles = {
        {gridX, gridY},     // Ô đặt bom
        {gridX - 1, gridY}, // Trái
        {gridX + 1, gridY}, // Phải
        {gridX, gridY - 1}, // Trên
        {gridX, gridY + 1}  // Dưới
    };

    // Kiểm tra va chạm với Enemy
    for (Enemy& enemy : enemies) {
        int enemyGridX = enemy.getX() / 32;
        int enemyGridY = enemy.getY() / 32;

        for (auto& tile : explosionTiles) {
            if (enemyGridX == tile.first && enemyGridY == tile.second) {
                enemy.die();
                break; // Không cần kiểm tra tiếp nếu đã chết
            }
        }
    }

    // Kiểm tra va chạm với Player
    int playerGridX = player->getX() / 32;
    int playerGridY = player->getY() / 32;

    for (auto& tile : explosionTiles) {
        if (playerGridX == tile.first && playerGridY == tile.second) {
            player->die();
            break;
        }
    }
}


// kích hoạt vụ nổ của bom
void Bomb::explode() {
    exploded = true;
    Mix_PlayChannel(-1, explosionSound, 0);
    std::cout << "Bomb exploded at (" << gridX << ", " << gridY << ")\n";
}

