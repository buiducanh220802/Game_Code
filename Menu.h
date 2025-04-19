#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>
class Menu {
public: 
	Menu(SDL_Renderer* renderer, const std::string& resPath);
	~Menu();
	bool show();

private: 
	SDL_Texture* loadTexture(const std::string& text, SDL_Color color);

	SDL_Texture* backgroundTexture;
	SDL_Texture* startButtonTexture;
	SDL_Texture* exitButtonTexture;
	Mix_Music* menuMusic;

	SDL_Rect startButtonRect;
	SDL_Rect exitButtonRect;
	SDL_Rect originalStartButtonRect;
	SDL_Rect originalExitButtonRect;
	SDL_Renderer* renderer;
	
	std::string resPath;
	bool loadAssets();
	void render();
	bool handleEvents(bool& running);

	SDL_Texture* loadTexture(const std::string& path);

};
