#include "Map.h"
#include <fstream>
#include <iostream>


const int TILE_SIZE = 32;
const int HUD_HEIGHT = 32;

Map::Map() // các constructor để khởi tạo các biến thành viên
    : grassTexture(nullptr), wallTexture(nullptr), brickTexture(nullptr),
    portalTexture(nullptr), bombItemTexture(nullptr), flameItemTexture(nullptr),
    speedItemTexture(nullptr), onealTexture(nullptr), kondoriaTexture(nullptr),
    bomberTexture(nullptr), bombTexture(nullptr) {
}
Map::~Map() { // giải phóng texture
    SDL_DestroyTexture(grassTexture);
    SDL_DestroyTexture(wallTexture);
    SDL_DestroyTexture(brickTexture);
    SDL_DestroyTexture(portalTexture);
    SDL_DestroyTexture(bombItemTexture);
    SDL_DestroyTexture(flameItemTexture);
    SDL_DestroyTexture(speedItemTexture);
    SDL_DestroyTexture(onealTexture);
    SDL_DestroyTexture(kondoriaTexture);
    SDL_DestroyTexture(bomberTexture);
    SDL_DestroyTexture(bombTexture);
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
    file.ignore();

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
            case 'f': grid[i][j] = BRICK; hiddenItems[{i, j}] = FLAME_ITEM; break;
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
        {&bombItemTexture, "powerup_bombs.png"}, {&flameItemTexture, "powerup_flamepass.png"},
        {&speedItemTexture, "powerup_speed.png"}, {&onealTexture, "oneal_right1.png"},
        {&kondoriaTexture, "kondoria_right1.png"}, {&bomberTexture, "player_right.png"}, {&bombTexture, "bomb.png" },
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
            SDL_Rect dstRect = { (int)col * TILE_SIZE + offsetX, (int)row * TILE_SIZE + offsetY + HUD_HEIGHT, TILE_SIZE, TILE_SIZE };
            SDL_RenderCopy(renderer, grassTexture, nullptr, &dstRect);

            SDL_Texture* texture = nullptr;
            switch (grid[row][col]) {
            case WALL: texture = wallTexture; break;
            case BRICK: texture = brickTexture; break;
            case BOMB: texture = bombTexture; break; // Hiển thị bom khi đặt
            }

            if (texture) {
                SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
            }
            // thêm hiển thị vật phẩm sau khi phá gạch (brick)
            switch (hiddenItems[{row, col}]) {
            case BOMB_ITEM: texture = bombItemTexture; break;
            case FLAME_ITEM: texture = flameItemTexture; break;
            case SPEED_ITEM: texture = speedItemTexture; break;
            case PORTAL: texture = portalTexture; break;
            }
            if (texture) {
                SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
            }
        }
    }
}

TileType Map::getTile(int x, int y) const { // trả lại tọa độ ô lưới
    if (y < 0 || y >= getHeight() || x < 0 || x >= getWidth()) {
        return WALL;
    }
    return grid[y][x];
}

bool Map::isWall(int x, int y) const {
    if (x < 0 || x >= getWidth() || y < 0 || y >= getHeight()) {
        return true;
    }
    return grid[y][x] == WALL;
}

bool Map::isBomb(int x, int y) const {
    return grid[y][x] == BOMB;
}

bool Map::canMove(int x, int y) const {
    if (x < 0 || x >= getWidth() || y < 0 || y >= getHeight()) return false;

    // Kiểm tra tất cả 4 góc của nhân vật (giả sử kích thước 32x32)
    return getTile(x, y) == GRASS &&
        getTile(x + 31 / TILE_SIZE, y) == GRASS &&
        getTile(x, y + 31 / TILE_SIZE) == GRASS &&
        getTile(x + 31 / TILE_SIZE, y + 31 / TILE_SIZE) == GRASS;
}


void Map::destroyTile(int x, int y) {
    if (getTile(x, y) == BRICK) {
        auto it = hiddenItems.find({ y, x });
        if (it != hiddenItems.end()) {
            grid[y][x] = it->second; // Gán vật phẩm vào bản đồ
            hiddenItems.erase(it);   // Xóa khỏi danh sách vật phẩm ẩn
        }
        else {
            grid[y][x] = GRASS;
        }
    }
}

void Map::removeItem(int x, int y) {
    TileType tile = getTile(x, y);
    if (tile == BOMB_ITEM || tile == FLAME_ITEM || tile == SPEED_ITEM) {
        grid[y][x] = GRASS;
    }

}

void Map::placeBomb(int x, int y) {
    if (y >= 0 && y < grid.size() && x >= 0 && x < grid[0].size()) {
        if (grid[y][x] == GRASS) {
            grid[y][x] = BOMB;
        }
    }
}
