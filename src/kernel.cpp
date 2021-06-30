#include "kernelutils.h"

#include "cstr.h"
#include "Paging/PageFrameAllocator.h"

extern "C" [[noreturn]] void _start(BootInfo* bootInfo) {
	KernelInfos kernelInfos = InitializeKernel(bootInfo);
	PageTableManager* pageTableManager = kernelInfos.pageTableManager;

	Renderer.Println("Kernel Initialized Successfully", Colors::GREEN);

	Renderer.Print("Free Memory: ");
	Renderer.Print(to_string(PageFrameAllocator::GetFreeRAM() / 1024), Colors::MAGENTA);
	Renderer.Print(" KB\n", Colors::MAGENTA);

	Renderer.Print("Used Memory: ");
	Renderer.Print(to_string(PageFrameAllocator::GetUsedRAM() / 1024), Colors::MAGENTA);
	Renderer.Print(" KB\n", Colors::MAGENTA);

	Renderer.Print("Reserved Memory: ");
	Renderer.Print(to_string(PageFrameAllocator::GetReservedRAM() / 1024), Colors::MAGENTA);
	Renderer.Print(" KB\n", Colors::MAGENTA);

	while(true);
}
