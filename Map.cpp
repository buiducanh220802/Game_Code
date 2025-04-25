#include "Map.h"
#include <fstream>
#include <iostream>


const int TILE_SIZE = 32;
const int HUD_HEIGHT = 32;

Map::Map() // các constructor để khởi tạo các biến thành viên
    : grassTexture(nullptr), wallTexture(nullptr), brickTexture(nullptr),
    portalTexture(nullptr), bombItemTexture(nullptr), detonatorItemTexture(nullptr),
    speedItemTexture(nullptr), onealTexture(nullptr), kondoriaTexture(nullptr),
    bomberTexture(nullptr) {
}
Map::~Map() { // giải phóng texture
    SDL_DestroyTexture(grassTexture);
    SDL_DestroyTexture(wallTexture);
    SDL_DestroyTexture(brickTexture);
    SDL_DestroyTexture(portalTexture);
    SDL_DestroyTexture(bombItemTexture);
    SDL_DestroyTexture(detonatorItemTexture);
    SDL_DestroyTexture(speedItemTexture);
    SDL_DestroyTexture(onealTexture);
    SDL_DestroyTexture(kondoriaTexture);
    SDL_DestroyTexture(bomberTexture);
    
}
// tải bản đồ từ file đã có .txt
void Map::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to load map file: " << filename << std::endl;
        return;
    }

    int level, rows, cols;
    file >> level >> rows >> cols;
    file >> std::ws;
    /*file.ignore();*/

    if (rows <= 0 || cols <= 0) {
        std::cerr << "Invalid map size in file: " << filename << std::endl;
        return;
    }
    // xóa dữ liệu cũ, tránh lỗi khi loading nhiều level
    grid.clear();
	grid.resize(rows, std::vector<TileType>(cols, GRASS)); // khởi tạo bản đồ full GRASS    
    hiddenItems.clear();

    for (int i = 0; i < rows; ++i) {
        std::string line;
        std::getline(file, line);

        if (line.size() != cols) {
            std::cerr << "Line length mismatch at row " << i << " in file: " << filename << std::endl;
            return;
        }
        // chuyển ký tự từ file sang kiểu TileType trong grid để vẽ bản đồ
        for (int j = 0; j < cols; ++j) {
            switch (line[j]) {
            case '#': grid[i][j] = WALL; break;
            case '*': grid[i][j] = BRICK; break;
            case 'p': grid[i][j] = BOMBER; break;
            case '1': grid[i][j] = ONEAL; break;
            case '2': grid[i][j] = KONDORIA; break;
            case 'b': grid[i][j] = BRICK; hiddenItems[{i, j}] = BOMB_ITEM; break;
            case 'f': grid[i][j] = BRICK; hiddenItems[{i, j}] = DETONATOR_ITEM; break;
            case 's': grid[i][j] = BRICK; hiddenItems[{i, j}] = SPEED_ITEM; break;
            case 'x': grid[i][j] = BRICK; hiddenItems[{i, j}] = PORTAL; break;
            default: grid[i][j] = GRASS;
            } // trên đây là các ký hiệu được mặc định trong bản đồ
        }
    }

}
// hàm load ảnh cho các biến thành viên
bool Map::loadTextures(SDL_Renderer* renderer) {
    std::vector<std::pair<SDL_Texture**, std::string>> textures = {
        {&grassTexture, "grass.png"}, {&wallTexture, "wall.png"},
        {&brickTexture, "brick.png"}, {&portalTexture, "portal.png"},
        {&bombItemTexture, "powerup_bombs.png"}, {&detonatorItemTexture, "powerup_detonator.png"},
        {&speedItemTexture, "powerup_speed.png"}, {&onealTexture, "oneal_right1.png"}
    };

    for (auto& tex : textures) {
        *tex.first = IMG_LoadTexture(renderer, ("D:/Project_1/x64/Debug/res/sprites/" + tex.second).c_str());
        if (!*tex.first) {
            std::cerr << "Failed to load texture: " << tex.second << std::endl;
            return false;
        }
    }
    return true;
}
// hàm vẽ bản đồ 
void Map::render(SDL_Renderer* renderer, int offsetX, int offsetY) {
    for (size_t row = 0; row < grid.size(); ++row) {
        for (size_t col = 0; col < grid[row].size(); ++col) {
            SDL_Rect dstRect = { (int)col * TILE_SIZE + offsetX, (int)row * TILE_SIZE + offsetY, TILE_SIZE, TILE_SIZE };

            // Luôn vẽ grass trước
            SDL_RenderCopy(renderer, grassTexture, nullptr, &dstRect);

            SDL_Texture* texture = nullptr;

            // Vẽ tường, gạch hoặc bom
            switch (grid[row][col]) {
            case WALL: texture = wallTexture; break;
            case BRICK: texture = brickTexture; break;
            }

            if (texture) {
                SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
            }

            // Nếu ô hiện tại là BRICK, không vẽ vật phẩm (vật phẩm vẫn ẩn bên dưới)
            if (grid[row][col] == BRICK) {
                continue;
            }

            // Vẽ vật phẩm nếu không còn gạch
            switch (hiddenItems[{row, col}]) {
            case BOMB_ITEM: texture = bombItemTexture; break;
            case DETONATOR_ITEM: texture = detonatorItemTexture; break;
            case SPEED_ITEM: texture = speedItemTexture; break;
            case PORTAL: texture = portalTexture; break;
            default: texture = nullptr; break;
            }

            if (texture) {
                SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
            }
        }
    }
}


TileType Map::getTile(int x, int y) const { // trả lại tọa độ ô lưới
    if (y < 0 || y >= getHeight() || x < 0 || x >= getWidth()) {
        std::cerr << "Warning: Invalid tile access (" << x << ", " << y << ")" << std::endl;
        return WALL;
    }
    return grid[y][x];
}

bool Map::isWall(int x, int y) const {
    if (x < 0 || x >= getWidth() || y < 0 || y >= getHeight()) {
        return true;
    }
    TileType tile = grid[y][x];
    return tile == WALL;
}

bool Map::isBomb(int x, int y) const {
    return grid[y][x] == BOMB;
}
void Map::destroyTile(int x, int y) {
    if (getTile(x, y) == BRICK) {
        auto it = hiddenItems.find({ y, x });

        // nếu có vật phẩm tại vị trí này
        if (it != hiddenItems.end()) {
            // gán loại tile là item tương ứng
            grid[y][x] = it->second; // Gán vật phẩm vào bản đồ

        }
        else {
            // không có item gán thành grass đi
            grid[y][x] = GRASS;
        }
    }
}

bool Map::isBrick(int x, int y) const {
    if (x < 0 || x >= getWidth() || y < 0 || y >= getHeight()) {
        return false;
    }
    return grid[y][x] == BRICK;
}

void Map::removeItemAt(int x, int y) {
    if (grid[y][x] == BOMB_ITEM || grid[y][x] == DETONATOR_ITEM ||
        grid[y][x] == SPEED_ITEM ) {
            grid[y][x] = GRASS;
            hiddenItems.erase({ y,x });
            std::cout << "Items reomved" << std::endl;
        }
}