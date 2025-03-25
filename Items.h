
#ifndef ITEM_H
#define ITEM_H

#include <SDL.h>
#include <SDL_mixer.h>

enum ItemType { SPEED, FLAME, BOMBFLAME };

class Item {
public:
    void init(ItemType type, int px, int py, SDL_Texture* tex);
    void render(SDL_Renderer* renderer);
    void playPickupSound();
    ItemType getType();
private:
    ItemType type;
    int x, y;
    SDL_Texture* texture = nullptr; // Thêm texture
};

#endif

