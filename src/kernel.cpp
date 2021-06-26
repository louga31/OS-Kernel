#include "Framebuffer.h"
#include "SimpleFont.h"
#include "BasicRenderer.h"
#include "cstr.h"
#include "efiMemory.h"
#include "memutils.h"
#include "Bitmap.h"
#include "PageFrameAllocator.h"

struct BootInfo {
	Framebuffer* framebuffer;
	PSF1_FONT* psf1_font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescriptorSize;
};

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

extern "C" void _start(BootInfo* bootInfo) {
	// Create the text renderer
    BasicRenderer renderer = BasicRenderer(bootInfo->framebuffer, bootInfo->psf1_font);

    // Initialize the PageFrameAllocator
    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mMapDescriptorSize;
	PageFrameAllocator::ReadEFIMemoryMap(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mMapDescriptorSize);

	// Lock the kernel memory
	uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
	uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;
	PageFrameAllocator::LockPages(&_KernelStart, kernelPages);

	renderer.Print("Free Memory: ");
	renderer.Print(to_string(PageFrameAllocator::GetFreeRAM() / 1024), MAGENTA);
	renderer.Print(" KB\n", MAGENTA);

	renderer.Print("Used Memory: ");
	renderer.Print(to_string(PageFrameAllocator::GetUsedRAM() / 1024), MAGENTA);
	renderer.Print(" KB\n", MAGENTA);

	renderer.Print("Reserved Memory: ");
	renderer.Print(to_string(PageFrameAllocator::GetReservedRAM() / 1024), MAGENTA);
	renderer.Print(" KB\n", MAGENTA);

	for (int i = 0; i < 20; ++i) {
		void* address = PageFrameAllocator::RequestPage();
		renderer.Print("AllocatedPageAddress: ");
		renderer.Print(to_hstring((uint64_t)address), YELLOW);
		renderer.Print("\n");
	}

	renderer.Print("Free Memory: ");
	renderer.Print(to_string(PageFrameAllocator::GetFreeRAM() / 1024), MAGENTA);
	renderer.Print(" KB\n", MAGENTA);

	renderer.Print("Used Memory: ");
	renderer.Print(to_string(PageFrameAllocator::GetUsedRAM() / 1024), MAGENTA);
	renderer.Print(" KB\n", MAGENTA);

	renderer.Print("Reserved Memory: ");
	renderer.Print(to_string(PageFrameAllocator::GetReservedRAM() / 1024), MAGENTA);
	renderer.Print(" KB\n", MAGENTA);
}
