#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <string>
#include <map>
#include <utility> // Bổ sung cho std::pair
#include "Items.h"

enum TileType { GRASS, WALL, BRICK, PORTAL, BOMBER, ONEAL, KONDORIA , BOMB_ITEM, FLAME_ITEM, SPEED_ITEM, BOMB };

class Map {
public:
    static const int TILE_SIZE = 32;

    Map(); // Constructor to initialize member variables
	~Map(); // Destructor to free resources
    void loadFromFile(const std::string& filename);
    void render(SDL_Renderer* renderer, int offsetX, int offsetY);

    TileType getTile(int x, int y) const;
    void destroyTile(int x, int y);
    bool loadTextures(SDL_Renderer* renderer);
    size_t getWidth() const { return grid.empty() ? 0 : grid[0].size(); }
    size_t getHeight() const { return grid.size(); }
    bool isWall(int x, int y) const;
    bool isBomb(int x, int y) const;
    /*bool canMove(int x, int y) const;*/
    bool isBrick(int x, int y) const;
   
private:
    std::vector<std::vector<TileType>> grid;
    std::map<std::pair<int, int>, TileType> hiddenItems; // Lưu vị trí & loại item bị giấu

    SDL_Texture* grassTexture;
    SDL_Texture* wallTexture;
    SDL_Texture* brickTexture;
    SDL_Texture* portalTexture;
    SDL_Texture* bombItemTexture;
    SDL_Texture* flameItemTexture;
    SDL_Texture* speedItemTexture;
    SDL_Texture* onealTexture;
    SDL_Texture* kondoriaTexture;
    SDL_Texture* bomberTexture;
	//SDL_Texture* bombTexture; // Texture cho bom    
};

#endif
