#include "heap.h"
#include "Paging/PageTableManager.h"
#include "Paging/PageFrameAllocator.h"

void* heapStart;
void* heapEnd;
HeapSegHdr* LastHdr;

void HeapSegHdr::CombineForward() {
	if (next == nullptr || !next->free)
		return;
	if (next == LastHdr)
		LastHdr = this;
	if (next->next != nullptr)
		next->next->previous = this;

	length = length + next->length + sizeof(HeapSegHdr);

	next = next->next;
}
void HeapSegHdr::CombineBackward() {
	if (previous != nullptr && previous->free)
		previous->CombineForward();
}
HeapSegHdr* HeapSegHdr::Split(size_t splitLength) {
	if (splitLength < 0x10)
		return nullptr;
	int64_t splitSegLength = length - splitLength - sizeof(HeapSegHdr);
	if (splitSegLength < 0x10)
		return nullptr;

	auto* newSplitHdr = (HeapSegHdr*)((size_t)this + splitLength + sizeof(HeapSegHdr));
	next->previous = newSplitHdr;
	newSplitHdr->next = next;
	next = newSplitHdr;
	newSplitHdr->previous = this;
	newSplitHdr->length = splitSegLength;
	newSplitHdr->free = free;
	length = splitLength;

	if (LastHdr == this)
		LastHdr = newSplitHdr;
	return newSplitHdr;
}
void InitializeHeap(void* heapAddress, size_t pageCount) {
	void* pos = heapAddress;

	// Allocate all the pages needed for the heap
	for (size_t i = 0; i < pageCount; ++i) {
		PageTableManager::MapMemory(pos, PageFrameAllocator::RequestPage());
		pos = (void*)((size_t)pos + 0x1000);
	}

	size_t heapLength = pageCount * 0x1000;

	heapStart = heapAddress;
	heapEnd = (void*)((size_t)heapAddress + heapLength);
	auto* startSeg = (HeapSegHdr*)heapAddress;
	startSeg->length = heapLength - sizeof(HeapSegHdr);
	startSeg->next = NULL;
	startSeg->previous = NULL;
	startSeg->free = true;

	LastHdr = startSeg;
}
void ExpandHeap(size_t length) {
	if (length % 0x1000) {
		length -= length % 0x1000;
		length += 0x1000;
	}

	size_t pageCount = length / 0x1000;
	auto* newSeg = (HeapSegHdr*)heapEnd;

	for (size_t i = 0; i < pageCount; ++i) {
		PageTableManager::MapMemory(heapEnd, PageFrameAllocator::RequestPage());
		heapEnd = (void*)((size_t)heapEnd + 0x1000);
	}

	newSeg->free = true;
	newSeg->previous = LastHdr;
	LastHdr->next = newSeg;
	LastHdr = newSeg;
	newSeg->next = nullptr;
	newSeg->length = length - sizeof(HeapSegHdr);
	newSeg->CombineBackward();
}

void* malloc(size_t size) {
	if (size == 0)
		return nullptr;
	if (size % 0x10 > 0) { // It is not a multiple of 0x10 (128)
		size -= (size % 0x10);
		size += 0x10;
	}

	auto* currentSeg = (HeapSegHdr*) heapStart;
	do {
		if (currentSeg->free) {
			if (currentSeg->length > size) {
				currentSeg->Split(size);
				currentSeg->free = false;
				return (void*)((uint64_t)currentSeg + sizeof(HeapSegHdr));
			}
			if (currentSeg->length == size) {
				currentSeg->free = false;
				return (void*)((uint64_t)currentSeg + sizeof(HeapSegHdr));
			}
		}
		currentSeg = currentSeg->next;
	} while (currentSeg != nullptr);
	ExpandHeap(size);
	return malloc(size);
}
void free(void* address) {
	auto* segment = (HeapSegHdr*)((uint64_t)address - sizeof(HeapSegHdr));
	segment->free = true;
	memset((void*)((uint64_t)segment + sizeof(HeapSegHdr)), 0x00, segment->length);
	segment->CombineForward();
	segment->CombineBackward();
}

void* operator new(size_t size) {
	return malloc(size);
}
void* operator new[](size_t size) {
	return malloc(size);
}

void operator delete(void* p) noexcept {
	free(p);
}
void operator delete[](void* p) noexcept {
	free(p);
}
