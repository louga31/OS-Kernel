#include "PageTableManager.h"

#include "PageMapIndexer.h"
#include "PageFrameAllocator.h"

void PageTableManager::MapMemory(void* virtualMemory, void* physicalMemory) {
	PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualMemory);
	PageDirectoryEntry PDE;

	// PageDirectoryPointer
	PDE = PML4->entries[indexer.PDP_i];
	PageTable* PDP;
	if (!PDE.GetFlag(PT_Flag::Present)) {
		PDP = (PageTable*)PageFrameAllocator::RequestPage();
		memset(PDP, 0, 0x1000);
		PDE.SetAddress((uint64_t)PDP >> 12);
		PDE.SetFlag(PT_Flag::Present, true);
		PDE.SetFlag(PT_Flag::ReadWrite, true);
		PML4->entries[indexer.PDP_i] = PDE;
	} else {
		PDP = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
	}

	// PageDirectory
	PDE = PDP->entries[indexer.PD_i];
	PageTable* PD;
	if (!PDE.GetFlag(PT_Flag::Present)) {
		PD = (PageTable*)PageFrameAllocator::RequestPage();
		memset(PD, 0, 0x1000);
		PDE.SetAddress((uint64_t)PD >> 12);
		PDE.SetFlag(PT_Flag::Present, true);
		PDE.SetFlag(PT_Flag::ReadWrite, true);
		PDP->entries[indexer.PD_i] = PDE;
	} else {
		PD = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
	}

	// PageTable
	PDE = PD->entries[indexer.PT_i];
	PageTable* PT;
	if (!PDE.GetFlag(PT_Flag::Present)) {
		PT = (PageTable*)PageFrameAllocator::RequestPage();
		memset(PT, 0, 0x1000);
		PDE.SetAddress((uint64_t)PT >> 12);
		PDE.SetFlag(PT_Flag::Present, true);
		PDE.SetFlag(PT_Flag::ReadWrite, true);
		PD->entries[indexer.PT_i] = PDE;
	} else {
		PT = (PageTable*)((uint64_t)PDE.GetAddress() << 12);
	}

	// Set page
	PDE = PT->entries[indexer.P_i];
	PDE.SetAddress((uint64_t)physicalMemory >> 12);
	PDE.SetFlag(PT_Flag::Present, true);
	PDE.SetFlag(PT_Flag::ReadWrite, true);
	PT->entries[indexer.P_i] = PDE;
}
