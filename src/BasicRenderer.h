#pragma once

#include <math.h>
#include <Framebuffer.h>
#include <SimpleFont.h>
#include <userinput/cursor.h>

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
    return Colors::WHITE;
}

extern uint32_t ClearColor;
extern uint32_t PrintColor;

class BasicRenderer
{
private:
    Point CursorPosition;
    Point CursorLimits;
    Framebuffer* framebuffer;
    PSF1_FONT* psf1_font;

    uint32_t MouseCursorBuffer[CURSOR_WIDTH * CURSOR_HEIGHT];
    uint32_t MouseCursorBufferAfter[CURSOR_WIDTH * CURSOR_HEIGHT];

public:
	Point dimensions;

	BasicRenderer() {}; // Do not use
    BasicRenderer(Framebuffer* framebuffer, PSF1_FONT* psf1_font) : framebuffer(framebuffer), psf1_font(psf1_font), dimensions(framebuffer->PixelsPerScanLine, framebuffer->Height) {};

	inline void SetCursorPosition(uint32_t x, uint32_t y) {
		CursorPosition.x = x;
		CursorPosition.y = y;
	}
	inline void SetCursorLimits(uint32_t x, uint32_t y) {
		CursorLimits.x = x;
		CursorLimits.y = y;
	}

	inline void PutPx(uint32_t x, uint32_t y, uint32_t color) {
		*(uint32_t*)((uint32_t*)framebuffer->BaseAddress + x + (y * framebuffer->PixelsPerScanLine)) = color;
	}
	inline uint32_t GetPx(uint32_t x, uint32_t y) {
		return *(uint32_t*)((uint32_t*)framebuffer->BaseAddress + x + (y * framebuffer->PixelsPerScanLine));
	}

    inline void NextLine() {
	    CursorPosition.x = CursorLimits.x;
	    CursorPosition.y += 16;
    }
	inline void PutChar(char chr, uint32_t color = PrintColor, bool haveBackground = false) {
		PutChar(chr, CursorPosition.x, CursorPosition.y);
		CursorPosition.x += 8;
		if (CursorPosition.x + 8 > framebuffer->Width){
			NextLine();
		}
	}
	void PutChar(char chr, uint32_t xOff, uint32_t yOff, uint32_t color = PrintColor, bool haveBackground = false);
	void ClearChar(uint32_t color = ClearColor);

    void Print(const char* str, uint32_t color = PrintColor, bool haveBackground = false);
    void Println(const char* str, uint32_t color = PrintColor, bool haveBackground = false);

    void Clear(uint32_t color = ClearColor);

    void ClearMouseCursor(Point64 position);
    void DrawOverlayMouseCursor(Point64 position, uint32_t color = PrintColor);
};

extern BasicRenderer Renderer;