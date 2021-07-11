#include "pci.h"

#include "cstr.h"
#include "BasicRenderer.h"
#include "Paging/PageTableManager.h"

namespace PCI {
	void EnumerateFunction(uint64_t deviceAddress, uint64_t function) {
		uint64_t offset = function << 12;
		uint64_t functionAddress = deviceAddress + offset;

		PageTableManager::MapMemory((void*)functionAddress, (void*) functionAddress);

		auto* pciDeviceHeader = (PCIDeviceHeader*)functionAddress;

		if (pciDeviceHeader->DeviceID == 0)
			return;
		if (pciDeviceHeader->DeviceID == 0xFFFF)
			return;

		Renderer.Print(GetVendorName(pciDeviceHeader->VendorID));
		Renderer.Print(" / ");
		Renderer.Print(GetDeviceName(pciDeviceHeader->VendorID, pciDeviceHeader->DeviceID));
		Renderer.Print(" / ");
		Renderer.Print(DeviceClasses[pciDeviceHeader->Class]);
		Renderer.Print(" / ");
		Renderer.Print(GetSubclassName(pciDeviceHeader->Class, pciDeviceHeader->Subclass));
		Renderer.Print(" / ");
		Renderer.Println(GetProgIFName(pciDeviceHeader->Class, pciDeviceHeader->Subclass, pciDeviceHeader->ProgIF));
	}
	void EnumerateDevice(uint64_t busAddress, uint64_t device) {
		uint64_t offset = device << 15;
		uint64_t deviceAddress = busAddress + offset;

		PageTableManager::MapMemory((void*)deviceAddress, (void*) deviceAddress);

		auto* pciDeviceHeader = (PCIDeviceHeader*)deviceAddress;

		if (pciDeviceHeader->DeviceID == 0)
			return;
		if (pciDeviceHeader->DeviceID == 0xFFFF)
			return;

		for (uint64_t function = 0; function < 8; ++function) {
			EnumerateFunction(deviceAddress, function);
		}
	}
	void EnumerateBus(uint64_t baseAddress, uint64_t bus) {
		uint64_t offset = bus << 20;
		uint64_t busAddress = baseAddress + offset;

		PageTableManager::MapMemory((void*)busAddress, (void*) busAddress);

		auto* pciDeviceHeader = (PCIDeviceHeader*)busAddress;

		if (pciDeviceHeader->DeviceID == 0)
			return;
		if (pciDeviceHeader->DeviceID == 0xFFFF)
			return;

		for (uint64_t device = 0; device < 32; ++device) {
			EnumerateDevice(busAddress, device);
		}
	}
	void EnumeratePCI(ACPI::MCFGHeader* mcfg) {
		uint64_t entries = ((mcfg->Header.Length) - sizeof(ACPI::MCFGHeader)) / sizeof(ACPI::DeviceConfig);
		for (int t = 0; t < entries; ++t) {
			auto* deviceConfig = (ACPI::DeviceConfig*)((uint64_t)mcfg + sizeof(ACPI::MCFGHeader) + (sizeof(ACPI::DeviceConfig) * t));
			for (uint64_t bus = deviceConfig->StartBus; bus < deviceConfig->EndBus; ++bus) {
				EnumerateBus(deviceConfig->BaseAddress, bus);
			}
		}
	}
}