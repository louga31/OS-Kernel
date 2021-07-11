#include "kernelutils.h"
#include <userinput/IO.h>
#include <PCI/pci.h>
#include <GDT/GDT.h>
#include <memory/heap.h>
#include <memory/Paging/PageFrameAllocator.h>
#include <interrupts/interrupts.h>
#include <userinput/mouse.h>

PageTableManager pageTableManager;
void PrepareMemory(BootInfo* bootInfo) {
	// Initialize the PageFrameAllocator
	uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescriptorSize;
	PageFrameAllocator::ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescriptorSize);

	// Lock the kernel memory
	uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
	uint64_t kernelPages = (uint64_t)kernelSize / 0x1000 + 1;
	PageFrameAllocator::LockPages(&_KernelStart, kernelPages);

	auto* PML4 = (PageTable*)PageFrameAllocator::RequestPage();
	memset(PML4, 0, 0x1000);
	PageTableManager::PML4 = PML4;

	for (uint64_t t = 0; t < GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mMapDescriptorSize); t+= 0x1000) {
		PageTableManager::MapMemory((void*)t, (void*)t);
	}

	auto fbBase = (uint64_t)bootInfo->framebuffer->BaseAddress;
	auto fbSize = (uint64_t)bootInfo->framebuffer->BufferSize + 0x1000;
	PageFrameAllocator::LockPages((void*)fbBase, fbSize / 0x1000 + 1);
	for (uint64_t t = fbBase; t < fbBase + fbSize; t += 0x1000) {
		PageTableManager::MapMemory((void*)t, (void*)t);
	}

	asm("mov %0, %%cr3" : : "r" (PML4));
}
void CreateRenderer(BootInfo* bootInfo) {
	// Create the text renderer
	Renderer = BasicRenderer(bootInfo->framebuffer, bootInfo->psf1_font);
}
IDTR idtr;
void SetIDTGate(void* handler, uint8_t entryOffset, uint8_t type_attr, uint8_t selector) {
	auto* interrupt = (IDTDescEntry*)(idtr.offset + entryOffset * sizeof(IDTDescEntry));
	interrupt->SetOffset((uint64_t)handler);
	interrupt->type_attr = type_attr;
	interrupt->selector = selector;
}
void PrepareInterrupts() {
	idtr.limit = 0x0FFF;
	idtr.offset = (uint64_t)PageFrameAllocator::RequestPage();

	SetIDTGate((void*)PageFault_Handler, 0xE, IDT_TA_InterruptGate, 0x08);
	SetIDTGate((void*)DoubleFault_Handler, 0x8, IDT_TA_InterruptGate, 0x08);
	SetIDTGate((void*)GeneralProtectionFault_Handler, 0xD, IDT_TA_InterruptGate, 0x08);
	SetIDTGate((void*)KeyboardInt_Handler, 0x21, IDT_TA_InterruptGate, 0x08);
	SetIDTGate((void*)MouseInt_Handler, 0x2C, IDT_TA_InterruptGate, 0x08);

	asm("lidt %0" : : "m" (idtr));

	RemapPIC();
}
void PrepareACPI(BootInfo* bootInfo) {
	auto* xsdt = (ACPI::SDTHeader*)(bootInfo->rsdp->XSDTAddress);
	auto mcfg = (ACPI::MCFGHeader*)ACPI::FindTable(xsdt, (char*)"MCFG");

	PCI::EnumeratePCI(mcfg);
}
KernelInfos InitializeKernel(BootInfo* bootInfo) {
	GDTDescriptor gdtDescriptor; // NOLINT(cppcoreguidelines-pro-type-member-init)
	gdtDescriptor.Size = sizeof(GDT) - 1;
	gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
	LoadGDT(&gdtDescriptor);

	KernelInfos kernelInfos; // NOLINT(cppcoreguidelines-pro-type-member-init)

	PrepareMemory(bootInfo);
	InitializeHeap((void*)0x0000100000000000, 0x10);

	CreateRenderer(bootInfo);

	PrepareInterrupts();

	InitPS2Mouse();

	outb(PIC1_DATA, 0b11111001);
	outb(PIC2_DATA, 0b11101111);
	asm ("sti"); // Re-enable the interrupts

	Renderer.Clear(); // Clear Screen
	PrepareACPI(bootInfo);

	//Renderer.Clear(); // Clear Screen

	return kernelInfos;
}
