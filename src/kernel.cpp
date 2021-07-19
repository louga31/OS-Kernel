#include "kernelutils.h"

#include <cstr.h>
#include <memory/Paging/PageFrameAllocator.h>
#include <scheduling/pit/pit.h>
#include <ahci/ahci.h>
#include <filesystem/fat.h>

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

	auto* buffer = (char*)PageFrameAllocator::RequestPage();
	memset(buffer, 0, 0x1000);
	AHCI::AHCIDriver* ahci = PCI::AHCIDrivers[0];
	ahci->Read(0, 4, 0, buffer);
	FAT::InitializeFAT((FAT::FATBootSector*)buffer);

//	for (uint64_t t = 0; t < 600; ++t) {
//		Renderer.Print(to_string(t));
//		Renderer.Print(" ");
//		PIT::Sleep(10);
//	}
	while(true) {
		asm ("hlt");
	}
}
