#pragma once

#include <types.h>

struct PSF1_HEADER {
	byte magic[2];
	byte mode;
	byte charsize;
};

struct PSF1_FONT {
	PSF1_HEADER* psf1_Header;
	void* glyphBuffer;
};