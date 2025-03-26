#include "Player.h"
#include "Map.h"
#include "Enemy.h"
#include "Bomb.h"
#include <iostream>

const int TILE_SIZE = 32;
const int HUD_HEIGHT = 32;

Player::Player(Map* map) {
    startX = 1;
    startY = 1;
    x = startX;
    y = startY;
    posX = x * TILE_SIZE;
    posY = y * TILE_SIZE;
    speed = 2.0f;
    moving = false;
    isDead = false;
    bombCount = 1;
    flameRange = 1;
    direction = Direction::NONE;
}

void Player::init(SDL_Renderer* renderer) {
    std::string spritePath = "D:/Project_1/x64/Debug/res/sprites/";

    // Tạo danh sách texture cho từng hướng
    std::vector<std::string> directions = { "down", "up", "left", "right" };

    for (const auto& dir : directions) {
        for (int i = 0; i < 3; ++i) {
            std::string fileName = spritePath + "player_" + dir + (i == 0 ? ".png" : "_" + std::to_string(i) + ".png");
            SDL_Texture* texture = IMG_LoadTexture(renderer, fileName.c_str());
            if (!texture) {
                std::cerr << "Failed to load " << fileName << ": " << IMG_GetError() << std::endl;
                continue;
            }
            if (dir == "down") walkAnimation.addFrame(DOWN, texture);
            else if (dir == "up") walkAnimation.addFrame(UP, texture);
            else if (dir == "left") walkAnimation.addFrame(LEFT, texture);
            else if (dir == "right") walkAnimation.addFrame(RIGHT, texture);
        }
    }
}

void Player::update(Map& map) {
    if (isDead) return;
    const Uint8* keys = SDL_GetKeyboardState(NULL); // nh?n tr?ng thái t? bàn phím
    if (!keys) return; // Bảo vệ tránh lỗi

    calculateMove(keys, map); // tính toán hu?ng di chuy?n
    if (moving) {
        walkAnimation.update(); // Chỉ cập nhật animation nếu nhân vật đang di chuyển
    }
}

void Player::render(SDL_Renderer* renderer) {
    if (isDead) return;

    SDL_Rect playerRect = { static_cast<int>(posX), static_cast<int>(posY), 32, 32 };

    if (moving) {
        walkAnimation.render(renderer, static_cast<int>(posX), static_cast<int>(posY));
    }
    else {
        SDL_Texture* currentFrame = walkAnimation.getFirstFrame(direction);
        if (!currentFrame) {
            std::cerr << "Error: Player texture is NULL! Using default texture." << std::endl;
            return;
        }

        // Lấy kích thước thực của frame
        int texW, texH;
        SDL_QueryTexture(currentFrame, nullptr, nullptr, &texW, &texH);
        playerRect.w = texW;
        playerRect.h = texH;

        SDL_RenderCopy(renderer, currentFrame, nullptr, &playerRect);
    }
}


void Player::move(int dx, int dy, Map& map) {
    float newX = posX + dx * TILE_SIZE;
    float newY = posY + dy * TILE_SIZE;

    if (canMove(newX, newY, map)) {
        posX = newX;
        posY = newY;
        x = static_cast<int>(posX / TILE_SIZE);
        y = static_cast<int>(posY / TILE_SIZE);
    }

    moving = false;
}

void Player::calculateMove(const Uint8* keys, Map& map) {
    if (keys[SDL_SCANCODE_UP]) {
        direction = Direction::UP;
        move(0, -1, map);
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        direction = Direction::DOWN;
        move(0, 1, map);
    }
    else if (keys[SDL_SCANCODE_LEFT]) {
        direction = Direction::LEFT;
        move(-1, 0, map);
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        direction = Direction::RIGHT;
        move(1, 0, map);
    }
    else {
        moving = false;
    }
}
bool Player::canMove(float newX, float newY, Map& map) {
    // Xác định hitbox của nhân vật
    int left = static_cast<int>(newX);
    int right = static_cast<int>(newX + TILE_SIZE - 1);
    int top = static_cast<int>(newY);
    int bottom = static_cast<int>(newY + TILE_SIZE - 1);

    // Kiểm tra 4 góc của nhân vật
    TileType topLeft = map.getTile(left / TILE_SIZE, top / TILE_SIZE);
    TileType topRight = map.getTile(right / TILE_SIZE, top / TILE_SIZE);
    TileType bottomLeft = map.getTile(left / TILE_SIZE, bottom / TILE_SIZE);
    TileType bottomRight = map.getTile(right / TILE_SIZE, bottom / TILE_SIZE);

    // Nếu bất kỳ góc nào là WALL hoặc BRICK thì không cho di chuyển
    if (topLeft == WALL || topRight == WALL || bottomLeft == WALL || bottomRight == WALL ||
        topLeft == BRICK || topRight == BRICK || bottomLeft == BRICK || bottomRight == BRICK) {
        return false;
    }

    // Kiểm tra va chạm với Bomb
    if (map.isBomb(left / TILE_SIZE, top / TILE_SIZE) ||
        map.isBomb(right / TILE_SIZE, top / TILE_SIZE) ||
        map.isBomb(left / TILE_SIZE, bottom / TILE_SIZE) ||
        map.isBomb(right / TILE_SIZE, bottom / TILE_SIZE)) {
        if (!map.isBomb(x / TILE_SIZE, y / TILE_SIZE)) { // Kiểm tra nếu Bomber đã rời khỏi bomb
            return false;
        }
    }
    return true;
}


// x? lý khi nhân v?t nh?t v?t ph?m
void Player::collectItem(TileType itemType) {
    switch (itemType) {
    case BOMB_ITEM:
        bombCount = std::min(bombCount + 1, 5); // Gi?i h?n t?i da 5 qu? bom
        break;
    case FLAME_ITEM:
        flameRange = std::min(flameRange + 1, 5); // Gi?i h?n ph?m vi n? t?i da 5 ô
        break;
    case SPEED_ITEM:
        speed = std::min(speed + 0.5f, 5.0f); // Gi?i h?n t?c d? t?i da 5.0
        break;
    default:
        break;
    }
}

bool Player::reachedPortal(const Map& map, const std::vector<Enemy>& enemies) {
    if (map.getTile(x, y) == TileType::PORTAL) {
        for (const auto& enemy : enemies) {
            if (enemy.getX() == x && enemy.getY() == y) {
                return false;
            }
        }
        return true;
    }
    return false;
}

void Player::resetPosition() {
    x = startX;
    y = startY;
    posX = x * TILE_SIZE;
    posY = y * TILE_SIZE;
    isDead = false;
}

void Player::placeBomb(Map& map) {
    if (bombCount > 0 && map.getTile(x, y) == GRASS) {
        map.placeBomb(x, y);
        bombCount--;
    }
}

void Player::die() {
    isDead = true;
}
std::pair<float, float> Player::getPosition() const {
    return { posX, posY };
}