#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include <vector> // Include vector header
#include "Map.h"
#include "Animation.h"
#include "Direction.h"
#include "Enemy.h"
class Enemy; // Khai báo chuyển tiếp tránh vòng lặp


class Player {
private:
    int x, y;
    int startX, startY;
    float posX, posY;
    float speed;
    bool moving;
    bool isDead;
    int bombCount;
    int flameRange;

    SDL_Texture* playerTexture;
    Direction direction;
    Animation walkAnimation;

public:
    Player(Map* map);
    void init(SDL_Renderer* renderer);
    void update(Map& map);
    void render(SDL_Renderer* renderer);
    std::pair<float, float> getPosition() const; // Hàm lấy vị trí người chơi
    void move(int dx, int dy, Map& map);
    void calculateMove(const Uint8* keys, Map& map);
    bool canMove(float newX, float newY, Map& map);
    void collectItem(TileType itemType);
    bool reachedPortal(const Map& map, const std::vector<Enemy>& enemies);
    void resetPosition();
    void placeBomb(Map& map);
    void die(); // thêm hàm để xử lý khi bị tiêu diệt
    int getX() const { return x; }
    int getY() const { return y; }
    bool isAlive() const { return !isDead; }
};

#endif
