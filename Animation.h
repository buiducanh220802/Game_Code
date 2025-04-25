
#ifndef ANIMATION_H
#define ANIMATION_H

#include <SDL.h>
#include <vector>
#include <map>
#include "Direction.h"

class Animation {
private:
    // lưu các frame theo từng hướng và mỗi hướng có vector chứa 3 png
    std::map<Direction, std::vector<SDL_Texture*>> frames; 

    // hướng hiện tại của animation
    Direction currentDirection;

    // frame hiện tại đang được hiện thị trong animation
    int currentFrame;

    //tổng ố frame trong hướng hiện tại
    int frameCount;

    // khoảng thời gian delay giữa 2 frame ( tính theo frame)
    int frameDelay; 

    // bộ đến để tính thời điểm chuyển frame
    int frameTimer; 

public:
    Animation(); // constuctor khởi tạo
    ~Animation(); // destructor để giải phóng tài nguyên

    // hàm trả về frame đầu tiên của một hướng nhất định
    SDL_Texture* getFirstFrame(Direction dir);

    // hàm thêm frame ảnh cho một hướng
    void addFrame(Direction dir, SDL_Texture* texture);
    
    // hàm cập nhật frame hiện tại và bộ đếm thời gian
    void update();

    // hàm để vẽ frame tại các tọa độ lên renderer
    void render(SDL_Renderer* renderer, int x, int y);

    // hàm thiết lập hướng hiện tại cho animation
    void setDirection(Direction dir);
    
    // trả về frame đầu tiên
    void reset();
    
    // trả về số lượng frame có mặt trong một hướng cụ thể --> để debug lỗi
    int getFrameCount(Direction dir) const;

};

#endif
