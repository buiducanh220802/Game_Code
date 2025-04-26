#ifndef PLAYER_H
#define PLAYER_H

#include <SDL.h>
#include<SDL_mixer.h>
#include <vector> // Include vector header
#include <memory>
#include "Map.h"
#include "Animation.h"
#include "Direction.h"
#include "Enemy.h"
class Enemy; // Khai báo chuyển tiếp tránh vòng lặp
class Bomb;

class Player {
private:
    int x, y;       // tọa độ ô lưới vuông
    int startX, startY;     // tọa ban đầu
    float posX, posY;       // tọa độ pixel
    float speed;        // tốc độ di chuyển của player
    bool moving;        // các cờ kiểm tra trạng thái
    bool isDead;
    bool dying;
    int deathTimer;     // bộ đếm thời gian chết
    int bombCount;      // bộ đếm số lượng bom đang mang có thể đặt
    int flameRange;
    float _targetX, _targetY; // Tọa độ mục tiêu di chuyển
    int invincibilityTimer; // thời gian miễn dịch bất tử của player

    SDL_Texture* playerTexture;
    Direction direction;
    Animation walkAnimation;        // Animation khi di chuyển player
    Animation deathAnimation;       // Animation khi player die
	Mix_Chunk* deathSound; // Âm thanh khi player chết
	Mix_Chunk* pickupSound; // Âm thanh khi thu thập vật phẩm
    Mix_Chunk* portalSound; // Âm thanh khi player qua cổng portal

public:
    Player(Map* map); // constructor
    void init(SDL_Renderer* renderer);  // khởi tạo các texure, animation, âm thanh
    void update(Map& map);  // cập nhật các trạng thái của player
    void render(SDL_Renderer* renderer);    // render player lên màn hình
    std::pair<float, float> getPosition() const; // Hàm lấy vị trí người chơi
    void move();        // xử lý di chuyển của player
    void calculateMove(const Uint8* keys, Map& map);    // tính toán di chuyển xử lý đầu vào di chuyển
    bool canMove(float newX, float newY, Map& map);     // tính đoán va chạm để có thể di chuyển cho người chơi
    void collectItem(TileType itemType);        // thu thập các items và tăng hiệu ứng sức mạnh cho player
    bool reachedPortal(const Map& map, const std::vector<std::unique_ptr<Enemy>>& enemies); // kiểm tra cổng ra vào portal
    void resetPosition();   // reset lại các vị trí sau khi qua màn
    void placeBomb(Map& map, std::vector<std::unique_ptr<Bomb>>& bombs, SDL_Renderer* renderer, std::vector<std::unique_ptr<Enemy>>& enemies);   // xử lý đặt bom của player
    void handleBombInput(const Uint8* keyState, std::vector<std::unique_ptr<Bomb>>& bombs, Map& map, SDL_Renderer* renderer, std::vector<std::unique_ptr<Enemy>>& enemies);  // đầu vào khi đặt boom
    void die(); // thêm hàm để xử lý khi bị tiêu diệt
    int getX() const { return posX; } // lấy tọa độ lưới ô vuông của người chơi
    int getY() const { return posY; }
    int getImmortal() const { return invincibilityTimer; }  // trả về trạng thái hết thời gian miễn dịch
    bool isAlive() const { return !isDead; }    // trả về các trạng thái
    void increaseBombCount();       // reset boom sau khi đặt
    void checkCollisionWithEnemies(const std::vector<std::unique_ptr<Enemy>>& enemies);     // kiểm tra va chạm với enemy
    void checkItemCollision(Map& map);  // kiểm tra thu thập items và tạo hiệu ứng khi thu thập

};

#endif
