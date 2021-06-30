#include "BasicRenderer.h"

void BasicRenderer::PutChar(char chr, unsigned int xOff, unsigned int yOff, unsigned int color) {
    unsigned int* pixPtr = (unsigned int*)framebuffer->BaseAddress;
    char* fontPtr = (char*)psf1_font->glyphBuffer + (chr * psf1_font->psf1_Header->charsize);

    for (unsigned long y = yOff; y < yOff + 16; y++)
    {
        for (unsigned long x = xOff; x < xOff + 8; x++)
        {
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0)
                *(unsigned int*)(pixPtr + x + (y * framebuffer->PixelsPerScanLine)) = color;
            else
                *(unsigned int*)(pixPtr + x + (y * framebuffer->PixelsPerScanLine)) = Colors::BLACK;
        }
        fontPtr++;
    }
}

void BasicRenderer::Print(const char* str, unsigned int color) {
    char* chr = (char*)str;
    while (*chr != 0)
    {
        if (*chr == '\n') {
            CursorPosition.x = 0;
            CursorPosition.y += 16;
            chr++;
            continue;
        }
        
        PutChar(*chr, CursorPosition.x, CursorPosition.y, color);
        CursorPosition.x += 8;
        if (CursorPosition.x + 8 > framebuffer->Width)
        {
            CursorPosition.x = 0;
            CursorPosition.y += 16;
        }
        
        chr++;
    }
}

void BasicRenderer::Println(const char* str, unsigned int color) {
    Print(str, color);
    Print("\n");
}