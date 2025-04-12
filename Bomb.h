#ifndef BOMB_H
#define BOMB_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include "Animation.h"
#include "Map.h"
#include "Enemy.h"
#include "Player.h"

class Bomb {
public:
    // Constructors and Destructor
    Bomb();  // Default constructor
    Bomb(SDL_Renderer* renderer, Map* map, Player* player, std::vector<Enemy>& enemies);
    ~Bomb();

    // Public Methods
    void place(int px, int py);  // Đặt bom tại tọa độ (px, py)
    void update();               // Cập nhật trạng thái bom
    void render(SDL_Renderer* renderer);  // Vẽ bom và hiệu ứng nổ
    bool isActive() const { return active; }  // Kiểm tra bom có đang hoạt động không
    void checkExplosionHit();  // Kiểm tra va chạm với Enemy và Player
    void explode();            // Kích hoạt vụ nổ của bom
    void resetExplosion();
    bool isFinished() const;


    // Getters
    int getGridX() const { return gridX; } // lấy tọa độ ô lưới
    int getGridY() const { return gridY; }

private:
    // Private Member Variables
    int gridX, gridY;  // Tọa độ của bom trên lưới
    int x, y;           // Tọa độ pixel của bom
    int timer;          // Thời gian còn lại để bom nổ
    bool active;        // Trạng thái bom (hoạt động hay không)
    bool exploded;      // Kiểm tra bom đã nổ hay chưa
    Player* player;     // Con trỏ đến đối tượng Player
    std::vector<Enemy>& enemies;  // Danh sách đối tượng Enemy
    Map* map;

    // SDL Resources
    Mix_Chunk* explosionSound;  // Âm thanh của vụ nổ
	Mix_Chunk* placeBombSound;  // Âm thanh khi đặt bom
    SDL_Texture* bombTexture;   // Texture cho bom
    Animation explosionCenter;  // Animation cho trung tâm vụ nổ
    Animation explosionUp;      // Animation cho nổ phía trên
    Animation explosionDown;    // Animation cho nổ phía dưới
    Animation explosionLeft;    // Animation cho nổ bên trái
    Animation explosionRight;   // Animation cho nổ bên phải

    // Private Methods
    bool loadTextures(SDL_Renderer* renderer);  // Tải các texture cho bom và hiệu ứng nổ
    void cleanUp();                             // Dọn dẹp tài nguyên khi kết thúc
    void renderExplosion(SDL_Renderer* renderer); // Vẽ các hướng của vụ nổ
};

#endif
