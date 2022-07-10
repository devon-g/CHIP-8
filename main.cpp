//
// Created by devon on 5/2/22.
//

#include <SDL2/SDL.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "CHIP8.h"

Uint32 decrement(Uint32 interval, void* param)
{
    uint8_t* val = static_cast<uint8_t*>(param);
    if (*val > 0)
        *val -= 1;
    return interval;
}

int main(int argc, char** argv)
{
    // Init random seed
    srand(time(nullptr));

    // Create a CHIP-8
    CHIP8 c8;

    // CHIP-8 display information
    const int CELL_SIZE = 20;

    const int SCREEN_WIDTH = CELL_SIZE*CHIP8::SCREEN_WIDTH;
    const int SCREEN_HEIGHT = CELL_SIZE*CHIP8::SCREEN_HEIGHT;

    const int N_KEYS = 16;
    const SDL_Scancode VALID_KEYS[] = {
        SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_C,
        SDL_SCANCODE_4, SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_D,
        SDL_SCANCODE_7, SDL_SCANCODE_8, SDL_SCANCODE_9, SDL_SCANCODE_E,
        SDL_SCANCODE_A, SDL_SCANCODE_0, SDL_SCANCODE_B, SDL_SCANCODE_F};

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Event event;
    SDL_TimerID delay_timer;
    SDL_TimerID sound_timer;

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS|SDL_INIT_TIMER) < 0)
        printf("Failed to initialize SDL: %s", SDL_GetError());
    window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    c8.load_rom(argv[1]);

    SDL_bool running = SDL_TRUE;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    // Begin delay and sound timers
    delay_timer = SDL_AddTimer(16, &decrement, &c8.DT);
    sound_timer = SDL_AddTimer(16, &decrement, &c8.ST);


    while (running)
    {
        // Listen for events
        while(SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    running = SDL_FALSE;
                    break;
                default: break;
            }
        }

        const uint8_t* pressed_key = SDL_GetKeyboardState(nullptr);
        bool valid_key_pressed = false;
        for (SDL_Scancode k : VALID_KEYS)
            if (pressed_key[k] == 1)
                valid_key_pressed = true;

        // Interpret CHIP8 program
        if (c8.PC < 0xFFF)
        {
            // Fetch the current instruction
            // (0xHH << 8) | 0xLL = 0xHH00 | 0xLL = 0xHHLL
            int instruction = (c8.memory[c8.PC] << 8) | c8.memory[c8.PC + 1];

            // Increment to next instruction
            c8.PC += 2;

            // Extract chunks of instruction
            uint16_t T = (instruction & 0xF000) >> 12;
            uint16_t X = (instruction & 0x0F00) >> 8;
            uint16_t Y = (instruction & 0x00F0) >> 4;
            uint16_t N = instruction & 0x000F;
            uint16_t NN = instruction & 0x00FF;
            uint16_t NNN = instruction & 0x0FFF;

            // Decode current instruction
            switch (T)
            {
                case 0x0:
                    switch (NN)
                    {
                        case 0xE0: // 00E0 - CLS
                            // Clear the display
                            std::fill(c8.screen, c8.screen + CHIP8::SCREEN_WIDTH*CHIP8::SCREEN_HEIGHT, 0);
                            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                            SDL_RenderClear(renderer);
                            break;
                        case 0xEE: // 00EE - RET
                            // Return from a subroutine
                            c8.PC = c8.stack[c8.SP];
                            if (c8.SP > 0)
                                c8.SP--;
                            break;
                        default: // 0NNN - SYS addr
                            // Ignore 0NNN instruction
                            break;
                    }
                    break;
                case 0x1:
                    // 1NNN - JP addr
                    // Jump to location NNN
                    c8.PC = NNN;
                    break;
                case 0x2:
                    // 2NNN - CALL addr
                    // Call subroutine at NNN
                    if (c8.SP < CHIP8::STACK_SIZE - 1)
                        c8.SP++;
                    c8.stack[c8.SP] = c8.PC;
                    c8.PC = NNN;
                    break;
                case 0x3:
                    // 3XNN - SE VX, byte
                    // Skip next instruction if VX = NN
                    if (c8.V[X] == NN)
                        c8.PC += 2;
                    break;
                case 0x4:
                    // 4XNN - SNE VX, byte
                    // Skip next instruction if VX != NN
                    if (c8.V[X] != NN)
                        c8.PC += 2;
                    break;
                case 0x5:
                    // 5XY0 - SE VX, VY
                    // Skip next instruction if VX = VY
                    if (c8.V[X] == c8.V[Y])
                        c8.PC += 2;
                    break;
                case 0x6:
                    // 6XNN - LD VX, byte
                    // Set VX = NN
                    c8.V[X] = NN;
                    break;
                case 0x7:
                    // 7XNN - ADD VX, byte
                    // Set VX = VX + NN
                    c8.V[X] += NN;
                    break;
                case 0x8:
                    switch (N)
                    {
                        case 0x0: // 8XY0 - LD VX, VY
                            // Set VX = VY
                            c8.V[X] = c8.V[Y];
                            break;
                        case 0x1: // 8XY1 - OR VX, VY
                            // Set VX = VX OR VY
                            // Bitwise or operation
                            c8.V[X] |= c8.V[Y];
                            break;
                        case 0x2: // 8XY2 - AND VX, VY
                            // Set VX = VX AND VY
                            // Bitwise and operation
                            c8.V[X] &= c8.V[Y];
                            break;
                        case 0x3: // 8XY3 - XOR VX, VY
                            // Set VX = VX XOR VY
                            // Bitwise exclusive or operation
                            c8.V[X] ^= c8.V[Y];
                            break;
                        case 0x4: // 8XY4 - ADD VX, VY
                            // Set VX = VX + VY, set VF = carry
                            // If carry VF = 1 else VF = 0
                            // Only store low byte of the result
                            if ((uint16_t)c8.V[X] + (uint16_t)c8.V[Y] > 0xFF)
                                c8.V[0xF] = 1;
                            else
                                c8.V[0xF] = 0;
                            c8.V[X] += c8.V[Y];
                            break;
                        case 0x5: // 8XY5 - SUB VX, VY
                            // If VX > VY, VF = 1 else VF = 0
                            // Set VX = VX - VY, set VF = NOT borrow
                            if (c8.V[X] > c8.V[Y])
                                c8.V[0xF] = 1;
                            else
                                c8.V[0xF] = 0;
                            c8.V[X] -= c8.V[Y];
                            break;
                        case 0x6: // 8XY6 - SHR VX {, VY}
                            // Set VX = VY SHR 1
                            // OR
                            // Set VX = VX SHR 1
                            // If least-significant bit of VX is 1, VF = 1 else VF = 0, then VX >> 1
                            c8.V[X] = c8.V[Y];

                            // Check if least-significant bit will be lost
                            if (c8.V[X] & 0x01)
                                c8.V[0xF] = 1;
                            else
                                c8.V[0xF] = 0;

                            c8.V[X] >>= 1;
                            break;
                        case 0x7: // 8XY7 - SUBN VX, VY
                            // Set VX = VY - VX, set VF = NOT borrow
                            // IF VY > VX, then VF = 1 else VF = 0
                            c8.V[X] = c8.V[Y] - c8.V[X];
                            if (c8.V[Y] > c8.V[X])
                                c8.V[0xF] = 1;
                            else
                                c8.V[0xF] = 0;
                            break;
                        case 0xE: // 8XYE - SHL VX {, VY}
                            // Set VX = VY SHL 1
                            // OR
                            // Set VX = VX SHL 1
                            // If most-significant bit of VX is 1, VF = 1 else VF = 0, then VX << 1
                            c8.V[X] = c8.V[Y];

                            // Check if most-significant bit will be lost
                            if ((c8.V[X] >> 7) & 0x01)
                                c8.V[0xF] = 1;
                            else
                                c8.V[0xF] = 0;

                            c8.V[X] <<= 1;
                            break;
                        default: break;
                    }
                    break;
                case 0x9:
                    // 9XY0 - SNE VX, VY
                    if (c8.V[X] != c8.V[Y])
                        c8.PC += 2;
                    break;
                case 0xA:
                    // ANNN - LD I, addr
                    // Set I = NNN
                    c8.I = NNN;
                    break;
                case 0xB:
                    // BNNN - JP V0, addr
                    c8.PC = NNN + c8.V[0];
                    break;
                case 0xC:
                    // CXNN - RND VX, byte
                    c8.V[X] = (rand() % 256) & NN;
                    break;
                case 0xD:
                    // DXYN - DRW VX, VY, nibble
                    // Reset VF flag
                    c8.V[0xF] = 0;

                    // Get target x and y coordinates and wrap if necessary
                    int x, y;
                    y = c8.V[Y] % 32;

                    // Go through each byte of sprite
                    for (int i = 0; i < N; i++)
                    {
                        // x = c8.V[X] % 64;
                        uint8_t sprite_byte = c8.memory[c8.I + i];
                        // Go through each bit of sprite byte
                        for (int d = 7; d >= 0; d--)
                        {
                            x = (c8.V[X] + (7 - d)) % 64;
                            uint8_t sprite_bit = (sprite_byte >> d) & 0b1;
                            // If screen pixel is on and sprite pixel is on, turn screen pixel off and set VF flag
                            if (c8.screen[CHIP8::SCREEN_WIDTH*y + x] == 1 && sprite_bit == 1)
                            {
                                c8.screen[CHIP8::SCREEN_WIDTH*y + x] = 0;
                                c8.V[0xF] = 1;

                                // Draw the pixel with SDL
                                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                                SDL_Rect pixel {x*CELL_SIZE, y*CELL_SIZE, CELL_SIZE, CELL_SIZE};
                                SDL_RenderFillRect(renderer, &pixel);
                                SDL_RenderPresent(renderer);
                            }
                            // Else if screen pixel is off and sprite pixel is on, turn screen pixel on
                            else if (c8.screen[CHIP8::SCREEN_WIDTH*y + x] == 0 && sprite_bit == 1)
                            {
                                c8.screen[CHIP8::SCREEN_WIDTH * y + x] = 1;
                                // Draw the pixel with SDL
                                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                                SDL_Rect pixel{x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                                SDL_RenderFillRect(renderer, &pixel);
                                SDL_RenderPresent(renderer);
                            }
                        }
                        // If we have reached the bottom of the screen stop drawing
                        if (y == CHIP8::SCREEN_HEIGHT - 1)
                            break;
                        y++;
                    }
                    break;
                case 0xE:
                    switch (NN)
                    {
                        case (0x9E):
                            // EX9E - SKP VX
                            // Skip next instruction if key with the value of VX is pressed
                            if (c8.V[X] == *pressed_key )
                                c8.PC += 2;
                            break;
                        case (0xA1):
                            // EXA1 - SKNP VX
                            // Skip next instruction if key with the value of VX is not pressed
                            if (c8.V[X] != *pressed_key)
                                c8.PC += 2;
                            break;
                        default: break;
                    }
                    break;
                case 0xF:
                    switch (NN)
                    {
                        case 0x07:
                            // LD VX, DT
                            c8.V[X] = c8.DT;
                            break;
                        case 0x0A:
                            // LD VX, K
                            // Wait for key press then save value to VX
                            // If key is being pressed store value in VX
                            if (pressed_key != nullptr)
                                c8.V[X] = *pressed_key;
                            else // If not being pressed repeat this instruction
                                c8.PC -= 2;
                            break;
                        case 0x15:
                            // FX15 - LD DT, VX
                            // Set delay timer to VX
                            c8.DT = c8.V[X];
                            break;
                        case 0x18:
                            // FX18 - LD ST, VX
                            // Set sound timer to VX
                            c8.ST = c8.V[X];
                            break;
                        case 0x1E:
                            // FX1E - ADD I, VX
                            // Set I to I + VX
                            c8.I += c8.V[X];
                            break;
                        case 0x29:
                            // FX29 - LD F, VX
                            // Set I to location of sprite for VX
                            c8.I = c8.memory[CHIP8::FONTSET_START_ADDRESS + c8.V[X]];
                            break;
                        case 0x33:
                            // FX33 - LD B, VX
                            // Store BCD representation of VX in memory locations I, I+1, and I+2
                            c8.memory[c8.I] = (c8.V[X]%1000) / 100;
                            c8.memory[c8.I + 1] = (c8.V[X]%100) / 10;
                            c8.memory[c8.I + 2] = (c8.V[X]%10);
                            break;
                        case 0x55:
                            // FX55 - LD [I], VX
                            // Store registers V0 through VX in memory starting at location I
                            for (int i = 0; i < X; i++)
                                c8.memory[c8.I + i] = c8.V[i];
                            break;
                        case 0x65:
                            // FX65 - LD VX, [I]
                            // Read registers V0 through VX from memory starting at location I
                            for (int i = 0; i < X; i++)
                                c8.V[i] = c8.memory[c8.I + i];
                            break;
                        default: break;
                    }
                    break;
                default: break;
            }
        }
    }

    SDL_RemoveTimer(delay_timer);
    SDL_RemoveTimer(sound_timer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
