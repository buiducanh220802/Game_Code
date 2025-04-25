#ifndef ENEMY_H
#define ENEMY_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Animation.h"
#include "Direction.h"
#include "Map.h"

class Enemy {
private:
    int x, y; // vị trí trên lưới
    float posX, posY; // vị trí thực tế tọa độ pixel
    float speed; // tốc độ di chuyển
    float _targetX, _targetY; // tọa độ đích đến tiếp theo
    std::string enemyType; // loại Enemy đang được gọi
    Direction direction; // hướng di chuyển hiện tại của enemy

    SDL_Texture* enemyTexture;      // texture của enemy
    Animation walkAnimation;        // hoạt ảnh di chuyển cho enemy
	Mix_Chunk* deathSound;          // âm thanh khi enemy chết

    bool alive;         //cập nhật trạng thái của enemy
    bool moving;        // kiểm tra trạng thái trong quá trình di chuyển của enemy
    
    // hàm hỗ trợ
    bool canMove(float newX, float newY, Map& map);  // hàm kiểm tra va chạm tính toán di chuyển của enemy
    int getRandomDirection();           // trả về một hướng ngẫu nhiên khi di chuyển xong

public:
    Enemy() = default;      // contructor mặc định
    Enemy(int x, int y);    // contructor khởi tạo vị trí ban đầu
    ~Enemy();               // destructor
    Enemy(const Enemy&) = delete;  // không cho phép enemy sao chép hay gán
    Enemy& operator=(const Enemy&) = delete;

    // các phương thức chính
    void init(SDL_Renderer* renderer,const std::string& enemyType); // khởi tạo enemy với renderer
    void update(Map& map);      // cập nhật trạng thái của enemy 
    void render(SDL_Renderer* renderer);        // render enemy lên màn hình
    void move();            // hàm xử lý di chuyển của enemy
    bool isAlive() const { return alive; }  // hàm lấy các trạng thái trả về của enemy phục vụ cho xử lý va chạm
    int getX() const { return posX; }  // lấy tọa độ ô pixel của enemy
    int getY() const { return posY; }
    bool die();     
};

#endif
