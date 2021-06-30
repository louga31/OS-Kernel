#pragma once

#include "types.h"
#include "efiMemory.h"

size_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapEntries, size_t mMapDescriptorSize);

int memcmp(const void* aptr, const void* bptr, size_t n);
void memset(void* start, uint8_t value, size_t num);