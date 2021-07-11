#include "kernelutils.h"

#include <cstr.h>
#include <memory/heap.h>
#include <memory/Paging/PageFrameAllocator.h>
#include <scheduling/pit/pit.h>

extern "C" [[noreturn]] void _start(BootInfo* bootInfo) {
	KernelInfos kernelInfos = InitializeKernel(bootInfo);

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

	Renderer.Println(to_hstring((uint64_t)malloc(0x8000)));
	void* address = malloc(0x8000);
	Renderer.Println(to_hstring((uint64_t)address));
	Renderer.Println(to_hstring((uint64_t)malloc(0x100)));
	free(address);

	Renderer.Println(to_hstring((uint64_t)malloc(0x100)));

	for (uint64_t t = 0; t < 600; ++t) {
		Renderer.Print(to_string(t));
		Renderer.Print(" ");
		PIT::Sleep(10);
	}

	while(true);
}
