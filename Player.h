#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include<SDL_mixer.h>
#include <vector> // Include vector header
#include <memory>
#include "Map.h"
#include "Animation.h"
#include "Direction.h"
#include "Enemy.h"
class Enemy; // Khai báo chuyển tiếp tránh vòng lặp
class Bomb;

class Player {
private:
    int x, y;
    int startX, startY;
    float posX, posY;
    float speed;
    bool moving;
    bool isDead;
    bool dying;
    int deathTimer;
    int bombCount;
    int flameRange;
    float _targetX, _targetY; // Tọa độ mục tiêu di chuyển
    //const std::vector<std::unique_ptr<Enemy>>& enemies;

    SDL_Texture* playerTexture;
    Direction direction;
    Animation walkAnimation;
    Animation deathAnimation; 
	Mix_Chunk* deathSound; // Âm thanh khi player chết
	Mix_Chunk* pickupSound; // Âm thanh khi thu thập vật phẩm

public:
    Player(Map* map);
    void init(SDL_Renderer* renderer);
    void update(Map& map);
    void render(SDL_Renderer* renderer);
    std::pair<float, float> getPosition() const; // Hàm lấy vị trí người chơi
    void move();
    void calculateMove(const Uint8* keys, Map& map);
    bool canMove(float newX, float newY, Map& map);
    void collectItem(TileType itemType);
    bool reachedPortal(const Map& map, const std::vector<std::unique_ptr<Enemy>>& enemies);
    void resetPosition();
    void placeBomb(Map& map, std::vector<Bomb>& bombs, SDL_Renderer* renderer, std::vector<std::unique_ptr<Enemy>>& enemies);
    void handleBombInput(const Uint8* keyState, std::vector<Bomb>& bombs, Map& map, SDL_Renderer* renderer, std::vector<std::unique_ptr<Enemy>>& enemies);
    void die(); // thêm hàm để xử lý khi bị tiêu diệt
    int getX() const { return posX; } // lấy tọa độ lưới ô vuông của người chơi
    int getY() const { return posY; }
    bool isAlive() const { return !isDead; }
    void increaseBombCount();
    void checkCollisionWithEnemies(const std::vector<std::unique_ptr<Enemy>>& enemies);
    void checkItemCollision(Map& map);

};

#endif
