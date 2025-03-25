#include "Enemy.h"
#include <cstdlib>
#include <memory>
#include <iostream>
Enemy::Enemy(const Enemy& other)
    : x(other.x), y(other.y), posX(other.posX), posY(other.posY),
    speed(other.speed), direction(other.direction), moving(other.moving),
    isSmart(other.isSmart), enemyTexture(other.enemyTexture), alive(other.alive),
    walkAnimation(other.walkAnimation) { // Copy animation

    if (other.ai) {  // Kiểm tra nếu AI tồn tại
        if (dynamic_cast<AISmart*>(other.ai.get())) {
            ai = std::make_shared<AISmart>();
        }
        else {
            ai = std::make_shared<AIRandom>();
        }
    }
    else {
        ai = nullptr; // Nếu other.ai không tồn tại
    }
}
Enemy& Enemy::operator=(const Enemy& other) {
    if (this != &other) {  // Tránh tự gán chính nó
        x = other.x;
        y = other.y;
        posX = other.posX;
        posY = other.posY;
        speed = other.speed;
        direction = other.direction;
        moving = other.moving;
        isSmart = other.isSmart;
        enemyTexture = other.enemyTexture;
        alive = other.alive;
        walkAnimation = other.walkAnimation;

        if (other.ai) {  // Kiểm tra nếu AI tồn tại
            if (dynamic_cast<AISmart*>(other.ai.get())) {
                ai = std::make_shared<AISmart>();
            }
            else {
                ai = std::make_shared<AIRandom>();
            }
        }
        else {
            ai = nullptr; // Nếu other.ai không tồn tại
        }
    }
    return *this;
}

Enemy::Enemy(bool smart)
    : x(0), y(0), posX(0.0f), posY(0.0f), speed(0.0f), direction(Direction::DOWN),
    moving(false), isSmart(smart), enemyTexture(nullptr), alive(true) {

    // Sử dụng std::make_unique để khởi tạo AI an toàn
    if (smart) {
        ai = std::make_shared<AISmart>();
    }
    else {
        ai = std::make_shared<AIRandom>();
    }
}
Enemy::~Enemy() = default; // Destructor mặc định
// khởi tạo Enemy với các giá trị mặc định
void Enemy::init(SDL_Renderer* renderer, int startX, int startY) {
    x = startX; // xác định vị trí ban đầu trên lưới
    y = startY;
    posX = static_cast<float>(startX);
    posY = static_cast<float>(startY);

    std::string pathPrefix = "D:/Project_1/x64/Debug/res/sprites/";
    std::string enemyType = isSmart ? "kondoria" : "oneal"; // chọn loại Enemy tương ứng với loại AI để thỏa mãn texture
    // tạo animation di chuyển cho Enemy
    const std::string directions[4] = { "up", "down", "left", "right" };
    for (int dir = 0; dir < 4; ++dir) { // có tổng 4 hướng di chuyển
        for (int i = 1; i <= 3; ++i) { // có 3 frame cho mỗi hướng di chuyển
            walkAnimation.addFrame(static_cast<Direction>(dir),
                IMG_LoadTexture(renderer, (pathPrefix + enemyType + "_" + directions[dir] + std::to_string(i) + ".png").c_str()));
        }
    }
}

// cập nhật trạng thái enemy
void Enemy::update(Map& map, const Player& player) {
    if (!ai) {
        std::cerr << "AI not loaded" << std::endl;
        return;
    }
    int moveDirection = ai->calculateDirection(player, x, y, map); // Tính toán hướng di chuyển
    int dx = 0, dy = 0; // 2 ký hiệu biến để xác định hướng di chuyển tiếp theo

    switch (moveDirection) { // chuyển đổi hướng di chuyển từ AI sang dx, dy
    case UP: dy = -1; direction = UP; break;
    case DOWN: dy = 1; direction = DOWN; break;
    case LEFT: dx = -1; direction = LEFT; break;
    case RIGHT: dx = 1; direction = RIGHT; break;
    default: moving = false; return;
    }
    if (dx != 0 || dy != 0) {
        move(dx, dy, map); // có hướng hợp lệ gọi hàm move để di chuyển
        moving = true;
    }
    else {
        moving = false;
    }

    walkAnimation.update(); // cập nhật animation di chuyển

}

// xử lý di chuyển của Enemy
void Enemy::move(int dx, int dy, Map& map) {
    float newPosX = posX + dx * speed; // tính vị trí mới và tốc độ speed
    float newPosY = posY + dy * speed;

    int newX = static_cast<int>((newPosX + 16) / 32); // Thêm 16 để tránh bị kẹt pixel
    int newY = static_cast<int>((newPosY + 16) / 32); // chuyển đổi toa độ pixel sang toạ độ lưới

    if (canMove(newX, newY, map)) { // kiểm tra xem có thể di chuyển đến ô mới không
        posX = newPosX; // cập nhật vị trí mới
        posY = newPosY; // cập nhật vị trí mới

        // Chỉ cập nhật x, y khi Enemy gần tới ô tiếp theo để tránh bị cập nhật liên tục khi chưa vào ô mới.
        if (abs(posX - newX * 32) < speed) x = newX;
        if (abs(posY - newY * 32) < speed) y = newY;

        walkAnimation.setDirection(direction); // cập nhật hướng di chuyển
    }
}

// kiểm tra xem có thể di chuyển đến ô mới không
bool Enemy::canMove(int newX, int newY, Map& map) {
    if (newX < 0 || newY < 0 || newX >= map.getWidth() || newY >= map.getHeight()) {
        return false; // Ngăn chặn Enemy đi ra khỏi bản đồ
    }
    // chỉ cho phép di chuyển trong các tile hợp lệ như cỏ, portal, item
    TileType tile = map.getTile(newX, newY);
    return tile == GRASS || tile == PORTAL || tile == BOMB_ITEM || tile == FLAME_ITEM || tile == SPEED_ITEM;
}

// vẽ Enemy lên màn hình
void Enemy::render(SDL_Renderer* renderer) {
    SDL_Rect enemyRect = { static_cast<int>(posX), static_cast<int>(posY), 32, 32 };
    // tạo Rect đẻ xác định vị trí của Enemy dựa trên bản đồ
    if (moving) { // nếu di chuyển hiện thị animation di chuyển
        walkAnimation.render(renderer, static_cast<int>(posX), static_cast<int>(posY));
    }
    else { // đứng yên thì hiển thị frame đầu tiên của animation
        SDL_RenderCopy(renderer, walkAnimation.getFirstFrame(direction), nullptr, &enemyRect);
    }
}

bool Enemy::die() {
    if (alive) {
        alive = false;
        std::cout << " Enemy died at (" << x << ", " << y << ")\n";
    }
    return alive;
}
