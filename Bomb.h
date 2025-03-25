#ifndef BOMB_H
#define BOMB_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include "Animation.h"
#include "Map.h"
#include "Enemy.h"
#include "Player.h"

class Bomb {
public:
    Bomb(SDL_Renderer* renderer,Map* gameMap, Player* player, std::vector<Enemy>& enemies);
    ~Bomb();
    Map* map;
    void place(int px, int py);
    void update();
    void render(SDL_Renderer* renderer);
    bool isActive() const { return active; }
    void checkExplosionHit();
    void explode();

private:
    int gridX, gridY;
    int x, y;
    int timer;
    bool active;
    bool exploded;
    Player* player;
    std::vector<Enemy> enemies;
    
    Mix_Chunk* explosionSound;
    SDL_Texture* bombTexture;
    Animation explosionCenter;
    Animation explosionUp;
    Animation explosionDown;
    Animation explosionLeft;
    Animation explosionRight;


};

#endif
