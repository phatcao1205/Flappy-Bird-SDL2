// game.h
#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include "pipe.h"
#include "bird.h"
#include "sound.h"
#include "background.h"

// Lớp Game quản lý toàn bộ logic của trò chơi
class Game {
public:
    Game();   // Hàm khởi tạo
    ~Game();  // Hàm hủy
    void run();  // Hàm chạy trò chơi

private:
    SDL_Window* window;    // Cửa sổ trò chơi
    SDL_Renderer* renderer;  // Renderer để vẽ lên màn hình
    Bird bird;             // Đối tượng chim
    std::vector<Pipe> pipes;  // Danh sách các ống
    bool running;          // Trạng thái chạy của cửa sổ
    bool gameOver;         // Trạng thái trò chơi (kết thúc hay không)
    Sound sound;           // Đối tượng quản lý âm thanh
    int score;             // Điểm số của người chơi
    std::vector<bool> pipePassed;  // Theo dõi xem chim đã vượt qua ống nào chưa
    SDL_Texture* digitTextures[10];  // Mảng lưu texture của các chữ số 0-9
    Background* background;          // Đối tượng quản lý nền và mặt đất
    PipeManager* pipeManager;        // Đối tượng quản lý ống
    BirdManager* birdManager;        // Đối tượng quản lý chim

    void handleEvents();  // Xử lý các sự kiện (nhấn phím, thoát game, v.v.)
    void update();        // Cập nhật trạng thái trò chơi
    void render();        // Vẽ các thành phần lên màn hình
    void initPipes();     // Khởi tạo các ống ban đầu
    void loadDigitTextures();  // Tải các hình ảnh chữ số
    void renderScore();        // Vẽ điểm số lên màn hình
    void restart();            // Khởi động lại trò chơi
};

#endif