#ifndef AI_H
#define AI_H

#include "Direction.h" // Đảm bảo Direction được định nghĩa trước
#include "Player.h"
#include "Map.h"

class Player; // Forward declaration to avoid AI not recognizing Player   

class AI {
public:
    virtual ~AI() = default;
    virtual Direction calculateDirection(const Player& player, int enemyX, int enemyY, Map& map) = 0;
};

class AIRandom : public AI {
public:
    Direction calculateDirection(const Player& player, int enemyX, int enemyY, Map& map) override;
};

class AISmart : public AI {
public:
    Direction calculateDirection(const Player& player, int enemyX, int enemyY, Map& map) override;
};

#endif // AI_H
