#include "mouse.h"
#include "../IO.h"
#include "../BasicRenderer.h"

void MouseWait() {
	uint64_t timeout = 100000;
	while (timeout-- && (inb(0x64) & 0b10) != 0) {}
}
void MouseWaitInput() {
	uint64_t timeout = 100000;
	while (timeout-- && !(inb(0x64) & 0b01)) {}
}
uint8_t MouseRead() {
	MouseWaitInput();
	return inb(0x60);
}
void MouseWrite(uint8_t value) {
	MouseWait();
	outb(0x64, 0xD4);
	MouseWait();
	outb(0x60, value);
}
void InitPS2Mouse() {
	outb(0x64, 0xA8); // Enable the auxiliary device -> mouse
	MouseWait();

	outb(0x64, 0x20); // Tells the keyboard controller that we want to send a command to the mouse
	MouseWait();

	MouseWaitInput();
	uint8_t status = inb(0x60);
	status |= 0b10;
	MouseWait();
	outb(0x64, 0x60);
	MouseWait();
	outb(0x60, status); // Setting the correct bit in the "compaq" status byte

	MouseWrite(0xF6); // Use default settings
	MouseRead();

	MouseWrite(0xF4); // Enable the mouse
	MouseRead();
}
uint8_t MouseCycle = 0;
uint8_t MousePacket[4];
bool MousePacketReady = false;
Point64 MousePosition;
Point64 OldMousePosition;
void HandlePS2Mouse(uint8_t data) {
	static bool skip = true;

	ProcessMousePacket();
	if (skip) {
		skip = false;
		return;
	}

	switch (MouseCycle) {
		case 0:
			if ((data & 0b00001000) == 0) // We are not in sync with the mouse
				break;
			MousePacket[0] = data;
			MouseCycle++;
			break;
		case 1:
			MousePacket[1] = data;
			MouseCycle++;
			break;
		case 2:
			MousePacket[2] = data;
			MousePacketReady = true;
			MouseCycle = 0;
			break;
	}
}
void ProcessMousePacket() {
	if (!MousePacketReady)
		return;
	bool xNegative, yNegative, xOverflow, yOverflow;
	xNegative = MousePacket[0] & PS2XSign;
	yNegative = MousePacket[0] & PS2YSign;
	xOverflow = MousePacket[0] & PS2XOverflow;
	yOverflow = MousePacket[0] & PS2YOverflow;

	if (!xNegative)
	{
		MousePosition.x += MousePacket[1];
		if (xOverflow)
			MousePosition.x += 255;
	} else {
		MousePacket[1] = 256 - MousePacket[1];
		MousePosition.x -= MousePacket[1];
		if (xOverflow)
			MousePosition.x -= 255;
	}

	if (!yNegative)
	{
		MousePosition.y -= MousePacket[2];
		if (yOverflow)
			MousePosition.y -= 255;
	} else {
		MousePacket[2] = 256 - MousePacket[2];
		MousePosition.y += MousePacket[2];
		if (yOverflow)
			MousePosition.y += 255;
	}

	if (MousePosition.x < 0)
		MousePosition.x = 0;
	if (MousePosition.x > Renderer.dimensions.x - 1)
		MousePosition.x = Renderer.dimensions.x - 1;
	if (MousePosition.y < 0)
		MousePosition.y = 0;
	if (MousePosition.y > Renderer.dimensions.y - 1)
		MousePosition.y = Renderer.dimensions.y - 1;

	static bool drawn = false;
	if (drawn)
		Renderer.ClearMouseCursor(OldMousePosition);
	Renderer.DrawOverlayMouseCursor(MousePosition);

	if (MousePacket[0] & LeftButton) {

	}
	if (MousePacket[0] & RightButton) {

	}
	if (MousePacket[0] & MiddleButton) {

	}

	drawn = true;
	MousePacketReady = false;
	OldMousePosition = MousePosition;
}