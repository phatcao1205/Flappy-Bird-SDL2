// bird.h
#ifndef BIRD_H
#define BIRD_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "gamestate.h"  // Bao gồm gamestate.h để sử dụng GameState

// Cấu trúc Bird đại diện cho chim
struct Bird {
    SDL_Rect rect;  // Vị trí và kích thước của chim
    float velocity; // Vận tốc của chim
    double angle;   // Góc xoay của chim (tính bằng độ)
};

// Lớp BirdManager quản lý texture và animation của chim
class BirdManager {
public:
    BirdManager(SDL_Renderer* renderer);  // Hàm khởi tạo
    ~BirdManager();                       // Hàm hủy
    void updateBird(Bird& bird, GameState gameState);  // Cập nhật trạng thái chim
    void render(SDL_Renderer* renderer, const Bird& bird);  // Vẽ chim

private:
    SDL_Renderer* renderer;
    SDL_Texture* birdTextures[3];  // Texture cho 3 trạng thái vỗ cánh
    int currentFrame;              // Frame hiện tại của animation
    int frameDelay;                // Thời gian giữa các frame (ms)
    int frameTimer;                // Bộ đếm thời gian để chuyển frame
    float hoverTimer;              // Bộ đếm thời gian cho hiệu ứng lơ lửng
};

#endif