#include "interrupts.h"
#include <panic.h>
#include <IO.h>
#include <userinput/keyboard.h>
#include <userinput/mouse.h>
#include <scheduling/pit/pit.h>

__attribute__((interrupt)) void PageFault_Handler(interrupt_frame* frame) {
	Panic("Page fault Detected");
	while (true);
}
__attribute__((interrupt)) void DoubleFault_Handler(interrupt_frame* frame) {
	Panic("Double fault Detected");
	while (true);
}
__attribute__((interrupt)) void GeneralProtectionFault_Handler(interrupt_frame* frame) {
	Panic("General Protection fault Detected");
	while (true);
}
__attribute__((interrupt)) void KeyboardInt_Handler(interrupt_frame* frame) {
	uint8_t scancode = inb(0x60);
	HandleKeyboard(scancode);
	PIC_EndMaster();
}
__attribute__((interrupt)) void MouseInt_Handler(struct interrupt_frame* frame) {
	uint8_t data = inb(0x60);
	HandlePS2Mouse(data);
	PIC_EndSlave();
}
__attribute__((interrupt)) void PITInt_Handler(struct interrupt_frame* frame) {
	PIT::Tick();
	PIC_EndMaster();
}

void PIC_EndMaster() {
	outb(PIC1_COMMAND, PIC_EOI);
}
void PIC_EndSlave() {
	outb(PIC2_COMMAND, PIC_EOI);
	outb(PIC1_COMMAND, PIC_EOI);
}
void RemapPIC() {
	uint8_t a1, a2;

	// Save the PICs state
	a1 = inb(PIC1_DATA);
	io_wait();
	a2 = inb(PIC2_DATA);
	io_wait();

	// Initialize the PICs
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();

	// Set the PICs interrupt offsets
	outb(PIC1_DATA, 0x20);
	io_wait();
	outb(PIC2_DATA, 0x28);
	io_wait();

	// Allow the PICs to talk to each others
	outb(PIC1_DATA, 4);
	io_wait();
	outb(PIC2_DATA, 2);
	io_wait();

	// Switch PICs to 8086 mode
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	// Restore the saved mapping
	outb(PIC1_DATA, a1);
	io_wait();
	outb(PIC2_DATA, a2);
}
