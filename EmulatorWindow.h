#pragma once
#include <stdio.h>
#include <fstream>
#include <SDL2/SDL.h>

class EmulatorWindow {
    public:
        EmulatorWindow();
        ~EmulatorWindow();
        bool handleKeys(unsigned char key[]);
        void updateDisplay(uint32_t display[], int pitch);

    private:
        SDL_Window *window;
        SDL_Renderer* renderer;
        SDL_Texture* texture;
};