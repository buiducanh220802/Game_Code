#include "Items.h"
#include <SDL_image.h>

const int TILE_SIZE = 32; // xác định kích thước của ô lưới

void Item::init(ItemType type, int px, int py, SDL_Texture* tex) {
    this->type = type;
    x = px * TILE_SIZE; // chuyển từ tọa độ lưới sang tọa độ pixel
    y = py * TILE_SIZE;
    texture = tex; // Lưu texture của item
}
// vẽ render Items
void Item::render(SDL_Renderer* renderer) {
    if (!texture) {
        SDL_Log("Error: Attempted to render an item without a texture!");
        return;
    }

    SDL_Rect itemRect = { x, y, TILE_SIZE, TILE_SIZE };
    SDL_RenderCopy(renderer, texture, nullptr, &itemRect);
    static int counter = 0;
    counter++;

    if (counter % 20 < 10) { // Hiệu ứng nhấp nháy mỗi 20 frame
        SDL_Rect itemRect = { x, y, TILE_SIZE, TILE_SIZE };
        SDL_RenderCopy(renderer, texture, nullptr, &itemRect);
    }
}

ItemType Item::getType() {
    return type;
}
void Item::playPickupSound() {
    Mix_Chunk* sound = Mix_LoadWAV("D:/Project_1/x64/Debug/res/sounds/Item.wav");
    if (!sound) return;
    Mix_PlayChannel(-1, sound, 0);
    Mix_FreeChunk(sound);
}
