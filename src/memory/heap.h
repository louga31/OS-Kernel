#pragma once

#include <types.h>

struct HeapSegHdr {
	size_t length;
	HeapSegHdr* next;
	HeapSegHdr* previous;
	bool free;
	void CombineForward();
	void CombineBackward();
	HeapSegHdr* Split(size_t splitLength);
};

void InitializeHeap(void* heapAddress, size_t pageCount);
void ExpandHeap(size_t length);

void* malloc(size_t size);
void free(void* address);

void* operator new(size_t size);
void* operator new[](size_t size);

void operator delete(void* p) noexcept;
void operator delete[](void* p) noexcept;