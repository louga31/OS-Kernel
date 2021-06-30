#include "keyboard.h"

bool isLeftShiftPressed;
bool isRightShiftPressed;
void HandleKeyboard(uint8_t scancode) {
	using namespace AZERTYKeyboard; // Change this to change keyboard configuration

	switch (scancode) {
		case LeftShift:
			isLeftShiftPressed = true;
			return;
		case LeftShift + 0x80:
			isLeftShiftPressed = false;
			return;
		case RightShift:
			isRightShiftPressed = true;
			return;
		case RightShift + 0x80:
			isRightShiftPressed = false;
			return;
		case Enter:
			Renderer.Print("\n");
			return;
		case SpaceBar:
			Renderer.PutChar(' ');
			return;
		case BackSpace:
			Renderer.ClearChar();
			return;
	}
	
	char ascii = Translate(scancode, isLeftShiftPressed | isRightShiftPressed);
	if (ascii != 0)
		Renderer.PutChar(ascii);
}
