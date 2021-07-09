#pragma once

#include "../types.h"
#include "../math.h"

#define LeftButton 0b00000001
#define RightButton 0b00000010
#define MiddleButton 0b00000100

#define PS2XSign 0b00010000
#define PS2YSign 0b00100000
#define PS2XOverflow 0b01000000
#define PS2YOverflow 0b10000000

void InitPS2Mouse();
void HandlePS2Mouse(uint8_t data);
void ProcessMousePacket();
extern Point64 MousePosition;
