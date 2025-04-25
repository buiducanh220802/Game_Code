#include "Enemy.h"
#include "EnemyTextureManager.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

const int TILE_SIZE = 32;

Enemy::Enemy(int startX, int startY) {
    posX = static_cast<float>(startX * TILE_SIZE);
    posY = static_cast<float>(startY * TILE_SIZE);
	_targetX = posX;
	_targetY = posY;
    speed = 0.5f;
    direction = static_cast<Direction>(rand() % 4); // Chọn hướng ngẫu nhiên ban đầu
    moving = false;
    alive = true;
}

Enemy::~Enemy() {
    //walkAnimation.cleanUp();
    std::cout << "Enemy destroyed and resources cleaned up.\n";
}
void Enemy::init(SDL_Renderer* renderer,const std::string& enemyType) {
	this->enemyType = enemyType;

    deathSound = Mix_LoadWAV("D:/Project_1/x64/Debug/res/sounds/ENEMY_DIE.wav");
    if (!deathSound) {
        std::cerr << "❌ Failed to load enemy death sound: " << Mix_GetError() << std::endl;
    }

	EnemyTextureManager::loadEnemyTextures(renderer, enemyType);

    for (Direction dir : {RIGHT, LEFT, UP, DOWN}) {
        for (int frame = 0; frame < 3; frame++) {
            SDL_Texture* tex = EnemyTextureManager::getTexture(enemyType, dir, frame);
            if (tex) {
                walkAnimation.addFrame(dir, tex);
            }
            else {
				std::cerr << "❌ Failed to load texture for enemy type: " << enemyType << ", direction: " << dir << ", frame: " << frame << std::endl;
            }
        }
    }

    walkAnimation.setDirection(direction);
    if (!walkAnimation.getFirstFrame(RIGHT)) {
        std::cout << "Error: Missing texture for RIGHT direction!" << std::endl;
    }
    if (!walkAnimation.getFirstFrame(direction)) {
        std::cout << "❌ No default frame loaded for direction: " << direction << std::endl;
    }
}


void Enemy::update(Map& map) {
    if (!alive) return;

    if (!moving) {
        int moveDirection = getRandomDirection(); // Chỉ dùng AI ngẫu nhiên

        if (moveDirection == 0) {
            direction = UP;
            _targetX = posX;
            _targetY = posY - TILE_SIZE;
        }
        else if (moveDirection == 1) {
            direction = RIGHT;
            _targetX = posX + TILE_SIZE;
            _targetY = posY;
        }
        else if (moveDirection == 2) {
            direction = DOWN;
            _targetX = posX;
            _targetY = posY + TILE_SIZE;
        }
        else if (moveDirection == 3) {
            direction = LEFT;
            _targetX = posX - TILE_SIZE;
            _targetY = posY;
        }
        else {
            /*std::cout << "Invalid move direction!" << std::endl;*/
            return;
        }

        if (canMove(_targetX, _targetY, map)) {
            moving = true;
            walkAnimation.setDirection(direction);
            /*std::cout << "OK canMove trả về true" << std::endl;*/
        }
        else {
            moving = false;
            /*std::cout << "NotOk canMove trả về false" << std::endl;*/
        }
    }

    if (moving) {
        move();
        walkAnimation.update();
    }   
    //std::cout << "Updating enemy position: (" << posX << ", " << posY << ")" << std::endl;
}

int Enemy::getRandomDirection() {
    return rand() % 4;
}
void Enemy::move() {
	float dx = _targetX - posX;
	float dy = _targetY - posY;

    if (std::abs(dx) <= speed && std::abs(dy) <= speed) {
		posX = _targetX;
		posY = _targetY;
        moving = false;
    }
    else {
        posX += (dx != 0) ? speed * (dx / std::abs(dx)) : 0;
        posY += (dy != 0) ? speed * (dy / std::abs(dy)) : 0;
    }
	//std::cout << "Enemy moving to (" << posX << ", " << posY << ")" << std::endl;
}
bool Enemy::canMove(float newX, float newY, Map& map) {
    int left = static_cast<int>(newX);
    int right = static_cast<int>(newX + TILE_SIZE - 1);
    int top = static_cast<int>(newY);
    int bottom = static_cast<int>(newY + TILE_SIZE - 1);

    TileType topLeft = map.getTile(left / TILE_SIZE, top / TILE_SIZE);
    TileType topRight = map.getTile(right / TILE_SIZE, top / TILE_SIZE);
    TileType bottomLeft = map.getTile(left / TILE_SIZE, bottom / TILE_SIZE);
    TileType bottomRight = map.getTile(right / TILE_SIZE, bottom / TILE_SIZE);

    // Enemy không thể đi qua WALL hoặc BRICK
    if (topLeft == WALL || topRight == WALL || bottomLeft == WALL || bottomRight == WALL ||
        topLeft == BRICK || topRight == BRICK || bottomLeft == BRICK || bottomRight == BRICK) {
        return false;
    }

    // Cho phép Enemy đi qua GRASS, PORTAL, hoặc ITEMs
    return true;
}

void Enemy::render(SDL_Renderer* renderer) {
    if (!alive) {
		std::cout << "Enemy is dead and cannot be rendered.\n";
		return;
    }

    if (!renderer) {
        std::cout << "❌ Renderer is NULL!(không render được enemy)\n";
        return;
    }

    //std::cout << "Rendering enemy at (" << posX << ", " << posY << ")" << std::endl;
    
   
    SDL_Rect enemyRect = { static_cast<int>(posX), static_cast<int>(posY), 32, 32 };
    if (moving) {
        walkAnimation.render(renderer, static_cast<int>(posX), static_cast<int>(posY));
    }
    else {
        SDL_Texture* currentFrame = walkAnimation.getFirstFrame(direction);
        if (!currentFrame) {
            std::cout << "Enemy missing frame for dir: " << direction << "\n";
            return;
        }
        if (SDL_RenderCopy(renderer, currentFrame, nullptr, &enemyRect) != 0) {
            std::cout << "SDL_RenderCopy failed: " << SDL_GetError() << "\n";
        }

        else {
            //std::cout << "Rendering enemy at (" << posX << ", " << posY << ")\n";
        }
    }
}
bool Enemy::die() {
    if (alive) {
        alive = false;
        std::cout << "Enemy died at (" << x << ", " << y << ")\n";
    }
	if (deathSound) {
		Mix_PlayChannel(-1, deathSound, 0);
	}
	else {
		std::cerr << "❌ Failed to play enemy death sound: " << Mix_GetError() << std::endl;
	}
    return alive;
}

