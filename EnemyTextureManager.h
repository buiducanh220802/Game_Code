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
	static void loadEnemyTextures(SDL_Renderer* renderer, const std::string& type);
	static SDL_Texture* getTexture(const std::string& type, Direction dir, int frame);
	static void cleanUp();

private:
	// enemyType --> direction --> frame
	static std::unordered_map<std::string, std::unordered_map<Direction, std::vector<SDL_Texture*>>> enemyTextures;
	
};
#endif  //ENEMY_TEXTURE_MANAGER_H