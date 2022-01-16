#include "chip8.h"

// initialize registers, memory, and external variables
void Chip8::initialize() { 
    // Set program variables
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;

    // clear memory
    for (int i = 0; i < 4096; i++) memory[i] = 0;
    for (int i = 0; i < 16; i++) {
        stack[i] = 0;
        V[i] = 0;
        key[i] = 0;
    }
    // clear display
    OP_00E0();

    // Load font
    loadFont();

    // Start timers
    delay_timer = 0;
    sound_timer = 0;
}


// Load font into the first 512 bits of memory
void Chip8::loadFont() {
    const unsigned int FONTSET_SIZE = 80;
    const uint16_t fontset[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    for (int i = 0; i < 80; i++) {
        memory[i] = fontset[i];
    }
}

// Load a rom into memory
void Chip8::loadROM(char* filename) {
    // Open file as a binary input stream, move the file pointer ot the end
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        // Get file size, create buffer to hold data
        std::streampos size = file.tellg();
        char* buffer = new char[size];

        // Fill buffer
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();
        
        // Load ROM contents into memory
        for (int i = 0; i < size; i++) {
            memory[0x200 + i] = buffer[i];
        }

        // Free the buffer
        delete[] buffer;
    }
}

// run one cycle of the emulation
void Chip8::emulateCycle() {
    // FETCH
    // Get opcode, increment pc
    opcode = (memory[pc] << 8) | memory[pc + 1];
    pc += 2;

    // DECODE
    // Get opcode identifiers
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;
    uint8_t N = opcode & 0x000F;
    uint8_t NN = opcode & 0x00FF;
    uint16_t NNN = opcode & 0x0FFF;

    // EXECUTE
    switch (opcode & 0xF000) { 
        case 0x0000:
            switch (opcode) {
                case 0x00E0:
                    OP_00E0();
                    break;
                case 0x00EE:
                    OP_OOEE();
                    break;
            }
            break;
        case 0x1000:
            OP_1NNN(NNN); 
            break;
        case 0x2000:
            OP_2NNN(NNN);
            break;
        case 0x3000:
            OP_3XNN(X, NN);
            break;
        case 0x4000:
            OP_4XNN(X, NN);
            break;
        case 0x5000:
            OP_5XY0(X, Y);
            break;
        case 0x6000:
            OP_6XNN(X, NN);
            break;
        case 0x7000:
            OP_7XNN(X, NN);
            break;
        case 0x8000:
            handle8Ops(X, Y);
            break;
        case 0x9000:
            OP_9XY0(X, Y);
        case 0xA000:
            OP_ANNN(NNN);
            break;
        case 0xB000:
            OP_BNNN(NNN);
            break;
        case 0xC000: 
            OP_CXNN(X, NN);
            break;
        case 0xD000:
            OP_DXYN(X, Y, N);
            break;
        case 0xE000:
            switch (opcode & 0x00FF) {
                case 0x9E:
                    OP_EX9E(X);
                    break;
                case 0xA1:
                    OP_EXA1(X);
                    break;
            }
            break;
        case 0xF000:
            handleFOps(X);
            break;     
        default:
            std::cout << "unrecognizable opcode" << std::endl;
    }

    // handle timers
    if (delay_timer > 0) delay_timer--;
    if (sound_timer > 0) {
        sound_timer--;
        std::cout << "BEEP" << std::endl;
    }
}

// check if the V[F] is on
bool Chip8::flagOn() {
    return V[0xF] == 1;
}

// OP CODES

// clear screen
void Chip8::OP_00E0() { 
    for (int i = 0; i < 64 * 32; i++) {
        display[i] = 0;
    }
}

// return from subroutine
void Chip8::OP_OOEE() {
    sp--;
    pc = stack[sp];
}

// Move PC to address NNN
void Chip8::OP_1NNN(uint16_t NNN) { 
    pc = NNN;
} 

// Call subroutine from mem location NNN
void Chip8::OP_2NNN(uint16_t NNN) {
    stack[sp] = pc;
    sp++;
    pc = NNN;
}

