#pragma once

#include "paging.h"

class PageTableManager {
public:
	PageTable* PML4;

	PageTableManager() {}; // Do not use
	PageTableManager(PageTable* PML4Address) : PML4(PML4Address) {};

	void MapMemory(void* virtualMemory, void* physicalMemory);
};
