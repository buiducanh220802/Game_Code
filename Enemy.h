#ifndef ENEMY_H
#define ENEMY_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Animation.h"
#include "Direction.h"
#include "Map.h"

class Enemy {
private:
    int x, y; // vị trí trên lưới
    float posX, posY; // vị trí thực tế
    float speed; // tốc độ di chuyển
    float _targetX, _targetY; // đích đến tiếp theo
    Direction direction;
    SDL_Texture* enemyTexture;
    Animation walkAnimation;
	Mix_Chunk* deathSound; // âm thanh khi enemy chết

    bool alive;
    bool moving;
    void move(int dx, int dy);
    bool canMove(float newX, float newY, Map& map);
    int getRandomDirection();

public:
    Enemy() = default;
    Enemy(int x, int y);
    ~Enemy();

    void init(SDL_Renderer* renderer,const std::string& enemyType);
    void update(Map& map);
    void render(SDL_Renderer* renderer);
    void move();
    bool isAlive() const { return alive; }
    int getX() const { return posX; }
    int getY() const { return posY; }
    bool die();
};

#endif
