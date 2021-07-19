#include "fat.h"
#include <BasicRenderer.h>
#include <cstr.h>
#include <string.h>
#include <memory/Paging/PageFrameAllocator.h>
#include <memory/heap.h>
#include <ahci/ahci.h>

namespace FAT {
	FATBootSector* bs;
	FATType type;

	void InitializeFAT(FATBootSector* bootSector) {
		bs = bootSector;
		Renderer.Println("Finding a specific file by path (/EFI/BOOT/BOOTX64.EFI): ", Colors::YELLOW);
		Traverse("/EFI/BOOT/BOOTX64.EFI");
		Renderer.Println("Showing the content of the whole filesystem:", Colors::YELLOW);
		ListDir(ReadDir((uint32_t)0), true);
	}

	void PrintEntry(DirectoryEntry* entry, uint8_t level) {
		for (int i = 0; i < level; ++i) {
			Renderer.Print("\t");
		}
		if (entry->attrib == FILE_DIRECTORY)
			Renderer.Print("D ");
		else
			Renderer.Print("F ");
		for (int i = 0; i < 8; ++i) {
			if (entry->name[i] == ' ')
				break;
			Renderer.PutChar((char)entry->name[i]);
		}
		if (entry->ext[0] != ' ') {
			Renderer.Print(".");
			for (int i = 0; i < 3; ++i) {
				if (entry->ext[i] == ' ')
					break;
				Renderer.PutChar((char)entry->ext[i]);
			}
		}
		if (entry->fileSize != 0) {
			Renderer.Print(" ");
			Renderer.Print(to_string((uint64_t)entry->fileSize / 1024));
			Renderer.Print(" KB");
		}
		Renderer.NextLine();
	}

	void ListDir(DirectoryEntry* entry, bool recursive, uint8_t level) {
		while (entry->name[0] != ENTRY_END) {
			if (entry->name[0] == ENTRY_FREE || (entry->attrib & FILE_LONG_NAME_MASK) == FILE_LONG_NAME) { //if the entry is a free entry or a long name
				entry++;
				continue;
			}
			if (strncmp((const char *)entry->name, ".", 1) == 0 || strncmp((const char *)entry->name, "..", 2) == 0) { // hiding the dot and double dot entries
				entry++;
				continue;
			}
			PrintEntry(entry, level);
			if (recursive && entry->attrib == FILE_DIRECTORY) {
				ListDir(ReadDir(entry), recursive, level + 1);
			}
			entry++;
		}
	}
	void Traverse(const char* path) {
		auto* ptr = (char*)malloc(sizeof(char) * strlen(path));
		char* freeptr = ptr;
		strcpy(ptr, path);

		uint8_t level = 0;
		DirectoryEntry* entry;
		ptr = strtok(ptr, "/");
		while (ptr != nullptr) {
			if (strcmp(ptr, "") == 0)
				entry = ReadDir((uint32_t)0);
			else {
				char filename[12];
				while(entry->name[0] != ENTRY_END && strncmp(filename, ptr, strlen(ptr)) != 0) {
					entry++;
					{
						uint8_t index = 0;
						for (int i = 0; i < 8; ++i) {
							if (entry->name[i] == ' ')
								break;
							filename[index] = (char)entry->name[i];
							index++;
						}
						if (entry->ext[0] != ' ') {
							filename[index] = '.';
							index++;
							for (int i = 0; i < 3; ++i) {
								if (entry->ext[i] == ' ')
									break;
								filename[index] = (char)entry->ext[i];
								index++;
							}
						}
					}
				}
				if (strncmp(filename, ptr, strlen(ptr)) == 0) {
					PrintEntry(entry, level);
					if (entry->attrib == FILE_DIRECTORY){
						entry = ReadDir(entry);
						level++;
					}
				}
			}
			ptr = strtok(nullptr, "/");
		}

		free(freeptr);
	}
	DirectoryEntry* ReadDir(uint32_t cluster) {
		if (cluster == 0) // If the cluster number is zero, assume we're trying to read from the root directory.
			cluster = bs->extendedSection.rootCluster;

		// Get the cluster chain of the directory. This contains the directory entries themselves.
		uint64_t nbCluster = 0;
		uint32_t* clusters = GetClusterChain(cluster, &nbCluster);

		// Try and read each cluster into a contiguous buffer.

		// This is the total number of bytes the directory is (in terms of on-disk dirents)
		uint64_t dirsize = nbCluster * bs->sectorsPerCluster * bs->bytesPerSector;

		// allocate a buffer, rounding up the size to a page.
		auto buffer = (uint64_t)AllocateBuffer(dirsize);
		auto outBuffer = buffer; // Save the buffer pointer as we will modify it later

		AHCI::AHCIDriver* ahci = PCI::AHCIDrivers[0];
		for (uint32_t i = 0; i < nbCluster; ++i) {
			ahci->Read(GetSectorFromCluster(cluster), bs->sectorsPerCluster, 0, (void*)buffer);
			buffer += bs->sectorsPerCluster * bs->bytesPerSector;
		}
		return (DirectoryEntry*)outBuffer;
	}
	uint32_t* GetClusterChain(uint32_t firstCluster, uint64_t* nbCluster) {
		// setup some stuff.
		auto* ret = (uint32_t*)malloc(32 * sizeof(uint32_t)); // TODO: Use a vector
		uint32_t retIndex = 0;
		uint32_t cluster = firstCluster;
		uint32_t cchain = 0;

		auto buffer = AllocateBuffer(bs->bytesPerSector);

		do {
			uint32_t sector = GetSectorFromCluster(cluster);
			uint32_t offset = (cluster * 4) / bs->bytesPerSector;

			// Read 512 bytes into the buffer
			AHCI::AHCIDriver* ahci = PCI::AHCIDrivers[0];
			ahci->Read(sector, bs->sectorsPerCluster, 0, buffer);

			auto* clusterChain = (uint8_t*)buffer;
			// using offset, we just index into the array to get the value we want.
			cchain = *((uint32_t*)&clusterChain[offset]) & 0x0FFFFFFF;

			// Include the cluster in the return array
			ret[retIndex] = cluster;
			retIndex++;

			cluster = cchain;

			// Increment so the caller have the number of clusters in the chain
			(*nbCluster)++;
		} while((cchain != 0) && (cchain & 0x0FFFFFFF) < END_CLUSTER_32);
		return ret;
	}
}
