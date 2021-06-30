#pragma once

#include "math.h"
#include "Framebuffer.h"
#include "SimpleFont.h"

enum Colors {
	BLACK       = 0xFF000000,
	RED         = 0xFFFF0000,
	GREEN       = 0xFF00FF00,
	BLUE        = 0xFF0000FF,
	YELLOW      = RED | GREEN,
	MAGENTA     = RED | BLUE,
	CYAN        = GREEN | BLUE,
	WHITE       = BLUE | GREEN | RED
};

inline unsigned int GetRainbowColor(unsigned int i) {
    switch (i % 7)
    {
        case 0:
            return Colors::RED;
        case 1:
            return Colors::GREEN;
        case 2:
            return Colors::BLUE;
        case 3:
            return Colors::YELLOW;
        case 4:
            return Colors::MAGENTA;
        case 5:
            return Colors::CYAN;
        case 6:
            return Colors::WHITE;
    }
}

class BasicRenderer
{
private:
    Point CursorPosition;
    Framebuffer* framebuffer;
    PSF1_FONT* psf1_font;

public:
	BasicRenderer() {}; // Do not use
    BasicRenderer(Framebuffer* framebuffer, PSF1_FONT* psf1_font) : framebuffer(framebuffer), psf1_font(psf1_font) {};

    void PutChar(char chr, unsigned int xOff, unsigned int yOff, unsigned int color);
    void Print(const char* str, unsigned int color = Colors::WHITE);
    void Println(const char* str, unsigned int color = Colors::WHITE);
};
