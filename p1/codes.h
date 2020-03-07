#pragma once

// First 1 bit determines starting point
// Afterwards 1 is dash and 0 is dot
const char codes[] = 
{
    0b11111100, // 0
    0b01111100, // 1
    0b00111100, // 2
    0b00011100, // 3
    0b00001100, // 4
    0b00000100, // 5
    0b00001100, // 6
    0b00011100, // 7
    0b00111100, // 8
    0b01111100, // 9
    0b00000000, // ASCII PADDING
    0b00000000, // ASCII PADDING
    0b00000000, // ASCII PADDING
    0b00000000, // ASCII PADDING
    0b00000000, // ASCII PADDING
    0b00000000, // ASCII PADDING
    0b00000000, // ASCII PADDING
    0b10100000, // A
    0b00011000, // B
    0b10101000, // C
    0b00110000, // D
    0b01000000, // E
    0b00101000, // F
    0b01110000, // G
    0b00001000, // H
    0b00100000, // I
    0b11101000, // J
    0b10110000, // K
    0b00101000, // L
    0b11100000, // M
    0b01100000, // N
    0b11110000, // O
    0b01101000, // P
    0b10111000, // Q
    0b01010000, // R
    0b00010000, // S
    0b11000000, // T
    0b10010000, // U
    0b10001000, // V
    0b11010000, // W
    0b10011000, // X
    0b11011000, // Y
    0b00111000, // Z
};