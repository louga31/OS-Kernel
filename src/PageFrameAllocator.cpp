#include "PageFrameAllocator.h"

uint64_t freeMemory;
uint64_t reservedMemory;
uint64_t usedMemory;
bool Initialized = false;

void PageFrameAllocator::ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t mMapDescriptorSize) {
	if (Initialized)
		return;
	Initialized = true;

	size_t mMapEntries = mMapSize / mMapDescriptorSize;

	void* largestFreeMemSeg = nullptr;
	size_t largestFreeMemSegSize = 0;

	for (size_t i = 0; i < mMapEntries; i++) {
		auto* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescriptorSize));
		if (desc->type == 7) { // EfiConventionalMemory
			if (desc->numberOfPages * 4096 > largestFreeMemSegSize) {
				largestFreeMemSeg = desc->physicalAddress;
				largestFreeMemSegSize = desc->numberOfPages * 4096;
			}
		}
	}

	size_t memorySize = GetMemorySize(mMap, mMapEntries, mMapDescriptorSize);
	freeMemory = memorySize;
	size_t bitmapSize = memorySize / 4096 / 8 + 1;

	InitBitmap(bitmapSize, largestFreeMemSeg);

	LockPages(&PageBitmap, PageBitmap.Size / 4096 + 1);

	for (size_t i = 0; i < mMapEntries; i++) {
		auto* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescriptorSize));
		if (desc->type != 7) { // not EfiConventionalMemory
			ReservePages(desc->physicalAddress, desc->numberOfPages);
		}
	}
}

void PageFrameAllocator::InitBitmap(size_t bitmapSize, void* bufferAddress) {
	PageBitmap.Size = bitmapSize;
	PageBitmap.Buffer = (uint8_t*)bufferAddress;
	for (size_t i = 0; i < bitmapSize; i++) {
		*(uint8_t*)(PageBitmap.Buffer + i) = 0;
	}
}

void PageFrameAllocator::FreePage(void* address) {
	uint64_t index = (uint64_t)address / 4096;
	if (!PageBitmap[index])
		return;
	PageBitmap.Set(index, false);
	freeMemory += 4096;
	usedMemory -= 4096;
}

void PageFrameAllocator::LockPage(void* address) {
	uint64_t index = (uint64_t)address / 4096;
	if (PageBitmap[index])
		return;
	PageBitmap.Set(index, true);
	freeMemory -= 4096;
	usedMemory += 4096;
}

void PageFrameAllocator::ReservePage(void* address) {
	uint64_t index = (uint64_t)address / 4096;
	if (PageBitmap[index])
		return;
	PageBitmap.Set(index, true);
	freeMemory -= 4096;
	reservedMemory += 4096;
}

void PageFrameAllocator::UnReservePage(void* address) {
	uint64_t index = (uint64_t)address / 4096;
	if (!PageBitmap[index])
		return;
	PageBitmap.Set(index, false);
	freeMemory += 4096;
	reservedMemory -= 4096;
}

void PageFrameAllocator::FreePages(void* address, uint64_t pageCount) {
	for (int i = 0; i < pageCount; ++i) {
		FreePage((void*)((uint64_t)address + (i * 4096)));
	}
}

void PageFrameAllocator::LockPages(void* address, uint64_t pageCount) {
	for (int i = 0; i < pageCount; ++i) {
		LockPage((void*)((uint64_t)address + (i * 4096)));
	}
}

void PageFrameAllocator::ReservePages(void* address, uint64_t pageCount) {
	for (int i = 0; i < pageCount; ++i) {
		ReservePage((void*)((uint64_t)address + (i * 4096)));
	}
}

void PageFrameAllocator::UnReservePages(void* address, uint64_t pageCount) {
	for (int i = 0; i < pageCount; ++i) {
		UnReservePage((void*)((uint64_t)address + (i * 4096)));
	}
}

void* PageFrameAllocator::RequestPage() {
	for (uint64_t index = 0; index < PageBitmap.Size * 8; ++index) {
		if (PageBitmap[index])
			continue;
		LockPage((void*)(index * 4096));
		return (void*)(index * 4096);
	}

	return nullptr; // TODO: Page Frame Swap to File
}

uint64_t PageFrameAllocator::GetFreeRAM() {
	return freeMemory;
}

uint64_t PageFrameAllocator::GetUsedRAM() {
	return usedMemory;
}

uint64_t PageFrameAllocator::GetReservedRAM() {
	return reservedMemory;
}
