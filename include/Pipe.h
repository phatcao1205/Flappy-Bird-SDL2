// pipe.h
#ifndef PIPE_H
#define PIPE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

// Cấu trúc Pipe đại diện cho một cặp ống (trên và dưới)
struct Pipe {
    int x, height;  // Tọa độ x và chiều cao của ống trên
};

// Lớp PipeManager quản lý texture và logic của ống
class PipeManager {
public:
    PipeManager(SDL_Renderer* renderer);  // Hàm khởi tạo
    ~PipeManager();                       // Hàm hủy
    bool checkCollision(int bx, int by, int bw, int bh, int px, int ph);  // Kiểm tra va chạm
    void render(SDL_Renderer* renderer, const Pipe& pipe, int baseHeight);  // Vẽ ống, truyền thêm baseHeight

private:
    SDL_Texture* pipeTexture;  // Texture của pipe-green.png
    int pipeWidth;             // Chiều rộng của ống (lấy từ texture)
};

#endif