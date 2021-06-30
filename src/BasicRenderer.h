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

static uint32_t ClearColor = Colors::BLACK;

class BasicRenderer
{
private:
    Point CursorPosition;
    Point CursorLimits;
    Framebuffer* framebuffer;
    PSF1_FONT* psf1_font;

public:
	BasicRenderer() {}; // Do not use
    BasicRenderer(Framebuffer* framebuffer, PSF1_FONT* psf1_font) : framebuffer(framebuffer), psf1_font(psf1_font) {};

    void SetCursorPosition(uint32_t x, uint32_t y);
    void SetCursorLimits(uint32_t x, uint32_t y);

    void PutPx(uint32_t x, uint32_t y, uint32_t color);

    void PutChar(char chr, uint32_t color = Colors::WHITE, bool haveBackground = false);
    void PutChar(char chr, uint32_t xOff, uint32_t yOff, uint32_t color = Colors::WHITE, bool haveBackground = false);
    void ClearChar(uint32_t color = ClearColor);

    void Print(const char* str, uint32_t color = Colors::WHITE, bool haveBackground = false);
    void Println(const char* str, uint32_t color = Colors::WHITE, bool haveBackground = false);

    void Clear(uint32_t color = ClearColor);
};

extern BasicRenderer Renderer;