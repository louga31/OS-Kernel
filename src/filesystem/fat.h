#pragma once
#include <types.h>

namespace FAT {
	//FAT constant values
#define END_CLUSTER_32 0x0FFFFFF8
#define BAD_CLUSTER_32 0x0FFFFFF7
#define FREE_CLUSTER_32 0x00000000

#define FILE_READ_ONLY 0x01
#define FILE_HIDDEN 0x02
#define FILE_SYSTEM 0x04
#define FILE_VOLUME_ID 0x08
#define FILE_DIRECTORY 0x10
#define FILE_ARCHIVE 0x20
#define FILE_LONG_NAME (FILE_READ_ONLY|FILE_HIDDEN|FILE_SYSTEM|FILE_VOLUME_ID)
#define FILE_LONG_NAME_MASK (FILE_READ_ONLY|FILE_HIDDEN|FILE_SYSTEM|FILE_VOLUME_ID|FILE_DIRECTORY|FILE_ARCHIVE)
#define FILE_LAST_LONG_ENTRY 0x40
#define ENTRY_FREE 0xE5
#define ENTRY_END 0x00
#define ENTRY_JAPAN 0x05
#define LAST_LONG_ENTRY 0x40

	enum FATType {
		FAT12,
		FAT16,
		FAT32,
		ExFAT
	};

	struct FAT32ExtendedBootSector {
		uint32_t tableSize;
		uint16_t extendedFlags;
		uint16_t fatVersion;
		uint32_t rootCluster;
		uint16_t fatInfo;
		uint16_t backupBSSector;
		uint8_t reserved0[12];
		uint8_t driveNumber;
		uint8_t reserved1;
		uint8_t bootsignature;
		uint32_t volumeID;
		uint8_t volumeLabel[11];
		uint8_t fatTypeLabel[8];
	} __attribute__((packed));

	struct FAT16ExtendedBootSector { // FAT16 a FAT12
		uint8_t biosDriveNum;
		uint8_t reserved1;
		uint8_t bootSignature;
		uint32_t volumeID;
		uint8_t volumeLabel[11];
		uint8_t fatTypeLabel[8];
	} __attribute__((packed));

	struct FATBootSector {
		uint8_t bootJMP[3];
		uint8_t oemName[8];
		uint16_t bytesPerSector;
		uint8_t sectorsPerCluster;
		uint16_t reservedSectorCount;
		uint8_t tableCount;
		uint16_t rootEntryCount;
		uint16_t totalSectors1;
		uint8_t mediaType;
		uint16_t tableSize;
		uint16_t sectorsPerTrack;
		uint16_t headSideCount;
		uint32_t hiddenSectorCount;
		uint32_t totalSectors2;

		FAT32ExtendedBootSector extendedSection;

//		//this will be cast to it's specific type once the driver actually knows what type of FAT this is.
//		uint8_t extendedSection[54];
	} __attribute__((packed));

	struct DirectoryEntry
	{
		unsigned char name[8];
		unsigned char ext[3];
		uint8_t attrib;
		uint8_t userAttrib;

		unsigned char undelete;
		uint16_t createTime;
		uint16_t createDate;
		uint16_t accessDate;
		uint16_t clusterHigh;

		uint16_t modifiedTime;
		uint16_t modifiedDate;
		uint16_t clusterLow;
		uint32_t fileSize;
	} __attribute__ ((packed));

	extern FATBootSector* bs;

	void InitializeFAT(FATBootSector* bootSector);

	void PrintEntry(DirectoryEntry* entry, uint8_t level = 0);
	void ListDir(DirectoryEntry* entry, bool recursive = false, uint8_t level = 0);

	inline uint32_t GetSectorFromCluster(uint32_t cluster) {
		static uint64_t firstDataSector = bs->reservedSectorCount + (bs->tableCount * bs->extendedSection.tableSize);
		return (cluster - 2) * bs->sectorsPerCluster + firstDataSector;
	}
	inline uint32_t GetClusterFromEntry(DirectoryEntry* entry) {
		return entry->clusterLow | (entry->clusterHigh >> 16);
	}

	void Traverse(const char* path);
	DirectoryEntry* ReadDir(uint32_t cluster);
	inline DirectoryEntry* ReadDir(DirectoryEntry* entry) {
		return ReadDir(GetClusterFromEntry(entry));
	}
	uint32_t* GetClusterChain(uint32_t firstCluster, uint64_t* nbCluster);
}