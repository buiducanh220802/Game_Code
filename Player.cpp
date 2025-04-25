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
    _targetX = posX;  // Đặt mục tiêu ban đầu
    _targetY = posY;
    speed = 1.0f;
    moving = false;
    isDead = false;
	dying = false;
	deathTimer = 0;
    bombCount = 1;
    flameRange = 1;
    direction = Direction::NONE;
}

void Player::init(SDL_Renderer* renderer) {
    deathSound = Mix_LoadWAV("D:/Project_1/x64/Debug/res/sounds/PLAYER_DIE.wav");
    if (!deathSound) {
        std::cerr << "❌ Failed to load player death sound: " << Mix_GetError() << std::endl;
    }
	pickupSound = Mix_LoadWAV("D:/Project_1/x64/Debug/res/sounds/Item.wav");
    if (!pickupSound) {
        std::cerr << "❌ Failed to load pickup sound: " << Mix_GetError() << std::endl;
    }
    std::string spritePath = "D:/Project_1/x64/Debug/res/sprites/";

    // Duyệt qua 4 hướng
    std::vector<std::pair<Direction, std::string>> directions = {
        {DOWN, "player_down"},
        {UP, "player_up"},
        {LEFT, "player_left"},
        {RIGHT, "player_right"}
    };

    for (const auto& dir : directions) {
        for (int i = 1; i <= 3; i++) { // Load đủ 3 frame
            std::string filePath = spritePath + dir.second + "_" + std::to_string(i) + ".png";
            SDL_Texture* texture = IMG_LoadTexture(renderer, filePath.c_str());
            if (!texture) {
                std::cerr << "⚠️ Failed to load texture: " << filePath << " | Error: " << IMG_GetError() << std::endl;
                continue;
            }
            walkAnimation.addFrame(dir.first, texture); // Thêm vào animation theo hướng
        }
    }
    // Load death animation
    for (int i = 1; i <= 3; i++) {
        std::string filePath = spritePath + "player_dead_" + std::to_string(i) + ".png";
        SDL_Texture* texture = IMG_LoadTexture(renderer, filePath.c_str());
        if (!texture) {
			std::cerr << "Failed to load death texture: " << filePath << " | Error: " << IMG_GetError() << std::endl;
        }
		deathAnimation.addFrame(DOWN, texture); // Tất cả hướng đều giống nhau
    }
    // Kiểm tra ít nhất có một frame load thành công
    if (walkAnimation.getFirstFrame(DOWN) == nullptr) {
        std::cerr << "🛑 ERROR: No textures loaded for player! Check file paths." << std::endl;
    }
    // Kiểm tra frame animation chết
    if (deathAnimation.getFirstFrame(DOWN) == nullptr) {
        std::cerr << "🛑 ERROR: No death animation frames loaded!" << std::endl;
    }
}


void Player::update(Map& map) {
    if (isDead) return;
    if (invincibilityTimer > 0) {
        invincibilityTimer--;
    }
    const Uint8* keys = SDL_GetKeyboardState(NULL); // nhận trạng thái từ bàn phím
    if (!keys) return; // Bảo vệ tránh lỗi
    /*std::cout << "Updating player..." << std::endl;*/
    calculateMove(keys, map); // tính toán hu?ng di chuy?n
   
    /*std::cout << "Player position: (" << posX << ", " << posY << ")" << std::endl;*/
    if (moving) {
        move();
        walkAnimation.setDirection(direction); // Cập nhật hướng animation
        walkAnimation.update(); // Chỉ cập nhật animation nếu nhân vật đang di chuyển
    }

    if (!moving) {
		checkItemCollision(map); // Kiểm tra va chạm với item
        
    }
    if (dying) {
        deathTimer--;
        deathAnimation.update();
        if (deathTimer <= 0) {
            dying = false;
        }
        return;
    }
}

