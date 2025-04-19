
#ifndef ITEM_H
#define ITEM_H

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include "TileType.h"

class Item {
public:
    Item(int x, int y, TileType type, SDL_Texture* textture);

    void render(SDL_Renderer* renderer, int offsetX = 0, int offsetY = 0);

	int getX() const { return x; }
	int getY() const { return y; }
	TileType getType() const { return type; }

private:
    TileType type;
    int x, y;
    SDL_Texture* texture = nullptr; // Thêm texture
};

#endif

