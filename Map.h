#ifndef MAP_H
#define MAP_H

#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <string>
#include <map>
#include <utility> // Bổ sung cho std::pair
#include <memory>   // sử dụng smart pointer
#include "Items.h"
#include "TileType.h"

class Map {
public:
    static const int TILE_SIZE = 32; // kích thước mỗi ô vuông là 32x32 pixel

    Map(); // Constructor 
	~Map(); // Destructor 
    void loadFromFile(const std::string& filename);   // hàm tải và đọc bản đồ từ file văn bản txt
    void render(SDL_Renderer* renderer, int offsetX, int offsetY);  // hàm vẽ render toàn bộ bản đồ lên màn hình
	void removeItemAt(int x, int y); // Xóa item tại vị trí (x, y)

    TileType getTile(int x, int y) const;   // lấy loại ô tại tọa độ (x, y)
    void destroyTile(int x, int y);     // phá hủy ô gạch tại vị trí (x, y)
    bool loadTextures(SDL_Renderer* renderer);  // tải các texture cần thiết

    //getter lấy chiều rộng và chiều cao của bản đồ theo đơn vị tile
    size_t getWidth() const { return grid.empty() ? 0 : grid[0].size(); }
    size_t getHeight() const { return grid.size(); }

    // hàm kiểm tra loại ô phục vụ cho các đối tượng khác để xử lý di chuyển, va chạm
    bool isWall(int x, int y) const;
    bool isBomb(int x, int y) const;
    bool isBrick(int x, int y) const;
   
private:
    std::vector<std::vector<TileType>> grid;    // vector lưu từng loại ô trong bản đồ
    std::map<std::pair<int, int>, TileType> hiddenItems; // Lưu vị trí & loại item bị giấu
    std::vector<std::unique_ptr<Item>> items;   //con trỏ thông minh

    // texture cho đối tượng ô ban đầu
    SDL_Texture* grassTexture;
    SDL_Texture* wallTexture;
    SDL_Texture* brickTexture;

    // texture cho từng loại item
    SDL_Texture* portalTexture;
    SDL_Texture* bombItemTexture;
    SDL_Texture* detonatorItemTexture;
    SDL_Texture* speedItemTexture;

    // texture cho enemy và player
    SDL_Texture* onealTexture;
    SDL_Texture* kondoriaTexture;
    SDL_Texture* bomberTexture;
};

#endif
