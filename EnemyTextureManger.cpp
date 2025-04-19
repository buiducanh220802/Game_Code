#include "EnemyTextureManager.h"
#include <SDL_image.h>
#include <iostream>

std::unordered_map<std::string, std::unordered_map<Direction, std::vector<SDL_Texture*>>> EnemyTextureManager::enemyTextures;

void EnemyTextureManager::loadEnemyTextures(SDL_Renderer* renderer, const std::string& type) {
    if (enemyTextures.find(type) != enemyTextures.end()) {
		std::cout << "Textures for type " << type << " already loaded." << std::endl;
        return; // đã load
	}
    const std::string directions[] = { "right", "left", "up", "down" };
    const Direction dirEnum[] = { RIGHT, LEFT, UP, DOWN };

    for (int i = 0; i < 4; i++) {
        Direction dir = dirEnum[i];
        for (int frame = 1; frame <= 3; frame++) {
            std::string path = "D:/Project_1/x64/Debug/res/sprites/" + type + "_" + directions[i] + std::to_string(frame) + ".png";
            SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());

            if (!texture) {
                std::cerr << "❌ Failed to load texture: " << path << " Error: " << IMG_GetError() << std::endl;
            }
            else {
                enemyTextures[type][dir].push_back(texture);
                //std::cout << "Loaded texture: " << path << std::endl;
            }
        }
    }
}

SDL_Texture* EnemyTextureManager::getTexture(const std::string& type, Direction dir, int frame) {
    auto typeIt = enemyTextures.find(type); // Tìm type trong unordered_map
    if (typeIt == enemyTextures.end()) {
		std::cerr << "❌ Texture type not found: " << type << std::endl;
        return nullptr; // Không tìm thấy type
    }
    auto dirIt = typeIt->second.find(dir); // Tìm direction trong type
    if (dirIt == typeIt->second.end()) {
		std::cerr << "❌ Texture direction not found: " << dir << std::endl;
        return nullptr; // Không tìm thấy direction
	}
    const auto& frames = dirIt->second;
    if (frame < 0 || frame >= frames.size()) return nullptr; // Kiểm tra frame hợp lệ

    return frames[frame];
}

void EnemyTextureManager::cleanUp() {
    for (auto& [type, dirMap] : enemyTextures) {
        for (auto& [dir, textures] : dirMap) {
            for (SDL_Texture* tex : textures) {
                if (tex) SDL_DestroyTexture(tex); // Giải phóng texture
            }
            textures.clear(); // Xóa tất cả textures
        }
    }
    enemyTextures.clear(); // Xóa hết tất cả các mục trong enemyTextures
	std::cout << "All enemy textures cleaned up." << std::endl;
}