// Skip instruction if VX == NN
void Chip8::OP_3XNN(uint8_t X, uint8_t NN) {
    if (V[X] == NN) pc += 2;
}

// Skip instruction if VX != NN
void Chip8::OP_4XNN(uint8_t X, uint8_t NN) {
    if (V[X] != NN) pc += 2;
}

// Skip instruction if VX == VY
void Chip8::OP_5XY0(uint8_t X, uint8_t Y) {
    if (V[X] == V[Y]) pc += 2;
}

// Set register VX to NN
void Chip8::OP_6XNN(uint8_t X, uint8_t NN) {
    V[X] = NN;
}

// Add value NN to regsiter VX
void Chip8::OP_7XNN(uint8_t X, uint8_t NN) {
    V[X] += NN;
}

// Sets VX to the value of VY
void Chip8::OP_8XY0(uint8_t X, uint8_t Y) {
    V[X] = V[Y];
} 

// Sets VX = VX | VY (bitwise or)
void Chip8::OP_8XY1(uint8_t X, uint8_t Y) {
    V[X] |= V[Y];
}

// Sets VX = VX & VY (bitwise and)
void Chip8::OP_8XY2(uint8_t X, uint8_t Y) {
    V[X] &= V[Y];
}

// Sets VX = VX ^ XY (bitwise xor)
void Chip8::OP_8XY3(uint8_t X, uint8_t Y) {
    V[X] ^= V[Y];
}

// VX += VY, VF set to 1 if there's a carry, 0 otherwise
void Chip8::OP_8XY4(uint8_t X, uint8_t Y) {
    uint16_t sum = V[X] + V[Y];
    if (sum > 255) {
        V[0xF] = 1;
    } else {
        V[0xF] = 0;
    }
    V[X] = sum & 0xFF;
}

// VX = VX - VY, if VX > VY, carry = 1, else carry = 0
void Chip8::OP_8XY5(uint8_t X, uint8_t Y) {
    if (V[X] > V[Y]) {
        V[0xF] = 1;
    } else {
        V[0xF] = 0;
    }

    V[X] -= V[Y];
}

// Stores the least significant bit of VX in VF
// Shifts VX to the right by 1
void Chip8::OP_8XY6(uint8_t X, uint8_t Y) {
    V[0xF] = V[X] & 0x01;
    V[X] >>= 1;
}

// VX = VY - VX, if VY > VX, carry = 1, else carry = 0
void Chip8::OP_8XY7(uint8_t X, uint8_t Y) {
    if (V[Y] > V[X]) {
        V[0xF] = 1;
    } else {
        V[0xF] = 0;
    }
    V[X] = V[Y] - V[X];
}

// Stores the most significant bit of VX in VF
// Shifts VX to the right by 1
void Chip8::OP_8XYE(uint8_t X, uint8_t Y) {
    V[0xF] = (V[X] & 0x80) >> 7;
    V[X] <<= 1;
}

// Skip the next instruction if VX != VY
void Chip8::OP_9XY0(uint8_t X, uint8_t Y) {
    if (V[X] != V[Y]) pc += 2;
}

// Set Index Regsiter to NNN
void Chip8::OP_ANNN(uint16_t NNN) {
    I = NNN;
}

// Sets pc to V0 + NNN
void Chip8::OP_BNNN(uint16_t NNN) {
    pc = V[0] + NNN;
}

// Sets VX to the result of a bitwise and on a random number
void Chip8::OP_CXNN(uint8_t X, uint8_t NN) {
    V[X] = (rand() % 255) && NN;
}


