#include "ahci.h"
#include <memory/heap.h>
#include <memory/Paging/PageTableManager.h>
#include <memory/Paging/PageFrameAllocator.h>

namespace AHCI {
#define HBA_PORT_DEV_PRESENT 0x03
#define HBA_PORT_IPM_ACTIVE 0x01

#define HBA_PxCMD_CR 0x8000
#define HBA_PxCMD_FRE 0x0010
#define HBA_PxCMD_ST 0x0001
#define HBA_PxCMD_FR 0x0000
#define HBA_PxIS_TFES (1 << 30)

#define SATA_SIG_ATAPI 0xEB140101
#define SATA_SIG_SEMB 0xC33C0101
#define SATA_SIG_PM 0x96690101
#define SATA_SIG_ATA 0x00000101

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
#define ATA_CMD_READ_DMA_EX 0x25

	void Port::Configure() {
		StopCMD();

		void* commandListBase = PageFrameAllocator::RequestPage();
		memset(commandListBase, 0, 1024);
		hbaPort->commandListBase = (uint32_t)(uint64_t)commandListBase;
		hbaPort->commandListBaseUpper = (uint32_t)((uint64_t)commandListBase >> 32);

		void* fisBase = PageFrameAllocator::RequestPage();
		memset(fisBase, 0, 256);
		hbaPort->fisBaseAddress = (uint32_t)(uint64_t)fisBase;
		hbaPort->fisBaseAddressUpper = (uint32_t)((uint64_t)fisBase >> 32);

		auto* cmdHeader = (HBACommandHeader*)((uint64_t)hbaPort->commandListBase + ((uint64_t)hbaPort->commandListBaseUpper << 32));

		for (int i = 0; i < 32; ++i) {
			cmdHeader[i].prdtLength = 8;

			void* cmdTableAddress = PageFrameAllocator::RequestPage();
			memset(cmdTableAddress, 0, 256);
			uint64_t address = (uint64_t)cmdTableAddress + (i << 8);
			cmdHeader[i].commandTableBaseAddress = (uint32_t)address;
			cmdHeader[i].commandTableBaseAddressUpper = (uint32_t)((uint64_t)cmdTableAddress >> 32);
		}

		StartCMD();
	}
	void Port::StartCMD() const {
		while (hbaPort->cmdSts & HBA_PxCMD_CR);

		hbaPort->cmdSts |= HBA_PxCMD_FRE;
		hbaPort->cmdSts |= HBA_PxCMD_ST;
	}
	void Port::StopCMD() const {
		hbaPort->cmdSts &= ~HBA_PxCMD_FRE;
		hbaPort->cmdSts &= ~HBA_PxCMD_ST;

		while (hbaPort->cmdSts & HBA_PxCMD_FR | hbaPort->cmdSts & HBA_PxCMD_CR);
	}
	bool Port::Read(uint64_t sector, uint32_t sectorCount, void* buffer) {
		uint32_t spin = 0;
		while ((hbaPort->taskFileData & (ATA_DEV_BUSY | ATA_DEV_DRQ) && spin < 1000000)) // Wait if device is busy
			spin++;
		if (spin == 1000000) // Port is stuck
			return false;

		uint32_t sectorL = sector;
		uint32_t sectorH = sector >> 32;

		hbaPort->interruptStatus = (uint32_t)-1; // Clear pending interrupt bits
		auto* cmdHeader = (HBACommandHeader*)hbaPort->commandListBase;
		cmdHeader->commandFISLength = sizeof(FIS_REG_H2D) / sizeof(uint32_t); // Command FIS size;
		cmdHeader->write = 0; //this is a read
		cmdHeader->prdtLength = 1;

		auto* commandTable = (HBACommandTable*)cmdHeader->commandTableBaseAddress;
		memset(commandTable, 0, sizeof(HBACommandTable) + (cmdHeader->prdtLength - 1) * sizeof(HBAPRDTEntry));

		commandTable->prdtEntry[0].dataBaseAddress = (uint32_t)(uint64_t)buffer;
		commandTable->prdtEntry[0].dataBaseAddressUpper = (uint32_t)((uint64_t)buffer >> 32);
		commandTable->prdtEntry[0].byteCount = (sectorCount << 9) - 1; // 512 bytes per sector
		commandTable->prdtEntry[0].interruptOnCompletion = 1;

		auto* cmdFIS = (FIS_REG_H2D*)(&commandTable->commandFIS);

		cmdFIS->fisType = FIS_TYPE_REG_H2D;
		cmdFIS->commandControl = 1; // Command
		cmdFIS->command = ATA_CMD_READ_DMA_EX;

		cmdFIS->lba0 = (uint8_t)sectorL;
		cmdFIS->lba1 = (uint8_t)(sectorL >> 8);
		cmdFIS->lba2 = (uint8_t)(sectorL >> 16);
		cmdFIS->lba3 = (uint8_t)sectorH;
		cmdFIS->lba4 = (uint8_t)(sectorH >> 8);
		cmdFIS->lba5 = (uint8_t)(sectorH >> 16);

		cmdFIS->deviceRegister = 1 << 6; // LBA mode

		cmdFIS->countLow = sectorCount & 0xFF;
		cmdFIS->countHigh = (sectorCount >> 8) & 0xFF;

		hbaPort->commandIssue = 1; // Tell the command to run

		do {
			if (hbaPort->interruptStatus & HBA_PxIS_TFES) // Read is not successful
				return false;
		} while (hbaPort->commandIssue != 0); // Wait until the read is done

		return true;
	}

	PortType CheckPortType(HBAPort* port) {
		uint32_t sataStatus = port->sataStatus;

		uint8_t deviceDetection = sataStatus & 0b111;
		uint8_t interfacePowerManagement = (sataStatus >> 8) & 0b111;

		if (deviceDetection != HBA_PORT_DEV_PRESENT || interfacePowerManagement != HBA_PORT_IPM_ACTIVE)
			return PortType::None;

		switch (port->signature) {
			case SATA_SIG_ATA:
				return PortType::SATA;
			case SATA_SIG_SEMB:
				return PortType::SEMB;
			case SATA_SIG_PM:
				return PortType::PM;
			case SATA_SIG_ATAPI:
				return PortType::SATAPI;
			default:
				return PortType::None;
		}
	}

	void AHCIDriver::ProbePorts() {
		uint32_t portsImplemented = ABAR->portsImplemented;
		for (int i = 0; i < 32; ++i) {
			if (portsImplemented & (1 << i)) { // Port is active
				PortType portType = CheckPortType(&ABAR->ports[i]);

				if (portType == PortType::SATA || portType == PortType::SATAPI) {
					ports[portCount] = new Port(&ABAR->ports[i], portType, portCount);
					portCount++;
				}
			}
		}
	}

	AHCIDriver::AHCIDriver(PCI::PCIDeviceHeader* pciBaseAddress) : pciBaseAddress(pciBaseAddress) {
		ABAR = (HBAMemory*)((PCI::PCIHeader0*)pciBaseAddress)->BAR5;

		PageTableManager::MapMemory(ABAR, ABAR);

		ProbePorts();

		for (int i = 0; i < portCount; ++i)
			ports[i]->Configure();
	}

	AHCIDriver::~AHCIDriver() {

	}
	bool AHCIDriver::Read(uint64_t sector, uint32_t sectorCount, uint8_t portNumber, void* buffer) {
		return ports[portNumber]->Read(sector, sectorCount, buffer);
	}
}
