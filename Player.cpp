#include "Player.h"
#include <iostream>
#include <SDL_image.h>

Player::Player(Map* map) {
    startX = 1; // xác định vị trí bắt đầu của người chơi trên bản đồ
    startY = 1;
    if (map->isWall(startX, startY)) {
        std::cerr << "Warning: Player spawn on WALL, moving to safe position!" << std::endl;
        startX = 1; startY = 1; // hoặc tìm vị trí trống khác
    }

    x = startX;
    y = startY;
    posX = static_cast<float>(x * 32); // chuyển từ tọa độ ô sang tọa độ pixel
    posY = static_cast<float>(y * 32 + 32);
    speed = 2.0f; // tốc độ di chuyển
    direction = DOWN; // điều hướng ban đầu
    moving = false;
    isDead = false;
    bombCount = 1;
    flameRange = 1;
    playerTexture = nullptr; // tránh con trỏ trỏ đến vùng nhớ không hợp lệ
}

// load hình ảnh cho nhân vật cùng các hiệu ứng animation
void Player::init(SDL_Renderer* renderer) {
    std::string spritePath = "D:/Project_1/x64/Debug/res/sprites/";

    walkAnimation.addFrame(DOWN, IMG_LoadTexture(renderer, (spritePath + "player_down.png").c_str()));
    walkAnimation.addFrame(DOWN, IMG_LoadTexture(renderer, (spritePath + "player_down_1.png").c_str()));
    walkAnimation.addFrame(DOWN, IMG_LoadTexture(renderer, (spritePath + "player_down_2.png").c_str()));

    walkAnimation.addFrame(UP, IMG_LoadTexture(renderer, (spritePath + "player_up.png").c_str()));
    walkAnimation.addFrame(UP, IMG_LoadTexture(renderer, (spritePath + "player_up_1.png").c_str()));
    walkAnimation.addFrame(UP, IMG_LoadTexture(renderer, (spritePath + "player_up_2.png").c_str()));

    walkAnimation.addFrame(LEFT, IMG_LoadTexture(renderer, (spritePath + "player_left.png").c_str()));
    walkAnimation.addFrame(LEFT, IMG_LoadTexture(renderer, (spritePath + "player_left_1.png").c_str()));
    walkAnimation.addFrame(LEFT, IMG_LoadTexture(renderer, (spritePath + "player_left_2.png").c_str()));

    walkAnimation.addFrame(RIGHT, IMG_LoadTexture(renderer, (spritePath + "player_right.png").c_str()));
    walkAnimation.addFrame(RIGHT, IMG_LoadTexture(renderer, (spritePath + "player_right_1.png").c_str()));
    walkAnimation.addFrame(RIGHT, IMG_LoadTexture(renderer, (spritePath + "player_right_2.png").c_str()));
}

// cập nhật trạng thái của người chơi
void Player::update(Map& map) {
    if (isDead) return;
    const Uint8* keys = SDL_GetKeyboardState(NULL); // nhận trạng thái từ bàn phím
    calculateMove(keys, map); // tính toán hướng di chuyển
    walkAnimation.update(); // cập nhật animation
}

// xử lý đầu vào để di chuyển nhân vật
void Player::calculateMove(const Uint8* keys, Map& map) {
	if (moving) return;
    int dx = 0, dy = 0;
    // xử lý di chuyển bằng các phím mũi tên
    if (keys[SDL_SCANCODE_UP]) { dy = -1; direction = UP; }
    else if (keys[SDL_SCANCODE_DOWN]) { dy = 1; direction = DOWN; }
    else if (keys[SDL_SCANCODE_LEFT]) { dx = -1; direction = LEFT; }
    else if (keys[SDL_SCANCODE_RIGHT]) { dx = 1; direction = RIGHT; }
	if (dx != 0 || dy != 0) move(dx, dy, map);
    //moving = (dx != 0 || dy != 0);
    // xử lý đặt boom
    if (keys[SDL_SCANCODE_SPACE]) {
        placeBomb(map);
    }
}

// xử lý di chuyển của nhân vật
void Player::move(int dx, int dy, Map& map) {
    float nextX = posX + dx * speed; // xử lý di chuyển theo tốc độ
    float nextY = posY + dy * speed;

    int tileX1 = static_cast<int>(nextX) / 32; // xác định ô lưới tiếp theo
    int tileY1 = static_cast<int>(nextY) / 32;
    int tileX2 = static_cast<int>(nextX + 31) / 32;
    int tileY2 = static_cast<int>(nextY + 31) / 32;

    if (canMove(tileX1, tileY1, map) && canMove(tileX2, tileY2, map)) {
        posX = nextX;
        posY = nextY;
        x = static_cast<int>(posX) / 32;
        y = static_cast<int>(posY) / 32;
        walkAnimation.setDirection(direction);
        std::cout << "Moved to: (" << posX << ", " << posY << ")" << std::endl;
	}
    else {
        moving = false;
        std::cout << "Blocked at: (" << posX << ", " << posY << ")" << std::endl;
    }
}

// kiểm tra khả năng va chạm với các ô lưới
bool Player::canMove(int newX, int newY, Map& map) {
    TileType tile = map.getTile(newX, newY);
    return tile == GRASS ||
        tile == PORTAL ||
        tile == BOMB_ITEM ||
        tile == FLAME_ITEM ||
        tile == SPEED_ITEM;
}

// xử lý khi nhân vật nhặt vật phẩm
void Player::collectItem(TileType itemType) {
    switch (itemType) {
    case BOMB_ITEM:
        bombCount = std::min(bombCount + 1, 5); // Giới hạn tối đa 5 quả bom
        break;
    case FLAME_ITEM:
        flameRange = std::min(flameRange + 1, 5); // Giới hạn phạm vi nổ tối đa 5 ô
        break;
    case SPEED_ITEM:
        speed = std::min(speed + 0.5f, 5.0f); // Giới hạn tốc độ tối đa 5.0
        break;
    default:
        break;
    }
}

// vẽ nhân vật trên bản đồ
void Player::render(SDL_Renderer* renderer) {
    if (isDead) return;

    SDL_Rect playerRect = { static_cast<int>(posX), static_cast<int>(posY), 32, 32 };
    if (moving) {
        walkAnimation.render(renderer, static_cast<int>(posX), static_cast<int>(posY));
    }
    else {
        SDL_RenderCopy(renderer, walkAnimation.getFirstFrame(direction), nullptr, &playerRect);
    }
}

bool Player::reachedPortal(const Map& map, const std::vector<Enemy>& enemies) {
    // Kiểm tra nếu còn kẻ địch sống sót
    for (const Enemy& enemy : enemies) {
        if (enemy.isAlive()) {
            return false; // Vẫn còn kẻ địch, không thể vào cổng
        }
    }

    // Chỉ có thể vào cổng khi tất cả kẻ địch bị tiêu diệt
    return map.getTile(x, y) == PORTAL;
}

void Player::resetPosition() {
    x = startX;
    y = startY;
    posX = static_cast<float>(startX * 32);
    posY = static_cast<float>(startY * 32);
    isDead = false;
}

std::pair<int, int> Player::getPosition() const {
    return { x, y };
}

void Player::placeBomb(Map& map) {
    if (bombCount > 0 && map.getTile(x, y) == GRASS) {
        map.placeBomb(x, y);
        bombCount--;
    }
}

void Player::die() {
    if (!isDead) {
        isDead = true;
        std::cout << "Player died" << std::endl;
    }
}

