// game.cpp
#include "game.h"
#include "constants.h"
#include "bird.h"
#include <cstdlib>
#include <ctime>
#include <SDL2/SDL_ttf.h>
#include <string>

// Hàm khởi tạo trò chơi
Game::Game() {
    SDL_Init(SDL_INIT_VIDEO);  // Khởi tạo SDL
    // Tạo cửa sổ trò chơi
    window = SDL_CreateWindow("Flappy Bird", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    // Tạo renderer để vẽ
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Khởi tạo đối tượng chim (cấp phát động)
    bird = new Bird();
    bird->rect = {100, SCREEN_HEIGHT / 2, 34, 24};  // Kích thước chim (dựa trên Flappy Bird gốc: 34x24)
    bird->velocity = 0;
    bird->angle = 0.0;  // Khởi tạo góc xoay ban đầu
    running = true;
    gameState = MENU;  // Bắt đầu ở trạng thái menu
    score = 0;  // Khởi tạo điểm số
    delayTimer = 0.0f;  // Khởi tạo bộ đếm thời gian delay

    srand(time(0));  // Khởi tạo seed cho số ngẫu nhiên

    // Tải các hình ảnh chữ số
    loadDigitTextures();

    // Khởi tạo background trước khi gọi initPipes()
    background = new Background(renderer);

    // Khởi tạo các ống
    initPipes();

    // Khởi tạo pipe manager và bird manager
    pipeManager = new PipeManager(renderer);
    birdManager = new BirdManager(renderer);

    // Khởi tạo menu
    menu = new Menu(renderer);
}

// Hàm hủy, dọn dẹp tài nguyên
Game::~Game() {
    // Giải phóng các texture chữ số
    for (int i = 0; i < 10; i++) {
        if (digitTextures[i]) {
            SDL_DestroyTexture(digitTextures[i]);
        }
    }
    delete bird;        // Giải phóng đối tượng chim
    delete background;  // Giải phóng background
    delete pipeManager; // Giải phóng pipe manager
    delete birdManager; // Giải phóng bird manager
    delete menu;        // Giải phóng menu
    SDL_DestroyRenderer(renderer);  // Hủy renderer
    SDL_DestroyWindow(window);      // Hủy cửa sổ
    SDL_Quit();                     // Thoát SDL
}

// Tải các hình ảnh chữ số từ 0-9
void Game::loadDigitTextures() {
    for (int i = 0; i < 10; i++) {
        std::string path = "assets/" + std::to_string(i) + ".png";
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            SDL_Log("Không thể tải hình ảnh %s: %s", path.c_str(), IMG_GetError());
            digitTextures[i] = nullptr;
            continue;
        }
        digitTextures[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        if (!digitTextures[i]) {
            SDL_Log("Không thể tạo texture từ %s: %s", path.c_str(), SDL_GetError());
        }
    }
}

// Khởi tạo các ống ban đầu
void Game::initPipes() {
    pipes.clear();
    pipePassed.clear();
    int baseHeight = background->getBaseHeight();
    int maxPipeHeight = SCREEN_HEIGHT - baseHeight - PIPE_GAP - MIN_BOTTOM_PIPE_HEIGHT - 50;  // Đảm bảo ống dưới có chiều cao tối thiểu
    for (int i = 0; i < 3; i++) {
        // Tạo chiều cao ngẫu nhiên cho ống, đảm bảo ống dưới có chiều cao tối thiểu
        int height = rand() % (maxPipeHeight - 50 + 1) + 50;
        // Thêm ống vào danh sách, cách nhau 300 pixel
        pipes.push_back({SCREEN_WIDTH + i * 300, height});
        pipePassed.push_back(false);  // Khởi tạo trạng thái vượt qua
    }
}

// Khởi động lại trò chơi
void Game::restart() {
    // Đặt lại vị trí, vận tốc và góc xoay của chim
    bird->rect = {100, SCREEN_HEIGHT / 2, 34, 24};
    bird->velocity = 0;
    bird->angle = 0.0;  // Đặt lại góc xoay

    // Đặt lại trạng thái game
    gameState = MENU;  // Quay lại trạng thái menu
    score = 0;
    delayTimer = 0.0f;  // Đặt lại bộ đếm thời gian delay

    // Khởi tạo lại các ống
    initPipes();
}

// Xử lý các sự kiện như nhấn phím hoặc thoát game
void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;  // Thoát game nếu nhấn nút thoát
        }
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_SPACE) {
                if (gameState == MENU) {
                    gameState = STARTING;  // Chuyển sang trạng thái delay (STARTING)
                    delayTimer = 0.0f;     // Đặt lại bộ đếm thời gian delay
                    sound.playWingSound(); // Phát âm thanh khi nhảy
                } else if (gameState == PLAYING) {
                    bird->velocity = JUMP_STRENGTH;  // Nhảy khi nhấn phím Space
                    sound.playWingSound();           // Phát âm thanh khi nhảy
                }
            }
            if (event.key.keysym.sym == SDLK_r && gameState == GAME_OVER) {
                restart();  // Khởi động lại game khi nhấn phím R ở trạng thái game over
            }
        }
    }
}

