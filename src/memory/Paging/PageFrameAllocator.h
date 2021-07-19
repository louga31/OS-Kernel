#pragma once

#include <types.h>
#include <Bitmap.h>
#include <memory/efiMemory.h>
#include <memory/memutils.h>

class PageFrameAllocator
{
private:
	inline static Bitmap PageBitmap = Bitmap();
	static void InitBitmap(size_t bitmapSize, void* bufferAddress);

	static void ReservePage(void* address);
	static void ReservePages(void* address, uint64_t pageCount);
	static void UnReservePage(void* address);
	static void UnReservePages(void* address, uint64_t pageCount);

public:
	static void ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t mMapDescriptorSize);

	static void FreePage(void* address);
	static void FreePages(void* address, uint64_t pageCount);
	static void LockPage(void* address);
	static void LockPages(void* address, uint64_t pageCount);

	static void* RequestPage();
	static void* RequestPages(uint64_t pageCount);

    static uint64_t GetFreeRAM();
    static uint64_t GetUsedRAM();
    static uint64_t GetReservedRAM();
};