#include "interrupts.h"

__attribute__((interrupt)) void PageFault_Handler(struct interrupt_frame* frame) {
	Renderer.Println("Page Fault detected", YELLOW);
	while (true);
}
