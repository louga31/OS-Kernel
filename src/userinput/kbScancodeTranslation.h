#pragma once

#include "../types.h"

namespace QWERTYKeyboard {
	const uint8_t LeftShift = 0x2A;
	const uint8_t RightShift = 0x36;
	const uint8_t Enter = 0x1C;
	const uint8_t BackSpace = 0x0E;
	const uint8_t SpaceBar = 0x39;

	extern const char ASCIITable[];
	char Translate(uint8_t scancode, bool uppercase);
}

namespace AZERTYKeyboard {
	const uint8_t LeftShift = 0x2A;
	const uint8_t RightShift = 0x36;
	const uint8_t Enter = 0x1C;
	const uint8_t BackSpace = 0x0E;
	const uint8_t SpaceBar = 0x39;

	extern const char ASCIITable[];
	char Translate(uint8_t scancode, bool uppercase);
}