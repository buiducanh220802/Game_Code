
#ifndef ANIMATION_H
#define ANIMATION_H

#include <SDL.h>
#include <vector>
#include <map>
#include "Direction.h"

class Animation {
private:
    std::map<Direction, std::vector<SDL_Texture*>> frames; // Frames theo hướng
    Direction currentDirection;
    int currentFrame;
    int frameCount;
    int frameDelay; // Thời gian giữa các frame
    int frameTimer; // Đếm thời gian giữa các frame

public:
    Animation();
    ~Animation();
    SDL_Texture* getFirstFrame(Direction dir);

    void addFrame(Direction dir, SDL_Texture* texture);
    void update();
    void render(SDL_Renderer* renderer, int x, int y);
    void setDirection(Direction dir);
    void reset();
    //void cleanUp(); // Thêm phương thức cleanUp để dọn dẹp tài nguyên
    int getFrameCount(Direction dir) const;

};

#endif
