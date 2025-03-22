// bird.cpp
#include "bird.h"
#include "constants.h"

// Hàm khởi tạo BirdManager
BirdManager::BirdManager(SDL_Renderer* renderer) : renderer(renderer), currentFrame(0), frameDelay(100), frameTimer(0) {
    // Tải 3 texture cho các trạng thái vỗ cánh
    const char* birdFiles[3] = {
        "assets/yellowbird-upflap.png",
        "assets/yellowbird-midflap.png",
        "assets/yellowbird-downflap.png"
    };

    for (int i = 0; i < 3; i++) {
        SDL_Surface* surface = IMG_Load(birdFiles[i]);
        if (!surface) {
            SDL_Log("Không thể tải %s: %s", birdFiles[i], IMG_GetError());
            birdTextures[i] = nullptr;
            continue;
        }
        birdTextures[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!birdTextures[i]) {
            SDL_Log("Không thể tạo texture từ %s: %s", birdFiles[i], SDL_GetError());
        }
    }
}

// Hàm hủy, giải phóng tài nguyên
BirdManager::~BirdManager() {
    for (int i = 0; i < 3; i++) {
        if (birdTextures[i]) {
            SDL_DestroyTexture(birdTextures[i]);
        }
    }
}

// Cập nhật trạng thái chim (vị trí, vận tốc, và animation)
void BirdManager::updateBird(Bird& bird) {
    bird.velocity += GRAVITY;  // Áp dụng trọng lực
    bird.rect.y += static_cast<int>(bird.velocity);  // Cập nhật vị trí y

    // Cập nhật animation
    frameTimer += 16;  // Giả sử mỗi frame của game là 16ms (60 FPS)
    if (frameTimer >= frameDelay) {
        frameTimer = 0;
        currentFrame = (currentFrame + 1) % 3;  // Chuyển đổi giữa 3 frame
    }
}

// Vẽ chim
void BirdManager::render(SDL_Renderer* renderer, const Bird& bird) {
    if (birdTextures[currentFrame]) {
        SDL_RenderCopy(renderer, birdTextures[currentFrame], nullptr, &bird.rect);
    }
}