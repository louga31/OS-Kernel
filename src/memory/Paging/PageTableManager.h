#pragma once

#include "paging.h"

class PageTableManager {
public:
	inline static PageTable* PML4 = nullptr;

	static void MapMemory(void* virtualMemory, void* physicalMemory);
};