void Player::render(SDL_Renderer* renderer) {
    if (isDead && dying) {
        deathAnimation.render(renderer, posX, posY);
        return;
    }

    SDL_Rect playerRect = { static_cast<int>(posX), static_cast<int>(posY), 32, 32 };
    //std::cout << "Rendering player at: (" << posX << ", " << posY << ")" << std::endl;
    if (moving) {
        walkAnimation.render(renderer, static_cast<int>(posX), static_cast<int>(posY));
    }
    else {
        SDL_Texture* currentFrame = walkAnimation.getFirstFrame(DOWN);
        if (!currentFrame) {
            std::cerr << "Error: Player texture is NULL!" << std::endl;
            return; // Không vẽ nếu không có texture
        }
        SDL_RenderCopy(renderer, currentFrame, nullptr, &playerRect);
    }
    
}

void Player::move() {
    if (!moving) return;

    // Tính khoảng cách di chuyển mỗi frame
    float dx = (_targetX - posX);
    float dy = (_targetY - posY);

    // Nếu đã gần đến vị trí cần đến, đặt hẳn vào ô đúng
    if (abs(dx) <= speed && abs(dy) <= speed) {
        posX = _targetX;
        posY = _targetY;
        moving = false;
		
    }
    else {
        // Di chuyển từng bước nhỏ theo speed
        posX += (dx != 0) ? speed * (dx / abs(dx)) : 0;
        posY += (dy != 0) ? speed * (dy / abs(dy)) : 0;
    }
}
void Player::calculateMove(const Uint8* keys, Map& map) {
    //std::cout << "Checking input..." << std::endl;

    if (!moving) { // Chỉ đặt mục tiêu mới khi không di chuyển
        if (keys[SDL_SCANCODE_UP]) {
            direction = Direction::UP;
            _targetX = posX;
            _targetY = posY - TILE_SIZE;
            //std::cout << "Input: UP -> Target: (" << _targetX << ", " << _targetY << ")" << std::endl;
        }
        else if (keys[SDL_SCANCODE_DOWN]) {
            direction = Direction::DOWN;
            _targetX = posX;
            _targetY = posY + TILE_SIZE;
            //std::cout << "Input: DOWN -> Target: (" << _targetX << ", " << _targetY << ")" << std::endl;
        }
        else if (keys[SDL_SCANCODE_LEFT]) {
            direction = Direction::LEFT;
            _targetX = posX - TILE_SIZE;
            _targetY = posY;
            //std::cout << "Input: LEFT -> Target: (" << _targetX << ", " << _targetY << ")" << std::endl;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            direction = Direction::RIGHT;
            _targetX = posX + TILE_SIZE;
            _targetY = posY;
            //std::cout << "Input: RIGHT -> Target: (" << _targetX << ", " << _targetY << ")" << std::endl;
        }
        else {
            //std::cout << "No input detected!" << std::endl;
        }
    }
    else {
        //std::cout << "Player is already moving, ignoring new input!" << std::endl;
    }

    if (canMove(_targetX, _targetY, map)) {
        moving = true;
        //std::cout << "Move allowed -> Start moving!" << std::endl;
    }
    else {
        //std::cout << "Move blocked!" << std::endl;
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


// xử lý khi nhân vật thu thập vật phẩm
void Player::collectItem(TileType itemType) {
    switch (itemType) {
    case BOMB_ITEM:
		bombCount = std::min(bombCount + 1, 5); // Giới hạn số bom tối đa mà người chơi có thể mang theo
        break;
    case DETONATOR_ITEM:
        invincibilityTimer = 240; // Thời gian miễn dịch với mọi va chạm
        break;
    case SPEED_ITEM:
        speed = std::min(speed + 0.5f, 5.0f); // Giới hạn tốc độ tối đa có thể đi
        break;
    default:
        break;
    }
}

bool Player::reachedPortal(const Map& map, const std::vector<std::unique_ptr<Enemy>>& enemies) {
    if (map.getTile(posX / TILE_SIZE, posY / TILE_SIZE) == TileType::PORTAL) {
        std::cout << "Player reached portal at (" << x << ", " << y << ")\n";  // Debug log
        for (const auto& enemy : enemies) {
            if (enemy->getX() == posX && enemy->getY() == posY) {
                return false;
            }
        }
        std::cout << "Portal is clear, player can pass.\n";  // Debug log
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

void Player::handleBombInput(const Uint8* keyState, std::vector<std::unique_ptr<Bomb>>& bombs, Map& map, SDL_Renderer* renderer, std::vector<std::unique_ptr<Enemy>>& enemies) {
    if (keyState[SDL_SCANCODE_SPACE]) {
        placeBomb(map, bombs, renderer, enemies);
    }
}

void Player::placeBomb(Map& map, std::vector<std::unique_ptr<Bomb>>& bombs, SDL_Renderer* renderer, std::vector<std::unique_ptr<Enemy>>& enemies) {

    std::pair<int, int> playerPos = getPosition(); // lấy vị trí người chơi
    int bombX = playerPos.first / 32;
    int bombY = playerPos.second / 32;
    // Kiểm tra xem đã có bom tại vị trí này chưa
    for (const auto& bomb : bombs) {
        if (bomb->isActive() && bomb->getGridX() == bombX && bomb->getGridY() == bombY) {
           /* std::cout << "Bomb already exists at (" << bombX << ", " << bombY << ")\n";*/
            return;
        }
    }

    // Kiểm tra nếu Player có thể đặt bom tại vị trí này
    if (bombCount > 0 && map.getTile(bombX, bombY) == GRASS) {
        // Tạo bom mới và thêm vào danh sách
        bombs.push_back(std::make_unique<Bomb>(renderer, &map, this, enemies));  // Chú ý truyền đúng tham số
        bombs.back()->place(bombX, bombY); // Đặt bom vào vị trí

        bombCount--; // Giảm số lượng bom của Player

        //std::cout << "Bomb placed at (" << bombX << ", " << bombY << ")\n";
    }
}

void Player::die() {
    isDead = true;
    dying = true;
    deathTimer = 60;
    deathAnimation.reset();
	if (deathSound) {
		Mix_PlayChannel(-1, deathSound, 0);
	}
	else {
		//std::cerr << "❌ Failed to play player death sound: " << Mix_GetError() << std::endl;
	}
}
void Player::increaseBombCount() {
    bombCount++;
}

std::pair<float, float> Player::getPosition() const {
    return { posX, posY };
}
void Player::checkCollisionWithEnemies(const std::vector<std::unique_ptr<Enemy>>& enemies) {
    if (isDead) return;
    if (invincibilityTimer > 0) return;
    SDL_Rect playerRect = { posX, posY, 32, 32 };

    for (const auto& enemy : enemies) {
        if (!enemy->isAlive()) continue;

        SDL_Rect enemyRect = { enemy->getX(), enemy->getY(), 32, 32 };

        bool collied = !(playerRect.x + playerRect.w <= enemyRect.x ||
            enemyRect.x + enemyRect.w <= playerRect.x ||
            playerRect.y + playerRect.h <= enemyRect.y ||
            enemyRect.y + enemyRect.h <= playerRect.y);

        if (collied) {
			//std::cout << "Player collided with enemy at (" << enemy->getX() << ", " << enemy->getY() << ")\n";
			die(); // Xử lý va chạm
			break; // Không cần kiểm tra các enemy còn lại
        }
    }
}

void Player::checkItemCollision(Map& map) {
    int tileX = static_cast<int>(posX) / TILE_SIZE;
    int tileY = static_cast<int>(posY) / TILE_SIZE;
    TileType tile = map.getTile(tileX, tileY);
	if (tile == BOMB_ITEM || tile == DETONATOR_ITEM || tile == SPEED_ITEM || tile == PORTAL) {
		collectItem(tile);
		map.removeItemAt(tileX, tileY); // Xóa item sau khi thu thập
		if (pickupSound) {
			Mix_PlayChannel(-1, pickupSound, 0);
		}
		else {
			//std::cerr << "❌ Failed to play pickup sound: " << Mix_GetError() << std::endl;
		}
		//std::cout << "Collected item at (" << tileX << ", " << tileY << ")\n";
	}
}

