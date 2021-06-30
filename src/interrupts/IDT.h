#pragma once

#include "../types.h"

#define IDT_TA_InterruptGate    0b10001110
#define IDT_TA_CallGate         0b10001100
#define IDT_TA_TrapGate         0b10001111

struct IDTDescEntry {
	uint16_t offset0;   // offset bits 0..15
	uint16_t selector;  // a code segment selector in GDT or LDT
	uint8_t ist;        // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
	uint8_t type_attr;  // type and attributes
	uint16_t offset1;   // offset bits 16..31
	uint32_t offset2;   // offset bits 32..63
	uint32_t zero;      // reserved

	void SetOffset(uint64_t offset);
	uint64_t GetOffset();
};

struct IDTR {
	uint16_t limit;
	uint64_t offset;
}__attribute((packed));