// Draw Sprite at coordinate VX, VY with a width of 8 pixels and height of N pixels
// I value does not change
// VF is set to 1 if any pixels were turned off, else its 0
void Chip8::OP_DXYN(uint8_t X, uint8_t Y, uint8_t N) {
    // Get coords from register
    uint32_t x = V[X] % 64;
    uint32_t y = V[Y] % 32;
    V[0xF] = 0;

    // height of N pixels, width of 8 pixels
    for (int height = 0; height < N; height++) {
        uint16_t curByte = memory[I + height];
        for (int width = 0; width < 8; width++) {
            int index = (y + height) * 64 + (x + width);
            uint32_t curScreenPixel = display[index];
            uint16_t curSpritePixel = curByte & (0x80 >> width); // get the bit at position 'width' from left to right.
            if (curSpritePixel) {
                if (curScreenPixel == 0xFFFFFFFF) { // if both the sprite and the screen are on, set VF to 1
                    V[0xF] = 1;
                }
                display[index] ^= 0xFFFFFFFF;
            }
        }
    }
}

// Skip instruction if key in VX is pressed
void Chip8::OP_EX9E(uint8_t X) {
    if (key[V[X]]) pc += 2;
}

// Skip instruction if key in VX is not pressed
void Chip8::OP_EXA1(uint8_t X) {
    if (!key[V[X]]) pc += 2;
}

// Sets VX to delay timer
void Chip8::OP_FX07(uint8_t X) {
    V[X] = delay_timer;
}

// Wait for a key press, then store it VX
void Chip8::OP_FX0A(uint8_t X) {
    bool foundKey = false;
    for (int i = 0; i < 16; i++) {
        if (key[i]) {
            V[X] = i;
            foundKey = true;
        }
    }
    if (!foundKey) pc -= 2;
}

// Sets delay timer to VX
void Chip8::OP_FX15(uint8_t X) {
    delay_timer = V[X];
}

// Sets the sound timer to VX
void Chip8::OP_FX18(uint8_t X) {
    sound_timer = V[X];
}

// Adds VX to I
void Chip8::OP_FX1E(uint8_t X) {
    I += V[X];
}

// Sets I to the location of the sprite for the character in VX
void Chip8::OP_FX29(uint8_t X) {
    I = V[X] * 5;
}

// Take the number in VX, convert it to three decimal digits
// store digits in memory @ address I
void Chip8::OP_FX33(uint8_t X) {
    uint16_t num = V[X];
   for (int i = 2; i >= 0; i--) {
       memory[I + i] = num % 10;
       num /= 10;
   }
}

// stores values from registers to memory
void Chip8::OP_FX55(uint8_t X) {
    for (int i = 0; i <= X; i++) {
        memory[I + i] = V[i];
    }
}

// stores values from memory to registers
void Chip8::OP_FX65(uint8_t X) {
    for (int i = 0; i <= X; i++) {
        V[i] = memory[I + i];
    }
}

// Performs the relevant math or logic operations
void Chip8::handle8Ops(uint8_t X, uint8_t Y) {
    switch (opcode & 0x000F) {
        case 0x0000:
            OP_8XY0(X, Y);
            break;
        case 0x0001:
            OP_8XY1(X, Y);
            break;
        case 0x0002:
            OP_8XY2(X, Y);
            break;
        case 0x0003:
            OP_8XY3(X, Y);
            break;
        case 0x0004:
            OP_8XY4(X, Y);
            break;
        case 0x0005:
            OP_8XY5(X, Y);
            break;
        case 0x0006:
            OP_8XY6(X, Y);
            break;
        case 0x0007:
            OP_8XY7(X, Y);
            break;
        case 0x000E:
            OP_8XYE(X, Y);
            break;
    }
}

void Chip8::handleFOps(uint8_t X) {
    switch (opcode & 0x00FF) {
        case 0x07:
            OP_FX07(X);
            break;
        case 0x0A:
            OP_FX0A(X);
            break;
        case 0x15:
            OP_FX15(X);
            break;
        case 0x18:
            OP_FX18(X);
            break;
        case 0x1E:
            OP_FX1E(X);
            break;
        case 0x29:
            OP_FX29(X);
            break;
        case 0x33:
            OP_FX33(X);
            break;
        case 0x55:
            OP_FX55(X);
            break;
        case 0x65:
            OP_FX65(X);
            break;
    }
}
