#pragma once
#ifndef ENEMY_TEXTURE_MANAGER_H
#define ENEMY_TEXTURE_MANAGER_H

#include <unordered_map>
#include <vector>
#include <string>
#include <SDL.h>
#include "Direction.h"

class EnemyTextureManager {
public:
	// load toàn bộ texture animation cho một loại enemy
	static void loadEnemyTextures(SDL_Renderer* renderer, const std::string& type);

	// trả về texture của enemy cụ thể
	static SDL_Texture* getTexture(const std::string& type, Direction dir, int frame);

	// dọn dẹp tài nguyên
	static void cleanUp();

private:
	// cấu trúc lưu trữ cho từng loại enemy
	// enemyType --> direction --> frame
	static std::unordered_map<std::string, std::unordered_map<Direction, std::vector<SDL_Texture*>>> enemyTextures;
	
};
#endif  //ENEMY_TEXTURE_MANAGER_H