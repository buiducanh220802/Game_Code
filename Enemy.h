#ifndef ENEMY_H
#define ENEMY_H

#include <SDL.h>
#include <SDL_image.h>
#include <memory>
#include "Animation.h"
#include "Direction.h"
#include "Map.h" // Include Map header
#include "AI.h"

class Player; // Forward declaration to avoid AI not recognizing Player
class AI; // Khai báo chuyển tiếp để tránh lỗi chưa nhận diện AI

class Enemy {
private:
    int x, y; // vị trí trên lưới
    float posX, posY; // vị trí thực tế
    float speed; // tốc độ di chuyển
    Direction direction;
    bool moving;
    bool isSmart; // Phân biệt giữa AI thông minh và AI ngẫu nhiên
    std::shared_ptr<AI> ai; // Trỏ đến AI đang sử dụng
    SDL_Texture* enemyTexture;
    Animation walkAnimation;
    void move(int dx, int dy, Map& map);
    bool canMove(int newX, int newY, Map& map);
    bool alive;
public:
    Enemy(const Enemy& other);
    Enemy& operator=(const Enemy& other);
    Enemy(bool smart);
    ~Enemy(); // Destructor to clean up AI pointer
    void init(SDL_Renderer* renderer, int startX, int startY);
    void update(Map& map, const Player& player);
    void render(SDL_Renderer* renderer);
    bool die(); // Xử lý khi bị tiêu diệt
    int getX() const { return x; }
    int getY() const { return y; }
    bool isAlive() const { return alive; }
};

#endif
