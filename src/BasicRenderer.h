#pragma once

#include "math.h"
#include "Framebuffer.h"
#include "SimpleFont.h"

#define BLACK       0xFF000000
#define RED         0xFFFF0000
#define GREEN       0xFF00FF00
#define BLUE        0xFF0000FF
#define YELLOW      (RED | GREEN)
#define MAGENTA     (RED | BLUE)
#define CYAN        (GREEN | BLUE)
#define WHITE       (BLUE | GREEN | RED)

inline unsigned int GetRainbowColor(unsigned int i) {
    switch (i % 7)
    {
        case 0:
            return RED;
        case 1:
            return GREEN;
        case 2:
            return BLUE;
        case 3:
            return YELLOW;
        case 4:
            return MAGENTA;
        case 5:
            return CYAN;
        case 6:
            return WHITE;     
    }
}

class BasicRenderer
{
private:
    Point CursorPosition;
    Framebuffer* framebuffer;
    PSF1_FONT* psf1_font;


public:
    BasicRenderer(Framebuffer* framebuffer, PSF1_FONT* psf1_font) : framebuffer(framebuffer), psf1_font(psf1_font) {};

    void PutChar(char chr, unsigned int xOff, unsigned int yOff, unsigned int color);
    void Print(const char* str, unsigned int color = WHITE);
};
