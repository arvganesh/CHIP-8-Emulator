#pragma once
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <iomanip>
class Chip8 {
    public:
        // Display and Keypad
        uint32_t display[64 * 32];
        uint8_t key[16]; // keypad

        // general purpose functions
        void loadROM(char* filename);
        void initialize();
        void emulateCycle();
        bool flagOn();

    private:
        // EMULATOR VARIABLES
        uint8_t memory[4096]; // memory
        uint16_t pc; // program counter
        uint16_t I; // index register
        uint16_t stack[16]; // program stack
        uint16_t sp; // stack pointer
        uint8_t delay_timer; // delay timer
        uint8_t sound_timer; // sound timer
        uint8_t V[16]; // variable registers
        uint16_t opcode; // current opcode

        // Font Loader
        void loadFont(); 

        // OP CODES
        void OP_00E0(); // clear screen
        void OP_OOEE(); // return
        void OP_1NNN(uint16_t NNN); // jump
        void OP_2NNN(uint16_t NNN); // calls subroutine
        void OP_3XNN(uint8_t X, uint8_t NN); // skip instruction 
        void OP_4XNN(uint8_t X, uint8_t NN); // skip instruction
        void OP_5XY0(uint8_t X, uint8_t Y); // skip
        void OP_6XNN(uint8_t X, uint8_t NN); // set register VX = NN
        void OP_7XNN(uint8_t X, uint8_t NN); // add value NN to register VX
        void OP_8XY0(uint8_t X, uint8_t Y); // assign values
        void OP_8XY1(uint8_t X, uint8_t Y); // bitwise or
        void OP_8XY2(uint8_t X, uint8_t Y); // bitwise and
        void OP_8XY3(uint8_t X, uint8_t Y); // bitwise xor
        void OP_8XY4(uint8_t X, uint8_t Y); // add with carry
        void OP_8XY5(uint8_t X, uint8_t Y); // subtract with borrow
        void OP_8XY6(uint8_t X, uint8_t Y); // right shift
        void OP_8XY7(uint8_t X, uint8_t Y); // subtract with borrow
        void OP_8XYE(uint8_t X, uint8_t Y); // left shift
        void OP_9XY0(uint8_t X, uint8_t Y); // skip
        void OP_ANNN(uint16_t NNN); // set index register to NNN
        void OP_BNNN(uint16_t NNN); // jump, add
        void OP_CXNN(uint8_t X, uint8_t NN); // random #
        void OP_DXYN(uint8_t X, uint8_t Y, uint8_t N); // display / draw
        void OP_EX9E(uint8_t X); // skip
        void OP_EXA1(uint8_t X); // skip
        void OP_FX07(uint8_t X); // delay timer
        void OP_FX0A(uint8_t X); // key press
        void OP_FX15(uint8_t X); // delay timer
        void OP_FX18(uint8_t X); // sound timer
        void OP_FX1E(uint8_t X); // add to index regsiter
        void OP_FX29(uint8_t X); // sprite
        void OP_FX33(uint8_t X); // store binary-coded decimal
        void OP_FX55(uint8_t X); // stores registers in memory
        void OP_FX65(uint8_t X); // fills registers
        void handle8Ops(uint8_t X, uint8_t Y); // handle opcodes starting with 8
        void handleFOps(uint8_t X); // handle opcodes starting with F
};
