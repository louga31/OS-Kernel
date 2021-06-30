#include "kernelutils.h"
#include "IO.h"
#include "GDT/GDT.h"
#include "Paging/PageFrameAllocator.h"
#include "interrupts/interrupts.h"

PageTableManager pageTableManager; // TODO: Malloc this when possible
void PrepareMemory(BootInfo* bootInfo, KernelInfos* kernelInfos) {
	// Initialize the PageFrameAllocator
	uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescriptorSize;
	PageFrameAllocator::ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescriptorSize);

	// Lock the kernel memory
	uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
	uint64_t kernelPages = (uint64_t)kernelSize / 0x1000 + 1;
	PageFrameAllocator::LockPages(&_KernelStart, kernelPages);

	auto* PML4 = (PageTable*)PageFrameAllocator::RequestPage();
	memset(PML4, 0, 0x1000);
	pageTableManager = PageTableManager(PML4);

	for (uint64_t t = 0; t < GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mMapDescriptorSize); t+= 0x1000) {
		pageTableManager.MapMemory((void*)t, (void*)t);
	}

	auto fbBase = (uint64_t)bootInfo->framebuffer->BaseAddress;
	auto fbSize = (uint64_t)bootInfo->framebuffer->BufferSize + 0x1000;
	PageFrameAllocator::LockPages((void*)fbBase, fbSize / 0x1000 + 1);
	for (uint64_t t = fbBase; t < fbBase + fbSize; t += 0x1000) {
		pageTableManager.MapMemory((void*)t, (void*)t);
	}

	asm("mov %0, %%cr3" : : "r" (PML4));
	kernelInfos->pageTableManager = &pageTableManager;
}
void CreateRenderer(BootInfo* bootInfo) {
	// Create the text renderer
	Renderer = BasicRenderer(bootInfo->framebuffer, bootInfo->psf1_font);
}
IDTR idtr; // TODO: Malloc this when possible
void PrepareInterrupts() {
	idtr.limit = 0x0FFF;
	idtr.offset = (uint64_t)PageFrameAllocator::RequestPage();

	auto* int_PageFault = (IDTDescEntry*)(idtr.offset + 0xE * sizeof(IDTDescEntry));
	int_PageFault->SetOffset((uint64_t)PageFault_Handler);
	int_PageFault->type_attr = IDT_TA_InterruptGate;
	int_PageFault->selector = 0x08;

	auto* int_DoubleFault = (IDTDescEntry*)(idtr.offset + 0x8 * sizeof(IDTDescEntry));
	int_DoubleFault->SetOffset((uint64_t)DoubleFault_Handler);
	int_DoubleFault->type_attr = IDT_TA_InterruptGate;
	int_DoubleFault->selector = 0x08;

	auto* int_GeneralProtectionFault = (IDTDescEntry*)(idtr.offset + 0xD * sizeof(IDTDescEntry));
	int_GeneralProtectionFault->SetOffset((uint64_t)GeneralProtectionFault_Handler);
	int_GeneralProtectionFault->type_attr = IDT_TA_InterruptGate;
	int_GeneralProtectionFault->selector = 0x08;

	auto* int_Keyboard = (IDTDescEntry*)(idtr.offset + 0x21 * sizeof(IDTDescEntry));
	int_Keyboard->SetOffset((uint64_t)KeyboardInt_Handler);
	int_Keyboard->type_attr = IDT_TA_InterruptGate;
	int_Keyboard->selector = 0x08;

	asm("lidt %0" : : "m" (idtr));

	RemapPIC();

	outb(PIC1_DATA, 0b11111101);
	outb(PIC2_DATA, 0b11111111);
	asm ("sti");
}
KernelInfos InitializeKernel(BootInfo* bootInfo) { // TODO: Malloc KernelInfos when possible
	GDTDescriptor gdtDescriptor; // NOLINT(cppcoreguidelines-pro-type-member-init)
	gdtDescriptor.Size = sizeof(GDT) - 1;
	gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
	LoadGDT(&gdtDescriptor);

	KernelInfos kernelInfos; // NOLINT(cppcoreguidelines-pro-type-member-init)
	PrepareMemory(bootInfo, &kernelInfos);
	CreateRenderer(bootInfo);

	PrepareInterrupts();

	Renderer.Clear(); // Clear Screen
	return kernelInfos;
}
