#include "EmulatorWindow.h"
#include "chip8.h"
#include <iostream>
#include <string>
#include <chrono>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <ROM> <Delay>\n";
		std::exit(EXIT_FAILURE);
    }

    EmulatorWindow game;
    Chip8 CPU;

    int pitch = sizeof(CPU.display[0]) * 64;

    CPU.initialize();
    CPU.loadROM(argv[1]);

    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    int cycleDelay = std::stoi(argv[2]);

    // Emulation Loop
    while (true) {
        auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (dt > cycleDelay) {
            lastCycleTime = currentTime;
            CPU.emulateCycle();
            game.updateDisplay(CPU.display, pitch);
            // std::cin >> wait;
        }

        if (game.handleKeys(CPU.key)) {
            break;
        }

    }

    return 0;
}