// Cập nhật trạng thái trò chơi
void Game::update() {
    // Cập nhật vị trí của background và base trong cả MENU, STARTING và PLAYING
    if (gameState != GAME_OVER) {
        background->update();
    }

    if (gameState == MENU) {
        // Ở trạng thái menu, chỉ cập nhật animation và hiệu ứng lơ lửng của chim
        birdManager->updateBird(*bird, gameState);
        return;
    }

    if (gameState == STARTING) {
        // Ở trạng thái STARTING, tiếp tục hiệu ứng lơ lửng cho chim
        birdManager->updateBird(*bird, MENU);  // Sử dụng logic của MENU để giữ chim lơ lửng

        // Cập nhật vị trí các ống
        for (size_t i = 0; i < pipes.size(); i++) {
            auto& pipe = pipes[i];
            pipe.x -= PIPE_SPEED;  // Di chuyển ống sang trái
            if (pipe.x + PIPE_WIDTH < 0) {  // Nếu ống ra khỏi màn hình
                pipe.x = SCREEN_WIDTH;  // Đặt lại vị trí ở bên phải
                int baseHeight = background->getBaseHeight();
                int maxPipeHeight = SCREEN_HEIGHT - baseHeight - PIPE_GAP - MIN_BOTTOM_PIPE_HEIGHT - 50;
                pipe.height = rand() % (maxPipeHeight - 50 + 1) + 50;  // Tạo chiều cao ngẫu nhiên
                pipePassed[i] = false;  // Đặt lại trạng thái vượt qua
            }
        }

        // Tăng bộ đếm thời gian delay
        delayTimer += 0.016f;  // 16ms mỗi frame (60 FPS)
        if (delayTimer >= 0.40f) {  // Sau 0,25 giây
            gameState = PLAYING;   // Chuyển sang trạng thái PLAYING
        }
        return;
    }

    if (gameState == GAME_OVER) {
        return;  // Không cập nhật nếu trò chơi đã kết thúc
    }

    birdManager->updateBird(*bird, gameState);  // Cập nhật vị trí, animation và góc xoay của chim

    // Cập nhật vị trí các ống
    for (size_t i = 0; i < pipes.size(); i++) {
        auto& pipe = pipes[i];
        pipe.x -= PIPE_SPEED;  // Di chuyển ống sang trái
        if (pipe.x + PIPE_WIDTH < 0) {  // Nếu ống ra khỏi màn hình
            pipe.x = SCREEN_WIDTH;  // Đặt lại vị trí ở bên phải
            int baseHeight = background->getBaseHeight();
            int maxPipeHeight = SCREEN_HEIGHT - baseHeight - PIPE_GAP - MIN_BOTTOM_PIPE_HEIGHT - 50;
            pipe.height = rand() % (maxPipeHeight - 50 + 1) + 50;  // Tạo chiều cao ngẫu nhiên
            pipePassed[i] = false;  // Đặt lại trạng thái vượt qua
        }

        // Kiểm tra nếu chim vượt qua ống
        if (!pipePassed[i] && bird->rect.x > pipe.x + PIPE_WIDTH) {
            pipePassed[i] = true;  // Đánh dấu đã vượt qua
            score++;               // Tăng điểm
            sound.playPointSound();  // Phát âm thanh ghi điểm
        }

        // Kiểm tra va chạm giữa chim và ống
        if (pipeManager->checkCollision(bird->rect.x, bird->rect.y, bird->rect.w, bird->rect.h, pipe.x, pipe.height)) {
            sound.playHitSound();  // Phát âm thanh va chạm
            sound.playDieSound();  // Phát âm thanh thua
            gameState = GAME_OVER;  // Chuyển sang trạng thái game over
        }
    }

    // Kết thúc game nếu chim chạm đỉnh hoặc chạm base
    int baseHeight = background->getBaseHeight();
    if (bird->rect.y + bird->rect.h > SCREEN_HEIGHT - baseHeight) {  // Chim chạm base
        bird->rect.y = SCREEN_HEIGHT - baseHeight - bird->rect.h;  // Đặt chim nằm trên base
        bird->velocity = 0;  // Dừng vận tốc
        sound.playHitSound();  // Phát âm thanh va chạm
        sound.playDieSound();  // Phát âm thanh thua
        gameState = GAME_OVER;  // Chuyển sang trạng thái game over
    }
    if (bird->rect.y < 0) {  // Chim chạm đỉnh màn hình
        sound.playHitSound();  // Phát âm thanh va chạm
        sound.playDieSound();  // Phát âm thanh thua
        gameState = GAME_OVER;  // Chuyển sang trạng thái game over
    }
}

