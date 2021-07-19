#include "PageFrameAllocator.h"

uint64_t freeMemory;
uint64_t reservedMemory;
uint64_t usedMemory;
uint64_t pageBitmapIndex = 0;

void PageFrameAllocator::ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t mMapDescriptorSize) {
	static bool Initialized = false;
	if (Initialized)
		return;
	Initialized = true;

	size_t mMapEntries = mMapSize / mMapDescriptorSize;

	void* largestFreeMemSeg = nullptr;
	size_t largestFreeMemSegSize = 0;

	for (size_t i = 0; i < mMapEntries; i++) {
		auto* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescriptorSize));
		if (desc->type == 7) { // EfiConventionalMemory
			if (desc->numberOfPages * 0x1000 > largestFreeMemSegSize) {
				largestFreeMemSeg = desc->physicalAddress;
				largestFreeMemSegSize = desc->numberOfPages * 0x1000;
			}
		}
	}

	size_t memorySize = GetMemorySize(mMap, mMapEntries, mMapDescriptorSize);
	freeMemory = memorySize;
	size_t bitmapSize = memorySize / 0x1000 / 8 + 1;

	InitBitmap(bitmapSize, largestFreeMemSeg);

	ReservePages(0, memorySize / 0x1000);

	for (size_t i = 0; i < mMapEntries; i++) {
		auto* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescriptorSize));
		if (desc->type == 7) { // EfiConventionalMemory
			UnReservePages(desc->physicalAddress, desc->numberOfPages);
		}
	}
	ReservePages(0, 0x100000 / 0x1000); // Reserve between 0 and 0x100000
	LockPages(PageBitmap.Buffer, PageBitmap.Size / 0x1000 + 1);
}
void PageFrameAllocator::InitBitmap(size_t bitmapSize, void* bufferAddress) {
	PageBitmap.Size = bitmapSize;
	PageBitmap.Buffer = (uint8_t*)bufferAddress;
	for (size_t i = 0; i < bitmapSize; i++) {
		*(uint8_t*)(PageBitmap.Buffer + i) = 0;
	}
}
void PageFrameAllocator::LockPage(void* address) {
	uint64_t index = (uint64_t)address / 0x1000;
	if (PageBitmap[index])
		return;
	if(PageBitmap.Set(index, true)) {
		freeMemory -= 0x1000;
		usedMemory += 0x1000;
	}
}
void PageFrameAllocator::FreePage(void* address) {
	uint64_t index = (uint64_t)address / 0x1000;
	if (!PageBitmap[index])
		return;
	if(PageBitmap.Set(index, false)) {
		freeMemory += 0x1000;
		usedMemory -= 0x1000;
		if (pageBitmapIndex > index)
			pageBitmapIndex = index;
	}
}
void PageFrameAllocator::ReservePage(void* address) {
	uint64_t index = (uint64_t)address / 0x1000;
	if (PageBitmap[index])
		return;
	if(PageBitmap.Set(index, true)) {
		freeMemory -= 0x1000;
		reservedMemory += 0x1000;
	}
}
void PageFrameAllocator::UnReservePage(void* address) {
	uint64_t index = (uint64_t)address / 0x1000;
	if (!PageBitmap[index])
		return;
	if(PageBitmap.Set(index, false)) {
		freeMemory += 0x1000;
		reservedMemory -= 0x1000;
		if (pageBitmapIndex > index)
			pageBitmapIndex = index;
	}
}
void PageFrameAllocator::LockPages(void* address, uint64_t pageCount) {
	for (int i = 0; i < pageCount; ++i) {
		LockPage((void*)((uint64_t)address + (i * 0x1000)));
	}
}
void PageFrameAllocator::FreePages(void* address, uint64_t pageCount) {
	for (int i = 0; i < pageCount; ++i) {
		FreePage((void*)((uint64_t)address + (i * 0x1000)));
	}
}
void PageFrameAllocator::ReservePages(void* address, uint64_t pageCount) {
	for (int i = 0; i < pageCount; ++i) {
		ReservePage((void*)((uint64_t)address + (i * 0x1000)));
	}
}
void PageFrameAllocator::UnReservePages(void* address, uint64_t pageCount) {
	for (int i = 0; i < pageCount; ++i) {
		UnReservePage((void*)((uint64_t)address + (i * 0x1000)));
	}
}
void* PageFrameAllocator::RequestPage() {
	for (; pageBitmapIndex < PageBitmap.Size * 8; ++pageBitmapIndex) {
		if (PageBitmap[pageBitmapIndex])
			continue;
		LockPage((void*)(pageBitmapIndex * 0x1000));
		return (void*)(pageBitmapIndex * 0x1000);
	}

	return nullptr; // TODO: Page Frame Swap to File
}
void* PageFrameAllocator::RequestPages(uint64_t pageCount) {
	for (; pageBitmapIndex < PageBitmap.Size * 8; ++pageBitmapIndex) {
		bool skip = false;
		for (int i = 0; i < pageCount; ++i) {
			if (PageBitmap[pageBitmapIndex + i]){
				skip = true;
				continue;
			}
		}
		if (skip)
			continue;
		for (int i = 0; i < pageCount; ++i) {
			LockPage((void*)((pageBitmapIndex + i) * 0x1000));
		}
		return (void*)(pageBitmapIndex * 0x1000);
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
