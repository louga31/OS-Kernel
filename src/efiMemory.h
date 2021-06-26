#pragma once

#include "types.h"

struct EFI_MEMORY_DESCRIPTOR {
    uint32_t type;            // Field size is 32 bits followed by 32 bit pad
    uint32_t Pad;             // Padding
    void*    physicalAddress; // Field size is 64 bits
    void*    virtualAddress;  // Field size is 64 bits
    uint64_t numberOfPages;   // Field size is 64 bits
    uint64_t attributes;      // Field size is 64 bits
};

extern const char* EFI_MEMORY_TYPE_STRINGS[];