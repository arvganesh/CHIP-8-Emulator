#include "EmulatorWindow.h"

// SDL Constructor
EmulatorWindow::EmulatorWindow() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_UNDEFINED, 
                                            SDL_WINDOWPOS_UNDEFINED, 640, 320, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
}

// SDL Destructor
EmulatorWindow::~EmulatorWindow() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

// Handle input
bool EmulatorWindow::handleKeys(unsigned char key[]) {
    SDL_Event event;
    SDL_KeyCode keyMap[16];

    // first row
    keyMap[0xA] = SDLK_z;
    keyMap[0] = SDLK_x;
    keyMap[0xB] = SDLK_c;
    keyMap[0xF] = SDLK_v;

    // second row
    keyMap[7] = SDLK_a;
    keyMap[8] = SDLK_s;
    keyMap[9] = SDLK_d;
    keyMap[0xE] = SDLK_f;

    // third row
    keyMap[4] = SDLK_q;
    keyMap[5] = SDLK_w;
    keyMap[6] = SDLK_e;
    keyMap[0xD] = SDLK_r;

    // fourth row
    keyMap[1] = SDLK_1;
    keyMap[2] = SDLK_2;
    keyMap[3] = SDLK_3;
    keyMap[0xC] = SDLK_4;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return true;
                break;
            case SDL_KEYDOWN:
                for (int i = 0; i < 16; i++) {
                    if (keyMap[i] == event.key.keysym.sym) {
                        key[i] = 1;
                    }
                }
                break;
            case SDL_KEYUP:
                for (int i = 0; i < 16; i++) {
                    if (keyMap[i] == event.key.keysym.sym) {
                        key[i] = 0;
                    }
                }
                break;
        }
    }
    return false;
}

// Update display based on display array
void EmulatorWindow::updateDisplay(uint32_t display[], int pitch) {
    SDL_UpdateTexture(texture, nullptr, display, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}