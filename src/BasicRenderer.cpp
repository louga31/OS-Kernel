#include "BasicRenderer.h"

BasicRenderer Renderer;

void BasicRenderer::SetCursorPosition(uint32_t x, uint32_t y) {
	CursorPosition.x = x;
	CursorPosition.y = y;
}
void BasicRenderer::SetCursorLimits(uint32_t x, uint32_t y) {
	CursorLimits.x = x;
	CursorLimits.y = y;
}
void BasicRenderer::PutPx(uint32_t x, uint32_t y, uint32_t color) {
	auto* pixPtr = (unsigned int*)framebuffer->BaseAddress;
	*(unsigned int*)(pixPtr + x + (y * framebuffer->PixelsPerScanLine)) = color;
}
void BasicRenderer::PutChar(char chr, uint32_t color, bool haveBackground) {
	PutChar(chr, CursorPosition.x, CursorPosition.y);
	CursorPosition.x += 8;
	if (CursorPosition.x + 8 > framebuffer->Width){
		CursorPosition.x = 0;
		CursorPosition.y += 16;
	}
}
void BasicRenderer::PutChar(char chr, uint32_t xOff, uint32_t yOff, uint32_t color, bool haveBackground) {
    auto* pixPtr = (uint32_t*)framebuffer->BaseAddress;
    char* fontPtr = (char*)psf1_font->glyphBuffer + (chr * psf1_font->psf1_Header->charsize);

    for (unsigned long y = yOff; y < yOff + 16; y++)
    {
        for (unsigned long x = xOff; x < xOff + 8; x++)
        {
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0)
	            PutPx(x, y, color);
            else if (haveBackground)
	            PutPx(x, y, Colors::BLACK);
        }
        fontPtr++;
    }
}
void BasicRenderer::ClearChar(uint32_t color) {
	if (CursorPosition.x >= 8) {
		CursorPosition.x -= 8;
	}

	unsigned long xOff = CursorPosition.x;
	unsigned long yOff = CursorPosition.y;

	auto* pixPtr = (uint32_t*)framebuffer->BaseAddress;
	for (unsigned long y = yOff; y < yOff + 16; y++)
	{
		for (unsigned long x = xOff; x < xOff + 8; x++)
		{
			PutPx(x, y, color);
		}
	}
	if (CursorPosition.x < 8) {
		if (CursorPosition.y >= 16){
			CursorPosition.x = framebuffer->Width;
			CursorPosition.y -= 16;
		}
		else{
			CursorPosition.y = 0;
		}
	}
}
void BasicRenderer::Print(const char* str, uint32_t color, bool haveBackground) {
    char* chr = (char*)str;
    while (*chr != 0)
    {
        if (*chr == '\n') {
            CursorPosition.x = CursorLimits.x;
            CursorPosition.y += 16;
            chr++;
            continue;
        }
        
        PutChar(*chr, CursorPosition.x, CursorPosition.y, color, haveBackground);
        CursorPosition.x += 8;
        if (CursorPosition.x + 8 > framebuffer->Width)
        {
            CursorPosition.x = 0;
            CursorPosition.y += 16;
        }
        
        chr++;
    }
}
void BasicRenderer::Println(const char* str, uint32_t color, bool haveBackground) {
    Print(str, color, haveBackground);
    Print("\n");
}
void BasicRenderer::Clear(uint32_t color) {
	uint64_t fbBase = (uint64_t)framebuffer->BaseAddress;
	uint64_t bytesPerScanLine = framebuffer->PixelsPerScanLine * 4;
	uint64_t fbHeight = framebuffer->Height;
	uint64_t fbSize = framebuffer->BufferSize;

	for (int vertical = 0; vertical < fbHeight; ++vertical) {
		uint64_t pixPrtBase = fbBase + (bytesPerScanLine * vertical);
		for (uint32_t* pixPtr = (uint32_t*)pixPrtBase; pixPtr < (uint32_t*)(pixPrtBase + bytesPerScanLine); ++pixPtr) {
			*pixPtr = color;
		}
	}
}
