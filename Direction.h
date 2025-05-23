﻿
#ifndef DIRECTION_H
#define DIRECTION_H
// các hướng phụ trợ cho animation, di chuyển của player và enemy
enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    CENTER,
    NONE
};
namespace std {
	template<>
	struct hash<Direction> {
		std::size_t operator()(const Direction& d) const noexcept {
			return std::hash<int>()(static_cast<int>(d));
		}
	};
}

#endif // DIRECTION_H
