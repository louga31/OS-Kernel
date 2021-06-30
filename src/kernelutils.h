#pragma once

#include "Framebuffer.h"
#include "SimpleFont.h"
#include "efiMemory.h"
#include "BasicRenderer.h"
#include "Paging/PageTableManager.h"

struct BootInfo {
	Framebuffer* framebuffer;
	PSF1_FONT* psf1_font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescriptorSize;
};

extern uint64_t _KernelStart; // NOLINT(bugprone-reserved-identifier)
extern uint64_t _KernelEnd; // NOLINT(bugprone-reserved-identifier)

struct KernelInfos {
	PageTableManager* pageTableManager;
	BasicRenderer* renderer;
};

void PrepareMemory(BootInfo* bootInfo);
void CreateRenderer(BootInfo* bootInfo, KernelInfos* kernelInfos);
KernelInfos InitializeKernel(BootInfo* bootInfo);