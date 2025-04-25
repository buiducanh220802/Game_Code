#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <string>
class Menu {
public: 
	Menu(SDL_Renderer* renderer, const std::string& resPath);
	~Menu();
	bool show(); // hiển thị Menu, trả về boole để biết người chơi có muốn chơi hay không

private: 
	// tạo texture từ text 
	SDL_Texture* loadTexture(const std::string& text, SDL_Color color);

	SDL_Texture* backgroundTexture;		// texture cho hình nền
	SDL_Texture* startButtonTexture;	// cho nút start game	
	SDL_Texture* exitButtonTexture;		// cho nút exit
					
	Mix_Music* menuMusic;		// nhạc nền tại Menu ban đầu

	SDL_Rect startButtonRect;	// vị trí và kích thước các nút
	SDL_Rect exitButtonRect;

	// lưu vị trí gốc để scale khi hover chuột
	SDL_Rect originalStartButtonRect;
	SDL_Rect originalExitButtonRect;

	SDL_Renderer* renderer;
	
	std::string resPath; // đường dẫn chính của thư mục chứa res
	bool loadAssets();	// tải các tài nguyên
	void render();		// render 
	bool handleEvents(bool& running);	// xử lý các dữ kiện chuột

	SDL_Texture* loadTexture(const std::string& path);	// load texture

};
