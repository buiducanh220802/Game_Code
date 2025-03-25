#include "Animation.h"

Animation::Animation() : currentDirection(DOWN), currentFrame(0), frameDelay(5), frameTimer(0) {}
// khởi tạo Animation với các giá trị mặc định gồm hướng mặc định ban đầu, frame hiện tại, thời gian delay giữa các frame, đếm thời gian giữa các frame
// độ trễ giữa các frame tức là cứ n lần update() thì mới đổ frame mới
Animation::~Animation() {
	for (auto& pair : frames) { // Duyệt qua tất cả các frame từ map frames
        for (auto texture : pair.second) {
            SDL_DestroyTexture(texture);
        }
		pair.second.clear(); // xóa vector texture sau khi giải phóng
    }
	frames.clear(); // xóa map frames sau khi giải phóng
}

void Animation::addFrame(Direction dir, SDL_Texture* texture) {
	frames[dir].push_back(texture); // thêm texture vào vector frames theo hướng dir
}
// cập nhật animation thay đổi frame hiện tại
void Animation::update() {
    frameTimer++;
    if (frameTimer >= frameDelay) {
        if (!frames[currentDirection].empty()) { // kiểm tra frames có tồn tại không trước khi cập nhật
            currentFrame = (currentFrame + 1) % frames[currentDirection].size();
        }
		frameTimer = 0; // reset frameTimer
    }
}
// render animation hiển thị frames hiện tại 
void Animation::render(SDL_Renderer* renderer, int x, int y) {
    if (!frames[currentDirection].empty()) { // kiểm tra hợp lệ mới render
		SDL_Rect dstRect = { x, y, 32, 32 }; // dùng tọa độ với kích thước sprite 32x32
        SDL_RenderCopy(renderer, frames[currentDirection][currentFrame], nullptr, &dstRect);
	} // sử dụng RenderCopy để vẽ frame hiện tại lên màn hình
}
// cập nhật các hướng di chuyển 
void Animation::setDirection(Direction dir) {
	if (dir != currentDirection) { // kiểm tra hướng mới có khác hướng hiện tại không
        currentDirection = dir;
        currentFrame = 0; // Reset frame khi đổi hướng
    }
}
// lấy frame đầu tiên của hướng di chuyển
SDL_Texture* Animation::getFirstFrame(Direction dir) {
    if (!frames[dir].empty()) {
        return frames[dir][0]; // Frame đầu tiên
    }
    return nullptr;
}
// reset animation về frame đầu tiên
void Animation::reset() {
    currentFrame = 0;
    frameTimer = 0;
}
