#include "AI.h"
#include <cstdlib>  // rand()
#include <cmath>    // abs()
#include <map>

// AI ngẫu nhiên: Chọn hướng đi ngẫu nhiên nhưng hợp lệ
Direction AIRandom::calculateDirection(const Player& player, int enemyX, int enemyY, Map& map) {
    std::vector<Direction> validDirections;
	// kiểm tra 4 hướng xung quanh
    if (map.getTile(enemyX, enemyY - 1) == GRASS) validDirections.push_back(UP);
    if (map.getTile(enemyX, enemyY + 1) == GRASS) validDirections.push_back(DOWN);
    if (map.getTile(enemyX - 1, enemyY) == GRASS) validDirections.push_back(LEFT);
    if (map.getTile(enemyX + 1, enemyY) == GRASS) validDirections.push_back(RIGHT);

    if (!validDirections.empty()) {
		int randIndex = rand() % validDirections.size(); // chọn ngẫu nhiên 1 hướng đi hợp lệ
        return validDirections[randIndex];
    }
    return DOWN;  // Nếu không có hướng nào hợp lệ, đứng yên
}

// AI thông minh: Tìm đường đến người chơi nhưng tránh tường
Direction AISmart::calculateDirection(const Player& player, int enemyX, int enemyY, Map& map) {
    int playerX, playerY;
	std::pair<int, int> playerPos = player.getPosition(); // lấy vị trí người chơi
    playerX = playerPos.first;
    playerY = playerPos.second;

    int dx = playerX - enemyX; // lấy khoảng cách theo cách trục giữa enemy và player;
    int dy = playerY - enemyY;

    if (abs(dx) > abs(dy)) { // ưu tiên di chuyển theo trục X
        if (dx > 0 && map.getTile(enemyX + 1, enemyY) == GRASS) return RIGHT;
        if (dx < 0 && map.getTile(enemyX - 1, enemyY) == GRASS) return LEFT;
    }
    if (dy > 0 && map.getTile(enemyX, enemyY + 1) == GRASS) return DOWN;
    if (dy < 0 && map.getTile(enemyX, enemyY - 1) == GRASS) return UP;

    return DOWN; // Nếu không thể đi đâu, đứng yên
}
// file AI.cpp chứa các hàm cài đặt cho AI phục vụ cho file Enemy.cpp 