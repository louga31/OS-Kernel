#include "memutils.h"

size_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapEntries, size_t mMapDescriptorSize) {
	static size_t memorySizeBytes = 0;
	if (memorySizeBytes > 0)
		return memorySizeBytes;
	
	for (size_t i = 0; i < mMapEntries; i++)
	{
		EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((size_t)mMap + (i * mMapDescriptorSize));
		memorySizeBytes += desc->numberOfPages * 0x1000;
	}
	
	return memorySizeBytes;
}

int memcmp(const void* aptr, const void* bptr, size_t n) {
	const byte* a = (byte*)aptr;
	const byte* b = (byte*)bptr;

	for (size_t i = 0; i < n; i++)
	{
		if (a[i] < b[i])
			return -1;
		if (a[i] > b[i])
			return 1;
	}
	return 0;
}

void memset(void* start, uint8_t value, size_t num) {
	for (size_t i = 0; i < num; ++i) {
		*(uint8_t*)((uint64_t)start + i) = value;
	}
}
