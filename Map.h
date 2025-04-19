#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <string>
#include <map>
#include <utility> // Bổ sung cho std::pair
#include <memory>
#include "Items.h"
#include "TileType.h"

class Map {
public:
    static const int TILE_SIZE = 32;

    Map(); // Constructor to initialize member variables
	~Map(); // Destructor to free resources
    void loadFromFile(const std::string& filename);
    void render(SDL_Renderer* renderer, int offsetX, int offsetY);
	void removeItemAt(int x, int y); // Xóa item tại vị trí (x, y)

    TileType getTile(int x, int y) const;
    void destroyTile(int x, int y);
    bool loadTextures(SDL_Renderer* renderer);


    size_t getWidth() const { return grid.empty() ? 0 : grid[0].size(); }
    size_t getHeight() const { return grid.size(); }

    bool isWall(int x, int y) const;
    bool isBomb(int x, int y) const;
    bool isBrick(int x, int y) const;
   
private:
    std::vector<std::vector<TileType>> grid;
    std::map<std::pair<int, int>, TileType> hiddenItems; // Lưu vị trí & loại item bị giấu
    std::vector<std::unique_ptr<Item>> items;

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
};

#endif
