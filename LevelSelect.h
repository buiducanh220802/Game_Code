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
    bool loadAssets(); // tải tất cả tài nguyên cần sử dụng như hình ảnh, âm thanh, texture
    void render();      // render menu lên màn hình
    bool handleEvents(bool& running, int& selectedLevel);   //xử lý dữ kiện chuột
    bool show();        // show menu lên màn hình
    int getSelectedLevel() const;       // lấy màn chơi đã chọn trả về 1 con số của màn chơi

private:
    SDL_Renderer* renderer;         // renderer chính
    std::string resPath;    // đường dẫn chính của thư mục res


    SDL_Texture* backgroundTexture;     // hình nền cho menu chọn level
    SDL_Texture* levelTextures[5];      // texture của 5 nút chọn level


    Mix_Music* bgMusic;     // nhạc nền khi hiển thị menu

    SDL_Rect levelRects[5];     // vị trí và kích thước của từng nút level


    SDL_Texture* loadTexture(const std::string& path); // load một texture từ file

    int selectedLevel;      // lưu giá trị màn chơi được chọn
    
};

#endif
