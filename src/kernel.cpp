#include "kernelutils.h"

#include "BasicRenderer.h"
#include "cstr.h"
#include "Paging/PageFrameAllocator.h"

extern "C" [[noreturn]] void _start(BootInfo* bootInfo) {
	KernelInfos kernelInfos = InitializeKernel(bootInfo);
	PageTableManager* pageTableManager = kernelInfos.pageTableManager;
	BasicRenderer* renderer = kernelInfos.renderer;

	renderer->Println("Kernel Initialized Successfully", Colors::GREEN);

	renderer->Print("Free Memory: ");
	renderer->Print(to_string(PageFrameAllocator::GetFreeRAM() / 1024), Colors::MAGENTA);
	renderer->Print(" KB\n", Colors::MAGENTA);

	renderer->Print("Used Memory: ");
	renderer->Print(to_string(PageFrameAllocator::GetUsedRAM() / 1024), Colors::MAGENTA);
	renderer->Print(" KB\n", Colors::MAGENTA);

	renderer->Print("Reserved Memory: ");
	renderer->Print(to_string(PageFrameAllocator::GetReservedRAM() / 1024), Colors::MAGENTA);
	renderer->Print(" KB\n", Colors::MAGENTA);

	while(true);
}