// Vẽ điểm số lên màn hình
void Game::renderScore() {
    // Chuyển điểm số thành chuỗi để lấy từng chữ số
    std::string scoreStr = std::to_string(score);
    int digitWidth = 30;  // Chiều rộng của mỗi chữ số
    int digitHeight = 50; // Chiều cao của mỗi chữ số
    int totalWidth = scoreStr.length() * digitWidth;  // Tổng chiều rộng của điểm số
    int startX = (SCREEN_WIDTH - totalWidth) / 2;    // Tọa độ x để căn giữa
    int startY = 50;                                 // Tọa độ y (gần đỉnh màn hình)

    // Vẽ từng chữ số
    for (size_t i = 0; i < scoreStr.length(); i++) {
        int digit = scoreStr[i] - '0';  // Chuyển ký tự thành số (0-9)
        if (digitTextures[digit]) {
            SDL_Rect digitRect = {static_cast<int>(startX + i * digitWidth), startY, digitWidth, digitHeight};
            SDL_RenderCopy(renderer, digitTextures[digit], nullptr, &digitRect);
        }
    }
}

// Vẽ các thành phần lên màn hình
void Game::render() {
    // Vẽ nền và mặt đất
    background->render();

    // Vẽ chim bằng birdManager
    birdManager->render(renderer, *bird);

    if (gameState == MENU) {
        // Ở trạng thái menu, chỉ vẽ menu
        menu->render();
    } else if (gameState == STARTING || gameState == PLAYING) {
        // Ở trạng thái STARTING hoặc PLAYING, vẽ các ống và điểm số
        int baseHeight = background->getBaseHeight();
        for (const auto& pipe : pipes) {
            pipeManager->render(renderer, pipe, baseHeight);
        }
        renderScore();
    } else if (gameState == GAME_OVER) {
        // Ở trạng thái game over, vẽ các ống, điểm số và thông báo "Game Over"
        int baseHeight = background->getBaseHeight();
        for (const auto& pipe : pipes) {
            pipeManager->render(renderer, pipe, baseHeight);
        }
        renderScore();

        // Hiển thị thông báo "Game Over"
        TTF_Init();
        TTF_Font* font = TTF_OpenFont("arial.ttf", 48);  // Cần có file font
        if (font) {
            SDL_Color textColor = {255, 0, 0, 255};  // Màu đỏ
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Game Over", textColor);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 24, 200, 48};
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

            // Giải phóng tài nguyên
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);
            TTF_CloseFont(font);
        }
        TTF_Quit();
    }

    SDL_RenderPresent(renderer);  // Hiển thị khung hình
}

// Hàm chạy vòng lặp chính của trò chơi
void Game::run() {
    while (running) {
        handleEvents();  // Xử lý sự kiện
        update();        // Cập nhật trạng thái
        render();        // Vẽ khung hình
        SDL_Delay(16);   // Tạm dừng 16ms để đạt khoảng 60 FPS
    }
}