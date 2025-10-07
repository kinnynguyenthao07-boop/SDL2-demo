#include <SDL2/SDL.h>
#include <iostream>
#include <cmath>

// Hàm vẽ vòng tròn (filled) đơn giản bằng pixel (đủ cho ví dụ)
void DrawFilledCircle(SDL_Renderer* ren, int cx, int cy, int radius) {
    for (int dy = -radius; dy <= radius; ++dy) {
        int dx_limit = (int)std::sqrt(radius*radius - dy*dy);
        for (int dx = -dx_limit; dx <= dx_limit; ++dx) {
            SDL_RenderDrawPoint(ren, cx + dx, cy + dy);
        }
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init lỗi: " << SDL_GetError() << "\n";
        return 1;
    }

    const int WIN_W = 640;
    const int WIN_H = 480;
    SDL_Window* window = SDL_CreateWindow("Nhân vật giơ tay - SDL2",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIN_W, WIN_H, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow lỗi: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* ren = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!ren) {
        std::cerr << "SDL_CreateRenderer lỗi: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Tạo texture cho cánh tay (1 hình chữ nhật đơn màu)
    const int ARM_W = 100;
    const int ARM_H = 18; // độ dày cánh tay
    SDL_Texture* armTex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, ARM_W, ARM_H);
    if (!armTex) {
        std::cerr << "Tạo arm texture lỗi: " << SDL_GetError() << "\n";
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Vẽ nội dung texture (màu da)
    SDL_SetRenderTarget(ren, armTex);
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, 0); // clear trong suốt
    SDL_RenderClear(ren);
    SDL_Rect r = {0, 0, ARM_W, ARM_H};
    SDL_SetRenderDrawColor(ren, 220, 180, 120, 255); // màu da
    SDL_RenderFillRect(ren, &r);
    SDL_SetRenderTarget(ren, NULL);

    bool running = true;
    SDL_Event e;

    // Vị trí nhân vật
    int centerX = WIN_W / 2;
    int baseY = WIN_H / 2 + 60;

    // Góc cánh tay (degree). 90: thẳng xuống; -30: đưa lên
    float angle = 90.0f;
    float angleDir = -1.0f; // hướng tăng/giam
    const float ANGLE_MIN = -30.0f;
    const float ANGLE_MAX = 90.0f;
    const float ANGLE_SPEED = 40.0f; // degree / second

    Uint32 lastTicks = SDL_GetTicks();

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) running = false;
            }
        }

        // delta time
        Uint32 now = SDL_GetTicks();
        float dt = (now - lastTicks) / 1000.0f;
        lastTicks = now;

        // Cập nhật góc (đi lên rồi hạ xuống lặp lại)
        angle += angleDir * ANGLE_SPEED * dt;
        if (angle <= ANGLE_MIN) { angle = ANGLE_MIN; angleDir = 1.0f; }
        if (angle >= ANGLE_MAX) { angle = ANGLE_MAX; angleDir = -1.0f; }

        // vẽ nền
        SDL_SetRenderDrawColor(ren, 135, 206, 235, 255); // sky blue
        SDL_RenderClear(ren);

        // Vẽ "mặt đất"
        SDL_SetRenderDrawColor(ren, 60, 179, 113, 255);
        SDL_Rect ground = {0, baseY + 90, WIN_W, WIN_H - (baseY + 90)};
        SDL_RenderFillRect(ren, &ground);

        // Vẽ thân (simple rectangle)
        SDL_SetRenderDrawColor(ren, 100, 100, 160, 255); // áo
        SDL_Rect body = {centerX - 25, baseY - 20, 50, 80};
        SDL_RenderFillRect(ren, &body);

        // Vẽ đầu (circle)
        DrawFilledCircle(ren, centerX, baseY - 60, 26);
        // Vẽ mắt (đơn giản)
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_Rect eye1 = {centerX - 10, baseY - 70, 5, 5};
        SDL_Rect eye2 = {centerX + 5, baseY - 70, 5, 5};
        SDL_RenderFillRect(ren, &eye1);
        SDL_RenderFillRect(ren, &eye2);

        // Vẽ chân
        SDL_SetRenderDrawColor(ren, 80, 40, 40, 255);
        SDL_Rect legL = {centerX - 20, baseY + 60, 12, 40};
        SDL_Rect legR = {centerX + 8, baseY + 60, 12, 40};
        SDL_RenderFillRect(ren, &legL);
        SDL_RenderFillRect(ren, &legR);

        // Vẽ cánh tay trái (tĩnh)
        SDL_Rect armLeft = {centerX - 25 - 60, baseY + 0, 60, 14};
        SDL_SetRenderDrawColor(ren, 220, 180, 120, 255);
        SDL_RenderFillRect(ren, &armLeft);

        // Vẽ cánh tay phải (dùng texture xoay)
        // Điểm gốc (pivot) cánh tay (gần vai)
        int pivotX = centerX + 25; // vai phải ở cạnh phải thân
        int pivotY = baseY + 0 + 7; // trung tâm chiều cao cánh tay

        // Vị trí renderCopyEx: chúng ta đặt dest rect sao cho pivot nằm tại (pivotX, pivotY)
        // Ta sẽ vẽ texture với origin (0, ARM_H/2) ở góc trái để pivot ở x=0,y=ARM_H/2
        SDL_Rect dest;
        dest.w = ARM_W;
        dest.h = ARM_H;
        // Tính toạ độ trên cửa sổ để pivot của dest (ở x=0,y=ARM_H/2) trùng với pivotX,pivotY:
        dest.x = pivotX - 0;                  // left = pivotX
        dest.y = pivotY - ARM_H / 2;         // top = pivotY - halfHeight

        // Góc SDL_RenderCopyEx: luân chuyển quanh điểm 'center' (xin dùng center = {0, ARM_H/2})
        SDL_Point center = {0, ARM_H / 2};

        // Gán màu nền của arm texture (đã tạo) => chỉ render texture
        SDL_RenderCopyEx(ren, armTex, NULL, &dest, -angle /* dấu trừ để góc thuận intuitive */, &center, SDL_FLIP_NONE);

        // Vẽ "vai" (nhỏ) che phần nối
        SDL_SetRenderDrawColor(ren, 100, 100, 160, 255);
        SDL_Rect shoulder = {pivotX - 6, pivotY - 6, 12, 12};
        SDL_RenderFillRect(ren, &shoulder);

        // Hiển thị
        SDL_RenderPresent(ren);

        // Giới hạn FPS cơ bản (nếu không dùng VSYNC)
        SDL_Delay(10);
    }

    SDL_DestroyTexture(armTex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
