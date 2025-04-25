#ifndef LEVELSELECT_H
#define LEVELSELECT_H

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

class LevelSelect {
public:
    LevelSelect(SDL_Renderer* renderer, const std::string& resPath);
    ~LevelSelect();
    bool loadAssets();
    void render();
    bool handleEvents(bool& running, int& selectedLevel);
    bool show();
    int getSelectedLevel() const;

private:
    SDL_Renderer* renderer;
    std::string resPath;
    SDL_Texture* backgroundTexture;
    SDL_Texture* levelTextures[5];
    Mix_Music* bgMusic;

    SDL_Rect levelRects[5];
    SDL_Texture* loadTexture(const std::string& path);

    int selectedLevel;
    
};

#endif
